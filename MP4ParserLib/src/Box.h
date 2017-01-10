#ifndef _BOX_H_
#define _BOX_H_

#include "Include.h"

typedef struct SampleInfo
{
	UINT32 sampleIndex;
	UINT32 sampleSize;
	UINT32 chunkIndex;
	bool   keyFrame;
	UINT32 decodingTime;
	UINT32 compositionTime;
	UINT32 fileBitOffset;
} SampleInfo;

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

// smhd
typedef struct SoundMediaHeaderBox : public FullBox
{
	UINT16 balance;
	SoundMediaHeaderBox(BYTE *buf) : FullBox(buf)
	{
		balance = 0;
	}

	int Get_sound_media_header(UINT64 &bytePosition);
	void Dump_audio_media_header_info();
}SoundMediaHeaderBox;

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

// stts
typedef struct TimeToSampleBox : public FullBox
{
	UINT32 entryCount;
	UINT32 *pSampleCount;
	UINT32 *pSampleDelta;
	TimeToSampleBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
		pSampleCount = NULL;
		pSampleDelta = NULL;
	}
	~TimeToSampleBox()
	{
		if (pSampleCount)
		{
			delete[] pSampleCount;
			pSampleCount = NULL;
		}
		if (pSampleDelta)
		{
			delete[] pSampleDelta;
			pSampleDelta = NULL;
		}
	}
	int Get_time_to_sample_box(UINT64 &bytePosition);
	void Dump_time_to_sample_info();
}TimeToSampleBox;

// stss
typedef struct SyncSampleBox : public FullBox
{
	UINT32 entryCount;
	UINT32 *pSampleNumber;
	SyncSampleBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
		pSampleNumber = NULL;
	}
	~SyncSampleBox()
	{
		if (pSampleNumber)
		{
			delete[] pSampleNumber;
			pSampleNumber = NULL;
		}
	}
	int Get_sync_sample_box(UINT64 &bytePosition);
	void Dump_sync_sample_info();
}SyncSampleBox;

// ctts
typedef struct CompositionOffsetBox : public FullBox
{
	UINT32 entryCount;
	UINT32 *pSampleCount;
	UINT32 *pSampleOffset;
	CompositionOffsetBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
		pSampleCount = NULL;
		pSampleOffset = NULL;
	}
	~CompositionOffsetBox()
	{
		if (pSampleCount)
		{
			delete[] pSampleCount;
			pSampleCount = NULL;
		}
		if (pSampleOffset)
		{
			delete[] pSampleOffset;
			pSampleOffset = NULL;
		}
	}
	int Get_composition_offset_box(UINT64 &bytePosition);
	void Dump_composition_offset_info();
} CompositionOffsetBox;

// stsc
typedef struct SampleToChunkBox : public FullBox
{
	UINT32 entryCount;
	UINT32 *pFirstChunk;
	UINT32 *pSamplesPerChunk;
	UINT32 *pSampleDiscriptionIdx;
	SampleToChunkBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
		pFirstChunk = NULL;
		pSamplesPerChunk = NULL;
		pSampleDiscriptionIdx = NULL;
	}
	~SampleToChunkBox()
	{
		if (pFirstChunk)
		{
			delete[] pFirstChunk;
			pFirstChunk = NULL;
		}
		if (pSamplesPerChunk)
		{
			delete[] pSamplesPerChunk;
			pSamplesPerChunk = NULL;
		}
		if (pSampleDiscriptionIdx)
		{
			delete[] pSampleDiscriptionIdx;
			pSampleDiscriptionIdx = NULL;
		}
	}
	int Get_sample_to_chunk_box(UINT64 &bytePosition);
	void Dump_sample_to_chunk_info();
} SampleToChunkBox;

// stsz
typedef struct SampleSizeBox : public FullBox
{
	UINT32 sampleSize;
	UINT32 sampleCount;
	UINT32 *pEntrySize;
	SampleSizeBox(BYTE *buf) : FullBox(buf)
	{
		sampleSize = 0;
		sampleCount = 0;
		pEntrySize = NULL;
	}
	~SampleSizeBox()
	{
		if (pEntrySize)
		{
			delete[] pEntrySize;
			pEntrySize = NULL;
		}
	}
	int Get_sample_size_box(UINT64 &bytePosition);
	void Dump_sample_size_info();
} SampleSizeBox;

