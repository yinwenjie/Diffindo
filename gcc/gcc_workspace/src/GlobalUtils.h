#ifndef _GLOBAL_UTILS_H_
#define _GLOBAL_UTILS_H_

#include "Configuration.h"

# if DUMP_TAG_INFO_ENABLED_LOG
extern std::ofstream g_logoutFile;//输出日志文件对象
#endif

void Read_data_lsb(void *dst, void *src, int length);
void endian_swap(BYTE *buf, int len);

// Get bool value from bit position..
int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);
// Parse bit stream using Expo-Columb coding
int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition);

#endif