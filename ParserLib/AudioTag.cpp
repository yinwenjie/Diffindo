#include "stdafx.h"
#include "ParserLib.h"

using namespace std;

AudioTag::AudioTag(BYTE *dataBuf, UINT32 dataSize)
{
	m_dataBuffer = dataBuf;
	m_dataSize = dataSize;
}

AudioTag::~AudioTag()
{
	if (m_audioTagHeader)
	{
		delete m_audioTagHeader;
		m_audioTagHeader = NULL;
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

	return kFlvParserError_NoError;
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

	return kFlvParserError_NoError;
}
