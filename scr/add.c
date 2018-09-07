

/**************************************************
* 文件名 :add.c
* 描述   :
* 建立   :
**************************************************/
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"

#include "citydifferent.h"
#include "SL8583.h"
#ifdef qPBOC_BUS
#include "qpboc_head.h"
#endif
#include "qpboc_8583.h"
#include "add.h"
#include "ProcCharacter.h"

unsigned int getRECORD_LEEN_O(void)
{
	return RECORD_LEN_O ;
}
//清除并格式当前存贮区
void lib_clr_Record_point_O(void)
{
	unsigned char buff[128];
	unsigned int crc32;
	unsigned int temp,uit;
#ifdef _debug_
	debugstring("lib Clr Record!!\r\n");
#endif
#ifndef ODA
return ;
#endif
	temp =FLASH1_START_O;
	for(uit=0; uit<7; uit++){
		clr_wdt();
		flasherase(ERASE64KBYTESECTOR, temp);
		temp+=0x10000;
		delayxms(2);
	}
	memset(buff, 0xEE, 64);
	memcpy(buff, "BDBD此块已经启用! ", 18);
	//	debugstring("lib Clr 1 \r\n");
	crc32 = GenerateCRC32(buff, 60);
	//	debugstring("lib Clr 2 \r\n");
	memcpy(buff+60, (unsigned char*)&crc32, 4);
	flashwrite(FLASH1_START_O, buff, 64);
	//	debugstring("lib Clr 3 \r\n");
	return;
}
unsigned int RecRomCheckFF_O(unsigned char *datt, unsigned int len)
{
	unsigned int i;
	unsigned int counter = 0;
	
	for(i=0; i<len; i++){
		if(datt[i] == 0xFF){
			counter++;
		}
	}
	return counter;
}
unsigned int RecRomCheck00_O(unsigned char *datt, unsigned int len)
{
	unsigned int i;
	unsigned int counter = 0;
	
	for(i=0; i<len; i++){
		if(datt[i] == 0){
			counter++;
		}
	}
	return counter;
}
//取记录指针
//返回需要写入的指针,参数返回头指针
//mode == 1 只返回是否满
int Get_Record_point_O(unsigned char *headp, unsigned char mode)
{
	unsigned char buff[RECORD_LEN_O];
	unsigned char tempbuf[RECORD_LEN_O];
	unsigned char pbuf0[RECORD_LEN_O];
	unsigned int crc32;
	unsigned int temp,uit;
	unsigned int addrF,current;
	unsigned int curdar,enddar,firstdar;
	unsigned int RECORDLEN;
	
	RECORDLEN = getRECORD_LEEN_O();
#ifndef ODA
return Ret_OK;
#endif
	temp=FLASH1_START_O;
	for(uit=0; uit<10; uit++){//取出标识，标识区的长度和记录长度一致
		flashread(temp, buff, 64);
		flashread(temp, tempbuf, 64);
		if(memcmp(buff, tempbuf, 64) == 0)
			break;
	}
	if(uit >= 10){
		debugstring("标识读出错误:");
	}
	crc32 = GenerateCRC32(buff, 60);
	memcpy((unsigned char*)&uit, buff+60, 4);
	
// #ifdef _debug_
// 	debugstring("G Crc32:");
// 	debugdata((unsigned char *)&crc32, 4, 1);
// 	debugdata(buff, 64, 1);
// #endif

	if((crc32 == uit)&&(memcmp(buff, "BDBD此块已经启用! ", 18) == 0)){//已经启用
		if(mode == 0x5a){//只需要返回是否还有空间，查看最后地址就行
			temp = FLASH1_START_O+FLASH_MAX_O;//查看5100条记录是否是全FF,不是就认为是满的
			memset(buff, 0, RECORDLEN);
			flashread(temp, buff, RECORDLEN);
			if(RecRomCheckFF_O(buff, RECORDLEN) > (RECORDLEN-4)){//只要大于全部数据中不是FF的小于4个就算是空的
				return 0xff;
			}
			else{
				temp += RECORDLEN;
				memset(buff, 0, RECORDLEN);
				flashread(temp, buff, RECORDLEN);
				if(RecRomCheckFF_O(buff, RECORDLEN) > (RECORDLEN-4)){
					return 0xff;
				}
				return -2;
			}
		}
	}
	else{//没有启用，擦除
#ifdef _debug_
		debugstring("擦除");
#endif
		temp = 0;
		for(uit=0; uit<16; uit++){//320K为5块
			flasherase(ERASE64KBYTESECTOR, temp);
			temp+=0x10000;
			Delay(2);
			//delayxms(2);
		}
		memset(buff, 0xEE, 64);
		memcpy(buff, "BDBD此块已经启用! ", 18);
		crc32 = GenerateCRC32(buff, 60);
		memcpy(buff+60, (unsigned char*)&crc32, 4);
		flashwrite(FLASH1_START_O, buff, 64);

		temp = FLASH1_START_O+RECORDLEN;
		memcpy(headp, (unsigned char*)&temp, 4);
		return temp;
	}
//查找当前需要写入的地址
	temp = FLASH1_START_O+64;
	memset(tempbuf, 0xff, RECORDLEN);
	memset(pbuf0, 0, RECORDLEN);
	addrF = 0;
	firstdar = 0;
	curdar = 0;
	enddar = ((LEN_EEPROM_O/RECORDLEN)-50); //FLASH_REC_MAX;
	while(1){//先找当前指针
		curdar = ((enddar-firstdar)/2)+firstdar;
		temp = curdar*RECORDLEN;
		temp += (FLASH1_START_O+64);

		clr_wdt();
		flashread(temp, buff, RECORDLEN);
// #ifdef _debug_
// 		debugstring("temp:");
// 		debugdata((unsigned char *)&temp, 4, 1);
// 		debugstring("firstdar:");
// 		debugdata((unsigned char *)&firstdar, 4, 1);
// 		debugstring("curdar:");
// 		debugdata((unsigned char *)&curdar, 4, 1);
// 		debugstring("enddar:");
// 		debugdata((unsigned char *)&enddar, 4, 1);
// 		debugstring("buff:");
// 		debugdata(buff, RECORDLEN, 1);
// #endif
		if(RecRomCheckFF_O(buff, RECORDLEN) > (RECORDLEN-4)){
//		if(memcmp(buff, tempbuf, RECORDLEN) == 0){
			if(firstdar==curdar)//已经找到
				break;
			else
			{
				enddar=curdar;
			}
		}
		else{
			if(enddar==curdar)//已经找到,没有空间了。需要采集
				break;
			else
			{
				firstdar=curdar+1;
			}
		}
	}
	
// #ifdef _debug_
// 	debugstring("temp  temp:");
// 	debugdata((unsigned char *)&temp, 4, 1);
// #endif
	current = temp; //这是当前指针
	
// #ifdef _debug_
// 	debugstring("----------------------\r\ntemp:");
// 	debugdata((unsigned char *)&temp, 4, 1);
// 	debugstring("----------------------\r\n");
// #endif
//	还需要找头指针
	addrF = 0;
	firstdar = 0;
	curdar = 0;
	enddar = current - (FLASH1_START_O+64);
	enddar /= RECORDLEN; //当前条数。

	while(1){
		curdar = ((enddar-firstdar)/2)+firstdar;
		temp = curdar*RECORDLEN;
		temp += (FLASH1_START_O+64);
		
		clr_wdt();
		flashread(temp, buff, RECORDLEN);
// #ifdef _debug_
// 		debugstring("temp:");
// 		debugdata((unsigned char *)&temp, 4, 1);
// 		debugstring("firstdar:");
// 		debugdata((unsigned char *)&firstdar, 4, 1);
// 		debugstring("curdar:");
// 		debugdata((unsigned char *)&curdar, 4, 1);
// 		debugstring("enddar:");
// 		debugdata((unsigned char *)&enddar, 4, 1);
// 		debugstring("buff:");
// 		debugdata(buff, RECORDLEN, 1);
// #endif
		if(RecRomCheck00_O(buff, RECORDLEN) < (RECORDLEN-2)){
//		if(memcmp(buff, pbuf0, RECORDLEN) != 0){
			if(firstdar==curdar)//已经找到
				break;
			else
			{
				enddar=curdar;
			}
		}
		else{
			if(enddar==curdar)//已经找到,没有空间了。需要采集
				break;
			else
			{
				firstdar=curdar+1;
			}
		}

	}
	addrF = temp;
	if(addrF == 0)
		addrF = 64;	//标识区长度64字节
	memcpy(headp, (unsigned char*)&addrF, 4);

// #ifdef _debug_
// 	debugstring("headp point:");
// 	debugdata(headp, 4, 1);
// 	debugstring("current point:");
// 	debugdata((unsigned char*)&current, 4, 1);
// #endif
	return current;
}

