#include "FLVParserLib.h"
#include "VideoTag.h"

using namespace std;

CVideoTag::CVideoTag(UINT32 idx, UINT8 *buf, UINT8 type) : CFlvTag(idx, buf, type)
{
	m_frameType = 0;
	m_codecID = 0;
	m_AVCPacketType = 0;
	m_CompositionTime = 0;

	m_decCfgRcrd = NULL;
}

CVideoTag::~CVideoTag()
{
	if (m_decCfgRcrd)
	{
		delete m_decCfgRcrd;
		m_decCfgRcrd = NULL;
	}
}

int CVideoTag::Parse_video_tag_detail()
{
	m_frameType = m_tagBuffer[11] >> 4;
	if (m_frameType < 1 || m_frameType > 5)
	{
		return kFlvParserError_IllegalVideoFrameType;
	}
	m_codecID = m_tagBuffer[11] & 0x0f;
	if (m_codecID != 7)
	{
		return kFlvParserError_UnsupportedVideoCodecID;
	}
	m_AVCPacketType = m_tagBuffer[12];
	Read_data_lsb(&m_CompositionTime, m_tagBuffer + 13, 3);

	if (m_AVCPacketType == 0)
	{
		m_decCfgRcrd = new AVCDecoderConfigurationRecord;
		m_decCfgRcrd->Parse_as_configuration_record(m_tagBuffer + 16);
	}

	return kFlvParserError_NoError;
}

void CVideoTag::Dump_video_tag_info()
{
#if DUMP_TAG_INFO_ENABLED_LOG

#if DUMP_VIDEO_TAG_INFO_ENABLED_LOG
	g_logoutFile << "Frame Type: " << to_string(m_frameType) << endl;
	g_logoutFile << "Codec ID: " << to_string(m_codecID) << endl;
	g_logoutFile << "AVCPacketType: " << to_string(m_AVCPacketType) << endl;
	g_logoutFile << "CompositionTime: " << to_string(m_CompositionTime) << endl;
#endif

#if DUMP_VIDEO_TAG_PAYLOAD_INFO
	dump_video_payload_info();
#endif

#endif
}

void CVideoTag::dump_video_payload_info()
{
	if (m_AVCPacketType == 0)
	{
#if DUMP_DECODER_CONFIG_RECORD_LOG
		g_logoutFile << "Configuration Version: " << to_string(m_decCfgRcrd->configurationVersion) << endl;
		g_logoutFile << "AVC Profile: " << to_string(m_decCfgRcrd->AVCProfileIndication) << endl;
		g_logoutFile << "Profile Compatibility: " << to_string(m_decCfgRcrd->profile_compatibility) << endl;
		g_logoutFile << "AVC Level: " << to_string(m_decCfgRcrd->AVCLevelIndication) << endl;
		g_logoutFile << "NALU Length Size: " << to_string(m_decCfgRcrd->lengthSize) << endl;
		g_logoutFile << "Number of SPS: " << to_string(m_decCfgRcrd->numSPS) << endl;
		g_logoutFile << "Number of PPS: " << to_string(m_decCfgRcrd->numPPS) << endl;
#endif
	}
	else
	{

	}
}
