#include <stdio.h>
#include "FLVParserLib.h"
#include "MP4ParserLib.h"

int main(int argc, char **argv)
{
	int err = 0;
	CMP4Parser mp4Parser(argv[1]);
	err = mp4Parser.Parse();
	if (err < 0)
	{
		return err;
	}

	return 0;
}