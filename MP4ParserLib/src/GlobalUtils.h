#ifndef _GLOBAL_UTILS_H_
#define _GLOBAL_UTILS_H_

extern std::ofstream g_logoutFile;//输出日志文件对象

UINT32 Get_lsb_uint32_value(BYTE *buf);

UINT64 Get_lsb_uint64_value(BYTE *buf);

void UINT32_to_fourcc(UINT32 value, char *str);

bool Fourcc_compare(BYTE *buf, char *str);

#endif