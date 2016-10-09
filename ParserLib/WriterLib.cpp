#include "stdafx.h"
#include "ParserLib.h"

using namespace std;

// Default writer constructor...
CFlvWriter::CFlvWriter(const char *outputFileName, const CFlvParser *parser)
{
	m_outputFileName = outputFileName;
	m_parser = parser;
}

// Default writer destructor...
CFlvWriter::~CFlvWriter()
{
	if (m_outputFileStream.is_open())
	{
		m_outputFileStream.close();
	}
}

// Init a FlvWriter for a output flv file...
int CFlvWriter::Init(bool videoFlag, bool audioFlag)
{
	m_outputFileStream.open(m_outputFileName, ios_base::binary);
	if (!m_outputFileStream.is_open())
	{
		return kFlvParserError_OpenOutputFileFailed;
	}

	BYTE flvHeader[13] = { 'F', 'L', 'V',  0x01, 0x00, 0x00, 0x00, 0x00, 0x09 , 0x00 , 0x00 , 0x00 , 0x00 };
	if (videoFlag)
	{
		flvHeader[4] |= 0x01;
	}
	if (audioFlag)
	{
		flvHeader[4] |= 0x04;
	}

	// Write FLV header..
	m_outputFileStream.write(reinterpret_cast<const char*>(flvHeader), 13 * sizeof(UINT8));
	if ((m_outputFileStream.rdstate() & ofstream::failbit) || (m_outputFileStream.rdstate() & ofstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	return kFlvParserError_NoError;
}

// Clone video tags to new FLV file..
int CFlvWriter::Clone_FLV_with_video()
{
	UINT32 prevTagSize = 0;
	CFlvParser *parser = const_cast<CFlvParser *>(m_parser);
	CFlvTag *tag = parser->Get_first_tag();

	while (tag)
	{
		if (tag->Get_tag_type() == TAG_TYPE_AUDIO)
		{
			tag = tag->m_nextTag;
			continue;
		}

		prevTagSize = tag->Get_tag_total_len();
		m_outputFileStream.write(reinterpret_cast<const char*>(parser->Get_parser_buffer() + tag->Get_start_pos()), prevTagSize);
		if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
		{
			m_outputFileStream.close();
			return kFlvParserError_WriteOutputFileFailed;
		}
		m_outputFileStream.flush();

		endian_swap(reinterpret_cast<BYTE*>(&prevTagSize), sizeof(UINT32));
		m_outputFileStream.write(reinterpret_cast<const char*>(&prevTagSize), sizeof(UINT32));
		if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
		{
			m_outputFileStream.close();
			return kFlvParserError_WriteOutputFileFailed;
		}
		m_outputFileStream.flush();

		tag = tag->m_nextTag;
	}

	return kFlvParserError_NoError;
}


int CFlvWriter::Extract_tags_with_range(UINT32 startIdx, UINT32 endIdx)
{
	CFlvParser *parser = const_cast<CFlvParser *>(m_parser);
	CFlvTag *tag = parser->Get_first_tag();

	while (tag)
	{
		if ((tag->Get_tag_index() < startIdx) && (tag->Get_tag_type() != TAG_TYPE_SCRIPT))
		{
			tag = tag->m_nextTag;
			continue;
		}

		write_tag(tag);

		tag = tag->m_nextTag;

		if (tag->Get_tag_index() > endIdx)
		{
			break;
		}
	}

	return kFlvParserError_NoError;
}


int CFlvWriter::Create_FLV_with_edited_tag(pTag_buf_edit_callback callback)
{
	CFlvParser *parser = const_cast<CFlvParser *>(m_parser);
	CFlvTag *tag = parser->Get_first_tag();

	while (tag)
	{
		callback(tag);

		write_tag(tag);

		tag = tag->m_nextTag;
	}
	return kFlvParserError_NoError;
}

int CFlvWriter::write_tag(CFlvTag *tag)
{
	BYTE streamID[3] = { 0 };

	// Write tag type..
	UINT8 tagType = tag->Get_tag_type();
	m_outputFileStream.write(reinterpret_cast<const char*>(&tagType), sizeof(UINT8));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write data size..
	UINT32 dataSize = tag->Get_tag_dataSize();
	endian_swap(reinterpret_cast<BYTE *>(&dataSize), 3);
	m_outputFileStream.write(reinterpret_cast<const char*>(&dataSize), 3 * sizeof(UINT8));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write timestamp
	UINT32 timestamp = tag->Get_tag_timestamp_ext();
	endian_swap(reinterpret_cast<BYTE *>(&timestamp), 3);
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
	m_outputFileStream.write(tag->Get_tag_payload_buf(), tag->Get_tag_dataSize());
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	// Write previous tag size..
	endian_swap(reinterpret_cast<BYTE *>(&dataSize), 3);
	UINT32 prevTagSize = dataSize + 11;
	endian_swap(reinterpret_cast<BYTE *>(&prevTagSize), 4);
	m_outputFileStream.write(reinterpret_cast<const char*>(&prevTagSize), sizeof(UINT32));
	if ((m_outputFileStream.rdstate() & ifstream::failbit) || (m_outputFileStream.rdstate() & ifstream::badbit))
	{
		m_outputFileStream.close();
		return kFlvParserError_WriteOutputFileFailed;
	}
	m_outputFileStream.flush();

	return kFlvParserError_NoError;
}