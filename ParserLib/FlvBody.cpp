#include "stdafx.h"
#include "ParserLib.h"
#include "FlvBody.h"

using namespace std;
// Constructor
CFlvBody::CFlvBody()
{
	m_tagCount = 0;
	m_videoTagCount = 0;
	m_audiotagCount = 0;
	m_firstTag = NULL;
	m_recentTag = NULL;
	m_prevTagSizeVec.clear();
}

// De-constructor
CFlvBody::~CFlvBody()
{
	m_prevTagSizeVec.clear();
	deleteTags();
}

// Open API: Parse FLV body
int CFlvBody::Parse(BYTE *FileBuf, UINT64 &byteCnt, UINT64 fileSize)
{
	int err = 0;
	int tagCnt = 0;

//	while (tagCnt < 10)
	while (byteCnt < fileSize)
	{
		UINT32 prevTagSize = 0;
		read_len_swap(reinterpret_cast<BYTE*>(&prevTagSize), FileBuf + byteCnt, sizeof(UINT32));
		byteCnt += sizeof(UINT32);
		m_prevTagSizeVec.push_back(prevTagSize);
		if ((prevTagSize != 0) && (tagCnt == 0))
		{
			return kFlvParserError_FirstPrevTagSizeNotZero;
		}

		if (byteCnt >= fileSize)
		{
			break;
		}

		CFlvTag *tag = new CFlvTag(tagCnt, FileBuf);
		err = tag->Parse(byteCnt);
		if (err < 0)
		{
			delete tag;
			return err;
		}

		if (tagCnt == 0)
		{
			m_firstTag = tag;
			m_recentTag = m_firstTag;
		}
		else
		{
			m_recentTag->m_nextTag = tag;
			m_recentTag = tag;
		}

		if (tag->Get_tag_type() == TAG_TYPE_VIDEO)
		{
			m_videoTagCount++;
		}
		if (tag->Get_tag_type() == TAG_TYPE_AUDIO)
		{
			m_audiotagCount++;
		}

		tagCnt++;
	}

	return kFlvParserError_NoError;
}

void CFlvBody::Dump_flv_tag_info()
{
	CFlvTag *tag = m_firstTag;

	while (tag)
	{
		tag->Dump_tag_info();
		tag = tag->m_nextTag;
	}

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile << "----------------------------------" << endl;
	g_logoutFile << "Video tag number: " << to_string(m_videoTagCount) << endl;
	g_logoutFile << "Audio tag number: " << to_string(m_audiotagCount) << endl;
	g_logoutFile << "----------------------------------" << endl;
#endif
}

CFlvTag* CFlvBody::Get_first_tag()
{
	return m_firstTag;
}

CFlvTag* CFlvBody::Get_script_data_tag()
{
	CFlvTag *tag = m_firstTag;

	while (tag->Get_tag_type() != TAG_TYPE_SCRIPT)
	{
		tag = tag->m_nextTag;
	}

	return tag;
}

void CFlvBody::deleteTags()
{
	CFlvTag *tag = m_firstTag, *tmpTag = NULL;

	while (tag)
	{
		tmpTag = tag->m_nextTag;
		delete tag;
		tag = tmpTag;
	}
}
