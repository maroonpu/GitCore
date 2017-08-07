#include "alsarec.h"

int AlsaRec::recToWav(const char *filename,uint32_t duration_time)
{
	const char *devicename = "default"; 
	int fd; 
	WAVContainer_t wav; 
	SNDPCMContainer_t record; 

	memset(&record, 0x0, sizeof(record));//initialization

	remove(filename); //delete old file

	//open/create file
	if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1) { 
		cerr<<"Error open: "<<filename<<endl;
	    	return -1; 
	} 

	//open pcm rec device
	if (snd_pcm_open(&record.handle, devicename, SND_PCM_STREAM_CAPTURE, 0) < 0) { 
		cerr<<"Error snd_pcm_open "<<devicename<<endl;
		goto Err; 
	} 

	//set wav head params
	if (SNDWAV_PrepareWAVParams(&wav,duration_time) < 0) { 
		cerr<<"Error SNDWAV_PrepareWAVParams"<<endl;
	    	goto Err; 
	} 

	//set device params
	if (SNDWAV_SetParams(&record, &wav) < 0) { 
		cerr<<"Error set_snd_pcm_params"<<endl;
	    	goto Err; 
	} 

	//start record
	SNDWAV_Record(&record, &wav, fd); 

	snd_pcm_drain(record.handle); 

	close(fd); 
	free(record.data_buf); 
	snd_pcm_close(record.handle); 
	return 0; 

Err: 
	close(fd); 
	remove(filename); 
	if (record.data_buf) free(record.data_buf); 
	if (record.handle) snd_pcm_close(record.handle); 
	return -1; 
}

int AlsaRec::SNDWAV_PrepareWAVParams(WAVContainer_t *wav,uint32_t duration_time) 
{ 
	//check legality
	assert(wav); 

	uint16_t channels = DEFAULT_CHANNELS; 
	uint16_t sample_rate = DEFAULT_SAMPLE_RATE; 
	uint16_t sample_length = DEFAULT_SAMPLE_LENGTH; 

	/* Const */ 
	wav->header.magic = WAV_RIFF; 
	wav->header.type = WAV_WAVE; 
	wav->format.magic = WAV_FMT; 
	wav->format.fmt_size = LE_INT(16); 
	wav->format.format = LE_SHORT(WAV_FMT_PCM); 
	wav->chunk.type = WAV_DATA; 

	/* User definition */ 
	wav->format.channels = LE_SHORT(channels); 
	wav->format.sample_rate = LE_INT(sample_rate); 
	wav->format.sample_length = LE_SHORT(sample_length); 

	/* See format of wav file */ 
	wav->format.blocks_align = LE_SHORT(channels * sample_length / 8); 
	wav->format.bytes_p_second = LE_INT((uint16_t)(wav->format.blocks_align) * sample_rate); 

	wav->chunk.length = LE_INT(duration_time * (uint32_t)(wav->format.bytes_p_second)); 
	wav->header.length = LE_INT((uint32_t)(wav->chunk.length) + sizeof(wav->chunk) + sizeof(wav->format) + sizeof(wav->header) - 8); 

	return 0; 
} 

