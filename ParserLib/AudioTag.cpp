#include "stdafx.h"
#include "ParserLib.h"

using namespace std;

AudioSpecificConfig::AudioSpecificConfig(BYTE *dataBuf)
{
	m_dataBuf = dataBuf;
}

AudioSpecificConfig::~AudioSpecificConfig()
{
}

int AudioSpecificConfig::Parse()
{
	int SampleFreqArr[13] = {96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350};

	m_audioObjectType = m_dataBuf[0] >> 3;
	if (m_audioObjectType >= 31)
	{
		return -255;
	}

	m_samplingFrequencyIndex = ((m_dataBuf[0] & 0x07) << 1) | ((m_dataBuf[1] & 0x80) >> 7);
	if ((m_samplingFrequencyIndex == 0x0f) || (m_samplingFrequencyIndex > 12) || (m_samplingFrequencyIndex < 0))
	{
		return -255;
	}
	else
	{
		m_samplingFrequency = SampleFreqArr[m_samplingFrequencyIndex];
	}
	m_channelConfiguration = (m_dataBuf[1] & 0x7F) >> 3;

	return kFlvParserError_NoError;
}

void AudioSpecificConfig::Dump_audio_speci_config()
{
#if DUMP_AUDIO_TAG_INFO_ENABLED_LOG
	g_logoutFile << "AudioObjectType: " << to_string(m_audioObjectType) << endl;
	g_logoutFile << "samplingFrequencyIndex: " << to_string(m_samplingFrequencyIndex) << endl;
	g_logoutFile << "samplingFrequency: " << to_string(m_samplingFrequency) << endl;
	g_logoutFile << "channelConfiguration: " << to_string(m_channelConfiguration) << endl;
#endif
}

AudioTag::AudioTag(BYTE *dataBuf, UINT32 dataSize)
{
	m_dataBuffer = dataBuf;
	m_dataSize = dataSize;
	m_audioTagHeader = NULL;
	m_audioSpecCfg = NULL;
}

AudioTag::~AudioTag()
{
	if (m_audioTagHeader)
	{
		delete m_audioTagHeader;
		m_audioTagHeader = NULL;
	}
	if (m_audioSpecCfg)
	{
		delete m_audioSpecCfg;
		m_audioSpecCfg = NULL;
	}
}


int AudioTag::Parse()
{
	int err = 0;
	m_audioTagHeader = new AudioTagHeader;
	err = get_audio_tag_header();
	if (err < 0)
	{
		return err;
	}

	if (0 == m_audioTagHeader->AACPacketType)
	{
		//AudioSpecificConfig...
		m_audioSpecCfg = new AudioSpecificConfig(m_dataBuffer + 2);
		m_audioSpecCfg->Parse();
	}

	return kFlvParserError_NoError;
}

void AudioTag::Dump_audio_tag_info()
{
#if DUMP_AUDIO_TAG_INFO_ENABLED_LOG
	int soundRateArr[4] = { 5500, 11000, 22000, 44000 };

	g_logoutFile << "Sound Format: " << to_string(m_audioTagHeader->soundFormat) <<endl;
	g_logoutFile << "Sound Rate: " << to_string(soundRateArr[m_audioTagHeader->soundRate]) << endl;
	g_logoutFile << "Sound Size: " << (m_audioTagHeader->soundSize ? "8 bits" : "16 bits") << endl;
	g_logoutFile << "Sound Type: " << (m_audioTagHeader->soundType ? "Stereo" : "Mono") << endl;
	if (10 == m_audioTagHeader->soundFormat)
	{
		g_logoutFile << "AAC Packet Type: " << (m_audioTagHeader->AACPacketType ? "AAC raw" : "AAC sequence header") << endl;
	}

	if (m_audioSpecCfg)
	{
		m_audioSpecCfg->Dump_audio_speci_config();
	}
#endif
}

int AudioTag::get_audio_tag_header()
{
	m_audioTagHeader->soundFormat = m_dataBuffer[0] >> 4;
	m_audioTagHeader->soundRate = (m_dataBuffer[0] & 0x0f) >> 2;
	m_audioTagHeader->soundSize = m_dataBuffer[0] & 0x2;
	m_audioTagHeader->soundType = m_dataBuffer[0] & 0x1;

	if (m_audioTagHeader->soundFormat == 10)
	{
		m_audioTagHeader->AACPacketType = m_dataBuffer[1];
	}
	else
	{
		m_audioTagHeader->AACPacketType = 255;
	}

	return kFlvParserError_NoError;
}
