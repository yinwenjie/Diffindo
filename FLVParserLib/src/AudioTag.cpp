#include "FLVParserLib.h"
#include "AudioTag.h"

using namespace std;

CAudioTag::CAudioTag(UINT32 idx, UINT8 *buf, UINT8 type) : CFlvTag(idx, buf, type)
{
	m_soundFormat = 0;
	m_soundRate = 0;
	m_soundSize = 0;
	m_soundType = 0;
	m_aacPacketType = 0;

	m_audioSpecCfg = NULL;
}

CAudioTag::~CAudioTag()
{
	if (m_audioSpecCfg)
	{
		delete m_audioSpecCfg;
		m_audioSpecCfg = NULL;
	}
}

int CAudioTag::Parse_audio_tag_detail()
{
	int err = 0;
	m_soundFormat = m_tagBuffer[11] >> 4;
	m_soundRate = (m_tagBuffer[11] & 0x0f) >> 2;
	m_soundSize = (m_tagBuffer[11] & 0x02) >> 1;
	m_soundType = m_tagBuffer[11] & 0x01;
	if (m_soundFormat == 10)
	{
		m_aacPacketType = m_tagBuffer[12];
	}
	else
	{
		m_aacPacketType = 255;
	}

	if (m_aacPacketType == 0)
	{
		m_audioSpecCfg = new AudioSpecificConfig;
		err = m_audioSpecCfg->Parse_audio_spec_config(m_tagBuffer + 13);
		if (err < 0)
		{
			return err;
		}
	}

	return kFlvParserError_NoError;
}

void CAudioTag::Dump_audio_tag_info()
{
#if DUMP_TAG_INFO_ENABLED_LOG

#if DUMP_AUDIO_TAG_INFO_ENABLED_LOG
	if (g_flvConfig.flvLogLevel >= 1)
	{
		int soundRateArr[4] = { 5500, 11000, 22000, 44000 };
		g_logoutFile << "Sound Format: " << to_string(m_soundFormat) << endl;
		g_logoutFile << "Sound Rate: " << to_string(soundRateArr[m_soundRate]) << endl;
		g_logoutFile << "Sound Size: " << (m_soundSize ? "8 bits" : "16 bits") << endl;
		g_logoutFile << "Sound Type: " << (m_soundType ? "Stereo" : "Mono") << endl;
		if (10 == m_soundFormat)
		{
			g_logoutFile << "AAC Packet Type: " << (m_aacPacketType ? "AAC raw" : "AAC sequence header") << endl;
		}
	}

	if (g_flvConfig.flvLogLevel == 2)
	{
		if (m_audioSpecCfg)
		{
			g_logoutFile << "AudioObjectType: " << to_string(m_audioSpecCfg->audioObjectType) << endl;
			g_logoutFile << "samplingFrequencyIndex: " << to_string(m_audioSpecCfg->samplingFrequencyIndex) << endl;
			g_logoutFile << "samplingFrequency: " << to_string(m_audioSpecCfg->samplingFrequency) << endl;
			g_logoutFile << "channelConfiguration: " << to_string(m_audioSpecCfg->channelConfiguration) << endl;
		}
	}
#endif

#endif
}
