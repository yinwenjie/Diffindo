#include "stdafx.h"
#include "ParserLib.h"

using namespace std;

CAVCDecoderConfigurationRecord::CAVCDecoderConfigurationRecord(BYTE *dataBuf)
{
	m_dataBuf = dataBuf;
	m_seqParamSet = NULL;
	m_picParamSet = NULL;
}

CAVCDecoderConfigurationRecord::~CAVCDecoderConfigurationRecord()
{
	if (m_seqParamSet)
	{
		delete m_seqParamSet;
		m_seqParamSet = NULL;
	}

	if (m_picParamSet)
	{
		delete m_picParamSet;
		m_picParamSet = NULL;
	}
}

int CAVCDecoderConfigurationRecord::Parse()
{
	m_configurationVersion = m_dataBuf[0];
	if (m_configurationVersion != 1)
	{
		return kFlvParserError_IllegalAVCDecoderConfigVersion;
	}
	m_AVCProfileIndication = m_dataBuf[1];
	m_profile_compatibility = m_dataBuf[2];
	m_AVCLevelIndication = m_dataBuf[3];
	m_lengthSizeMinusOne = m_dataBuf[4] & 0x03;
	m_numOfSequenceParameterSets = m_dataBuf[5] & 0x1F;
	if (1 != m_numOfSequenceParameterSets)
	{
		return kFlvParserError_MultipleSequenceParamaterSets;
	}
	memcpy_s(&m_spsLength, 2, m_dataBuf + 6, 2);
	endian_swap(reinterpret_cast<BYTE *>(&m_spsLength), 2);
	m_seqParamSet = new BYTE[m_spsLength];
	memcpy_s(m_seqParamSet, m_spsLength, m_dataBuf + 8, m_spsLength);


	m_numOfPictureParameterSets = m_dataBuf[8 + m_spsLength];
	if (1 != m_numOfPictureParameterSets)
	{
		return kFlvParserError_MultiplePictureParamaterSets;
	}
	memcpy_s(&m_ppsLength, 2, m_dataBuf + 9 + m_spsLength, 2);
	endian_swap(reinterpret_cast<BYTE *>(&m_ppsLength), 2);
	m_picParamSet = new BYTE[m_ppsLength];
	memcpy_s(m_picParamSet, m_ppsLength, m_dataBuf + 11 + m_spsLength, m_ppsLength);

	return kFlvParserError_NoError;
}

void CAVCDecoderConfigurationRecord::Dump_decoder_configuration_record()
{
#if DUMP_DECODER_CONFIG_RECORD_LOG
	g_logoutFile << "Configuration Version: " << to_string(m_configurationVersion) << endl;
	g_logoutFile << "AVC Profile: " << to_string(m_AVCProfileIndication) << endl;
	g_logoutFile << "AVC Level: " << to_string(m_AVCLevelIndication) << endl;
	g_logoutFile << "NAL Unit Length: " << to_string(m_lengthSizeMinusOne + 1) << endl;
	g_logoutFile << "Number of SPS: " << to_string(m_numOfSequenceParameterSets) << endl;
	g_logoutFile << "Number of PPS: " << to_string(m_numOfPictureParameterSets) << endl;
#endif
}

BYTE* CAVCDecoderConfigurationRecord::Get_sps_buf()
{
	return m_seqParamSet;
}

UINT16 CAVCDecoderConfigurationRecord::Get_sps_len()
{
	return m_spsLength;
}

BYTE* CAVCDecoderConfigurationRecord::Get_pps_buf()
{
	return m_picParamSet;
}

UINT16 CAVCDecoderConfigurationRecord::Get_pps_len()
{
	return m_ppsLength;
}


VideoTag::VideoTag(BYTE *dataBuf, UINT32 dataSize)
{
	m_dataBuffer = dataBuf;
	m_dataSize = dataSize;
	m_videoTagHeader = NULL;
	m_nals = NULL;
	m_firstUnit = NULL;
}

VideoTag::~VideoTag()
{
	if (m_videoTagHeader)
	{
		delete m_videoTagHeader;
		m_videoTagHeader = NULL;
	}

	if (m_firstUnit)
	{
		NALUnit *unit = m_firstUnit;
		while (unit)
		{
			NALUnit *next = unit->nextNalUnit;
			delete unit;
			unit = next;
		}
		m_firstUnit = NULL;
	}
}

