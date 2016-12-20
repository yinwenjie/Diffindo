#include "FLVParserLib.h"

int main(int argc, char **argv)
{
	CFlvParser flvParser(argv[1]);
	flvParser.Parse();
	
	CFlvWriter flvWriter("outfile.flv", &flvParser);
	flvWriter.Init(true, true);
	flvWriter.Clone_with_tag_time_stamp_range(0, 5000);
	
	return 0;
}