/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/
#include <alsa/asoundlib.h>
#include <math.h>

#define BUFFERSIZE 4096
#define PERIOD_SIZE 1024
#define PERIODS 2
#define SAMPLE_RATE 16000
#define CHANNELS 2
#define FSIZE 2*CHANNELS

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API


int main(int argc, char *argv[]) {

	long loops; //define the record time.
	int rc;	//return code.
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;
 	
	int err;
	char *file;

	int fd;

	if(argc ==2)
	{
		file = argv[1];
	}
	else
	{
		file = "output.raw";
	}

	fd = open(file,O_WRONLY|O_CREAT,0777);
	if( fd ==-1)
	{
		printf("open file:%s fail.\n",file);
		exit(1);
	}


	/* Open PCM device for recording (capture). */
	err = snd_pcm_open(&handle, "default",SND_PCM_STREAM_CAPTURE, 0);
	if (err < 0) {
		fprintf(stderr,"unable to open pcm device: %s\n",
		snd_strerror(err));
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

	/* Set buffer time 500000. */
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


	/* Write the parameters to the driver */
	err = snd_pcm_hw_params(handle, params);
		if (err < 0) {
			fprintf(stderr,"unable to set hw parameters: %s\n",
			snd_strerror(err));
			exit(1);
		}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params,&frames, &dir);
	size = frames * FSIZE; /* 2 bytes/sample, 1 channels */
	buffer = (char *) malloc(size);

	printf("period size = %d frames\n", (int)frames);
	printf("read buffer size = %d\n",size);


	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params,&val, &dir);
	printf("period time is: %d\n",val);
	loops = 100;

	/*print alsa config parameter*/

	snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
	printf("buffer time = %d us\n", val);

	snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *) &val);
	printf("buffer size = %d frames\n", val);

	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);

	while (loops > 0) {
		loops--;

		rc = snd_pcm_readi(handle, buffer, frames);

		if (rc == -EPIPE) {
		// EPIPE means overrun 
			fprintf(stderr, "overrun occurred\n");
			err=snd_pcm_prepare(handle);
			if( err <0){
				fprintf(stderr, "Failed to recover form overrun : %s\n",
				snd_strerror(err));
				exit(1);
			}
		}
		else if (rc < 0) {
			fprintf(stderr,"error from read: %s\n",snd_strerror(rc));
			exit(1);
		} 
		else if (rc != (int)frames) {
			fprintf(stderr, "short read, read %d frames\n", rc);
		
		}
		rc = write(fd, buffer, size);
		if (rc <0){
			perror("fail to write to audio file\n");
		}

	}

	close(fd);
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);

	return 0;
}
