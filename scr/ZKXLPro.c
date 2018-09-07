/*
//中科讯联2.4G读头测试（演示）程序
只是用默认密钥消费（充值）
*/
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "..\include\include.h"
#include "szct.h"

#ifdef ZKXL_24G_

#define ZKXL_HEAD 0
#define ZKXL_CMD 1
#define ZKXL_LEN 2
#define ZKXL_DATA 4

extern void Sound_cityCMD(unsigned char sMode);
extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned char GetDateTime(void);
extern void debugstring(char *str);
extern void debugdata(unsigned char *value, unsigned int uclen, unsigned char mode);
extern unsigned int get_s_sum1(unsigned char mode);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void SoundMessage(unsigned char cmd);
extern void error(unsigned char code1,unsigned char Index);
extern void SetTcpPacketTTLTime(unsigned char val);

extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char restore_flag;
extern unsigned int g24GDisFlash;//24G卡界面刷新
extern unsigned char cardSound;			//刷卡没有成功标志
extern unsigned int card_ser;
extern stcomtime SysTime;
extern unsigned int a_sumR;//手机钱包中的真实金额
extern unsigned int a_sum1,s_sum1;
extern Parameter5 cardMessage;
extern unsigned char gMCardCand;
extern unsigned int c_serial;
unsigned char gcomBuf[256];
volatile unsigned short gComindex;
volatile unsigned char flag;
unsigned int gZKXLdly;
typedef struct
{
	unsigned char cBalance[4];			/* 电子钱包余额 */
	unsigned char cSnq[2];				/* 电子钱包交易序列号 */
	unsigned char cOverdraftLimit[3];	/* 透支限制 */
	unsigned char cKeyVer;				/* 密钥版本 */
	unsigned char cKeyEn;				/* 密钥标识 */
	unsigned char cRand[4];				/* 随机数 */
}pboc_init_capp_purchase_res;

typedef struct
{
	unsigned char cBalance[4];			/* 电子钱包余额 */
	unsigned char cSnq[2];				/* 电子钱包交易序列号 */
	unsigned char cKeyVer;				/* 密钥版本 */
	unsigned char cKeyEn;				/* 密钥标识 */
	unsigned char cRand[4];				/* 随机数 */
	unsigned char cMac1[4];				// MAC1
}pboc_init_capp_load_res;

typedef struct{
	unsigned char gSerila[8];//EUI
	unsigned int a_sum;
	unsigned int s_sum;
}stcardinfo;
stcardinfo gCardinfo;

void comParaInit(void)
{
	gComindex = 0;
	flag = 0;
	gZKXLdly = 0;
}

void ZKXL_Hand(unsigned char vl)
{
	unsigned short len;

//	debugdata((unsigned char*)&vl, 1, 0);
	if(gComindex == 0){
		if(vl != 0x5A)
			return;
	}
	if(flag == 0x5A){//上个数据包没处理完
		comParaInit();
//		debugstring("*");
		return;
	}

	gcomBuf[gComindex++] = vl;
	if(gComindex > 4){//已经收完了长度，可以判断是否收完所有数据
		memcpy((unsigned char*)&len, gcomBuf+ZKXL_LEN, 2);	
		over_turn(2, (unsigned char*)&len);
		if(len > 256)
		{
			comParaInit();
		}
		else if(gComindex >= (len+6)){//收完成
// 			debugdata(gcomBuf, gComindex, 0);
// 			debugstring("|");
			flag = 0x5A;
		}
	}
}

unsigned char sndcarddat(unsigned char cmd, unsigned char *dat, unsigned short len)
{
	unsigned char buff[256];
	unsigned short i;
	
	comParaInit();

	i = 0;
	buff[i++] = 0x5A;
	buff[i++] = cmd;
	buff[i++] = (len)/256;
	buff[i++] = (len)%256;
	if(len > 0){
		memcpy(buff+i, dat, len);
		i+=len;
	}
	buff[i++] = 0xCA;

#ifdef _debug_
	debugstring("snddat:");
	debugdata(buff, i, 1);
#endif

	com_snd(G24Com, i, buff);
	return 0;
}
//发送命令，要等应答
unsigned short sndCmd(unsigned char *dat, unsigned short len, unsigned char *rev)
{
	unsigned short us;
	unsigned int dlyo = 0;

	sndcarddat(0x20, dat, len);//发送命令
	while(1){//等应答
		if(flag == 0x5A){
#ifdef _debug_
			debugstring("sndCmdR:");
 			debugdata(gcomBuf, gComindex, 1);
#endif
			memcpy((unsigned char*)&us, gcomBuf+ZKXL_LEN, 2);
			over_turn(2,(unsigned char*)&us);
			memcpy(rev, gcomBuf+ZKXL_DATA, us);
			if((us==3)&&(rev[2]==8)){//命令正在执行
				comParaInit();
			}
			else{
				comParaInit();
				return us;
			}
		}
		delayxms(1);
		if(dlyo++ > 500){
#ifdef _debug_
			debugstring("s o:");
#endif
			return 0;
		}
	}
}

