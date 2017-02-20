#include "MP4ParserLib.h"
#include "Box.h"

using namespace std;

int Box::Get_box_struct()
{
	size = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	boxType = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	if (size == 1)
	{
		largeSize = Get_lsb_uint64_value(boxBuffer, usedBytesLength);		
	}

	// 'uuid' not supported.. 
	
	return kMP4ParserError_NoError;
}

void Box::Dump_box_info()
{
	char forcc[5] = { 0 };

	cout << "--------------------------------------------" << endl;
	cout << "Box Size: " << to_string(size) << endl;
	UINT32_to_fourcc(boxType, forcc);
	cout << "Box Type: " << forcc << endl;

#if DUMP_MP4_INFO_ENABLED_LOG
	g_logoutFile << "--------------------------------------------" << endl;
	g_logoutFile << "Box Size: " << to_string(size) << endl;
	g_logoutFile << "Box Type: " << forcc << endl;
#endif
}

void FullBox::Dump_full_box_info()
{
	Dump_box_info();

	cout << "Version: " << to_string(version) << endl;
	cout << "Flags: " << to_string(flags) << endl;

#if DUMP_MP4_INFO_ENABLED_LOG
	g_logoutFile << "Version: " << to_string(version) << endl;
	g_logoutFile << "Flags: " << to_string(flags) << endl;
#endif
}

int FullBox::Get_full_box_struct()
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}

	version = boxBuffer[usedBytesLength];
	for (int idx = 0; idx < 3; idx++)
	{
		flags += (boxBuffer[usedBytesLength + idx] << (8 * idx));
	}

	usedBytesLength += 4;

	return kMP4ParserError_NoError;
}

int FileTypeBox::Get_file_type_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}

	majorBrand = Get_lsb_uint32_value(boxBuffer, usedBytesLength);	
	minorVersion = Get_lsb_uint32_value(boxBuffer, usedBytesLength);	

	cbNum = (size - usedBytesLength) / sizeof(UINT32);
	if (cbNum > 0)
	{
		compatibleBrands = new UINT32[cbNum];
		for (int idx = 0; idx < cbNum; idx ++)
		{
			compatibleBrands[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);			
		}
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void FileTypeBox::dump_file_type_box()
{
	char forcc[5] = { 0 };

	Dump_box_info();

	UINT32_to_fourcc(majorBrand, forcc);
	cout << "Major Brand: " << forcc << endl;
	cout << "Minor Version: " << to_string(minorVersion) << endl;

	cout << "Compatible Brands: ";
	for (int idx = 0; idx < cbNum; idx++)
	{
		UINT32_to_fourcc(compatibleBrands[idx], forcc);
		cout << forcc << " ";
	}
	cout << endl;

#if DUMP_MP4_INFO_ENABLED_LOG
	UINT32_to_fourcc(majorBrand, forcc);
	g_logoutFile << "Major Brand: " << forcc << endl;
	g_logoutFile << "Minor Version: " << to_string(minorVersion) << endl;

	g_logoutFile << "Compatible Brands: ";
	for (int idx = 0; idx < cbNum; idx++)
	{
		UINT32_to_fourcc(compatibleBrands[idx], forcc);
		g_logoutFile << forcc << " ";
	}
	g_logoutFile << endl;
#endif
}

int MovieBox::Get_movie_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	if (g_mp4Config.mp4Configlevel > 1)
	{
		g_sampleInfoFile.open("samples.txt");
		if (!g_sampleInfoFile.is_open())
		{
			return kMP4ParserError_OpenOutputFileFailed;
		}
	}

	// Parse mvhd box...
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "mvhd"))
	{
		mvhdBox = new MovieHeaderBox(boxBuffer + usedBytesLength);
		mvhdBox->Get_movie_header(usedBytesLength);
		mvhdBox->Dump_movie_header_info();
	}

	while (usedBytesLength < size)
	{
		// Parse trak box...
		if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "trak"))
		{
			trakBox = new TrackBox(boxBuffer + usedBytesLength);
			trakBox->Get_track(usedBytesLength);
		}

		// Parse udta box...
		if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "udta"))
		{
			udtaBox = new UserDataBox(boxBuffer + usedBytesLength);
			udtaBox->Get_user_data_box(usedBytesLength);
		}
	}

	if (g_mp4Config.mp4Configlevel > 1)
	{
		if (g_sampleInfoFile.is_open())
		{
			g_sampleInfoFile.close();
		}
	}
	bytePosition += size;
	return kMP4ParserError_NoError;
}

