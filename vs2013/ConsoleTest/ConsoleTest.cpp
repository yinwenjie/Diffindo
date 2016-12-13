#include <stdio.h>
#include "FLVParserLib.h"

int main(int argc, char **argv)
{
	CFlvParser flvParser(argv[1]);
	flvParser.Parse();
	return 0;
}