
/*
ADSound.c
����GPS������վʱ��д������ͷ������β��
��������λ���� 0x3B0000-0x3B8000 ��32K��
���嶨�忴�����ػ������������.xlsx��
*/
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "Font.h"
#include "StructDef.h"

extern stcomtime SysTime;

//�ļ�ͷƫ����
#define ADHeadoffset FLASH_GPS_AD_START
//ѭ������������ַƫ����
#define ADXunhuanoffset (ADHeadoffset+64)
//����������ַƫ����
#define ADZhengxiangoffset (ADXunhuanoffset+64)
//����������ַƫ����
#define ADFanxiangoffset (ADZhengxiangoffset+256)
//�ı���Ϣƫ����
#define ADInfooffset (ADFanxiangoffset+256)

//������Ϣ���250��
#define ADInfoMaxNum 250
//վ���������125��
#define ADZhanDianMaxNum 125

//��ȡһ�У�һ�в�����len�ֽ�,�����rbuff���ȱ����LENҪ��1
unsigned int FlashGetLine(unsigned int addr, unsigned char*rbuff, unsigned int len)
{
	char *pe;

	memset(rbuff, 0, len);//��Ҫ�и�������
	flashread(addr, (unsigned char*)rbuff, len);

	pe = strstr((char *)rbuff, "\r\n");
	if(pe != NULL){
		pe += 2;
	}
	else{//û�ҵ��س����з������е����ݶ��㲢����
		pe = (char*)rbuff+len;
	}
	*pe = 0;	//��һ��������
	return (pe-(char*)rbuff);
}

