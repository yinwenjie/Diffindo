// FLVParserLib.cpp : Define DLL export functions
//
#include "Include.h"
#include "FLVParserLib.h"

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

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.open("log.txt");
#endif

}

CFlvParser::~CFlvParser()
{
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
