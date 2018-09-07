

/**************************************************
* 文件名 :SelMessage.c
* 描述   :发送短消息文件
*		后台收到记录后，会发送一个短信命令到车载机。然后车载机把信息发给命令指定的手机号。
* 建立   :26/6/2012	by KSLi
**************************************************/
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"
#include "szct.h"
#include "Mydes.h"

extern unsigned char gmissOldflag;
extern unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.
extern void gprs_snd(unsigned int len, char *dat);

#ifdef SelMsg_enable

//发送短信息变量，不保存，关机后丢失。
stSelMsg gsendMsg;

unsigned char getFeMSGaddr(unsigned char mode)
{
	unsigned char i;
	unsigned int crc32;
	unsigned char buff[128];
	
	for(i=0; i<32; i++){
		sysferead(BIT_GPSDiaoDuCMD_INFO+(i*32), 32, buff);
		crc32 = __cpuCrc32(buff, 28);
		if(mode == 0){//找空位
			if(memcmp((unsigned char*)&crc32, buff+28, 4) != 0){//空位
				break;
			}
		}
		else{
			if(memcmp((unsigned char*)&crc32, buff+28, 4) == 0){//有数据
				break;
			}
		}
	}
	return i;
}

//初始化短信息变量
void SelMsgInit(void)
{
	memset((unsigned char*)&gsendMsg, 0,  sizeof(stSelMsg));

}
void debugSelMsg(void)
{
	debugdata((unsigned char*)&gsendMsg, sizeof(stSelMsg), 1);
}

//显示没有传的短信
void disUNMsg(void)
{
	stSaveMSG sams;
	char disbuf[32][20];
	unsigned char index;
	unsigned char i;
	unsigned int crc32;

	index = 0;
	for(i=0; i<32; i++){
		sysferead(BIT_GPSDiaoDuCMD_INFO+(i*32), 32, (unsigned char*)&sams);
		crc32 = __cpuCrc32((unsigned char*)&sams, 28);
		if(memcmp((unsigned char*)&crc32, sams.CrC, 4) == 0){//有数据
			memset(disbuf[index], 0, 20);
			sprintf(disbuf[index], "%02d:", index);
			strcat(disbuf[index], sams.SellPhoneNo);
// 			strcat(disbuf[index], ":");
// 			BCD2Ascii(sams.datatime+1, (unsigned char*)disbuf[index]+strlen(disbuf[index]), 5);
			index++;
		}
	}
	if(index == 0){
		MessageBox(0, "没有要发送的短信!");
		beep(3, 50, 50);
		delayxs(1);
	}
	else
		dispageInfo(index, disbuf);
}

