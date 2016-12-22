#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* 设置输出日志开关 */
#define DUMP_TAG_INFO_ENABLED_CONSOLE 0					//在控制台窗口输出Tag信息
#define DUMP_VIDEO_TAG_INFO_ENABLED_CONSOLE 0			//控制台窗口输出视频Tag详细信息

#define DUMP_TAG_INFO_ENABLED_LOG 1						//在日志文件中输出Tag信息
#define DUMP_VIDEO_TAG_INFO_ENABLED_LOG 1				//在日志文件中输出视频Tag详细信息

#define DUMP_VIDEO_TAG_PAYLOAD_INFO 1
#define DUMP_DECODER_CONFIG_RECORD_LOG 1				//在日志文件中输出AVCDecoderConfigurationRecord信息
#define DUMP_NAL_UNIT_INFO_LOG 1						//在日志文件中输出NAL Unit信息

#define DUMP_AUDIO_TAG_INFO_ENABLED_LOG 1				//在日志文件中输出音频Tag详细信息

#define DUMP_ONLY_VIDEO_TAG_INFO 1						//只输出视频Tag的信息

#define DUMP_SCRIPT_INFO_ENABLED_LOG 1					//在日志文件中输出视频script信息
#define DUMP_SCRIPT_INFO_ENABLED_CONSOLE 1				//控制台窗口输出Script Tag详细信息

typedef struct RuntimeConfiguration
{
	UINT8 flvLogLevel;		// 0:Flv tag;	1:Video/Audio tag;	2:Video/Audio payload
	RuntimeConfiguration()
	{
		flvLogLevel = 0;
	}
} RuntimeConfiguration;

#endif // _CONFIGURATION_H_