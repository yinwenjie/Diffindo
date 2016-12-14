#ifndef _GLOBAL_UTILS_H_
#define _GLOBAL_UTILS_H_

#include "Configuration.h"

# if DUMP_TAG_INFO_ENABLED_LOG
extern std::ofstream g_logoutFile;//输出日志文件对象
#endif

void Read_data_lsb(void *dst, void *src, int length);

#endif