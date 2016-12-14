
#ifdef _MSC_VER
#ifdef FLVPARSERLIB_EXPORTS
#define FLVPARSERLIB_API __declspec(dllexport)
#else
#define FLVPARSERLIB_API __declspec(dllimport)
#endif
#endif

#ifdef __GNUC__
#define FLVPARSERLIB_API  
#endif

#include "Include.h"

// Flv tag types definition...
#define TAG_TYPE_AUDIO 8
#define TAG_TYPE_VIDEO 9
#define TAG_TYPE_SCRIPT 18

// Flv parsing error codes...
const int kFlvParserError_NoError = 0;
const int kFlvParserError_NullFileName = -1;
const int kFlvParserError_OpenInputFileFailed = -2;
const int kFlvParserError_ReadInputFileFailed = -3;

class FLVPARSERLIB_API CFlvParser
{
public:
	CFlvParser(const char *fileName);
	~CFlvParser();

	// Parse FLV file...
	int Parse();

private:
	const char    *m_fileName;		// input file name...
	UINT64        m_fileSize;		// input file size in bytes...
	BYTE          *m_fileBuf;		// buffer pointer of input file...
	UINT64        m_bytesCnt;		// bytes counter processed...

	std::ifstream m_inFileStream;	// input file stream...
	int		open_input_file();		// open input file by file name...
	int     get_input_file_data();	// read input file data to buffer...
	void	close_input_file();		// close input file...
	void	dump_flv_file_info();	// dump global info of flv file...
};