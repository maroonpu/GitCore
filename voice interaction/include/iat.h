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

//宏定义语音听写状态
#define IAT_SUCCESS         0
#define IAT_FAILED            1
#define IATLOGIN_FAILED 2

using namespace std;

struct speech_rec 
{
	const char * session_id;
	const char *rslt;
	int ep_stat;
	int rec_stat;
	int audio_status;
	struct recorder *recorder;
	int callflag;
};

#define DEFAULT_FORMAT {WAVE_FORMAT_PCM,1,16000,32000,2,16,sizeof(WAVEFORMATEX)}

class Iat
{
	public:
		struct speech_rec iat;

		int iatFunc(void);
		static void iatCallback(char *data, unsigned long len, void *user_para);
};

#endif