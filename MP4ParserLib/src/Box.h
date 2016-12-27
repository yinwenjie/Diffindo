#ifndef _BOX_H_
#define _BOX_H_

#include "Include.h"

typedef struct Box
{
	BYTE *boxBuffer;
	UINT64 usedBytesLength;

	UINT32 size;
	UINT32 boxType;
	UINT64 largeSize;
	UINT8  userType[16];
	Box(BYTE *buf)
	{
		boxBuffer = buf;
		usedBytesLength = 0;
		size = 0;
		boxType = 0;
		memset(userType, 0, 16 * sizeof(UINT8));
	}
	int Get_box_struct();
};

typedef struct FullBox : public Box
{
	UINT8 version;
	UINT32 flags;
	FullBox(BYTE *buf) : Box(buf)
	{
		version = 0;
		flags = 0;
	}
	int Get_full_box_struct();
};

// File Type Box 
typedef struct FileTypeBox : public Box
{
	UINT32 majorBrand;
	UINT32 minorVersion;
	UINT32 *compatibleBrands;
	UINT8  cbNum;

	FileTypeBox(BYTE *buf) : Box(buf)
	{
		majorBrand = 0;
		minorVersion = 0;
		compatibleBrands = NULL;
		cbNum = 0;
	}
	~FileTypeBox()
	{
		if (compatibleBrands)
		{
			delete[] compatibleBrands;
			compatibleBrands = NULL;
		}
	}
	int Get_file_type_box(UINT64 &bytePosition);
	void  dump_file_type_box();
} FileTypeBox;

// Movie Header Box
typedef struct MovieHeaderBox : public FullBox
{
	UINT64 creationTime;
	UINT64 modificationTime;
	UINT32 timeScale;
	UINT64 duration;

	UINT32 rate;
	UINT32 volume;
	UINT32 nextTrackID;
	UINT32 matrix[9];

	MovieHeaderBox(BYTE *buf) : FullBox(buf)
	{
		creationTime = 0;
		modificationTime = 0;
		timeScale = 0;
		duration = 0;

		rate = 0;
		volume = 0;
		nextTrackID = 0;

		memset(matrix, 0, 9 * sizeof(UINT32));
	}
	int Get_movie_header(UINT64 &bytePosition);
	void Dump_movie_header_info();
} MovieHeaderBox;

// Movie Box
typedef struct MovieBox : public Box
{
	MovieHeaderBox *mvhdBox;
	MovieBox(BYTE *buf) : Box(buf)
	{
		mvhdBox = NULL;
	}
	int Get_movie_box(UINT64 &bytePosition);
} MovieBox;

#endif