unsigned int cal_space_O(void)
{
	int headp, curp;
	
	curp = Get_Record_point_O((unsigned char*)&headp, 0x5a);
	if (curp == -2)
		return 0;
	else
		return 0xff;
}

unsigned char libWriteRecord_O( unsigned char *buffer)
{
	unsigned char i=0;
	unsigned char buff[RECORD_LEN_O];
	unsigned int temp, itt=0;
	unsigned int RECORDLEN;
	 MSG_LOG("do %s\r\n",__FUNCTION__);
	 #ifndef ODA
return Ret_OK;
#endif
	RECORDLEN = getRECORD_LEEN_O();
	
	memset(buff, 0x00, RECORDLEN);
	memcpy(buff, buffer, RECORDLEN);
	
	temp = Get_Record_point_O(buff, 0);
	
#ifdef _debug_
	debugstring("-------------------\r\ncur:");

	debugdata((unsigned char*)&temp, 4, 1);	
	MSG_LOG("当前指针=%04x\r\n",temp);
	memcpy((unsigned char *)&itt, (unsigned char*)&buff, 4);
	MSG_LOG("头指针=%04x\r\n",itt);
	flashread(temp, buff, RECORDLEN);
	debugdata(buff, RECORDLEN, 1);
#endif
	
	for(i=0; i<3; i++) 
	{
		flashwrite(temp, buffer, RECORDLEN);
#ifdef _debug_
		debugstring("-------------------\r\ncur:");
		debugdata((unsigned char*)&temp, 4, 1);
		debugstring("WriteRecord : \r\n");		
		debugdata(buffer,RECORDLEN,1);
#endif
		
		flashread(temp, buff, RECORDLEN);
#ifdef _debug_
		debugstring("WriteRecordCheck ulofset2: \r\n");
		debugdata(buff, RECORDLEN, 1);
#endif
		if(memcmp(buffer, buff, RECORDLEN) == 0) 
		{
		
			return 0;
		}
		else{//写错，正区写全01，副区写实数据，并写到下一数据地址
#ifdef _debug_
			debugstring("write record ERROR!!!\r\n");
			debugdata((unsigned char*)&temp, 4, 1);
#endif
			//			spiInit();
			//			delayxms(2);
			if(RecRomCheckFF_O(buff, RECORDLEN) <= (RECORDLEN-4)){//不是全FF，写了部分，但没写对。地址累加
#ifdef _debug_
				debugstring("@@@@@@@@@@@@@@@@@@@@");
				debugdata(buff, RECORDLEN, 1);
#endif
				memset(buff, 0x01, RECORDLEN);
				flashwrite(temp, buff, RECORDLEN);
				temp += RECORDLEN;
				delayxms(2);
			}
			else{//还是全FF，没写进去，不累加地址
				if(i>0)
					i--;	//测试不停的写，如果写了后还是全FF.
				itt++;
				if(itt > 10){	//写10次不成功，就退出。
					i = 3;
					break;
				}
				delayxms(1);
#ifdef _debug_
				debugstring("$$$$$$$$$$$$$$$$$$$");
				debugdata((unsigned char*)&itt, 4, 1);
#endif
			}
		}
	}

	cls();
	display(2, 2, "Data Flash Error!", 1);
	memset(buffer, 0, 20);
	memcpy(buffer, "ADDR:", 5);
	BCD2Ascii((unsigned char *)&temp, buffer+5, 4);
	display(6,0,(char*)buffer, 0);
	led_on(LED_RED|LED_GREEN);
	for(;;)
		delayxms(50);
}




