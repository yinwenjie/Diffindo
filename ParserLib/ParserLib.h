// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PARSERLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PARSERLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PARSERLIB_EXPORTS
#define PARSERLIB_API __declspec(dllexport)
#else
#define PARSERLIB_API __declspec(dllimport)
#endif
#include <fstream>

// Data type...
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef unsigned long long UINT64;
typedef UINT8 BYTE;

// Flv parsing error codes...
const int kFlvParserError_NoError = 0;
const int kFlvParserError_NullFileName = -1;
const int kFlvParserError_OpenInputFileFailed = -2;
const int kFlvParserError_ReadInputFileFailed = -3;
const int kFlvParserError_IllegalFlvHeader = -4;
const int kFlvParserError_FirstPrevTagSizeNotZero = -5;
const int kFlvParserError_IllegalTagType = -6;
const int kFlvParserError_StreamIDNotZero = -7;
const int kFlvParserError_OpenOutputFileFailed = -8;
const int kFlvParserError_WriteOutputFileFailed = -9;
const int kFlvParserError_UnsupportedCodec = -10;
const int kFlvParserError_IllegalAVCDecoderConfigVersion = -11;
const int kFlvParserError_MultipleSequenceParamaterSets = -12;
const int kFlvParserError_MultiplePictureParamaterSets = -13;
const int kFlvParserError_NoSPSorPPSExists = -14;
const int kFlvParserError_IllegalNALUHeaderLen = -15;
const int kFlvParserError_MultipleSliceInFrame = -16;

// Flv parsing error description...
extern PARSERLIB_API const char* errorHints[17];

// Flv header structure
typedef struct _tFlvHeader
{
	UINT8  signature[3];
	UINT8  version;
	bool   videoFlag;
	bool   audioFlag;
	UINT32 dataOffset;
} FlvHeader;

class CFlvBody;

// Flv tag types definition...
#define TAG_TYPE_AUDIO 8
#define TAG_TYPE_VIDEO 9
#define TAG_TYPE_SCRIPT 18

// Flv tag structure...
class VideoTag;
class AudioTag;
class PARSERLIB_API CFlvTag
{
public:
	CFlvTag(UINT32 tagIdx, BYTE *FileBuf);
	~CFlvTag();

	// Parse bit stream to FlvTag object
	// Param: byteCnt - start position in memory buffer
	int Parse(UINT64 &byteCnt);

	// Parse detailed information of video or audio tag
	int Parse_detail();
	
	// Dump tag information to console or log file
	void Dump_tag_info();

	// Return tag type...
	UINT8 Get_tag_type();
	// Return tag buffer length...
	UINT64 Get_tag_total_len();
	// Return start position of this tag in data buffer...
	UINT64 Get_start_pos();
	// Return actual payload size of this tag...
	UINT32 Get_tag_dataSize();
	// Return timestamp of this tag...
	UINT32 Get_tag_timestamp_ext();
	// Return payload buffer pointer...
	const char * Get_tag_payload_buf();
	// Return total tag index...
	UINT32 Get_tag_index();
	// Modify time stamp of this tag
	void Set_tag_timestamp(UINT32 timestamp);

	// Pointer to next tag in the list
	CFlvTag* m_nextTag;
private:
	UINT32	m_tagIdx;				//total tag index
	BYTE*	m_fileBuf;				//file data buffer
	UINT64	m_tagStartPosition;		//start position of this tag
	UINT64	m_tagEndPosition;		//end position of this tag

	UINT8	m_tagType;				//current tag type - script data / audio / video
	UINT32	m_dataSize;				//payload data size of this tag
	UINT32	m_timeStamp;			//presenting time of this tag
	UINT8	m_timeStampExtension;	//timestamp extension
	UINT32	m_streamID;				//streamID, must be 0

	BYTE*	m_tagDataBuf;			//databuffer of data payload

	VideoTag *m_videoTag;			//point of VideoTag object
	AudioTag *m_audioTag;			//point of AudioTag object
};

// Flv parsing core class...
class PARSERLIB_API CFlvParser
{
public:
	CFlvParser(const char *fileName);
	~CFlvParser();

	// Parse flv file...
	int Parse();

	// Find the first tag of this flv...
	CFlvTag *Get_first_tag();

	// Return memory buffer of the parser...
	BYTE *Get_parser_buffer();

private:
	const char    *m_fileName;		// input file name...
	UINT64        m_fileSize;		// input file size in bytes...
	BYTE          *m_fileBuf;		// buffer pointer of input file...
	UINT64        m_bytesCnt;		// bytes counter processed...
	
	std::ifstream m_inFileStream;	// input stream object...
	int     open_input_file();		// open input file by file name...
	int     get_input_file_data();	// read input file data to buffer...
	void	dump_flv_file_info();	// dump global info of flv file...

	FlvHeader     *m_flvHeader;		// flv header object
	int     create_flv_header();	// create flv header from input data
	void	dump_flv_header_info();	// dump flv header info to log file or console

	CFlvBody	  *m_flvBody;		// flv body object
	int		create_flv_body();		// create flv body from input data
	void	dump_flv_tag_info();	// dump flv body info to log file or console
};


