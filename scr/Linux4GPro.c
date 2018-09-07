

#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"
#include "DEF8583.h"

#include "szct.h"


//固定位置为二进制文件的0x1000处，CPU的位置内在应用程序的起始位置加上0x1000.
char  const  MachineCode[64]  __attribute__ ((at(0x8010000+0x1000))) = "二维码车载机ESF3000L\x00";

extern stcomtime SysTime;
extern unsigned char restore_flag;

extern unsigned char Xor8(const unsigned char *pData, unsigned long DataLen);
extern void gprs_send_CMD(unsigned char cmd, unsigned int len, unsigned char *dat);
//发送命 令后等待应答，并把应答数据返回。
extern int gprs_send_CMD_WaitACK(unsigned char cmd, unsigned int len, unsigned char *dat, unsigned char *odat);

//在Linux模块中查找黑名单,前提是通过linux HTTP模式把黑名单下载到了LINUX模块的文件中。
unsigned int Linux4GFindBLK(unsigned char *sno)
{
	unsigned char sndbuf[16];
	int ret;
	
	memset(sndbuf, 0, sizeof(sndbuf));
	memcpy(sndbuf, SL8583FileFLAG_BLK, 3);
	sndbuf[3] = 10;	//LINUX内的黑名单的单条为10字节
	memcpy(sndbuf+4, sno, BLK_SNO_LEN);//都发10字节，如果不够后面补0
	
	ret = gprs_send_CMD_WaitACK(0x0A, 14, sndbuf, sndbuf);
	MSG_LOG("LinuxBLKF ret=%d\r\n", ret);
	if(ret >= 0){
		BCD_LOG(sndbuf, ret, 1);
		if(memcmp(sndbuf, "\x90\x00", 2) == 0){//是黑名单
			return 0;
		}
	}
	return 0xFFFFFFFF;
}



//处理Linux模块文件下到车载机中，只处理程序文件
int DealComData0x2B(unsigned char *inbuf)
{//数据类型1字节【1=刷卡记录、2=当前记录文件、3=历史记录、4=车载机程序（模块发送）】+文件长度(4字节）+偏移量（4字节）+数据长度（4字节）+数据（L)
	unsigned int addr, itmp, pos, flen, offset, dlen, i;
	unsigned char buftemp[64];
	pos = 0;
	if(inbuf[pos] != 4){//不是程序文件,当前只有程序文件，标识为4
		return -1;
	}
	pos++;
	memcpy((unsigned char *)&flen, inbuf+pos, 4);	//文件长度
	pos += 4;
	memcpy((unsigned char *)&offset, inbuf+pos, 4);	//偏移量
	pos += 4;
	memcpy((unsigned char *)&dlen, inbuf+pos, 4);	//数据长度
	pos += 4;

	if(offset == 0){//第一个包，需要擦除
		clr_wdt();
		addr = FLASH_PRO_START;//写入的地址 程序起始地址
		for(i=0; i<7; i++){
#ifdef _debug_
			debugstring("清空存贮器X:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//先擦除 
			addr += 0x10000;//下一个64K的块
		}
		flasherase(ERASE4KBYTESECTOR, FLASH_PRO_FLAG);//先擦除标志区4K
	}
	
	sprintf((char*)buftemp, "P:%d%% ",  (((offset+dlen)*100)/flen));
	miniDispstr(14, 0, (char*)buftemp, 0);
	addr = FLASH_PRO_START+offset;

	flashwrite(addr, inbuf+pos, dlen);	//写入数据

	MSG_LOG("%s, flen:%d, offset:%d, dlen:%d\r\n", __FUNCTION__, flen, offset, dlen);

	itmp = offset+dlen;
	if(itmp >= flen)//下载完成
	{
		memset(buftemp, '$', 4);
		pos = 4;
		buftemp[pos++] = 0; 
		buftemp[pos++] = 9;
		buftemp[pos++] = 0x2B;
		buftemp[pos++] = 0;
		
		buftemp[pos++] = 0x90;
		buftemp[pos++] = 0x00;
		i = pos+1;
		memcpy(buftemp+4, (unsigned char*)&i, 2);
		over_turn(2, buftemp+4);	//重新填入长度
		buftemp[pos++] = Xor8(buftemp,pos);
		
#ifdef _debug_
		printf("%s:", __FUNCTION__);
		debugdata(buftemp,pos,1);
#endif
		com_snd(COM_GPRS, pos, buftemp);
		
		
		delayxs(10);
		
// 		feread(BIT_PRO_LINUX_CRC32, 4, (unsigned char*)&addr);//读取CRC32值,,,不要这个，有时通过USB下载时得不到CRC32的值 
// 		crc32 = calFileCRC(FLASH_PRO_START, itmp);//计算CRC32值 
// 		MSG_LOG("file len:%d\r\n",itmp);
// 		for(i=0;i<5;i++){
// 			MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32, addr);
// 			if(crc32 != addr)
// 				crc32 = calFileCRC(FLASH_TEMP_START,itmp);
// 			else
// 				break;
// 		}
// 		
// 		if (i>=5)
// 		{
// 			return 0;
// 		}
		
		flashread(FLASH_PRO_START+0x1000, buftemp, 64);	//读出标识
		buftemp[63] = 0;
		if(memcmp(MachineCode, buftemp, strlen(MachineCode)) != 0){//不是这个车载机的程序
			cls();
			MessageBox(1, "不是本设备程序");
			display(6, 0, (char*)buftemp, DIS_CENTER);
			beep(5, 50, 50);
			restore_flag = 3;
			while(1){
				if(getkey(1) == KEY_ENT){
					return 1;
				}
				delayxms(50);
			}
		}

		memset(buftemp, 0, 64);
		memcpy(buftemp,"程序需要更新", 12);
		memcpy(buftemp+12,gsl8583FileDownPara.Miss_ver, 2);
		memcpy(buftemp+14, (unsigned char*)&SysTime, 7);
#ifdef _debug_
		debugstring("程序下载完成:");
		debugdata(buftemp, 21, 1);
#endif
		display(3,0,"完成,请重启...",DIS_CENTER);
		addr = FLASH_PRO_FLAG;
		
#ifdef _debug_
		debugstring("程序下载完成2:");
		debugdata(buftemp, 34, 1);
#endif
		flashwrite(addr, buftemp, 64);
		
		beep(1,10,10);
		delayxms(400);
		ReStartCPU();//下载完成，重启设备以便更新程序。
		while(1);
	}
	return 0;
}






