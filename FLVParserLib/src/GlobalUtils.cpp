#include "Include.h"
#include "GlobalUtils.h"

# if DUMP_TAG_INFO_ENABLED_LOG
std::ofstream g_logoutFile; //输出日志文件对象
#endif

void Read_data_lsb(void *dst, void *src, int length)
{
	for (int idx = 0; idx < length; idx++)
	{
		*(reinterpret_cast<BYTE*>(dst) + idx) = *(reinterpret_cast<BYTE*>(src) + length - idx - 1);
	}
}

// Get bool value from bit position..
int Get_bit_at_position(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition)
{
	UINT8 mask = 0, val = 0;

	mask = 1 << (7 - bitPosition);
	val = ((buf[bytePosition] & mask) != 0);
	if (++bitPosition > 7)
	{
		bytePosition++;
		bitPosition = 0;
	}

	return val;
}

// Parse bit stream using Expo-Columb coding
int Get_uev_code_num(UINT8 *buf, UINT8 &bytePosition, UINT8 &bitPosition)
{
	UINT8 val = 0, prefixZeroCount = 0;
	int prefix = 0, surfix = 0;

	while (true)
	{
		val = Get_bit_at_position(buf, bytePosition, bitPosition);
		if (val == 0)
		{
			prefixZeroCount++;
		}
		else
		{
			break;
		}
	}
	prefix = (1 << prefixZeroCount) - 1;
	for (size_t i = 0; i < prefixZeroCount; i++)
	{
		val = Get_bit_at_position(buf, bytePosition, bitPosition);
		surfix += val * (1 << (prefixZeroCount - i - 1));
	}

	prefix += surfix;

	return prefix;
}

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
