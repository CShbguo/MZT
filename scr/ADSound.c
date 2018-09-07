
/*
ADSound.c
用于GPS语音报站时编写的语音头及语音尾。
此语音的位置在 0x3B0000-0x3B8000 共32K。
具体定义看《车载机存贮器分配表.xlsx》
*/
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "Font.h"
#include "StructDef.h"

extern stcomtime SysTime;

//文件头偏移量
#define ADHeadoffset FLASH_GPS_AD_START
//循环语音索引地址偏移量
#define ADXunhuanoffset (ADHeadoffset+64)
//正向索引地址偏移量
#define ADZhengxiangoffset (ADXunhuanoffset+64)
//反向索引地址偏移量
#define ADFanxiangoffset (ADZhengxiangoffset+256)
//文本信息偏移量
#define ADInfooffset (ADFanxiangoffset+256)

//语音信息最大250条
#define ADInfoMaxNum 250
//站点序列最大125个
#define ADZhanDianMaxNum 125

//读取一行，一行不超过len字节,传入的rbuff长度必须比LEN要大1
unsigned int FlashGetLine(unsigned int addr, unsigned char*rbuff, unsigned int len)
{
	char *pe;

	memset(rbuff, 0, len);//需要有个结束符
	flashread(addr, (unsigned char*)rbuff, len);

	pe = strstr((char *)rbuff, "\r\n");
	if(pe != NULL){
		pe += 2;
	}
	else{//没找到回车换行符，所有的数据都算并返回
		pe = (char*)rbuff+len;
	}
	*pe = 0;	//加一个结束符
	return (pe-(char*)rbuff);
}

