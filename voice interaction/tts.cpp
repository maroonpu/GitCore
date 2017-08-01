#include "tts.h"

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char		riff[4];                // = "RIFF"
	int		size_8;                 // = FileSize - 8
	char		wave[4];                // = "WAVE"
	char		fmt[4];                 // = "fmt "
	int		fmt_size;		// = 下一个结构体的大小 : 16

	short int	format_tag;             // = PCM : 1
	short int	channels;               // = 通道数 : 1
	int		samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int		avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int	block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int	bits_per_sample;        // = 量化比特数: 8 | 16

	char		data[4];                // = "data";
	int		data_size;              // = 纯数据长度 : FileSize - 44 
} wave_pcm_hdr;

/* 默认wav音频头部数据 */
wave_pcm_hdr default_wav_hdr = 
{
	{ 'R', 'I', 'F', 'F' },
	0,
	{'W', 'A', 'V', 'E'},
	{'f', 'm', 't', ' '},
	16,
	1,
	1,
	16000,
	32000,
	2,
	16,
	{'d', 'a', 't', 'a'},
	0  
};

/**
* 方法功能描述 语音合成函数
* @param  字符指针 audio_text 语音合成文本材料
* @return 整型 语音合成状态	
* @author 蒲乐辉
* @time   2017年7月15日
*/	
int Tts::ttsFunc(const char* audio_text)
{
	int ret;

	//用户登录
	ret = MSPLogin(NULL, NULL, "appid = 5965787c, work_dir = .");
	if (MSP_SUCCESS != ret){return TTSLOGIN_FAILED;}

	const char* session_begin_params = "voice_name = nannan, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";	
	//开始一次语音合成，分配语音合成资源
	const char* sessionID = QTTSSessionBegin(session_begin_params, &ret);
	if(MSP_SUCCESS != ret){return TTS_FAILED;}

	//合成文本长度最大支持8192字节
	if(strlen(audio_text)>8192) {audio_text="小灰灰说不了那么多话！";}

	//写入待合成的文本
	ret = QTTSTextPut(sessionID,audio_text,strlen(audio_text),NULL);	
	if(MSP_SUCCESS != ret){return TTS_FAILED;}

	wave_pcm_hdr wav_hdr = default_wav_hdr;
	int synth_status;
	unsigned int audio_len;
	
	//打开音频文件
	FILE* fp = fopen("tts.wav", "wb");

	//添加wav音频头
	fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp);

	while(1)
	{
		const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
		if(NULL != data)
		{
			//写入音频
			fwrite(data, audio_len, 1, fp);
			wav_hdr.data_size += audio_len; //计算data_size大小
		}

		if(synth_status == MSP_TTS_FLAG_DATA_END || ret != MSP_SUCCESS)
		{
			break;
		}
	}
	/* 修正wav文件头数据的大小 */
	wav_hdr.size_8 += wav_hdr.data_size + (sizeof(wav_hdr) - 8);
	
	/* 将修正过的数据写回文件头部,音频文件为wav格式 */
	fseek(fp, 4, 0);
	fwrite(&wav_hdr.size_8,sizeof(wav_hdr.size_8), 1, fp); //写入size_8的值
	fseek(fp, 40, 0); //将文件指针偏移到存储data_size值的位置
	fwrite(&wav_hdr.data_size,sizeof(wav_hdr.data_size), 1, fp); //写入data_size的值
	
	//关闭音频文件
	fclose(fp);
	fp = NULL;

	//结束本次语音合成
	ret = QTTSSessionEnd(sessionID, "normal end");
	if(ret != MSP_SUCCESS){return TTS_FAILED;}

	//退出登录
	MSPLogout(); 

	return TTS_SUCCESS;
}