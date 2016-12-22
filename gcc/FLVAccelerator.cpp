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
		return -1;
	}

	double factor = atof(argv[3]);
	CFlvWriter flvWriter(argv[2], &flvParser);
	flvWriter.Init(true, true);
	flvWriter.Clone_with_accelerating_factor(factor);

	return 0;
}