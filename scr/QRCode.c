
/*
 �ļ�����QRCode.c
 ����  ��ʵ�ֶ�ά�����ݵĽ��ա�

  */



#include "stdio.h"
#include "string.h"
#include "..\include\include.h"
#include "szct.h"
#include "Font.h"
#include "QRCode.h"

#include "Fatfiles.h"
#include "..\fathead\ff.h"
#include "usbFile.h"
#include "qpboc_8583.h"

#ifdef QR_CODE_COM

#include "..\Lib\RSA.h"

extern Parameter5 cardMessage;
extern stPricetable NewPriceTab;
extern pFistVary_1 pFistVary;
extern Parameter3 ComSumeFile;
extern unsigned int c_serial;
extern unsigned int card_ser;
extern unsigned char cardSound;			//ˢ��û�гɹ���־
extern stcomtime SysTime;
extern unsigned int a_sum1,s_sum1;
extern stBusVerIfo gBusVerInfo;
extern unsigned int startTime,endTime,nowTime,endTimeEn;

extern unsigned char gucRestorFlag;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char gMCardCand;


extern unsigned int a_sumR;//�ֻ�Ǯ���е���ʵ���
extern Parameter2 POINT_MESSAGE;
extern unsigned char gchn_eng;//��Ӣ�ı�־
extern unsigned char BUflag;//���ػ�״̬ 0x02 ��Ҫˢ˾���� 
extern unsigned char gucSoudnorm;
extern unsigned char PsamNo[6];
extern unsigned char DriveCardNo[4];

extern unsigned char gPSAMin;//psam������λ��



extern unsigned char Xor8(const unsigned char *pData, unsigned long DataLen);
extern unsigned int get_s_sum1(unsigned char mode);
extern void Sound_cityCMD(unsigned char sMode);
extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
//extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void SoundFreeManage(char *sstr);
extern unsigned char OperPublicMessage(void);
extern unsigned char getMiKey(void);
extern void error(unsigned char code1,unsigned char Index);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char GetDateTime(void);
extern void getCpuInfo(stCpuInfo *inP);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned char getMiKey(void);
extern unsigned char Delay_Normal(void);
extern void disp_no_swipe(void);
extern void error(unsigned char code1,unsigned char Index);
extern void ControlDis(unsigned char type);
extern unsigned char checkBusInfo(void);
extern unsigned char  CountPointRead(void);
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char CheckSno(unsigned char mode,unsigned char ID,unsigned char *ptr);
extern void getCpuDeviceNO(unsigned char *dat);
//ȡCPU�е���·����Ϣ.
extern int getCpuLineNo(void);


extern unsigned int g24GDisFlash;//24G������ˢ��
extern unsigned char restore_flag;


unsigned char qrcodeRevBuf[100];
volatile unsigned char qrcodeIndex,qrcodeRevStart;

unsigned int QR_BLK_Ver=0;	//���ͨ��ά��������� �汾
unsigned char TACkeydata[16];




void QR_Code_handler(unsigned char value)
{
//	com_snd(COM1, 1, (unsigned char*)&value);//test
	
	//	if (qrcodeIndex==0)	{
	set_timer0(5,2);	
	//	}
	// 	else if (get_timer0(1)==0){
	// 		qrcodeIndex =0;
	// 		qrcodeRevFlg = 0;
	// 	}
	qrcodeRevStart = 1;
	
	if(qrcodeIndex >= 100)
		return;
	
	qrcodeRevBuf[qrcodeIndex] = value;
	qrcodeIndex++;
	
	//	if (qrcodeIndex>=48){		//��Ļ��ʾ���3��,3*16=48, ��ʱ���4.2ms
	//		qrcodeRevBuf[qrcodeIndex] = 0;
	//		qrcodeRevBuf[qrcodeIndex++] = 0;
	
	//		qrcodeRevFlg = 1;
	//		qrcodeIndex =0;
	
	//	}
	
	
}


void show_QR_Ver(void)
{
	char disbuf[30][30];
//	unsigned char  uct;
	
//	unsigned char buff[32];
	unsigned short kk;
	unsigned int i,index=0;
	
	for(i=0; i<30; i++)
		memset(disbuf[i], 0,sizeof(disbuf[i]));
	
	
	// 	f_unlink("1:/curdat/QRB_tmp.bin");
	// 	f_unlink("1:/curdat/QRB.bin");
	
	
	feread(BIT_SPKVER,2,gBusVerInfo.busVoiceVer);

	memcpy((unsigned char *)&kk,gBusVerInfo.busVoiceVer,2);
	//over_turn(2, (unsigned char *)&kk);
	sprintf((char *)disbuf[index++], "��Կ�汾:%04u", kk);

	
	
	feread(BIT_TACVER,2,gBusVerInfo.busTacVer);
	memcpy((unsigned char *)&kk,gBusVerInfo.busTacVer,2);
	//over_turn(2, (unsigned char *)&kk);
	sprintf((char *)disbuf[index++], "TAC�汾:%04u", kk);
	
		
	feread(BIT_QPOC_,2,gBusVerInfo.QPOCVer);

	memcpy((unsigned char *)&kk,gBusVerInfo.QPOCVer,2);
	//over_turn(2, (unsigned char *)&kk);
	sprintf((char *)disbuf[index++], "������Կ�汾:%04u", kk);

	
	dispageInfo_L(index, disbuf);
	
	
}
//�˵����ã�����ÿ�ζ�����
void set_QR_para(void)
{
	char buff[32];
	unsigned char Ccom  = QR_CODE_COM;
	
	qrcodeIndex =0;
	qrcodeRevStart = 0;
	
	com_init(Ccom, 115200);
	COM_it_enable(Ccom);
	
	
	delayxms(5);
	memset(buff,0,32);
	//������nls0006010
	strcpy(buff,(const char *)"NLS0006010;");
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	memset(buff,0,32);
	//��ͬ�����ӳ�����2S
	strcpy(buff,(const char *)"NLS0313010=2000;");
	
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	//ʹ��ǰ׺
	delayxms(30);
	memset(buff,0,32);
	strcpy(buff,(const char *)"NLS0305010;");
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	delayxms(30);
	memset(buff,0,32);
	//�����Զ���ǰ׺0XEEEE
	strcpy(buff,(const char *)"NLS0300000=0xEEEE;");
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	delayxms(30);
	memset(buff,0,32);
	//ʹ�ܽ�����
	strcpy(buff,(const char *)"NLS0309010;");
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	//���ý�����Ϊ0x0D0A
	delayxms(30);
	memset(buff,0,32);
	strcpy(buff,(const char *)"NLS0310000=0x0D0A;");
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	
	//�ر�����nls0006000
	memset(buff,0,32);
	delayxms(30);
	strcpy(buff,( char *)"NLS0006000;");
	com_snd(Ccom,strlen(buff), (unsigned char*)buff);
	
	cls();
	//display(0,0,"�̻�����", DIS_CENTER);
	display(3,0,"���ö�ά��������", 0);
	
	delayxms(1000);
	// 	//����0x06 Ϊ��ȷ��0x15 Ϊ����
}