//处理临时区的文件，因历史原因，此临时区位于：FLASH_GPS_SOUND_START。
//这里是用于转存公钥的。所以处理完后需要把此区清除。如用到公钥，请另行下载。
unsigned char DealADTmpFile(void)
{
	
	unsigned int addr;
	unsigned int infaddr;//当前写入信息的序号。每段信息都是100字节。
	char buff[260];
	unsigned short slineNo;

	unsigned char XunHuanbuf[64];//循环语音序号
	unsigned char ZDbuff[2][125][2];//正(反)向站点的头语音位置序号，和尾语音位置序号。共250字节。存贮时256字节，需加两字节FF和4字节CRC32，序号为0或大于250时则认为此站没有自定义语音
	unsigned short usVer;
	unsigned char ZFflag=0;//正反向标志，默认正向。
	unsigned char zdflag=0;//是否是语音头或尾标志。0=头，1=尾。

	char Ver[16];
	unsigned int i, uiLen, t, crc32;
	char *p, *p1;

	memset(buff, 0, sizeof(buff));
	flashread(FLASH_GPS_SOUND_START, (unsigned char*)buff, 64);
	if(memcmp(buff, "语音定义", 8) != 0){//是AD指令头文件
		return ST_ERROR;
	}
	cls();
	display(0,0,"AD文件处理!",DIS_CENTER|DIS_CONVERT);
	
	MSG_LOG("OtheerFiles Ver:");
	MSG_LOG((char*)buff);

	p1 = strstr((char*)buff, "文件版本");
	if(p1 == NULL){//文件没有版本，不合法
		display(2, 0, "无文件版本号", DIS_CENTER);
		beep(1, 50, 50);
		delayxms(200);
		return ST_ERROR;
	}
	memset(Ver, 0, sizeof(Ver));
	p = Ver;
	for(i=0; i<32; i++){
		if((*p1 >= '0')&&(*p1 <= '9')){//有效值
			*p++ = *p1;
		}
		else if(p != Ver){//已经取得版本号，这里是版本号结束，如果没取到，说明还没有到版本号的地方
			break;
		}
		p1++;
	}
	usVer = __atol(Ver);
	if(i == 0){//不合法
		display(2, 0, "版本号不能为0", DIS_CENTER);
		beep(1, 50, 50);
		delayxms(200);
		return ST_ERROR;
	}

	
	p1 = strstr((char*)buff, "线路号");
	if(p1 == NULL){//文件没有版本，不合法
		display(2, 0, "无线路号", DIS_CENTER);
		beep(1, 50, 50);
		delayxms(200);
		return ST_ERROR;
	}
	memset(Ver, 0, sizeof(Ver));
	p1 += 6;
	p = Ver;
	for(i=0; i<16; i++){
		if((*p1 >= '0')&&(*p1 <= '9')){//有效值
			*p++ = *p1;
		}
		else if(p != Ver){//已经取得数字，如果没取到，说明还没有到版本号的地方
			break;
		}
		p1++;
	}
	slineNo = __atol(Ver);

	//下面是处理信息

	flasherase(ERASE32KBYTESECTOR, ADHeadoffset);
	memset((unsigned char*)ZDbuff, 0xFF, sizeof(ZDbuff));
	memset(XunHuanbuf, 0xFF, sizeof(XunHuanbuf));

	infaddr = 0;
	for(addr=FLASH_GPS_SOUND_START; addr<FLASH_GPS_SOUND_END;)
	{
		memset(buff, 0, sizeof(buff));
		uiLen = FlashGetLine(addr, (unsigned char*)buff, 256);	//读出一行。返回一行数据的长度
		addr += uiLen;	//地址指向下一行

		MSG_LOG("GetLine i=%d\r\n", i);
		MSG_LOG(buff);

		if(memcmp(buff, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 10) == 0)//文件结束
			break;
		zdflag = 0xFF;//不是站点头也不是站点尾信息
		p = strstr(buff, "正向");
		if(p != NULL){//下面是正向的定义
			MSG_LOG("正向 ");
			ZFflag = 0;
		}
		else if(strstr(buff, "反向") !=  NULL){//说明下面的定义是反向站点的
			MSG_LOG("反向 ");
			ZFflag = 1;
		}
		else if(strstr(buff, "循环") != NULL){
			MSG_LOG("循环 ");
			ZFflag = 2;
		}
		else if(strstr(buff, "站点头") != NULL){
			MSG_LOG("站点头 ");
			zdflag = 0;
		}
		else if(strstr(buff, "站点尾") != NULL){
			MSG_LOG("站点尾 ");
			zdflag = 1;
		}
		else if(strstr(buff, "语音:") != NULL){
			MSG_LOG("语音: ");
			zdflag = 0;
		}
		else if(strstr(buff, "结束") != NULL){
			MSG_LOG("结束 ");
			break;
		}

		if(zdflag <= 1){//是站点头或尾，需要对信息进行处理
			p1 = strstr(buff, ":");	//找第一个:号。
			if(p1 == NULL){//没有，这一行数据无效
				continue;	//进入下一行数据检测
			}
			p1++;//跳过第一个:号，下面应该是数据。
			while(p1 <= (buff+uiLen)){//找数字
				memset(Ver, 0, sizeof(Ver));
				p = Ver;
				t = ((buff+uiLen)-p1);//后面的数据长度
				for(i=0; i<t; i++){
					if((*p1 >= '0')&&(*p1 <= '9')){//有效值
						*p++ = *p1++;
					}
					else{//不是数字了。可以进行处理
						break;
					}
				}
				if(p != Ver){//有数字则处理。并把语音的偏移量写入
					i = __atol(Ver);
					if(ZFflag<2){
						if((i>0)&&(i <= 125)){//站号大于125的是非法的，而且编号是从1开始的
							ZDbuff[ZFflag][i-1][zdflag] = infaddr;
						}
					}
					else if(ZFflag == 2){	//是循环语音
						if((i>0)&&(i<=60)){//只能有60条。从1开始
							XunHuanbuf[i-1] = infaddr;
						}
					}

					MSG_LOG("数字站点=%d\r\n", i);
					MSG_LOG(buff);
				}
				if(*p1 == ':'){//再碰到：号则表示后面是信息了，要写入后面的信息，并且结束此行的处理
					p1++;
					t = uiLen-(p1-buff);//信息长度。
					p1[t-1] = 0;	//在信息最后加入结束符，就是把回车换行写成00了。
					p1[t-2] = 0;	//在信息最后加入结束符
					flashwrite(ADInfooffset+(infaddr*100), (unsigned char*)p1, t);
					infaddr++;//指向下一个要写的的序号

					MSG_LOG("写入信息 infaddr=%d\r\n", infaddr);
					MSG_LOG(buff);

					break;
				}
				else
					p1++;//跳过这个不是数字的位置，指向下一个位置
			}
		}
	}
	if(infaddr != 0){//整个文件处理结束，如果有写入信息，则把包头及序号写入，否则就不用更新了
		memset(buff, 0xFF, 64);
		memcpy(buff, "语音定义", 8);
		memcpy(buff+8, (unsigned char*)&usVer, 2);
		memcpy(buff+10, (unsigned char*)&SysTime, 7);
		memcpy(buff+17, (unsigned char*)&slineNo, 2);
		crc32 = __cpuCrc32((unsigned char*)buff, 60);
		memcpy(buff+60, (unsigned char*)&crc32, 4);
		flashwrite(ADHeadoffset, (unsigned char*)buff, 64);	//写入文件头

		MSG_LOG("写入文件头:");
		BCD_LOG((unsigned char*)buff, 64, 1);

		crc32 = __cpuCrc32(XunHuanbuf, 60);
		memcpy(XunHuanbuf+60, (unsigned char*)&crc32, 4);
		flashwrite(ADXunhuanoffset, XunHuanbuf, 64);
		MSG_LOG("写入循环索引:");
		BCD_LOG((unsigned char*)XunHuanbuf, 64, 1);

		memcpy(buff, (unsigned char*)ZDbuff[0], 250);
		crc32 = __cpuCrc32((unsigned char*)buff, 252);
		memcpy(buff+252, (unsigned char*)&crc32, 4);
		flashwrite(ADZhengxiangoffset, (unsigned char*)buff, 256);	//写入正向索引
		MSG_LOG("写入正向索引:");
		BCD_LOG((unsigned char*)buff, 256, 1);
		
		memcpy(buff, (unsigned char*)ZDbuff[1], 250);
		crc32 = __cpuCrc32((unsigned char*)buff, 252);
		memcpy(buff+252, (unsigned char*)&crc32, 4);
		flashwrite(ADFanxiangoffset, (unsigned char*)buff, 256);	//写入反向索引
		MSG_LOG("写入反向索引:");
		BCD_LOG((unsigned char*)buff, 256, 1);
	}
	flasherase(ERASE32KBYTESECTOR, FLASH_GPS_SOUND_START);
	return 0;
}

