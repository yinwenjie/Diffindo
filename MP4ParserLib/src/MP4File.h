#ifndef _MP4_FILE_H_
#define _MP4_FILE_H_

#include "DataTypes.h"
#include "Box.h"

class CMP4File
{
public:
	CMP4File(BYTE *fileBuffer);
	~CMP4File();

	FileTypeBox *ftypBox;
	MovieBox    *moovBox;
	MediaDataBox *mdatBox;

	int Parse();

private:
	BYTE  *m_fileBuffer;
	UINT64 m_fileBytePosition;

	int   get_file_type_box();
	int   get_movie_box();
	int   get_mdat_box();
};


#endif