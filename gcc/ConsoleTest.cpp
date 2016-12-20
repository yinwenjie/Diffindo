#include "FLVParserLib.h"

int main(int argc, char **argv)
{
	CFlvParser flvParser(argv[1]);
	flvParser.Parse();
	
	CFlvWriter flvWriter("outfile.flv", &flvParser);
	flvWriter.Init(true, true);
	flvWriter.Clone_FLV_with_video();
	
	return 0;
}