int findSelMsg(void)
{
	stSaveMSG sams;
	unsigned char t;
	unsigned char index, len;

	if(gsendMsg.TLL > 0){//多次发不成功，延时后发送
#ifdef _debug_
		debugstring("fs0:");
		debugdata((unsigned char*)&gsendMsg.TLL, 1, 0);
#endif
		return 1;
	}
	if(gsendMsg.Style == SELPHONE_SENDED){
#ifdef _debug_
		debugstring("findSelMsg del:");
		debugdata((unsigned char*)&gsendMsg.index, 1, 1);
#endif
		if(gsendMsg.index < 32){
			memset((unsigned char*)&sams, 0xFF, sizeof(stSaveMSG));
			sysfewrite(BIT_GPSDiaoDuCMD_INFO+(gsendMsg.index*32), 32, (unsigned char*)&sams);
		}
		memset((unsigned char*)&gsendMsg, 0,  sizeof(stSelMsg));
	}
	else if(gsendMsg.Style == SELPHONE_valid){//发送区有短信发送
		return 0;
	}

	index = getFeMSGaddr(1);
	if(index >= 32){//没有短信
		return -1;
	}
#ifdef _debug_
	debugstring("findSelMsg1 :");
	debugdata((unsigned char*)&index, 1, 1);
#endif
	sysferead(BIT_GPSDiaoDuCMD_INFO+(index*32), 32, (unsigned char*)&sams);
#ifdef _debug_
	debugstring("findSelMsg2 :");
	debugdata((unsigned char*)&sams, 32, 1);
#endif
		
	gsendMsg.Style = SELPHONE_valid;
	strcpy(gsendMsg.SellPhoneNo, sams.SellPhoneNo);

	len = 7;
	for(t=0; t<7; t++){ //日期时间
		gsendMsg.SendMsg[t*4] = 0;
		gsendMsg.SendMsg[(t*4)+1] = (sams.datatime[t]>>4)+'0';
		gsendMsg.SendMsg[(t*4)+2] = 0;
		gsendMsg.SendMsg[(t*4)+3] = (sams.datatime[t]&0x0F)+'0';
	}
	len *= 4;
	//	memcpy(gSelMsg[i].SendMsg, msg, len);
#ifdef BAOSHI_LM
	memcpy(gsendMsg.SendMsg+len, "\x62\xC9\x71\x64\x8F\x66\x90\x1A\x8F\xC7\x00\x2E", 12);//
#else
	memcpy(gsendMsg.SendMsg+len, "\x62\xC9\x57\x1F\x8F\x66\x90\x1A\x8F\xC7\x00\x2E", 12);//
#endif
	len += 12;
	gsendMsg.len = len;
	gsendMsg.times = 0;
	gsendMsg.TLL = 0;
	gsendMsg.index = index;
#ifdef _debug_
	debugstring("findSelMsg3 :");
	debugdata((unsigned char*)&gsendMsg, sizeof(stSelMsg), 1);
#endif
	return 0;
}

void addSelMsg(char *SelNo, char *msg, unsigned char len)
{
	stSaveMSG sams;
	unsigned char index;
	unsigned int crc32;

	index = getFeMSGaddr(0);
	if(index >= 32)//已经满了
		index = 0;

	sams.style = SELPHONE_valid;
	memset(sams.SellPhoneNo, 0, sizeof(sams.SellPhoneNo));
	if(strlen(SelNo) < 14)
		strcpy(sams.SellPhoneNo, SelNo);
	else
		memcpy(sams.SellPhoneNo, SelNo, 13);
	memcpy(sams.datatime, msg, 7);
	crc32 = __cpuCrc32((unsigned char*)&sams, 28);
	memcpy(sams.CrC, (unsigned char*)&crc32, 4);
	sysfewrite(BIT_GPSDiaoDuCMD_INFO+(index*32), 32, (unsigned char*)&sams);
}
#endif
//AT^HCMGS="13590292897"
//>  发送内容
//^HCMGSS:36	//成功后返回
void sendmsgHead(void)
{
#ifdef SelMsg_enable
	char sndbuf[64];

	strcpy(sndbuf, "AT^HCMGS=\"");
	if(gsendMsg.times > 5){//号码出错,发送次数太多
		gsendMsg.TLL = 120;//延迟2分钟
		gmissflag = 0;
		return;
	}
	strcat(sndbuf, gsendMsg.SellPhoneNo);
	strcat(sndbuf, "\"\r\n");
	gprs_snd(strlen(sndbuf), sndbuf);
	gsendMsg.times++;
#else
	gmissflag = 0;
#endif
}

void sendmsgBody(void)
{
#ifdef SelMsg_enable
	char sndbuf[150];
	memset(sndbuf, 0, 150);
	memcpy(sndbuf, gsendMsg.SendMsg, gsendMsg.len);
	memcpy(sndbuf+gsendMsg.len, "\x00\x1A", 2);
	debugstring("sned msg body:");
	debugdata((unsigned char*)sndbuf, gsendMsg.len+2, 1);
	gprs_snd(gsendMsg.len+2, sndbuf);
	gmissflag = MISS_G_MSG2;
#else
	gprs_snd(2, "\x00\x30\x00\x1A");
#endif
}

void SendmsgEnd(void){
#ifdef SelMsg_enable
	gmissflag = 0;
	gsendMsg.Style = SELPHONE_SENDED;//已经发送，做个标记然后在FIND中删除
#else
	;
#endif
}



