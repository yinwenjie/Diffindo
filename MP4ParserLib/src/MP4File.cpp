#include "MP4ParserLib.h"
#include "MP4File.h"

using namespace std;

CMP4File::CMP4File(BYTE *fileBuffer)
{
	m_fileBuffer = fileBuffer;
	m_fileBytePosition = 0;
	ftypBox = NULL;
	moovBox = NULL;
}

CMP4File::~CMP4File()
{
	if (ftypBox)
	{
		delete ftypBox;
		ftypBox = NULL;
	}
}

int CMP4File::Parse()
{
	int err = get_file_type_box();
	if (err < 0 )	
	{
		return err;
	}

	err = get_movie_box();
	if (err < 0)
	{
		return err;
	}

	return kMP4ParserError_NoError;
}

int CMP4File::get_file_type_box()
{
	int err = 0;
	ftypBox = new FileTypeBox(m_fileBuffer);
	err = ftypBox->Get_file_type_box(m_fileBytePosition);
	if (err < 0)
	{
		return err;
	}
	ftypBox->dump_file_type_box();

	return kMP4ParserError_NoError;
}

int CMP4File::get_movie_box()
{
	if (Fourcc_compare(m_fileBuffer + m_fileBytePosition + 4, "moov"))
	{
		moovBox = new MovieBox(m_fileBuffer + m_fileBytePosition);
		int err = moovBox->Get_movie_box(m_fileBytePosition);
		if (err < 0)
		{
			return err;
		}
	}

	return kMP4ParserError_NoError;
}
