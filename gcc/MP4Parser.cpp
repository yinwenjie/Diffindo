#include <stdio.h>
#include "MP4ParserLib.h"

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		g_mp4Config.mp4Configlevel = 0;
	}
	else 
	{
		g_mp4Config.mp4Configlevel = atoi(argv[2]);
	}
	
	int err = 0;
	CMP4Parser mp4Parser(argv[1]);
	err = mp4Parser.Parse();
	if (err < 0)
	{
		return err;
	}

	return 0;
}