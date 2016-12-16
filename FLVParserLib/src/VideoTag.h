#ifndef _VIDEO_TAG_H_
#define _VIDEO_TAG_H_

#include "FLVTag.h"
#include "DataTypes.h"

typedef struct NALUnit
{	
	UINT32 naluLength;
	BYTE *naluBuffer;
	struct NALUnit *nextNalu;
	NALUnit()
	{
		naluBuffer = NULL;
		naluLength = 0;
		nextNalu = NULL;
	}
	int Get_nal_unit(BYTE *inputBuffer, bool paramaterSets)
	{
		int sizeLength = 4;
		if (paramaterSets)
		{
			sizeLength = 2;
		} 
		
		Read_data_lsb(&naluLength, inputBuffer, sizeLength);
		if (naluLength == 0)
		{
			return kFlvParserError_EmptyNALUnit;
		}
		naluBuffer = new BYTE[naluLength];
		memcpy(naluBuffer, inputBuffer + sizeLength, naluLength);

		return kFlvParserError_NoError;
	}
	~NALUnit()
	{
		if (naluBuffer)
		{
			delete[] naluBuffer;
			naluBuffer = NULL;
		}
	}
} NALUnit;

typedef struct AVCDecoderConfigurationRecord
{
	UINT8 configurationVersion;
	UINT8 AVCProfileIndication;
	UINT8 profile_compatibility;
	UINT8 AVCLevelIndication;
	UINT8 lengthSize;
	UINT8 numSPS;
	NALUnit *sps;
	UINT8 numPPS;
	NALUnit *pps;
	AVCDecoderConfigurationRecord()
	{
		configurationVersion = 0;
		AVCProfileIndication = 0;
		profile_compatibility = 0;
		AVCLevelIndication = 0;
		lengthSize = 0;
		numSPS = 0;
		sps = NULL;
		numPPS = 0;
		pps = NULL;
	}
	int Parse_as_configuration_record(BYTE *buf)
	{
		int bytePosition = 0;
		configurationVersion = buf[bytePosition++];
		AVCProfileIndication = buf[bytePosition++];
		profile_compatibility = buf[bytePosition++];
		AVCLevelIndication = buf[bytePosition++];
		lengthSize = (buf[bytePosition++] & 0x03) + 1;
		if (lengthSize < 4)
		{
			return kFlvParserError_UnsupportedNALUnitLengthSize;
		}
		numSPS = buf[bytePosition++] & 0x1f;
		if (numSPS == 1)
		{
			sps = new NALUnit;
			sps->Get_nal_unit(buf + bytePosition, true);
			bytePosition += (2 + sps->naluLength);
		}
		else
		{
			return kFlvParserError_MultipleSPSPPSinVideoHeader;
		}
		numPPS = buf[bytePosition++];
		if (numPPS == 1)
		{
			pps = new NALUnit;
			pps->Get_nal_unit(buf + bytePosition, true);
		} 
		else
		{
			return kFlvParserError_MultipleSPSPPSinVideoHeader;
		}
		
		return kFlvParserError_NoError;
	}
	~AVCDecoderConfigurationRecord()
	{
		if (sps)
		{
			delete sps;
			sps = NULL;
		}
		if (pps)
		{
			delete pps;
			pps = NULL;
		}
	}
} AVCDecoderConfigurationRecord;

class CVideoTag : public CFlvTag
{
public:
	CVideoTag(UINT32 idx, UINT8 *buf, UINT8 type);
	virtual ~CVideoTag();

	UINT8 m_frameType;
	UINT8 m_codecID;
	UINT8 m_AVCPacketType;
	int   m_CompositionTime;

	int Parse_video_tag_detail();
	void Dump_video_tag_info();

	AVCDecoderConfigurationRecord *m_decCfgRcrd;

	NALUnit *m_nalu;

private:
	void dump_video_payload_info();
	int parse_nal_units();
};

#endif