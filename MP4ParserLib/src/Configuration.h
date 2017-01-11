#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define DUMP_MP4_INFO_ENABLED_LOG 1

typedef struct MP4RuntimeConfiguration
{
	UINT8 mp4Configlevel;
	MP4RuntimeConfiguration()
	{
		mp4Configlevel = 0;
	}
} MP4RuntimeConfiguration;

#endif