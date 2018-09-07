

#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"
#include "DEF8583.h"

#include "szct.h"


//�̶�λ��Ϊ�������ļ���0x1000����CPU��λ������Ӧ�ó������ʼλ�ü���0x1000.
char  const  MachineCode[64]  __attribute__ ((at(0x8010000+0x1000))) = "��ά�복�ػ�ESF3000L\x00";

extern stcomtime SysTime;
extern unsigned char restore_flag;

extern unsigned char Xor8(const unsigned char *pData, unsigned long DataLen);
extern void gprs_send_CMD(unsigned char cmd, unsigned int len, unsigned char *dat);
//������ ���ȴ�Ӧ�𣬲���Ӧ�����ݷ��ء�
extern int gprs_send_CMD_WaitACK(unsigned char cmd, unsigned int len, unsigned char *dat, unsigned char *odat);

//��Linuxģ���в��Һ�����,ǰ����ͨ��linux HTTPģʽ�Ѻ��������ص���LINUXģ����ļ��С�
unsigned int Linux4GFindBLK(unsigned char *sno)
{
	unsigned char sndbuf[16];
	int ret;
	
	memset(sndbuf, 0, sizeof(sndbuf));
	memcpy(sndbuf, SL8583FileFLAG_BLK, 3);
	sndbuf[3] = 10;	//LINUX�ڵĺ������ĵ���Ϊ10�ֽ�
	memcpy(sndbuf+4, sno, BLK_SNO_LEN);//����10�ֽڣ�����������油0
	
	ret = gprs_send_CMD_WaitACK(0x0A, 14, sndbuf, sndbuf);
	MSG_LOG("LinuxBLKF ret=%d\r\n", ret);
	if(ret >= 0){
		BCD_LOG(sndbuf, ret, 1);
		if(memcmp(sndbuf, "\x90\x00", 2) == 0){//�Ǻ�����
			return 0;
		}
	}
	return 0xFFFFFFFF;
}



//����Linuxģ���ļ��µ����ػ��У�ֻ��������ļ�
int DealComData0x2B(unsigned char *inbuf)
{//��������1�ֽڡ�1=ˢ����¼��2=��ǰ��¼�ļ���3=��ʷ��¼��4=���ػ�����ģ�鷢�ͣ���+�ļ�����(4�ֽڣ�+ƫ������4�ֽڣ�+���ݳ��ȣ�4�ֽڣ�+���ݣ�L)
	unsigned int addr, itmp, pos, flen, offset, dlen, i;
	unsigned char buftemp[64];
	pos = 0;
	if(inbuf[pos] != 4){//���ǳ����ļ�,��ǰֻ�г����ļ�����ʶΪ4
		return -1;
	}
	pos++;
	memcpy((unsigned char *)&flen, inbuf+pos, 4);	//�ļ�����
	pos += 4;
	memcpy((unsigned char *)&offset, inbuf+pos, 4);	//ƫ����
	pos += 4;
	memcpy((unsigned char *)&dlen, inbuf+pos, 4);	//���ݳ���
	pos += 4;

	if(offset == 0){//��һ��������Ҫ����
		clr_wdt();
		addr = FLASH_PRO_START;//д��ĵ�ַ ������ʼ��ַ
		for(i=0; i<7; i++){
#ifdef _debug_
			debugstring("��մ�����X:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 
			addr += 0x10000;//��һ��64K�Ŀ�
		}
		flasherase(ERASE4KBYTESECTOR, FLASH_PRO_FLAG);//�Ȳ�����־��4K
	}
	
	sprintf((char*)buftemp, "P:%d%% ",  (((offset+dlen)*100)/flen));
	miniDispstr(14, 0, (char*)buftemp, 0);
	addr = FLASH_PRO_START+offset;

	flashwrite(addr, inbuf+pos, dlen);	//д������

	MSG_LOG("%s, flen:%d, offset:%d, dlen:%d\r\n", __FUNCTION__, flen, offset, dlen);

	itmp = offset+dlen;
	if(itmp >= flen)//�������
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
		over_turn(2, buftemp+4);	//�������볤��
		buftemp[pos++] = Xor8(buftemp,pos);
		
#ifdef _debug_
		printf("%s:", __FUNCTION__);
		debugdata(buftemp,pos,1);
#endif
		com_snd(COM_GPRS, pos, buftemp);
		
		
		delayxs(10);
		
// 		feread(BIT_PRO_LINUX_CRC32, 4, (unsigned char*)&addr);//��ȡCRC32ֵ,,,��Ҫ�������ʱͨ��USB����ʱ�ò���CRC32��ֵ 
// 		crc32 = calFileCRC(FLASH_PRO_START, itmp);//����CRC32ֵ 
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
		
		flashread(FLASH_PRO_START+0x1000, buftemp, 64);	//������ʶ
		buftemp[63] = 0;
		if(memcmp(MachineCode, buftemp, strlen(MachineCode)) != 0){//����������ػ��ĳ���
			cls();
			MessageBox(1, "���Ǳ��豸����");
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
		memcpy(buftemp,"������Ҫ����", 12);
		memcpy(buftemp+12,gsl8583FileDownPara.Miss_ver, 2);
		memcpy(buftemp+14, (unsigned char*)&SysTime, 7);
#ifdef _debug_
		debugstring("�����������:");
		debugdata(buftemp, 21, 1);
#endif
		display(3,0,"���,������...",DIS_CENTER);
		addr = FLASH_PRO_FLAG;
		
#ifdef _debug_
		debugstring("�����������2:");
		debugdata(buftemp, 34, 1);
#endif
		flashwrite(addr, buftemp, 64);
		
		beep(1,10,10);
		delayxms(400);
		ReStartCPU();//������ɣ������豸�Ա���³���
		while(1);
	}
	return 0;
}






