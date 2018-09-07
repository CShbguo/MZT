#ifndef _STYIDAKEY_HEAD_
#define _STYIDAKEY_HEAD_
//艺达的密钥算法
extern int calc_key(unsigned char *s0_data,unsigned char *key_data);
//计算M1 keyA
//S0 keyA  A0A1A2A3A4A5 , 读出S0B0,S0B1,S0B2  共48字节, 代入函数
// 输入: s0_data , 16x3=48
// 输出:S0-S9 keyA, 每6字节为一个key, 6x10=60
#endif
