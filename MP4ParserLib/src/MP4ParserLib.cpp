#include "MP4ParserLib.h"

using namespace std;

CMP4Parser::CMP4Parser(const char *inputMP4FileName)
{
	m_fileName = inputMP4FileName;
	m_fileSize = 0;
	m_fileBuf = NULL;
	m_bytesCnt = 0;

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
	return 0;
}