unsigned char sndcarddat40(unsigned char cmd, unsigned char *dat, unsigned short len)
{
	unsigned char buff[256];
	unsigned short i;
	unsigned char t;
	
	comParaInit();
	
	i = 0;
	buff[i++] = 0x5A;
	buff[i++] = cmd;
	buff[i++] = (len)/256;
	buff[i++] = (len)%256;
	if(len > 0){
		memcpy(buff+i, dat, len);
		i+=len;
	}
	buff[i] = 0;//XOR CMD to DAT
	for(t=0; t<len+3; t++){
		buff[i] ^=  buff[t+1];
	}

	i++;
	buff[i++] = 0xCA;
	
#ifdef _debug_
	debugstring("snddat:");
	debugdata(buff, i, 1);
#endif
	
	com_snd(G24Com, i, buff);
	return 0;
}
//发送命令，要等应答
unsigned short sndCmd40(unsigned char *dat, unsigned short len, unsigned char *rev, unsigned char cmd)
{
	unsigned short us;
	unsigned int dlyo=0;
	
	sndcarddat40(cmd, dat, len);//发送命令
	
	while(1){//等应答
		if(flag == 0x5A){
#ifdef _debug_
			debugstring("sndCmdR:");
			debugdata(gcomBuf, gComindex, 1);
#endif
			memcpy((unsigned char*)&us, gcomBuf+ZKXL_LEN, 2);
			over_turn(2,(unsigned char*)&us);
			memcpy(rev, gcomBuf+ZKXL_DATA, us);
			if((us==3)&&(rev[2]==8)){//命令正在执行
				comParaInit();
				dlyo = 0;
			}
			else{
				comParaInit();
				return us;
			}
		}
		delayxms(1);
		if(dlyo++ > 500){
#ifdef _debug_
			debugstring("s o:");
#endif
			return 0;

		}
	}
}

//5a f1 00 00 ca                             FF 15 查SIM卡版本
void checkSimVer(void)
{
	unsigned short us;
	unsigned char dat[20];
	unsigned int dlyo = 0;
	
	sndcarddat(0xF1, dat, 0);//发送命令
	while(1){//等应答
		if(flag == 0x5A){
#ifdef _debug_
			debugstring("sndCmdR:");
			debugdata(gcomBuf, gComindex, 1);
#endif
			memcpy((unsigned char*)&us, gcomBuf+ZKXL_LEN, 2);
			over_turn(2,(unsigned char*)&us);
			memcpy(dat, gcomBuf+ZKXL_DATA, us);//5A200003FF1500CA
			if((us==3)&&(dat[2]==8)){//命令正在执行
				comParaInit();
				dlyo=0;
			}
			else{
				comParaInit();
				break;
			}
		}
		if(dlyo++ > 500){
#ifdef _debug_
			debugstring("s o:");
#endif
			return;
		}
	}
}
unsigned char cmdDeal(void)
{
	unsigned char snddat[256];
	unsigned short i;
	if(gcomBuf[1] == 0xAA){//有手机卡进入
		memcpy((unsigned char*)&i, gcomBuf+2, 2);
		over_turn(2, (unsigned char*)&i);
		if(i > 8)
			i = 8;
		memset(gCardinfo.gSerila, 0, 8);
		memcpy(gCardinfo.gSerila, gcomBuf+4, i);
		
		memcpy(snddat, "\x90\x00\x00", 3);
		sndcarddat(0xAA, snddat, 3);//发送应答 
		return 1;
	}
	else if(gcomBuf[1] == 0x10){//4.0版返回询卡
		if(gcomBuf[3] == 9){
			memset(gCardinfo.gSerila, 0, 8);
			memcpy(gCardinfo.gSerila, gcomBuf+4, 8);
#ifdef _debug_
			debugstring("卡号:");
			debugdata(gCardinfo.gSerila, 8, 1);
#endif
			return 1;//有卡号
		}
	}
	return 0;
}
void SLXK_endCard(void)
{
	unsigned int dlyo =0;
	unsigned char snddd[16];
	comParaInit();
	while(1){
		memcpy(snddd, "\x5A\x10\x00\x01\x0A\x1B\xCA", 7);//5A 10 05 14 CA 5A 10 05 14 CA
		com_snd(G24Com, 7, snddd);
		delayxms(150);
		//debugstring("END Card::");
		if((flag == 0x5A)&&(gcomBuf[3] == 9)){
			comParaInit();
			dlyo = 0;
		}
		if(dlyo++ > 5)
			return;
	}
}

//V3.4协议
void CPUPuchace34(void)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	unsigned int i;
	pboc_init_capp_purchase_res stuInitPurchaseRet;
	//wangzhiyong@xunfang.com

