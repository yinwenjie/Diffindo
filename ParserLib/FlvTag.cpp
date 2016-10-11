#include "stdafx.h"
#include "ParserLib.h"

using namespace std;
CFlvTag::CFlvTag(UINT32 tagIdx, BYTE *FileBuf)
{
	m_tagIdx = tagIdx;
	m_fileBuf = FileBuf;
	m_nextTag = NULL;
	m_tagStartPosition = 0;
	m_tagEndPosition = 0;

	m_tagType = 0;
	m_dataSize = 0;
	m_timeStamp = 0;
	m_timeStampExtension = 0;
	m_streamID = 0;

	m_tagDataBuf = NULL;
}

CFlvTag::~CFlvTag()
{
	if (m_tagDataBuf)
	{
		delete[] m_tagDataBuf;
	}
}

int CFlvTag::Parse(UINT64 &byteCnt)
{
	m_tagStartPosition = byteCnt;
	m_tagType = m_fileBuf[byteCnt++];
	bool filter = static_cast<bool>(m_tagType & 0x20);
	m_tagType = m_tagType & 0x1F;
	if ((m_tagType != TAG_TYPE_AUDIO) && (m_tagType != TAG_TYPE_VIDEO) && (m_tagType != TAG_TYPE_SCRIPT))
	{
		return kFlvParserError_IllegalTagType;
	}

	read_len_swap(reinterpret_cast<BYTE*>(&m_dataSize), m_fileBuf + byteCnt, 3 * sizeof(UINT8));
	byteCnt += 3;

	read_len_swap(reinterpret_cast<BYTE*>(&m_timeStamp), m_fileBuf + byteCnt, 3 * sizeof(UINT8));
	byteCnt += 3;

	m_timeStampExtension = m_fileBuf[byteCnt++];

	read_len_swap(reinterpret_cast<BYTE*>(&m_streamID), m_fileBuf + byteCnt, 3 * sizeof(UINT8));
	byteCnt += 3;
	if (m_streamID != 0)
	{
		return kFlvParserError_StreamIDNotZero;
	}

	m_tagEndPosition = byteCnt + m_dataSize;
	byteCnt += m_dataSize;

	m_tagDataBuf = new BYTE[m_dataSize];
	memcpy_s(m_tagDataBuf, m_dataSize, m_fileBuf + m_tagStartPosition + 11, m_dataSize);

	int err = Parse_detail();
	if (err < 0)
	{
		return err;
	}

	return kFlvParserError_NoError;
}

int CFlvTag::Parse_detail()
{
	int err = 0;
	if (m_tagType == TAG_TYPE_VIDEO)
	{
		m_videoTag = new VideoTag(m_tagDataBuf, m_dataSize);
		err = m_videoTag->Parse();
		if (err < 0)
		{
			return err;
		}
	}
	else if (m_tagType == TAG_TYPE_AUDIO)
	{
		m_audioTag = new AudioTag(m_tagDataBuf, m_dataSize);
		err = m_audioTag->Parse();
		if (err < 0)
		{
			return err;
		}
	}
	return kFlvParserError_NoError;
}

void CFlvTag::Dump_tag_info()
{
#if DUMP_TAG_INFO_ENABLED_LOG
	const char *tagTypeName[3] = 
	{
		"Audio Tag",
		"Video Tag",
		"Script Data Tag"
	};

	int typeIdx = (m_tagType == 8) ? 0 : ((m_tagType == 9) ? 1 : 2);
	g_logoutFile << "----------------------------------" << endl;
	g_logoutFile << "Tag Index: " << to_string(m_tagIdx) << endl;
	g_logoutFile << "Tag Type: " << tagTypeName[typeIdx] << endl;
	g_logoutFile << "Tag DataSize: " << to_string(m_dataSize) << endl;
	g_logoutFile << "Tag Timestamp: " << to_string(m_timeStamp) << endl;
#endif

#if DUMP_VIDEO_TAG_INFO_ENABLED_LOG
	if (m_videoTag)
	{
		m_videoTag->Dump_video_tag_info();
	}
#endif

#if DUMP_AUDIO_TAG_INFO_ENABLED_LOG
	if (m_audioTag)
	{
		m_audioTag->Dump_audio_tag_info();
	}
#endif

}

UINT8 CFlvTag::Get_tag_type()
{
	return m_tagType;
}

// Get start point of this tag in data buffer
UINT64 CFlvTag::Get_start_pos()
{
	return m_tagStartPosition;
}

// Get total length of this tag
UINT64 CFlvTag::Get_tag_total_len()
{
	return m_tagEndPosition - m_tagStartPosition;
}

UINT32 CFlvTag::Get_tag_dataSize()
{
	return m_dataSize;
}

UINT32 CFlvTag::Get_tag_timestamp_ext()
{
	return (static_cast<UINT32>(m_timeStampExtension) << 24) | m_timeStamp;
}

const char * CFlvTag::Get_tag_payload_buf()
{
	return reinterpret_cast<const char *>(m_tagDataBuf);
}

UINT32 CFlvTag::Get_tag_index()
{
	return m_tagIdx;
}

void CFlvTag::Set_tag_timestamp(UINT32 timestamp)
{
	m_timeStamp = timestamp;
}