//返回0，正确，其它错误,参数返回中ADver 和 ADlineNO是十六进制，低位在前。ADdatetime为BCD码。
int getADSoundFileVer(unsigned char *ADver, unsigned char *ADlineNO, unsigned char *ADdatetime)
{
	unsigned char buff[70];
	unsigned int xcrc32;
	
	memset(buff, 0, sizeof(buff));
	memset(ADver, 0, 2);

	flashread(FLASH_GPS_AD_START, (unsigned char*)buff, 64);
	
	MSG_LOG("AD Sound Ver:");
	BCD_LOG(buff, 64, 1);
	
	if(memcmp(buff, "语音定义", 8) != 0){
		return 1;
	}
	xcrc32 = __cpuCrc32(buff, 60);

	if(memcmp((unsigned char*)&xcrc32, buff+60, 4) != 0){
		MSG_LOG("CRC32 Error %08X", xcrc32);
		return 2;
	}

	memcpy(ADver, buff+8, 2);//版本号
	memcpy(ADdatetime, buff+10, 7);
	memcpy(ADlineNO, buff+17, 2);

	MSG_LOG("Ver1:");
	BCD_LOG(ADver, 2, 1);
	MSG_LOG("ADdatetime:");
	BCD_LOG(ADdatetime, 7, 1);
	MSG_LOG("ADlineNO:");
	BCD_LOG(ADlineNO, 2, 1);
	return 0;
}

