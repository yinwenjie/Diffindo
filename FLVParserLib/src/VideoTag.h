#ifndef _VIDEO_TAG_H_
#define _VIDEO_TAG_H_

#include "FLVTag.h"
#include "DataTypes.h"

class CVideoTag : public CFlvTag
{
public:
	CVideoTag(UINT32 idx, UINT8 *buf, UINT8 type);
	virtual ~CVideoTag();

	UINT8 m_frameType;
	UINT8 m_codecID;
	UINT8 m_AVCPacketType;
	int   m_CompositionTime;

	int Parse_video_tag_detail();
	void Dump_video_tag_info();
};

#endif