#ifndef _FLV_BODY_H_
#define _FLV_BODY_H_

class CFlvBody
{
public:
	CFlvBody();
	~CFlvBody();

	int Parse(BYTE *FileBuf, UINT64 &byteCnt, UINT64 fileSize);

private:
	UINT32	m_tagCount;
	UINT32  m_videoTagCount;
	UINT32	m_audiotagCount;
};

#endif