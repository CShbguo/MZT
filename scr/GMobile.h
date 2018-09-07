#ifndef _GMOBILE_HEAD_FILE_
#define _GMOBILE_HEAD_FILE_
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "Gszct.h"
#if 0
extern stcomtime SysTime;
extern unsigned char DriveCardNo[4];
//extern stLcdCmd gLcdCmd;

extern unsigned char gmissOldflag;//上个任务的任务标志 0x00 登陆移动,签到.
extern unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.
//0x01 下载参数
//0x02 程序下载请求
//0x03 程序下载结果通知
//0x04 批结算
//0x05 批上送交易
//0xF0 登陆公交后台
//0xF1 下载黑名单
stMobileParameter gMobileParameter;//移动中心参数
stMobilStyle gMobilStyle;//签到状态，包括其它任务信息，KEK 程序 参数下载状态，开机需检测
extern unsigned short gSendOverTime;//发送后收应答超时计数器
unsigned int gMobilPOSNS;//收单方终端系统跟踪号（POS流水号）
unsigned int gMobilREC_Start;//记录指针头
unsigned int gMobilREC_End;//移动指针尾

//unsigned int gMobilSendPoint;//要发送的记录指针
//unsigned int gMobilSendPointBAK;//备份
//unsigned int gMTrad63MSG1BAK;
//unsigned int gMTrad63MSG2BAK;

//unsigned int gMTrad63MSG1;//消费总次数
//unsigned int gMTrad63MSG2;//消费总金额
stdownProgPara downProgPara;//程序下载任务情况

unsigned short g8583ACK;//8083包的应答码

unsigned char gmobileProVer[14];//移动程序版本，从程序存贮器取出

unsigned char gmissNotTrad;//此变量指示是否在做批传,现已经没有使用

unsigned int gMobilConnectErr;//连移动平台错误。如是0没错误，否刚是错误

unsigned int gSendSeril;//上传移动数据时的序列号。
unsigned int gmobileBakSeril;//服务器的跟踪号
unsigned char gseflag;

stMobilSend mobilsendStyle;
unsigned char gM_Senddll;//延时传输

unsigned char gmobileErrTimes;//移动连接错误次数据,包括应答.

unsigned char gmobileHeadFlag;//移动回应包是否先发总长度
#endif
#endif
