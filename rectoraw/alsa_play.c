#define ALSA_PCM_NEW_HW_PARAMS_API
//#include "alsahead.h"
#include <alsa/asoundlib.h>
#include <math.h>

#define BUFFERSIZE 4096
#define PERIOD_SIZE 1024
#define PERIODS 2
#define SAMPLE_RATE 16000
#define CHANNELS 2
#define FSIZE 2*CHANNELS


/*
*Usage: play audio.raw
*
*/

int main(int argc, char *argv[]) {
	long loops;
	int rc;
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val;

	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;
	char *inFile;

	int fd;
	int err;

	if(argc ==2)
	{
		inFile = argv[1];
	}
	else
	{
		inFile = "output.raw";
	}

	fd = open(inFile,O_RDONLY);

	rc = snd_pcm_open(&handle, "default",SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr,"unable to open pcm device: %s\n",snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	err=snd_pcm_hw_params_any(handle, params);
		if (err < 0) {
		    fprintf(stderr, "Can not configure this PCM device: %s\n",
		            snd_strerror(err));
		    exit(1);
		}
	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	err=snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);
		if (err < 0) {
		    fprintf(stderr,
		            "Failed to set PCM device to interleaved: %s\n",
		            snd_strerror(err));
		    exit(1);
		}
	/* Signed 16-bit little-endian format */
	err=snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_S16_LE);
		if (err < 0) {
		    fprintf(stderr,
		            "Failed to set PCM device to 16-bit signed PCM: %s\n",
		            snd_strerror(err));
		    exit(1);
		}
	/* One channels (mono) */
	err=snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
		if (err < 0) {
		    fprintf(stderr, "Failed to set PCM device to mono: %s\n",
		            snd_strerror(err));
		    exit(1);
		}
	/* 16000 bits/second sampling rate (CD quality) */
	val = SAMPLE_RATE;
	err=snd_pcm_hw_params_set_rate_near(handle, params,&val, &dir);
		if (err < 0) {
		    fprintf(stderr, "Failed to set PCM device to sample rate =%d: %s\n",
		            val,snd_strerror(err));
		    exit(1);
		}
	/*
	frames = PERIOD_SIZE;
	snd_pcm_hw_params_set_period_size_near(handle,params, &frames, &dir);
	*/
	/*
	snd_pcm_hw_params_get_buffer_time_max(params, &buffer_time, 0);
	period_time = buffer_time / 4;
	err=snd_pcm_hw_params_set_period_time_near(handle, params,&period_time, 0);
		if (err < 0) {
			fprintf(stderr,"Failed to set PCM device to period time =%u: %s\n",
			period_time,snd_strerror(err));
			exit(1);
		}
	*/

	unsigned int buffer_time,period_time;
	snd_pcm_hw_params_get_buffer_time_max(params, &buffer_time, 0);
	if ( buffer_time >500000)
		buffer_time = 500000;
	period_time = buffer_time / 4;
	err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, 0);
		if (err < 0) {
		    fprintf(stderr, "Failed to set PCM device to buffer time =%d: %s\n",
		            buffer_time,snd_strerror(err));
		    exit(1);
		}

	err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, 0);
		if (err < 0) {
		    fprintf(stderr, "Failed to set PCM device to period time =%d: %s\n",
		            period_time,snd_strerror(err));
		    exit(1);
		}


	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr,"unable to set hw parameters: %s\n",snd_strerror(rc));
		exit(1);
	}

	snd_pcm_hw_params_get_period_size(params, &frames,&dir);
	size = frames * FSIZE;
	buffer = (char *) malloc(size);
	printf("period size = %d frames\n", (int)frames);
	printf("read buffer size = %d\n",size);

	snd_pcm_hw_params_get_period_time(params,&val, &dir);
	printf("period time is: %d\n",val);
	loops = 100;

	snd_pcm_hw_params_get_buffer_time(params, 
		                                &val, &dir);
	printf("buffer time = %d us\n", val);

	snd_pcm_hw_params_get_buffer_size(params,
		                     (snd_pcm_uframes_t *) &val);
	printf("buffer size = %d frames\n", val);

	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);


	while (loops > 0) {
		loops--;
		rc = read(fd, buffer, size);
		if (rc == 0) {
			fprintf(stderr, "end of file on input\n");
			break;
		} 
		else if (rc != size) {
			fprintf(stderr,"short read: read %d bytes\n", rc);
		}
		rc = snd_pcm_writei(handle, buffer, frames);
		if (rc == -EPIPE) {
			fprintf(stderr, "underrun occurred\n");
			err=snd_pcm_prepare(handle);
			if( err <0){
				fprintf(stderr, "can not recover from underrun: %s\n",snd_strerror(err));
			}
		
		} 
		else if (rc < 0) {
			fprintf(stderr,"error from writei: %s\n",snd_strerror(rc));
		}  
		else if (rc != (int)frames) {
			fprintf(stderr,"short write, write %d frames\n", rc);
		}
	}
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
	close(fd);
	return 0;
}
