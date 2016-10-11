// ConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ParserLib.h"
#include <iostream>
using namespace std;

ofstream timeline;
int videoIdx = 0, audioIdx = 0;

void edit_tag(CFlvTag *tag)
{
	timeline << "-----------------------------" << endl;
	if (tag->Get_tag_type() == TAG_TYPE_VIDEO)
	{
		timeline << "Video Idx: " << videoIdx++ << endl;
		timeline << "Time Stamp: " << to_string(tag->Get_tag_timestamp_ext())<< endl;
	}
	else if (tag->Get_tag_type() == TAG_TYPE_AUDIO)
	{
		if (audioIdx < 1)
		{
			audioIdx++;
			return;
		}
		double actualTime = (audioIdx - 1) * 1024000.0 / 22050.0;
		timeline << "Audio Idx: " << audioIdx++ << endl;
		timeline << "Time Stamp: " << to_string(tag->Get_tag_timestamp_ext()) << endl;
		timeline << "Actual Time: " << to_string(actualTime) << endl;
	}
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

	timeline.open("timeline.txt");
	timeline << argv[1] << endl;

	CFlvWriter writer(argv[2], &parser);
	if (writer.Init(true, true))
	{
		cout << argv[2] << " : " << errorHints[-err] << endl;
		return -1;
	}
//	writer.Clone_FLV_with_video();
	writer.Create_FLV_with_edited_tag(edit_tag);
//	writer.Extract_tags_with_range(0, 10000);

	timeline.close();

    return 0;
}

