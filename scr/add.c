

/**************************************************
* �ļ��� :add.c
* ����   :
* ����   :
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
//�������ʽ��ǰ������
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
	memcpy(buff, "BDBD�˿��Ѿ�����! ", 18);
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
//ȡ��¼ָ��
//������Ҫд���ָ��,��������ͷָ��
//mode == 1 ֻ�����Ƿ���
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
	for(uit=0; uit<10; uit++){//ȡ����ʶ����ʶ���ĳ��Ⱥͼ�¼����һ��
		flashread(temp, buff, 64);
		flashread(temp, tempbuf, 64);
		if(memcmp(buff, tempbuf, 64) == 0)
			break;
	}
	if(uit >= 10){
		debugstring("��ʶ��������:");
	}
	crc32 = GenerateCRC32(buff, 60);
	memcpy((unsigned char*)&uit, buff+60, 4);
	
// #ifdef _debug_
// 	debugstring("G Crc32:");
// 	debugdata((unsigned char *)&crc32, 4, 1);
// 	debugdata(buff, 64, 1);
// #endif

	if((crc32 == uit)&&(memcmp(buff, "BDBD�˿��Ѿ�����! ", 18) == 0)){//�Ѿ�����
		if(mode == 0x5a){//ֻ��Ҫ�����Ƿ��пռ䣬�鿴����ַ����
			temp = FLASH1_START_O+FLASH_MAX_O;//�鿴5100����¼�Ƿ���ȫFF,���Ǿ���Ϊ������
			memset(buff, 0, RECORDLEN);
			flashread(temp, buff, RECORDLEN);
			if(RecRomCheckFF_O(buff, RECORDLEN) > (RECORDLEN-4)){//ֻҪ����ȫ�������в���FF��С��4�������ǿյ�
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
	else{//û�����ã�����
#ifdef _debug_
		debugstring("����");
#endif
		temp = 0;
		for(uit=0; uit<16; uit++){//320KΪ5��
			flasherase(ERASE64KBYTESECTOR, temp);
			temp+=0x10000;
			Delay(2);
			//delayxms(2);
		}
		memset(buff, 0xEE, 64);
		memcpy(buff, "BDBD�˿��Ѿ�����! ", 18);
		crc32 = GenerateCRC32(buff, 60);
		memcpy(buff+60, (unsigned char*)&crc32, 4);
		flashwrite(FLASH1_START_O, buff, 64);

		temp = FLASH1_START_O+RECORDLEN;
		memcpy(headp, (unsigned char*)&temp, 4);
		return temp;
	}
//���ҵ�ǰ��Ҫд��ĵ�ַ
	temp = FLASH1_START_O+64;
	memset(tempbuf, 0xff, RECORDLEN);
	memset(pbuf0, 0, RECORDLEN);
	addrF = 0;
	firstdar = 0;
	curdar = 0;
	enddar = ((LEN_EEPROM_O/RECORDLEN)-50); //FLASH_REC_MAX;
	while(1){//���ҵ�ǰָ��
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
			if(firstdar==curdar)//�Ѿ��ҵ�
				break;
			else
			{
				enddar=curdar;
			}
		}
		else{
			if(enddar==curdar)//�Ѿ��ҵ�,û�пռ��ˡ���Ҫ�ɼ�
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
	current = temp; //���ǵ�ǰָ��
	
// #ifdef _debug_
// 	debugstring("----------------------\r\ntemp:");
// 	debugdata((unsigned char *)&temp, 4, 1);
// 	debugstring("----------------------\r\n");
// #endif
//	����Ҫ��ͷָ��
	addrF = 0;
	firstdar = 0;
	curdar = 0;
	enddar = current - (FLASH1_START_O+64);
	enddar /= RECORDLEN; //��ǰ������

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
			if(firstdar==curdar)//�Ѿ��ҵ�
				break;
			else
			{
				enddar=curdar;
			}
		}
		else{
			if(enddar==curdar)//�Ѿ��ҵ�,û�пռ��ˡ���Ҫ�ɼ�
				break;
			else
			{
				firstdar=curdar+1;
			}
		}

	}
	addrF = temp;
	if(addrF == 0)
		addrF = 64;	//��ʶ������64�ֽ�
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
	MSG_LOG("��ǰָ��=%04x\r\n",temp);
	memcpy((unsigned char *)&itt, (unsigned char*)&buff, 4);
	MSG_LOG("ͷָ��=%04x\r\n",itt);
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
		else{//д������дȫ01������дʵ���ݣ���д����һ���ݵ�ַ
#ifdef _debug_
			debugstring("write record ERROR!!!\r\n");
			debugdata((unsigned char*)&temp, 4, 1);
#endif
			//			spiInit();
			//			delayxms(2);
			if(RecRomCheckFF_O(buff, RECORDLEN) <= (RECORDLEN-4)){//����ȫFF��д�˲��֣���ûд�ԡ���ַ�ۼ�
#ifdef _debug_
				debugstring("@@@@@@@@@@@@@@@@@@@@");
				debugdata(buff, RECORDLEN, 1);
#endif
				memset(buff, 0x01, RECORDLEN);
				flashwrite(temp, buff, RECORDLEN);
				temp += RECORDLEN;
				delayxms(2);
			}
			else{//����ȫFF��ûд��ȥ�����ۼӵ�ַ
				if(i>0)
					i--;	//���Բ�ͣ��д�����д�˺���ȫFF.
				itt++;
				if(itt > 10){	//д10�β��ɹ������˳���
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
	if(addr < FLASH2_START1_O){//��ǰ����д0,��ʷ����д0
		memset(disbuf, 0, RECORDLEN);
		flashwrite(addr, disbuf, RECORDLEN);//������д��ȫ0��
		
		delayxms(1);
		memset(disbuf_1,0xff,RECORDLEN);
		flashread(addr,disbuf_1,RECORDLEN);
		MSG_LOG("������==����\r\n");
		BCD_LOG(disbuf_1,RECORDLEN,1);
		if (memcmp(disbuf,disbuf_1,RECORDLEN)!=0)
		{
			MSG_LOG("дʧ��\r\n");
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
		//�����¼������û�м�¼�ɷ�������E2PROM��û�з��ɹ��ļ�¼
		if (curp > headp)
		{
			sum = (curp - headp) / RECORDLEN;	
		}
		else {
			sum = 0;
			if (curp >= (FLASH1_START_O + FLASH_RCLR_O)) {//0x3E800�����¼��ַ�Ѿ��ﵽ��40000����64�ֽڣ��������ô����������¿�ʼ
				//clr_Record_point();
				lib_clr_Record_point_O();//ԭ���õ��� clr_Record_point();   ���������ѱ������ĵ�ַҲ�������
				
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
	case ODA_FeRC_Init://��ʼ��
		MSG_LOG("��ʼ��ODAĿ¼�ṹ\n ");
		lib_clr_Record_point_O();

		break;
	case ODA_FeRC_Write://д��¼
		MSG_LOG("дFLAHS ODA8583��¼:");
		re_inforLen = GET_INT16(re_infor + 2) + 4;
		MSG_LOG("ODA��¼:%d\n", re_inforLen);
		if (re_inforLen > 512) {
			MSG_LOG("ODA��¼�޷�д�볬��512B�ļ�¼:%d\n", re_inforLen);
			retCode = Ret_Err_Overflow;
			break;
		}
		libWriteRecord_O(re_infor);
		break;
	case ODA_FeRC_Dlelt://ɾ��
		MSG_LOG("ɾODA8583��¼\n ");
	
		if (FeRC_Dlelt_O()==1) {
		
			retCode = Ret_Err_Format;
			break;
		}

		break;
	case ODA_FeRC_Check://�鿴�Ƿ��м�¼
		//MSG_LOG("C");

		
		if (GJRec_Send_O() !=0 ) {
			MSG_LOG("ODA�м�¼\r\n");
			retCode = Ret_YES;
		}
		else
		{
			//MSG_LOG("�鿴�Ƿ��м�¼\r\n");
			retCode = Ret_NO;
			break;
		}
		break;
	case ODA_FeRC_READ://

		MSG_LOG("����¼:");
		RECORDLEN = getRECORD_LEEN_O();
		curp = Get_Record_point_O((unsigned char*)&headp, 0);
		flashread(headp, re_infor, RECORDLEN);
		BCD_LOG(re_infor, RECORDLEN, 1);
		re_inforLen = GET_INT16(re_infor + 2) + 4;
		if (re_inforLen > 512) {
			MSG_LOG("ODA��¼������512 B�ļ�¼:%d\n", re_inforLen);
			retCode = Ret_Err_Overflow;
			break;
		}
		break;
	default:
			MSG_LOG("����\r\n");
		break;
	}

	return retCode;
}