void QR_Code_init_par(unsigned char Ccom)
{
	qrcodeIndex =0;
	qrcodeRevStart = 0;
	
	com_init(Ccom, 115200);
	COM_it_enable(Ccom);
}

Card_QRC_infor ewm_card; 

//��֯��ά���¼����˾���⣩
void BuildQRCRecorde(unsigned char delType, unsigned char *recBuf, unsigned char *qrcdat, unsigned int qrcdatLen)
{
	stOtherQRCrecord *rRecordDeal;
	unsigned int temp;

	CountPointRead();//����BIT_POINT_MESSAGE  zcz
	rRecordDeal = (stOtherQRCrecord *)recBuf;




	memcpy(rRecordDeal->rQrcrecHead, "��ά��", 6);
	if (delType == ID_REC_QRC_GZT)
		memcpy(rRecordDeal->rQrcrecHead2, "A6", 2);
	else if (delType == ID_REC_QRC_TX)
		memcpy(rRecordDeal->rQrcrecHead2, "A4", 2);
	else if (delType == ID_REC_QRC_ZFB)
		memcpy(rRecordDeal->rQrcrecHead2, "A5", 2);

	else if (delType == ID_REC_QRC_PBOC)
		memcpy(rRecordDeal->rQrcrecHead2, "A7", 2);       

	else
		return;
	POINT_MESSAGE.MonthDealPointer++;
	if (POINT_MESSAGE.MonthDealPointer > 0x3B9AC9FF)
		POINT_MESSAGE.MonthDealPointer = 0;
	temp = POINT_MESSAGE.MonthDealPointer;
	memcpy(rRecordDeal->rCardDealNumb, (unsigned char *)&temp, 4);
	CountPoint(1);
	Get_SerialNumF4(rRecordDeal->rDevSerial);
	rRecordDeal->rCardType = cardMessage.card_catalog;
	rRecordDeal->rDealType = delType;
	memset(rRecordDeal->ruserTimes, 0, 2);

	memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&a_sum1, 4);
	memcpy(rRecordDeal->rDealMoney, (unsigned char *)&s_sum1, 3);
	memcpy(rRecordDeal->rDealTime, (unsigned char*)&SysTime, 7);
	memcpy(rRecordDeal->rDriverNo, DriveCardNo, 4);
	getCpuDeviceNO(pFistVary.DeviceNo);	//ȡ�豸��
	ASCII2ZBcod((char*)pFistVary.DeviceNo, rRecordDeal->rDeviceNo, 4);
	getCpuLineNo();
	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 2);
	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);//����汾
	memcpy(rRecordDeal->rTicket, NewPriceTab.busPrice, 2);	//Ʊ�ۣ�2�ֽڣ����65536��
	memset(rRecordDeal->rQRCdat, 0, sizeof(rRecordDeal->rQRCdat));
	
	MSG_LOG("rQRCdat(%d):", qrcdatLen);
	BCD_LOG(qrcdat, qrcdatLen, 1);
	memcpy(rRecordDeal->rQRCdat, qrcdat, qrcdatLen);
	memcpy(rRecordDeal->rTAC, "\xEE\xFF\xDD\xCC", 4);
	
	MSG_LOG("rQRCdat-bcd :");
	BCD_LOG(recBuf, 256, 1);
	return;
}


//��ͨ��¼������� 11  ��ʷ��¼������ 29  ��Ҫ�� ���� PSAM���� @zzc2016��5��21��11:37:30
void BuildRecorde_QRC(unsigned char delType, unsigned char *recBuf)
{
	DEALRECODE_QRC *rRecordDeal;
	unsigned int temp;
	unsigned char buff_test[4];
	unsigned char initdata[8];
	unsigned char Tac[4];
	//	sysferead(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN,(unsigned char*)&POINT_MESSAGE);
	CountPointRead();//����BIT_POINT_MESSAGE  zcz
	rRecordDeal = (DEALRECODE_QRC *)recBuf;
	
	memset(recBuf, 0, sizeof(DEALRECODE));
	
	temp =POINT_MESSAGE.MoneyDealPointer;
	POINT_MESSAGE.MoneyDealPointer++;
	if(POINT_MESSAGE.MoneyDealPointer > 0x3B9AC9FF)
		POINT_MESSAGE.MoneyDealPointer = 0;
	temp = POINT_MESSAGE.MoneyDealPointer;
	memcpy(rRecordDeal->rCardDealNumb,(unsigned char *)&temp,4);
	
	Get_SerialNumF4(rRecordDeal->rSerialNum);					//���к�
	
	rRecordDeal->rCardType = cardMessage.card_catalog;
	rRecordDeal->rDealType = delType;						//��������   ��˾��ά��  0xA3
	
	
	memcpy(rRecordDeal->rPublishNumb,cardMessage.PublishBicker,10);
	
	
	//	temp = 500;
	//	memcpy(rRecordDeal->rAfterMoney,(unsigned char *)&temp,4);
	memcpy(rRecordDeal->rAfterMoney,ewm_card.Balance,4);
	
	
	memcpy(rRecordDeal->rDealMoney,(unsigned char *)&s_sum1,3);
	memcpy(rRecordDeal->rDealTime,(unsigned char*)&SysTime, 7);
	//	getCpuDriverNum();
	//	memcpy(rRecordDeal->rDriverNo,StufferNO,4);
	memcpy(rRecordDeal->rDriverNo, DriveCardNo, 4);	//˾����.
	memcpy(rRecordDeal->rCityCode,ewm_card.CompanyCode,2); //��Ӫ������
	getCpuLineNo();
	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 2);
	memcpy(rRecordDeal->rCounter,ewm_card.Counter,4);
	memcpy(rRecordDeal->rTicketMac,ewm_card.TicketMAC,4);
	memcpy(rRecordDeal->rTacVer,ewm_card.TacVersion,4);
	
	getCpuDeviceNO(pFistVary.DeviceNo);	//ȡ�豸��	
	ASCII2ZBcod((char*)pFistVary.DeviceNo, rRecordDeal->rDeviceNo, 4);
	
//	debugdata(rRecordDeal->rCardDealNumb,59,1);
	
