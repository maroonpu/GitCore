#ifndef __TTS__
#define __TTS__

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"

//宏定义语音合成状态
#define	  TTS_SUCCESS         0 
#define	  TTS_FAILED            1
#define  TTSLOGIN_FAILED  2 

using namespace std;

/**
* 类功能描述 语音合成
* @author    蒲乐辉
* @time      2017年7月15日
*/
class Tts
{
	public:
		/**
		* 方法功能描述 语音合成函数
		* @param  字符指针 audio_text 语音合成文本材料
		* @return 整型 语音合成状态	
		* @author 蒲乐辉
		* @time   2017年7月15日
		*/	
		int ttsFunc(const char* audio_text);	
};

#endif