int UserDataBox::Get_user_data_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	bytePosition += size;
	return kMP4ParserError_NoError;
}

int MovieHeaderBox::Get_movie_header(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	if (version == 1)
	{
		creationTime = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		modificationTime = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		timeScale = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		duration = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
	} 
	else
	{
		creationTime = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		modificationTime = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		timeScale = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		duration = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	rate = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	volume = Get_lsb_uint32_value(boxBuffer, usedBytesLength) >> 16;
	usedBytesLength += 8;

	for (int idx = 0; idx < 9; idx++)
	{
		matrix[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}
	usedBytesLength += 24;

	nextTrackID = Get_lsb_uint32_value(boxBuffer, usedBytesLength);

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void MovieHeaderBox::Dump_movie_header_info()
{
	cout << "creationTime: " << to_string(creationTime) << endl;
	cout << "modificationTime: " << to_string(modificationTime) << endl;
	cout << "timeScale: " << to_string(timeScale) << endl;
	cout << "duration: " << to_string(duration) << endl;
	cout << "rate: " << to_string(rate) << endl;
	cout << "volume: " << to_string(volume) << endl;
	cout << "matrix: ";
	for (int idx = 0; idx < 9; idx++)
	{
		cout << to_string(matrix[idx]) << " ";
	}
	cout << endl;
	cout << "nextTrackID: " << to_string(nextTrackID) << endl;

#if DUMP_MP4_INFO_ENABLED_LOG

	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "creationTime: " << to_string(creationTime) << endl;
		g_logoutFile << "modificationTime: " << to_string(modificationTime) << endl;
		g_logoutFile << "timeScale: " << to_string(timeScale) << endl;
		g_logoutFile << "duration: " << to_string(duration) << endl;
		g_logoutFile << "rate: " << to_string(rate) << endl;
		g_logoutFile << "volume: " << to_string(volume) << endl;
		g_logoutFile << "matrix: ";
		for (int idx = 0; idx < 9; idx++)
		{
			g_logoutFile << to_string(matrix[idx]) << " ";
		}
		g_logoutFile << endl;
		g_logoutFile << "nextTrackID: " << to_string(nextTrackID) << endl;
	}
	
#endif
}

// mdhd box...
int MediaHeaderBox::Get_media_header(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	if (version == 1)
	{
		creationTime = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		modificationTime = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		timeScale = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		duration = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
	}
	else
	{
		creationTime = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		modificationTime = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		timeScale = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		duration = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}
	languageCode = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	languageCode = (languageCode >> 16) & 0x7fff;

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void MediaHeaderBox::Dump_media_header_info()
{
	cout << "creationTime: " << to_string(creationTime) << endl;
	cout << "modificationTime: " << to_string(modificationTime) << endl;
	cout << "timeScale: " << to_string(timeScale) << endl;
	cout << "duration: " << to_string(duration) << endl;
	cout << "language: " << to_string(languageCode) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "creationTime: " << to_string(creationTime) << endl;
		g_logoutFile << "modificationTime: " << to_string(modificationTime) << endl;
		g_logoutFile << "timeScale: " << to_string(timeScale) << endl;
		g_logoutFile << "duration: " << to_string(duration) << endl;
		g_logoutFile << "language: " << to_string(languageCode) << endl;
	}
#endif
}

int Handlerbox::Get_handler_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	usedBytesLength += 4;
	handlerType = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	usedBytesLength += 12;

	name = new char[size - usedBytesLength];
	memcpy(name, boxBuffer + usedBytesLength, size - usedBytesLength);

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void Handlerbox::Dump_handler_box_info()
{
	char forcc[5] = { 0 };
	UINT32_to_fourcc(handlerType, forcc);

	cout << "handler_type: " << forcc << endl;
	cout << "name: " << name << endl;

#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "handler_type: " << forcc << endl;
		g_logoutFile << "name: " << name << endl;
	}
#endif
}

// vmhd box...
int VideoMediaHeaderBox::Get_video_media_header(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void VideoMediaHeaderBox::Dump_video_media_header_info()
{
	cout << "graphicsmode: " << to_string(graphicsmode) << endl;
	cout << "opcolor: " << to_string(opcolor[0]) << " " << to_string(opcolor[1]) << " " << to_string(opcolor[2]) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "graphicsmode: " << to_string(graphicsmode) << endl;
		g_logoutFile << "opcolor: " << to_string(opcolor[0]) << " " << to_string(opcolor[1]) << " " << to_string(opcolor[2]) << endl;
	}
#endif
}

// smhd
int SoundMediaHeaderBox::Get_sound_media_header(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void SoundMediaHeaderBox::Dump_audio_media_header_info()
{
	cout << "balance: " << to_string(balance) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "balance: " << to_string(balance) << endl;
	}
#endif
}

// dref
int DataReferenceBox::Get_data_ref(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void DataReferenceBox::Dump_data_ref_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
	}
#endif
}

