#ifndef __MYVLC__
#define __MYVLC__

#include <iostream>
#include <vlc/vlc.h>
#include <unistd.h>

using namespace std;

/**
* 类功能描述 音频播放
* @author    蒲乐辉
* @time      2017年7月15日
*/
class AudioPlay
{
	public:
		libvlc_time_t mediaLength;

		/**
		* 方法功能描述 播放音频文件
		* @param  字符指针 path 音频文件路径
		* @author 蒲乐辉
		* @time   2017年7月15日
		*/	
		void playAudio(const char *path);	
};

#endif