int VideoTag::Parse()
{
	int err = 0;
	m_videoTagHeader = new VideoTagHeader;
	err = create_video_tag_header();
	if (err < 0)
	{
		return err;
	}

	if (m_videoTagHeader->AVPacketType == 0)
	{
		// AVCDecoderConfigurationRecord
		m_decoderConfigRecord = new CAVCDecoderConfigurationRecord(m_dataBuffer + 5);
		m_decoderConfigRecord->Parse();
	}
	else if (m_videoTagHeader->AVPacketType == 1)
	{
		m_nals = new NALUnitsBuffer;
		m_nals->bufStart = m_dataBuffer + 5;
		m_nals->lengths = m_dataSize - 5;
		parse_nal_unit();
	}

	return kFlvParserError_NoError;
}

void VideoTag::Dump_video_tag_info()
{
#if DUMP_VIDEO_TAG_INFO_ENABLED_CONSOLE
	cout << "Frame Type: " << to_string(m_videoTagHeader->frameType) << endl;
	cout << "CodecID: " << to_string(m_videoTagHeader->codecID) << endl;
	cout << "AVPacketType: " << to_string(m_videoTagHeader->AVPacketType) << endl;
	cout << "compositionTime: " << to_string(m_videoTagHeader->compositionTime) << endl;
#endif

#if DUMP_VIDEO_TAG_INFO_ENABLED_LOG
	g_logoutFile << "Frame Type: " << to_string(m_videoTagHeader->frameType) << endl;
	g_logoutFile << "CodecID: " << to_string(m_videoTagHeader->codecID) << endl;
	g_logoutFile << "AVPacketType: " << to_string(m_videoTagHeader->AVPacketType) << endl;
	g_logoutFile << "compositionTime: " << to_string(m_videoTagHeader->compositionTime) << endl;
#endif

	if (m_decoderConfigRecord)
	{
		m_decoderConfigRecord->Dump_decoder_configuration_record();
	}
	if (m_firstUnit)
	{
		dump_nal_info();
	}
}

BYTE * VideoTag::Get_sps_buf_with_len(UINT16 &spsLen)
{
	if (!m_decoderConfigRecord)
	{
		spsLen = 0;
		return NULL;
	}

	spsLen = m_decoderConfigRecord->Get_sps_len();

	return m_decoderConfigRecord->Get_sps_buf();
}

BYTE * VideoTag::Get_pps_buf_with_len(UINT16 &ppsLen)
{
	if (!m_decoderConfigRecord)
	{
		ppsLen = 0;
		return NULL;
	}

	ppsLen = m_decoderConfigRecord->Get_pps_len();

	return m_decoderConfigRecord->Get_pps_buf();
}

int VideoTag::create_video_tag_header()
{
	int err = 0;
	UINT8 buf = m_dataBuffer[0];
	m_videoTagHeader->frameType = (buf & 0xF0) >> 4;
	m_videoTagHeader->codecID = (buf & 0x0F);
	if (m_videoTagHeader->codecID != 7)
	{
		return kFlvParserError_UnsupportedCodec;
	}
	m_videoTagHeader->AVPacketType = m_dataBuffer[1];

	UINT32 compTime = 0;
	memcpy_s(&compTime, sizeof(UINT32), m_dataBuffer + 2, 3);
	endian_swap(reinterpret_cast<BYTE*>(&compTime), 3);
	m_videoTagHeader->compositionTime = compTime;

	return kFlvParserError_NoError;
}

int VideoTag::parse_nal_unit()
{
	UINT32 totalLen = 0, len = 0;

	while (totalLen < m_nals->lengths)
	{
		read_len_swap(reinterpret_cast<BYTE*>(&len), m_nals->bufStart + totalLen, sizeof(UINT32));
		NALUnit *nalUnit = new NALUnit;
		memset(nalUnit, 0, sizeof(NALUnit));

		// Get nal unit type
		nalUnit->nalUnitType = m_nals->bufStart[4 + totalLen] & 0x1F;

		// Check no-multiple-slice-in-frame
		if (!(0x80 & m_nals->bufStart[5]) && ((nalUnit->nalUnitType == 5) || (nalUnit->nalUnitType == 1)))
		{
			return kFlvParserError_MultipleSliceInFrame;
		}

		if (!m_firstUnit)
		{
			m_firstUnit = nalUnit;
			m_currUnit = nalUnit;
		}
		else
		{
			m_currUnit->nextNalUnit = nalUnit;
			m_currUnit = nalUnit;
		}
		totalLen += 4 + len;
	}

	return kFlvParserError_NoError;
}

void VideoTag::dump_nal_info()
{
#if DUMP_NAL_UNIT_INFO_LOG
	NALUnit *unit = m_firstUnit;
	while (unit)
	{
		g_logoutFile << "NAL Unit Type: " << to_string(unit->nalUnitType) << endl;
		unit = unit->nextNalUnit;
	}
#endif
}
