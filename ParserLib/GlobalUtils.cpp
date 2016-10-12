#include "stdafx.h"
#include "GlobalUtils.h"

# if DUMP_TAG_INFO_ENABLED_LOG
std::ofstream g_logoutFile; //输出日志文件对象
#endif

// 内存字节翻转
void endian_swap(BYTE *buf, int len)
{
	for (int idx = 0; idx < len / 2; idx++)
	{
		BYTE temp = buf[idx];
		buf[idx] = buf[len - idx - 1];
		buf[len - idx - 1] = temp;
	}
}

// 从源地址读取数据到目标地址，并翻转字节序
void read_len_swap(BYTE *dst, BYTE *src, int len)
{
	memcpy_s(dst, len, src, len);
	endian_swap(dst, len);
}

