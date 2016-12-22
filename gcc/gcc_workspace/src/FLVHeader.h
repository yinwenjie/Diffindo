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
		signature[0] = 'F';
		signature[1] = 'L';
		signature[2] = 'V';
		version = 0;
		videoFlag = false;
		audioFlag = false;
		dataOffset = 0;
	}
} FlvHeader;

#endif