#ifndef _GLOBAL_UTILS_H_
#define _GLOBAL_UTILS_H_

extern std::ofstream g_logoutFile;//输出日志文件对象

/*
函数: endian_swap
作用：内存字节翻转
参数：
	buf: 待处理数据缓存指针；
	len: 翻转数据的长度；
返回值：无
*/
void endian_swap(BYTE *buf, int len);

/*
函数: read_len_swap
作用：从源地址读取数据到目标地址，并翻转字节序
参数：
	src: 源数据地址；
	dst: 目标数据地址
	len: 翻转数据的长度；
返回值：无
*/
void read_len_swap(BYTE *dst, BYTE *src, int len);

#endif