//	memcpy((unsigned char *)&temp,ewm_card.TacVersion,4);
	
	
	memset(initdata,0,8);
	CountMac(TACkeydata,initdata,rRecordDeal->rCardDealNumb,59,Tac);
// 	debugstring("Tac= ");
// 	debugdata(Tac,4,1);
	memcpy(rRecordDeal->rTAC,Tac,4);
	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);//����汾
	
	SYSgetbussinessNO(buff_test);//���ֽ��̻����
	rRecordDeal->rRemain=buff_test[0];
	rRecordDeal->rReseverX=buff_test[1];
	
	memcpy(rRecordDeal->rEwmData,ewm_card.TagCode,46);
	
}

unsigned int SLZRQRC_Decode(char *iQRdata, char *key,unsigned char *oData)
{
	//
	unsigned char obuf[1024];
	R_RANDOM_STRUCT randomStruct;
	R_RSA_PUBLIC_KEY rrk;
		unsigned int rlen=0,iQRlen=0,keylen=0;
	int i;
	unsigned char dbuf[100];
	unsigned char databuf[200];
	//	unsigned char dbuf[64]={0x7B,0xF1,0x24,0x10,0xD6,0xD1,0x64,0x13,0xC4,0xCB,0xD7,0xF6,0xC3,0x2A,0x04,0x34,0xD9,0x43,0x2A,0x36,0x67,0x50,0x7B,0x82,0xC0,0xE8,0x95,0x85,0x87,0xD0,0x9E,0x48,0x7D,0x20,0x53,0x09,0xF2,0xC3,0xFE,0x3D,0x56,0x9C,0x9C,0x97,0xE8,0x14,0x4B,0x0D,0x60,0x87,0x5D,0x06,0x3F,0x1D,0x94,0xFA,0x52,0x51,0x67,0x75,0xA9,0x25,0x97,0x29};
	//	unsigned char dbuf[64]={0xA8,0xC3,0xAD,0x68,0x3F,0x25,0x5F,0xE2,0x8C,0x71,0x0F,0xC9,0x24,0xF8,0x8A,0x5C,0x07,0x46,0x6D,0x69,0x2F,0xFE,0x1E,0x15,0x85,0xBD,0x57,0x38,0x66,0x51,0x3B,0x3C,0x5E,0xB5,0x43,0x38,0x29,0x88,0xA5,0xBB,0x77,0x87,0xE5,0x40,0xE3,0x74,0x56,0x04,0xB5,0x55,0x9E,0x26,0x52,0x35,0xFB,0x9D,0x2A,0x7F,0x43,0x3E,0xC8,0x7A,0x1E,0xD6};
	//	const char base64_key[176]="AwEAAbUfj/H5A2NAr/sykFXq/AvfpYAOsIU28wzQtBZOSHcssfPNKWglzVGHzmDY8lbPMJPl/JV9QbIU2ZBcNdgwnFqVnUOobbnOytJPvYDl7BjRBaFAMnWggAADxOzU6v/mAwvhrXdBR0s6NXbky1FBbGnMIZ8KJrY3cbGZsNcj6NTN";
	//	const char base64_key[92]="AwEAAdwkoujX/Z8FsRE2CknPpoev9Aj3mXye352yE8w+cmszK4HT+4U4riDNagEn65WpuarVhSTbVj1LOrl+ne4Cg98=";
	unsigned char keydata[200];
	
	iQRlen=base64_decode(iQRdata, databuf);	//�Զ�ά�����ݽ��� 
	memcpy(dbuf,databuf,iQRlen);
	
	MSG_LOG("iQRlen:::");	
    BCD_LOG(dbuf,iQRlen,1);
	
	
	keylen=base64_decode(key, keydata);
	memcpy((unsigned char *)&rlen,keydata+1,3);
	MSG_LOG("reln:::");	
    BCD_LOG((unsigned char *)&rlen,3,1);
	
	MSG_LOG("keylen:::");	
    BCD_LOG(keydata+3,keylen,1);
	
	//	rlen = 0x10001;	////��Կָ��
	
	i=64;												//��Կ����
	BCD_LOG(keydata+4,i,1);
	memcpy(rrk.modulus,keydata+4,i);
	rrk.bits = i*8;//��Կģ����λ��
	rrk.exponent[0] =((rlen>>16)&0xFF);//��Կָ��
	rrk.exponent[1] =((rlen>>8)&0xFF);
	rrk.exponent[2] =(rlen&0xFF);
	
	BCD_LOG(rrk.modulus, rrk.bits/8, 1);
	BCD_LOG(dbuf, 64, 1);   //
	MSG_LOG("rrk.exponent:%02X%02X%02X\r\n", rrk.exponent[0], rrk.exponent[1], rrk.exponent[2]);
	BCD_LOG((unsigned char *)&rlen,4,1);
	
	i = RSAPublicEncrypt(obuf, &rlen, dbuf, 64, &rrk, &randomStruct);  //���ܳ���64�ֽ�
	BCD_LOG((unsigned char *)&rlen,4,1);
	if(i != 0){
		//	ERROR("SIM APUD-2 is ERROR:%d\r\n",i);
		return 0;
	}
	
	MSG_LOG("obuf::");
	BCD_LOG(obuf, rlen, 1);//0001FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000000000010000000001DE5576B7C42A3F3AC80000BE531256262F00
	memcpy(oData, obuf+rlen-44, 44);	//ȡ���44�ֽڣ���ά��Ʊ�ݳ���
	return 44;
}

