#include "FLVParserLib.h"

int main(int argc, char **argv)
{
if (argc == 3)
	{
		g_config.flvLogLevel = atoi(argv[2]);
	}

	int err = 0;
	CFlvParser flvParser(argv[1]);
	err = flvParser.Parse();
	if (err = 0)
	{
		printf("Error code: %d\n", err);
		return -1;
	}
	
	return 0;
}