// stco
typedef struct ChunkOffsetBox : public FullBox
{
	UINT32 entryCount;
	UINT32 *pChunkOffset;
	ChunkOffsetBox(BYTE *buf) : FullBox(buf)
	{
		entryCount = 0;
		pChunkOffset = NULL;
	}
	~ChunkOffsetBox()
	{
		if (pChunkOffset)
		{
			delete[] pChunkOffset;
			pChunkOffset = NULL;
		}
	}
	int Get_chunk_offset_box(UINT64 &bytePosition);
	void Dump_chunk_offset_info();
}ChunkOffsetBox;

// stbl
typedef struct SampleTableBox : public Box
{
	SampleDescriptionBox	*stsdBox;
	TimeToSampleBox			*sttsBox;
	SyncSampleBox			*stssBox;
	CompositionOffsetBox	*cttsBox;
	SampleToChunkBox		*stscBox;
	SampleSizeBox			*stszBox;
	ChunkOffsetBox			*stcoBox;

	SampleInfo				*sampleInfoArr;

	SampleTableBox(BYTE *buf) : Box(buf)
	{
		stsdBox = NULL;
		sttsBox = NULL;
		stssBox = NULL;
		cttsBox = NULL;
		stscBox = NULL;
		stszBox = NULL;
		stcoBox = NULL;

		sampleInfoArr = NULL;
	}
	~SampleTableBox()
	{
		if (stsdBox)
		{
			delete stsdBox;
			stsdBox = NULL;
		}

		if (sttsBox)
		{
			delete sttsBox;
			stsdBox = NULL;
		}
		if (stssBox)
		{
			delete stssBox;
			stssBox = NULL;
		}
		if (cttsBox)
		{
			delete cttsBox;
			cttsBox = NULL;
		}
		if (stscBox)
		{
			delete stscBox;
			stscBox = NULL;
		}
		if (stszBox)
		{
			delete stszBox;
			stszBox = NULL;
		}
		if (stcoBox)
		{
			delete stcoBox;
			stcoBox = NULL;
		}
		if (sampleInfoArr)
		{
			delete[] sampleInfoArr;
			sampleInfoArr = NULL;
		}
	}
	int Get_sample_table(UINT64 &bytePosition);
	void Calculate_sample_info();
}SampleTableBox;

// minf
typedef struct MediaInfoBox : public Box
{
	VideoMediaHeaderBox		*vmhdBox;
	SoundMediaHeaderBox		*smhdBox;
	DataInfoBox				*dinfBox;
	SampleTableBox			*stblBox;
	MediaInfoBox(BYTE *buf) : Box(buf)
	{
		vmhdBox = NULL;
		smhdBox = NULL;
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
		if (smhdBox)
		{
			delete smhdBox;
			smhdBox = NULL;
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

// mdia
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

// trak
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

// meta
typedef struct MetaBox : public FullBox
{
	Handlerbox		*hdlrBox;
	MetaBox(BYTE *buf) : FullBox(buf)
	{
		hdlrBox = NULL;
	}
	~MetaBox()
	{
		if (hdlrBox)
		{
			delete hdlrBox;
			hdlrBox = NULL;
		}
	}
	int Get_meta_box(UINT64 &bytePosition);
}MetaBox;

// udta
typedef struct UserDataBox : public Box
{
	MetaBox *metaBox;
	UserDataBox(BYTE *buf) : Box(buf)
	{
		metaBox = NULL;
	}
	~UserDataBox()
	{
		if (metaBox)
		{
			delete metaBox;
			metaBox = NULL;
		}
	}
	int Get_user_data_box(UINT64 &bytePosition);
}UserDataBox;

// mvhd
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

// moov
typedef struct MovieBox : public Box
{
	MovieHeaderBox  *mvhdBox;
	TrackBox		*trakBox;
	UserDataBox		*udtaBox;
	MovieBox(BYTE *buf) : Box(buf)
	{
		mvhdBox = NULL;
		trakBox = NULL;
		udtaBox = NULL;
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
		if (!udtaBox)
		{
			delete udtaBox;
			udtaBox = NULL;
		}
	}
	int Get_movie_box(UINT64 &bytePosition);
} MovieBox;

#endif