unsigned char FeRC_Dlelt_O(void)
{
	unsigned int curp=0, headp=0;
	unsigned int RECORDLEN=0;
	unsigned int addr;
	unsigned char disbuf[RECORD_LEN_O],disbuf_1[RECORD_LEN_O];
	RECORDLEN = getRECORD_LEEN_O();
	#ifndef ODA
return Ret_OK;
#endif
	curp = Get_Record_point_O((unsigned char*)&headp, 0);
	addr=headp;
	if(addr < FLASH2_START1_O){//当前区才写0,历史区不写0
		memset(disbuf, 0, RECORDLEN);
		flashwrite(addr, disbuf, RECORDLEN);//把这条写成全0，
		
		delayxms(1);
		memset(disbuf_1,0xff,RECORDLEN);
		flashread(addr,disbuf_1,RECORDLEN);
		MSG_LOG("读出来==数据\r\n");
		BCD_LOG(disbuf_1,RECORDLEN,1);
		if (memcmp(disbuf,disbuf_1,RECORDLEN)!=0)
		{
			MSG_LOG("写失败\r\n");
			return 1;
		}
	}

	return 0;
}
int GJRec_Send_O(void)
{

	unsigned int sum;
	unsigned int curp, headp;
	unsigned int RECORDLEN;
	
	RECORDLEN = getRECORD_LEEN_O();
	#ifndef ODA
return Ret_OK;
#endif
		curp = Get_Record_point_O((unsigned char*)&headp, 0);
		//如果记录缓冲中没有记录可发将发送E2PROM中没有发成功的记录
		if (curp > headp)
		{
			sum = (curp - headp) / RECORDLEN;	
		}
		else {
			sum = 0;
			if (curp >= (FLASH1_START_O + FLASH_RCLR_O)) {//0x3E800如果记录地址已经达到了40000条（64字节），则重置存贮区，重新开始
				//clr_Record_point();
				lib_clr_Record_point_O();//原来用的是 clr_Record_point();   这个函数会把备份区的地址也清掉。。
				
			}
			
		}
	
	return sum;
}


