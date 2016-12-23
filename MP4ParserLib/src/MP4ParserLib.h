#ifdef _MSC_VER
#ifdef MP4PARSERLIB_EXPORTS
#define MP4PARSERLIB_API __declspec(dllexport)
#else
#define MP4PARSERLIB_API __declspec(dllimport)
#endif
#endif

#include "Include.h"

class MP4PARSERLIB_API CMP4Parser
{
public:
	CMP4Parser(const char *inputMP4FileName);
	~CMP4Parser();

	int Parse();

private:
	const char    *m_fileName;		// input file name...
	UINT64        m_fileSize;		// input file size in bytes...
	BYTE          *m_fileBuf;		// buffer pointer of input file...
	UINT64        m_bytesCnt;		// bytes counter processed...
};