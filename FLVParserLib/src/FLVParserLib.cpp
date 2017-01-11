// FLVParserLib.cpp : Define DLL export functions
//
#include "Include.h"
#include "FLVParserLib.h"
#include "FLVHeader.h"
#include "FLVBody.h"
#include "FLVTag.h"
#include "VideoTag.h"

using namespace std;

FLVPARSERLIB_API FLVRuntimeConfiguration g_flvConfig;

CFlvParser::CFlvParser(const char *fileName)
{
	if (fileName)
	{
		m_fileName = fileName;
	}

	m_fileSize = 0;
	m_fileBuf = NULL;
	m_bytesCnt = 0;

	m_flvHeader = NULL;
	m_flvBody = NULL;

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.open("log.txt");
#endif

}

CFlvParser::~CFlvParser()
{
	if (m_fileBuf)
	{
		delete[] m_fileBuf;
		m_fileBuf = NULL;
	}

	if (m_flvHeader)
	{
		delete m_flvHeader;
		m_flvHeader = NULL;
	}

	if (m_flvBody)
	{
		delete m_flvBody;
		m_flvBody = NULL;
	}

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.close();
#endif
}

//*************** Public API ***************
int CFlvParser::Parse()
{
	int err = open_input_file();
	if (err < 0)
	{
		return err;
	}

	err = get_input_file_data();
	if (err < 0)
	{
		return err;
	}
	dump_flv_file_info();

	m_flvHeader = new FlvHeader;
	err = create_flv_header();
	if (err < 0)
	{
		return err;
	}
	dump_flv_header_info();

	m_flvBody = new CFlvBody;
	err = create_flv_body();
	if (err < 0)
	{
		return err;
	}

	return kFlvParserError_NoError;
}

//*************** Private Methods ***************
int CFlvParser::open_input_file()
{
	if (!m_fileName)
	{
		return kFlvParserError_NullFileName;
	}

	m_inFileStream.open(m_fileName, ios_base::binary);
	if (!m_inFileStream.is_open())
	{
		return kFlvParserError_OpenInputFileFailed;
	}

	return kFlvParserError_NoError;
}

int CFlvParser::get_input_file_data()
{
	m_inFileStream.seekg(0, ios_base::end);
	m_fileSize = m_inFileStream.tellg();
	m_inFileStream.seekg(0, ios_base::beg);

	m_fileBuf = new BYTE[m_fileSize];
	m_inFileStream.read(reinterpret_cast<char *>(m_fileBuf), m_fileSize);
	if ((m_inFileStream.rdstate() & ifstream::failbit) || (m_inFileStream.rdstate() & ifstream::badbit))
	{
		close_input_file();
		return kFlvParserError_ReadInputFileFailed;
	}

	close_input_file();
	return kFlvParserError_NoError;
}

void CFlvParser::close_input_file()
{
	if (m_inFileStream.is_open())
	{
		m_inFileStream.close();
	}
}

void CFlvParser::dump_flv_file_info()
{
	cout << "***********************************" << endl;
	cout << "File Name: " << m_fileName << endl;
	cout << "File Size: " << to_string(m_fileSize) << endl;
	cout << "***********************************" << endl;

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile << "***********************************" << endl;
	g_logoutFile << "File Name: " << m_fileName << endl;
	g_logoutFile << "File Size: " << to_string(m_fileSize) << endl;
	g_logoutFile << "***********************************" << endl;
#endif
}

int CFlvParser::create_flv_header()
{
	m_flvHeader->signature[0] = m_fileBuf[m_bytesCnt + 0];
	m_flvHeader->signature[1] = m_fileBuf[m_bytesCnt + 1];
	m_flvHeader->signature[2] = m_fileBuf[m_bytesCnt + 2];

	if (m_flvHeader->signature[0] != 'F' || m_flvHeader->signature[1] != 'L' || m_flvHeader->signature[2] != 'V')
	{
		return kFlvParserError_IllegalFlvHeader;
	}

	m_flvHeader->version = m_fileBuf[m_bytesCnt + 3];
	m_flvHeader->audioFlag = m_fileBuf[m_bytesCnt + 4] & 0x04;
	m_flvHeader->videoFlag = m_fileBuf[m_bytesCnt + 4] & 0x01;
	if (m_fileBuf[m_bytesCnt + 4] & 0xFA)
	{
		return kFlvParserError_IllegalFlvHeader;
	}

	Read_data_lsb(&(m_flvHeader->dataOffset), m_fileBuf + m_bytesCnt + 5, 4);
	
	m_bytesCnt += 9;

	return kFlvParserError_NoError;
}