int save_ODA_infor(unsigned char mode, unsigned char *re_infor) {


	unsigned char buff[512];
	unsigned int addr = 0;
	unsigned int curp=0, headp=0;
	unsigned int RECORDLEN=0;
	int re_inforLen = 0;
	//	int tmpI = 0;
	//	stTlv tlvCardStyle;
#ifndef ODA
return Ret_OK;
#endif
	int retCode = Ret_OK;
	if (mode != ODA_FeRC_Check) {
		MSG_LOG("save_repurse_infor(%02X):%d\n", mode, re_inforLen);
	}
	switch (mode)
	{
	case ODA_FeRC_Init://初始化
		MSG_LOG("初始化ODA目录结构\n ");
		lib_clr_Record_point_O();

		break;
	case ODA_FeRC_Write://写记录
		MSG_LOG("写FLAHS ODA8583记录:");
		re_inforLen = GET_INT16(re_infor + 2) + 4;
		MSG_LOG("ODA记录:%d\n", re_inforLen);
		if (re_inforLen > 512) {
			MSG_LOG("ODA记录无法写入超入512B的记录:%d\n", re_inforLen);
			retCode = Ret_Err_Overflow;
			break;
		}
		libWriteRecord_O(re_infor);
		break;
	case ODA_FeRC_Dlelt://删掉
		MSG_LOG("删ODA8583记录\n ");
	
		if (FeRC_Dlelt_O()==1) {
		
			retCode = Ret_Err_Format;
			break;
		}

		break;
	case ODA_FeRC_Check://查看是否有记录
		//MSG_LOG("C");

		
		if (GJRec_Send_O() !=0 ) {
			MSG_LOG("ODA有记录\r\n");
			retCode = Ret_YES;
		}
		else
		{
			//MSG_LOG("查看是否有记录\r\n");
			retCode = Ret_NO;
			break;
		}
		break;
	case ODA_FeRC_READ://

		MSG_LOG("读记录:");
		RECORDLEN = getRECORD_LEEN_O();
		curp = Get_Record_point_O((unsigned char*)&headp, 0);
		flashread(headp, re_infor, RECORDLEN);
		BCD_LOG(re_infor, RECORDLEN, 1);
		re_inforLen = GET_INT16(re_infor + 2) + 4;
		if (re_inforLen > 512) {
			MSG_LOG("ODA记录读出超512 B的记录:%d\n", re_inforLen);
			retCode = Ret_Err_Overflow;
			break;
		}
		break;
	default:
			MSG_LOG("其它\r\n");
		break;
	}

	return retCode;
}