//	checkSimVer()

	for(i=0; i<3; i++){
		memcpy(snddat, "\x00\x70\x00\x00\x01", 5);//打开通道
		ret = sndCmd(snddat, 5, revbuff);
		if(ret == 0){
			return;
		}
#ifdef _debug_
		debugstring("open channel:");
		debugdata(revbuff, ret, 1);
#endif
		if(memcmp(revbuff, "\x01\x90\x00", 2) == 0){//通道被OK
			break;
		}
		else{
			memcpy(snddat, "\x00\x70\x80\x01\x00", 5);//关闭通道1
			ret = sndCmd(snddat, 5, revbuff);
			if(ret == 0){
				return;
			}
#ifdef _debug_
			debugstring("close channel:");
			debugdata(revbuff, ret, 1);
#endif
		}
	}

	if(i >= 3)
		return;

	GetDateTime();
	memcpy(snddat, "\x01\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);//Select PSE
	ret = sndCmd(snddat, 19, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("Select 34 PSE:");
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd(snddat, 5, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("取响应:");//6F 19 84 0E 31 50 41 59 2E 53 59 53 2E 44 44 46 30 31 A5 04 9F 08 01 01 88 01 15 90 00 00 
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	
	memcpy(snddat, "\x01\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 14);// Select AID
	ret = sndCmd(snddat, 14, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("Select AID:");
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd(snddat, 5, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("取响应:");//6F328409A00000000386980701A5049F0801019F0C1E6E56531794F68054010107170000000000000001FFFFFFFFFFFFFFFF0000900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}

	gCardinfo.s_sum = 1;

//	memcpy(snddat, "\x81\x50\x01\x02\x0B\x03\x00\x00\x03\xE8\x00\x00\x00\x00\x00\x01", 16); // 消费初始化
	memcpy(snddat, "\x81\x50\x01\x02\x0B\x03\x00\x00\x00\x01\x01\x23\x45\x67\x89\x01", 16); // 消费初始化
	memcpy(snddat+6, (unsigned char*)&gCardinfo.s_sum, 4);
	over_turn(4, snddat+6);
	ret = sndCmd(snddat, 16, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("消费初始化:");//610F00
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd(snddat, 5, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("初始化响应:");//00000064 0000 000000 01 82 538EC13F 900000
							   //00000064 0000 000000 01 82 7CA3576D 900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	memcpy((unsigned char*)&stuInitPurchaseRet, revbuff, sizeof(pboc_init_capp_purchase_res));

	//计算消费时MAC1
	memset(revbuff, 0xff, 20);//KEY
	memcpy(revbuff+20, stuInitPurchaseRet.cRand, 4);//随机数
	memcpy(revbuff+24, stuInitPurchaseRet.cSnq, 2);
	memcpy(revbuff+26, "\x00\x01", 2);


	memcpy(snddat, (unsigned char*)&gCardinfo.s_sum, 4);
	over_turn(4, snddat);
	snddat[4] = 0x06;
	memcpy(snddat+5, "\x01\x23\x45\x67\x89\x01", 6);
	memcpy(snddat+11, (unsigned char*)&SysTime, 7);//共18
#ifdef _debug_
	debugstring("revbuff::");
	debugdata(revbuff, 28, 1);

	debugstring("MACdat::");
	debugdata(snddat, 18, 1);
#endif
	memset(revbuff+64, 0, 16);
	Mac(revbuff, revbuff+20, snddat, 18, revbuff+64, revbuff+100);
#ifdef _debug_
	debugstring("MAC::");
	debugdata(revbuff+100, 4, 1);
#endif
	memcpy(snddat, "\x81\x54\x01\x00\x0F\x00\x00\x00\x01", 9); // 消费 终端序列号
	memcpy(snddat+9, (unsigned char*)&SysTime, 7);//交易日期时间
	memcpy(snddat+16, revbuff+100, 4);//MAC1

	ret = sndCmd(snddat, 20, revbuff);//5A200014815401000F00000001201201131553374F0A0814CA
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("消费:");//610F00
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd(snddat, 5, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("消费响应:");//00000064 0000 000000 01 82 538EC13F 900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	
	//815C 0002 04 // 查询余额
	memcpy(snddat, "\x81\x5C\x00\x02\x04", 5);
	ret = sndCmd(snddat, 5, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("余额:");
	debugdata(revbuff, ret, 1);//00000064900000
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	memcpy((unsigned char*)&gCardinfo.a_sum, revbuff, 4);
	over_turn(4, (unsigned char*)&gCardinfo.a_sum);

#ifdef _debug_
	debugstring("消费完成:\r\n");
	sprintf((char*)revbuff, "余额:%d.%02d\r\n", gCardinfo.a_sum/100, gCardinfo.a_sum%100);
	debugstring((char*)revbuff);
	sprintf((char*)revbuff, "扣款:%d.%02d\r\n", gCardinfo.s_sum/100, gCardinfo.s_sum%100);
	debugstring((char*)revbuff);
#endif

	cls();
	display(0,0,"消费结果:",0);
	sprintf((char*)revbuff, "余额:%d.%02d", gCardinfo.a_sum/100, gCardinfo.a_sum%100);
	display(2,2,(char*)revbuff, 0);
	sprintf((char*)revbuff, "扣款:%d.%02d", gCardinfo.s_sum/100, gCardinfo.s_sum%100);
	display(4,2,(char*)revbuff, 0);
	audio_old(1);
	
	sprintf((char*)revbuff, "%d.%02d", gCardinfo.s_sum/100, gCardinfo.s_sum%100);
	LED_Dis3((char*)revbuff);
	led_on(LED_GREEN);
puchaseENd:
	SLXK_endCard();
	memcpy(snddat, "\x00\x70\x80\x01\x00", 5);//通讯完后关闭通道1
	ret = sndCmd(snddat, 5, revbuff);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("ENd close channel:");
	debugdata(revbuff, ret, 1);
#endif

}


unsigned char ZKXLCPU_Read_Money(unsigned char *remb, unsigned char mode)
{
	unsigned char sndbuf[64];
	unsigned char revbuf[64];
	unsigned char ret;
#ifdef _debug_
	unsigned int temp;
#endif
	
	//815C 0002 04 // 查询余额
	memcpy(sndbuf, "\x81\x5C\x00\x02\x04", 5);
	ret = sndCmd40(sndbuf, 5, revbuf, 0x20);
	if(ret == 0){
		return 1;
	}

#ifdef _debug_
	debugstring("---remb:====");
	debugdata(revbuf,ret, 1);
	memcpy((unsigned char*)&temp, revbuf, 4);
	over_turn(4, (unsigned char*)&temp);
	sprintf((char *)sndbuf, "余额:%d.%02d", temp/100, temp%100);
	debugstring((char*)sndbuf);
#endif
	memcpy(remb, revbuf, 4);
	return 0;
}

extern unsigned char getMiKey(void);
//V4.0协议
unsigned char CPUPuchace40(void)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned char CPUPKEY[16];
	unsigned short ret, ii;
	unsigned int i, value, pmoney, itemp;
	pboc_init_capp_purchase_res stuInitPurchaseRet;
	unsigned char Cardtype;	//此标志是哪家的卡？华虹：611B，恒宝：6117，捷德：611B，大唐：611B
	//wangzhiyong@xunfang.com
							//恒宝的卡，AID不一样，消费、圈存的密钥索引不一样。《文档8——2.4GHz射频SIM卡PBOC应用的差异V1.2.pdf》
//	checkSimVer();
	
	Cardtype = 0x17;//默认的是恒宝,选择PSE后根据返回长度再判断。

	gMCardCand = CARDSTYLE_24ZKXL;
	a_sum1 = 0;
	
	for(i=0; i<3; i++){//0070000001
		memcpy(snddat, "\x00\x70\x00\x00\x01", 5);//打开通道
		ret = sndCmd40(snddat, 5, revbuff, 0x20);
		if(ret == 0){
			return ST_ERROR;
		}
#ifdef _debug_
		debugstring("open channel:");
		debugdata(revbuff, ret, 1);
#endif
		if(memcmp(revbuff, "\x01\x90\x00", 3) == 0){//通道被OK
			break;
		}
		else{
			memcpy(snddat, "\x00\x70\x80\x01\x00", 5);//关闭通道1
			ret = sndCmd40(snddat, 5, revbuff, 0x20);
			if(ret == 0){
				return ST_ERROR;
			}
#ifdef _debug_
			debugstring("close channel:");
			debugdata(revbuff, ret, 1);
#endif
		}
	}
	
	if(i >= 3)
		return ST_ERROR;
	
	GetDateTime();

	memcpy(snddat, "\x01\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);//Select PSE
	ret = sndCmd40(snddat, 19, revbuff, 0x20);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("40 Select PSE:");
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		return ST_ERROR;
	}
	Cardtype = revbuff[1];//长度。华虹：611B，恒宝：6117。

	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("get Respond:");//6F 19 84 0E 31 50 41 59 2E 53 59 53 2E 44 44 46 30 31 A5 04 9F 08 01 01 88 01 15 90 00 00 
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		return ST_ERROR;
	}
	
	if(Cardtype == 0x17)
		memcpy(snddat, "\x01\xA4\x04\x00\x09\x10\x00\x00\x00\x00\x00\x10\x00\x03", 14);// Select AID   恒宝
	else
		memcpy(snddat, "\x01\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 14);// Select AID   华虹

	ret = sndCmd40(snddat, 14, revbuff, 0x20);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("选择 AID:");
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		return ST_ERROR;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("取响应:");//6F328409A00000000386980701A5049F0801019F0C1E6E56531794F68054010107170000000000000001FFFFFFFFFFFFFFFF0000900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		return ST_ERROR;
	}
	
//---读15文件------
	memcpy(snddat, "\x01\xB0\x95\x00\x1E", 5);//取响应  有30个字节：5A2000036C1E0051CA
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("读15文件取响应:");//读15文件1E=30字节 取响应:6E56531794F68054010107170000000000000001FFFFFFFFFFFFFFFF0000900000
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[ret-3] == 0x61){
		memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
		snddat[4] = revbuff[ret-2];
		ret = sndCmd40(snddat, 5, revbuff, 0x20);
		if(ret == 0){
			return ST_ERROR;
		}
	}
#ifdef _debug_
	debugstring("取响应:");//6F328409A00000000386980701A5049F0801019F0C1E6E56531794F68054010107170000000000000001FFFFFFFFFFFFFFFF0000900000
	debugdata(revbuff, ret, 1);	//A0000755E649508F0301320301000000000000002013060620200101A911900000
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00", 2) != 0){
		return ST_ERROR;
	}

	cardMessage.card_catalog = revbuff[10];//卡类
	if(cardMessage.card_catalog == 50)//还没有售出的卡
		cardMessage.card_catalog = 0x41;
		//return CARD_WHITE_BUS;
	if(cardMessage.card_catalog < 32)
		cardMessage.card_catalog += CARD_NORMAL;
//	memcpy((unsigned char*)&stuPubInfZaoZuang, revbuff+12, 8);//卡序列号
	memcpy(cardMessage.CityCode, revbuff+2, 2);//城市代码
	memcpy(cardMessage.CardCheckCode, revbuff+4, 4);//MAC码
	memcpy(cardMessage.TradeCode, revbuff+8, 2);//行业代码
	memcpy(cardMessage.PublishBicker, revbuff+12, 4);//发卡流水号

	memcpy((unsigned char*)&c_serial, cardMessage.PublishBicker, 4);//这里是没有取物理卡号使用的是流水号
	
#ifdef _debug_
	debugstring("cardMessage.card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1, 1);
//	debugdata((unsigned char*)&stuPubInfZaoZuang,50, 1);
#endif

	memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
	over_turn(4, (unsigned char*)&itemp);
	memcpy((unsigned char*)&value, revbuff+20, 4);//启用日期
	over_turn(4, (unsigned char*)&value);
	memcpy((unsigned char*)&pmoney, revbuff+24, 4);//有效日期
	over_turn(4, (unsigned char*)&pmoney);
	
#ifdef _debug_
	debugstring("卡号 启用，有效期：");
	debugdata(cardMessage.PublishBicker, 4, 1);
	debugdata((unsigned char*)&value, 4, 1);
	debugdata((unsigned char*)&pmoney, 4, 1);
#endif
// 	if((itemp < value)||(itemp > pmoney)){
// 		return CARD_NO_TIME;//过期或末到期
// 	}
//---读16文件------
/*	memcpy(snddat, "\x01\xB0\x96\x00\xC8", 5);//取响应  有200个字节
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("读16文件取响应:");//读16文件取响应:00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	*/
	ZKXLCPU_Read_Money((unsigned char*)&value, 3);
	over_turn(4, (unsigned char*)&value);	//如果重刷需要比较
	
	if(cardSound == 0xaa){//需要重刷
		if(card_ser!=c_serial){//不是同一张卡，不能处理
			return ST_ERROR;
		}
		//		debugstring("Raa1:");
		//		debugdata((unsigned char*)&card_ser, 4, 1);
		//		debugdata((unsigned char*)&c_serial, 4, 1);
		// 		
		// #ifdef _debug_CPU_
		// 		debugstring("需要重刷 ZBGGQB:开始\r\n");
		// #endif
		// 		memcpy(sndbuf, "\x02\x00\xA4\x04\x00\x06",6);
		// 		memcpy(sndbuf+6, "ZBGGQB", 6);
		// 		deal2or3(sndbuf);
		// 		ret = RfPassthrough(revbuf, 2, sndbuf, 12);
		// 		if(ret == 0)
		// 			return ST_ERROR;
		// 		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		// 			return ST_ERROR;
		// 		
		// #ifdef _debug_CPU_
		// 		debugstring("需要重刷 ZBGGQB:");
		// 		debugdata(revbuf, ret, 1);
		// #endif
#ifdef _debug_
		debugstring("aa余额:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
		if(value < a_sumR){//上次已经扣成功
			memcpy(snddat, "\x81\x5A\x00\x06\x02", 5);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(snddat+6, (unsigned char*)&ii, 2);
			snddat[9] = 8;
#ifdef _debug_
			debugstring("防拔:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(snddat, 10, 1);
#endif
			ret = sndCmd40(snddat, 5, revbuff, 0x20);
#ifdef _debug_
			debugdata(revbuff, ret, 1);
#endif
			if(ret == 0)//返回超时,错误
				return ST_ERROR;
			if((memcmp(revbuff+ret-2, "\x90\x00", 2)!=0) &&\
				(memcmp(revbuff+ret-2, "\x6C\x08", 2)!=0))
				goto endPuchace;//取不成功02 69 85 
			
			//			memcpy(TACC, revbuff+revLen-6, 4);//返回的TAC码
			//			memcpy(sndbuf, revbuff+revLen-10, 4);//用MAC2码
			//			return ST_ERROR;
			goto endPuchace;
		}
		// 		memcpy(sndbuf, "\x02\x00\xa4\x00\x00\x02\xdf\x01", 8);
		// #ifdef _debug_CPU_
		// 		debugstring("选主文件:");
		// 		debugdata(sndbuf, 8, 1);
		// #endif
		// 		deal2or3(sndbuf);
		// 		revLen = RfPassthrough(revbuf, 2, sndbuf, 8);
		// #ifdef _debug_CPU_
		// 		debugdata(revbuf, revLen, 1);
		// #endif
		// 		if(revLen == 0)
		// 			return ST_ERROR;
	}
	
	a_sumR = value;
	
#ifdef _debug_
	debugstring("a_sumR:");
	debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
	
	s_sum1 = get_s_sum1(0);
	if((s_sum1 == 0) || (s_sum1 > 9900))
		return 22;
//-------------------------------------------------
//	memcpy(snddat, "\x81\x50\x01\x02\x0B\x03\x00\x00\x03\xE8\x00\x00\x00\x00\x00\x01", 16); // 消费初始化
	if(Cardtype == 0x17)//恒宝消费密钥索引01
		memcpy(snddat, "\x81\x50\x01\x02\x0B\x01\x00\x00\x00\x01\x01\x23\x45\x67\x89\x01", 16); // 消费初始化
	else//其它的消费密钥索引03
		memcpy(snddat, "\x81\x50\x01\x02\x0B\x03\x00\x00\x00\x01\x01\x23\x45\x67\x89\x01", 16); // 消费初始化
	memcpy(snddat+6, (unsigned char*)&s_sum1, 4);
	over_turn(4, snddat+6);
	ret = sndCmd40(snddat, 16, revbuff, 0x20);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("消费初始化:");//610F00
	debugdata((unsigned char*)&s_sum1, 4, 1);
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] == 0x61){
		memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
		snddat[4] = revbuff[1];
		ret = sndCmd40(snddat, 5, revbuff, 0x20);
		if(ret == 0){
			return ST_ERROR;
		}
#ifdef _debug_
		debugstring("初始化响应:");//00000064 0000 000000 01 82 538EC13F 900000
								   //00000064 0000 000000 01 82 7CA3576D 900000
		debugdata(revbuff, ret, 1);
#endif
	
	}

	if(memcmp(revbuff+ret-3, "\x94\x01", 2)==0){
		return 3;
	}
	else if(memcmp(revbuff+ret-3, "\x90\x00", 2)!=0)
		return 5;

	memcpy((unsigned char*)&stuInitPurchaseRet, revbuff, sizeof(pboc_init_capp_purchase_res));

	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);
	over_turn(4, (unsigned char*)&a_sum1);
	a_sum1 -= s_sum1;//余额
#ifdef _debug_
	debugstring("a_sum1:");
	debugdata((unsigned char*)&a_sum1, 4, 1);
#endif

	gMCardCand = CARDSTYLE_CPU;
#ifdef NO_CPU_PSAM_
	if(getMiKey() != ST_OK)	//验证卡的认证码
		return 8;
	//----------计算KEY----------
// #ifdef BUS_PRO_JSB
// 	memcpy(sndbuf, cardMessage.CardCheckCode, 4);//数据源：MAC+卡流水+卡流水前2字节+6个0
// 	memcpy(sndbuf+4, cardMessage.PublishBicker, 4);
// 	memcpy(sndbuf+8, cardMessage.PublishBicker, 2);
// 	memset(sndbuf+10, 0, 6);
// 	
// 	getJiuJiang_CPUKey(sndbuf, CPUPKEY);
// #else
	memcpy(CPUPKEY, cardMessage.MoneyKEYA, 6);
	memcpy(CPUPKEY+6, cardMessage.RecodeKEYA, 6);
	memcpy(CPUPKEY+12, cardMessage.MonthKEYA, 4);
//#endif
#endif
	//计算消费时MAC1
//	memset(CPUPKEY, 0xff, 20);//测试使用默认KEY
	memcpy(revbuff+20, stuInitPurchaseRet.cRand, 4);//随机数
	memcpy(revbuff+24, stuInitPurchaseRet.cSnq, 2);
	memcpy(revbuff+26, "\x00\x01", 2);

#ifdef _debug_
	debugstring("CPUPKEY:");
	debugdata(CPUPKEY, 16, 1);
#endif

	memcpy(snddat, (unsigned char*)&s_sum1, 4);
	over_turn(4, snddat);
	snddat[4] = 0x06;
	memcpy(snddat+5, "\x01\x23\x45\x67\x89\x01", 6);
	memcpy(snddat+11, (unsigned char*)&SysTime, 7);//共18
#ifdef _debug_
	debugstring("revbuff::");
	debugdata(revbuff, 28, 1);

	debugstring("MACdat::");
	debugdata(snddat, 18, 1);
#endif
	memset(revbuff+64, 0, 16);
	Mac(CPUPKEY, revbuff+20, snddat, 18, revbuff+64, revbuff+100);
#ifdef _debug_
	debugstring("MAC::");
	debugdata(revbuff+100, 4, 1);
#endif
	memcpy(snddat, "\x81\x54\x01\x00\x0F\x00\x00\x00\x01", 9); // 消费 终端序列号
	memcpy(snddat+9, (unsigned char*)&SysTime, 7);//交易日期时间
	memcpy(snddat+16, revbuff+100, 4);//MAC1

	ret = sndCmd40(snddat, 20, revbuff, 0x20);//5A200014815401000F00000001201201131553374F0A0814CA
	if(ret == 0){
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}
#ifdef _debug_
	debugstring("消费:");//610F00
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}
#ifdef _debug_
	debugstring("消费响应:");//00000064 0000 000000 01 82 538EC13F 900000
	debugdata(revbuff, ret, 1);
#endif
	
	if(memcmp(revbuff+ret-3, "\x93\x02", 2)==0)
		return 9;
	if(memcmp(revbuff+ret-3, "\x90\x00", 2)!=0){
		//		debugdata(revbuf, 20, 1);
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}//其它错误
	
	//815C 0002 04 // 查询余额
	memcpy(snddat, "\x81\x5C\x00\x02\x04", 5);
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
// 	if(ret == 0){
// 		return;
// 	}
#ifdef _debug_
	debugstring("余额:");
	debugdata(revbuff, ret, 1);//00000064900000
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto endPuchace;//puchaseENd;
	}
	else{
		memcpy((unsigned char*)&a_sum1, revbuff, 4);
		over_turn(4, (unsigned char*)&a_sum1);
	}

endPuchace:
#ifdef _debug_
	debugstring("消费完成:\r\n");
	sprintf((char*)revbuff, "余额:%d.%02d\r\n", a_sum1/100, a_sum1%100);
	debugstring((char*)revbuff);
	sprintf((char*)revbuff, "扣款:%d.%02d\r\n", s_sum1/100, s_sum1%100);
	debugstring((char*)revbuff);
#endif

	snddat[0] = 2;
	sndCmd40(snddat, 1, revbuff, 0x22);//5A2200010526CA
	
	cardSound = 0;
	card_ser = 0;


	Save_delayCardinfo(0);
	Sound_cityCMD(0);
	BuildRecorde(ID_REC_TOLL, revbuff);
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
	WriteRecord(revbuff, 0);
#ifdef _debug_
	debugstring("ENd close channel:");
	debugdata(revbuff, ret, 1);
#endif

	return ST_OK;
}

unsigned char ZKXL_upBinary(unsigned char ID, unsigned char *dat, unsigned char len)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned char keyWrt[16];
	unsigned char Random[8];
	unsigned char TACC[16];
	unsigned short ret;

	memcpy(snddat, "\x01\x84\x00\x00\x04", 5);//取响应
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return 1;
	}
#ifdef _debug_
	debugstring("ZKXL_upBinary随机数:");
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		return 2;
	}
	memset(Random, 0, 8);
	memcpy(Random, revbuff, 4);

	memcpy(snddat, "\x04\xD6\x95\x00\x08\x11\x22\x33\x44", 9);//取响应
	memset(keyWrt, 0xFF, 16);
	
	debugstring("Random::");
	debugdata(Random, 8, 1);

	CountMac(keyWrt, Random, snddat, 9, TACC);//32
	memcpy(snddat+9, TACC, 4);
#ifdef _debug_
	debugstring("MAC::");
	debugdata(Random, 8, 1);
	debugdata(snddat, 13, 1);
	debugdata(TACC, 8, 1);
#endif

	snddat[0] |= 0x01;
	ret = sndCmd40(snddat, 13, revbuff, 0x20);
	if(ret == 0){
		return 1;
	}
#ifdef _debug_
	debugstring("ZKXL_upBinary取响应:");//6F 19 84 0E 31 50 41 59 2E 53 59 53 2E 44 44 46 30 31 A5 04 9F 08 01 01 88 01 15 90 00 00 
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		return 2;
	}

	return 0;
}

