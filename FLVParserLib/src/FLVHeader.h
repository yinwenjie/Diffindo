#ifndef _FLV_HEADER_H_
#define _FLV_HEADER_H_

#include "DataTypes.h"

typedef struct FlvHeader
{
	UINT8  signature[3];
	UINT8  version;
	bool   videoFlag;
	bool   audioFlag;
	UINT32 dataOffset;

	FlvHeader()
	{
		signature[3] = { 0 };
		version = 0;
		videoFlag = false;
		audioFlag = false;
		dataOffset = 0;
	}
} FlvHeader;

#endif