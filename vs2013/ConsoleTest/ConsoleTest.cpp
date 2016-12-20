#include <stdio.h>
#include "FLVParserLib.h"

int main(int argc, char **argv)
{
	int err = 0;
	CFlvParser flvParser(argv[1]);
	err = flvParser.Parse();
	if (err = 0)
	{
		printf("Error code: %d\n", err);
	}

	CFlvWriter flvWriter("outfile.flv", &flvParser);
	flvWriter.Init(true, true);
//	flvWriter.Clone_FLV_with_video();
//	flvWriter.Clone_with_tag_index_range(0, 500);
	flvWriter.Clone_with_tag_time_stamp_range(0, 5000);

	return 0;
}