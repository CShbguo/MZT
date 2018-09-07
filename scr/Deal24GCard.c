
#include "stdio.h"
#include "szct.h"
#ifndef NO_MOBILE_24G_
#include "Deal24GCard.h"
#include "E_GJGPRS.h"

//#define _G24_FANG_BA_

#define _debug_G24Deal_

extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void BlightOpen(void);
extern unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern void SetTcpPacketTTLTime(unsigned char val);

extern unsigned char gMCardCand;
extern Parameter5 cardMessage;
extern unsigned char DriveCardNo[4];
extern pFistVary_1 pFistVary;
extern Parameter3 ComSumeFile;
extern unsigned char gchn_eng;//��Ӣ�ı�־
extern unsigned short gErrorFlag;
extern unsigned char gPsamErrorFlag;//PSAM�Ƿ����

unsigned int a_sumR24G;//�ֻ�Ǯ���е���ʵ���
unsigned int s_sum124G;
unsigned int a_sum124G;

unsigned char GetMobilStarBAK(unsigned char *stradd)
{
	unsigned char i, k;
	unsigned int temp;
	unsigned char crc;
	unsigned char crctemp;
	unsigned char buff[20];
	

	for(i=0; i<5; i++){
		sysferead(BIT_MobilREC_Start_BAK, 4, (unsigned char*)&temp);
		sysferead(BIT_MobilREC_Start_BAK_CRC, 1, (unsigned char*)&crc);

		memcpy(buff, (unsigned char*)&temp, 4);
		crctemp = 0xA6;
		for(k=0; k<4; k++)
			crctemp ^= buff[k];
		
#ifdef _debug_
		debugstring("GetMobilStarBAK\r\n");
		debugstring("buff:");
		debugdata(buff, 4, 1);
		debugstring("crctemp:");
		debugdata((unsigned char*)&crctemp, 1, 1);
		debugstring("crc:");
		debugdata((unsigned char*)&crc, 1, 1);
#endif
		if(crctemp == crc){
			memcpy(stradd, (unsigned char*)&temp, 4);
			return ST_OK;
		}
	}
	return ST_ERROR;
}

void writeMobilStarBAK(unsigned char *stradd)
{
	unsigned char k;
	unsigned char crctemp;
	unsigned char buff[10];
	unsigned char tt[10];
	
	memcpy(buff, stradd, 4);
	crctemp = 0xA6;
	for(k=0; k<4; k++)
		crctemp ^= buff[k];
	
	for(k=0;k<5;k++){
#ifdef _debug_
		debugstring("writeMobilStarBAK1\r\n");
#endif
		sysfewrite(BIT_MobilREC_Start_BAK, 4, buff);
		sysfewrite(BIT_MobilREC_Start_BAK_CRC, 1, (unsigned char*)&crctemp);
//		delayxms(1);
		
		sysferead(BIT_MobilREC_Start_BAK, 4, tt);
		if(memcmp(tt, buff, 4) == 0){
			return;
		}
	}
}

