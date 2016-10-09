#ifndef _FLV_BODY_H_
#define _FLV_BODY_H_
#include <vector>

class CFlvTag; 

class CFlvBody
{
public:
	CFlvBody();
	~CFlvBody();
	
	int		Parse(BYTE *FileBuf, UINT64 &byteCnt, UINT64 fileSize);
	void	Dump_flv_tag_info();

	CFlvTag* Get_first_tag();
	CFlvTag* Get_script_data_tag();

private:
	CFlvTag *m_firstTag;
	CFlvTag *m_recentTag;
	UINT32	m_tagCount;
	UINT32  m_videoTagCount;
	UINT32	m_audiotagCount;

	std::vector<UINT32> m_prevTagSizeVec;

	void	deleteTags();
};

#endif
