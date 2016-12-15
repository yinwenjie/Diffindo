#include "FLVParserLib.h"
#include "FLVTag.h"

using namespace std;

CFlvTag::CFlvTag(UINT32 idx, UINT8 *buf, UINT8 type)
{
	memset(this, 0, sizeof(CFlvTag));

	m_tagIdx = idx;
	m_tagBuffer = buf;
	m_tagType = type;
	m_nextTag = NULL;
}

CFlvTag::~CFlvTag()
{
	
}

int CFlvTag::Parse_flv_tag()
{
	Read_data_lsb(&m_dataSize, m_tagBuffer + 1, 3);
	Read_data_lsb(&m_timeStamp, m_tagBuffer + 4, 3);
	m_timeStampExtension = m_tagBuffer[7];
	Read_data_lsb(&m_streamID, m_tagBuffer + 8, 3);
	if (m_streamID != 0)
	{
		return kFlvParserError_StreamIDNotZero;
	}

	return kFlvParserError_NoError;
}

void CFlvTag::Dump_tag_info()
{
#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile << "-----------------------------------" << endl;
	g_logoutFile << "Tag Index: " << to_string(m_tagIdx) << endl;
	g_logoutFile << "Tag Type: " << (m_tagType == 18 ? "Script" : (m_tagType == 8 ? "Audio" : "Video")) << endl;
	g_logoutFile << "Data Size: " << to_string(m_dataSize) << endl;
	g_logoutFile << "TimeStamp: " << to_string(m_timeStamp) << endl;
	g_logoutFile << "TimeStampExtension: " << to_string(m_timeStampExtension) << endl;
	g_logoutFile << "StreamID: " << to_string(m_streamID) << endl;
#endif
}
