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
	void Dump_box_info();
}Box;

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
	void Dump_full_box_info();
}FullBox;

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

// Track Header box
typedef struct TrackHeaderBox : public FullBox
{
	UINT64 creationTime;
	UINT64 modificationTime;
	UINT32 trackID;
	UINT64 duration;

	UINT16 layer;
	UINT16 alternateGroup;
	UINT16 volume;
	UINT32 matrix[9];

	UINT32 width;
	UINT32 height;

	TrackHeaderBox(BYTE *buf) : FullBox(buf)
	{
		creationTime = 0;
		modificationTime = 0;
		trackID = 0;
		duration = 0;

		layer = 0;
		alternateGroup = 0;
		volume = 0;

		width = 0;
		height = 0;
		memset(matrix, 0, 9 * sizeof(UINT32));
	}
	int Get_track_header(UINT64 &bytePosition);
	void Dump_track_header_info();
} TrackHeaderBox;

// Edit List Box
typedef struct EditListBox : public FullBox
{
	UINT32 entryCnt;
	UINT64 *segmentDuration;
	INT64  *mediaTime;
	short  *mediaRateInteger;
	short  *mediaRateFraction;
	EditListBox(BYTE *buf) : FullBox(buf)
	{
		entryCnt = 0;
		segmentDuration = NULL;
		mediaTime = NULL;
		mediaRateInteger = NULL;
		mediaRateFraction = NULL;
	}
	~EditListBox()
	{
		if (segmentDuration)
		{
			delete segmentDuration;
			segmentDuration = NULL;
		}
		if (mediaTime)
		{
			delete mediaTime;
			mediaTime = NULL;
		}
		if (mediaRateInteger)
		{
			delete mediaRateInteger;
			mediaRateInteger = NULL;
		}
		if (mediaRateFraction)
		{
			delete mediaRateFraction;
			mediaRateFraction = NULL;
		}
	}
	int Get_edit_list_box(UINT64 &bytePosition);
	void Dump_edit_list_info();
} EditListBox;

// Edit Box
typedef struct EditBox : public Box
{
	EditListBox *elstBox;
	EditBox(BYTE *buf) : Box(buf)
	{
		elstBox = NULL;
	}
	~EditBox()
	{
		if (elstBox)
		{
			delete elstBox;
			elstBox = NULL;
		}
	}
	int Get_edit_box(UINT64 &bytePosition);
} EditBox;

// Media Header Box
typedef struct MediaHeaderBox : public FullBox
{
	UINT64 creationTime;
	UINT64 modificationTime;
	UINT32 timeScale;
	UINT64 duration;

	UINT32 languageCode;

	MediaHeaderBox(BYTE *buf) : FullBox(buf)
	{
		creationTime = 0;
		modificationTime = 0;
		timeScale = 0;
		duration = 0;
		languageCode = 0;
	}

	int Get_media_header(UINT64 &bytePosition);
	void Dump_media_header_info();
}MediaHeaderBox;

// Handler Reference Box
typedef struct Handlerbox : public FullBox
{
	UINT32 handlerType;
	char *name;
	Handlerbox(BYTE *buf) : FullBox(buf)
	{
		handlerType = 0;
		name = NULL;
	}
	~Handlerbox()
	{
		if (name)
		{
			delete name;
			name = NULL;
		}
	}
	int Get_handler_box(UINT64 &bytePosition);
	void Dump_handler_box_info();
} Handlerbox;

// vmhd box
typedef struct VideoMediaHeaderBox : public FullBox
{
	UINT16 graphicsmode;
	UINT16 opcolor[3];
	VideoMediaHeaderBox(BYTE *buf) : FullBox(buf)
	{
		graphicsmode = 0;
		memset(opcolor, 0, 3 * sizeof(UINT16));
	}
	int Get_video_media_header(UINT64 &bytePosition);
	void Dump_video_media_header_info();
} VideoMediaHeaderBox;

// dref
typedef struct DataReferenceBox : public FullBox
{
	UINT32 entryCount;
	DataReferenceBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
	}
	int Get_data_ref(UINT64 &bytePosition);
	void Dump_data_ref_info();
} DataReferenceBox;

