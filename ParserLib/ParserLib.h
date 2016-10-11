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

extern PARSERLIB_API int nParserLib;

PARSERLIB_API int fnParserLib(void);

extern bool g_Dump_Tag_Info_Enabled_Log;

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

	int Parse(UINT64 &byteCnt);
	int Parse_detail();

	void Dump_tag_info();

	UINT8 Get_tag_type();
	UINT64 Get_tag_total_len();
	UINT64 Get_start_pos();
	UINT32 Get_tag_dataSize();
	UINT32 Get_tag_timestamp_ext();
	const char * Get_tag_payload_buf();
	UINT32 Get_tag_index();

	void Set_tag_timestamp(UINT32 timestamp);

	CFlvTag* m_nextTag;
private:
	UINT32	m_tagIdx;
	BYTE*	m_fileBuf;
	UINT64	m_tagStartPosition;
	UINT64	m_tagEndPosition;

	UINT8	m_tagType;
	UINT32	m_dataSize;
	UINT32	m_timeStamp;
	UINT8	m_timeStampExtension;
	UINT32	m_streamID;

	BYTE*	m_tagDataBuf;

	VideoTag *m_videoTag;
	AudioTag *m_audioTag;
};

// Flv parsing core class...
class PARSERLIB_API CFlvParser
{
public:
	CFlvParser(const char *fileName);
	~CFlvParser();

	int Parse();

	CFlvTag *Get_first_tag();
	BYTE *Get_parser_buffer();

private:
	const char    *m_fileName;
	UINT64        m_fileSize;
	BYTE          *m_fileBuf;
	UINT64        m_bytesCnt;
	
	std::ifstream m_inFileStream;
	int     open_input_file();
	int     get_input_file_data();
	void	dump_flv_file_info();

	FlvHeader     *m_flvHeader;
	int     create_flv_header();
	void	dump_flv_header_info();

	CFlvBody	  *m_flvBody;
	int		create_flv_body();
	void	dump_flv_tag_info();
};


class CAVCDecoderConfigurationRecord;

typedef struct VideoTagHeader
{
	UINT8 frameType;
	UINT8 codecID;
	UINT8 AVPacketType;
	int	  compositionTime;
} VideoTagHeader;

typedef struct AudioTagHeader
{
	UINT8 soundFormat;
	UINT8 soundRate;
	bool  soundSize;
	bool  soundType;
	UINT8 AACPacketType;
} AudioTagHeader;

typedef struct NALUnitsBuffer
{
	BYTE *bufStart;
	int lengths;
} NALUnitsBuffer;

typedef struct NALUnit
{
	UINT8 nalUnitType;
	UINT8 sliceType;

	NALUnit *nextNalUnit;
} NALUnit;

class CAVCDecoderConfigurationRecord
{
public:
	CAVCDecoderConfigurationRecord(BYTE *dataBuf);
	~CAVCDecoderConfigurationRecord();

	int Parse();
	void Dump_decoder_configuration_record();

	BYTE* Get_sps_buf();
	UINT16 Get_sps_len();
	BYTE* Get_pps_buf();
	UINT16 Get_pps_len();

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

class PARSERLIB_API VideoTag
{
public:
	VideoTag(BYTE *dataBuf, UINT32 dataSize);
	~VideoTag();

	VideoTagHeader *m_videoTagHeader;

	int Parse();
	void Dump_video_tag_info();
	BYTE *Get_sps_buf_with_len(UINT16 &spsLen);
	BYTE *Get_pps_buf_with_len(UINT16 &ppsLen);

private:
	BYTE *m_dataBuffer;
	UINT32 m_dataSize;

	NALUnitsBuffer *m_nals;
	NALUnit *m_firstUnit, *m_currUnit;
	CAVCDecoderConfigurationRecord *m_decoderConfigRecord;

	int get_video_tag_header();
	int parse_nal_unit();
	void dump_nal_info();
};

class AudioSpecificConfig
{
public:
	AudioSpecificConfig(BYTE *dataBuf);
	~AudioSpecificConfig();

	int Parse();
	void Dump_audio_speci_config();

private:
	BYTE *m_dataBuf;

	UINT8 m_audioObjectType;
	UINT8 m_samplingFrequencyIndex;
	UINT32 m_samplingFrequency;
	UINT8 m_channelConfiguration;
};


class PARSERLIB_API AudioTag
{
public:
	AudioTag(BYTE *dataBuf, UINT32 dataSize);
	~AudioTag();

	AudioTagHeader *m_audioTagHeader;
	AudioSpecificConfig *m_audioSpecCfg;

	int Parse();
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

 	int write_tag(CFlvTag *tag);
};