//充值 //流程参照：《中国金融集成电路（IC）卡规范第2部分(发布稿-20100513).pdf》
void ZKXL_Charge(void)
{	
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	unsigned int i;
	pboc_init_capp_load_res stuInitLoadRet;
	//wangzhiyong@xunfang.com

//	checkSimVer();

	for(i=0; i<3; i++){
		memcpy(snddat, "\x00\x70\x00\x00\x01", 5);//打开通道
		ret = sndCmd40(snddat, 5, revbuff, 0x20);
		if(ret == 0){
			return;
		}
#ifdef _debug_
		debugstring("1open channel:");
		debugdata(revbuff, ret, 1);
#endif
		if(memcmp(revbuff, "\x01\x90\x00", 2) == 0){//通道被OK
			break;
		}
		else{
			memcpy(snddat, "\x00\x70\x80\x01\x00", 5);//关闭通道1
			ret = sndCmd40(snddat, 5, revbuff, 0x20);
			if(ret == 0){
				return;
			}
#ifdef _debug_
			debugstring("1close channel:");
			debugdata(revbuff, ret, 1);
#endif
		}
	}

	if(i >= 3)
		return;

	GetDateTime();
	memcpy(snddat, "\x01\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);//Select PSE
	ret = sndCmd40(snddat, 19, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("1Select PSE:");
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("1取响应:");//6F 19 84 0E 31 50 41 59 2E 53 59 53 2E 44 44 46 30 31 A5 04 9F 08 01 01 88 01 15 90 00 00 
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	
	memcpy(snddat, "\x01\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 14);// Select AID
	ret = sndCmd40(snddat, 14, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("1Select AID:");
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("1取响应:");//6F328409A00000000386980701A5049F0801019F0C1E6E56531794F68054010107170000000000000001FFFFFFFFFFFFFFFF0000900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}

	if(ZKXL_upBinary(0x16, revbuff, 4) != 0)
		return;

//8150 0002 0B 04000003E8000000000001  密钥索引+交易金额+终端机编号
//	memcpy(snddat, "\x81\x50\x01\x02\x0B\x03\x00\x00\x03\xE8\x00\x00\x00\x00\x00\x01", 16); // 圈存初始化
	memcpy(snddat, "\x81\x50\x00\x02\x0B\x04\x00\x00\x00\x01\x01\x23\x45\x67\x89\x01", 16); //
	memcpy(snddat+6, (unsigned char*)&gCardinfo.s_sum, 4);
	over_turn(4, snddat+6);
	ret = sndCmd40(snddat, 16, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("圈存初始化:");//610F00
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("圈存初始化响应:");//00000049 0001 01 82 5745DD38 2B42C407 900000
							   //00000064 0000 000000 01 82 7CA3576D 900000
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	memcpy((unsigned char*)&stuInitLoadRet, revbuff, sizeof(pboc_init_capp_load_res));

	//验证MAC1
	memset(revbuff, 0xff, 20);//KEY
	memcpy(revbuff+20, stuInitLoadRet.cRand, 4);//随机数
	memcpy(revbuff+24, stuInitLoadRet.cSnq, 2);
	memcpy(revbuff+26, "\x80\x00", 2);


//indata2:
	memcpy(snddat, stuInitLoadRet.cBalance, 4);//余额（前）
	memcpy(snddat+4, (unsigned char*)&gCardinfo.s_sum, 4);//交易金额
	over_turn(4, snddat+4);
	snddat[8] = 0x02;//交易类型标识
	memcpy(snddat+9, "\x01\x23\x45\x67\x89\x01", 6);//终端机编号
#ifdef _debug_
	debugstring("revbuff::");
	debugdata(revbuff, 28, 1);

	debugstring("MACdat::");
	debugdata(snddat, 15, 1);
#endif
	memset(revbuff+64, 0, 16);
	Mac(revbuff, revbuff+20, snddat, 15, revbuff+64, revbuff+100);
#ifdef _debug_
	debugstring("MAC1::");
	debugdata(revbuff+100, 4, 1);
	debugdata(stuInitLoadRet.cMac1, 4, 1);
#endif
	if(memcmp(revbuff+100, stuInitLoadRet.cMac1, 4) != 0){//验证MAC1
		cls();
		display(1, 0, "充值MAC1错误!", DIS_CENTER);
		delayxs(2);
		return;
	}
	
	//计算MAC2
	memset(revbuff, 0xff, 20);//KEY
	memcpy(revbuff+20, stuInitLoadRet.cRand, 4);//随机数
	memcpy(revbuff+24, stuInitLoadRet.cSnq, 2);
	memcpy(revbuff+26, "\x80\x00", 2);
	
	
	//indata2:
	memcpy(snddat, (unsigned char*)&gCardinfo.s_sum, 4);//交易金额
	over_turn(4, snddat);
	snddat[4] = 0x02;//交易类型标识
	memcpy(snddat+5, "\x01\x23\x45\x67\x89\x01", 6);//终端机编号
	memcpy(snddat+11, (unsigned char*)&SysTime.year_h, 7);
#ifdef _debug_
	debugstring("MAC2 revbuff::");
	debugdata(revbuff, 28, 1);
	
	debugstring("MAC2 dat::");
	debugdata(snddat, 18, 1);
#endif
	memset(revbuff+64, 0, 16);
	Mac(revbuff, revbuff+20, snddat, 18, revbuff+64, revbuff+100);
#ifdef _debug_
	debugstring("MAC2::");
	debugdata(revbuff+100, 4, 1);//计算MAC2
#endif

	//8152 0000 0B 20110106103733514EF8EA// 圈存
	memcpy(snddat, "\x81\x52\x00\x00\x0B",5); // 圈存 终端序列号
	memcpy(snddat+5, (unsigned char*)&SysTime, 7);//交易日期时间
	memcpy(snddat+12, revbuff+100, 4);//MAC2

	ret = sndCmd40(snddat, 16, revbuff, 0x20);//5A200014815401000F00000001201201131553374F0A0814CA
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("圈存:");//610F00
	debugdata(revbuff, ret, 1);
#endif
	if(revbuff[0] != 0x61){
		goto puchaseENd;
	}
	memcpy(snddat, "\x01\xC0\x00\x00\x04", 5);//取响应
	snddat[4] = revbuff[1];
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("圈存响应:");//00000064 0000 000000 01 82 538EC13F 900000
	debugdata(revbuff, ret, 1);//TAC
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	
	//815C 0002 04 // 查询余额
	memcpy(snddat, "\x81\x5C\x00\x02\x04", 5);
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("余额:");
	debugdata(revbuff, ret, 1);//00000064900000
#endif
	if(memcmp(revbuff+ret-3, "\x90\x00\x00", 3) != 0){
		goto puchaseENd;
	}
	memcpy((unsigned char*)&gCardinfo.a_sum, revbuff, 4);
	over_turn(4, (unsigned char*)&gCardinfo.a_sum);

#ifdef _debug_
	debugstring("充值完成:\r\n");
	sprintf((char*)revbuff, "余额:%d.%02d\r\n", gCardinfo.a_sum/100, gCardinfo.a_sum%100);
	debugstring((char*)revbuff);
	sprintf((char*)revbuff, "充值:%d.%02d\r\n", gCardinfo.s_sum/100, gCardinfo.s_sum%100);
	debugstring((char*)revbuff);
#endif

	cls();
	display(0,0,"充值结果:",0);
	sprintf((char*)revbuff, "余额:%d.%02d", gCardinfo.a_sum/100, gCardinfo.a_sum%100);
	display(2,2,(char*)revbuff, 0);
	sprintf((char*)revbuff, "充值:%d.%02d", gCardinfo.s_sum/100, gCardinfo.s_sum%100);
	display(4,2,(char*)revbuff, 0);
	audio_old(1);
	
	sprintf((char*)revbuff, "%d.%02d", gCardinfo.s_sum/100, gCardinfo.s_sum%100);
	LED_Dis3((char*)revbuff);
	led_on(LED_GREEN);

puchaseENd:
	SLXK_endCard();
	memcpy(snddat, "\x00\x70\x80\x01\x00", 5);//通讯完后关闭通道1
	ret = sndCmd40(snddat, 5, revbuff, 0x20);
	if(ret == 0){
		return;
	}
#ifdef _debug_
	debugstring("ENd close channel:");
	debugdata(revbuff, ret, 1);
#endif

}
extern void face_time(void);
void ZKXL_Main(void)
{
	unsigned char snddd[64];
	unsigned char Reddd[32];
	unsigned char rflag;

	if(F_FLAG_VARY.stop_flag == 1){
		return;
	}

	if(gZKXLdly == 0){
		memcpy(snddd, "\x5A\x10\x00\x01\x0A\x1B\xCA", 7);//5A 10 05 14 CA 5A 10 05 14 CA
		com_snd(G24Com, 7, snddd);
		gZKXLdly = 1;
	}
	else{
		if(flag == 0x5A){
#ifdef _debug_
			debugstring("ROK:");
			debugdata(gcomBuf, gComindex, 1);//5A10000902271720510318020043CA
#endif
			if(cmdDeal() == 1){
				//CPUPuchace34();
				rflag = CPUPuchace40();

				if(rflag != ST_OK){
					snddd[0] = 2;
					sndCmd40(snddd, 1, Reddd, 0x22);//5A2200010526CA
				}
				sprintf((char*)snddd, "错误:%02d ", rflag);
				
				switch (rflag)
				{
				case ST_ERROR:
					return;	
				case CARD_WHITE_BUS:				//白卡
					error(ERR_CARD_WHITE_BUS,0);
					break;
				case CARD_BLACK_PRO:
					cls();
					display(0,0,"警告：黑名单卡!",0);
					display(2,4,"请投币!!",0);
					SoundMessage(INVALID_CARD);
					break;
				case 22:
					display(0,0,"提示:", DIS_Cls);
					display(2,0,"票价信息不正确",DIS_CENTER);

					break;
				case CARD_NO_TIME:
					cls();
					display(3, 0, "卡过期或没启用", DIS_CENTER);
					break;
				case 3:
					cls();
					display(0, 0, (char*)snddd, 0);
					display(3, 0, "余额不足", DIS_CENTER);
					SoundMessage(SOUND_FLING_MONEY);
					break;
				case 4:
					cls();
					display(0, 0, (char*)snddd, 0);
					display(3, 0, "扣次不能连刷", DIS_CENTER);
					SoundMessage(SOUND_FLING_MONEY);
					break;
				case 6:
					cls();
					display(0, 0, (char*)snddd, 0);
					display(3, 0, "MAC1计算错误", DIS_CENTER);
					break;
				case 9:
				case 8:
					cls();
					display(0, 0, (char*)snddd, 0);
					display(3, 0, "非法卡", DIS_CENTER);
					SoundMessage(INVALID_CARD);
					break;
				case ST_OK://成功
					SetTcpPacketTTLTime(RECORD_TTL);
					break;
				default:
					// 		cls();
					// 		display(0, 0, disbuf, 0);
					// 		display(3, 0, "末定义错误", DIS_CENTER);
					break;
				}
//				SLXK_endCard();
				g24GDisFlash = 3;
				restore_flag = 0;
			}
			gComindex = 0;
			flag = 0;
		}
		else{
			if(++gZKXLdly > 20){
				gZKXLdly = 0;
			}
		}
	}
}

#endif