class CAVCDecoderConfigurationRecord;

// Video tag header structure
typedef struct VideoTagHeader
{
	UINT8 frameType;
	UINT8 codecID;
	UINT8 AVPacketType;
	int	  compositionTime;
} VideoTagHeader;

// Audio tag header structure
typedef struct AudioTagHeader
{
	UINT8 soundFormat;
	UINT8 soundRate;
	bool  soundSize;
	bool  soundType;
	UINT8 AACPacketType;
} AudioTagHeader;

// NAL Unit buffer structure
typedef struct NALUnitsBuffer
{
	BYTE *bufStart;
	int lengths;
} NALUnitsBuffer;

// NAL Unit structure
typedef struct NALUnit
{
	UINT8 nalUnitType;
	UINT8 sliceType;

	NALUnit *nextNalUnit;
} NALUnit;

// AVCDecoderConfigurationRecord structure
class CAVCDecoderConfigurationRecord
{
public:
	CAVCDecoderConfigurationRecord(BYTE *dataBuf);
	~CAVCDecoderConfigurationRecord();

	// Parse AVCDecoderConfigurationRecord from bit stream
	int Parse();
	// Dump AVCDecoderConfigurationRecord information to log file
	void Dump_decoder_configuration_record();

	BYTE* Get_sps_buf();	// return sps buffer pointer
	UINT16 Get_sps_len();	// return sps length
	BYTE* Get_pps_buf();	// return pps buffer pointer
	UINT16 Get_pps_len();	// return Pps length

private:
	BYTE *m_dataBuf;

	UINT8 m_configurationVersion;
	UINT8 m_AVCProfileIndication;
	UINT8 m_profile_compatibility;
	UINT8 m_AVCLevelIndication;
	UINT8 m_lengthSizeMinusOne;
	UINT8 m_numOfSequenceParameterSets;
	UINT16 m_spsLength;
	BYTE *m_seqParamSet;

	UINT8 m_numOfPictureParameterSets;
	UINT16 m_ppsLength;
	BYTE *m_picParamSet;
};

// video tag structure, containing video tag header + AVCDecoderConfigurationRecord / NALs
class PARSERLIB_API VideoTag
{
public:
	VideoTag(BYTE *dataBuf, UINT32 dataSize);
	~VideoTag();

	// VideoTagHeader object
	VideoTagHeader *m_videoTagHeader;

	// Parse video tag info from bit stream
	int Parse();

	// Dump video tag info to log file
	void Dump_video_tag_info();

	BYTE *Get_sps_buf_with_len(UINT16 &spsLen);	// Get sps buffer pointer and sps length
	BYTE *Get_pps_buf_with_len(UINT16 &ppsLen);	// Get pps buffer pointer and pps length

private:
	BYTE *m_dataBuffer;
	UINT32 m_dataSize;

	NALUnitsBuffer *m_nals;
	NALUnit *m_firstUnit, *m_currUnit;
	CAVCDecoderConfigurationRecord *m_decoderConfigRecord;

	int create_video_tag_header();	// Create video tag header from bit stream
	int parse_nal_unit();			// pause nal units info
	void dump_nal_info();			// dump nal units info to log file
};

// AudioSpecificConfig class
class AudioSpecificConfig
{
public:
	AudioSpecificConfig(BYTE *dataBuf);
	~AudioSpecificConfig();

	// Parse bit stream to AudioSpecificConfig info
	int Parse();

	// Dump AudioSpecificConfig info to log file
	void Dump_audio_speci_config();

private:
	BYTE *m_dataBuf;

	UINT8 m_audioObjectType;
	UINT8 m_samplingFrequencyIndex;
	UINT32 m_samplingFrequency;
	UINT8 m_channelConfiguration;
};

// AudioTag class
class PARSERLIB_API AudioTag
{
public:
	AudioTag(BYTE *dataBuf, UINT32 dataSize);
	~AudioTag();
		
	AudioTagHeader *m_audioTagHeader;		//Audio tag header object
	AudioSpecificConfig *m_audioSpecCfg;	//AudioSpecificConfig object

	// Parse Audio tag from bit stream
	int Parse();

	// Dump audio tag info to log file
	void Dump_audio_tag_info();

private:
	BYTE *m_dataBuffer;
	UINT32 m_dataSize;

	int get_audio_tag_header();
};


typedef void(*pTag_buf_edit_callback)(CFlvTag *tag);

// Flv file writer...
class PARSERLIB_API CFlvWriter
{
public:
	CFlvWriter(const char *outputFileName, const CFlvParser *parser);
	~CFlvWriter();

	int Init(bool videoFlag, bool audioFlag);
 	int Clone_FLV_with_video();

	// Clone flv tags to output flv file after edited
 	int Create_FLV_with_edited_tag(pTag_buf_edit_callback callback);

	// Extract flv tags with start and end index
	int Extract_tags_with_range(UINT32 startIdx, UINT32 endIdx);

private:
	const char *m_outputFileName;
	const CFlvParser *m_parser;
	std::ofstream m_outputFileStream;

 	int write_tag(CFlvTag *tag);	// Write tag to output file
};