int dealQRC_SLZR(char *QRCdata, unsigned char *Rdata)
{//��ά���ʽ��ǰ�����ֽڹ̶�'SL'������������
	char buff1[256];
	char SPKKey[200];
	unsigned char TACKey[200];
	unsigned char ewqdata[100];
//	char *p1;
	int i,uilen,addr,ver,tt,t1,t2;

	unsigned char initdata[8];
	unsigned char ewmdata[35];	
	unsigned char outtac[4];

	memcpy(buff1,(unsigned char*)QRCdata,strlen(QRCdata));
	if(memcmp(buff1, "SL", 2) != 0)
		return -1;		//��ά���ʽ����
	memcpy(ewm_card.TagCode,qrcodeRevBuf+2,2);							//���ʶ��  SL

	addr = FLASH_MOBILE_START;							//ȡ��Կ
	flashread(addr, (unsigned char *)&SPKKey, sizeof(SPKKey));
	
	addr = FLASH_QRCTAC_START;							//ȡ��Կ  4�ֽڰ汾+16�ֽ�����									
	flashread(addr, TACKey, 20*TACLEN);
#ifdef _debug_
	debugstring("TAC��Կ:");
	debugdata(TACKey,20*TACLEN,1);

	for(i=0;i<TACLEN;i++)
	{
 		tt =0;
 		memcpy((unsigned char *)&tt,TACKey+i*(4+16),4);
// 		over_turn(4,(unsigned char &)&tt)
// 		MSG_LOG("Ver:%d TAC:",tt);	
		MSG_LOG("VER:");	
		BCD_LOG((unsigned char*)&tt, 4, 0);
		MSG_LOG("TAC:");	
		BCD_LOG(TACKey+i*(4+16)+4,16,1);

	}
#endif

	uilen = SLZRQRC_Decode(buff1+2,SPKKey,ewqdata);	   //���ܶ�ά������	
	memcpy(ewm_card.TagCode+2,ewqdata,uilen);
	MSG_LOG("ewm_card::: ");	
	BCD_LOG(ewm_card.TagCode,uilen+2, 1);

	BCD_LOG(ewm_card.TagCode,2,1);
	BCD_LOG(ewm_card.CompanyCode,2,1);
	BCD_LOG(ewm_card.AccountNo,10,1);
	BCD_LOG((unsigned char *)&ewm_card.Accounttype,1,1);
	BCD_LOG(ewm_card.Counter,4,1);
	BCD_LOG(ewm_card.ExpiredTime,7,1);
	BCD_LOG(ewm_card.MaxDealMoney,4,1);
	BCD_LOG(ewm_card.Balance,4,1);
	BCD_LOG(ewm_card.TicketMAC,4,1);
	BCD_LOG(ewm_card.TacVersion,4,1);
	BCD_LOG(ewm_card.TAC,4,1);

	

	memcpy(&c_serial,ewm_card.TAC,4);

	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
	{
		disp_no_swipe();
		return ST_OK;	
	}

	
	i = cardMessage.card_catalog;
	if (i >0x80)
	{
		i -= 0x80;
	}
	if (i >0x40)
	{
		i -= 0x40;
	}
	
	if( (NewPriceTab.rate[i] == 104) || (NewPriceTab.rate[i] == 204))
	{
		MessageBox(1, "��ά���ֹˢ��");
		return ST_OK;
	}


	MSG_LOG("111����:0x%04X\r\n",cardMessage.card_catalog);

// 	debugstring("ewm_card:");
// 	debugdata((unsigned char *)&ewm_card,sizeof(ewm_card),1);
// 
#ifdef _debug_
	debugstring("QR TACVER:");
	debugdata(ewm_card.TacVersion,4,1);
#endif



	memcpy((unsigned char *)&ver,ewm_card.TacVersion,4);			//��TAC   �ȱȽ�TAC�汾  ��ʱ5���汾����TAC�汾һ�µ�TAC���Ƚ�
	sprintf((char *)(buff1+10),"%08u",ver);	//
	Ascii2BCD(buff1+10,buff1,8);
	MSG_LOG("buff:");
	BCD_LOG(buff1,4,1);
	memcpy((unsigned char*)&ver, buff1,4);

	MSG_LOG("ver::: ");	
	BCD_LOG((unsigned char*)&ver, 4, 1);
	for(i=0;i<TACLEN;i++)
	{
		memcpy((unsigned char *)&tt,TACKey+i*(4+16),4);
		MSG_LOG("tt::: ");	
		BCD_LOG((unsigned char*)&tt, 4, 1);
		if(tt == ver)
		{
			memset(initdata,0,8);
			memset(outtac,0,sizeof(outtac));
			memcpy(ewmdata,ewm_card.CompanyCode,sizeof(ewm_card)-6);
			memcpy(TACkeydata,TACKey+i*(4+16)+4,16);
			CountMac(TACkeydata,initdata,ewmdata,sizeof(ewm_card)-6,outtac);
			
// 			debugstring("outtac����::: ");	
// 			debugdata(outtac, 4, 1);

			
			if(memcmp(outtac,ewm_card.TAC,4) != 0){
				debugstring("TAC����!\r\n");
				debugdata(outtac, 4, 1);
			
				return -4;

			}
			break;
		}
	}
	if(i>=TACLEN){
		MSG_LOG("TAC����!\r\n");
		return -4;
	}


//	cardMessage.card_catalog = ewm_card.Accounttype;		//����
	memcpy(cardMessage.PublishBicker,ewm_card.AccountNo,10);//�ʺű��
	memcpy(&a_sum1,ewm_card.Balance,4);						//���
	MSG_LOG("a_sum1:%d\r\n",a_sum1);
	
	BCD_LOG((unsigned char*)&SysTime,7,1);
	BCD_LOG((unsigned char*)&ewm_card.ExpiredTime,7,1);

	t1 = BCDTime2Long((unsigned char *)&ewm_card.ExpiredTime);	//��ʱ��ת����
	t2 = BCDTime2Long((unsigned char*)&SysTime);	//��ʱ��ת����

// 	BCD_LOG((unsigned char *)&t1,4,1);
// 	BCD_LOG((unsigned char *)&t2,4,1);
	if((t2 - t1) > 0){
		//debugstring("��ʱ!\r\n");
		return -3;			//��ʱ
	}
	
	i = cardMessage.card_catalog;
	
	if( (NewPriceTab.rate[i] == 102) || (NewPriceTab.rate[i] == 202))
	{
		s_sum1 = 0;
	}else{

		s_sum1 = get_s_sum1(0);	//ȡ���ѽ��
	}

#ifdef _debug_
	s_sum1 = 0;
	MSG_LOG("����0Ԫ--\r\n");
#endif


	if(a_sum1 < s_sum1){
		//debugstring("����!\r\n");
		return -5;
	}

#ifndef _debug_
	if((s_sum1 == 0))
	{
		

		display(0,0,"��ʾ:", DIS_Cls);
		display(2,0,"Ʊ�۲���ȷ!",DIS_CENTER);
		g24GDisFlash = 1;
		restore_flag = 0;
		delayxms(1500);
		return -6 ;
	}

#endif
//	Save_delayCardinfo(0);
	
	save_h_month();



	BuildRecorde_QRC(ID_REC_QRC_SLZR, Rdata);
	
	return 128;

}
extern unsigned int ghisCur;//��ʷ��ǰָ��
unsigned char qrDisp_history(unsigned char mode, unsigned int Index)
{
	unsigned char buffer[80];
	unsigned int temp;//,temp1;
//	unsigned char i;
	stOtherQRCrecord historyCheck;

	unsigned int addr;
	int curp;

#ifdef _debug_
	debugstring("qFIND:Index:");
	debugdata((unsigned char*)&Index, 4, 1);
#endif
	if (mode == 1)
		Index += 2;
	else {
		if (Index > 2)
			Index -= 2;
		else
			Index = 2;
	}
	#ifdef _debug_
	debugstring("qFIND:Index:");
	debugdata((unsigned char*)&Index, 4, 1);
#endif
	if ((ghisCur <= FLASH2_START1) || (ghisCur >= FLASH2_START2END)) {
		writeBackRec(buffer, 0, 0xA5);
	}
	curp = ghisCur;
	addr = (Index /*+ 1*/) * RECORD_LEN;
#ifdef _debug_
	debugstring("qFIND:curp:");
	debugdata((unsigned char*)&curp, 4, 1);
	debugstring("qFIND:addr:");
	debugdata((unsigned char*)&addr, 4, 1);
#endif
	if (addr > (curp - FLASH2_START1))
	{
		addr = FLASH2_START2END - (addr - (curp - FLASH2_START1));
	}
	else
		addr = curp - addr;

	flashread(addr, buffer, 8);
#ifdef _debug_
	debugstring("qHistory:");
	debugdata((unsigned char*)&addr, 4, 1);
	debugdata(buffer, 64, 1);
#endif

	if (memcmp(buffer, "��ά��", 6) != 0)
		return ST_ERROR;//������������

	flashread(addr , (unsigned char *)&historyCheck, 256);

#ifdef _debug_
	debugstring("find:");
	debugdata((unsigned char*)&Index, 4, 1);
	debugdata((unsigned char*)&historyCheck, 132, 1);
	debugdata((unsigned char*)&historyCheck.rDealType, 1, 1);
#endif
	memset(buffer, 0xff, 70);
	if (memcmp(buffer, (unsigned char*)&historyCheck, RECORD_LEN) == 0) {
		return 0xEE;
	}
	temp = 0;
	switch ((historyCheck.rDealType))
	{
	case ID_REC_QRC_PBOC:
		cls();
		memset(buffer, 0, 20);
		temp = 0;
		memcpy((unsigned char*)&temp, historyCheck.rDealMoney, 3);
	//	over_turn(4, (unsigned char*)&temp);
		sprintf((char*)buffer, _Font_KouKuandian, temp / 100, temp % 100);
		display(2, 0, (char*)buffer, 0);
		memset(buffer, 0, 20);
		temp = 0;
// 		memcpy((unsigned char*)&temp, historyCheck.rAferMoney, 4);
// 		over_turn(4, (unsigned char*)&temp);
// 
// 		// 		memcpy((unsigned char*)&temp1, historyCheck.rDealMoney, 3);
// 		// 		temp -= temp1;
 		sprintf((char*)buffer, _Font_Balancedian, temp / 100, temp % 100);
 		display(4, 0, (char*)buffer, 0);
		dis_time_mini(6, (stcomtime*)&historyCheck.rDealTime);
			display(6, 0,"ʱ��:",0);
		break;

	case ID_REC_BLKLIST:
		cls();
		display(3, 0, "�ڿ���¼", DIS_CENTER);
		break;
	default:
		return ST_ERROR;
	}

	sprintf((char*)buffer, "%d ", Index + 1);
	miniDispstr(9, (21 - strlen((char*)buffer)), (char*)buffer, DIS_RIGHT);
	temp = 0;
 
	memset(buffer, 0, 40);
	strcat(buffer,"������:");
     
	memcpy(buffer+strlen("������:"),historyCheck.rQRCdat+1,historyCheck.rQRCdat[0]);
//	BCD2Ascii(historyCheck.rQRCdat, buffer, 10);
	display(0, 0, (char*)buffer, 0);
	return ST_OK;
}


