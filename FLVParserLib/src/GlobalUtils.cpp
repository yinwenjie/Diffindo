#include "Include.h"
#include "GlobalUtils.h"

# if DUMP_TAG_INFO_ENABLED_LOG
std::ofstream g_logoutFile; //输出日志文件对象

void Read_data_lsb(void *dst, void *src, int length)
{
	for (int idx = 0; idx < length; idx++)
	{
		*(reinterpret_cast<BYTE*>(dst) + idx) = *(reinterpret_cast<BYTE*>(src) + length - idx - 1);
	}
}

#endif