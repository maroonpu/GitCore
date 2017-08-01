#include "myvlc.h"

/**
* 方法功能描述 播放音频文件
* @param  字符指针 path 音频文件路径
* @author 蒲乐辉
* @time   2017年7月15日
*/	
void AudioPlay::playAudio(const char *path)
{
	libvlc_instance_t * inst;
	libvlc_media_player_t *mp;
	libvlc_media_t *m;

	/* Load VLC engine */
	inst = libvlc_new (0, NULL);

	/* Create a new item */
	/* warning:play *.wav may display error code,but not exactly */
	m = libvlc_media_new_path (inst, path);

	/* Create a media player playing environement */
	mp = libvlc_media_player_new_from_media (m);

	/* No need to keep the media now */
	libvlc_media_release (m);

	/* play the media_player */
	libvlc_media_player_play (mp);

	/* wait a little bit for loading */
	usleep(5*1000);

	/* get the length of the media in ms */
	mediaLength = libvlc_media_player_get_length(mp);

	usleep (mediaLength*1000); /* Let it play a bit */

	/* Stop playing */
	libvlc_media_player_stop (mp);

	/* Free the media_player */
	libvlc_media_player_release (mp);

	libvlc_release (inst);
}	