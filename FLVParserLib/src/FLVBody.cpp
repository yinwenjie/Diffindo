#include "FLVParserLib.h"
#include "FLVBody.h"
#include "FLVTag.h"
#include "VideoTag.h"

CFlvBody::CFlvBody()
{
	m_tagCount = 0;
	m_videoTagCount = 0;
	m_audiotagCount = 0;

	m_firstTag = NULL;
	m_recentTag = NULL;
}

CFlvBody::~CFlvBody()
{
	deleteTags();
}

int CFlvBody::Parse(BYTE *FileBuf, UINT64 &byteCnt, UINT64 fileSize)
{
	int err = 0;

	while (byteCnt < fileSize)
	{
		CFlvTag *tag = NULL;
		UINT32 prevTagSize = 0;
		Read_data_lsb(&prevTagSize, FileBuf + byteCnt, 4);
		byteCnt += sizeof(UINT32);

		if ((prevTagSize != 0) && (m_tagCount == 0))
		{
			return kFlvParserError_FirstPrevTagSizeNotZero;
		}
		if (byteCnt >= fileSize)
		{
			break;
		}

		UINT8 tagType = FileBuf[byteCnt];
		switch (tagType)
		{
		case TAG_TYPE_VIDEO:
			tag = new CVideoTag(m_tagCount, FileBuf + byteCnt, tagType);
			break;
		case TAG_TYPE_AUDIO:
		case TAG_TYPE_SCRIPT:
			tag = new CFlvTag(m_tagCount, FileBuf + byteCnt, tagType);
			break;
		default:
			return kFlvParserError_IllegalTagType;
		}

		err = tag->Parse_flv_tag();
		if (err < 0)
		{
			return err;
		}
		tag->Dump_tag_info();

		if (m_tagCount == 0)
		{
			m_firstTag = tag;
			m_recentTag = m_firstTag;
		}
		else
		{
			m_recentTag->m_nextTag = tag;
			m_recentTag = tag;
		}

		m_tagCount++;
		byteCnt += tag->m_dataSize + 11;
	}

	return kFlvParserError_NoError;
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