unsigned char G24PsamInitialize(void)
{
	unsigned char ii=0;
	//	unsigned char pos=0;
	unsigned char buffer[128];
	
	gPsamErrorFlag |= 2;
	
	g24GProFlag = 0;//����

	memset(G24PsamNo, 0, 6);
#ifdef _debug_
	debugstring("G24PSAM init\r\n");
#endif
	ii = SamColdReset(PSAM_YD, 9600, SAM_VOL5, buffer);
	
	if(ii == 0){
		return ST_ERROR;
	}
	
#ifdef _debug_
	debugstring("G24PSAM art:");
	debugdata(buffer, ii, 1);
#endif
	
	delayxms(5);
	if(Read_Binary(PSAM_YD, 21, 0, 14, buffer)==0) 
	{
#ifdef _debug_
		debugstring("G24Read_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("G24read 21 Binary:");
	debugdata(buffer,14,1);
#endif
	delayxms(100);
	
	if(Read_Binary(PSAM_YD, 22, 0, 6, buffer)==0) 
	{
#ifdef _debug_
		debugstring("G24Read_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
	
#ifdef _debug_
	debugstring("G24read 22 Binary:");
	debugdata(buffer,6,1);
#endif
	memcpy(G24PsamNo,buffer,6);
	
	delayxms(100);
	
#ifdef _debug__
	debugstring("G24Select File ");
#endif
// 	if(Select_File(PSAM_YD, 0x1001)==0) //0x3f01
// 	{
// 		debugstring("G24Select_File error\r\n");
//		return ST_ERROR;
// 	}
	memcpy(buffer, "1PAY.SYS.ADF01", 14);
	if(Select_FileF(PSAM_YD, buffer, 14)==0) //0x3f01
	{
#ifdef _debug_
		debugstring("G24Select_File1 error\r\n");
#endif
		return ST_ERROR;
	}
	delayxms(10);
	
	if(Read_Binary(PSAM_YD, 0x17, 0, 25, buffer)==0) 
	{
#ifdef _debug_
		debugstring("G24Read_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
	
#ifdef _debug_
	debugstring("G24read 17 Binary:");
	debugdata(buffer,25,1);
	debugstring("G24OK\r\n");
#endif
	
	memcpy((unsigned char*)&stuSamPubInf_LL, buffer, sizeof(stuSamPubInf_LL));

	gPsamErrorFlag &= 0xFD;

	g24PurError = 10;// ��ʼ���ܷ����ֻ���ͷ��
	gG24Ver = 0;
	gG24ReaderErr = 0;
	memset(gG24VerBuf, '0', 21);
	gG24VerBuf[20] = 0;
	return ST_OK;
}

unsigned int Crc(unsigned char *Buff, int Len)
{
	unsigned char i;
	unsigned short crc;
	unsigned int data;
	//crc-ccitt
	unsigned short poly=0x8408;	 //1000 0100 0000 1000  == x12+x5+1
	
	//	crc = nSeed;
	crc=0xffff;
	if (Len == 0)
		return 0;
	do
	{
		for (i=0, data=(unsigned int)0xff & *Buff++;
		i < 8;
		i++, data >>= 1)
		{
			if ((crc & 0x0001) ^ (data & 0x0001))
				crc = (crc >> 1) ^ poly;
			else  
				crc >>= 1;
		}
	} while (--Len);
	crc = ~crc;
	over_turn(2, (unsigned char*)&crc);
	return crc;
}

void g24ComParaClr(void)
{
	memset(g24ComRBuf, 0, 10);
	g24ComRindex = 0;
}

unsigned char g24ComSend(unsigned char *sndDat, unsigned char len)
{
	unsigned int dly = 0;
	
	unsigned char crc;
	unsigned char sendbuf[256];
	unsigned short index, i;

	index = 0;
	sendbuf[index++] = 0x02;//STX
	sendbuf[index++] = len/256;
	sendbuf[index++] = len%256;
	memcpy(sendbuf+index, sndDat, len);
	index += len;
	
	crc = 0;
	for(i=0; i<len; i++){
		crc ^= sndDat[i];
	}
	sendbuf[index++] = crc;
	sendbuf[index++] = 0x03;

	g24ComFlag = 0xaa;//�ڽ���g24��ͷ
	g24ComParaClr();
	com_snd(COM5, index, sendbuf);

	while(1){
		if(g24ComFlag == 0x55)
			return 1;
		dly++;
		if(dly > 900000){ //900000
// 			sendbuf[0] = 0x77;
// 			sendbuf[1] = g24ComFlag;
// 			com_snd(COM1, 2, sendbuf);
			return 0;
		}
	}
//	return 1;
}

unsigned char g24ComSend_only(unsigned char *sndDat, unsigned char len)
{
//	unsigned int dly = 0;
	
	unsigned char crc;
	unsigned char sendbuf[512];
	unsigned short index, i;
	
	index = 0;
	sendbuf[index++] = 0x02;//STX
	sendbuf[index++] = len/256;
	sendbuf[index++] = len%256;
	memcpy(sendbuf+index, sndDat, len);
	index += len;
	
	crc = 0;
	for(i=0; i<len; i++){
		crc ^= sndDat[i];
	}
	sendbuf[index++] = crc;
	sendbuf[index++] = 0x03;
	
	g24ComFlag = 0xaa;//�ڽ���g24��ͷ
	g24ComParaClr();
	com_snd(COM5, index, sendbuf);
	return 1;
}
void Rest24Card(void)
{
//	unsigned char ret;
	unsigned char sendbuf[20];
	
	g24PurError = 10;// ��ʼ���ܷ����ֻ���ͷ��//
	memcpy(sendbuf, "\xA1\x12", 2);
//	ret = 
	g24ComSend(sendbuf, 2);
	gG24Ver = 0;
}
 
//��24G������ѭ��
unsigned char G24Profile(void)
{
	unsigned char buffer[100];
//	unsigned int relen;
	unsigned short ss=0;
	
	if(g24GProFlag == 0){
		memcpy(buffer, "\xA2\x31\x01\xf4", 4);//ѯ��
		g24ComSend_only(buffer, 4);
		g24GProFlag = 1;
		return 0;
	}
	else{
		if(g24ComFlag == 0x55){
			g24GProFlag = 0;
#ifdef _debug_G24Deal_
			debugstring("ѯ������:");
			debugdata(g24ComRBuf, g24ComRindex,1);
#endif

			memcpy((unsigned char*)&ss, g24ComRBuf+3, 2);//���ص�״̬��
			if(ss == 0x0000){//�ɹ�
				gG24ReaderErr = 0;
				gErrorFlag &= (~ERROR_24READER);
				memset(gG24CardNo, 0, 10);
// 				debugstring("cardNOOO:");
// 				debugdata(g24ComRBuf, g24ComRindex,1);
// 				if(buffer[5] <= 8)
// 					memcpy(gG24CardNo, g24ComRBuf+6, buffer[5]);
// 				else
					memcpy(gG24CardNo, g24ComRBuf+6, 8);
				return 1;
			}
			else if (ss == 0x09a0)//�Լ�ʧ�ܣ�����ģ��
			{
				gG24ReaderErr++;
				if(gG24ReaderErr >= 50){
					gErrorFlag |= ERROR_24READER;
					Rest24Card();
					gG24ReaderErr = 0;
				}
			}
		}
		else{
			g24GProFlag++;
			if(g24GProFlag > 200){
//				miniDispstr(3,0,"P out!",0);
				gErrorFlag |= ERROR_24READER;
				g24GProFlag = 0;
			}
		}
	}
	return 0;
}


void g24Find_History(unsigned int Index,unsigned char *buff)
{
	unsigned int addr;
	addr=(Index+1) * MOBILE_REC_LEN;
	if(addr>gMobilREC_End)
	{
		addr-=gMobilREC_End;
		addr=LEN_M_REC-addr;
	}
	else
		addr=gMobilREC_End-addr;
	addr += FLASH_M_REC_START;
	flashread(addr, buff, 83);
#ifdef _debug_
	debugstring("addr:");
	debugdata((unsigned char*)&addr, 4, 0);
#endif
}
unsigned char g24Disp_history(unsigned int Index)
{
	unsigned char buffer[50];
	unsigned int temp;
	stMobileRecord historyCheck;
	
	g24Find_History(Index,(unsigned char *)&historyCheck);
#ifdef _debug_
	debugstring(" find:");
	debugdata((unsigned char*)&Index, 4, 1);
	debugdata((unsigned char*)&historyCheck, 83, 1);
	debugdata((unsigned char*)&historyCheck.dealType, 1, 1);
#endif
	temp=0;

	cls();
	if(gchn_eng == 'E')
		display(0,0,"No. :",0);
	else
		display(0,0,"����:",0);
	memset(buffer, 0, 50);
	BCD2Ascii(historyCheck.dealCardNo, buffer, 5);
//	display(0,0,(char*)buffer, 0);
	miniDispstr(0,7,(char*)buffer,0);
	memset(buffer, 0, 50);
	BCD2Ascii(historyCheck.dealCardNo+5, buffer, 5);
	miniDispstr(1,7,(char*)buffer,0);

	memset(buffer,0,20);
	temp=0;
	memcpy((unsigned char*)&temp,historyCheck.purseMoney,4);
	over_turn(4, (unsigned char*)&temp);
	if(gchn_eng == 'E')
		sprintf((char*)buffer, "deduct :%d.%02d", temp/100, temp%100);
	else
		sprintf((char*)buffer, "�ۿ�:%d.%02d", temp/100, temp%100);
	display(2,0,(char*)buffer,0);
	memset(buffer,0,20);
	temp=0;
	memcpy((unsigned char*)&temp, historyCheck.afterMoney, 4);
	over_turn(4, (unsigned char*)&temp);
	if(gchn_eng == 'E')
		sprintf((char*)buffer, "balance:%d.%02d", temp/100, temp%100);
	else
		sprintf((char*)buffer, "���:%d.%02d", temp/100, temp%100);
	display(4,0,(char*)buffer,0);
	dis_time_mini(7, (stcomtime*)&historyCheck.date);

	temp = ((gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC)/MOBILE_REC_LEN;
	sprintf((char*)buffer, "%d/%d ", Index+1, temp);
	miniDispstr(6, (21-strlen((char*)buffer)), (char*)buffer, 0);

	return ST_OK;
}
void show24gCard(void)
{
	unsigned int indexn;
	unsigned int index_his;
	unsigned int startadd;
	unsigned char ii;
	
	if(gchn_eng == 'E')
		display(0,2,"--query record--",0);
	else
		display(0,2,"--��¼��ѯ--",0);
	indexn=(gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
	indexn/=MOBILE_REC_LEN;
	index_his=0;
	if(indexn==0)
	{
		cls();
		if(gchn_eng == 'E')
			display(3,4, "no record",DIS_CENTER);
		else
			display(3,4, "��ˢ����¼",DIS_CENTER);
		delayxs(1);
		return;
	}
	startadd=0;

	g24Disp_history(0);

	display(4,14, "��",0);
	index_his=startadd;
	while(1)
	{
		
		if(profile(0) !=ST_ERROR)
			return;
		delayxms(50);
		ii=getkey(1);
		if(ii==ESC)
			return;
		else if(ii==DOWN)//2003.9.15
		{
			if(index_his>startadd)
				index_his--;
			g24Disp_history(index_his);
		}
		else if(ii==UP)
		{
			if(index_his<indexn-1)
				index_his++;
			g24Disp_history(index_his);
		}
		else if(ii==ENTER){
			index_his = startadd;
			g24Disp_history(index_his);
		}
		if(index_his==indexn-1)//���������������
			display(4,14,"��",0);
		else if(index_his==startadd)
			display(4,14,"��",0);
		else 
			display(4,14,"��",0);
	}
}
extern void clr_mobile_record(void);
unsigned char getmobileADD(void)
{
	//unsigned char i=0;
	unsigned int temp;//,addr;
// 	for(i=0; i<10; i++){
// 		sysferead(BIT_MobilREC_Start, 4, (unsigned char*)&temp);
// //		delayxms(1);
// 		sysferead(BIT_MobilREC_Start, 4, (unsigned char*)&addr);
// 		if(temp == addr)
// 			break;
// 		debugstring("#0 ");
// 	}
	sysferead(BIT_MobilREC_End, 4, (unsigned char*)&temp);
#ifdef _debug_
	debugstring("G24WriteRecord gMobilREC_Start:");
	debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
	debugstring("temp:");
	debugdata((unsigned char*)&temp, 4, 1);
#endif
	if((temp != gMobilREC_Start)||((gMobilREC_Start % MOBILE_REC_LEN) != 0)||(gMobilREC_Start > LEN_M_REC)){
//		delayxms(1);
		sysferead(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
		if((gMobilREC_Start % MOBILE_REC_LEN) != 0){//����ٴ��Ǵ�ģ�ȡ����
			GetMobilStarBAK((unsigned char*)&gMobilREC_Start);//
		}
		if((temp != gMobilREC_Start)||((gMobilREC_Start % MOBILE_REC_LEN) != 0)){
			sysferead(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
			if((gMobilREC_Start % MOBILE_REC_LEN) != 0){//����ٴ��Ǵ�ģ�ȡ����
				GetMobilStarBAK((unsigned char*)&gMobilREC_Start);//
			}
		}
	}

	sysferead(BIT_MobilREC_End, 4, (unsigned char*)&temp);
// 	for(i=0; i<10; i++){
// 		sysferead(BIT_MobilREC_End, 4, (unsigned char*)&temp);
// //		delayxms(1);
// 		sysferead(BIT_MobilREC_End, 4, (unsigned char*)&addr);
// 		if(temp == addr)
// 			break;
// 	debugstring("#4");
// 	}
#ifdef _debug_
	debugstring("G24WriteRecord gMobilREC_Start:");
	debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
	debugstring("temp:");
	debugdata((unsigned char*)&temp, 4, 1);
#endif
	if((temp != gMobilREC_End)||((gMobilREC_End % MOBILE_REC_LEN) != 0)){
//		delayxms(1);
		sysferead(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
		if((gMobilREC_End % MOBILE_REC_LEN) != 0){//����ٴ��Ǵ�ģ�ȡ����
			sysferead(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
		}
	}

	if((gMobilREC_Start % MOBILE_REC_LEN) != 0){//ͷָ�����
		gMobilREC_Start -= (gMobilREC_Start%MOBILE_REC_LEN);
		sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
		writeMobilStarBAK((unsigned char*)&gMobilREC_Start);
		temp = (gMobilREC_End+LEN_M_REC-mobilsendStyle.StartAddr)%LEN_M_REC;//����������
		temp /= 128;
		if(temp > 200){
			clr_mobile_record();
		}
		return ST_ERROR;
	}
	else if(gMobilREC_Start > LEN_M_REC){//ͷָ�����,������
		clr_mobile_record();
		return ST_ERROR;
	}
	if((gMobilREC_End % MOBILE_REC_LEN) != 0){//ָ�����
		gMobilREC_End += (MOBILE_REC_LEN-(gMobilREC_End%MOBILE_REC_LEN));
		sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
		sysfewrite(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
		temp = (gMobilREC_End+LEN_M_REC-mobilsendStyle.StartAddr)%LEN_M_REC;//����������
		temp /= 128;
		if(temp > 200){
			clr_mobile_record();
		}
		return ST_ERROR;
	}
	return ST_OK;
}

unsigned char G24WriteRecord(unsigned char *datp)
{
	unsigned char i=0;
	unsigned char buff[MOBILE_REC_LEN+10], datbuf[MOBILE_REC_LEN+10];
	unsigned int temp,addr;
	
	getmobileADD();

	temp =0;
	temp = gMobilREC_Start;
	memset(datbuf, 0xff, MOBILE_REC_LEN);
	while(1){
		if((temp % MASK64K)==0){
			if(temp == gMobilREC_End)//��ͷָ����������FFFFF��
				break;
		}
		flashread(FLASH_M_REC_START+temp, buff, MOBILE_REC_LEN);

// 		debugstring("ddd:");
// 		debugdata(buff, MOBILE_REC_LEN, 1);

		if(memcmp(buff, datbuf, MOBILE_REC_LEN) == 0)
			break;
		temp+=MOBILE_REC_LEN;
		if(temp >= LEN_M_REC)
			temp = 0;
	}
	gMobilREC_End = temp;

	memset(buff, 0x00, MOBILE_REC_LEN);
	memset(datbuf, 0, MOBILE_REC_LEN);
	memcpy(datbuf, datp, 83);//һ����¼83�ֽ�

	temp=gMobilREC_End;
	addr = FLASH_M_REC_START+temp;
#ifdef _debug_
	debugstring("-------------------\r\ngMobilREC_End:");
	debugdata((unsigned char*)&temp, 4, 1);
	flashread(addr, buff, MOBILE_REC_LEN);
	debugdata(buff, MOBILE_REC_LEN, 1);
#endif

	for(i=0; i<3; i++) 
	{
		if((addr%MASK64K) == 0){
			flasherase(ERASE64KBYTESECTOR, addr);
// 			debugstring("ERASE64KBYTESECTOR:");
// 			debugdata((unsigned char*)&addr, 4, 1);
		}
		flashwrite(addr, datbuf, MOBILE_REC_LEN);
#ifdef _debug_
		debugstring("WriteRecord : \r\n");		
		debugdata(datbuf,MOBILE_REC_LEN,1);
#endif
		flashread(addr, buff, MOBILE_REC_LEN);
#ifdef _debug_
		debugstring("WriteRecordCheck ulofset2: \r\n");
		debugdata(buff, MOBILE_REC_LEN, 1);
#endif
		if(memcmp(datbuf, buff, MOBILE_REC_LEN) == 0) 
		{
			temp += MOBILE_REC_LEN;
			if(temp >= LEN_M_REC)
				temp = 0;
			gMobilREC_End = temp;
			sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
			sysfewrite(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
			return ST_OK;
		}
		else{
			if(gMobilREC_End == gMobilREC_Start){//�����ȣ�д����ȥ����ʼֵ����
				gMobilREC_Start = 0;
				sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
				writeMobilStarBAK((unsigned char*)&gMobilREC_Start);
				gMobilREC_End = 0;
				sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
				sysfewrite(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
				temp=gMobilREC_End;
				addr = FLASH_M_REC_START+temp;
			}
			else{
				temp += MOBILE_REC_LEN;
				if(temp >= LEN_M_REC)
					temp = 0;
				gMobilREC_End = temp;
				sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
				sysfewrite(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
			}
		}
	}
	cls();
	if(gchn_eng == 'E')
		display(2, 2, "E2PROM error!", 1);
	else
		display(2, 2, "E2PROMд����,��Ҫά��!", 1);
	led_on(LED_RED|LED_GREEN);
	for(;;);
}

void G24BuildRecord(unsigned char *datbuf, unsigned char *psamSer, unsigned char *TACC)
{
	stMobileRecord recodt;
	unsigned char buff[100];

	memcpy(recodt.AID, MobileAID, 16);//ADI??
	recodt.dealType = 0x02;
	memcpy(recodt.dealCardNo, stuPubInf.cApp_sn, 10);
	sprintf((char*)buff, "%012d", s_sum124G);
	Ascii2BCD(buff, recodt.dealMoney, 12);
//	memcpy(recodt.dealMoney, buff, 6);
//	memcpy(recodt.dealDate, (unsigned char*)&SysTime, 7);
	memcpy(recodt.dealDate, stuDebitPurchase.cDateTime, 7);
	sprintf((char*)buff, "%06d", gMobilPOSNS);
	Ascii2BCD(buff, recodt.posSerial, 6);
	memcpy(recodt.Cardtag, gG24CardNo, 8);//----
	recodt.KEYflagNO = 3;//gMobileParameter.KEK_SN;
	recodt.KEYVer = stuSamInitPurchase.cKeyVer;
	memcpy(recodt.TAC, TACC, 4);//
	memcpy(recodt.purseMoney, (unsigned char*)&s_sum124G, 4);
	over_turn(4, recodt.purseMoney);
	recodt.pursestyle = 0x06;
	memcpy(recodt.deviceNo, G24PsamNo, 6);//
	memcpy(recodt.PSAMdealSerial, psamSer, 4);//
//	memcpy(recodt.date, (unsigned char*)&SysTime, 7);
	memcpy(recodt.date, stuDebitPurchase.cDateTime, 7);
	memcpy(recodt.afterMoney, (unsigned char *)&a_sum124G, 4);
	over_turn(4, recodt.afterMoney);

	memcpy(datbuf, (unsigned char*)&recodt, sizeof(stMobileRecord));
}

void G_G24BuildRec(unsigned char *datbuf, unsigned char *psamSer, unsigned char *TACC)
{
	MobileDEALRECODE recodt;
	unsigned char buff[100];
	
	memcpy(buff, MobileAID, 16);
	ASCII2ZBcod(pFistVary.DeviceNo, recodt.rDeviceNo, 4);
	memcpy(recodt.rLineNo, pFistVary.LineNo, 2);
//	memcpy(recodt.AID1, buff, 9);//ADI??
//	memcpy(recodt.AID2, buff+9, 3);//ADI??
	recodt.dealType = 0x04;//�ƶ��Ĺ�����¼
	memcpy(recodt.rDriverNo, DriveCardNo, 4);
	memcpy(recodt.dealCardNo, stuPubInf.cApp_sn, 10);

	sprintf((char*)buff, "%06d", gMobilPOSNS);
	Ascii2BCD(buff, recodt.posSerial, 6);
	recodt.KEYflagNO = 0xff;//3;//gMobileParameter.KEK_SN;--��������
	recodt.KEYVer = 0xff;//stuSamInitPurchase.cKeyVer;//--��������
//	memcpy(recodt.TAC, TACC, 4);//TAC--��������
	memset(recodt.TAC, 0, 4);
	memcpy(recodt.purseMoney, (unsigned char*)&s_sum124G, 4);
	over_turn(4, recodt.purseMoney);
	recodt.pursestyle = 0x06;
	memcpy(recodt.deviceNo, G24PsamNo, 6);//----
//	memcpy(recodt.PSAMdealSerial, psamSer, 4);//----
	Ascii2BCD(gMobileParameter.posNo, recodt.PSAMdealSerial, 8);//����POS�ն˺�
	memcpy(recodt.date, stuDebitPurchase.cDateTime, 7);
	if(a_sum124G > 200000)
		a_sum124G %= 100000;
	memcpy(recodt.afterMoney, (unsigned char *)&a_sum124G, 4);//--��������
	over_turn(4, recodt.afterMoney);
//	memset(recodt.afterMoney, 0, 4);
	
	memcpy(datbuf, (unsigned char*)&recodt, sizeof(stMobileRecord));
}
//�鿴��ͷ�汾��Ϣ
void check24GVer(void)
{
	unsigned char ret;
	unsigned char sendbuf[101];
	//���ڶ�ͷ 02 00 27 00 00 0500000000000000 0000000000000000 14 4E4D4652532D563531304D483336585335303435 15 03 
	//1.0�¶�ͷ02 00 27 00 00 0500000000000000 0000000000000000 14 4E4D4652532D5635313048483336425335313030 0A 03 
	//BS5100 XS5045
//	g24PurError = 10;// ��ʼ���ܷ����ֻ���ͷ��
	memcpy(sendbuf, "\xA1\x11", 2);
	ret = g24ComSend(sendbuf, 2);
	gG24Ver = 0xA4;
//#ifdef _debug_G
//	debugstring("check24GVer:");
//	debugdata((unsigned char*)&gG24Ver, 1, 1);	
//#endif
	if(ret == 1){
#ifdef _debug_
		debugstring("24G Ver:");
//		debugdata((unsigned char*)&g24ComRindex, 4, 1);
		debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
		if(g24ComRindex>23)
		{
			ret = g24ComRBuf[21];
			if(ret > 20)
				ret = 20;
			memset(gG24VerBuf,0,21);
			memcpy(gG24VerBuf, g24ComRBuf+22, ret);

			memset(sendbuf, 0, 100);
			memcpy(sendbuf, g24ComRBuf+23, g24ComRindex-23);
			if(strstr((char*)sendbuf, "BS5100")!=0){
				gG24Ver = 0xA2;
			}
			else if(strstr((char*)sendbuf, "XS5045")!=0){
				gG24Ver = 0xA1;
			}
			else{
				gG24Ver = 0xA3;
			}
		}

#ifdef _debug_
		debugstring("ͷ:");
		debugdata((unsigned char*)&gG24Ver, 1, 1);	
#endif
		g24ComFlag = 0;//�ڽ���g24��ͷ
	}
}
void Check24Card(void)
{
	unsigned short sst=0;
	unsigned int dly=0;
	unsigned char ret;
	unsigned char sendbuf[20];

	if(g24GProFlag == 1){//����һ���������
		while(1){
			if(g24ComFlag == 0x55)
				break;
			dly++;
			if(dly > 900000){
				break;
			}
		}
	}

	for(dly=0;dly<2;dly++){
		g24GProFlag = 0;
		memcpy(sendbuf, "\xE0\x09", 2);
		ret = g24ComSend(sendbuf, 2);
		if(ret == 1){
#ifdef _debug_
			debugstring("checkReader:");
			debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
			memcpy((unsigned char*)&sst, g24ComRBuf+3, 2);
			if((sst == 0x09a0)||(sst == 0x55e0)||(sst == 0x05e0)){//����������������������ģ��,��ʱ
				Rest24Card();
			}
		}
	}
}
//��24G������״̬
void G24ReadStyle(void)
{
	unsigned short sst=0;
	unsigned int times=0;
	unsigned char ret;
	unsigned char sendbuf[40];
	
	cls();
	if(gchn_eng == 'E'){
//		display(0,0,"check 2.4GReader",DIS_CENTER|DIS_CONVERT);
		display(6,0,"exit        test",0);
	}
	else{
//		display(0,0,"���Զ�ͷ״̬",DIS_CENTER|DIS_CONVERT);
		display(6,0,"�˳�        ����",0);
	}
	memset(sendbuf, 0, 20);
	BCD2Ascii((unsigned char*)&gG24Ver, sendbuf, 1);
	miniDispstr(1, 19, (char*)sendbuf, 0);
	miniDispstr(0, 1, (char*)gG24VerBuf, 0);

	restore_flag = 3;
	while(1){
		memcpy(sendbuf, "\xE0\x09", 2);
		ret = g24ComSend(sendbuf, 2);
		if(ret == 1){
			beep(1, 100, 100);
			if(gchn_eng == 'E'){
				sprintf((char*)sendbuf, "style:%02X%02X\x00", g24ComRBuf[3], g24ComRBuf[4]);
			}
			else
				sprintf((char*)sendbuf, "״̬:%02X%02X\x00", g24ComRBuf[3], g24ComRBuf[4]);
			display(2,0,(char*)sendbuf, DIS_CENTER);
			times++;
			memcpy((unsigned char*)&sst, g24ComRBuf+3, 2);
			switch (sst)
			{
			case 0:
				if(gchn_eng == 'E'){
					display(4,0,"  success  ", DIS_CENTER);
				}
				else
					display(4,0,"  ��������  ", DIS_CENTER);
				break;
			case 0x09a0:
				if(gchn_eng == 'E'){
					display(4,0,"selfchekc fail", DIS_CENTER);
				}
				else
					display(4,0,"�ϵ��Լ�ʧ��", DIS_CENTER);
				break;
			case 0x0aa0:
				if(gchn_eng == 'E'){
					display(4,0,"waite card", DIS_CENTER);
				}
				else
					display(4,0,"�ȴ���Ƭ����", DIS_CENTER);
				break;
			case 0x55e0:
				if(gchn_eng == 'E'){
					display(4,0,"environment unconventionality", DIS_CENTER);
				}
				else
					display(4,0,"  �����쳣  ", DIS_CENTER);
				break;
			case 0xaae0:
				if(gchn_eng == 'E'){
					display(4,0,"reader locked", DIS_CENTER);
				}
				else
					display(4,0," ���������� ", DIS_CENTER);
				break;
			case 0x05e0:
				if(gchn_eng == 'E'){
					display(4,0,"no activation", DIS_CENTER);
				}
				else
					display(4,0,"������δ����", DIS_CENTER);
				break;
			}
			sprintf((char*)sendbuf, "%u", times);
			display(6,0,(char*)sendbuf, DIS_CENTER);
		}
		while(1){
			if(profile(0) !=ST_ERROR)
				return;
			ret = getkey(0);
			if(ret == ESC)
				return;
			if(ret == KEY_ENT)
				break;
			delayxms(50);
		}		
	}
//	return;
}

//���貿��CPU������ָ֤��
unsigned char CpuCardCheck(unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0xca;
	Scmd.P1 = 0x00;
	Scmd.P2 = 0x00;
	Scmd.LC = len;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_CPU_
	debugstring("scmd:");
	debugdata(pData, len, 1);
#endif
	IccCommand(PSAM_YD, &Scmd, &Rcmd);
#ifdef _debug_CPU_
	debugstring("rcmd:");
	debugdata((unsigned char*)&Rcmd, 10, 1);
	debugdata((unsigned char*)&Rcmd.SW1, 2, 1);
#endif
	if(memcmp((unsigned char*)&Rcmd.SW1, "\x90\x00", 2)!=0)// *(unsigned int*)&Rcmd.SW1 != 0x9000 )
	{
		memcpy(pMac, &Rcmd.SW1, 2);
		return ST_ERROR;
	}
	memcpy(pMac, Rcmd.DATA, Rcmd.LE);
	return ST_OK;
}

unsigned char Picc_Read_Money(unsigned char *remb)
{
	unsigned char cmdbuff[50];
//	unsigned char revbuff[50];
	unsigned char ret;
#ifdef _debug_G24Deal_
	unsigned int temp;
#endif
//	memcpy(cmdbuff,"\xA2\x33\x00\xB0\x96\x00\x"
	memcpy(cmdbuff,"\xA2\x33\x80\x5c\x00\x02\x04",7);
	ret = g24ComSend(cmdbuff, 7);
#ifdef _debug_G24Deal_
	debugstring("---remb0:====");
	debugdata(g24ComRBuf,g24ComRindex, 1);
#endif
//---remb0:====02 00 08 00 00 00 00 19 B1 90 00 38 03 
// ---remb:====02 00 08 00 00 00 00 19 B1 90 00 38 03 
	if(ret == 0)
		return 1;
	if(g24ComRindex < 12)
		return 1;
	if(memcmp(g24ComRBuf+g24ComRindex-4, "\x90\x00", 2)!=0)
		return 1;//ȡ���ɹ�

#ifdef _debug_G24Deal_
	debugstring("---remb:====");
	debugdata(g24ComRBuf,g24ComRindex, 1);
	memcpy((unsigned char*)&temp, g24ComRBuf+5, 4);
	over_turn(4, (unsigned char*)&temp);
	sprintf((char *)cmdbuff, "���:%d.%02d", temp/100, temp%100);
	debugstring((char*)cmdbuff);
#endif
	memcpy(remb, g24ComRBuf+5, 4);

	return 0;
	//	cmdbuff[0] = 0x03;
	//	picc_b_exchange_block_2(cmdbuff, 1, revbuff+10, 4);
}

void G24End_Card(void)
{
	unsigned char ret;
//	unsigned int revl;
	unsigned int dly=0;
	unsigned char sendbuf[20];

	while(1){
		memcpy(sendbuf, "\xE0\x02", 2);
		ret = g24ComSend(sendbuf, 2);
		if(ret == 1){
//			debugdata(g24ComRBuf, g24ComRindex, 1);
			if(g24ComRBuf[5] != 1)
				return;
			delayxms(50);
			dly = 0;
		}
		dly++;
		if(dly > 10)
			break;
	}
}

void g24save_normal(void)
{
	memcpy(g24PCard_delay.CardSno,gG24CardNo,8);
	g24PCard_delay.Sec=SysTime.seconds;
}

unsigned char g24Delay_Normal(void)
{
	int i;
	unsigned char c,c1;
	
	if(memcmp(g24PCard_delay.CardSno,gG24CardNo,4))
	{
		memset(g24PCard_delay.CardSno,0,4);
		return 0;
	}
	
	c = (SysTime.seconds>>4)*10+(SysTime.seconds&0x0f);
	c1 = (g24PCard_delay.Sec>>4)*10+(g24PCard_delay.Sec&0x0f);
	
	i=((c+60)-c1)%60;
	if(i>1)
	{
		memset(g24PCard_delay.CardSno,0,4);
		return 0;
	}
	else 
		return 1;
}

unsigned char G24DealCard(void)
{
	unsigned char i, *pCh;
	unsigned char sndbuf[100];
	unsigned char revbuf[100];
	unsigned char Redom[20];
	unsigned char TACC[4];
//	unsigned char PSAMDsno[4];
	unsigned int revLen;
	unsigned int value;
	unsigned int pmoney;
	unsigned int itemp;
	unsigned short ii;

	//	debugstring("G24 Deal Card!!\r\n");
	memcpy(sndbuf, "\xA2\x33\x00\xa4\x04\x00\x10", 7);
	memcpy(sndbuf+7, MobileAID, 16);
#ifdef _debug_G24Deal_
	debugstring("G24 Select:");
	debugdata(sndbuf, 23, 1);
#endif
	revLen = g24ComSend(sndbuf, 23);
#ifdef _debug_G24Deal_
	debugstring("G24 Select ACK:");
	debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
	if(revLen == 0)//���س�ʱ,����
		return ST_ERROR;
	if((g24ComRBuf[3]!=0) || (g24ComRBuf[4] !=0))
		return ST_ERROR;
	i = g24ComRBuf[2];
	pCh = g24ComRBuf+3;
	while (i--)
	{
		if( (*pCh == 0x9F) && (*(pCh+1) == 0x0C) ) break;
		pCh++;
	}
	
	memcpy((unsigned char*)&stuPubInf, pCh+3, sizeof(typea_public_inf));//�������Զ����ݵ�FCI
#ifdef _debug_G24Deal_
	debugdata((unsigned char*)&stuPubInf,sizeof(typea_public_inf), 1);
	debugstring("ʡ����:");
	debugdata((unsigned char*)&stuPubInf.cAppVer, 1, 1);
#endif
	
	memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//��ǰ����
	over_turn(4, (unsigned char*)&itemp);
	memcpy((unsigned char*)&value, stuPubInf.cIssueDate, 4);//��������
	over_turn(4, (unsigned char*)&value);
	memcpy((unsigned char*)&pmoney, stuPubInf.cValidDate, 4);//��Ч����
	over_turn(4, (unsigned char*)&pmoney);
	
	if((itemp < value)||(itemp > pmoney)){
		return CARD_NO_TIME;//���ڻ�ĩ����
	}

	if(g24PurError == 0xaa){//��Ҫ��ˢ
//		debugstring("Raa1:");
//		debugdata(g24ErrorCardSno, 8, 1);
//		debugdata(gG24CardNo, 8, 1);
		if(memcmp(g24ErrorCardSno, gG24CardNo, 8) != 0){
#ifdef MOBILE_SHIYAN_
			return ST_ERROR;//����ˢ�������ֻ���ֻ��ˢͬһ�ֻ�
#else
			g24PurError = 0;
			memset(g24ErrorCardSno, 0, 8);//����ˢ�����ֻ���
			goto PurStart;
#endif
		}
		
		for(i=0;i<3;i++){
			if(Picc_Read_Money((unsigned char*)&value) == 0)
				break;
		}
		if(i >= 3){
			return ST_ERROR;
		}
		over_turn(4, (unsigned char*)&value);
#ifdef _debug_G24Deal_
		debugstring("aa���:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR24G, 4, 1);
#endif
		if(value < a_sumR24G){//�ϴ��Ѿ��۳ɹ�
			memcpy(sndbuf, "\xA2\x33\x80\x5A\x00\x06\x02", 7);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
 			over_turn(2, (unsigned char*)&ii);
 			ii += 1;
 			over_turn(2, (unsigned char*)&ii);
			memcpy(sndbuf+7, (unsigned char*)&ii, 2);
			sndbuf[9] = 8;
#ifdef _debug_G24Deal_
			debugstring("����:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(sndbuf, 10, 1);
#endif
			revLen = g24ComSend(sndbuf, 10);//
#ifdef _debug_G24Deal_
			debugdata(g24ComRBuf, g24ComRindex, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(revLen == 0)//���س�ʱ,����
				return ST_ERROR;
			if((g24ComRBuf[3]!=0) || (g24ComRBuf[4] !=0))
				return ST_ERROR;
			if(memcmp(g24ComRBuf+g24ComRindex-4, "\x90\x00", 2)!=0)
				return ST_ERROR;//ȡ���ɹ�

			memcpy(TACC, g24ComRBuf+g24ComRindex-8, 4);//���ص�TAC��
			memcpy(sndbuf, g24ComRBuf+g24ComRindex-12, 4);//��MAC2��
//			return ST_ERROR;
			goto CPUMAC2deal;
		}
	}
#ifndef MOBILE_SHIYAN_
PurStart:
#endif
	a_sumR24G = 0;
	for(i=0;i<3;i++){
		if(Picc_Read_Money((unsigned char*)&a_sumR24G) == 0)
			break;
	}
	if(i >= 3){//������������˳�
		return ST_ERROR;
	}
	over_turn(4, (unsigned char*)&a_sumR24G);

#ifdef _debug_G24Deal_
	debugstring("a_sumR24G:");
	debugdata((unsigned char*)&a_sumR24G, 4, 1);
#endif
//---------------��֤����ͷ------------------------------
#ifdef _G24_FANG_BA_
	if(Get_Challenge(PSAM_YD, Redom) == 0){//PSAM��ȡ�����:7 Get Chellenge
		return 10;//PSAM��ȡ�������
	}
#ifdef _debug_G24Deal_
	debugstring("Get Challenge ACK:");
	debugdata(Redom, 4, 1);
#endif

	memcpy(sndbuf, "\xA1\x13", 2);
	sndbuf[2] = 0x00;//gMobileParameter.KEK_SN; 1 3 4 2 
	memcpy(sndbuf+3, Redom, 4);
#ifdef _debug_G24Deal_
	debugstring("A1 13:");
	debugdata(sndbuf, 7, 1);
#endif
	revLen = g24ComSend(sndbuf, 7);//9 ��д����֤,����RID��MAC
#ifdef _debug_G24Deal_
	debugstring("G24 mm ACK:");
	debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
	if((g24ComRBuf[3]!=0)||(g24ComRBuf[4]!=0))
		return 11;
	if(revLen <12)//����ͷ��MAC��.���س�ʱ,����
		return 11;
	memcpy(Redom, g24ComRBuf+5, 12);//RID+MAC
#ifdef _debug_G24Deal_
	debugstring("G24 mm ACK2:");
	debugdata(Redom, 12, 1);
#endif
#endif
//---------------������֤����ͷ--------------------------

	memcpy(sndbuf, "\xa2\x33\x80\x50\x01\x02\x0B", 7); i = 7;//11 INITIALIZE FOR PURCHASE����
	sndbuf[i++] = 0x03;//stuSamPubInf_LL.kid;

	cardMessage.card_catalog = CARD_MOBILE_BUS;

	s_sum124G = get_s_sum1(0);//ȡ�����ѽ��

	if((s_sum124G == 0) || (s_sum124G > 10000))
		return 22;

	value = s_sum124G;
	over_turn(4, (unsigned char*)&value);
	memcpy(sndbuf+i, (unsigned char*)&value, 4); i+=4;
	memcpy(sndbuf+i, G24PsamNo, 6); i += 6;
	sndbuf[i++] = 0x0f;
#ifdef _debug_G24Deal_
	debugstring("���ѳ�ʼ��:");
	debugdata(sndbuf, i, 1);
#endif
	revLen = g24ComSend(sndbuf, i);//11 ����INITIALIZE FOR PURCHASE����
#ifdef _debug_G24Deal_
	debugstring("���ѳ�ʼ��ACK:");
	debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
	if(revLen == 0)
		return 4;
	else{
		if(memcmp(g24ComRBuf+5, "\x94\x01", 2)==0)
			return 3;//����
		else if(memcmp(g24ComRBuf+g24ComRindex-4, "\x90\x00", 2)!=0)//spi  -5
			return 5;
	}//02 00 13 00 00 00 00 86 C7 02 5E 00 00 00 01 82 65 30 27 F6 90 00 8A 03 
	
#ifdef _debug_G24Deal_
	debugstring("MAC1����ָ��:");
#endif
	memcpy((unsigned char*)&stuInitPurchaseRet, g24ComRBuf+5, sizeof(stuInitPurchaseRet));
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum124G, stuInitPurchaseRet.cBalance, 4);//���
	over_turn(4, (unsigned char*)&a_sum124G);
	
#ifdef _debug_G24Deal_
	debugstring("MAC1 a_sum124G:");
	debugdata((unsigned char*)&a_sum124G, 4, 1);
#endif
	if((a_sumR24G < a_sum124G)&&(a_sumR24G > 0))
		a_sum124G = a_sumR24G;

	stuSamInitPurchase.cType = 0x06;
	
	memcpy(stuSamInitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	stuSamInitPurchase.cKeyVer = stuInitPurchaseRet.cKeyVer;
	stuSamInitPurchase.cKeyEn = stuInitPurchaseRet.cKeyEn;
	stuSamInitPurchase.cKeyFactor[0] = stuInitPurchaseRet.cKeyVer;
	memcpy(stuSamInitPurchase.cKeyFactor+1, stuPubInf.cApp_sn+3, 7);
//	memcpy(stuSamInitPurchase.cPublisherSign, G24PsamNo, 6);//���...
	stuSamInitPurchase.cPublisherSign[0] = stuPubInf.cAppVer;//18;ʡ����
	stuSamInitPurchase.cPublisherSign[1] = 0x80;
	memset(stuSamInitPurchase.cPublisherSign+2, 0, 6);
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInf.cPublisherSign, 8);//���...

	memcpy(stuSamInitPurchase.cReaderMAC, Redom, 12);//�����ͷRID �� MAC
#ifdef _debug_G24Deal_
	debugdata((unsigned char*)&stuSamInitPurchase,(0x30), 1);//+12
#endif
	memset(revbuf, 0xff, 50);//13 MAC1���� INIT_SAM_FOR_PURCHASE+MAC

#ifdef _G24_FANG_BA_
	if(CpuCardSetMac(PSAM_YD, revbuf, (unsigned char*)&stuSamInitPurchase, (0x30)) == ST_ERROR)//0x30 0x24
	{
		return 6;
	}
#else
	if(CpuCardSetMac(PSAM_YD, revbuf, (unsigned char*)&stuSamInitPurchase, (0x24)) == ST_ERROR)//0x30 0x24 ����Ҫ��֤
	{
		return 6;
	}
#endif
#ifdef _debug_G24Deal_
	debugstring("MAC OK!:");
	debugdata(revbuf, 8, 1);
#endif
	memcpy(stuDebitPurchase.cTermSnq, revbuf, 4);
	memcpy(stuDebitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	memcpy(stuDebitPurchase.cMac, revbuf+4, 4);
	memcpy(g24PSAMDsno, revbuf, 4);//--4�ֽڵ��ն��ѻ��������  �� MAC1
	
	memcpy(sndbuf, "\xA2\x33\x80\x54\x01\x00\x0F", 7); i = 7;
	memcpy(sndbuf+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	
	memset(revbuf, 0xff, 50);
#ifdef _debug_G24Deal_
	debugstring("chek Mac1:");
	debugdata(sndbuf, i, 1);
#endif
	revLen = g24ComSend(sndbuf, i);//16 DIBITIFOR PURCHASE
#ifdef _debug_G24Deal_
	debugstring("chek Mac1 OK:");
	debugdata((unsigned char*)&g24ComRindex, 4, 1);
	debugdata(g24ComRBuf, g24ComRindex ,1);
#endif

	if(g24ComRindex < 5){
//		debugdata(revbuf, 20, 1);
		g24PurError = 0xaa;//������Ҫ��ˢ��
		memcpy(g24ErrorCardSno, gG24CardNo, 8);
		return 7;
	}//02 00 0C 00 00 AF DE A1 0A 0C E8 14 4B 90 00 F1 03 
	if(memcmp(g24ComRBuf+g24ComRindex-4, "\x93\x02", 2)==0){//MAC��Ч// SPI -5
//		debugdata(revbuf, 20, 1);
//		g24PurError = 0xaa;//������Ҫ��ˢ��
//		memcpy(g24ErrorCardSno, gG24CardNo, 8);
		return 8;
	}//MAC��Ч
	if(memcmp(g24ComRBuf+g24ComRindex-4, "\x90\x00", 2)!=0){
//		debugdata(revbuf, 20, 1);
		g24PurError = 0xaa;//������Ҫ��ˢ��
		memcpy(g24ErrorCardSno, gG24CardNo, 8);
		return 7;
	}//��������

	memcpy(TACC, g24ComRBuf+g24ComRindex-12, 4);//���ص�TAC�� SPI -13
	memcpy(sndbuf, g24ComRBuf+g24ComRindex-8, 4);//��MAC2��  SPI -9
	
CPUMAC2deal:
	memcpy(cardMessage.PbocTac, TACC, 4);	// TAC��
#ifdef _debug_G24Deal_
	debugstring("TAC:");
	debugdata(TACC, 4, 1);
	debugstring("MAC2:");
	debugdata(sndbuf, 4, 1);
	debugdata((unsigned char*)&a_sum124G, 4, 1);
#endif
//	return 0;
	if(CPUMAC2(PSAM_YD, revbuf, sndbuf, 4) != ST_OK){//19	CREDIT_PSAM_FOR_PUCHASE У��MAC2
		debugstring("����MAC2 ERROR\r\n");
	}

#ifdef _debug_G24Deal_
	debugstring("����MAC2:");
	debugdata(revbuf, 10, 1);
#endif
	a_sum124G -= s_sum124G;

// 	cls();
// 	display(0,0,"���ѽ��:",0);
// 	sprintf((char*)revbuf, "���:%d.%02dԪ", (a_sum124G/100), (a_sum124G%100));
// 	display(2,0,(char*)revbuf,0);
// 	sprintf((char*)revbuf, "�ۿ�:%d.%02dԪ", (s_sum124G/100), (s_sum124G%100));
// 	display(4,0,(char*)revbuf,0);
	
	memset(g24ErrorCardSno, 0, 8);
	g24PurError = 0;
#ifdef AUDIO_SOUND_OLD
	audio_old(Audio_DONG);
#else
	audio(Audio_DONG);
#endif
	money_msg(ID_REC_TOLL, a_sum124G, s_sum124G, 1);
//	money_msg(ID_REC_TOLL, a_sumR24G, s_sum124G, 1);

#ifdef _debug_G24Deal_
	debugstring("-------\r\n");
	debugstring("���ѳɹ�:\r\n");
 	sprintf((char*)revbuf, "���:%d.%02dԪ\r\n", (a_sum124G/100), (a_sum124G%100));
	debugstring((char*)revbuf);
 	sprintf((char*)revbuf, "�ۿ�:%d.%02dԪ\r\n", (s_sum124G/100), (s_sum124G%100));
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif

	g24save_normal();

 	G24BuildRecord(revbuf, g24PSAMDsno, TACC);//���ѳɹ�����֯��¼
	
//	debugstring("rec-dat:");
//	debugdata(revbuf, 64, 1);

	G24WriteRecord(revbuf);//д���¼

	G_G24BuildRec(revbuf, g24PSAMDsno, TACC);
	WriteRecord(revbuf, 0);
	
	gMobilPOSNS++;
	if(gMobilPOSNS > 999999)
		gMobilPOSNS = 1;
	sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);

	return 0;//���ѳɹ�
}

unsigned char mode2or3;
void deal2or3(unsigned char*sndd)
{
	if(mode2or3 == 2){
		sndd[0] = 2;
		mode2or3 = 3;
	}
	else{
		sndd[0] = 3;
		mode2or3 = 2;
	}
}


//�ƶ���SIM��PASS����
unsigned char PassCard_Read_Money(unsigned char *remb)
{
	unsigned char cmdbuff[50];
	unsigned char revbuff[50];
	unsigned char ret;
#ifdef _debug_
	unsigned int temp;
#endif
	//	memcpy(cmdbuff,"\xA2\x33\x00\xB0\x96\x00\x"
	memcpy(cmdbuff,"\x02\x80\x5c\x00\x02\x04",6);
	deal2or3(cmdbuff);
	ret = RfPassthrough(revbuff, 2, cmdbuff, 6);
	if(ret == 0){
		return 1;
	}
#ifdef _debug_
	debugstring("---remb0:====");
	debugdata(revbuff,ret, 1);
#endif
	//---remb0:====02 00 08 00 00 00 00 19 B1 90 00 38 03 
	// ---remb:====02 00 08 00 00 00 00 19 B1 90 00 38 03 
	if(ret == 0)
		return 1;
	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
		return 1;//ȡ���ɹ�
	
#ifdef _debug_
	debugstring("---remb:====");
	debugdata(revbuff,ret, 1);
	memcpy((unsigned char*)&temp, revbuff+1, 4);
	over_turn(4, (unsigned char*)&temp);
	sprintf((char *)cmdbuff, "���:%d.%02d", temp/100, temp%100);
	debugstring((char*)cmdbuff);
#endif
	memcpy(remb, revbuff+1, 4);
	
	return 0;
}

//�ƶ���SIM��PASS����
unsigned char G24DealPassCard(void)
{
	unsigned char i, *pCh;
	unsigned char sndbuf[100];
	unsigned char revbuf[256];
	unsigned char Redom[20];
	unsigned char TACC[4];
//	unsigned char PSAMDsno[4];
	unsigned int revLen;
	unsigned int value;
	unsigned int pmoney;
	unsigned int itemp;
	unsigned short ii;
	
	memcpy(sndbuf,"\xe0\x50",2);
	revLen = RfPassthrough(revbuf, 2, sndbuf, 2);
	mode2or3 = 2;
	delayxms(2);

#ifdef _debug_
	debugstring("CPU pass start!\r\n");
	debugdata(revbuf, revLen, 1);
#endif
	//	debugstring("G24 Deal Card!!\r\n");
	memcpy(sndbuf, "\x02\x00\xa4\x04\x00\x10", 6);
	memcpy(sndbuf+6, MobileAID, 16);
	deal2or3(sndbuf);
#ifdef _debug_
	debugstring("G24 Select:");
	debugdata(sndbuf, 22, 1);
#endif
	revLen = RfPassthrough(revbuf, 2, sndbuf, 22);
	if(revLen == 0){
		return 1;
	}

#ifdef _debug_
	debugstring("G24 Select ACK:");
	debugdata(revbuf, revLen, 1);
#endif
	if(revLen == 0)//���س�ʱ,����
		return ST_ERROR;
	i = revLen;//g24ComRBuf[2];
	pCh = revbuf+1;
	while (i--)
	{
		if( (*pCh == 0x9F) && (*(pCh+1) == 0x0C) ) break;
		pCh++;
	}
	
	memcpy((unsigned char*)&stuPubInf, pCh+3, sizeof(typea_public_inf));//�������Զ����ݵ�FCI
#ifdef _debug_
	debugdata((unsigned char*)&stuPubInf,sizeof(typea_public_inf), 1);
	debugstring("ʡ����:");
	debugdata((unsigned char*)&stuPubInf.cAppVer, 1, 1);
#endif
	
	memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//��ǰ����
	over_turn(4, (unsigned char*)&itemp);
	memcpy((unsigned char*)&value, stuPubInf.cIssueDate, 4);//��������
	over_turn(4, (unsigned char*)&value);
	memcpy((unsigned char*)&pmoney, stuPubInf.cValidDate, 4);//��Ч����
	over_turn(4, (unsigned char*)&pmoney);
#ifdef _debug_
	debugstring("��Ч��:");
	debugdata((unsigned char*)&pmoney, 4, 1);
#endif
// 	if((itemp < value)||(itemp > pmoney)){
// 		return CARD_NO_TIME;//���ڻ�ĩ����
// 	}

	if(g24PurError == 0xaa){//��Ҫ��ˢ
//		debugstring("Raa1:");
//		debugdata(g24ErrorCardSno, 8, 1);
//		debugdata(gG24CardNo, 8, 1);
		if(memcmp(g24ErrorCardSno, gG24CardNo, 8) != 0){
#ifdef MOBILE_SHIYAN_
			return ST_ERROR;//����ˢ�������ֻ���ֻ��ˢͬһ�ֻ�
#else
			g24PurError = 0;
			memset(g24ErrorCardSno, 0, 8);//����ˢ�����ֻ���
			goto PurStart;
#endif
		}
		
		for(i=0;i<3;i++){
			if(PassCard_Read_Money((unsigned char*)&value) == 0)
				break;
		}
		if(i >= 3){
			return ST_ERROR;
		}
		over_turn(4, (unsigned char*)&value);
#ifdef _debug_
		debugstring("aa���:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR24G, 4, 1);
#endif
		if(value < a_sumR24G){//�ϴ��Ѿ��۳ɹ�
			memcpy(sndbuf, "\x02\x80\x5A\x00\x06\x02", 6);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
 			over_turn(2, (unsigned char*)&ii);
 			ii += 1;
 			over_turn(2, (unsigned char*)&ii);
			memcpy(sndbuf+6, (unsigned char*)&ii, 2);
			sndbuf[8] = 8;
#ifdef _debug_
			debugstring("����:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(sndbuf, 9, 1);
#endif
			deal2or3(sndbuf);
			revLen = RfPassthrough(revbuf, 2, sndbuf, 10);
			if(revLen == 0){
				return 1;
			}
#ifdef _debug_
			debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(revLen == 0)//���س�ʱ,����
				return ST_ERROR;
			if(memcmp(revbuf+revLen-2, "\x90\x00", 2)!=0)
				return ST_ERROR;//ȡ���ɹ�

			memcpy(TACC, revbuf+revLen-6, 4);//���ص�TAC��
			memcpy(sndbuf, revbuf+revLen-10, 4);//��MAC2��
//			return ST_ERROR;
			goto CPUMAC2deal;
		}
	}
#ifndef MOBILE_SHIYAN_
PurStart:
#endif
	a_sumR24G = 0;
	for(i=0;i<3;i++){
		if(PassCard_Read_Money((unsigned char*)&a_sumR24G) == 0)
			break;
	}
	if(i >= 3){//������������˳�
		return ST_ERROR;
	}
	over_turn(4, (unsigned char*)&a_sumR24G);

#ifdef _debug_
	debugstring("a_sumR24G:");
	debugdata((unsigned char*)&a_sumR24G, 4, 1);
#endif

	memcpy(sndbuf, "\x02\x80\x50\x01\x02\x0B", 6); i = 6;//11 INITIALIZE FOR PURCHASE����
	sndbuf[i++] = 0x03;//stuSamPubInf_LL.kid;

	cardMessage.card_catalog = CARD_MOBILE_BUS;

	s_sum124G = get_s_sum1(0);//ȡ�����ѽ��

	if((s_sum124G == 0) || (s_sum124G > 10000))
		return 22;

	value = s_sum124G;
	over_turn(4, (unsigned char*)&value);
	memcpy(sndbuf+i, (unsigned char*)&value, 4); i+=4;
	memcpy(sndbuf+i, G24PsamNo, 6); i += 6;
	sndbuf[i++] = 0x0f;
	deal2or3(sndbuf);
#ifdef _debug_
	debugstring("���ѳ�ʼ��:");
	debugdata(sndbuf, i, 1);
#endif
	revLen = RfPassthrough(revbuf, 2, sndbuf, i);
	if(revLen == 0){
		return 1;
	}//11 ����INITIALIZE FOR PURCHASE����
#ifdef _debug_
	debugstring("���ѳ�ʼ��ACK:");
	debugdata(revbuf, revLen, 1);
#endif
	if(revLen == 0)
		return 4;
	else{
		if(memcmp(revbuf+1, "\x94\x01", 2)==0)
			return 3;//����
		else if(memcmp(revbuf+revLen-2, "\x90\x00", 2)!=0)//spi  -5
			return 5;
	}//02 00 13 00 00 00 00 86 C7 02 5E 00 00 00 01 82 65 30 27 F6 90 00 8A 03 
	
#ifdef _debug_
	debugstring("MAC1����ָ��:");
#endif
	memcpy((unsigned char*)&stuInitPurchaseRet, revbuf+1, sizeof(stuInitPurchaseRet));
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum124G, stuInitPurchaseRet.cBalance, 4);//���
	over_turn(4, (unsigned char*)&a_sum124G);
	
#ifdef _debug_
	debugstring("MAC1 a_sum124G:");
	debugdata((unsigned char*)&a_sum124G, 4, 1);
#endif
	if((a_sumR24G < a_sum124G)&&(a_sumR24G > 0))
		a_sum124G = a_sumR24G;

	stuSamInitPurchase.cType = 0x06;
	
	memcpy(stuSamInitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	stuSamInitPurchase.cKeyVer = stuInitPurchaseRet.cKeyVer;
	stuSamInitPurchase.cKeyEn = stuInitPurchaseRet.cKeyEn;
	stuSamInitPurchase.cKeyFactor[0] = stuInitPurchaseRet.cKeyVer;
	memcpy(stuSamInitPurchase.cKeyFactor+1, stuPubInf.cApp_sn+3, 7);
//	memcpy(stuSamInitPurchase.cPublisherSign, G24PsamNo, 6);//���...
	stuSamInitPurchase.cPublisherSign[0] = stuPubInf.cAppVer;//18;ʡ����
	stuSamInitPurchase.cPublisherSign[1] = 0x80;
	memset(stuSamInitPurchase.cPublisherSign+2, 0, 6);
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInf.cPublisherSign, 8);//���...

	memcpy(stuSamInitPurchase.cReaderMAC, Redom, 12);//�����ͷRID �� MAC
#ifdef _debug_
	debugdata((unsigned char*)&stuSamInitPurchase,(0x30), 1);//+12
#endif
	memset(revbuf, 0xff, 50);//13 MAC1���� INIT_SAM_FOR_PURCHASE+MAC

#ifdef _G24_FANG_BA_
	if(CpuCardSetMac(PSAM_YD, revbuf, (unsigned char*)&stuSamInitPurchase, (0x30)) == ST_ERROR)//0x30 0x24
	{
		return 6;
	}
#else
	if(CpuCardSetMac(PSAM_YD, revbuf, (unsigned char*)&stuSamInitPurchase, (0x24)) == ST_ERROR)//0x30 0x24 ����Ҫ��֤
	{
		return 6;
	}
#endif
#ifdef _debug_
	debugstring("MAC OK!:");
	debugdata(revbuf, 8, 1);
#endif
	memcpy(stuDebitPurchase.cTermSnq, revbuf, 4);
	memcpy(stuDebitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	memcpy(stuDebitPurchase.cMac, revbuf+4, 4);
	memcpy(g24PSAMDsno, revbuf, 4);//--4�ֽڵ��ն��ѻ��������  �� MAC1
	
	memcpy(sndbuf, "\x02\x80\x54\x01\x00\x0F", 6); i = 6;
	memcpy(sndbuf+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	
	memset(revbuf, 0xff, 50);
	deal2or3(sndbuf);
#ifdef _debug_
	debugstring("chek Mac1:");
	debugdata(sndbuf, i, 1);
#endif
	revLen = RfPassthrough(revbuf, 2, sndbuf, i);
	if(revLen == 0){
		return 1;
	}//16 DIBITIFOR PURCHASE
#ifdef _debug_
	debugstring("chek Mac1 OK:");
	debugdata((unsigned char*)&revLen, 4, 1);
	debugdata(revbuf, revLen ,1);
#endif

	if(revLen < 1){
//		debugdata(revbuf, 20, 1);
		g24PurError = 0xaa;//������Ҫ��ˢ��
		memcpy(g24ErrorCardSno, gG24CardNo, 8);
		return 7;
	}//02 00 0C 00 00 AF DE A1 0A 0C E8 14 4B 90 00 F1 03 
	if(memcmp(revbuf+revLen-2, "\x93\x02", 2)==0){//MAC��Ч// SPI -5
//		debugdata(revbuf, 20, 1);
//		g24PurError = 0xaa;//������Ҫ��ˢ��
//		memcpy(g24ErrorCardSno, gG24CardNo, 8);
		return 8;
	}//MAC��Ч
	if(memcmp(revbuf+revLen-2, "\x90\x00", 2)!=0){
//		debugdata(revbuf, 20, 1);
		g24PurError = 0xaa;//������Ҫ��ˢ��
		memcpy(g24ErrorCardSno, gG24CardNo, 8);
		return 7;
	}//��������

	memcpy(TACC, revbuf+revLen-10, 4);//���ص�TAC�� SPI -13
	memcpy(sndbuf, revbuf+revLen-6, 4);//��MAC2��  SPI -9
	
CPUMAC2deal:
	memcpy(cardMessage.PbocTac, TACC, 4);	// TAC��
#ifdef _debug_
	debugstring("TAC:");
	debugdata(TACC, 4, 1);
	debugstring("MAC2:");
	debugdata(sndbuf, 4, 1);
	debugdata((unsigned char*)&a_sum124G, 4, 1);
#endif
	
//	return 0;
	if(CPUMAC2(PSAM_YD, revbuf, sndbuf, 4) != ST_OK){//19	CREDIT_PSAM_FOR_PUCHASE У��MAC2
		debugstring("����MAC2 ERROR\r\n");
	}

#ifdef _debug_
	debugstring("����MAC2:");
	debugdata(revbuf, 10, 1);
#endif
	a_sum124G -= s_sum124G;

// 	cls();
// 	display(0,0,"���ѽ��:",0);
// 	sprintf((char*)revbuf, "���:%d.%02dԪ", (a_sum124G/100), (a_sum124G%100));
// 	display(2,0,(char*)revbuf,0);
// 	sprintf((char*)revbuf, "�ۿ�:%d.%02dԪ", (s_sum124G/100), (s_sum124G%100));
// 	display(4,0,(char*)revbuf,0);
	
	memset(g24ErrorCardSno, 0, 8);
	g24PurError = 0;
#ifdef AUDIO_SOUND_OLD
	audio_old(Audio_DONG);
#else
	audio(Audio_DONG);
#endif
	money_msg(ID_REC_TOLL, a_sum124G, s_sum124G, 1);
//	money_msg(ID_REC_TOLL, a_sumR24G, s_sum124G, 1);

#ifdef _debug_
	debugstring("-------\r\n");
	debugstring("���ѳɹ�:\r\n");
 	sprintf((char*)revbuf, "���:%d.%02dԪ\r\n", (a_sum124G/100), (a_sum124G%100));
	debugstring((char*)revbuf);
 	sprintf((char*)revbuf, "�ۿ�:%d.%02dԪ\r\n", (s_sum124G/100), (s_sum124G%100));
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif

	g24save_normal();

 	G24BuildRecord(revbuf, g24PSAMDsno, TACC);//���ѳɹ�����֯��¼
	
//	debugstring("rec-dat:");
//	debugdata(revbuf, 64, 1);

	G24WriteRecord(revbuf);//д���¼

	G_G24BuildRec(revbuf, g24PSAMDsno, TACC);
	WriteRecord(revbuf, 0);
	
	gMobilPOSNS++;
	if(gMobilPOSNS > 999999)
		gMobilPOSNS = 1;
	sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);

	return 0;//���ѳɹ�
}

extern void SoundMessage(unsigned char cmd);
extern void DisRetry(void);
extern void DisRetry2(void);

void G24MainPASSCard(void)
{
	unsigned int uitmp;
	unsigned char ret;
	char disbuf[20];

	if(F_FLAG_VARY.stop_flag) 
		return;

#ifdef MOBILE_SHIYAN_
	if(g24PurError == 0xaa)
		DisRetry();
#else
	if(g24PurError == 0xaa)
		DisRetry2();
#endif
	
	gMCardCand = CARDSTYLE_24Mobile;
	BlightOpen();
	
	if(gPsamErrorFlag&2){
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"error:",1);
			display(3,0,"PSAM2 error!",0);
		}
		else{
			display(0,0,"����:",1);
			display(3,0,"�ƶ�PSAM����!",0);
		}
		restore_flag = 3;
		delayxs(1);
		return;
	}
	if(g24Delay_Normal())
		return;
	
	//�¼����������ж�
	getmobileADD();
	
	uitmp = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
	uitmp /= MOBILE_REC_LEN;
	if(uitmp >= 500){//��������
		cls();
		display(0,0,"����:",1);
		display(3,0,"��������!",DIS_CENTER);
		beep(5,50,50);
		g24GDisFlash = 5;
		restore_flag = 0;
		return;//
	}

	ret = G24DealPassCard();
	if(gchn_eng == 'E')
		sprintf(disbuf, "code:%02d ", ret);
	else
		sprintf(disbuf, "����:%02d ", ret);
	gM_Senddll = RECORD24G_TTL;
	switch (ret)
	{
	case ST_ERROR:
		return;
	case 22:
		if(gchn_eng == 'E'){
			display(0,0,"hint:", DIS_Cls);
			display(2,0,"price is error",DIS_CENTER);
		}
		else{
			display(0,0,"��ʾ:", DIS_Cls);
			display(2,0,"Ʊ����Ϣ����ȷ",DIS_CENTER);
		}
		G24End_Card();
		restore_flag = 3;
		return;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"out date card",DIS_CENTER);
		}
		else{
			display(3, 0, "�����ڻ�û����", DIS_CENTER);
		}
		break;
	case 3:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"NO SAVE ACCOUNT",DIS_CENTER);
		}
		else{
			display(3, 0, "����", DIS_CENTER);
		}
		SoundMessage(SOUND_FLING_MONEY);
		break;
	case 6:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"MAC1 eror",DIS_CENTER);
		}
		else{
			display(3, 0, "MAC1�������", DIS_CENTER);
		}
		break;
	case 7:
#ifndef MOBILE_SHIYAN_
#ifndef BUS_PINGXIANG_
#ifdef AUDIO_SOUND_OLD
		audio_old(Audio_TRY_AGAIN);		//����ˢ

#else
		audio(Audio_TRY_AGAIN);		//����ˢ
#endif
#endif 
#endif
		break;
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"illegal card",DIS_CENTER);
		}
		else
			display(3, 0, "�Ƿ���", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		break;
	case ST_OK://�ɹ�
		G24End_Card();
		restore_flag = 3;
		SetTcpPacketTTLTime(RECORD_TTL);
		set_delay_send(set_delay_send_val); //��ʱ������
		return;
	default:
// 		cls();
// 		display(0, 0, disbuf, 0);
// 		display(3, 0, "ĩ�������", DIS_CENTER);
		break;
	}
	g24GDisFlash = 3;
//	delayxs(3);
	restore_flag = 0;
}

void G24MainCard(void)
{
	unsigned int uitmp;
	unsigned char ret;
	char disbuf[20];

	if(F_FLAG_VARY.stop_flag) 
		return;

	if((g24PurError>0)&&(g24PurError<0xa0))//���ֻ�ģ���ʼ�����
		return;
#ifdef MOBILE_SHIYAN_
	if(g24PurError == 0xaa)
		DisRetry();
#else
	if(g24PurError == 0xaa)
		DisRetry2();
#endif

	if((gG24Ver <= 0xA0)||(gG24Ver > 0xA3)){//ûȡ���汾
		check24GVer();
	}

	if(G24Profile() != 1){
		return;
	}
	
	gMCardCand = CARDSTYLE_24Mobile;
	BlightOpen();
// 	if((gmissflag == MISS_M_Settlement)||(gmissflag == MISS_M_TradeBatch)||(gmissNotTrad == 0x55)){
// 		cls();
// 		display(0,0,"��ʾ:", 0);
// 		display(2,0,"���ڴ�����",DIS_CENTER);
// 		display(4,0,"�ֻ����Ժ�",DIS_CENTER);
// 		restore_flag = 3;
// 		G24End_Card();
// 		
// 		g24GDisFlash = 3;
// 		restore_flag = 0;
// 		return;
// 	}

#ifdef _debug_G24Deal_
	debugstring("ѯ���ɹ�:CSNO:");
	debugdata(gG24CardNo, 8, 1);
#endif
	
	if(gPsamErrorFlag&2){
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"error:",1);
			display(3,0,"PSAM2 error!",0);
		}
		else{
			display(0,0,"����:",1);
			display(3,0,"�ƶ�PSAM����!",0);
		}
		restore_flag = 3;
		delayxs(1);
		return;
	}
	if(g24Delay_Normal())
		return;
	
	//�¼����������ж�
	getmobileADD();
	
	uitmp = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
	uitmp /= MOBILE_REC_LEN;
	if(uitmp >= 500){//��������
		cls();
		display(0,0,"����:",1);
		display(3,0,"��������!",DIS_CENTER);
		beep(5,50,50);
		g24GDisFlash = 5;
		restore_flag = 0;
		return;//
	}

	ret = G24DealCard();
	if(gchn_eng == 'E')
		sprintf(disbuf, "code:%02d ", ret);
	else
		sprintf(disbuf, "����:%02d ", ret);
	gM_Senddll = RECORD24G_TTL;
	switch (ret)
	{
	case ST_ERROR:
		return;
	case 22:
		if(gchn_eng == 'E'){
			display(0,0,"hint:", DIS_Cls);
			display(2,0,"price is error",DIS_CENTER);
		}
		else{
			display(0,0,"��ʾ:", DIS_Cls);
			display(2,0,"Ʊ����Ϣ����ȷ",DIS_CENTER);
		}
		G24End_Card();
		restore_flag = 3;
		return;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"out date card",DIS_CENTER);
		}
		else{
			display(3, 0, "�����ڻ�û����", DIS_CENTER);
		}
		break;
	case 3:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"NO SAVE ACCOUNT",DIS_CENTER);
		}
		else{
			display(3, 0, "����", DIS_CENTER);
		}
		SoundMessage(SOUND_FLING_MONEY);
		break;
	case 6:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"MAC1 eror",DIS_CENTER);
		}
		else{
			display(3, 0, "MAC1�������", DIS_CENTER);
		}
		break;
	case 7:
#ifndef MOBILE_SHIYAN_
#ifndef BUS_PINGXIANG_
#ifdef AUDIO_SOUND_OLD
		audio_old(Audio_TRY_AGAIN);		//����ˢ

#else
		audio(Audio_TRY_AGAIN);		//����ˢ
#endif
#endif 
#endif
		break;
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"illegal card",DIS_CENTER);
		}
		else
			display(3, 0, "�Ƿ���", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		break;
	case ST_OK://�ɹ�
		G24End_Card();
		restore_flag = 3;
		SetTcpPacketTTLTime(RECORD_TTL);
		return;
	default:
// 		cls();
// 		display(0, 0, disbuf, 0);
// 		display(3, 0, "ĩ�������", DIS_CENTER);
		break;
	}
	g24GDisFlash = 2;
//	delayxs(3);
	restore_flag = 0;
}
#endif