// dinf
typedef struct DataInfoBox : public Box
{
	DataReferenceBox *drefBox;
	DataInfoBox(BYTE *buf) : Box(buf)
	{
		drefBox = NULL;
	}
	~DataInfoBox()
	{
		if (drefBox)
		{
			delete drefBox;
			drefBox = NULL;
		}
	}
	int Get_data_info_box(UINT64 &bytePosition);
} DataInfoBox;

// stsd
typedef struct SampleDescriptionBox : public FullBox
{
	UINT32 entryCount;
	SampleDescriptionBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
	}
	~SampleDescriptionBox()
	{

	}

	int Get_sample_description_box(UINT64 &bytePosition);
	void Dump_sample_description_info();
} SampleDescriptionBox;

// stbl
typedef struct SampleTableBox : public Box
{
	SampleDescriptionBox	*stsdBox;
	SampleTableBox(BYTE *buf) : Box(buf)
	{
		stsdBox = NULL;
	}
	~SampleTableBox()
	{
		if (stsdBox)
		{
			delete stsdBox;
			stsdBox = NULL;
		}
	}
	int Get_sample_table(UINT64 &bytePosition);
}SampleTableBox;

// minf
typedef struct MediaInfoBox : public Box
{
	VideoMediaHeaderBox		*vmhdBox;
	DataInfoBox				*dinfBox;
	SampleTableBox			*stblBox;
	MediaInfoBox(BYTE *buf) : Box(buf)
	{
		vmhdBox = NULL;
		dinfBox = NULL;
		stblBox = NULL;
	}
	~MediaInfoBox()
	{
		if (vmhdBox)
		{
			delete vmhdBox;
			vmhdBox = NULL;
		}
		if (dinfBox)
		{
			delete dinfBox;
			dinfBox = NULL;
		}
		if (stblBox)
		{
			delete stblBox;
			stblBox = NULL;
		}
	}
	int Get_media_info_box(UINT64 &bytePosition);
} MediaInfoBox;

// Media Box
typedef struct MediaBox : public Box
{
	MediaHeaderBox	*mdhdBox;
	Handlerbox		*hdlrBox;
	MediaInfoBox	*minfBox;
	MediaBox(BYTE *buf) : Box(buf)
	{
		mdhdBox = NULL;
		hdlrBox = NULL;
		minfBox = NULL;
	}
	~MediaBox()
	{
		if (mdhdBox)
		{
			delete mdhdBox;
			mdhdBox = NULL;
		}
		if (hdlrBox)
		{
			delete hdlrBox;
			hdlrBox = NULL;
		}
		if (minfBox)
		{
			delete minfBox;
			minfBox = NULL;
		}
	}
	int Get_media(UINT64 &bytePosition);
}MediaBox;

// Track Box
typedef struct TrackBox : public Box
{
	TrackHeaderBox *tkhdBox;
	EditBox *edtsBox;
	MediaBox *mdiaBox;
	TrackBox(BYTE *buf) : Box(buf)
	{
		tkhdBox = NULL;
		edtsBox = NULL;
		mdiaBox = NULL;
	}
	~TrackBox()
	{
		if (tkhdBox)
		{
			delete tkhdBox;
			tkhdBox = NULL;
		}
		if (edtsBox)
		{
			delete edtsBox;
			edtsBox = NULL;
		}
		if (mdiaBox)
		{
			delete mdiaBox;
			mdiaBox = NULL;
		}
	}
	int Get_track(UINT64 &bytePosition);
} TrackBox;

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
	MovieHeaderBox  *mvhdBox;
	TrackBox		*trakBox;
	MovieBox(BYTE *buf) : Box(buf)
	{
		mvhdBox = NULL;
		trakBox = NULL;
	}
	~MovieBox()
	{
		if (!mvhdBox)
		{
			delete mvhdBox;
			mvhdBox = NULL;
		}
		if (!trakBox)
		{
			delete trakBox;
			trakBox = NULL;
		}
	}
	int Get_movie_box(UINT64 &bytePosition);
} MovieBox;

#endif