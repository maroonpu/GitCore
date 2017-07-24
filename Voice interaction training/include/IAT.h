#ifndef __IAT__
#define __IAT__

 #include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "linuxrec.h"
#include <json/json.h>
#include "TTS.h"

using namespace std;

struct speech_rec 
{
	const char * session_id;
	int ep_stat;
	int rec_stat;
	int audio_status;
	struct recorder *recorder;
	int nullflag;
};

#define DEFAULT_FORMAT {WAVE_FORMAT_PCM,1,16000,32000,2,16,sizeof(WAVEFORMATEX)}

class IAT
{
	public:
		void iatFunc()
		{
			int ret;
			const char* session_id = NULL;
			const char* session_begin_params=
				"sub = iat, domain = iat, language = zh_cn, "
				"accent = mandarin, sample_rate = 16000, "
				"result_type = plain, result_encoding = utf8";	
			WAVEFORMATEX wavfmt = DEFAULT_FORMAT;

			MSPLogin(NULL, NULL, "appid = 596584e7");

			iat.ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
			iat.rec_stat = MSP_REC_STATUS_SUCCESS;
			iat.audio_status = MSP_AUDIO_SAMPLE_FIRST;
			iat.nullflag = 0;
			create_recorder(&iat.recorder, IAT::iatCallback, (void*)&iat);
			
			open_recorder(iat.recorder, get_default_input_dev(), &wavfmt);
			
			session_id = QISRSessionBegin( NULL, session_begin_params, &ret );
			
			iat.session_id = session_id;
			iat.ep_stat = MSP_EP_LOOKING_FOR_SPEECH;
			iat.rec_stat = MSP_REC_STATUS_SUCCESS;
			iat.audio_status = MSP_AUDIO_SAMPLE_FIRST;
	
			start_record(iat.recorder);

			cout<<"请说话："<<endl;

			sleep(10);

			if (iat.nullflag == 0)
			{
				cout<<"小灰灰：你在说什么？"<<endl;
				TTS::ttsFunc("你在说什么？");
				stop_record(iat.recorder);
			}


			while((!is_record_stopped(iat.recorder)));

			QISRSessionEnd(session_begin_params,"normal");

			close_recorder(iat.recorder);

			destroy_recorder(iat.recorder);
							
			MSPLogout();

		}

		static void iatCallback(char *data, unsigned long len, void *user_para)
		{
			int ret;
			const char *rslt = NULL;
			struct speech_rec *sr = (struct speech_rec *)user_para;

			if(len == 0 || data == NULL)
				return;

			// sr = (struct speech_rec *)user_para;

			if(sr == NULL || sr->ep_stat >= MSP_EP_AFTER_SPEECH)
				return;
			
			ret = QISRAudioWrite(sr->session_id, data, len, sr->audio_status, &sr->ep_stat, &sr->rec_stat);
			if(ret!=MSP_SUCCESS){printf("写入失败\n");}
			// else{printf("成功6\n");}

			sr->audio_status = MSP_AUDIO_SAMPLE_CONTINUE;

			if (MSP_REC_STATUS_SUCCESS == sr->rec_stat)
			{ 
				rslt = QISRGetResult(sr->session_id, &sr->rec_stat, 0, &ret);
				if (MSP_SUCCESS != ret){printf("获取失败\n");}
				else
				{
					Json::Reader  reader ;
					Json::Value     value  ;

					Turing tur;
					
					char* str = (char*)malloc(BUFSIZE);
					memset(str, 0, BUFSIZE);

					string postData,infoMSG;
					infoMSG=rslt;
					cout<<"我："<<rslt<<endl;
					
					postData="key=84779b356eb9497d92ada64a70a3fb64&info="+infoMSG;
					char* postDataPoint  = &postData[0];
					// postDataPoint = &postData[0];
					if(tur.HttpPost("http://www.tuling123.com/openapi/api", postDataPoint,str))
					{
						char* strPort = strchr(str, '{');
						if(reader.parse(strPort,value))
						{
							cout<<"小灰灰："<<value["text"].asString()<<endl;
							TTS::ttsFunc(&value["text"].asString()[0]);
							free(str);
						}

					} 

					stop_record(sr->recorder);
					sr->nullflag = 1;


				}
			}
		}	

		struct speech_rec iat;

	private:



};

#endif