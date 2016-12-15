#include "FLVParserLib.h"
#include "VideoTag.h"

using namespace std;

CVideoTag::CVideoTag(UINT32 idx, UINT8 *buf, UINT8 type) : CFlvTag(idx, buf, type)
{
	m_frameType = 0;
	m_codecID = 0;
	m_AVCPacketType = 0;
	m_CompositionTime = 0;
}

CVideoTag::~CVideoTag()
{

}

int CVideoTag::Parse_video_tag_detail()
{
	m_frameType = m_tagBuffer[11] >> 4;
	m_codecID = m_tagBuffer[11] & 0x0f;
	m_AVCPacketType = m_tagBuffer[12];
	Read_data_lsb(&m_CompositionTime, m_tagBuffer + 13, 3);

	return kFlvParserError_NoError;
}

void CVideoTag::Dump_video_tag_info()
{
#if DUMP_TAG_INFO_ENABLED_LOG

#ifdef DUMP_VIDEO_TAG_INFO_ENABLED_LOG
	g_logoutFile << "Frame Type: " << to_string(m_frameType) << endl;
	g_logoutFile << "Codec ID: " << to_string(m_codecID) << endl;
	g_logoutFile << "AVCPacketType: " << to_string(m_AVCPacketType) << endl;
	g_logoutFile << "CompositionTime: " << to_string(m_CompositionTime) << endl;
#endif

#endif
}
