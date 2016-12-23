#ifndef _BOX_H_
#define _BOX_H_

#include "Include.h"

typedef struct Box
{
	UINT32 size;
	UINT32 boxType;
	UINT64 largeSize;
	UINT8  userType[16];
	Box(UINT32 type)
	{
		size = 0;
		boxType = type;
		largeSize = 0;
		memset(userType, 0, 16 * sizeof(UINT8));
	}
};

typedef struct FullBox : public Box
{
	UINT8 version;
	UINT32 flags;
	FullBox(UINT32 type, UINT8 v, UINT32 f) : Box(type)
	{
		version = v;
		flags = f;
	}
};
#endif