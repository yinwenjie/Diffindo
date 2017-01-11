#include "FLVParserLib.h"

int main(int argc, char **argv)
{
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
	
	return 0;
}