void CFlvParser::dump_flv_header_info()
{
	cout << "----------------------------------" << endl;
	cout << "FLV Version: " << to_string(m_flvHeader->version) << endl;
	cout << "Video Flag: " << to_string(m_flvHeader->videoFlag) << endl;
	cout << "Audio Flag: " << to_string(m_flvHeader->audioFlag) << endl;
	cout << "Data offset: " << to_string(m_flvHeader->dataOffset) << endl;
	cout << "----------------------------------" << endl;

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile << "FLV Version: " << to_string(m_flvHeader->version) << endl;
	g_logoutFile << "Video Flag: " << to_string(m_flvHeader->videoFlag) << endl;
	g_logoutFile << "Audio Flag: " << to_string(m_flvHeader->audioFlag) << endl;
	g_logoutFile << "Data offset: " << to_string(m_flvHeader->dataOffset) << endl;
#endif
}

int CFlvParser::create_flv_body()
{
	int err = m_flvBody->Parse(m_fileBuf, m_bytesCnt, m_fileSize);
	if (err < 0)
	{
		return err;
	}

	return kFlvParserError_NoError;
}

CFlvWriter::CFlvWriter(const char *outputFileName, const CFlvParser *parser)
{
	m_outputFileName = outputFileName;
	m_parser = parser;
}

CFlvWriter::~CFlvWriter()
{
	if (m_outputFileStream.is_open())
	{
		m_outputFileStream.close();
	}
}

