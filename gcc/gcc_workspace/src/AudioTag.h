#ifndef _AUDIO_TAG_H_
#define _AUDIO_TAG_H_

#include "FLVTag.h"
#include "DataTypes.h"

typedef struct AudioSpecificConfig
{
	UINT8  audioObjectType;
	UINT8  samplingFrequencyIndex;
	UINT32 samplingFrequency;
	UINT8  channelConfiguration;
	AudioSpecificConfig()
	{
		audioObjectType = 0;
		samplingFrequencyIndex = 0;
		samplingFrequency = 0;
		channelConfiguration = 0;
	}
	int Parse_audio_spec_config(BYTE *buf)
	{
		int SampleFreqArr[13] = { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 };

		audioObjectType = buf[0] >> 3;
		if (audioObjectType >= 31)
		{
			return -255;
		}
		samplingFrequencyIndex = ((buf[0] & 0x07) << 1) | ((buf[1] & 0x80) >> 7);
		if ((samplingFrequencyIndex == 0x0f) || (samplingFrequencyIndex > 12) || (samplingFrequencyIndex < 0))
		{
			return -255;
		}
		else
		{
			samplingFrequency = SampleFreqArr[samplingFrequencyIndex];
		}
		channelConfiguration = (buf[1] & 0x7F) >> 3;

		return kFlvParserError_NoError;
	}
} AudioSpecificConfig;

class CAudioTag : public CFlvTag
{
public:
	CAudioTag(UINT32 idx, UINT8 *buf, UINT8 type);
	~CAudioTag();

	UINT8 m_soundFormat;
	UINT8 m_soundRate;
	UINT8 m_soundSize;
	UINT8 m_soundType;
	UINT8 m_aacPacketType;

	AudioSpecificConfig *m_audioSpecCfg;

	int Parse_audio_tag_detail();
	void Dump_audio_tag_info();
};

#endif