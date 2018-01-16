#include <stdio.h>
#include "FLVParserLib.h"
//#include "MP4ParserLib.h"

int main(int argc, char **argv)
{
// 	if (argc < 3)
// 	{
// 		g_mp4Config.mp4Configlevel = 0;
// 	}
// 	else 
// 	{
// 		g_mp4Config.mp4Configlevel = atoi(argv[2]);
// 	}
// 
// 	int err = 0;
// 	CMP4Parser mp4Parser(argv[1]);
// 	err = mp4Parser.Parse();
// 	if (err < 0)
// 	{
// 		return err;
// 	}

	if (argc == 4)
	{
		g_flvConfig.inputName = argv[1];
		g_flvConfig.outputName = argv[2];
		g_flvConfig.flvLogLevel = atoi(argv[3]);
	}
	else if (argc == 3)
	{
		g_flvConfig.inputName = argv[1];
		g_flvConfig.outputName = argv[2];
	}

	int err = 0;
	CFlvParser flvParser(argv[1]);
	err = flvParser.Parse();
	if (err < 0)
	{
		printf("Error code: %d\n", err);
		return -1;
	}

	double videoTSArr[10] = {0, 0, 0.01, 0.05, 0.1, 0.15, 0.19, 0.2, 0.25, 0.27};

	CFlvWriter flvWriter("video.flv", &flvParser);
	flvWriter.Init(true, true);
//	flvWriter.Clone_FLV_with_video();
	flvWriter.Clone_with_new_flv_timestamp_array(videoTSArr, 10, NULL, 0);

	return 0;
}