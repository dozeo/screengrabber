#ifdef __cplusplus 
extern "C" { 
#endif

#ifdef WIN32
	// the following define is necessary to avoid an error in using macros from stdint.h header file
	// see http://ffmpeg.zeranoe.com/forum/viewtopic.php?f=5&t=130
	// for more details
	#ifndef __STDC_CONSTANT_MACROS
		#define __STDC_CONSTANT_MACROS
	#endif
#endif

	#include <libavcodec/avcodec.h>
	#include <libavformat/avio.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/opt.h>
	#include <libavutil/time.h>

#ifdef __cplusplus 
} 
#endif