//������ʱ�����ļ�������ʷԭ�򣬴���ʱ��λ�ڣ�FLASH_GPS_SOUND_START��
//����������ת�湫Կ�ġ����Դ��������Ҫ�Ѵ�����������õ���Կ�����������ء�
unsigned char DealADTmpFile(void)
{
	
	unsigned int addr;
	unsigned int infaddr;//��ǰд����Ϣ����š�ÿ����Ϣ����100�ֽڡ�
	char buff[260];
	unsigned short slineNo;

	unsigned char XunHuanbuf[64];//ѭ���������
	unsigned char ZDbuff[2][125][2];//��(��)��վ���ͷ����λ����ţ���β����λ����š���250�ֽڡ�����ʱ256�ֽڣ�������ֽ�FF��4�ֽ�CRC32�����Ϊ0�����250ʱ����Ϊ��վû���Զ�������
	unsigned short usVer;
	unsigned char ZFflag=0;//�������־��Ĭ������
	unsigned char zdflag=0;//�Ƿ�������ͷ��β��־��0=ͷ��1=β��

	char Ver[16];
	unsigned int i, uiLen, t, crc32;
	char *p, *p1;

	memset(buff, 0, sizeof(buff));
	flashread(FLASH_GPS_SOUND_START, (unsigned char*)buff, 64);
	if(memcmp(buff, "��������", 8) != 0){//��ADָ��ͷ�ļ�
		return ST_ERROR;
	}
	cls();
	display(0,0,"AD�ļ�����!",DIS_CENTER|DIS_CONVERT);
	
	MSG_LOG("OtheerFiles Ver:");
	MSG_LOG((char*)buff);

	p1 = strstr((char*)buff, "�ļ��汾");
	if(p1 == NULL){//�ļ�û�а汾�����Ϸ�
		display(2, 0, "���ļ��汾��", DIS_CENTER);
		beep(1, 50, 50);
		delayxms(200);
		return ST_ERROR;
	}
	memset(Ver, 0, sizeof(Ver));
	p = Ver;
	for(i=0; i<32; i++){
		if((*p1 >= '0')&&(*p1 <= '9')){//��Чֵ
			*p++ = *p1;
		}
		else if(p != Ver){//�Ѿ�ȡ�ð汾�ţ������ǰ汾�Ž��������ûȡ����˵����û�е��汾�ŵĵط�
			break;
		}
		p1++;
	}
	usVer = __atol(Ver);
	if(i == 0){//���Ϸ�
		display(2, 0, "�汾�Ų���Ϊ0", DIS_CENTER);
		beep(1, 50, 50);
		delayxms(200);
		return ST_ERROR;
	}

	
	p1 = strstr((char*)buff, "��·��");
	if(p1 == NULL){//�ļ�û�а汾�����Ϸ�
		display(2, 0, "����·��", DIS_CENTER);
		beep(1, 50, 50);
		delayxms(200);
		return ST_ERROR;
	}
	memset(Ver, 0, sizeof(Ver));
	p1 += 6;
	p = Ver;
	for(i=0; i<16; i++){
		if((*p1 >= '0')&&(*p1 <= '9')){//��Чֵ
			*p++ = *p1;
		}
		else if(p != Ver){//�Ѿ�ȡ�����֣����ûȡ����˵����û�е��汾�ŵĵط�
			break;
		}
		p1++;
	}
	slineNo = __atol(Ver);

	//�����Ǵ�����Ϣ

	flasherase(ERASE32KBYTESECTOR, ADHeadoffset);
	memset((unsigned char*)ZDbuff, 0xFF, sizeof(ZDbuff));
	memset(XunHuanbuf, 0xFF, sizeof(XunHuanbuf));

	infaddr = 0;
	for(addr=FLASH_GPS_SOUND_START; addr<FLASH_GPS_SOUND_END;)
	{
		memset(buff, 0, sizeof(buff));
		uiLen = FlashGetLine(addr, (unsigned char*)buff, 256);	//����һ�С�����һ�����ݵĳ���
		addr += uiLen;	//��ַָ����һ��

		MSG_LOG("GetLine i=%d\r\n", i);
		MSG_LOG(buff);

		if(memcmp(buff, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 10) == 0)//�ļ�����
			break;
		zdflag = 0xFF;//����վ��ͷҲ����վ��β��Ϣ
		p = strstr(buff, "����");
		if(p != NULL){//����������Ķ���
			MSG_LOG("���� ");
			ZFflag = 0;
		}
		else if(strstr(buff, "����") !=  NULL){//˵������Ķ����Ƿ���վ���
			MSG_LOG("���� ");
			ZFflag = 1;
		}
		else if(strstr(buff, "ѭ��") != NULL){
			MSG_LOG("ѭ�� ");
			ZFflag = 2;
		}
		else if(strstr(buff, "վ��ͷ") != NULL){
			MSG_LOG("վ��ͷ ");
			zdflag = 0;
		}
		else if(strstr(buff, "վ��β") != NULL){
			MSG_LOG("վ��β ");
			zdflag = 1;
		}
		else if(strstr(buff, "����:") != NULL){
			MSG_LOG("����: ");
			zdflag = 0;
		}
		else if(strstr(buff, "����") != NULL){
			MSG_LOG("���� ");
			break;
		}

		if(zdflag <= 1){//��վ��ͷ��β����Ҫ����Ϣ���д���
			p1 = strstr(buff, ":");	//�ҵ�һ��:�š�
			if(p1 == NULL){//û�У���һ��������Ч
				continue;	//������һ�����ݼ��
			}
			p1++;//������һ��:�ţ�����Ӧ�������ݡ�
			while(p1 <= (buff+uiLen)){//������
				memset(Ver, 0, sizeof(Ver));
				p = Ver;
				t = ((buff+uiLen)-p1);//��������ݳ���
				for(i=0; i<t; i++){
					if((*p1 >= '0')&&(*p1 <= '9')){//��Чֵ
						*p++ = *p1++;
					}
					else{//���������ˡ����Խ��д���
						break;
					}
				}
				if(p != Ver){//��������������������ƫ����д��
					i = __atol(Ver);
					if(ZFflag<2){
						if((i>0)&&(i <= 125)){//վ�Ŵ���125���ǷǷ��ģ����ұ���Ǵ�1��ʼ��
							ZDbuff[ZFflag][i-1][zdflag] = infaddr;
						}
					}
					else if(ZFflag == 2){	//��ѭ������
						if((i>0)&&(i<=60)){//ֻ����60������1��ʼ
							XunHuanbuf[i-1] = infaddr;
						}
					}

					MSG_LOG("����վ��=%d\r\n", i);
					MSG_LOG(buff);
				}
				if(*p1 == ':'){//�������������ʾ��������Ϣ�ˣ�Ҫд��������Ϣ�����ҽ������еĴ���
					p1++;
					t = uiLen-(p1-buff);//��Ϣ���ȡ�
					p1[t-1] = 0;	//����Ϣ����������������ǰѻس�����д��00�ˡ�
					p1[t-2] = 0;	//����Ϣ�����������
					flashwrite(ADInfooffset+(infaddr*100), (unsigned char*)p1, t);
					infaddr++;//ָ����һ��Ҫд�ĵ����

					MSG_LOG("д����Ϣ infaddr=%d\r\n", infaddr);
					MSG_LOG(buff);

					break;
				}
				else
					p1++;//��������������ֵ�λ�ã�ָ����һ��λ��
			}
		}
	}
	if(infaddr != 0){//�����ļ���������������д����Ϣ����Ѱ�ͷ�����д�룬����Ͳ��ø�����
		memset(buff, 0xFF, 64);
		memcpy(buff, "��������", 8);
		memcpy(buff+8, (unsigned char*)&usVer, 2);
		memcpy(buff+10, (unsigned char*)&SysTime, 7);
		memcpy(buff+17, (unsigned char*)&slineNo, 2);
		crc32 = __cpuCrc32((unsigned char*)buff, 60);
		memcpy(buff+60, (unsigned char*)&crc32, 4);
		flashwrite(ADHeadoffset, (unsigned char*)buff, 64);	//д���ļ�ͷ

		MSG_LOG("д���ļ�ͷ:");
		BCD_LOG((unsigned char*)buff, 64, 1);

		crc32 = __cpuCrc32(XunHuanbuf, 60);
		memcpy(XunHuanbuf+60, (unsigned char*)&crc32, 4);
		flashwrite(ADXunhuanoffset, XunHuanbuf, 64);
		MSG_LOG("д��ѭ������:");
		BCD_LOG((unsigned char*)XunHuanbuf, 64, 1);

		memcpy(buff, (unsigned char*)ZDbuff[0], 250);
		crc32 = __cpuCrc32((unsigned char*)buff, 252);
		memcpy(buff+252, (unsigned char*)&crc32, 4);
		flashwrite(ADZhengxiangoffset, (unsigned char*)buff, 256);	//д����������
		MSG_LOG("д����������:");
		BCD_LOG((unsigned char*)buff, 256, 1);
		
		memcpy(buff, (unsigned char*)ZDbuff[1], 250);
		crc32 = __cpuCrc32((unsigned char*)buff, 252);
		memcpy(buff+252, (unsigned char*)&crc32, 4);
		flashwrite(ADFanxiangoffset, (unsigned char*)buff, 256);	//д�뷴������
		MSG_LOG("д�뷴������:");
		BCD_LOG((unsigned char*)buff, 256, 1);
	}
	flasherase(ERASE32KBYTESECTOR, FLASH_GPS_SOUND_START);
	return 0;
}

