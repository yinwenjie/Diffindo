#include "MP4ParserLib.h"
#include "Box.h"

using namespace std;

int Box::Get_box_struct()
{
	size = Get_lsb_uint32_value(boxBuffer);
	boxType = Get_lsb_uint32_value(boxBuffer + 4);
	usedBytesLength += 8;
	if (size == 1)
	{
		largeSize = Get_lsb_uint64_value(boxBuffer + 8);
		usedBytesLength += 8;
	}

	// 'uuid' not supported.. 
	
	return kMP4ParserError_NoError;
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

	majorBrand = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
	usedBytesLength += 4;
	minorVersion = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
	usedBytesLength += 4;

	cbNum = (size - usedBytesLength) / sizeof(UINT32);
	if (cbNum > 0)
	{
		compatibleBrands = new UINT32[cbNum];
		for (int idx = 0; idx < cbNum; idx ++)
		{
			compatibleBrands[idx] = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
			usedBytesLength += 4;
		}
	}

	bytePosition += usedBytesLength;
	return kMP4ParserError_NoError;
}

void FileTypeBox::dump_file_type_box()
{
	char forcc[5] = { 0 };

	cout << "--------------------------------------------" << endl;
	cout << "Box Size: " << to_string(size) << endl;
	UINT32_to_fourcc(boxType, forcc);
	cout << "Box Type: " << forcc << endl;
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
	g_logoutFile << "--------------------------------------------" << endl;
	g_logoutFile << "Box Size: " << to_string(size) << endl;
	UINT32_to_fourcc(boxType, forcc);
	g_logoutFile << "Box Type: " << forcc << endl;
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
	UINT64 subBoxSize = 0;
	err = Get_box_struct();
	if (err < 0)
	{
		return err;
	}

	if (Fourcc_compare(boxBuffer + usedBytesLength + 4, "mvhd"))
	{
		mvhdBox = new MovieHeaderBox(boxBuffer + usedBytesLength);
		mvhdBox->Get_movie_header(usedBytesLength);
		mvhdBox->Dump_movie_header_info();
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
		creationTime = Get_lsb_uint64_value(boxBuffer + usedBytesLength);
		usedBytesLength += 8;
		modificationTime = Get_lsb_uint64_value(boxBuffer + usedBytesLength);
		usedBytesLength += 8;
		timeScale = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
		usedBytesLength += 4;
		duration = Get_lsb_uint64_value(boxBuffer + usedBytesLength);
		usedBytesLength += 8;
	} 
	else
	{
		creationTime = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
		usedBytesLength += 4;
		modificationTime = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
		usedBytesLength += 4;
		timeScale = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
		usedBytesLength += 4;
		duration = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
		usedBytesLength += 4;
	}

	rate = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
	usedBytesLength += 4;
	volume = Get_lsb_uint32_value(boxBuffer + usedBytesLength) >> 16;
	usedBytesLength += 12;

	for (int idx = 0; idx < 9; idx++)
	{
		matrix[idx] = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
		usedBytesLength += 4;
	}
	usedBytesLength += 24;

	nextTrackID = Get_lsb_uint32_value(boxBuffer + usedBytesLength);
	usedBytesLength += 4;

	bytePosition += usedBytesLength;
	return kMP4ParserError_NoError;
}

void MovieHeaderBox::Dump_movie_header_info()
{
	char forcc[5] = { 0 };

	cout << "--------------------------------------------" << endl;
	cout << "Box Size: " << to_string(size) << endl;
	UINT32_to_fourcc(boxType, forcc);
	cout << "Box Type: " << forcc << endl;
	cout << "Version: " << to_string(version) << endl;
	cout << "Flags: " << to_string(flags) << endl;
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
	g_logoutFile << "--------------------------------------------" << endl;
	g_logoutFile << "Box Size: " << to_string(size) << endl;
	UINT32_to_fourcc(boxType, forcc);
	g_logoutFile << "Box Type: " << forcc << endl;
	g_logoutFile << "Version: " << to_string(version) << endl;
	g_logoutFile << "Flags: " << to_string(flags) << endl;
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
