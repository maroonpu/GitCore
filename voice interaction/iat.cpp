#include "iat.h"

int Iat::iatFunc(void)
{
	int ret;
	const char* session_id = NULL;
	const char* session_begin_params=
		"sub = iat, domain = iat, language = zh_cn, "
		"accent = mandarin, sample_rate = 16000, "
		"result_type = plain, result_encoding = utf8";	
	WAVEFORMATEX wavfmt = DEFAULT_FORMAT;

	//用户登录
	ret = MSPLogin(NULL, NULL, "appid = 5965787c, work_dir = .");
	if(MSP_SUCCESS != ret){return IATLOGIN_FAILED;}

	iat.ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
	iat.rec_stat = MSP_REC_STATUS_SUCCESS;
	iat.audio_status = MSP_AUDIO_SAMPLE_FIRST;
	iat.callflag = 0;

	//创建录音机
	create_recorder(&iat.recorder, Iat::iatCallback, (void*)&iat);
	
	//打开录音机
	open_recorder(iat.recorder, get_default_input_dev(), &wavfmt);
	
	//开始一次语音识别
	session_id = QISRSessionBegin( NULL, session_begin_params, &ret );
	if(MSP_SUCCESS != ret)
	{
		close_recorder(iat.recorder);
		destroy_recorder(iat.recorder);

		return IAT_FAILED;
	}
	
	iat.session_id = session_id;
	iat.ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
	iat.rec_stat = MSP_REC_STATUS_SUCCESS;
	iat.audio_status = MSP_AUDIO_SAMPLE_FIRST;

	//开始录音
	start_record(iat.recorder);

	cout<<"请说话："<<endl;
  	
  	//线程休眠5s,经测试>5s比较好
	sleep(5);

	//0---未回调 1---成功  2---回调为空  3---语音听写失败
	switch(iat.callflag) 
	{
		case 0:{
			//停止录音
			stop_record(iat.recorder);
			while(!is_record_stopped(iat.recorder));

			cout<<"小灰灰：你在说什么？"<<endl;
			iat.rslt = "你在说什么";	
		}break;
		case 1:break;
		case 2:{
			//停止录音
			stop_record(iat.recorder);
			while(!is_record_stopped(iat.recorder));

			cout<<"小灰灰：你在说什么？"<<endl;
			iat.rslt = "你在说什么";	
		}break;
		case 3:{
			stop_record(iat.recorder);
			while(!is_record_stopped(iat.recorder));
			close_recorder(iat.recorder);
			destroy_recorder(iat.recorder);

			return IAT_FAILED;
		}break;
		default:break;
	}
	cout<<"callflag:"<<iat.callflag<<endl;
	//结束本次语音识别
	QISRSessionEnd(session_begin_params,"normal");
	
	//退出登录					
	MSPLogout();

	//关闭录音机
	close_recorder(iat.recorder);
	
	//销毁录音机
	destroy_recorder(iat.recorder);

	return IAT_SUCCESS;
}

void Iat::iatCallback(char *data, unsigned long len, void *user_para)
{
	int ret;
	struct speech_rec *sr = (struct speech_rec *)user_para;

	if(len == 0 || data == NULL){sr->callflag = 2;return;}

	if(sr == NULL || sr->ep_stat >= MSP_EP_AFTER_SPEECH){sr->callflag = 2;return;}
	
	//写入本次识别的音频
	ret = QISRAudioWrite(sr->session_id, data, len, sr->audio_status, &sr->ep_stat, &sr->rec_stat);
	if(MSP_SUCCESS != ret){sr->callflag = 3;return;}

	sr->audio_status = MSP_AUDIO_SAMPLE_CONTINUE;

	if (MSP_REC_STATUS_SUCCESS == sr->rec_stat)
	{ 
		//获取识别结果
		sr->rslt = QISRGetResult(sr->session_id, &sr->rec_stat, 0, &ret);
		if (MSP_SUCCESS != ret){sr->callflag = 3;return;}
		else
		{
			stop_record(sr->recorder);
			while(!is_record_stopped(sr->recorder));
			
			sr->callflag = 1;
			return;
		}	
	}
}	