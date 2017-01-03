#include <stdio.h>
#include "FLVParserLib.h"

int main(int argc, char **argv)
{
	int err = 0;
	CFlvParser flvParser(argv[1]);
	err = flvParser.Parse();
	if (err < 0)
	{
		printf("Error code: %d\n", err);
		return -1;
	}

	char* funcIdx = argv[3];
	if (!strcmp(funcIdx, "-v"))
	{
		CFlvWriter flvWriter(argv[2], &flvParser);
		flvWriter.Init(true, true);
		flvWriter.Clone_FLV_with_video();
	}
	else if (!strcmp(funcIdx, "-idx_range"))
	{
		CFlvWriter flvWriter(argv[2], &flvParser);
		flvWriter.Init(true, true);
		UINT32 startIdx = atoi(argv[4]);
		UINT32 endIdx = atoi(argv[5]);
		flvWriter.Clone_with_tag_index_range(startIdx, endIdx);
	}
	else if (!strcmp(funcIdx, "-ts_range"))
	{
		CFlvWriter flvWriter(argv[2], &flvParser);
		flvWriter.Init(true, true);
		UINT32 startTS = atoi(argv[4]);
		UINT32 endTS = atoi(argv[5]);
		flvWriter.Clone_with_tag_time_stamp_range(startTS, endTS);
	}
	else if (!strcmp(funcIdx, "-raw_h264"))
	{
		CFlvWriter flvWriter(argv[2], &flvParser);
		flvWriter.Extract_H264_raw_stream();
	}

	return 0;
}