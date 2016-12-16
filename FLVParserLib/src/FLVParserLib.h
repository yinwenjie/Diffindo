
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
const int kFlvParserError_IllegalFlvHeader = -4;
const int kFlvParserError_FirstPrevTagSizeNotZero = -5;
const int kFlvParserError_StreamIDNotZero = -6;
const int kFlvParserError_IllegalTagType = -7;
const int kFlvParserError_IllegalVideoFrameType = -8;
const int kFlvParserError_UnsupportedVideoCodecID = -9;
const int kFlvParserError_MultipleSPSPPSinVideoHeader = -10;
const int kFlvParserError_UnsupportedNALUnitLengthSize = -11;
const int kFlvParserError_EmptyNALUnit = -12;

typedef struct FlvHeader FlvHeader;
class CFlvBody;
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

	FlvHeader *m_flvHeader;			// Flv header instance...
	int     create_flv_header();	// create flv header from input data
	void	dump_flv_header_info();	// dump flv header info

	CFlvBody *m_flvBody;			// Flv body instance...
	int     create_flv_body();		// create flv body from input data

};