///
void SLZRQRCmain(char *QRCdat)
{
	int ret,i;
	//char disbuf[64];
	unsigned char rrrecd[256];
	
	GetDateTime();
	gMCardCand = CARDSTYLE_QR;
#ifdef _M1KEY_CLOUD_
	cardMessage.card_catalog = CARD_NORMAL_BUS;
#else

	cardMessage.card_catalog = CARD_QR_BUS;			//
	cardMessage.Zicard_catalog = 0;
#endif

	MSG_LOG("��ά�뿨��̶�:0x%04X\r\n",cardMessage.card_catalog);


#ifdef _debug_
			debugstring("��ά��===����::: ");
			debugstring((char*)QRCdat);
			debugstring("\r\n");
				BCD_LOG(QRCdat,60,1);
#endif	

	ret = strlen(QRCdat);
	MSG_LOG("��ά�볤��:%d%\r\n",ret);
// 	ret = dealQRC_SLZR(QRCdat,rrrecd);
// //	printf("OK:ret = %d\r\n record:", ret);
// 	if(ret > 0){
// 		//debugdata(rrrecd, ret, 1);
// 		
// 		money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
// 		SoundMessage(SOUND_DEAL);
// 		
// 		WriteRecord(rrrecd, 0);	
// 		
// 		return ;
// 	}
	
	if ((memcmp(QRCdat, "SL", 2) == 0))
	{
		ret = dealQRC_SLZR(QRCdat, rrrecd);
		//	printf("OK:ret = %d\r\n record:", ret);
		if (ret > 0) {
			//debugdata(rrrecd, ret, 1);

			money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
			SoundMessage(SOUND_DEAL);
			set_delay_send(set_delay_send_val); //��ʱ������
			WriteRecord(rrrecd, 0);

			return;
		}
	}
	else if (memcmp(QRCdat, "62", 2) == 0/*  ret < 30*/) {
//#if SWITCH_PBOC
// 		if ((g_supportQR&qr_weiXin) == 0) {
// 			if (QRCdat[0] == '1') {
// 				MessageBox(1, "��֧��΢�Ÿ�����");
// 				ret = -999;
// 			}
// 		}
// 		if ((g_supportQR&qr_aliPay) == 0) {
// 			if (QRCdat[0] == '2') {
// 				MessageBox(1, "��֧��֧����������");
// 				ret = -999;
// 			}
// 		}
		if (ret != -999) {
			ret = qpboc_qr_main(QRCdat, rrrecd);
			MSG_LOG("qpboc_qr_mainret:%d\n", ret);
		}
//#else	// ��ʱ��֧�ָ�����
//		ret = -1;
//#endif
	}
	else
	{
			ret = -1;
	}
		if (ret > 0) {
		MSG_LOG("money_msg:%d\n", s_sum1);
		//debugdata(rrrecd, ret, 1);
		//a_sum1 = a_sum1 - s_sum1;
		money_msg(ID_REC_TOLL, -1, s_sum1, 0);
		MSG_LOG("SoundMessage:\n");
		SoundMessage(SOUND_DEAL);

		MSG_LOG("QTXQRCWriteRecord:\n");
	//	QTXQRCWriteRecord(rrrecd);
		i = 0;
		while(1){
			WriteRecord(rrrecd+i, 0);//��ά��256�ֽڣ���ʵ�ʵķ�������д��
			i += RECORD_LEN;
			if(i >= 256)
				break;	//д��
		}
		return;
	}
	if(ret == -1){
		MessageBox(1, "��ά���ʽ����");
	}
	else if(ret == -2){
		MessageBox(1, "��ά���ʽ����");
	}
	else if(ret == -3){
		MessageBox(1, "��ά��ʱ�����");
	}
	else if(ret == -4){
		MessageBox(1, "��ά��TAC����");
	}
	else if(ret == -5){
		MessageBox(1, "��ά������");
	}
	else {
		MessageBox(1, "����ʧ��");
	}

	led_off(ALL_LED);
	SoundMessage(SOUND_FLING_MONEY);




	return ;

	
}
int QRC_CardDeal_SLZR(void)
{
	
//	MSG_LOG("in:%s", (char*)qrcodeRevBuf+2);
	
	if(qrcodeRevBuf[0]==0xDD)
		SLZRQRCmain((char*)qrcodeRevBuf+1);
	else
		SLZRQRCmain((char*)qrcodeRevBuf+2);
	
	return 0;
}
void QR_Code_Test_4500R(void)
{
	char *p;
//	unsigned short crc16=0,ustemp=0;
//	unsigned char buffer[128];
	unsigned char resPonse=0;
// 	unsigned int uilen=0;

	if (qrcodeRevStart)				//���ս�����
	{
		if (get_timer0(2)==0){		//��ʱ�������
			qrcodeRevBuf[qrcodeIndex] = 0;
//			qrcodeRevBuf[qrcodeIndex++] = 0;
#ifdef _debug_
			debugstring("��ά������::: ");
			debugstring((char*)qrcodeRevBuf);
			debugstring("\r\n");
#endif			
// 			cls();
// 			if(strlen((char*)qrcodeRevBuf) > 48)
// 				miniDispstr(0,0,(char *)qrcodeRevBuf,0);
// 			else
			p = strchr((char*)qrcodeRevBuf, 0x0D);
			if(p!=null)
				*p = 0;		//ȥ��0D0A
//			display(0,0,(char *)qrcodeRevBuf+2,0);
#ifdef _debug_
			debugdata(qrcodeRevBuf, 100, 1);
#endif

#if 0
			cls();
			//miniDispstr(0,0,(char *)qrcodeRevBuf+2,0);
			display(0,0,(char *)qrcodeRevBuf+2,0);
			audio(Audio_DONG);
			
			g24GDisFlash = 2;
			restore_flag = 0;
			
			qrcodeIndex =0;
			qrcodeRevStart =0;
			set_timer0(0, 2);

			return;
#endif
			

			MSG_LOG("qrcodeIndex:%d,ewmlen:%d\r\n",qrcodeIndex,sizeof(Card_QRC_infor));
			memset(&ewm_card,0,sizeof(Card_QRC_infor));
			
			Ascii2BCD(qrcodeRevBuf+2,(unsigned char *)&ewm_card,sizeof(Card_QRC_infor)*2);

			MSG_LOG("shuju:");
			BCD_LOG((unsigned char *)&ewm_card,sizeof(Card_QRC_infor),1);
			//over_turn(4,(unsigned char *)cardMessage.PublishBicker);
			//cardMessage.card_catalog = CARD_NORMAL;
			//over_turn(4,(unsigned char *)&a_sum1);

			MSG_LOG("a_sum1:%d\r\n",a_sum1);




			resPonse = QRC_CardDeal_SLZR();//��˾��ά�봦��




			switch(resPonse) {
			case ST_ERROR:
				return;
			case CARD_NO_Busins:
				restore_flag = 1;
				led_on(LED_RED);
				MessageBox(1, Card_NotBzcard);
				sprintf((char*)irda_rxBuf, "BN:%02X%02X", cardMessage.TradeCode[0], cardMessage.TradeCode[1]);
				display(4, 0, (char*)irda_rxBuf, DIS_CENTER);
				end_card();
				break;
			case ST_OK:
				
				end_card();
				break;
			case CARD_NO_TIME:		
				if(F_FLAG_VARY.stop_flag) return;
				cls();
				display(0,0,Card_Worring,0);
				display(2,1,Expired_card,0);
				beep(3, 100, 50);
				audio(Audio_FLING_MONEY);	//��Ͷ��

				restore_flag=3;
				end_card();
				break;
			case CARD_forbid:
				MessageBox(1, Card_forbidCard);
				led_on(LED_RED);
				restore_flag=3;
				end_card();
				break;
			case CARD_BLACK_PRO:
			case CARD_BLACK:
				cls();
				display(2,0,Please_Coin,DIS_CENTER);
				display(0,0,"����:��������!",0);
				SoundMessage(INVALID_CARD);
				restore_flag=3;
				end_card();
				break;
			case CARD_STOPWORK:		
				error(ERR_CARD_STOP_WORK,0);
				break;
			case CARD_NO_SYSTEM:	
				error(ERR_CARD_NO_SYSTEM,0);
				break;
			case CARD_WHITE_BUS:				//�׿�
				error(ERR_CARD_WHITE_BUS,0);
				break;
			case CARD_NO_USE:
				error(ERR_CARD_NO_USE,0);
				break;
			case INVALID_CARD:
				SoundMessage(INVALID_CARD);
				restore_flag=3;
				end_card();
				break;


			case CARD_MONEY:
				if(checkBusInfo() > 10)
				{
					cls();
					display(0,0,Card_Worring,1);
					display(3,2,"�豸��Ϣ����",0);
				}
				else{
					//if(MoneyResultManage_QRC(0)==ST_ERROR)
					//	break;   //Ǯ����
				}
				end_card();

				g24GDisFlash = 2;
				restore_flag = 0;

				
				break;
			case MONTH_CARD:

				end_card();
				g24GDisFlash = 2;
				restore_flag=0;

				break;
		#ifndef BUS_PRO_JSB
			case CARD_STUFF_BUS:
				//FreeConsume();
				end_card();
				break;
		#endif
			case 150:
				restore_flag=3;

				break;
			case CONTROL_CARD:
				//ControlMagane();
				restore_flag=3;
				break;
			default:
				break;
			}



			
			g24GDisFlash = 2;
			restore_flag = 0;

			qrcodeIndex =0;
			qrcodeRevStart =0;
			set_timer0(0, 2);
		}
	}
	
}









