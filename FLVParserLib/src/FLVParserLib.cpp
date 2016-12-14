// FLVParserLib.cpp : Define DLL export functions
//
#include "Include.h"
#include "FLVParserLib.h"
#include "FLVHeader.h"
#include "FLVBody.h"

using namespace std;

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

/*	m_flvBody = new CFlvBody;
	err = create_flv_body();
	if (err < 0)
	{
		return err;
	}*/

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
	g_logoutFile << "----------------------------------" << endl;
	g_logoutFile << "FLV Version: " << to_string(m_flvHeader->version) << endl;
	g_logoutFile << "Video Flag: " << to_string(m_flvHeader->videoFlag) << endl;
	g_logoutFile << "Audio Flag: " << to_string(m_flvHeader->audioFlag) << endl;
	g_logoutFile << "Data offset: " << to_string(m_flvHeader->dataOffset) << endl;
	g_logoutFile << "----------------------------------" << endl;
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
