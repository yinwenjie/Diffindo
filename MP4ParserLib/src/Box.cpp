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
	UINT32_to_fourcc(boxType, forcc);
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

	bytePosition += usedBytesLength;
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

	// Parse mvhd box...
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "mvhd"))
	{
		mvhdBox = new MovieHeaderBox(boxBuffer + usedBytesLength);
		mvhdBox->Get_movie_header(usedBytesLength);
		mvhdBox->Dump_movie_header_info();
	}

	// Parse trak box...
	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "trak"))
	{
		trakBox = new TrackBox(boxBuffer + usedBytesLength);
		trakBox->Get_track(usedBytesLength);
	}

	bytePosition += usedBytesLength;
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

	bytePosition += usedBytesLength;
	return kMP4ParserError_NoError;
}

void MovieHeaderBox::Dump_movie_header_info()
{
	Dump_full_box_info();

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
#endif
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

	bytePosition += usedBytesLength;
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

	bytePosition += usedBytesLength;
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

	bytePosition += usedBytesLength;
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

	bytePosition += usedBytesLength;
	return kMP4ParserError_NoError;
}

void EditListBox::Dump_edit_list_info()
{
	Dump_box_info();

	cout << "entry_count: " << to_string(entryCnt) << endl;
	for (int idx = 0; idx < entryCnt; idx++)
	{
		cout << "segment_duration " << to_string(idx) << ": " << to_string(segmentDuration[idx]) << endl;
		cout << "media_time " << to_string(idx) << ": " << to_string(mediaTime[idx]) << endl;
		cout << "media_rate_integer " << to_string(idx) << ": " << to_string(mediaRateInteger[idx]) << endl;
		cout << "media_rate_fraction " << to_string(idx) << ": " << to_string(mediaRateFraction[idx]) << endl;
	}

#if DUMP_MP4_INFO_ENABLED_LOG
	g_logoutFile << "entry_count: " << to_string(entryCnt) << endl;
	for (int idx = 0; idx < entryCnt; idx++)
	{
		g_logoutFile << "segment_duration " << to_string(idx) << ": " << to_string(segmentDuration[idx]) << endl;
		g_logoutFile << "media_time " << to_string(idx) << ": " << to_string(mediaTime[idx]) << endl;
		g_logoutFile << "media_rate_integer " << to_string(idx) << ": " << to_string(mediaRateInteger[idx]) << endl;
		g_logoutFile << "media_rate_fraction " << to_string(idx) << ": " << to_string(mediaRateFraction[idx]) << endl;
	}
#endif
}
