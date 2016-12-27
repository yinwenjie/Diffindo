#include "MP4ParserLib.h"
#include "MP4File.h"

using namespace std;

CMP4Parser::CMP4Parser(const char *inputMP4FileName)
{
	m_fileName = inputMP4FileName;
	m_fileSize = 0;
	m_fileBuf = NULL;
	m_bytesCnt = 0;

	m_MP4File = NULL;

	g_logoutFile.open("MP4.txt");
	g_logoutFile << m_fileName << endl;
}

CMP4Parser::~CMP4Parser()
{
	if (g_logoutFile.is_open())
	{
		g_logoutFile.close();
	}
}

int CMP4Parser::Parse()
{
	int err = 0;
	err = open_input_file();
	if (err < 0)
	{
		return err;
	}

	err = get_input_file_data();
	if (err < 0)
	{
		return err;
	}
	dump_mp4_file_info();

	m_MP4File = new CMP4File(m_fileBuf);
	m_MP4File->Parse();

	close_input_file();
	return kMP4ParserError_NoError;
}

int CMP4Parser::open_input_file()
{
	if (!m_fileName)
	{
		return kMP4ParserError_NullFileName;
	}

	m_inFileStream.open(m_fileName, ios_base::binary);
	if (!m_inFileStream.is_open())
	{
		return kMP4ParserError_OpenInputFileFailed;
	}

	return kMP4ParserError_NoError;
}

int CMP4Parser::get_input_file_data()
{
	m_inFileStream.seekg(0, ios_base::end);
	m_fileSize = m_inFileStream.tellg();
	m_inFileStream.seekg(0, ios_base::beg);

	m_fileBuf = new BYTE[m_fileSize];
	m_inFileStream.read(reinterpret_cast<char *>(m_fileBuf), m_fileSize);

	if ((m_inFileStream.rdstate() & ifstream::failbit) || (m_inFileStream.rdstate() & ifstream::badbit))
	{
		m_inFileStream.close();
		return kMP4ParserError_ReadInputFileFailed;
	}

	return kMP4ParserError_NoError;
}

void CMP4Parser::dump_mp4_file_info()
{
	cout << "***********************************" << endl;
	cout << "File Name: " << m_fileName << endl;
	cout << "File Size: " << to_string(m_fileSize) << endl;
	cout << "***********************************" << endl;

#if DUMP_MP4_INFO_ENABLED_LOG
	g_logoutFile << "***********************************" << endl;
	g_logoutFile << "File Name: " << m_fileName << endl;
	g_logoutFile << "File Size: " << to_string(m_fileSize) << endl;
	g_logoutFile << "***********************************" << endl;
#endif
}

void CMP4Parser::close_input_file()
{
	if (m_inFileStream.is_open())
	{
		m_inFileStream.close();
	}
}