//����0����ȷ����������,����������ADver �� ADlineNO��ʮ�����ƣ���λ��ǰ��ADdatetimeΪBCD�롣
int getADSoundFileVer(unsigned char *ADver, unsigned char *ADlineNO, unsigned char *ADdatetime)
{
	unsigned char buff[70];
	unsigned int xcrc32;
	
	memset(buff, 0, sizeof(buff));
	memset(ADver, 0, 2);

	flashread(FLASH_GPS_AD_START, (unsigned char*)buff, 64);
	
	MSG_LOG("AD Sound Ver:");
	BCD_LOG(buff, 64, 1);
	
	if(memcmp(buff, "��������", 8) != 0){
		return 1;
	}
	xcrc32 = __cpuCrc32(buff, 60);

	if(memcmp((unsigned char*)&xcrc32, buff+60, 4) != 0){
		MSG_LOG("CRC32 Error %08X", xcrc32);
		return 2;
	}

	memcpy(ADver, buff+8, 2);//�汾��
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
//���뷽��վ����(վ���Ŵ�1��ʼ)�����ͷ������β������
//����ķ������1�������ѭ��������indd��Ч�������ͷβ��һ���ġ�
void ADgetInSound(unsigned char drict, unsigned char indd, char *HeadStr, char *endStr)
{
//	unsigned int index;
	unsigned char ofbuf[8];
	unsigned char rbuff[128];
	int ret;

	HeadStr[0] = 0;	//����ַ�BUFF
	endStr[0] = 0;

	ret = getADSoundFileVer(rbuff, rbuff+10, rbuff+20);
	if(ret!=0)
		return;
	if((indd > 0) && (indd <= ADZhanDianMaxNum))
		indd--;	//�����վ���Ŷ��Ǵ�1��ʼ�����125 .  ��ȡ�ĵ�ַ��0��ʼ�ģ���������Ҫ��1
	else
		return;

	if(drict == 0){
		flashread(ADZhengxiangoffset+(indd*2), ofbuf, 2);	//������Ӧλ�õ�����ƫ����,0ͷ��1β
	}
	else if(drict == 1){//����
		flashread(ADFanxiangoffset+(indd*2), ofbuf, 2);	//������Ӧλ�õ�����ƫ����,0ͷ��1β
	}
	else{//ѭ������
		flashread(ADXunhuanoffset+(ADXunhuanIndex), ofbuf, 2);	//������Ӧλ�õ�����ƫ��������1�ֽ���Ч
		if(ofbuf[0] > ADInfoMaxNum){//ֵ��Ч���ص�0
			if(ADXunhuanIndex == 0){//�Ѿ���0�ˣ�˵��û��ѭ���������˳�
				return;
			}
			ADXunhuanIndex = 0;
			flashread(ADXunhuanoffset+(ADXunhuanIndex), ofbuf, 2);
		}
		ADXunhuanIndex++;
		if(ADXunhuanIndex>=60)
			ADXunhuanIndex = 0;
		
	}
	MSG_LOG("ȡ���� drict=%d, indd=%d, ofbuf=%02X%02X\r\n", drict, indd, ofbuf[0], ofbuf[1]);
	if(ofbuf[0]<ADInfoMaxNum){
		flashread(ADInfooffset+(ofbuf[0]*100), rbuff, 100);	//����ͷ
		rbuff[99] = 0;//Ϊ���ݴ������ӽ�����
		strcpy(HeadStr, (char*)rbuff);
		MSG_LOG("ȡ����ͷ:%s\r\n", HeadStr);
	}
	if(drict > 1){//ѭ������������β��ͷһ��������Ҫ��ȡ��
		strcpy(endStr, HeadStr);
	}
	else{
		if(ofbuf[1]<ADInfoMaxNum){
			flashread(ADInfooffset+(ofbuf[1]*100), rbuff, 100);	//����β
			rbuff[99] = 0;//Ϊ���ݴ������ӽ�����
			strcpy(endStr, (char*)rbuff);
			
			MSG_LOG("ȡ����β:%s", endStr);
		}
	}
}

void debugADsoundInfo(void)
{
	unsigned char buff[256];
	unsigned char sloff[260];
	unsigned int crc32, i;

	getADSoundFileVer(sloff, sloff+10, sloff+20);
	printf("��������汾:%02X%02X\r\n", sloff[0], sloff[1]);
	printf("����ʱ��:%02X%02X%02X%02X%02X%02X%02X\r\n", sloff[20], sloff[21], sloff[22], sloff[23], sloff[24], sloff[25], sloff[26]);
	printf("��·��:%02X%02X\r\n", sloff[10], sloff[11]);
	
	flashread(ADXunhuanoffset, sloff, 64);	//��������
	printf("ѭ������\r\n");
	debugdata(sloff, 64, 1);
	crc32 = __cpuCrc32(sloff, 60);
	if(memcmp((unsigned char*)&crc32, sloff+60, 4) == 0){
		for(i=0; i<60; i++){
			clr_wdt();
			if(sloff[i] < ADInfoMaxNum){
				printf("����:%d:", i+1);
				flashread(ADInfooffset+(sloff[i]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
		}
	}
	else{
		printf("У���:%08X\r\n", crc32);
	}
	
	flashread(ADZhengxiangoffset, sloff, 256);	//��������
	printf("����\r\n");
	debugdata(sloff, 256, 1);
	crc32 = __cpuCrc32(sloff, 252);
	if(memcmp((unsigned char*)&crc32, sloff+252, 4) == 0){
		for(i=0; i<ADZhanDianMaxNum; i++){
			clr_wdt();
			if(sloff[i*2] < ADInfoMaxNum){
				printf("վ��:%d ͷ:", i+1);
				flashread(ADInfooffset+(sloff[i*2]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
			if(sloff[i*2+1] < ADInfoMaxNum){
				printf("վ��:%d β:", i+1);
				flashread(ADInfooffset+(sloff[i*2+1]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
		}
	}
	else{
		printf("У���:%08X\r\n", crc32);
	}
	
	flashread(ADFanxiangoffset, sloff, 256);	//��������
	printf("����\r\n");
	debugdata(sloff, 256, 1);
	crc32 = __cpuCrc32(sloff, 252);
	if(memcmp((unsigned char*)&crc32, sloff+252, 4) == 0){
		for(i=0; i<ADZhanDianMaxNum; i++){
			clr_wdt();
			if(sloff[i*2] < ADInfoMaxNum){
				printf("վ��:%d ͷ:", i+1);
				flashread(ADInfooffset+(sloff[i*2]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
			if(sloff[i*2+1] < ADInfoMaxNum){
				printf("վ��:%d β:", i+1);
				flashread(ADInfooffset+(sloff[i*2+1]*100), buff, 100);
				printf((char*)buff);
				printf("\r\n");
			}
		}
	}
	else{
		printf("У���:%08X\r\n", crc32);
	}
	
}

