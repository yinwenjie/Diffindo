#include "Include.h"
#include "GlobalUtils.h"

std::ofstream g_logoutFile; //输出日志文件对象

UINT16 Get_lsb_uint16_value(BYTE *buf, UINT64 &offset)
{
	UINT16 val = 0;
	BYTE *payloadBuf = buf + offset;

	val += (payloadBuf[0] << 8);
	val += payloadBuf[1];

	offset += 2;
	return val;
}

UINT32 Get_lsb_uint32_value(BYTE *buf, UINT64 &offset)
{
	UINT32 val = 0;
	BYTE *payloadBuf = buf + offset;
	for (int idx = 0; idx < 4; idx++)
	{
		val += payloadBuf[idx] << (3 - idx) * 8;
	}

	offset += 4;
	return val;
}

UINT64 Get_lsb_uint64_value(BYTE *buf, UINT64 &offset)
{
	UINT32 val = 0;
	BYTE *payloadBuf = buf + offset;
	for (int idx = 0; idx < 8; idx++)
	{
		val += payloadBuf[idx] << (7 - idx);
	}

	offset += 8;
	return val;
}

void UINT32_to_fourcc(UINT32 value, char *str)
{
	UINT32 mask = 0xff000000;
	for (int idx = 0; idx < 4; idx++)
	{
		UINT32 cvalue = value & (mask >> 8 * idx);
		cvalue >>= 8 * (3 - idx);
		str[idx] = cvalue;
	}
	str[4] = 0;
}

bool Fourcc_compare(BYTE *buf, const char *str)
{
	for (int idx = 0; idx < 4; idx++)
	{
		if (buf[idx] != str[idx])
		{
			return false;
		}
	}

	return true;
}