volatile unsigned char ADXunhuanIndex = 0;
//输入方向及站点编号(站点编号从1开始)，输出头语音或尾语音。
//输入的方向大于1，则输出循环语音，indd无效，输出的头尾是一样的。
void ADgetInSound(unsigned char drict, unsigned char indd, char *HeadStr, char *endStr)
{
//	unsigned int index;
	unsigned char ofbuf[8];
	unsigned char rbuff[128];
	int ret;

	HeadStr[0] = 0;	//清空字符BUFF
	endStr[0] = 0;

	ret = getADSoundFileVer(rbuff, rbuff+10, rbuff+20);
	if(ret!=0)
		return;
	if((indd > 0) && (indd <= ADZhanDianMaxNum))
		indd--;	//传入的站点编号都是从1开始。最大125 .  读取的地址从0开始的，所以这里要减1
	else
		return;

	if(drict == 0){
		flashread(ADZhengxiangoffset+(indd*2), ofbuf, 2);	//读出对应位置的语音偏移量,0头，1尾
	}
	else if(drict == 1){//反向
		flashread(ADFanxiangoffset+(indd*2), ofbuf, 2);	//读出对应位置的语音偏移量,0头，1尾
	}
	else{//循环语音
		flashread(ADXunhuanoffset+(ADXunhuanIndex), ofbuf, 2);	//读出对应位置的语音偏移量，第1字节有效
		if(ofbuf[0] > ADInfoMaxNum){//值无效，回到0
			if(ADXunhuanIndex == 0){//已经是0了，说明没有循环语音，退出
				return;
			}
			ADXunhuanIndex = 0;
			flashread(ADXunhuanoffset+(ADXunhuanIndex), ofbuf, 2);
		}
		ADXunhuanIndex++;
		if(ADXunhuanIndex>=60)
			ADXunhuanIndex = 0;
		
	}
	MSG_LOG("取语音 drict=%d, indd=%d, ofbuf=%02X%02X\r\n", drict, indd, ofbuf[0], ofbuf[1]);
	if(ofbuf[0]<ADInfoMaxNum){
		flashread(ADInfooffset+(ofbuf[0]*100), rbuff, 100);	//语音头
		rbuff[99] = 0;//为了容错，最后添加结束符
		strcpy(HeadStr, (char*)rbuff);
		MSG_LOG("取语音头:%s\r\n", HeadStr);
	}
	if(drict > 1){//循环语音，语音尾和头一样，不需要再取了
		strcpy(endStr, HeadStr);
	}
	else{
		if(ofbuf[1]<ADInfoMaxNum){
			flashread(ADInfooffset+(ofbuf[1]*100), rbuff, 100);	//语音尾
			rbuff[99] = 0;//为了容错，最后添加结束符
			strcpy(endStr, (char*)rbuff);
			
			MSG_LOG("取语音尾:%s", endStr);
		}
	}
}

void debugADsoundInfo(void)
{
	unsigned char buff[256];
	unsigned char sloff[260];
	unsigned int crc32, i;

	getADSoundFileVer(sloff, sloff+10, sloff+20);
	printf("广告语音版本:%02X%02X\r\n", sloff[0], sloff[1]);
	printf("更新时间:%02X%02X%02X%02X%02X%02X%02X\r\n", sloff[20], sloff[21], sloff[22], sloff[23], sloff[24], sloff[25], sloff[26]);
	printf("线路号:%02X%02X\r\n", sloff[10], sloff[11]);
	
	flashread(ADXunhuanoffset, sloff, 64);	//正向索引
	printf("循环语音\r\n");
	debugdata(sloff, 64, 1);
	crc32 = __cpuCrc32(sloff, 60);
	if(memcmp((unsigned char*)&crc32, sloff+60, 4) == 0){
		for(i=0; i<60; i++){
			clr_wdt();
			if(sloff[i] < ADInfoMaxNum){
				printf("语音:%d:", i+1);
				flashread(ADInfooffset+(sloff[i]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
		}
	}
	else{
		printf("校验错:%08X\r\n", crc32);
	}
	
	flashread(ADZhengxiangoffset, sloff, 256);	//正向索引
	printf("正向\r\n");
	debugdata(sloff, 256, 1);
	crc32 = __cpuCrc32(sloff, 252);
	if(memcmp((unsigned char*)&crc32, sloff+252, 4) == 0){
		for(i=0; i<ADZhanDianMaxNum; i++){
			clr_wdt();
			if(sloff[i*2] < ADInfoMaxNum){
				printf("站点:%d 头:", i+1);
				flashread(ADInfooffset+(sloff[i*2]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
			if(sloff[i*2+1] < ADInfoMaxNum){
				printf("站点:%d 尾:", i+1);
				flashread(ADInfooffset+(sloff[i*2+1]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
		}
	}
	else{
		printf("校验错:%08X\r\n", crc32);
	}
	
	flashread(ADFanxiangoffset, sloff, 256);	//反向索引
	printf("反向\r\n");
	debugdata(sloff, 256, 1);
	crc32 = __cpuCrc32(sloff, 252);
	if(memcmp((unsigned char*)&crc32, sloff+252, 4) == 0){
		for(i=0; i<ADZhanDianMaxNum; i++){
			clr_wdt();
			if(sloff[i*2] < ADInfoMaxNum){
				printf("站点:%d 头:", i+1);
				flashread(ADInfooffset+(sloff[i*2]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
			if(sloff[i*2+1] < ADInfoMaxNum){
				printf("站点:%d 尾:", i+1);
				flashread(ADInfooffset+(sloff[i*2+1]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
		}
	}
	else{
		printf("校验错:%08X\r\n", crc32);
	}
	
}

