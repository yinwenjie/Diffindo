#ifndef _FLV_TAG_H_
#define _FLV_TAG_H_

#include "DataTypes.h"

class CFlvTag
{
public:
	CFlvTag(UINT32 idx, UINT8 *buf, UINT8 type);
	virtual ~CFlvTag();

	UINT32  m_prevTagSize;

	UINT32	m_tagIdx;				//total tag index
	UINT8	m_tagType;				//current tag type - script data / audio / video
	UINT32	m_dataSize;				//payload data size of this tag
	UINT32	m_timeStamp;			//presenting time of this tag
	UINT8	m_timeStampExtension;	//timestamp extension
	UINT32	m_streamID;				//streamID, must be 0
	
	// Pointer to next tag in the list
	CFlvTag* m_nextTag;

	int Parse_flv_tag();
	void Dump_tag_info();

	UINT8*	m_tagBuffer;			//tag data buffer
};

#endif