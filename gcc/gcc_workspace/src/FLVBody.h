#ifndef _FLV_BODY_H_
#define _FLV_BODY_H_

class CFlvTag;
class CFlvBody
{
public:
	CFlvBody();
	~CFlvBody();

	int Parse(BYTE *FileBuf, UINT64 &byteCnt, UINT64 fileSize);
	CFlvTag *Get_first_tag();

private:
	UINT32	m_tagCount;
	UINT32  m_videoTagCount;
	UINT32	m_audiotagCount;

	CFlvTag *m_firstTag;
	CFlvTag *m_recentTag;

	void	deleteTags();
};

#endif