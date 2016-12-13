// FLVParserLib.cpp : 定义 DLL 应用程序的导出函数。
//
#include "Include.h"
#include "FLVParserLib.h"

using namespace std;

CFlvParser::CFlvParser(const char *fileName)
{
	if (!fileName)
	{
		m_fileName = fileName;
	}

	m_fileSize = 0;
	m_fileBuf = nullptr;
	m_bytesCnt = 0;

#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.open("log.txt");
	g_logoutFile << fileName << endl;
#endif

}

CFlvParser::~CFlvParser()
{
#if DUMP_TAG_INFO_ENABLED_LOG
	g_logoutFile.close();
#endif
}