// dinf
int DataInfoBox::Get_data_info_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "dref"))
	{
		drefBox = new DataReferenceBox(boxBuffer + usedBytesLength);
		drefBox->Get_data_ref(usedBytesLength);
		drefBox->Dump_data_ref_info();
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

int SampleDescriptionBox::Get_sample_description_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void SampleDescriptionBox::Dump_sample_description_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
	}
#endif
}

int TimeToSampleBox::Get_time_to_sample_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	pSampleCount = new UINT32[entryCount];
	pSampleDelta = new UINT32[entryCount];

	for (int idx = 0; idx < entryCount; idx++)
	{
		pSampleCount[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		pSampleDelta[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void TimeToSampleBox::Dump_time_to_sample_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
		for (int idx = 0; idx < entryCount; idx++)
		{
			g_logoutFile << to_string(idx) << "\t";
			g_logoutFile << "sample_count: " << to_string(pSampleCount[idx]) << "	sample_delta: " << to_string(pSampleDelta[idx]) << endl;
		}
	}
#endif
}

int SyncSampleBox::Get_sync_sample_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	pSampleNumber = new UINT32[entryCount];
	for (int idx = 0; idx < entryCount; idx++)
	{
		pSampleNumber[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void SyncSampleBox::Dump_sync_sample_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
		for (int idx = 0; idx < entryCount; idx++)
		{
			g_logoutFile << to_string(idx) << "\t";
			g_logoutFile << "sample_number: " << to_string(pSampleNumber[idx]) << endl;
		}
	}
#endif
}

int CompositionOffsetBox::Get_composition_offset_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	pSampleCount = new UINT32[entryCount];
	pSampleOffset = new UINT32[entryCount];
	for (int idx = 0; idx < entryCount; idx++)
	{
		pSampleCount[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		pSampleOffset[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void CompositionOffsetBox::Dump_composition_offset_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
		for (int idx = 0; idx < entryCount; idx++)
		{
			g_logoutFile << to_string(idx) << "\t";
			g_logoutFile << "sample_count: " << to_string(pSampleCount[idx]) << "	sample_offset: " << to_string(pSampleOffset[idx]) << endl;
		}
	}
#endif
}

int SampleToChunkBox::Get_sample_to_chunk_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	pFirstChunk = new UINT32[entryCount];
	pSamplesPerChunk = new UINT32[entryCount];
	pSampleDiscriptionIdx = new UINT32[entryCount];

	for (int idx = 0; idx < entryCount; idx++)
	{
		pFirstChunk[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		pSamplesPerChunk[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		pSampleDiscriptionIdx[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void SampleToChunkBox::Dump_sample_to_chunk_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
		for (int idx = 0; idx < entryCount; idx++)
		{
			g_logoutFile << to_string(idx) << "\t";
			g_logoutFile << "first_chunk: " << to_string(pFirstChunk[idx]) << "	samples_per_chunk: " << to_string(pSamplesPerChunk[idx]) << "	sample_description_index: " << to_string(pSampleDiscriptionIdx[idx]) << endl;
		}
	}
#endif
}

int SampleSizeBox::Get_sample_size_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	sampleSize = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	sampleCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);

	if (sampleSize == 0)
	{
		pEntrySize = new UINT32[sampleCount];
		for (int idx = 0; idx < sampleCount; idx++)
		{
			pEntrySize[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		}
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void SampleSizeBox::Dump_sample_size_info()
{
	cout << "sample_size: " << to_string(sampleSize) << endl;
	cout << "sample_count: " << to_string(sampleCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "sample_size: " << to_string(sampleSize) << endl;
		g_logoutFile << "sample_count: " << to_string(sampleCount) << endl;
		if (sampleSize == 0)
		{
			for (int idx = 0; idx < sampleCount; idx++)
			{
				g_logoutFile << to_string(idx) << "\t";
				g_logoutFile << "entry_size: " << to_string(pEntrySize[idx]) << endl;
			}
		}
	}
#endif
}

int ChunkOffsetBox::Get_chunk_offset_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCount = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	pChunkOffset = new UINT32[entryCount];
	for (int idx = 0; idx < entryCount; idx++)
	{
		pChunkOffset[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void ChunkOffsetBox::Dump_chunk_offset_info()
{
	cout << "entry_count: " << to_string(entryCount) << endl;
#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCount) << endl;
		for (int idx = 0; idx < entryCount; idx++)
		{
			g_logoutFile << to_string(idx) << "\t";
			g_logoutFile << "chunk_offset: " << to_string(pChunkOffset[idx]) << endl;
		}
	}
#endif
}

int SampleTableBox::Get_sample_table(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();
		
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stsd"))
	{
		stsdBox = new SampleDescriptionBox(boxBuffer + usedBytesLength);
		stsdBox->Get_sample_description_box(usedBytesLength);
		stsdBox->Dump_sample_description_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stts"))
	{
		sttsBox = new TimeToSampleBox(boxBuffer + usedBytesLength);
		sttsBox->Get_time_to_sample_box(usedBytesLength);
		sttsBox->Dump_time_to_sample_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stss"))
	{
		stssBox = new SyncSampleBox(boxBuffer + usedBytesLength);
		stssBox->Get_sync_sample_box(usedBytesLength);
		stssBox->Dump_sync_sample_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "ctts"))
	{
		cttsBox = new CompositionOffsetBox(boxBuffer + usedBytesLength);
		cttsBox->Get_composition_offset_box(usedBytesLength);
		cttsBox->Dump_composition_offset_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stsc"))
	{
		stscBox = new SampleToChunkBox(boxBuffer + usedBytesLength);
		stscBox->Get_sample_to_chunk_box(usedBytesLength);
		stscBox->Dump_sample_to_chunk_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stsz"))
	{
		stszBox = new SampleSizeBox(boxBuffer + usedBytesLength);
		stszBox->Get_sample_size_box(usedBytesLength);
		stszBox->Dump_sample_size_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stco"))
	{
		stcoBox = new ChunkOffsetBox(boxBuffer + usedBytesLength);
		stcoBox->Get_chunk_offset_box(usedBytesLength);
		stcoBox->Dump_chunk_offset_info();
	}

	if (g_mp4Config.mp4Configlevel > 1)
	{
		Calculate_sample_info();
	}
	bytePosition += size;
	return kMP4ParserError_NoError;
}

void SampleTableBox::Calculate_sample_info()
{
	UINT32 sampleCount = stszBox->sampleCount;
		
	sampleInfoArr = new SampleInfo[sampleCount];
	memset(sampleInfoArr, 0, sampleCount * sizeof(SampleInfo));
	
	if (stszBox)
	{
		// sample size...
		for (int idx = 0; idx < sampleCount; idx++)
		{
			sampleInfoArr[idx].sampleIndex = idx;
			sampleInfoArr[idx].sampleSize = stszBox->pEntrySize[idx];
		}
	}	

	// sample decoding time...
	if (sttsBox)
	{
		int totalSampleCnt = 0;
		for (int idx = 0; idx < sttsBox->entryCount; idx++)
		{
			int sub_idx = 0;
			for (; sub_idx < sttsBox->pSampleCount[idx]; sub_idx++)
			{
				if (totalSampleCnt == 0 && sub_idx == 0)
				{
					sampleInfoArr[0].decodingTime = 0;
				}
				else
				{
					sampleInfoArr[totalSampleCnt + sub_idx].decodingTime = sttsBox->pSampleDelta[idx] + sampleInfoArr[totalSampleCnt + sub_idx - 1].decodingTime;
				}
			}
			totalSampleCnt += sub_idx;
		}
	}

	if (stssBox)
	{
		// key frame...
		for (int idx = 0; idx < stssBox->entryCount; idx++)
		{
			sampleInfoArr[stssBox->pSampleNumber[idx]].keyFrame = true;
		}
	}

	if (cttsBox)
	{
		// composition time...
		int totalSampleCnt = 0;
		for (int idx = 0; idx < cttsBox->entryCount; idx++)
		{
			int sub_idx = 0;
			for (; sub_idx < cttsBox->pSampleCount[idx]; sub_idx++)
			{
				sampleInfoArr[totalSampleCnt + sub_idx].compositionTime = cttsBox->pSampleOffset[idx];
			}
			totalSampleCnt += sub_idx;
		}
	}

	// chunk index & offset...
	if (stscBox && stcoBox)
	{
		UINT32 *chunkNumbers = new UINT32[stscBox->entryCount];
		memset(chunkNumbers, 0, sizeof (UINT32)* stscBox->entryCount);
		UINT32 totalChunkIdx = 0, totalSampleIdx = 0, chunkFileOffset = 0;

		int idx = 0;
		for (; idx < stscBox->entryCount; idx++)
		{
			if (idx == stscBox->entryCount - 1)
			{
				chunkNumbers[idx] = stcoBox->entryCount - stscBox->pFirstChunk[idx - 1];
			}
			else
			{
				chunkNumbers[idx] = stscBox->pFirstChunk[idx + 1] - stscBox->pFirstChunk[idx];
			}

			for (int chunk_idx = 0; chunk_idx < chunkNumbers[idx]; chunk_idx++)
			{
				chunkFileOffset = stcoBox->pChunkOffset[totalChunkIdx + chunk_idx];
				UINT32 offsetInChunk = 0;

				for (int sample_idx = 0; sample_idx < stscBox->pSamplesPerChunk[idx]; sample_idx++)
				{
					sampleInfoArr[sample_idx + totalSampleIdx].chunkIndex = totalChunkIdx + chunk_idx;
					sampleInfoArr[sample_idx + totalSampleIdx].fileBitOffset = chunkFileOffset + offsetInChunk;
					offsetInChunk += sampleInfoArr[sample_idx + totalSampleIdx].sampleSize;
				}
				totalSampleIdx += stscBox->pSamplesPerChunk[idx];
			}

			totalChunkIdx += chunkNumbers[idx];
		}

		delete[] chunkNumbers;
	}

	// log out...
	{
		g_sampleInfoFile << "********************Sample Table*******************" << endl;

		for (int idx = 0; idx < sampleCount; idx++)
		{
			g_sampleInfoFile << "Sample " << to_string(sampleInfoArr[idx].sampleIndex) << "";
			g_sampleInfoFile << ";\t Sample Size: " << to_string(sampleInfoArr[idx].sampleSize);
			g_sampleInfoFile << ";\t Chunk Index: " << to_string(sampleInfoArr[idx].chunkIndex);
			g_sampleInfoFile << ";\t Key Frame: " << to_string(sampleInfoArr[idx].keyFrame);
			g_sampleInfoFile << ";\t Decoding Time: " << to_string(sampleInfoArr[idx].decodingTime);
			g_sampleInfoFile << ";\t Composition Time: " << to_string(sampleInfoArr[idx].compositionTime);
			g_sampleInfoFile << ";\t File offset: " << to_string(sampleInfoArr[idx].fileBitOffset);
			g_sampleInfoFile << endl;
		}
	}
}

// minf box...
int MediaInfoBox::Get_media_info_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "vmhd"))
	{
		vmhdBox = new VideoMediaHeaderBox(boxBuffer + usedBytesLength);
		vmhdBox->Get_video_media_header(usedBytesLength);
		vmhdBox->Dump_video_media_header_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "smhd"))
	{
		smhdBox = new SoundMediaHeaderBox(boxBuffer + usedBytesLength);
		smhdBox->Get_sound_media_header(usedBytesLength);
		smhdBox->Dump_audio_media_header_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "dinf"))
	{
		dinfBox = new DataInfoBox(boxBuffer + usedBytesLength);
		dinfBox->Get_data_info_box(usedBytesLength);
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "stbl"))
	{
		stblBox = new SampleTableBox(boxBuffer + usedBytesLength);
		stblBox->Get_sample_table(usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

int MediaBox::Get_media(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "mdhd"))
	{
		mdhdBox = new MediaHeaderBox(boxBuffer + usedBytesLength);
		mdhdBox->Get_media_header(usedBytesLength);
		mdhdBox->Dump_media_header_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "hdlr"))
	{
		hdlrBox = new Handlerbox(boxBuffer + usedBytesLength);
		hdlrBox->Get_handler_box(usedBytesLength);
		hdlrBox->Dump_handler_box_info();
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "minf"))
	{
		minfBox = new MediaInfoBox(boxBuffer + usedBytesLength);
		minfBox->Get_media_info_box(usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

int TrackBox::Get_track(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	// Parse tkhd...
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "tkhd"))
	{
		tkhdBox = new TrackHeaderBox(boxBuffer + usedBytesLength);
		tkhdBox->Get_track_header(usedBytesLength);
		tkhdBox->Dump_track_header_info();
	}

	// Parse edts...
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "edts"))
	{
		edtsBox = new EditBox(boxBuffer + usedBytesLength);
		edtsBox->Get_edit_box(usedBytesLength);
	}

	// Parse mdia
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "mdia"))
	{
		mdiaBox = new MediaBox(boxBuffer + usedBytesLength);
		mdiaBox->Get_media(usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

int TrackHeaderBox::Get_track_header(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}

	if (version == 1)
	{
		creationTime = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		modificationTime = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		trackID = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		usedBytesLength += 4;
		duration = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
	}
	else
	{
		creationTime = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		modificationTime = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		trackID = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		usedBytesLength += 4;
		duration = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}
	usedBytesLength += 8;

	layer = Get_lsb_uint16_value(boxBuffer, usedBytesLength);
	alternateGroup = Get_lsb_uint16_value(boxBuffer, usedBytesLength);
	volume = Get_lsb_uint16_value(boxBuffer, usedBytesLength);
	usedBytesLength += 2;

	for (int idx = 0; idx < 9; idx++)
	{
		matrix[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
	}

	width = Get_lsb_uint32_value(boxBuffer, usedBytesLength) >> 16;
	height = Get_lsb_uint32_value(boxBuffer, usedBytesLength) >> 16;

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void TrackHeaderBox::Dump_track_header_info()
{
	Dump_full_box_info();

	cout << "creationTime: " << to_string(creationTime) << endl;
	cout << "modificationTime: " << to_string(modificationTime) << endl;
	cout << "trackID: " << to_string(trackID) << endl;
	cout << "duration: " << to_string(duration) << endl;
	cout << "layer: " << to_string(layer) << endl;
	cout << "alternateGroup: " << to_string(alternateGroup) << endl;
	cout << "volume: " << to_string(volume) << endl;
	cout << "matrix: ";
	for (int idx = 0; idx < 9; idx++)
	{
		cout << to_string(matrix[idx]) << " ";
	}
	cout << endl;
	cout << "width: " << to_string(width) << endl;
	cout << "height: " << to_string(height) << endl;

#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "creationTime: " << to_string(creationTime) << endl;
		g_logoutFile << "modificationTime: " << to_string(modificationTime) << endl;
		g_logoutFile << "trackID: " << to_string(trackID) << endl;
		g_logoutFile << "duration: " << to_string(duration) << endl;
		g_logoutFile << "layer: " << to_string(layer) << endl;
		g_logoutFile << "alternateGroup: " << to_string(alternateGroup) << endl;
		g_logoutFile << "volume: " << to_string(volume) << endl;
		g_logoutFile << "matrix: ";
		for (int idx = 0; idx < 9; idx++)
		{
			g_logoutFile << to_string(matrix[idx]) << " ";
		}
		g_logoutFile << endl;
		g_logoutFile << "width: " << to_string(width) << endl;
		g_logoutFile << "height: " << to_string(height) << endl;
	}
#endif
}

int EditBox::Get_edit_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	// Parse elst...
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "elst"))
	{
		elstBox = new EditListBox(boxBuffer + usedBytesLength);
		elstBox->Get_edit_list_box(usedBytesLength);
		elstBox->Dump_edit_list_info();
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

int EditListBox::Get_edit_list_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_full_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_full_box_info();

	entryCnt = Get_lsb_uint32_value(boxBuffer, usedBytesLength);

	segmentDuration = new UINT64[entryCnt];
	mediaTime = new INT64[entryCnt];
	mediaRateInteger = new short[entryCnt];
	mediaRateFraction = new short[entryCnt];

	for (int idx = 0; idx < entryCnt; idx++)
	{
		if (version == 1)
		{
			segmentDuration[idx] = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
			mediaTime[idx] = Get_lsb_uint64_value(boxBuffer, usedBytesLength);
		} 
		else
		{
			segmentDuration[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
			mediaTime[idx] = Get_lsb_uint32_value(boxBuffer, usedBytesLength);
		}

		mediaRateInteger[idx] = Get_lsb_uint16_value(boxBuffer, usedBytesLength);
		mediaRateFraction[idx] = Get_lsb_uint16_value(boxBuffer, usedBytesLength);
	}

	bytePosition += size;
	return kMP4ParserError_NoError;
}

void EditListBox::Dump_edit_list_info()
{
	cout << "entry_count: " << to_string(entryCnt) << endl;
	for (int idx = 0; idx < entryCnt; idx++)
	{
		cout << "segment_duration " << to_string(idx) << ": " << to_string(segmentDuration[idx]) << endl;
		cout << "media_time " << to_string(idx) << ": " << to_string(mediaTime[idx]) << endl;
		cout << "media_rate_integer " << to_string(idx) << ": " << to_string(mediaRateInteger[idx]) << endl;
		cout << "media_rate_fraction " << to_string(idx) << ": " << to_string(mediaRateFraction[idx]) << endl;
	}

#if DUMP_MP4_INFO_ENABLED_LOG
	if (g_mp4Config.mp4Configlevel > 0)
	{
		g_logoutFile << "entry_count: " << to_string(entryCnt) << endl;
		for (int idx = 0; idx < entryCnt; idx++)
		{
			g_logoutFile << "segment_duration " << to_string(idx) << ": " << to_string(segmentDuration[idx]) << endl;
			g_logoutFile << "media_time " << to_string(idx) << ": " << to_string(mediaTime[idx]) << endl;
			g_logoutFile << "media_rate_integer " << to_string(idx) << ": " << to_string(mediaRateInteger[idx]) << endl;
			g_logoutFile << "media_rate_fraction " << to_string(idx) << ": " << to_string(mediaRateFraction[idx]) << endl;
		}
	}
#endif
}


int MediaDataBox::Get_mdat_box(UINT64 &bytePosition)
{
	int err = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}
	Dump_box_info();

	bytePosition += size;
	return kMP4ParserError_NoError;
}