unsigned char save_QR_blkVer(unsigned int a)
{
	MSG_LOG("QR_blkVer:%d\r\n",a);
	sysfewrite(BIT_QR_BLK_VER,BIT_QR_BLK_VER_LEN,(unsigned char *)&a);
	return ST_OK;
}
unsigned int get_QR_blkVer(void)
{
	unsigned int a;
	sysferead(BIT_QR_BLK_VER,BIT_QR_BLK_VER_LEN,(unsigned char *)&a);
	return a;
}




//1 ������  0 ����  uiid  mode 0��������  1,��������
unsigned char find_BLK_QR(unsigned char *UIID,unsigned char *UserID ,unsigned char mode)
{

	FRESULT res;
	FILINFO finf;
	FIL rfile;
	unsigned int length=0,i=0,uitemp=0;
	// 		float f1;
	DIR tdir;
//	unsigned char buf[64];
	char fileName[50];
	unsigned char find_flag=0;

	st_QR_BLK blk;
	unsigned char blksize=sizeof(blk);

	stcomtime Time_blk;

	
	
	res = f_opendir(&tdir, "1:/curdat");
	if(res != FR_OK){
		dis_messgebox("��Ŀ¼��", res, 12);
		res = f_mkdir("1:/curdat");
		if(res != FR_OK){
			dis_messgebox("����Ŀ¼��", res, 12);
			return 0;
		}
	}
	
	memset(fileName, 0, sizeof(fileName));
	strcpy(fileName, "1:/curdat/QRB.bin");	//���ͨ��ά�������

	MSG_LOG(" Find blk FileName:%s\r\n", fileName);
	//û�������ļ����Լ�����
	res = f_open(&rfile, fileName, FA_OPEN_EXISTING | FA_READ);
	if((res != FR_OK))//if(access("0:/current/Rindex.txt",0)!=0)
	{
		if (mode ==0)
		{		
			save_QR_blkVer(0);//������
		}
		//dis_messgebox("��QR�ļ���", res, 12);
		return 0;
	}
	MSG_LOG("cell data 2\r\n");



	res = f_stat(fileName, &finf);
	if(res != FR_OK){
		f_close(&rfile);
		//dis_messgebox("QR�ļ���Ϣ��", res, 12);
		return 0;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);


	MSG_LOG("qrlen:%d\r\n",blksize);


	if (( length < blksize )|| (length >=  QR_BLK_MAX*blksize) || (length ==0))
	{
		QR_BLK_Ver = 0;
		save_QR_blkVer(QR_BLK_Ver);//̫���ˣ�����
		MSG_LOG("Ver:%u��������\r\n",QR_BLK_MAX*blksize);
		//����һ���µĿյ��ļ�

		f_close(&rfile);
// 		MSG_LOG("�������ļ�:%s\r\n", fileName);
// 		res = f_open(&rfile, fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);//���ǽ������ļ�

		f_unlink(fileName);				//дʧ�ܣ�ɾ���Ѿ�д���ļ�
		
		memset(fileName, 0, sizeof(fileName));
		strcpy(fileName, "1:/curdat/QRB_tmp.bin");	//���ͨ��ά�������
		f_unlink(fileName);				//дʧ�ܣ�ɾ���Ѿ�д���ļ�




		return 0;
	}

	
//ֱ������ȥ
	if (mode ==0)
	{
		return 0;
	}
	

	i = length;




#ifdef _debug_
	MSG_LOG("Search:");
	BCD_LOG(UIID,3,1);
	BCD_LOG(UserID,8,1);
#endif

	do 
	{
		i -= blksize;
		f_lseek(&rfile, i);	//��λ���ļ�
		if(_Fread((void *)&blk, blksize, &rfile) != blksize){
			break;		
		}

		MSG_LOG("UID:");
		BCD_LOG(blk.UIID,3,1);
		MSG_LOG("USER_ID:");
		BCD_LOG(blk.UserID,8,1);

		memcpy(&uitemp,blk.EenTime,4);
		over_turn(4,(unsigned char *)&uitemp);
		Long2BCDTime(uitemp, (unsigned char *)&Time_blk);

		MSG_LOG("endtime:");
		BCD_LOG((unsigned char *)&Time_blk,7,1);


		if (memcmp(&SysTime.year_h,&Time_blk.year_h,4) !=0)
		{
			break;
		}

		if (memcmp(UIID,blk.UIID,3) !=0)
		{
			continue;
		}

		if (memcmp(UserID,blk.UserID,8) !=0)
		{
			continue;
		}
		
		if (blk.DealType[0] != 0)//������״̬
		{
			find_flag = 1;
			break;
		}

	} while (i);




	
	f_close(&rfile);


	if (find_flag)
	{
		return 1;
	}

	return 0;


}




