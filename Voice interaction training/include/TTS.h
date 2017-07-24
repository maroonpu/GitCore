#ifndef __TTS__
#define __TTS__

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include <AL/al.h> 
#include <AL/alc.h> 

using namespace std;

/* wav音频头部格式 */
typedef struct _wave_pcm_hdr
{
	char            riff[4];                // = "RIFF"
	int		size_8;                 // = FileSize - 8
	char            wave[4];                // = "WAVE"
	char            fmt[4];                 // = "fmt "
	int		fmt_size;		// = 下一个结构体的大小 : 16

	short int       format_tag;             // = PCM : 1
	short int       channels;               // = 通道数 : 1
	int		samples_per_sec;        // = 采样率 : 8000 | 6000 | 11025 | 16000
	int		avg_bytes_per_sec;      // = 每秒字节数 : samples_per_sec * bits_per_sample / 8
	short int       block_align;            // = 每采样点字节数 : wBitsPerSample / 8
	short int       bits_per_sample;        // = 量化比特数: 8 | 16

	char            data[4];                // = "data";
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
class TTS
{
	public:
		static void ttsFunc(const char* audio_text)
		{
			int ret;
			const char* session_begin_params = "voice_name = nannan, text_encoding = utf8, sample_rate = 16000, speed = 50, volume = 50, pitch = 50, rdn = 2";	
			const char* sessionID = QTTSSessionBegin(session_begin_params, &ret);
			if(ret != MSP_SUCCESS){cout<<"开始合成失败"<<endl;}
			// else cout<<"开始语音合成"<<endl;

			// const char* audio_text = "亲爱的用户，您好，这是一个语音合成示例，感谢您对科大讯飞语音技术的支持！科大讯飞是亚太地区最大的语音上市公司，股票代码：002230";
			if(strlen(audio_text)>8192) audio_text="小灰灰说不了那么多话！";
			ret = QTTSTextPut(sessionID,audio_text,strlen(audio_text),NULL);	
			if(ret != MSP_SUCCESS){cout<<"写入文本失败"<<endl;}
			// else cout<<"成功写入文本"<<endl;

			// wave_pcm_hdr wav_hdr = default_wav_hdr;
			int synth_status;
			unsigned int audio_len;
			FILE* fp = fopen("demo.wav", "wb");

			// fwrite(&wav_hdr, sizeof(wav_hdr), 1, fp);
			while(1)
			{
				const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
				if(data != NULL)
				{

					// const ALCchar *   devices; 
					ALCdevice *       mainDev; 
					ALCcontext *      mainContext; 
					ALuint            buffer; 
					ALuint            source; 
					ALint             playState; 			

					// printf("Opening playback device:\n"); 
					mainDev = alcOpenDevice(NULL); 
					if (mainDev == NULL) 
					{ 
					  // printf("Unable to open playback device!\n"); 
					  exit(1); 
					} 
					alcGetString(mainDev, ALC_DEVICE_SPECIFIER); 
					// printf("   opened device '%s'\n", devices); 
					mainContext = alcCreateContext(mainDev, NULL); 
					if (mainContext == NULL) 
					{ 
					  // printf("Unable to create playback context!\n"); 
					  exit(1); 
					} 
					// printf("   created playback context\n"); 

					// Make the playback context current 使回放上下文当前
					alcMakeContextCurrent(mainContext); 
					alcProcessContext(mainContext); 

					alGenBuffers(1, &buffer); 
					alGenSources(1, &source); 
					alBufferData(buffer, AL_FORMAT_STEREO16, data, audio_len, 8000); 
					alSourcei(source, AL_BUFFER, buffer); 
					alSourcePlay(source); 

					// Wait for the source to stop playing 
					playState = AL_PLAYING; 
					while (playState == AL_PLAYING) 
					{ 
					  // printf("  source %d is playing...\r", source); 
					  fflush(stdout); 
					  // alGetSourcei(source, AL_SOURCE_STATE, &playState); 
					  usleep(100000); 
					} 
					// printf("\nDone with playback.\n"); 
					fflush(stdout); 

					// Shut down OpenAL 
					alDeleteSources(1, &source); 
					alDeleteBuffers(1, &buffer); 

					alcMakeContextCurrent(NULL); 
					alcCloseDevice(mainDev); 

					// fwrite(data, audio_len, 1, fp);
					// wav_hdr.data_size += audio_len;
				}
				if(synth_status == MSP_TTS_FLAG_DATA_END || ret != MSP_SUCCESS)
				{
					break;
				}
			}

			fclose(fp);
			fp = NULL;

			ret = QTTSSessionEnd(sessionID, "normal end");
			if(ret != MSP_SUCCESS){cout<<"结束语音合成失败"<<endl;}
			// else cout<<"结束语音合成"<<endl;			
		}
	
};

#endif





