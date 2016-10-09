#ifndef _GLOBAL_UTILS_H_
#define _GLOBAL_UTILS_H_

extern std::ofstream g_logoutFile;

void endian_swap(BYTE *buf, int len);

void read_len_swap(BYTE *dst, BYTE *src, int len);

#endif