int write_BLK_QR(unsigned char *filehead,unsigned char *wbuf,unsigned int blknum)
{

	FRESULT res;
	FILINFO finf;
	FIL rfile;
	unsigned int length=0;//,i=0,uitemp=0;
	// 		float f1;
	DIR tdir;
//	unsigned char buf[64];
	char fileName[50];
 //	unsigned char find_flag=0;
	
	st_QR_BLK blk;
	unsigned char blksize=sizeof(blk);
	
//	stcomtime Time_blk;
	
	unsigned int serverMaxVer=0,packMaxVer=0;


	memcpy(&serverMaxVer,filehead+2,4);
	over_turn(4,(unsigned char *)&serverMaxVer);

	memcpy(&packMaxVer,filehead+6,4);
	over_turn(4,(unsigned char *)&packMaxVer);



	MSG_LOG("Maxid:%d,SMaxID:%d,num:%d\r\n",packMaxVer,serverMaxVer,blknum);

	if (blknum ==0)//û������
	{
		save_QR_blkVer(packMaxVer);//̫���ˣ�����
		return 0;
	}


	
	
	res = f_opendir(&tdir, "1:/curdat");
	if(res != FR_OK){
		dis_messgebox("��Ŀ¼��", res, 12);
		res = f_mkdir("1:/curdat");
		if(res != FR_OK){
			dis_messgebox("����Ŀ¼��", res, 12);
			return -1;
		}
	}
	
	memset(fileName, 0, sizeof(fileName));
	strcpy(fileName, "1:/curdat/QRB_tmp.bin");	//���ͨ��ά�������
	
	MSG_LOG("Write QR FileName:%s\r\n", fileName);
	MSG_LOG("��|�������ļ�:%s\r\n", fileName);

	//û�������ļ����Լ�����
	res = f_open(&rfile, fileName, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("��QR�ļ���", res, 12);
		return -2;
	}

	res = f_stat(fileName, &finf);
	if(res != FR_OK){
		f_close(&rfile);
		dis_messgebox("QR�ļ���Ϣ��", res, 12);
		return -3;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//

	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);


	f_lseek(&rfile, length);	//��λ���ļ������

	if(_Fwrite(wbuf, blknum*blksize, &rfile) !=  blknum*blksize){
		f_close(&rfile);
		dis_messgebox("дQR�ļ���", res, 12);
		return -4;
	}


	//��������(2B)+ƽ̨�汾(4B)+��ǰ�汾(4B)+��ǰ��С(4B)+����

	
	
	if (packMaxVer == serverMaxVer)
	{

		//�����ļ�
		MSG_LOG("QRB_tmp.bin -->QRB.bin");

		f_close(&rfile);

		cp_file("1:/curdat/QRB_tmp.bin","1:/curdat/QRB.bin");


// 		MSG_LOG("DEL %s\r\n",fileName);
// 		f_unlink(fileName);				//дʧ�ܣ�ɾ���Ѿ�д���ļ�


		
	}


	save_QR_blkVer(packMaxVer);



	f_close(&rfile);

	return 0;

	

}