int CFlvWriter::Init(bool videoFlag, bool audioFlag)
{
	m_outputFileStream.open(m_outputFileName, ios_base::binary);
	if (!m_outputFileStream.is_open())
	{
		return kFlvParserError_OpenOutputFileFailed;
	}

	BYTE flvHeader[9] = { 'F', 'L', 'V', 0x01, 0x00, 0x00, 0x00, 0x00, 0x09};
	if (videoFlag)
	{
		flvHeader[4] |= 0x01;
	}
	if (audioFlag)
	{
		flvHeader[4] |= 0x04;
	}

	// Write FLV header..
	m_outputFileStream.write(reinterpret_cast<const char*>(flvHeader), 9 * sizeof(UINT8));
	if ((m_outputFileStream.rdstate() & ofstream::failbit) || (m_outputFileStream.rdstate() & ofstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	return kFlvParserError_NoError;
}

int CFlvWriter::Clone_FLV_with_video()
{
	CFlvTag *tag = m_parser->m_flvBody->Get_first_tag();

	while (tag)
	{
		if (tag->m_tagType == TAG_TYPE_AUDIO)
		{
			tag = tag->m_nextTag;
			continue;
		}
		
		write_tag(tag);

		tag = tag->m_nextTag;
	}

	return kFlvParserError_NoError;
}

int CFlvWriter::Clone_with_tag_index_range(UINT32 startIdx, UINT32 endIdx)
{
	if (startIdx > endIdx)
	{
		return kFlvParserError_IllegalFlvWriterIdxRange;
	}

	CFlvTag *tag = m_parser->m_flvBody->Get_first_tag();
	while (tag)
	{
		if (tag->m_tagIdx < startIdx)
		{
			tag = tag->m_nextTag;
			continue;
		}

		if (tag->m_tagIdx > endIdx)
		{
			break;
		}

		write_tag(tag);

		tag = tag->m_nextTag;
	}

	return kFlvParserError_NoError;
}

int CFlvWriter::Clone_with_tag_time_stamp_range(UINT32 startTS, UINT32 endTS)
{
	if (startTS > endTS)
	{
		return kFlvParserError_IllegalFlvWriterIdxTimeStamp;
	}

	CFlvTag *tag = m_parser->m_flvBody->Get_first_tag();
	while (tag)
	{
		if (tag->m_timeStamp < startTS)
		{
			tag = tag->m_nextTag;
			continue;
		}

		if (tag->m_timeStamp > endTS)
		{
			break;
		}

		write_tag(tag);

		tag = tag->m_nextTag;
	}
	return kFlvParserError_NoError;
}

int CFlvWriter::Clone_with_accelerating_factor(double factor)
{
	if (factor > 10.0)
	{
		factor = 10.0;
	}
	if (factor == 0.0)
	{
		factor = 1;
	}

	CFlvTag *tag = m_parser->m_flvBody->Get_first_tag();
	while (tag)
	{
		tag->m_timeStamp /= factor;

		write_tag(tag);

		tag = tag->m_nextTag;
	}

	return kFlvParserError_NoError;
}

int CFlvWriter::Extract_H264_raw_stream()
{
	m_outputH264Stream.open(m_outputFileName, ios_base::binary);
	if (!m_outputH264Stream.is_open())
	{
		return kFlvParserError_OpenOutputFileFailed;
	}

	CFlvTag *tag = m_parser->m_flvBody->Get_first_tag();
	BYTE *spsBuf = NULL, *ppsBuf = NULL;
	UINT16 spsLen = 0, ppsLen = 0;
	NALUnit *nalu = NULL;

	while (tag)
	{
		if (tag->m_tagType != TAG_TYPE_VIDEO)
		{
			tag = tag->m_nextTag;
			continue;
		}

		CVideoTag *videoTagPtr = dynamic_cast<CVideoTag *>(tag);
		if (NULL != videoTagPtr)
		{
			if (videoTagPtr->m_AVCPacketType == 0)
			{
				if (videoTagPtr->m_decCfgRcrd)
				{
					spsBuf = videoTagPtr->m_decCfgRcrd->sps->naluBuffer;
					spsLen = videoTagPtr->m_decCfgRcrd->sps->naluLength;

					ppsBuf = videoTagPtr->m_decCfgRcrd->pps->naluBuffer;
					ppsLen = videoTagPtr->m_decCfgRcrd->pps->naluLength;

					//write sps...
					write_nalu(spsBuf, spsLen);

					//write pps...
					write_nalu(ppsBuf, ppsLen);
				}
			} 
			else
			{
				nalu = videoTagPtr->m_nalu;
				if (!nalu)
				{
					tag = tag->m_nextTag;
					continue;
				}

				do
				{
					write_nalu(nalu->naluBuffer, nalu->naluLength);
					nalu = nalu->nextNalu;
				} while (nalu);
			}
		}
		tag = tag->m_nextTag;
	}

	m_outputH264Stream.close();
	return kFlvParserError_NoError;
}

int CFlvWriter::write_tag(CFlvTag *tag)
{
	BYTE streamID[3] = { 0 };

	UINT32 prevTagSize = tag->m_prevTagSize;
	endian_swap(reinterpret_cast<BYTE *>(&prevTagSize), 4);
	m_outputFileStream.write(reinterpret_cast<const char*>(&prevTagSize), sizeof(UINT32));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write tag type..
	UINT8 tagType = tag->m_tagType;
	m_outputFileStream.write(reinterpret_cast<const char*>(&tagType), sizeof(UINT8));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write data size..
	UINT32 dataSize = tag->m_dataSize;
	endian_swap(reinterpret_cast<BYTE *>(&dataSize), 3);
	m_outputFileStream.write(reinterpret_cast<const char*>(&dataSize), 3 * sizeof(UINT8));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write timestamp
	UINT32 timestamp = tag->m_timeStamp;
	endian_swap(reinterpret_cast<BYTE *>(&timestamp), 3);
	timestamp += tag->m_timeStampExtension;
	m_outputFileStream.write(reinterpret_cast<const char*>(&timestamp), sizeof(UINT32));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write stream id
	m_outputFileStream.write(reinterpret_cast<const char*>(streamID), 3 * sizeof(BYTE));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write payload data
	m_outputFileStream.write(reinterpret_cast<const char*>(tag->m_tagBuffer + 11), tag->m_dataSize);
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	return kFlvParserError_NoError;
}

int CFlvWriter::write_nalu(BYTE *nalBuffer, UINT32 nalLen)
{
	UINT8 prefix[4] = { 0, 0, 0, 1 };

	m_outputH264Stream.write(reinterpret_cast<const char*>(prefix), 4 * sizeof(BYTE));
	if ((m_outputH264Stream.rdstate() & ifstream::failbit) || (m_outputH264Stream.rdstate() & ifstream::badbit))
	{
		m_outputH264Stream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputH264Stream.flush();

	m_outputH264Stream.write(reinterpret_cast<const char*>(nalBuffer), nalLen * sizeof(BYTE));
	if ((m_outputH264Stream.rdstate() & ifstream::failbit) || (m_outputH264Stream.rdstate() & ifstream::badbit))
	{
		m_outputH264Stream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputH264Stream.flush();

	return kFlvParserError_NoError;
}
