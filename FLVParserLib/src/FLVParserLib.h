#ifdef FLVPARSERLIB_EXPORTS
#define FLVPARSERLIB_API __declspec(dllexport)
#else
#define FLVPARSERLIB_API __declspec(dllimport)
#endif

#include "Include.h"

// Flv tag types definition...
#define TAG_TYPE_AUDIO 8
#define TAG_TYPE_VIDEO 9
#define TAG_TYPE_SCRIPT 18

class FLVPARSERLIB_API CFlvParser
{
public:
	CFlvParser(const char *fileName);
	~CFlvParser();

private:
	const char    *m_fileName;		// input file name...
	UINT64        m_fileSize;		// input file size in bytes...
	BYTE          *m_fileBuf;		// buffer pointer of input file...
	UINT64        m_bytesCnt;		// bytes counter processed...
};