//���ͨ322������ֲ������
extern int calFileCRC(unsigned int fileName,unsigned int fileLen);
//ȡ �汾 
unsigned char LoadfileVer(unsigned int flash_Addr,unsigned char *outver)
{
	unsigned char buftemp[32];
	unsigned int crc32,len,flashcrc;
	unsigned short Ver=0;
	
	//	char *ptemp=NULL;//,*p1=NULL;	//��ʼ�������
	//	char * p1 =NULL,p2=NULL;
	
	flashread(flash_Addr,buftemp,12);
	memcpy(&Ver,buftemp,4);
	memcpy(&len,buftemp+4,4);
	memcpy(&flashcrc,buftemp+8,4);
	
	MSG_LOG("load,file:");
	BCD_LOG(buftemp,12,1);
	
	
	memset(outver,0,2);
	
	if ((Ver ==0) || (Ver == 0xffff))
	{
		return ST_ERROR;
	}
	if (len >4096)
	{
		return ST_ERROR;
	}
	crc32 = calFileCRC(flash_Addr+12,len);				
	//	MSG_LOG("cal crc32:%08X,crd32%08X\r\n",crc32,flashcrc);
	if (crc32 != flashcrc)
	{
		return ST_ERROR;
	}	
	MSG_LOG("LoadfileVer:%08X\r\n",flash_Addr);
	BCD_LOG(outver,2,1);
	memcpy(outver,&Ver,2);
	
	return ST_OK;
}



unsigned char saveSPKInfo(unsigned int flash_Addr)
{
	unsigned int ulTemp;
//    unsigned int len;
	unsigned char buftemp[4000];
	unsigned int crc32,flashcrc,len_all;
	unsigned short Ver=0;

	//У���ļ�

	flashread(flash_Addr,buftemp,12);
	memcpy(&Ver,buftemp,4);
	memcpy(&len_all,buftemp+4,4);
	memcpy(&flashcrc,buftemp+8,4);


	MSG_LOG("save TOO Long%d\r\n",len_all);
	BCD_LOG(buftemp,12,1);

	crc32 = calFileCRC(flash_Addr+12,len_all);				
	MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32,flashcrc);
	if (crc32 != flashcrc)
	{
		return ST_ERROR;
	}
	//����ʱ

	flashread(flash_Addr+12,buftemp,len_all);
	MSG_LOG("buftemp%d\r\n",len_all);
	BCD_LOG(buftemp,len_all,1);


    //����汾������
	memcpy(gBusVerInfo.busVoiceVer,(unsigned char *)&Ver,2);
 	sysfewrite(BIT_SPKVER, 2, gBusVerInfo.busVoiceVer);
	
	ulTemp = FLASH_MOBILE_START;              
	flasherase(ERASE4KBYTESECTOR, ulTemp);
   
//	flashwrite(ulTemp,buftemp+22,SPK_DATA_LEN);	
//	flashwrite(ulTemp,buftemp,SPK_DATA_LEN);
	flashwrite(ulTemp,buftemp,len_all);

	

#ifdef _debug_
	
	ulTemp = FLASH_MOBILE_START;
	flashread(ulTemp,buftemp, len_all);

	debugstring("������Կ���ݣ�");
	debugdata(buftemp,len_all,1);	
#endif

	return ST_OK;

}

unsigned char saveTACInfo(unsigned int flash_Addr)
{
	unsigned int ulTemp;
//	unsigned char i;
//	unsigned char buf[20];
    unsigned int i=0;
	unsigned char buftemp[4000];
	unsigned int crc32,flashcrc,len_all;
	unsigned short Ver=0;
	//	temp1 = BlackLength*4;

	//У���ļ�

	flashread(flash_Addr,buftemp,12);
	memcpy(&Ver,buftemp,4);
	memcpy(&len_all,buftemp+4,4);
	memcpy(&flashcrc,buftemp+8,4);

/*	if (len_all >512)
	{
	//	MSG_LOG("save TOO Long%d\r\n",len_all);
		return ST_ERROR;
	}
*/
	MSG_LOG("save TOO Long%d\r\n",len_all);
	BCD_LOG(buftemp,12,1);

	crc32 = calFileCRC(flash_Addr+12,len_all);				
	MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32,flashcrc);
	if (crc32 != flashcrc)
	{
		return ST_ERROR;
	}
	//����ʱ

	flashread(flash_Addr+12,buftemp,len_all);

	MSG_LOG("buftemp_asc::%d\r\n",len_all);
	BCD_LOG(buftemp,len_all,1);
//														����У��
	
	Ascii2BCD(buftemp,buftemp,len_all);
//
	MSG_LOG("buftemp_bcd::%d\r\n",len_all);
	BCD_LOG(buftemp,len_all,1);

	//����汾������
	memcpy(gBusVerInfo.busTacVer,(unsigned char *)&Ver,2);
	sysfewrite(BIT_TACVER, 2, gBusVerInfo.busTacVer);


	ulTemp = FLASH_QRCTAC_START;              
	flasherase(ERASE4KBYTESECTOR, ulTemp);
	for(i=0;i<TACLEN;i++)
	{
		ulTemp = FLASH_QRCTAC_START + i*20;
		
		flashwrite(ulTemp,buftemp+i*TAC_DATA_LEN,4);	   //TAC�汾		
		ulTemp += 4;
		flashwrite(ulTemp,buftemp+i*TAC_DATA_LEN+8,16);	   //TAC
	}
	


// #ifdef _debug_
	
	ulTemp = FLASH_QRCTAC_START;
	flashread(ulTemp,buftemp, 20*TACLEN);

// 	debugstring("����TAC��Կ���ݣ�");
// 	debugdata(buftemp,17*TACLEN,1);	
//#endif
	return ST_OK;

}

#endif

