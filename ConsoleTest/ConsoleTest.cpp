// ConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ParserLib.h"
#include <iostream>
using namespace std;

ofstream timeline;
int videoIdx = 0, audioIdx = 0;
double videoFrameGap = 1000.0 / 23.985, audioTagDuration = 1024000.0 / 22050.0;
UINT32 lastVideoTimeStamp = 0, lastAudioTimeStamp = 0;
bool lastTagAudio = false;

void edit_tag(CFlvTag *tag)
{
	if (tag->Get_tag_index() <= 5000)
	{
		if (tag->Get_tag_type() == TAG_TYPE_VIDEO)
		{
			lastVideoTimeStamp = tag->Get_tag_timestamp_ext();
		}
		else if (tag->Get_tag_type() == TAG_TYPE_AUDIO)
		{
			lastAudioTimeStamp = tag->Get_tag_timestamp_ext();
		}

		return;
	}

	timeline << "-----------------------------" << endl;
	if (tag->Get_tag_type() == TAG_TYPE_VIDEO)
	{
		timeline << "Video Idx: " << videoIdx++ << endl;
		timeline << "Time Stamp: " << to_string(tag->Get_tag_timestamp_ext())<< endl;

		if (lastTagAudio)
		{
			tag->Set_tag_timestamp((UINT32)lastAudioTimeStamp + 15);
		}
		else
		{
			UINT32 thisTimeStamp = ((lastVideoTimeStamp + videoFrameGap) < (lastAudioTimeStamp + audioTagDuration)) ? (lastVideoTimeStamp + videoFrameGap) : (lastAudioTimeStamp + audioTagDuration);
			tag->Set_tag_timestamp(thisTimeStamp);
		}
		lastVideoTimeStamp = tag->Get_tag_timestamp_ext();
		lastTagAudio = false;
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
		timeline << "Time Stamp:  " << to_string(tag->Get_tag_timestamp_ext()) << endl;
//		timeline << "Actual Time: " << to_string(actualTime) << endl;

//		tag->Set_tag_timestamp((UINT32)actualTime);

		lastAudioTimeStamp = tag->Get_tag_timestamp_ext();
		lastTagAudio = true;
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
//	writer.Extract_tags_with_range(10, 1778);

	timeline.close();

    return 0;
}

