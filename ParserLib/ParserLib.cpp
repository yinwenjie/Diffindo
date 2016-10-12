// ParserLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ParserLib.h"
#include "FlvBody.h"

using namespace std;

// Flv parsing error description...
PARSERLIB_API const char* errorHints[17] = 
{
	NULL,
	"Error: input file name should not be empty",
	"Error: opening input file failed",
	"Error: reading input file content failed",
	"Error: input file not start with 'FLV'",
	"Error: first previous tag size not zero",
	"Error: tag type should be only 'video', 'audio' or 'script data'",
	"Error: tag streamID not zero",
	"Error: opening output file failed",
	"Error: writing output file failed",
	"Error: Codec not supported",
	"Error: AVCDecoderConfigurationRecord Version illegal",
	"Error: Multiple SPS in Decoder Configuration not supported",
	"Error: Multiple PPS in Decoder Configuration not supported",
	"Error: Cannot find SPS or PPS in Decoder Configuration",
	"Error: NALU header length illegal, should only be 1, 2 or 4",
	"Error: Multiple slice in frame not supported"
};

// Constructor
CFlvParser::CFlvParser(const char *fileName)
{
	m_fileName = fileName;
	m_fileSize = 0;
	m_fileBuf = NULL;
	m_bytesCnt = 0;

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.open("log.txt");
	g_logoutFile << fileName << endl;
#endif

}

// De-constructor
CFlvParser::~CFlvParser()
{
	if (m_fileBuf)
	{
		delete[] m_fileBuf;
		m_fileBuf = NULL;
	}

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.close();
#endif
}

// Parse FLV file
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
	dump_flv_tag_info();

	return kFlvParserError_NoError;
}

// Get first tag of parser
CFlvTag * CFlvParser::Get_first_tag()
{
	return m_flvBody->Get_first_tag();
}

// Get file buffer position...
BYTE * CFlvParser::Get_parser_buffer()
{
	return m_fileBuf;
}


//======================Private Methods==========================
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
		m_inFileStream.close();
		return kFlvParserError_ReadInputFileFailed;
	}

	m_inFileStream.close();
	return kFlvParserError_NoError;
}


void CFlvParser::dump_flv_file_info()
{
	cout << "***********************************" << endl;
	cout << "File Name: " << m_fileName << endl;
	cout << "File Size: " << to_string(m_fileSize) << endl;
	cout << "***********************************" << endl;
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

	memcpy(&m_flvHeader->dataOffset, &m_fileBuf[m_bytesCnt + 5], 4);
	endian_swap(reinterpret_cast<BYTE *>(&m_flvHeader->dataOffset), 4);

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
}

int	CFlvParser::create_flv_body()
{
	int err = m_flvBody->Parse(m_fileBuf, m_bytesCnt, m_fileSize);
	if (err < 0)
	{
		return err;
	}

	return kFlvParserError_NoError;
}

void CFlvParser::dump_flv_tag_info()
{
	m_flvBody->Dump_flv_tag_info();
}