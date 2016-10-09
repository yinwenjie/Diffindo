// ConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ParserLib.h"
#include <iostream>
using namespace std;

void edit_tag(CFlvTag *tag)
{
	return;
}

int main(int argc, char **argv)
{
	int err = 0;
	CFlvParser parser(argv[1]);
	err = parser.Parse();
	if (err < 0)
	{
		cout << argv[1] <<" : " << errorHints[-err] << endl;
		return -1;
	}

	return 0;

	CFlvWriter writer(argv[2], &parser);
	if (writer.Init(true, true))
	{
		cout << argv[2] << " : " << errorHints[-err] << endl;
		return -1;
	}
	writer.Clone_FLV_with_video();
//	writer.Create_FLV_with_edited_tag(edit_tag);
//	writer.Extract_tags_with_range(0, 10000);

    return 0;
}

