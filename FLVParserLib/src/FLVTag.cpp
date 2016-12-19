#include "FLVParserLib.h"
#include "FLVTag.h"
#include "VideoTag.h"
#include "AudioTag.h"

using namespace std;

CFlvTag::CFlvTag(UINT32 idx, UINT8 *buf, UINT8 type)
{
	m_tagBuffer = buf;
	m_nextTag = NULL;

	m_tagIdx = idx;
	m_tagType = type;
	m_dataSize = 0;
	m_timeStamp = 0;
	m_timeStampExtension = 0;
	m_streamID = 0;
}

CFlvTag::~CFlvTag()
{
}

int CFlvTag::Parse_flv_tag()
{
	Read_data_lsb(&m_dataSize, m_tagBuffer + 1, 3);
	Read_data_lsb(&m_timeStamp, m_tagBuffer + 4, 3);
	m_timeStampExtension = m_tagBuffer[7];
	Read_data_lsb(&m_streamID, m_tagBuffer + 8, 3);
	if (m_streamID != 0)
	{
		return kFlvParserError_StreamIDNotZero;
	}

	if (m_tagType == TAG_TYPE_VIDEO)
	{
		CVideoTag *videoTagPtr = dynamic_cast<CVideoTag *>(this);
		if (NULL != videoTagPtr)
		{
			videoTagPtr->Parse_video_tag_detail();			
		}
	}
	else if (m_tagType == TAG_TYPE_AUDIO)
	{
		CAudioTag *audioTagPtr = dynamic_cast<CAudioTag *>(this);
		if (NULL != audioTagPtr)
		{
			audioTagPtr->Parse_audio_tag_detail();
		}
	}
	
	return kFlvParserError_NoError;
}

void CFlvTag::Dump_tag_info()
{
#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile << "-----------------------------------" << endl;
	g_logoutFile << "Tag Index: " << to_string(m_tagIdx) << endl;
	g_logoutFile << "Tag Type: " << (m_tagType == 18 ? "Script" : (m_tagType == 8 ? "Audio" : "Video")) << endl;
	g_logoutFile << "Data Size: " << to_string(m_dataSize) << endl;
	g_logoutFile << "TimeStamp: " << to_string(m_timeStamp) << endl;
	g_logoutFile << "TimeStampExtension: " << to_string(m_timeStampExtension) << endl;
	g_logoutFile << "StreamID: " << to_string(m_streamID) << endl;

	if (m_tagType == TAG_TYPE_VIDEO)
	{
		CVideoTag *videoTagPtr = dynamic_cast<CVideoTag *>(this);
		if (NULL != videoTagPtr)
		{
			videoTagPtr->Dump_video_tag_info();
		}
	}
	else if (m_tagType == TAG_TYPE_AUDIO)
	{
		CAudioTag *audioTagPtr = dynamic_cast<CAudioTag *>(this);
		if (NULL != audioTagPtr)
		{
			audioTagPtr->Dump_audio_tag_info();
		}
	}

#endif
}
