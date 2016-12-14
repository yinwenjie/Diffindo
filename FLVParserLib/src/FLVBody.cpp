#include "FLVParserLib.h"
#include "FLVBody.h"

CFlvBody::CFlvBody()
{
	m_tagCount = 0;
	m_videoTagCount = 0;
	m_audiotagCount = 0;
}

CFlvBody::~CFlvBody()
{

}

int CFlvBody::Parse(BYTE *FileBuf, UINT64 &byteCnt, UINT64 fileSize)
{
	int err = 0;

	while (byteCnt < fileSize)
	{
		UINT32 prevTagSize = 0;
		Read_data_lsb(&prevTagSize, FileBuf + byteCnt, 4);
		byteCnt += sizeof(UINT32);

		if ((prevTagSize != 0) && (m_tagCount == 0))
		{
			return kFlvParserError_FirstPrevTagSizeNotZero;
		}
		if (byteCnt >= fileSize)
		{
			break;
		}


	}

	return kFlvParserError_NoError;
}