//为音频设备设置读写参数 
int AlsaRec::SNDWAV_SetParams(SNDPCMContainer_t *sndpcm, WAVContainer_t *wav) 
{ 
	snd_pcm_hw_params_t *hwparams; 
	snd_pcm_format_t format; 
	uint32_t exact_rate; 
	uint32_t buffer_time, period_time; 
 
	/* Allocate the snd_pcm_hw_params_t structure on the stack. */ 
	snd_pcm_hw_params_alloca(&hwparams); 
     
	/* Init hwparams with full configuration space */ 
	if (snd_pcm_hw_params_any(sndpcm->handle, hwparams) < 0) { 
		cerr<<"Error snd_pcm_hw_params_any"<<endl;
		goto ERR_SET_PARAMS; 
	} 
 
	if (snd_pcm_hw_params_set_access(sndpcm->handle, hwparams
	, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) { 
		cerr<<"Error snd_pcm_hw_params_set_access"<<endl;
		goto ERR_SET_PARAMS; 
	} 
 
	/* Set sample format */ 
	if (SNDWAV_P_GetFormat(wav, &format) < 0) { 
		cerr<<"Error get_snd_pcm_format"<<endl;
	    	goto ERR_SET_PARAMS; 
	} 
    	if (snd_pcm_hw_params_set_format(sndpcm->handle, hwparams, format) < 0) { 
        		cerr<<"Error snd_pcm_hw_params_set_format"<<endl;
	        	goto ERR_SET_PARAMS; 
    	} 
    	sndpcm->format = format; 
 
	/* Set number of channels */ 
	if (snd_pcm_hw_params_set_channels(sndpcm->handle, hwparams
	, LE_SHORT(wav->format.channels)) < 0) { 
	    	cerr<<"Error snd_pcm_hw_params_set_channels"<<endl;
	    	goto ERR_SET_PARAMS; 
	} 
    	sndpcm->channels = LE_SHORT(wav->format.channels); 
 
    	/* Set sample rate. If the exact rate is not supported */ 
    	/* by the hardware, use nearest possible rate.         */  
    	exact_rate = LE_INT(wav->format.sample_rate); 
	if (snd_pcm_hw_params_set_rate_near(sndpcm->handle, hwparams, &exact_rate, 0) < 0) { 
        		cerr<<"Error snd_pcm_hw_params_set_rate_near"<<endl;
        		goto ERR_SET_PARAMS; 
    	} 
    	if (LE_INT(wav->format.sample_rate) != exact_rate) { 
    		cerr<<"The rate "<<LE_INT(wav->format.sample_rate)<<" Hz is not supported by your hardware."
    		<<endl<<"==> Using "<<exact_rate<<" Hz instead.";
	} 
 
    	if (snd_pcm_hw_params_get_buffer_time_max(hwparams, &buffer_time, 0) < 0) { 
        		cerr<<"Error snd_pcm_hw_params_get_buffer_time_max"<<endl;
        		goto ERR_SET_PARAMS; 
    	} 
    	if (buffer_time > 500000) buffer_time = 500000; 
    	period_time = buffer_time / 4; 
 
    	if (snd_pcm_hw_params_set_buffer_time_near(sndpcm->handle, hwparams
	, &buffer_time, 0) < 0) { 
        		cerr<<"Error snd_pcm_hw_params_set_buffer_time_near"<<endl;
       		goto ERR_SET_PARAMS; 
    	} 
 
    	if (snd_pcm_hw_params_set_period_time_near(sndpcm->handle, hwparams
	, &period_time, 0) < 0) { 
        		cerr<<"Error snd_pcm_hw_params_set_period_time_near"<<endl;
        		goto ERR_SET_PARAMS; 
    	} 
 
    	/* Set hw params */ 
    	if (snd_pcm_hw_params(sndpcm->handle, hwparams) < 0) { 
       		cerr<<"Error snd_pcm_hw_params(handle, params)"<<endl;
        		goto ERR_SET_PARAMS; 
    	} 
 
    	snd_pcm_hw_params_get_period_size(hwparams, &sndpcm->chunk_size, 0);     
    	snd_pcm_hw_params_get_buffer_size(hwparams, &sndpcm->buffer_size); 
    	if (sndpcm->chunk_size == sndpcm->buffer_size) {  
    		cerr<<"Can't use period equal to buffer size ("<<sndpcm->chunk_size<<" == "
    		<<sndpcm->buffer_size<<")"<<endl;       
        		goto ERR_SET_PARAMS; 
    	} 
 
    	sndpcm->bits_per_sample = snd_pcm_format_physical_width(format); 
    	sndpcm->bits_per_frame = sndpcm->bits_per_sample * LE_SHORT(wav->format.channels); 
     
    	sndpcm->chunk_bytes = sndpcm->chunk_size * sndpcm->bits_per_frame / 8; 
 
    	/* Allocate audio data buffer */ 
    	sndpcm->data_buf = (uint8_t *)malloc(sndpcm->chunk_bytes); 
    	if (!sndpcm->data_buf) { 
    		cerr<<"Error malloc: [data_buf]"<<endl;
        		goto ERR_SET_PARAMS; 
    	} 
 
    	return 0; 
 
ERR_SET_PARAMS: 
    	return -1; 
} 

int AlsaRec::SNDWAV_P_GetFormat(WAVContainer_t *wav, snd_pcm_format_t *snd_format) 
{    
	if (LE_SHORT(wav->format.format) != WAV_FMT_PCM) 
		return -1; 

	switch (LE_SHORT(wav->format.sample_length)) { 
		case 16: 
			*snd_format = SND_PCM_FORMAT_S16_LE; 
			break; 
		case 8: 
			*snd_format = SND_PCM_FORMAT_U8; 
			break; 
		default: 
			*snd_format = SND_PCM_FORMAT_UNKNOWN; 
			break; 
	} 

	return 0; 
} 

void AlsaRec::SNDWAV_Record(SNDPCMContainer_t *sndpcm, WAVContainer_t *wav, int fd) 
{ 
	uint64_t rest; 
	ssize_t c, frame_size; 

	//写入wav文件头 
	if (WAV_WriteHeader(fd, wav) < 0) { 
		exit(-1); 
	} 

	rest = wav->chunk.length; 
	while (rest > 0) { 
		c = (rest <= (uint64_t)sndpcm->chunk_bytes) ? (size_t)rest : sndpcm->chunk_bytes; 
		frame_size = c * 8 / sndpcm->bits_per_frame; 
		if (SNDWAV_ReadPcm(sndpcm, frame_size) != frame_size) 
			break; 

		if (write(fd, sndpcm->data_buf, c) != c) {
			cout<<"Error SNDWAV_Record[write]"<<endl; 
			exit(-1); 
		} 

	rest -= c; 
	} 
} 

int AlsaRec::WAV_WriteHeader(int fd, WAVContainer_t *container) 
{ 
	assert((fd >=0) && container); 

	if (WAV_P_CheckValid(container) < 0) 
		return -1; 

	if (write(fd,&container->header,sizeof(container->header))!=sizeof(container->header)
	||write(fd,&container->format,sizeof(container->format))!=sizeof(container->format)
	||write(fd,&container->chunk,sizeof(container->chunk))!=sizeof(container->chunk)) { 
		cout<<"Error WAV_WriteHeader"<<endl;
		return -1; 
	} 

	return 0; 
} 

int AlsaRec::WAV_P_CheckValid(WAVContainer_t *container) 
{ 
	if (container->header.magic != WAV_RIFF || 
	container->header.type != WAV_WAVE || 
	container->format.magic != WAV_FMT || 
	container->format.fmt_size != LE_INT(16) || 
	(container->format.channels != LE_SHORT(1) && container->format.channels != LE_SHORT(2))
	|| container->chunk.type != WAV_DATA) {
		cout<<"non standard wav file."<<endl;
		return -1; 
	} 

	return 0; 
} 

ssize_t AlsaRec::SNDWAV_ReadPcm(SNDPCMContainer_t *sndpcm, size_t rcount) 
{ 
	ssize_t r; 
	size_t result = 0; 
	size_t count = rcount; 
	uint8_t *data = sndpcm->data_buf; 

	if (count != sndpcm->chunk_size) { 
		count = sndpcm->chunk_size; 
	} 

	while (count > 0) { 
		r = snd_pcm_readi(sndpcm->handle, data, count); 
		if (r == -EAGAIN || (r >= 0 && (size_t)r < count)) { 
			snd_pcm_wait(sndpcm->handle, 1000); 
		} else if (r == -EPIPE) { 
			snd_pcm_prepare(sndpcm->handle); 
			cout<<"<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>"<<endl;
		} else if (r == -ESTRPIPE) { 
			cout<<"<<<<<<<<<<<<<<< Need suspend >>>>>>>>>>>>>>>"<<endl;
		} else if (r < 0) { 
			cout<<"Error snd_pcm_writei: "<<snd_strerror(r)<<endl;
			exit(-1); 
		} 

		if (r > 0) { 
			result += r; 
			count -= r; 
			data += r * sndpcm->bits_per_frame / 8; 
		} 
	} 

	return rcount; 
} 

// // to many bugs 
// void AlsaRec::SNDDATA_Record(SNDPCMContainer_t *sndpcm, WAVContainer_t *wav) 
// { 
// 	uint64_t rest; 
// 	ssize_t c,frame_size; 

// 	//写入wav文件头 
// 	// if (WAV_WriteHeader(fd, wav) < 0) { 
// 	// 	exit(-1); 
// 	// } 

// 	rest = wav->chunk.length; 
// 	while (rest > 0) { 
// 		c = (rest <= (uint64_t)sndpcm->chunk_bytes) ? (size_t)rest : sndpcm->chunk_bytes; 
// 		frame_size = c * 8 / sndpcm->bits_per_frame; 
// 		if (SNDWAV_ReadPcm(sndpcm, frame_size) != frame_size) 
// 			break; 

// 		// if (write(fd, sndpcm->data_buf, c) != c) {
// 		// 	cout<<"Error SNDWAV_Record[write]"<<endl; 
// 		// 	exit(-1); 
// 		// } 
		
// 	// data += sndpcm->data_buf;
// 	rest -= c; 
// 	}

// } 


// int AlsaRec::recToData(uint32_t duration_time)
// {
// 	const char *devicename = "default"; 
// 	// int fd; 
// 	WAVContainer_t wav; 
// 	SNDPCMContainer_t record; 

// 	memset(&record, 0x0, sizeof(record));//initialization

// 	// remove(filename); //delete old file

// 	// //open/create file
// 	// if ((fd = open(filename, O_WRONLY | O_CREAT, 0644)) == -1) { 
// 	// 	cerr<<"Error open: "<<filename<<endl;
// 	//     	return -1; 
// 	// } 

// 	//open pcm rec device
// 	if (snd_pcm_open(&record.handle, devicename, SND_PCM_STREAM_CAPTURE, 0) < 0) { 
// 		cerr<<"Error snd_pcm_open "<<devicename<<endl;
// 		goto Err; 
// 	} 

// 	// set wav head params
// 	if (SNDWAV_PrepareWAVParams(&wav,duration_time) < 0) { 
// 		cerr<<"Error SNDWAV_PrepareWAVParams"<<endl;
// 	    	goto Err; 
// 	} 

// 	//set device params
// 	if (SNDWAV_SetParams(&record, &wav) < 0) { 
// 		cerr<<"Error set_snd_pcm_params"<<endl;
// 	    	goto Err; 
// 	} 
// //
// 	//start record
// 	SNDDATA_Record(&record, &wav); 
// 	// len   =  wav.chunk.length;

// 	snd_pcm_drain(record.handle); 

// 	// close(fd); 
// 	free(record.data_buf); 
// 	snd_pcm_close(record.handle); 
// 	return 0; 

// Err: 
// 	// close(fd); 
// 	// remove(filename); 
// 	if (record.data_buf) free(record.data_buf); 
// 	if (record.handle) snd_pcm_close(record.handle); 
// 	return -1; 
// }

