
/**************************************************
* �ļ��� :main.c
* ����   :JC-3000���ļ�
* ����   :21/3/2008	by KSLi
**************************************************/
#include "string.h"
#include "stdio.h"
//#include "Board.h"
#include "Icon.h"
#include "..\include\include.h"
//#include "public.h"
#include "Gszct.h"
#include "szct.h"
//#include "timer_isr.h"
#include "Mydes.h"
#include "mainBU.h"
#include "citydifferent.h"
#include "Font.h"
#include "usbFile.h"
#include "QRCode.h"
#include "bluetooth.h"
#include "qpboc_8583.h"
#ifdef _FASH_FATs_SYS_
#include "Fatfiles.h"
#endif

#ifndef NO_MOBILE_SERVISE
#include "E_Gmobile.h"
#endif
#include "E_Deal24GCard.h"
#ifndef NO_GJ_SERVISE
#include "E_GJGPRS.h"
#endif
#ifdef BUS_CPU_
#include "E_CPUCard.h"
#include "Cpucard.h"
#endif
#ifdef qPBOC_BUS
#include "qpboc_head.h"
#include "qPBOC.h"
#endif
#include "qpboc_8583.h"
#ifdef FengDuan_BU_
#include "FenDuanTicket.h"
#endif

#ifdef BUS_PRO_JTB
#include "CPUCARJTB.h"
#endif

#ifdef BUS_PRO_JSB
#include "CPU_Union.h"
#endif

#include "StructDef.h"

#include "ProcCharacter.h"
//#include "MFRC531.h"
//#include "crc8.h"

//extern TCP_IP_PACKET1 tcpipPacket;

//extern BLACK_ADD2 BLACK_ADD;
extern stLineInfo LineInfo[2];//��·��Ϣ�������վ����Ϣ ��������0�� ����    1�鷴��
extern volatile unsigned char USART1mode;//��ǰ���ڴ���ʲô����״̬
unsigned int startTime,endTime,nowTime,endTimeEn;
#ifdef CARD_SOUND
	unsigned char cardSound;			//ˢ��û�гɹ���־
	unsigned int card_ser;			
#endif
#ifdef GPRS_SOUD_ENABLE
extern volatile unsigned char ucMC323Mode;//GPRSģ��Ĺ���״̬��A1 �е绰����,��Ҫ��ʾ��A2��ʾ��ɣ�A3�������绰��AX�Ķ�������������δ����
#endif
unsigned char RC531offtime;
unsigned char CardIscity;	// 1-���ľ�Ŀ������� ��ͨ������
unsigned int ghisCur;//��ʷ��ǰָ��
Parameter3 ComSumeFile;
unsigned char gPsamErrorFlag;//PSAM�Ƿ����
unsigned int g24GDisFlash;//24G������ˢ��
unsigned char restore_flag;
fFlagVary_1 F_FLAG_VARY;
Parameter2 POINT_MESSAGE;
stPricetable NewPriceTab;

unsigned char PsamNo[6];
unsigned char DriveCardNo[4];
unsigned char StufferNO[4];
Parameter8 CARD_PUBLIC;
Parameter8 CARD_PUBLIC1;
pFistVary_1 pFistVary;
//������ÿ������
unsigned short gBlackBlockNum[16];
void PRICE_init_frist(void);
unsigned int last_card_no;
#ifdef BUS_PRO_JTB
extern unsigned char CPUPsamInitJTB(void);
extern unsigned char WriRecorJTB(unsigned char *rec,unsigned char mode);
#endif
//������:�ֻ���03\�մ￨ 04\������01\CPU��02
unsigned char gMCardCand;
unsigned int c_serial;
Parameter5 cardMessage;

struct pMonthTime_1 MothDelayTime;
unsigned char BUflag;//���ػ�״̬ 0x02 ��Ҫˢ˾���� 

unsigned char gucSoudnorm;
unsigned char gucEndTimeFlag;
//��Щ����Ҫ������Ů��
unsigned char nnAudioflag;

extern unsigned char USART2mode;//��ǰ���ڴ���ʲô����״̬


#ifdef BUS_SYBAOSHI_//ֻ���ֻ����ţ�û��M1������
volatile unsigned char gDX24ComFlag;//���ڽ��ձ�־
unsigned char gDX24ComRBuf[500];
unsigned int gDX24ComRindex;
unsigned int gDX24GProFlag;//24G��ѯ��ָ�� 0:��Ҫ��ѯ������ ����:����ѯ������,�Ȼ�Ӧ����Ϣ
#endif 

#ifdef _TTSSound_enable_
extern unsigned char xfSounding;
#endif

unsigned char CheckSno(unsigned char mode,unsigned char ID,unsigned char *ptr);
unsigned char checkBusInfo(void);
void saveCpuInfo(unsigned char mode, unsigned char *dat);

void clr_Record_point(void);
void testXF(void);
void ControlDis(unsigned char type);
unsigned char GetDateTime(void);
unsigned char FreeConsume(void);
//extern void DEBUG_It(void);
extern void GPRSSASK(unsigned char *dat, unsigned char cmd, unsigned int len);
extern void GPRS_close_Gt900(unsigned int dlyt);
#ifndef NO_MOBILE_24G_
extern void first_clr_mobile_para(void);
extern void clr_mobile_record(void);
extern void getMobilePara(void);
#endif
extern void main_DiaoDu(void);
extern unsigned char ValidDevNo(unsigned char *dvcbuf);
extern void debugCOM1tring(char *str);//--ͨ������4
extern void debugCOM1data(unsigned char *value, unsigned int uclen, unsigned char mode);//----ͨ������4
#ifdef _GPS_ENABLE_
extern void main_Gps(void);
extern void WriteGPSRec(unsigned char mode);
#endif

extern void Save_delayCardinfo(unsigned char Cmode);
extern void Sound_cityCMD(unsigned char sMode);
extern unsigned char SYSgetbussinessNO(unsigned char *dat);

#ifndef NO_GJ_SERVISE
extern void aoutMcheckIP(void);
extern void loadGPRSFE(void);
extern void gprs_par_init(void);
extern void ClrGPRSFE(void);
extern void GPRS_INIT(unsigned int band);
extern void delallBlacInbus(void);
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char gmissOldflag;
extern unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.
extern unsigned short gSendOverTime;//���ͺ���Ӧ��ʱ������
#endif
extern unsigned char gWIfiIpConfig[18];//IP config��Ϣ���洦MAC 6+IP 4+ Mask 4 + Gateway 4 
#ifdef _CPU_24G_
extern void CPU24GMainCard(void);
extern void G24init(void);
extern void Check24Card(void);
extern void RestCPU24Card(void);
#endif
extern int getCardtype(char *cardD, unsigned char type);
#ifdef _YiDa_Card_
extern void YiDaMainCard(void);
extern void YD_BuildGPSMsginfo(unsigned char *buff);
#endif
#ifdef ZKXL_24G_
extern void comParaInit(void);
extern void ZKXL_Main(void);
#endif
#ifdef _XMSHDZ_24G_
extern void OcomParaInit(void);
extern void XMSH_Main(void);
#endif

#ifdef Card_typeB_enable_
	unsigned char gCartTypeA0B1;
#endif
#ifdef KEYBOARD
	#define KEYBOARD_INPUT 0x55
	unsigned char KeyBoardStyle=0;	//�����Ƿ������룬0x55-�м������룬����-û�м������롣û�м�������ʱʹ�û���Ʊ�ۣ��м�������ʱʹ������Ʊ��
	extern void KeyDis(unsigned int value,unsigned char *ptr);
	extern unsigned char KeyDeal(void);
	extern void LedDisplay(unsigned char *ptr);
	extern void keyboard_open(void);
	extern void SendCommandkey(unsigned char p1,unsigned char p2);
#endif

#ifdef OTHER_GPRS
	extern void gprs_other_sndbuinfo(void);
#endif

#ifdef _TTSSound_enable_
	extern unsigned char DealADTmpFile(void);
	//����0����ȷ����������,����������ADver �� ADlineNO��ʮ�����ƣ���λ��ǰ��ADdatetimeΪBCD�롣
	extern int getADSoundFileVer(unsigned char *ADver, unsigned char *ADlineNO, unsigned char *ADdatetime);
	extern void debugADsoundInfo(void);
#endif


	//U�̲ɼ�������
extern int main_usb_collect(void);

// #ifdef BUS_PRO_JTB
// extern	unsigned char CPUPsamInitJTB(void);
// #endif
//��ȡ��������ˢ����ʱʱ��
extern unsigned char Waittime(void);

#ifdef BUS_YICHUN
//�Ƿ��кܶ�����û�д����ˣ�����о������
void checkDataSend(void)
{
	unsigned int curp, headp, ltemp;
	char disbuf[16];
	
	curp = Get_Record_point((unsigned char*)&headp, 0);
	ltemp=0;
	ltemp=curp - headp;
	ltemp/=RECORD_LEN;
	if(ltemp >= 50){
		sprintf(disbuf, "%u", ltemp);
		miniDispstr(6, 0, disbuf, 0);
		led_on(LED_RED);
	}
}
#endif


#ifdef _XMSHDZ_24G_
extern int Profile_XMSH24G(unsigned char *scardID);
extern void XMSFBreaKCard(void);
extern void comParaInit(void);
extern unsigned char XMSHRFstyle;
extern void XMSH_end_card(void);
#endif

//���2.4G���ڣ�ѯ2.4G���Ƿ�ӽ�����STProLib�����á�
unsigned char Profile_24G(void)
{
#ifdef _XMSHDZ_24G_
	unsigned char buff[10];
	int ret;
	
	ret = Profile_XMSH24G(buff);
	if(ret == 1){// �п�
		XMSFBreaKCard();
		comParaInit();
		return ST_OK;
	}
	else if(ret >= 0){//�յ���ͷ����
		comParaInit();
		if((ret > 0)||(XMSHRFstyle > 1)){
			XMSFBreaKCard();
		}
	}
#endif
	return ST_ERROR;
}
void end_card_24G(void)
{
#ifdef _XMSHDZ_24G_
	XMSH_end_card();
#endif
}

void SYSsetdatetime(void)
{
//	const unsigned char row[] = {1,2,3,4,6,7,9,10,12,13,15,16,18,19};
	unsigned char buff[50];
	unsigned char buff1[50];
	stcomtime sSysTime;
	unsigned char pos=0;
	unsigned char keyv;
	unsigned char index;

	cls();
	display(0,0,ModyfiDataTime, DIS_CENTER);
	display(2, 0, InputDataTime, 0);

	timeread((unsigned char *)&sSysTime);
	BCD2Ascii((unsigned char*)&sSysTime.year_h, buff+pos,2); pos = 4;
	buff[pos] = '-'; pos++;
	BCD2Ascii((unsigned char*)&sSysTime.month, buff+pos,1); pos += 2;
	buff[pos] = '-'; pos++;
	BCD2Ascii((unsigned char*)&sSysTime.day, buff+pos,1); pos += 2;
	buff[pos] = ' '; pos++;
	BCD2Ascii((unsigned char*)&sSysTime.hours, buff+pos,1); pos += 2;
	buff[pos] = ':'; pos++;
	BCD2Ascii((unsigned char*)&sSysTime.minutes, buff+pos,1); pos += 2;
	buff[pos] = ':'; pos++;
	BCD2Ascii((unsigned char*)&sSysTime.seconds, buff+pos,1); pos += 2;
	buff[pos] = 0;
	miniDispstr(5, 1, (char*)buff, 0);
	index = 0;
	buff1[0] = buff[index];
	buff1[1] = 0;
	miniDispstr(5, 1, (char*)buff1, 1);
//	debugstring((char*)buff);
//	debugstring((char*)buff1);
//	delayxms(200);
	keyv = 0;
	while(1){
		keyv = getkey(1);
//		if(keyv != 0){
//			debugstring("K:");
//			debugdata((unsigned char*)&keyv, 1, 1);
//		}
		switch(keyv) {
		case KEY_5:  //��ֵ��1
			buff[index]++;
			if(buff[index] > '9')
				buff[index] = '0';
			buff1[0] = buff[index];
			buff1[1] = 0;
			miniDispstr(5, index+1, (char*)buff1, 1);
			break;
//		case KEY_DOWN:
//			if(buff[index] == '0')
//				buff[index] = '9';
//			else
//				buff[index]--;
//			buff1[0] = buff[index];
//			buff1[1] = 0;
//			miniDispstr(5, index+1, (char*)buff1, 1);
//			break;
		case KEY_3:
			return;
//		case KEY_F1:   //����1λ
//			if(index>0)
//				index--;
//			else 
//				index = 18;
//			if((index==4)||(index==7)||(index==10)||(index==13)||(index==16))
//				index--;
//			miniDispstr(5, 1, (char*)buff, 0);
//			buff1[0] = buff[index];
//			buff1[1] = 0;
//			miniDispstr(5, index+1, (char*)buff1, 1);
// 			break;
		case KEY_1:
			index++;
			if(index>18)
			{
				index = 0;
				keyv=0;			
				pos=0;
				Ascii2BCD(buff, (unsigned char *)&sSysTime.year_h, 4);
				Ascii2BCD(buff+5, (unsigned char *)&sSysTime.month, 2);
				Ascii2BCD(buff+8, (unsigned char *)&sSysTime.day, 2);
				Ascii2BCD(buff+11, (unsigned char *)&sSysTime.hours, 2);
				Ascii2BCD(buff+14, (unsigned char *)&sSysTime.minutes, 2);
				Ascii2BCD(buff+17, (unsigned char *)&sSysTime.seconds, 2);
				if(time_invalid((unsigned char*)&sSysTime)==0){
					debugdata((unsigned char*)&sSysTime, 7, 1);
					display(2, 0, _Font_Inputerror, DIS_CENTER);
					delayxs(1);
					index=0;
					miniDispstr(5, 1, (char*)buff, 0);
					buff1[0] = buff[index];
					buff1[1] = 0;
					miniDispstr(5, index+1, (char*)buff1, 1);
					break;
				}
				timewrite((unsigned char*)&sSysTime);
				display(2, 0, _Font_Complete, DIS_CENTER|DIS_ClsLine);
				delayxs(1);
				return;
			}
			if((index==4)||(index==7)||(index==10)||(index==13)||(index==16))
				index++;
			miniDispstr(5, 1, (char*)buff, 0);
			buff1[0] = buff[index];
			buff1[1] = 0;
			miniDispstr(5, index+1, (char*)buff1, 1);
			break;
		default:
			break;
		}
		delayxms(20);
	}
}

void getProVer(void)
{	
	unsigned char ver[64];
	unsigned short j,k;
	
	getGpsProVER(ver);//����汾��
	
// 	MSG_LOG("����汾��:");
// 	BCD_LOG(ver, 64, 1);

	memcpy(gBusVerInfo.busProVer, ver, 2);//ȡǰ2λ�汾��
	memcpy((unsigned char*)&j, ver, 2);//ȡǰ2λ�汾��
	k = SOFT_VER_TIME_LOG;
	memset(ver, 0x01, 7);
	memcpy(ver, ver+2, 4);
	if((j >= 9999)||(time_invalid(ver)==0)||(k>j)){//����汾���Ϸ�,��û��GPRS�汾,��GPRS�汾С
		j = SOFT_VER_TIME_LOG;
		memcpy(gBusVerInfo.busProVer, (unsigned char*)&j, 2);
	}
	
// 	MSG_LOG("������汾��:");
// 	BCD_LOG(gBusVerInfo.busProVer, 2, 1);
}

#ifdef BUS_HBYS_//����ͳ��
void save_month_tongji(unsigned char modee)
{
	unsigned char tbuf[2][14];
	unsigned char mothflag;
	unsigned int addr;

	addr = BIT_MOTH_TOJI_2;
	sysferead(addr, 14, tbuf[0]);
	addr += 14;
	sysferead(addr, 14, tbuf[1]);

	MSG_LOG("m_tongji0:");
	BCD_LOG(tbuf[0], 14, 1);

	mothflag = (SysTime.month % 2);
	addr = __cpuCrc32(tbuf[mothflag], 10);
	if(memcmp((unsigned char*)&addr, tbuf[mothflag]+10, 4) != 0){//У���
		memset(tbuf[mothflag], 0, 14);
	}
	if(memcmp(tbuf[mothflag], (unsigned char*)&SysTime.year_l, 2) != 0){//���ǵ�ǰ��
		memset(tbuf[mothflag], 0, 14);
	}
	tbuf[mothflag][0] = SysTime.year_l;
	tbuf[mothflag][1] = SysTime.month;
	memcpy((unsigned char*)&addr, tbuf[mothflag]+2, 4);//ˢ���ܴ���
	addr += 1;
	memcpy(tbuf[mothflag]+2, (unsigned char*)&addr, 4);
	if(modee == ID_REC_TOLL){	//Ǯ�����ѲŻ��
		memcpy((unsigned char*)&addr, tbuf[mothflag]+6, 4);//ˢ���ܽ��4
		addr += s_sum1;
		memcpy(tbuf[mothflag]+6, (unsigned char*)&addr, 4);
	}
	addr = __cpuCrc32(tbuf[mothflag], 10);
	memcpy(tbuf[mothflag]+10, (unsigned char*)&addr, 4);

	addr = BIT_MOTH_TOJI_2;
	addr += (mothflag*14);

	MSG_LOG("m_tongji:");
	BCD_LOG(tbuf[mothflag], 14, 1);

	sysfewrite(addr, 14, tbuf[mothflag]);
}

void show_month_tongji(void)
{
	unsigned char tbuf[2][14];
	char disbuf[8][20];
	unsigned char disindex;
	unsigned int addr;
	
	addr = BIT_MOTH_TOJI_2;
	sysferead(addr, 14, tbuf[0]);
	addr += 14;
	sysferead(addr, 14, tbuf[1]);

	MSG_LOG("show_tongji0:");
	BCD_LOG(tbuf[0], 14, 1);
	BCD_LOG(tbuf[1], 14, 1);
	
	for(disindex=0; disindex<8; disindex++)
		memset(disbuf[disindex], 0, 20);
	disindex = 0;
	

	MSG_LOG("show_tongji1:");
	BCD_LOG((unsigned char*)&disindex, 1, 1);

	addr = __cpuCrc32(tbuf[0], 10);

	MSG_LOG("show_tongji2:");
	BCD_LOG((unsigned char*)&addr, 4, 1);

	strcpy(disbuf[disindex], "  ˢ���·�ͳ��  ");
	disindex++;

	if(memcmp((unsigned char*)&addr, tbuf[0]+10, 4) != 0){//У���
		memset(tbuf[0], 0, 14);
	}
	else
	{
		sprintf(disbuf[disindex], "%02X��%02X��", tbuf[0][0], tbuf[0][1]);
		disindex++;
		memcpy((unsigned char*)&addr, tbuf[0]+2, 4);
		sprintf(disbuf[disindex], "�ܴ���:%u", addr);
		disindex++;
		memcpy((unsigned char*)&addr, tbuf[0]+6, 4);
		sprintf(disbuf[disindex], "�ܽ��:%d.%02dԪ", addr/100, addr%100);
		disindex++;
	}

	MSG_LOG("show_tongji3:");

	addr = __cpuCrc32(tbuf[1], 10);

	MSG_LOG("show_tongji4:");
	BCD_LOG((unsigned char*)&addr, 4, 1);

	if(memcmp((unsigned char*)&addr, tbuf[1]+10, 4) != 0){//У���
		memset(tbuf[1], 0, 14);
	}
	else{
		sprintf(disbuf[disindex], "%02X��%02X��", tbuf[1][0], tbuf[1][1]);
		disindex++;
		memcpy((unsigned char*)&addr, tbuf[1]+2, 4);
		sprintf(disbuf[disindex], "�ܴ���:%u", addr);
		disindex++;
		memcpy((unsigned char*)&addr, tbuf[1]+6, 4);
		sprintf(disbuf[disindex], "�ܽ��:%d.%02dԪ", addr/100, addr%100);
		disindex++;
	}
	if(disindex < 3){
		MessageBox(0, "��ˢ��ͳ��!");
		delayxs(1);
		return;
	}

	MSG_LOG("show_tongji5:");
	BCD_LOG(tbuf[0], 14, 1);
	BCD_LOG(tbuf[1], 14, 1);

	dispageInfo(disindex, disbuf);
}
#endif	//��ͳ��


void intdev(void)
{
	open_wdt();
	port_init();
	
	/*���ڳ�������*/
#ifdef _debug_
//	com_init(COM1, 115200);
	com_init(COM1, 256000);//3000L 1.3 BOOT �汾���Ͽ��������
#else


#if defined BUS_YICHUN || defined Card_snd1111
	com_init(COM1, 9600); //�˴������Ȼ� 9600
#elif KEYB_COM==1 && defined KEYBOARD
	com_init(COM1, 19200); //��Ҫ�Ӽ���


#elif defined OTHER_GPRS
	#ifdef COM2RJ45_
		com_init(COM1, 19200);	//�������Ȼ� �Լ�Э�� 19200
	#else
		com_init(COM1, 19200);	//�������Ȼ� �Լ�Э�� 19200
	#endif
#elif defined _GPS_Station_ExKey_enable_
	com_init(COM1, 19200);	//�ⲿ����վ���� ��Σ��¾ɶ���COM1��
#else
	com_init(COM1, 256000);//3000L 1.3 BOOT �汾���Ͽ��������
#endif
#endif //#ifdef _debug_
  
	COM_it_enable(COM1);




	//����RS232���
#ifdef KEY_PAD
	com_init(COM4, 38400);
#elif KEYB_COM == 4 && defined KEYBOARD//��Ӽ��� 19200
	com_init(COM4, 19200);
#else
	com_init(COM4, 115200);
#endif
	COM_it_enable(COM4);

//	timer0_init();
	
	disinit();
	MSG_LOG("intDev 1");
	led_init();
	beepinit();
	MSG_LOG("intDev 3");
	key_init();
	
	audioinit();
	spiInit();
	MSG_LOG("intDev 6");
	I2C_EE_Init();
	timeinit();
	MSG_LOG("intDev 5");
	LED_3INT();
	MSG_LOG("intDev 7");
	
	com2port();
	MSG_LOG("intDev 8");
	voice_init();
	MSG_LOG("intDev 9");
	
	InitRC531();

	saminit();
	MSG_LOG("intDev 10");

	Rc531TxClose();
	RC531Close();

	MSG_LOG("intDev 4");

	set_timer0(500, 1);

// 	while(1){
// 		clr_wdt();
// 		display(0,0,"����......",0);
// 
// 		if(get_timer0(1) == 0){
// 			led_convert(0xff);
// 			set_timer0(500, 1);
// 		}
// 	}

#ifndef NO_GJ_SERVISE
#ifdef Modem_BUS_
	GPRS_INIT(115200);
#elif defined ALL4G
	GPRS_INIT(115200);//�˶˿�����GPRS���ӿ�
#else

	GPRS_INIT(9600);//�˶˿�����GPRS���ӿ�
#endif
#endif	

#ifdef _CPU_24G_
	com_init(G24Com, 115200);
	COM_it_enable(G24Com);
#elif defined _GPS_ENABLE_
 	com_init(GPS_COM, 9600);
	COM_it_enable(GPS_COM);
#elif ((defined _XMSHDZ_24G_)||(defined ZKXL_24G_)||(!defined NO_MOBILE_24G_))
 	com_init(G24Com, 115200);
	COM_it_enable(G24Com);
#else
	com_init(COM5, 9600);
//	COM_it_enable(COM5);
#endif

#ifdef QR_CODE_COM
	QR_Code_init_par(QR_CODE_COM);//��ά��˿ڱ�����ʼ��
#endif



	usb_init();
	MSG_LOG("intDev ok");
}

unsigned char  CountPointRead(void)
{
	unsigned char CRC;
	unsigned char i=0;
	for(i=0;i<5;i++)
	{
		feread(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN ,(unsigned char*)&POINT_MESSAGE);
		CRC=CRC8_Tab((unsigned char*)&POINT_MESSAGE,sizeof(Parameter2)-1);
		if(CRC==POINT_MESSAGE.crc)
			break;

		MSG_LOG("CRC_ERRO\r\n");
		BCD_LOG((unsigned char*)&CRC,1,1);
		BCD_LOG((unsigned char *)&POINT_MESSAGE,sizeof(POINT_MESSAGE),1);
	//	beep(5,100,100);
	}
		if(i>=5)
			return ST_ERROR;
		else
			return ST_OK;
	
}
void CountPoint(unsigned char mode)
{

	POINT_MESSAGE.crc=CRC8_Tab((unsigned char*)&POINT_MESSAGE,sizeof(Parameter2)-1);
	fewrite(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN ,(unsigned char*)&POINT_MESSAGE);

}

void GetEndWrite(unsigned char *Bbuf)
{
#ifdef BUS_HANDAN_XX
	Get_data_1 rRecord;
	unsigned char buffer[50];
	unsigned int temp200;
	int curp,headp;
	
	curp = Get_Record_point((unsigned char*)&headp, 0);

	MSG_LOG("GetEndWrite!!\r\n");

	rRecord.rDealType=ID_REC_PAKAGE;
	
	memcpy(rRecord.rPublishNumb,cardMessage.PublishBicker,4);
	POINT_MESSAGE.ControlDealPointer+=1;
	if(POINT_MESSAGE.ControlDealPointer>0x3B9AC9FFl)
		POINT_MESSAGE.ControlDealPointer=0;
	memcpy(rRecord.rCardDealNumb, (unsigned char*)&POINT_MESSAGE.ControlDealPointer, 4);
	memcpy(rRecord.rDealTime,(unsigned char*)&SysTime,7);
	memcpy(rRecord.MoneySum, (unsigned char*)&ComSumeFile.SumMoney, 4);
	memcpy(rRecord.MonthSum,(unsigned char*)&ComSumeFile.SumMti, 2);
	temp200=(unsigned int)((curp+LEN_EEPROM-headp)%LEN_EEPROM);
	temp200/=32;
	temp200+=3;
	memcpy(rRecord.RecordSum, (unsigned char*)&temp200, 2);
	rRecord.rReserveByte=0xff;
	
	//	CountMAC((DEALRECODE*)&rRecord);//MAC
	memcpy(Bbuf, (unsigned char*)&rRecord, RECORD_LEN);

	rRecord.rDealType=ID_REC_MOBILE;
	rRecord.rCardType=cardMessage.card_catalog;
	memcpy(rRecord.rPublishNumb,cardMessage.PublishBicker,4);
	POINT_MESSAGE.ControlDealPointer+=1;
	if(POINT_MESSAGE.ControlDealPointer>0x3B9AC9FFl)
		POINT_MESSAGE.ControlDealPointer=0;
	memcpy(rRecord.rCardDealNumb, (unsigned char*)&POINT_MESSAGE.ControlDealPointer, 4);
	memcpy(rRecord.rDealTime,(unsigned char*)&SysTime,7);
	memset(rRecord.MoneySum, 0, 4);
	memset(rRecord.MonthSum, 0, 2);
	memcpy(rRecord.RecordSum,pFistVary.LineNo,2);
	rRecord.rReserveByte=pFistVary.LineNo[2];
	//	CountMAC((DEALRECODE*)&rRecord);//MAC
	memcpy(Bbuf+RECORD_LEN, (unsigned char*)&rRecord, RECORD_LEN);
	
	memset(buffer,0,14);
	BCD2Ascii((unsigned char*)&SysTime, buffer+32, 7);
	memcpy(buffer,buffer+32,9);
	
	buffer[9]=ID_REC_NO;
	memcpy(buffer+10,buffer+41,3);
	memcpy(buffer+13,pFistVary.DeviceNo,9);
	
	memcpy((unsigned char*)&temp200,pFistVary.fileSno,2);
	sprintf((char*)buffer+22,"%04d",temp200);
	memset(buffer+26,0xff,6);
	
	//	CountMAC((DEALRECODE*)buffer);//MAC
	memcpy(Bbuf+(RECORD_LEN*2), buffer, RECORD_LEN);
	CountPoint(2);

	BCD_LOG(Bbuf, 32, 1);
	BCD_LOG(Bbuf+RECORD_LEN, 32, 1);
	BCD_LOG(Bbuf+(RECORD_LEN*2), 32, 1);
#endif
}

void BlightOpen(void)
{
	gucBLight = 200;
	dislight(1);
}
/*
unsigned char CountMAC(DEALRECODE *recordd)
{
	unsigned char pos=0,re=0;
	unsigned char buffer[50];

	MSG_LOG("CountTAC	0\r\n");

	//��ʼ������
	memset(recordd->rMAC, 0x00, 4);
	memset(buffer, 0x00, 16);
	//��֯��Կ��ɢ����
	memcpy(buffer, recordd->rPublishNumb , 4);	//������ˮ ������ɢ����
	buffer[4]=0xff;
	memcpy(buffer+8, pFistVary.PostCode, 2);	//���д��� һ����ɢ����
	buffer[10]=0xff;
	
	MSG_LOG("PSAMInitForDescrypt(0x44, 0x01, 16, buffer) \r\nbuffer= ");
	BCD_LOG(buffer,16,1);

	//des�����ʼ��
	re=init_for_descrypt(gPSAMin,0x44,0x01,0x10,buffer);
	if(re==0)
	{
		MSG_LOG("PSAMInitForDescrypt error !\r\n");
		return ST_ERROR;
	}
	
	//�����������
	memset(buffer, 0x00, 8);
	memcpy(buffer+8, (unsigned char *)&recordd, 28);
	//	memcpy(buffer,(unsigned char *)&rRecord.RECORD_MONEY[wBuffer_po],28);
	buffer[28+8]=0x80;
	memset(buffer+29+8, 0, 3);
	pos=0;
	pos=MAC(gPSAMin,0x05,0,0x28,buffer);
	if(pos!=0)
	{
		memcpy(recordd->rMAC, buffer, 4);
		return ST_OK;
	}
	else{
		return ST_ERROR;
	}
}
*/



//����8���ֽ� ��Card_typeProcess  ������������
void getCpuDeviceNO(unsigned char *dat)
{
	stCpuInfo cpuinfo;
	unsigned int offset;
	unsigned char buff[16];
	
	memset(buff, '0', 16);
	buff[15] = 0;
	offset = ((unsigned char*)&cpuinfo.deviceNO)-((unsigned char*)&cpuinfo);
	cpuInfoRead(buff, offset, 8);
	//	MSG_LOG((char*)buff); 
	//����豸��
	if((memcmp(buff, "00000000", 8)==0)||\
		(memcmp(buff, "00000001", 8)==0)||\
		(ValidDevNo(buff) == 1)){
		sysferead(BIT_DEVICE_NO,9,buff);
	}
	memcpy(dat, buff, 8);
}
//����ȡ˾�����ŵĺ���
int getCpuDriveNo(void)
{
	stCpuInfo cpuinfo;
	unsigned int offset, pi;
	
	offset = ((unsigned char*)&cpuinfo.DriverNO[0])-((unsigned char*)&cpuinfo);
	cpuInfoRead((unsigned char*)&pi, offset, 4);
	
	if(pi ==0){
		
		sysferead(BIT_DRICE_CARD,4,DriveCardNo);
	}
	else{
		memcpy((unsigned char*)&DriveCardNo, (unsigned char*)&pi, 4);
		
	}
	
	return pi;
}
//ȡCPU�е�Ʊ����Ϣ.
int getCpuPrice(void)
{
	stCpuInfo cpuinfo;
	unsigned int offset, pi;
#ifdef KEYBOARD
	if(KeyBoardStyle == KEYBOARD_INPUT){//����Ӽ������룬���ó��ڽ�����Ʊ�ۡ�
		sysferead(BIT_KeyPrice, 4, (unsigned char*)&pi);//2014.7.17
		memcpy(NewPriceTab.busPrice, (unsigned char*)&pi, 4);
	}
	else{
#endif
		offset = ((unsigned char*)&cpuinfo.buPrice[0])-((unsigned char*)&cpuinfo);
		cpuInfoRead((unsigned char*)&pi, offset, 4);
		
		if(pi > 20000){
			sysferead(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);
		}
		else{
			memcpy(NewPriceTab.busPrice, (unsigned char*)&pi, 4);
		}
#ifdef KEYBOARD
	}
#endif
	
	return pi;
}

//ȡCPU�е��ۿ���Ϣ.
int getCpuRate(void)
{
	stCpuInfo cpuinfo;
	unsigned char gcrbuf[32];
	unsigned int offset;
	
	offset = ((unsigned char*)&cpuinfo.rate[0])-((unsigned char*)&cpuinfo);
	cpuInfoRead(gcrbuf, offset, 32);
	
	// 	debugstring("gCR--");
	// 	debugdata(gcrbuf, 32, 1);
	
	for(offset=0; offset<32; offset++){
		NewPriceTab.rate[offset] = gcrbuf[offset];
	}

// 	memset(NewPriceTab.rate, 100, 32);
// 	NewPriceTab.rate[0] = 100;
// 	NewPriceTab.rate[1] = 80;
// 	
// 	NewPriceTab.rate[2] = 202;
// 	NewPriceTab.rate[3] = 50;
// 	NewPriceTab.rate[4] = 202;
// 	NewPriceTab.rate[5] = 80;
	return 0;
}
//ȡCPU�е���·����Ϣ.
int getCpuLineNo(void)
{
	stCpuInfo cpuinfo;
	unsigned int offset;
	
	offset = ((unsigned char*)&cpuinfo.LineNo[0])-((unsigned char*)&cpuinfo);
	cpuInfoRead(pFistVary.LineNo, offset, 3);
	
	// 	debugstring("gCL--");
	// 	debugdata(pFistVary.LineNo, 3, 1);
	
	return 0;
}


void addStatMoney(unsigned char mode, unsigned int moneyv)
{
	unsigned char index;
	stStaeInfo stainto;
	unsigned int it, ii;
	
	if((mode != ID_REC_TOLL)&&(mode != ID_REC_MON))
		return;
	
	for(ii=0;ii<3;ii++){
		sysferead(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);//ÿ�����һ����ʷָ���ͳ������ ����ʱ��7+��ֵ�ۼƽ��4+�����ۼƽ��4+�ۿ�����2+����11+CRC32(4)
		
		it = GenerateCRC32((unsigned char*)&stainto, (sizeof(stStaeInfo)-4));

		MSG_LOG("Ra stStaeInfo:");
		BCD_LOG((unsigned char*)&stainto, sizeof(stStaeInfo), 1);
		BCD_LOG((unsigned char*)&it, 4, 1);
		
		if(stainto.sCrc32 == it)
			break;
	}
	if(ii >= 3){
		memset((unsigned char*)&stainto, 0, sizeof(stStaeInfo));
		memcpy(stainto.DataTime, (unsigned char*)&SysTime, 7);
		MSG_LOG("bͳ�Ƽ�������\r\n");
	}
	
	for(index=0; index<8; index++){//�Ҵ˲���Ա�ı���λ��
		if(memcmp(stainto.infoBuf[index].cardNo, DriveCardNo, 4) == 0){
			break;
		}
		if(memcmp(stainto.infoBuf[index].cardNo, "\x00\x00\x00\x00", 4) == 0){
			break;
		}
	}
	if(index >= 8)
		index = 7;

	MSG_LOG("check index1:");
	BCD_LOG((unsigned char*)&index, 1, 1);
	MSG_LOG("::");
	BCD_LOG((unsigned char*)&stainto.infoBuf[index], sizeof(stSatone), 1);

	it = 0;
	if(mode == ID_REC_TOLL){//Ǯ��
		memcpy(stainto.infoBuf[index].cardNo, DriveCardNo, 4);
		stainto.infoBuf[index].chargeA += moneyv;
		
		stainto.infoBuf[index].SaleA++;
		it = GenerateCRC32((unsigned char*)&stainto, sizeof(stStaeInfo)-4);
		stainto.sCrc32 = it;
		sysfewrite(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);
	}
	else if(mode == ID_REC_MON){//��Ʊ
		memcpy(stainto.infoBuf[index].cardNo, DriveCardNo, 4);
		stainto.infoBuf[index].RseverA += 1;//s_sum1;
		
		stainto.infoBuf[index].SaleA++;
		it = GenerateCRC32((unsigned char*)&stainto, sizeof(stStaeInfo)-4);
		stainto.sCrc32 = it;
		sysfewrite(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);
	}
	MSG_LOG("add moneyEND!\r\n");
}

unsigned int getRECORD_LEEN(void)
{
	return RECORD_LEN;
}
unsigned char WriteRecord(unsigned char *buffer, unsigned char mode)
{
	unsigned char ret;

	MSG_LOG("write REC:");
	BCD_LOG(buffer, RECORD_LEN, 1);
// #ifdef BUS_PRO_JTB
// 	WriRecorJTB(buffer,1);
// #else
	ret = libWriteRecord(BIT_COMSUME_FILE, buffer, mode);
//#endif
// #ifdef _FASH_FATs_SYS_
// 	WriteRecordFAT(buffer, 0);//��¼д�뵽FAT�ļ���
// #endif
	return ret;
}

#ifdef BUS_HANDAN_XX
void Write_Head_Record(unsigned char mode)
{
	Get_data_2 rRecord;
	Record_Control_32B rRecord2;
	//	unsigned char buffer[50];
	unsigned char i;
	//д��һ��˾������¼
	GetDateTime();//��ʱ��
	
	rRecord.rCardType=CARD_DRIVER_BUS;
	rRecord.rDealType = ID_REC_DRIVENO;
	memcpy(rRecord.rPublishNumb, DriveCardNo, 4);
	if(memcmp(rRecord.rPublishNumb,"\x00\x00\x00\x00",4)==0)
		rRecord.rPublishNumb[0]=1;
	
	POINT_MESSAGE.ControlDealPointer+=1;
	if(POINT_MESSAGE.ControlDealPointer>0x3B9AC9FFl)
		POINT_MESSAGE.ControlDealPointer=1;
	memcpy(rRecord.rCardDealNumb, (unsigned char*)&POINT_MESSAGE.ControlDealPointer, 4);
	
	memcpy(rRecord.rDealTime, (unsigned char *)&SysTime, 7);
	
	memcpy((unsigned char *)rRecord.rDriveNo,rRecord.rPublishNumb, 4);
	
	memset((unsigned char *)rRecord.rLineNo, 0, 4);
	memcpy((unsigned char *)rRecord.rLineNo+2, pFistVary.LineNo, 2);
	rRecord.rLineNO_1 = pFistVary.LineNo[2];
	//	memcpy((unsigned char *)cardMessage.PublishBicker, (unsigned char *)DriveCardNo, 4);
	
//	CountMAC((DEALRECODE*)&rRecord);//MAC
	//	CountPoint(2);		
	WriteRecord((unsigned char*)&rRecord, 0);
	
	//д��һ����·��¼
	memset((unsigned char*)&rRecord2, 0xff, RECORD_LEN);
	rRecord2.rCardType=0;//CARD_LINE_SET;
	rRecord2.rDealType=ID_REC_LINENO;
	//memcpy(rRecord.RECORD_HEAD[wBuffer_po].rPublishNumb,cardMessage.PublishBicker,4);
	memcpy(rRecord2.rPublishNumb, DriveCardNo, 4);
	memcpy(rRecord2.rPublishNumb2, DriveCardNo, 4);
	POINT_MESSAGE.ControlDealPointer+=1;
	if(POINT_MESSAGE.ControlDealPointer>0x3B9AC9FFl)
		POINT_MESSAGE.ControlDealPointer=1;
	memcpy(rRecord2.rCardDealNumb, (unsigned char*)&POINT_MESSAGE.ControlDealPointer, 4);
	
	MSG_LOG("DeviceNo:");
	BCD_LOG(pFistVary.DeviceNo,9,1);
	MSG_LOG("LineNo:");
	BCD_LOG(pFistVary.LineNo,3,1);
	
	if(mode)
		memcpy(rRecord2.rDelaTime,pFistVary.DeviceNo,9);
	else
		memset(rRecord2.rDelaTime,0x30,9);
	memcpy(rRecord2.rPublishNumb2+2,pFistVary.LineNo,2);
	rRecord2.rReserveByte[0]=pFistVary.LineNo[2];
	for(i=0;i<4;i++)
		rRecord2.rReserveByte[1+i]=NewPriceTab.busPrice[i];
	
//	CountMAC((DEALRECODE*)&rRecord2);//MAC
	CountPoint(2);		
	WriteRecord((unsigned char*)&rRecord2, 0);
}

void BuildRecorde_32(unsigned char delType, unsigned char *recBuf)
{
	DEALRECODE_32B *rRecordDeal;
	Record_Control_32B *cntrlrRecord;
	unsigned int temp;
	
	rRecordDeal = (DEALRECODE_32B *)recBuf;
	cntrlrRecord = (Record_Control_32B *)recBuf;
	if((delType&0x7F) == ID_REC_TOLL){
		temp = a_sum1;//+s_sum1;
		memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&temp, 4);
		temp =POINT_MESSAGE.MoneyDealPointer;
		POINT_MESSAGE.MoneyDealPointer++;
		if(POINT_MESSAGE.MoneyDealPointer > 0x3B9AC9FF)
			POINT_MESSAGE.MoneyDealPointer = 0;
		memcpy(rRecordDeal->rCardDealNumb, (unsigned char*)&temp, 4);
		rRecordDeal->rCardType = cardMessage.card_catalog;

		memcpy(rRecordDeal->rDealMoney, (unsigned char*)&s_sum1, 3);
		memcpy(rRecordDeal->rDealTime, (unsigned char*)&SysTime, 7);
		rRecordDeal->rDealType = ID_REC_TOLL;
		memcpy(rRecordDeal->rMoneyDealSum, (unsigned char*)&CARD_PUBLIC.MoneyDealSum, 2);
		memcpy(rRecordDeal->rPublishNumb, cardMessage.PublishBicker, 4);
		rRecordDeal->rRecordType = ID_REC_TOLL;
	}
	else if((delType&0x7F) == ID_REC_MON){
		temp = dis_sum2;
#ifdef twicePuchase
		temp += 1;
#endif
		memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&temp, 4);
		POINT_MESSAGE.MonthDealPointer++;
		if(POINT_MESSAGE.MonthDealPointer > 0x3B9AC9FF)
			POINT_MESSAGE.MonthDealPointer = 0;
		memcpy(rRecordDeal->rCardDealNumb, (unsigned char*)&POINT_MESSAGE.MonthDealPointer, 4);
		rRecordDeal->rCardType = cardMessage.card_catalog;
		if(s_sum1 != 0)
			memcpy(rRecordDeal->rDealMoney, "\x01\x00\x00", 3);
		else
			memcpy(rRecordDeal->rDealMoney, "\x00\x00\x00", 3);
		memcpy(rRecordDeal->rDealTime, (unsigned char *)&SysTime, 7);
		rRecordDeal->rDealType = ID_REC_MON;
		memcpy(rRecordDeal->rMoneyDealSum, (unsigned char*)&CARD_PUBLIC.MonthDealSum, 2);
		memcpy(rRecordDeal->rPublishNumb, cardMessage.PublishBicker, 4);
		rRecordDeal->rRecordType = ID_REC_MON;
	}
	else{		
		memset(recBuf, 0xff, RECORD_LEN);
		POINT_MESSAGE.ControlDealPointer+=1;
		if(POINT_MESSAGE.ControlDealPointer>0x3B9AC9FFl)
			POINT_MESSAGE.ControlDealPointer=1;
		memset(recBuf,0,22);
		memcpy(cntrlrRecord->rCardDealNumb ,(unsigned char*)&POINT_MESSAGE.ControlDealPointer, 4);
		cntrlrRecord->rCardType = cardMessage.card_catalog;
		cntrlrRecord->rDealType = delType;
		memcpy(cntrlrRecord->rDelaTime, (unsigned char*)&SysTime, 7);
		memcpy(cntrlrRecord->rPublishNumb, cardMessage.PublishBicker, 4);
		memcpy(cntrlrRecord->rPublishNumb2, cardMessage.PublishBicker, 4);
		cntrlrRecord->rRecordType = delType;
	}
	card_ser = c_serial;
	CountPoint(0);
}

#endif

extern unsigned char Card_Area ;//��������   0x80:����  0x40:��ͤ 0x20:��̨  0x10:����  0x08:����  0x04:ƽ����  0x02:������  0x01:��������

//��ͨ��¼������� 11  ��ʷ��¼������ 29  ��Ҫ�� ���� PSAM���� @zzc2016��5��21��11:37:30
void BuildRecorde(unsigned char delType, unsigned char *recBuf)
{
// #ifdef BUS_HANDAN_
// 	BuildRecorde_32(delType, recBuf);
// #else
	DEALRECODE *rRecordDeal;
	unsigned int temp;

	unsigned short crc16;
	stCpuInfo cpuinfo;


	MSG_LOG("do %s\r\n",__FUNCTION__);


	getCpuInfo(&cpuinfo);
	memset(recBuf,0,RECORD_LEN);	



		CountPointRead();//����BIT_POINT_MESSAGE  zcz
	rRecordDeal = (DEALRECODE *)recBuf;
	if((delType&0x7F) == ID_REC_TOLL||delType==ID_REC_HJY_REC||(delType&0x7F) == ID_REC_MON){//
		if(gMCardCand==CARDSTYLE_NORM1)
			memcpy(rRecordDeal->rMoneyDealSum, CARD_PUBLIC.MoneyDealSum, 2);
		else
			memcpy(rRecordDeal->rMoneyDealSum,stuInitPurchaseRet.cSnq,2);

		if ((delType&0x7F) == ID_REC_MON)
		{
			
			memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&dis_sum2, 4);
		}else{
			
			memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&a_sum1, 4);
		}

		memcpy(rRecordDeal->rDealMoney, (unsigned char *)&s_sum1, 3);
	}
	
	temp =POINT_MESSAGE.MoneyDealPointer;
	if(delType!=ID_REC_HJY_REC)
	POINT_MESSAGE.MoneyDealPointer++;
	if(POINT_MESSAGE.MoneyDealPointer > 0xFFFFFF)
		POINT_MESSAGE.MoneyDealPointer = 0;
	CountPoint(0);
	Get_SerialNumF4(rRecordDeal->rDeviceNo);
	rRecordDeal->rRemain = Card_Area; //add hb�����ж�
	rRecordDeal->rPublishNumb_a = cardMessage.PublishBicker[4];
	memcpy(rRecordDeal->rPublishNumb, cardMessage.PublishBicker, 4);
	memcpy(rRecordDeal->rCardDealNumb, stuDebitPurchase.cTermSnq, 4);
	rRecordDeal->rCardType = cardMessage.card_catalog;
	memcpy(rRecordDeal->rDealTime, (unsigned char*)&SysTime, 7);
	rRecordDeal->rDealType = delType;

	getCpuDeviceNO(pFistVary.DeviceNo);	//����ȡ�豸��
	memcpy(rRecordDeal->PbocTac, cardMessage.PbocTac, 4);
	MSG_LOG("TAC:");
	BCD_LOG(rRecordDeal->PbocTac,4,1);


	memcpy(rRecordDeal->rDriverNo, DriveCardNo, 4);
	getCpuLineNo();
	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 2);
	rRecordDeal->rCardCand = gMCardCand;
// 	memcpy(rRecordDeal->rBussinessNo, "\x99\x98", 2);//�̻�����Կ��ڱ��Ϊ׼������ȡˢ�����ڱ��
//	getbussinessNO(rRecordDeal->rBussinessNo);

	SYSgetbussinessNO(rRecordDeal->rBussinessNo);


	if (gMCardCand == CARDSTYLE_JTB)
	{
		memcpy(rRecordDeal->rDealTime, stuSamInitPurchaseJTB.cDateTime, 7);
		rRecordDeal->rCardType = cardMessage.card_catalog;
		memcpy(rRecordDeal->rPsamNum,PsamNumJTB,6);
	}else{

		memcpy(rRecordDeal->rDealTime,stuDebitPurchase.cDateTime, 7);
		rRecordDeal->rCardType = cardMessage.card_catalog_bak;
		rRecordDeal->rConpernyNum[0] = cardMessage.Zicard_catalog;
		rRecordDeal->rConpernyNum[1] = cardMessage.card_catalog;
		memcpy(rRecordDeal->rPsamNum,PsamNo,6);
	}

	memcpy(rRecordDeal->rUnionCardNum,cardMessage.PublishBicker,10);
	//���������
	memcpy(rRecordDeal->rFactoryNum, cardMessage.FactorFlg, 4);	//��������
	memcpy(rRecordDeal->rCityCode, cardMessage.CityCode, 2);//���д���
	 rRecordDeal->rReseverX=9;//
	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);//����汾
	over_turn(2,rRecordDeal->rProVer);
//	memcpy(rRecordDeal->rDeviceNum,GxBusInfo.DeviceNum,4);//�ն˺�
	Ascii2BCD(pFistVary.DeviceNo,rRecordDeal->rDeviceNum,8);
	//***************************************************
	memcpy(rRecordDeal->rFactoryNum2,Factory_NUM,4);
	memcpy(rRecordDeal->rUnionCardNum,cardMessage.PublishBicker,10);//����
	memcpy(rRecordDeal->rHistorySearl,jtb18file.rEPsearl,2);
	memcpy(rRecordDeal->rHistoryMoney,jtb18file.rPursPrice+1,3);
	rRecordDeal->rHistoryDealType=jtb18file.rPursType;
	memcpy(rRecordDeal->rHistoryDevice,jtb18file.rPSAMnum,6);
	memcpy(rRecordDeal->rHistoryDealTime,jtb18file.rPursTime,7);
//	memcpy(rRecordDeal->rConpernyNum,GxBusInfo.CompernyNum,4);//��˾����
	memcpy(rRecordDeal->rLoadType,jtb15file.cFCI,2);
//	memcpy(rRecordDeal->rLineBefore,GxBusInfo.BusLine,2);
//	memset(rRecordDeal->rResetxxx,0xFF,9);
// 	memcpy(rRecordDeal->rDrivePublicnum,GxBusInfo.DriverPublicNum,8);
	memcpy(rRecordDeal->rDiviceSeal,(unsigned char*)&temp,4);

	crc16 = Getcrc16(rRecordDeal->rPublishNumb, RECORD_LEN-2, 0xFF, 0xFF);
	memcpy(rRecordDeal->Crc16,(unsigned char*)&crc16,2);
	if(((delType&0x7F) == ID_REC_TOLL)||((delType&0x7F) == ID_REC_MON)){
		addStatMoney((delType&0x7F), s_sum1);
		//addSCtimes(1);//�ܴ���
		//addSCtimesDay(1);
	}
	
	MSG_LOG("BLD REC:");
	BCD_LOG(recBuf, RECORD_LEN, 1);
	
}


//�л������������շѻ�ر��շ� 1 �� 2�ر�
void SwitchBusClose(unsigned char ucClose)
{
	unsigned char buffer[LEN_F_FLAG_VARY];
	restore_flag = 1;
	F_FLAG_VARY.stop_flag = ucClose;
	memcpy(buffer,(unsigned char*)&F_FLAG_VARY,LEN_F_FLAG_VARY);
	sysfewrite(BIT_F_FLAG_VARY,LEN_F_FLAG_VARY,buffer);
#ifdef _Counter_SWcard_times	//ˢ������������
	addSCtimes(0xFF);
#endif	
	return;
}

unsigned char IsValidPublic(unsigned char * buf)
{
	if(buf[0]>0x39) return 0;
	
	if((buf[3]!=0x01)&&(buf[3]!=0x02)) return 0;

	if (buf[7]!=0x10) return 0;
	
	return 1;
}

unsigned char GethisMessage(void)
{
	unsigned char i=0;
	unsigned char buffer[20];
	unsigned char s_no;
	//clr_dog();

	MSG_LOG("public MM:");
	BCD_LOG((unsigned char*)&CARD_PUBLIC, 16, 1);
	BCD_LOG((unsigned char*)&CARD_PUBLIC.DealPo, 1, 1);

#ifdef FengDuan_BU_
	if((CARD_PUBLIC.DealPo>6) || (CARD_PUBLIC.DealPo<1))
#else
	if((CARD_PUBLIC.DealPo>9) || (CARD_PUBLIC.DealPo<1))
#endif
	{
		memset((unsigned char*)&gHisMassage, 0, 16);
		return ST_OK;
	}
	if(CARD_PUBLIC.DealPo == 1)
#ifdef FengDuan_BU_
		i = 6;
#else
		i = 9;
#endif
	else
		i = CARD_PUBLIC.DealPo - 1;
	
	s_no=((unsigned char)(i-1)/3)+3;

	MSG_LOG("s_no1:");
	BCD_LOG((unsigned char*)&s_no,1,1);

	if(MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,s_no*4)==0)
	{
		MSG_LOG("OperPublicMessage MifareAuthKey error\r\n ");
		return ST_ERROR; 
	}
	s_no=((((unsigned char)(i-1)/3)+3)*4)+(i-1)%3;

	MSG_LOG("s_no2:");
	BCD_LOG((unsigned char*)&s_no,1,1);

	if(MifareRead(s_no,buffer)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.publicMessageSector*4);
#endif
		return ST_ERROR;
	}
	memcpy((unsigned char*)&gHisMassage,buffer,16);

	MSG_LOG("gHismassage:");
	BCD_LOG((unsigned char*)&gHisMassage,16,16);

	return ST_OK;
}

//�޸�����Ĺ�����Ϣ��
unsigned char RepairPublicSector(void)
{
	unsigned char i=0;
	unsigned char buff[60];
	//clr_dog();
	
	MSG_LOG("RepairPublicSector 1\r\n");

	if(cardMessage.publicMessageSector >= 16){//û�й�����Ϣ��
		return ST_OK;
	}
	CARD_PUBLIC.DelaFlag=0x02;
	CARD_PUBLIC.DealPo = 1;
	CARD_PUBLIC.BlackFlag = 0;
	CARD_PUBLIC.FileFlag = 0;

	memset((unsigned char *)buff,0,16);
	memcpy((unsigned char *)buff,(unsigned char *)&CARD_PUBLIC,8);
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
	{
		return ST_ERROR;
	}
	
	buff[15]=MiBlockInvalid(0,15,buff);
	
	MSG_LOG("RepairPublicSector	3\r\n");
	BCD_LOG(buff,16,1);

	for(i=0;i<3;i++){
		MifareWrite(cardMessage.publicMessageSector*4, buff);
		memset(buff+30, 0, 16);
		MifareRead(cardMessage.publicMessageSector*4, buff+30);
		if(memcmp(buff, buff+30, 16) == 0)
			break;
	}
	if(i>=3)
		return ST_ERROR;
	return ST_OK;

}
unsigned char OperPublicMessage(void)
{
	unsigned char i=0;
	unsigned char buff[60];
#ifdef MOBILE_XIANYANG_
	unsigned short kk;
#endif
	//clr_dog();
	
	MSG_LOG("Oper PublicMessage 1\r\n");

	if(cardMessage.publicMessageSector >= 16){//û�й�����Ϣ��
		memset((unsigned char*)&CARD_PUBLIC1, 0, sizeof(CARD_PUBLIC1));
		CARD_PUBLIC1.DealPo = 1;
		CARD_PUBLIC1.DelaFlag = 2;
		return ST_OK;
	}

	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
	{
		return ST_ERROR; 
	}
	
	MSG_LOG("Oper PublicMessage 2\r\n");

	
	//��������Ϣ
	if(MifareRead(cardMessage.publicMessageSector*4,buff)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.publicMessageSector*4);
#endif
		return ST_ERROR;
	}
	if((MiBlockInvalid(1,15,buff)==ST_ERROR)||(buff[0]==0)||(buff[0]>9)){
		gucRestorFlag |= RES_PUBLIC_Z;
	}
	
	MSG_LOG("sector 6 1:");
	BCD_LOG(buff, 16 ,1);

	if(MifareRead(cardMessage.publicMessageSector*4+1,buff+30)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.publicMessageSector*4+1);
#endif
		return ST_ERROR;
	}

	MSG_LOG("sector 6 1:");
	BCD_LOG(buff+30, 16 ,1);

	if(MiBlockInvalid(1,15,buff+30)==ST_ERROR){
		gucRestorFlag |= RES_PUBLIC_F;
	}
	if((gucRestorFlag & RES_PUBLIC_Z) && (gucRestorFlag & RES_PUBLIC_F)){
		return CARD_PUBLIC_BAD;
	}
	else if((gucRestorFlag & RES_PUBLIC_Z) == 0)
	{
		if(buff[3] == 0x01)
			gucRestorFlag |= RES_PUBLIC_FLAG;
	}
	for(i=0;i<8;i++){
		if((buff[i]^buff[30+i]) != 0xff){
			gucRestorFlag |= RES_PUBLIC_D;
			break;
		}
	}

	MSG_LOG("sector 6:");
	BCD_LOG(buff, 16 ,1);
	BCD_LOG(buff+30, 16 ,1);

	memcpy((unsigned char *)&CARD_PUBLIC, buff, CARD_PULICH_MESSAGE_LEN);
	memcpy((unsigned char *)&CARD_PUBLIC1, buff+30, CARD_PULICH_MESSAGE_LEN);
	if((CARD_PUBLIC.BlackFlag == 0x04)||(CARD_PUBLIC1.BlackFlag == 0xfb))
		return CARD_BLACK;
	if((CARD_PUBLIC.BlackFlag == 0x05)||(CARD_PUBLIC.DelaFlag == 0x05))
		return CARD_CHAGEERR;
// 	if(buff[10] == 0){
// 		if(buff[30] != 0xff){
// 			buff[10] = ~buff[30];
// 			buff[11] = ~buff[31];
// 			buff[12] = ~buff[32];
// 		}
// 	}
	if(gucRestorFlag & RES_PUBLIC_Z){
//		memcpy(buff, (unsigned char*)&CARD_PUBLIC1, 8);
		memcpy(buff, buff+30, 16);
		for(i=0;i<16;i++){
			buff[i] = ~buff[i];//����
		}
		memcpy((unsigned char*)&CARD_PUBLIC, buff, 8);
	}
#ifdef MOBILE_XIANYANG_
	kk = 0;
	gUserTimes.Tyear = buff[10];
	memcpy((unsigned char*)&kk, buff+11, 2);
	if((cardSound == 0xaa)&&(kk == gUserTimes.Ttimes)){//�������ˢ�������Ҵ����Ϳ�����ȣ��Ѿ�д�룬����1
		gUserTimes.Ttimes--;
	}
	else
		gUserTimes.Ttimes = kk;

	if(gUserTimes.Ttimes <= 800)
		gUserTimes.Ttimes++;//ʹ�ô����ȼ�1

	MSG_LOG("GuserTimes:");
	BCD_LOG((unsigned char*)&gUserTimes.Tyear, 1 ,0);
	BCD_LOG((unsigned char*)&gUserTimes.Ttimes, 2 ,0);

	if((gUserTimes.Tyear == 0)||(gUserTimes.Ttimes > 850)){//��ͷ��ʼ
		gUserTimes.Tyear = SysTime.year_l;
		gUserTimes.Ttimes = 0;

		MSG_LOG("���¿�ʼ:");
		BCD_LOG((unsigned char*)&gUserTimes.Tyear, 1 ,0);
		BCD_LOG((unsigned char*)&gUserTimes.Ttimes, 2 ,0);

	}
#endif
	return ST_OK;

// 	i = GethisMessage();
// 	return i;
}

int PurchasOk_Money(unsigned char mode)
{
	unsigned char i=0,s_no=0;
	unsigned char buffer_1[20],buffer2[20];
	unsigned short stemp=0;
	
	if(mode == 0){
		if(cardMessage.publicMessageSector >= 16){//û�й�����Ϣ��
			return ST_OK;
		}
		CARD_PUBLIC.DelaFlag=0x01;
		memset((unsigned char *)buffer2,0,16);
		memcpy((unsigned char *)buffer2,(unsigned char *)&CARD_PUBLIC,8);
		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
		{
			return ST_ERROR;
		}
		
		buffer2[15]=MiBlockInvalid(0,15,(unsigned char *)buffer2 );
		memcpy(buffer_1, buffer2, 16);
		
		//	OSTimeDlyHMSM(0, 0, 0, 30);///////////////////////
		
		//	MifWrite(cardMessage.publicMessageSector*4, (unsigned char *)buffer_1);

		MSG_LOG("PurchasOk	3\r\n");
		BCD_LOG(buffer2,16,1);

		if(MifareWrite(cardMessage.publicMessageSector*4, buffer_1)==0)//��д������Ϣ����Ϊ 1
		{
			return ST_ERROR;
		}
		
	}
	else if(cardMessage.publicMessageSector < 16){//��д����ʱд�����׼�¼	
		MSG_LOG("PurchasOk	4\r\n");
		BCD_LOG((unsigned char*)&cardMessage.publicMessageSector, 1, 1);
		BCD_LOG(cardMessage.PublichKEYA, 6, 1);

		CARD_PUBLIC.DealPo+=1;
		
#ifdef FengDuan_BU_
		if(CARD_PUBLIC.DealPo>6)
			CARD_PUBLIC.DealPo=1;
#else
		if(CARD_PUBLIC.DealPo>9)
			CARD_PUBLIC.DealPo=1;
#endif
//		if(mode==1)											
//		{
			memcpy((unsigned char*)&stemp, CARD_PUBLIC.MoneyDealSum, 2);
			stemp+=1;
//		}
// 		if(mode==0)
// 		{
// 			memcpy((unsigned char*)&stemp, CARD_PUBLIC.MonthDealSum, 2);
// 			stemp+=1;
// 		}
		memcpy(CARD_PUBLIC.MoneyDealSum, (unsigned char*)&stemp, 2);

		CARD_PUBLIC.DelaFlag=0x02;
		memset((unsigned char *)buffer2,0,16);
		memcpy((unsigned char *)buffer2,(unsigned char *)&CARD_PUBLIC,8);

#ifdef MOBILE_XIANYANG_
		buffer2[10] = SysTime.year_l;
		memcpy(buffer2+11, (unsigned char*)&gUserTimes.Ttimes, 2);//����ʹ�ô���,��ȡ��ʱ�Ѿ���1

		MSG_LOG("gUserTimes:");
		BCD_LOG((unsigned char*)&gUserTimes.Ttimes,2,1);
#endif

		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
		{
			return ST_ERROR;
		}
		
		buffer2[15]=MiBlockInvalid(0,15,(unsigned char *)buffer2 );
		memcpy(buffer_1, buffer2, 16);
		
		MSG_LOG("PurchasOk	7\r\n");
		BCD_LOG(buffer_1,16,1);

	//	MifWrite(cardMessage.publicMessageSector*4, (unsigned char *)buffer_1);
		s_no = (cardMessage.publicMessageSector*4);
		for(i=0;i<3;i++){
			MifareWrite(s_no, buffer_1);
			memset(buffer2, 0xee, 16);
			MifareRead(s_no, buffer2);
			if(memcmp(buffer2, buffer_1, 16) == 0){
				break;
			}
		}
		if(i>=3)
			return ST_ERROR;

		for(i=0;i<16;i++)
			buffer_1[i]=~buffer_1[i];
	

		MSG_LOG("PurchasOk	8\r\n");
		BCD_LOG(buffer_1,16,1);

//		MifWrite(25, (unsigned char *)buffer_1);
		s_no = (cardMessage.publicMessageSector*4)+1;
		for(i=0;i<3;i++){
			MifareWrite(s_no, buffer_1);
			memset(buffer2, 0xee, 16);
			MifareRead(s_no, buffer2);
			if(memcmp(buffer2, buffer_1, 16) == 0){
				break;
			}
		}
		if(i>=3)
			return ST_ERROR;

		MSG_LOG("PurchasOk	9\r\n");
	}

	if(cardMessage.dealRecordSector[0] >= 16)//û�м�¼��
		return ST_OK;

	MSG_LOG("PurchasOk dealRecordSector!\r\n");

#ifdef FengDuan_BU_	//�ֶ��շ��к�һ����¼����ʹ����
	if((CARD_PUBLIC.DealPo==0) || (CARD_PUBLIC.DealPo > 6))
#else
	if((CARD_PUBLIC.DealPo==0) || (CARD_PUBLIC.DealPo > 9))
#endif
		CARD_PUBLIC.DealPo = 1;
	s_no=((unsigned char)(CARD_PUBLIC.DealPo-1)/3)+cardMessage.dealRecordSector[0];
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,s_no*4)==0)
	{
		return ST_ERROR;
	}

	MSG_LOG("====%d===\r\n", CARD_PUBLIC.DealPo);

	s_no=((((unsigned char)(CARD_PUBLIC.DealPo-1)/3)+cardMessage.dealRecordSector[0])*4)+(CARD_PUBLIC.DealPo-1)%3;

	MSG_LOG("PurchasOk	5\r\n");

	MifareWrite(s_no,buffer_1);

	MSG_LOG("PurchasOk END\r\n");

	return ST_OK;
}

//д�����׼�¼
//�޸Ĺ���������־
unsigned char PurchasOk(unsigned char mode)
{   
	cardRecord1 card_record;
	unsigned char snPo=0,s_no=0;
	unsigned char buffer_1[50];
	unsigned char buffer_2[20];
	unsigned char i,ii;
	unsigned char sector;
	unsigned int itemp;
//	unsigned char err_po;
//	unsigned char err_po1;
//	unsigned char err=ST_OK;
//	unsigned char err1=ST_OK;
	unsigned short stemp=0;

	for(i=0;i<error_pointer;i++)
	{
		if(mode==1){
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONTH) continue;
		}
		else if(mode==0){
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONEY) continue;
		}
		if(memcmp(ErrorDetail[i].Cardno,(unsigned char *)&c_serial,4)==0)
		{
			switch(ErrorDetail[i].ErrorStep)
			{
			case OPER_RECORD_PROFILE:		//д���м�¼����	
//				err_po=i;
//				errerr=ST_ERROR;          //˵���д�����
				break;				
			case OPER_PUBLIC_MESSAGE:		
//				err_po1=i;
//				err1=ST_ERROR;
				goto continue_pulic_message;
			default:
				break;
			}
		}
	}


	MSG_LOG("PurchasOk --1\r\n");
	
#ifdef FengDuan_BU_
	if((CARD_PUBLIC.DealPo==0) || (CARD_PUBLIC.DealPo > 6))
#else
	if((CARD_PUBLIC.DealPo==0) || (CARD_PUBLIC.DealPo > 9))
#endif
		CARD_PUBLIC.DealPo = 1;
	snPo=CARD_PUBLIC.DealPo;

	BCD_LOG((unsigned char *)&snPo,1,1);

	itemp = a_sum1+s_sum1;
	memcpy(card_record.Balance, (unsigned char*)&itemp, 4);
	memcpy(card_record.DealMoney, (unsigned char*)&s_sum1, 3);
	memcpy(card_record.DealTime, &SysTime.day, 4);
	card_record.DealType = mode;
#ifdef BUS_PRO_JTB
	memcpy(card_record.DeviceNo,PsamNumJTB+2,4);//ȡ���4���ֽ�
#else
	Ascii2BCD(pFistVary.DeviceNo, card_record.DeviceNo, 8);
#endif


	if(cardMessage.dealRecordSector[0] < 16){
		sector = ((snPo-1)/3) +cardMessage.dealRecordSector[0];
//		BCD_LOG(1,1,(unsigned char *)&s_no);	
		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,sector*4)==0)
		{
			CheckSno(mode,OPER_RECORD_PROFILE,(unsigned char*)&card_record);
//			MSG_LOG("MifareAuthKey error\r\n ");
			return ST_ERROR; 
		}	

//		MSG_LOG("PurchasOk --2\r\n");
		s_no=((((snPo-1)/3)+3)*4)+(snPo-1)%3;
		memset(buffer_1,0,16);
		memcpy(buffer_1, (unsigned char*)&card_record, LEN_CARD_RECORD);
//		MSG_LOG("PurchasOk --3\r\n");

		MSG_LOG("д����¼:\r\n");
		BCD_LOG(buffer_1,16,1);

		if(MifareWrite(s_no,buffer_1)==0)
		{
			CheckSno(mode,OPER_RECORD_PROFILE,(unsigned char*)&card_record);
//		MSG_LOG("д����¼ERROR\r\n");
			return ST_ERROR;
		}
	}
	///////////////////////////////////////////////////////////////////////////////
//�޸Ĺ�������
continue_pulic_message:
	if(cardMessage.publicMessageSector >= 16){//û�й�����Ϣ��
		return ST_OK;
	}

	CARD_PUBLIC.DelaFlag=0x02;

	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
	{
		MSG_LOG("MifareAuthKey error\r\n ");
		CheckSno(mode,OPER_PUBLIC_MESSAGE,(unsigned char*)&CARD_PUBLIC);
		return ST_ERROR; 
	}

	MSG_LOG("PurchasOk --6\r\n");

	
	CARD_PUBLIC.DealPo+=1;
#ifdef FengDuan_BU_
	if(CARD_PUBLIC.DealPo>6)
#else
	if(CARD_PUBLIC.DealPo>9)
#endif
		CARD_PUBLIC.DealPo=1;
// 	if(mode==1)											
// 	{
// 		memcpy((unsigned char*)&stemp, CARD_PUBLIC.MonthDealSum, 2);
// 		stemp+=1;
// 	}
// 	if(mode==0)
// 	{
		memcpy((unsigned char*)&stemp, CARD_PUBLIC.MoneyDealSum, 2);
		stemp+=1;
// 	}
	memcpy(CARD_PUBLIC.MoneyDealSum, (unsigned char*)&stemp, 2);

	memset(buffer_1, 0, 16);
	memcpy(buffer_1, (unsigned char*)&CARD_PUBLIC, 8);
// 	buffer_1[Public_dealpo] = CARD_PUBLIC.DealPo;
// 	buffer_1[Public_blackflag] = CARD_PUBLIC.BlackFlag;
// 	buffer_1[Public_delaflag] = CARD_PUBLIC.DelaFlag;
// 	buffer_1[Public_fileflag] = CARD_PUBLIC.FileFlag;
// 	memcpy(buffer_1+Public_moneydealsum, (unsigned char*)&CARD_PUBLIC.MoneyDealSum, 2);
// 	memcpy(buffer_1+Public_monthdealsum, (unsigned char*)&CARD_PUBLIC.MonthDealSum, 2);

	buffer_1[15]=MiBlockInvalid(0,15,buffer_1);
	if(MifareWrite(cardMessage.publicMessageSector*4,buffer_1)==0)
	{
		CheckSno(mode,OPER_PUBLIC_MESSAGE,(unsigned char*)&CARD_PUBLIC);
		return ST_ERROR;
	}
	for(i=0;i<16;i++)
	{
		buffer_2[i]=~buffer_1[i];
	}

	if(MifareWrite(cardMessage.publicMessageSector*4+1,buffer_2)==0)
	{
		return ST_ERROR;
	}

	MSG_LOG("Purchase OK End!\r\n");

	for(i=0;i<error_pointer;i++)
	{
		if(mode==1){
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONTH) continue;
		}
		else if(mode==0){
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONEY) continue;
		}
		if(memcmp(ErrorDetail[i].Cardno,(unsigned char *)&c_serial,4)==0)
		{
			switch(ErrorDetail[i].ErrorStep)
			{
			case OPER_PUBLIC_MESSAGE_W:
			case OPER_RECORD_PROFILE:		
			case OPER_PUBLIC_MESSAGE:
				memcpy((unsigned char*)&a_sum1,ErrorDetail[i].AferToll,4);
				memcpy((unsigned char*)&s_sum1,ErrorDetail[i].TollValue,4);
				break;
			}
			for(ii=i;ii<error_pointer;ii++)
				memcpy((unsigned char *)&ErrorDetail[ii],(unsigned char *)&ErrorDetail[ii+1],ERROR_DETAIL_LEN);
			error_pointer-=1;
			memset((unsigned char *)&ErrorDetail[error_pointer],0,ERROR_DETAIL_LEN);
			break;
		}
	}	
	for(i=0;i<1;i++)  //ERROR_DETAIL_SUM
	{
		memset(buffer_1,0,32);
		memcpy(buffer_1,(unsigned char *)&ErrorDetail[i],ERROR_DETAIL_LEN);
		sysfewrite(BIT_ERROR_DETAIL+i*ERROR_DETAIL_LEN,ERROR_DETAIL_LEN,buffer_1);
	}
	
	memset(buffer_1,0,2);
	buffer_1[0]=error_pointer;
	sysfewrite(BIT_ERROR_POINT,1,buffer_1);
	return ST_OK;
	
}
void error(unsigned char code1,unsigned char Index)
{
  	unsigned char pos=0;
	char disp[40];

	Index = Index;
	pos = pos;

	memset(disp, 0, 32);
	switch(code1)
	{
	case ERR_NO_PSAM:
		cls();
		if(gchn_eng == 'E')
			display(0,0,"hint:",1);
		else
			display(0,0,"��ʾ:",1);
		
		if(gchn_eng == 'E')
			memcpy(disp,(unsigned char *)"PSAM no exist!",13);
		else
			memcpy(disp,(unsigned char *)"PSAM��������!",13);
		SoundMessage(code1);
		//DisplayString(1,2,disp,0,0);
		display(2,2,disp,0);
		restore_flag=1;
		for(;;);
	case ERR_INIT_PSAM:
		SoundMessage(code1);
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"PSAM init error!",0);
		}
		else{
			display(0,0,"��ʾ:",1);
			display(2,2,"��ʼ��PSAM������!",0);
		}
		for(;;);
	  //	break;
	case ERR_READ_TIME:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"sys time error!", 0);
		}
		else{
			display(0,0,"��ʾ:",1);
			display(2,0,"ϵͳʱ�䲻��ȷ!",0);
		}
		restore_flag=1;
		break;
	case ERR_CARD_STOP_WORK:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"card was halted!", 0);
		}
		else{
			display(0,0,"��ʾ:",1);
			memcpy(disp,"�ÿ��Ѿ���ͣ��!",15);
		}
		//DisplayString(1,2,disp,0,0);
		display(2,2,disp,0);
		restore_flag=1;
		break;
	case ERR_CARD_NO_SYSTEM:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"no system card!", 0);
		}
		else{
			display(0,0,"��ʾ:",1);
			memcpy(disp,"�ÿ����Ǳ�ϵͳ�����Ŀ�!",23);
		}
		//DisplayString(1,2,disp,0,0);
		display(2,2,disp,0);
		restore_flag=1;
		break;
	case ERR_CARD_WHITE_BUS:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"Prompt:",1);
			strcpy(disp,"Card type is error!");
		}
		else{
			display(0,0,"��ʾ:",1);
			strcpy(disp,"�ÿ�����50����δ���ۿ�!");
		}
		display(2,2,disp,0);
		restore_flag=1;
		break;
	case ERR_CARD_NO_USE:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"card was halted!", 0);
		}
		else{
			display(2,0,"�ÿ�û������,",0);
			display(4,0,"���뷢������ϵ!",0);
		}
		restore_flag=1;
		break;
	case ERR_MONEY_SN:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"Ticket amount error!", 0);
		}
		else{
			display(0,0,"��ʾ:",1);
			display(2,1,"Ʊ�����Ƿ�,",0);
			display(4,1,"���ܽ�������!",0);
		}
		restore_flag=1;
		break;
	case ERR_CHARGE_MEONY:
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"ticket balance is lack!", 0);
		}
		else{
			display(0,0,"��ʾ:",1);
			memcpy(disp,"���㣡   ",13);
		}
		//DisplayString(1,2,disp,0,0);
		display(2,2,disp,0);
		restore_flag=1;
		break;
	case ERR_CARD_PUBLIC:
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"card error!", 0);
		}
		else{
			display(2,1,"��������������,", 0);
			display(4,0,"�뵽��ֵ�㴦�ָ�",0);
		}
		restore_flag=1;
		break;
	case ERR_MONTH_BAD:
		if(gchn_eng == 'E'){
			display(0,0,"hint:",1);
			display(2,0,"month ticket error!", 0);
		}
		else{
			display(2,2,"��Ʊ����Ǯ����ʽ����!",0);
		}
		restore_flag=1;
		break;
	default:
		return;
	}
	restore_flag=1;
	end_card();
}

#ifdef _Counter_SWcard_times
//�������ж���ˢ���Ĵ���,���ش��� mode=0����ˢ������ mode=1����������
int getSCtimes(unsigned char mode)
{
	unsigned char gsctBuf[16];
	unsigned int itemp;

	if(mode == 0)
		sysferead(BIT_SWAPCARD_TIMES, 8, gsctBuf);
	else
		sysferead(BIT_SWTIMES_MAX, 8, gsctBuf);
	itemp = __cpuCrc32(gsctBuf, 4);//ǰ4�ֽڵ�CRCУ����
	if(memcmp((unsigned char*)&itemp, gsctBuf+4, 4) == 0){//��Ч
		memcpy((unsigned char*)&itemp, gsctBuf, 4);
	}
	else{
		if(mode == 1)
			itemp = 52;//Ĭ��ֵ
		else
			itemp = 0;
	}
	return itemp;
}
//�����Ӵ� atimes=0xFF,����������
void addSCtimes(unsigned char atimes)
{
	unsigned char adsctBuf[16];
	unsigned int i;
	
	if(atimes == 0xFF){//���
		memset(adsctBuf, 0, 4);
	}
	else{
		i = getSCtimes(0);
		i += atimes;
		memcpy(adsctBuf, (unsigned char*)&i, 4);
	}
	i = __cpuCrc32(adsctBuf, 4);//ǰ4�ֽڵ�CRCУ����
	memcpy(adsctBuf+4, (unsigned char*)&i, 4);
	sysfewrite(BIT_SWAPCARD_TIMES, 8, adsctBuf);
}
//�ж�ˢ�������Ƿ񵽴����ޡ����������е�����
unsigned char checkSCtimes(void)
{
	unsigned int iMax;
	unsigned int i;

	iMax = getSCtimes(1);//����������
	i = getSCtimes(0);	//�����Ѿ�ˢ�Ĵ���

	if(i >= iMax){
		return ST_ERROR;
	}
	else{
		return ST_OK;
	}
}
//����ˢ��������
void setSCtimesMax(void)
{
	unsigned int i, iMax;
	char buffer[20];
	unsigned char ret, dispflag;

	i=0;

	cls();
	display(0,0,"�������ˢ������",0);
	
	feread(BIT_SWTIMES_MAX, 8, (unsigned char*)buffer);
	i = __cpuCrc32((unsigned char*)buffer, 4);//ǰ4�ֽڵ�CRCУ����
	if(memcmp((unsigned char*)&i, buffer+4, 4) == 0){//��Ч
		memcpy((unsigned char*)&iMax, buffer, 4);	//
	}
	else{//Ĭ��ֵ
		iMax = 52;
	}

	if(iMax >= 100)
		iMax = 99;	
	dispflag = 1;
	memset(buffer,0,10);
	sprintf(buffer, "%02d", iMax);
	i = 0;

	while(1)
	{
		delayxms(50);
		
		if(dispflag){
			dispflag = 0;
			display(3, 7, buffer, 0);
			buffer[8] = buffer[i];
			display(3, 7+i, buffer+8, 1);			
		}
		ret =getkey(1);
		switch(ret)
		{
#ifndef _New_Bu_mode_
		case UP:
			if((buffer[i]<='0')||(buffer[i]>'9'))
				buffer[i]='9';
			else if((buffer[i]<='9')&&(buffer[i]>'0'))
				buffer[i]--;
			dispflag = 1;
			break;
		case F2:  
			i++;
			if(i >= 2)
				i = 0;
			dispflag = 1;
			break;
		case F1:
			if(i>0)
				i--;
			else
				i = 1;
			dispflag = 1;
			break;
		case DOWN: 
#else
		case UP:
#endif  
			if((buffer[i]<'0')||(buffer[i]>='9'))
				buffer[i]='0';
			else if((buffer[i]<'9')&&(buffer[i]>='0'))
				buffer[i]++;
			dispflag = 1;
			break;
		case ESC:
			return;
		case ENTER:
			dispflag = 1;
#ifdef _New_Bu_mode_
			i++;
#else
			i = 2;
#endif
			if(i >= 2){
				iMax = (buffer[0]-'0');
				iMax *= 10;
				iMax += (buffer[1] - '0');
				
				memcpy(buffer, (unsigned char*)&iMax, 4);
				i = __cpuCrc32((unsigned char*)buffer, 4);//ǰ4�ֽڵ�CRCУ����
				memcpy(buffer+4, (unsigned char*)&i, 4);

				sysfewrite(BIT_SWTIMES_MAX,8, (unsigned char*)buffer);
				return;
			}
			break;
		default:
			break;
		}
	}
}
#endif

//��ʼ����������Ŀ�
unsigned char PsamInitialize_SLZR(void)
{
	unsigned char ii=0;
	unsigned char buffer[128];
	
#ifdef _TEST_YANPIAO_
	///////////////////////////ƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱ
	return;
#endif
	ii =ii;
	gPsamErrorFlag |= 1;
	
	display(0,0,"���ڼ��PSAM��2",0);
	display(2,1,"��ȴ�......",0);

	MSG_LOG("PSAM init\r\n");

	ii = SamColdReset(PSAM_YD, 9600, SAM_VOL3, buffer);
	
	MSG_LOG("PSAM art:");
	BCD_LOG(buffer, ii, 1);

	delayxms(5);
	if(Read_Binary(PSAM_YD, 0x16, 0, 6, buffer)==0) 
	{
		MSG_LOG("Read_Binary error\r\n");
		BCD_LOG(buffer, 8, 1);
		return ST_ERROR;
	}

	MSG_LOG("read Binary:");
	BCD_LOG(buffer,6,1);

	memcpy(PsamNo,buffer,6);
	
	delayxms(100);
	

	MSG_LOG("Select File ");

	if(Select_File(PSAM_YD, 0x1003)==0) //0x3f01
	{
		MSG_LOG("Select_File error\r\n");
		return ST_ERROR;
	}

	MSG_LOG("PSAM PIN:\r\n");
	memcpy(buffer, "\x12\x34\x56", 3);

	if(Verify_private_PIN(PSAM_YD, 3, buffer) == 0){
		MSG_LOG("verify PIN ERROR!!\r\n");
		return ST_ERROR;
	}

	MSG_LOG("OK2\r\n");

	gPsamErrorFlag &= 0xFE;//PSAM������
	
	return ST_OK;
}

#ifdef BUS_CPU_
extern sam_pub SamPubInf_LL;
#endif
unsigned char PsamInitialize(void)
{
	unsigned char ii=0;
//	unsigned char pos=0;		
	unsigned char buffer[128];
	
#ifdef _TEST_YANPIAO_
	///////////////////////////ƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱ
	return;
#endif
	ii =ii;
	gPsamErrorFlag |= 1;
	gPSAMin = PSAM_GJ;
	
	cls();

	display(0,0,"���ڼ��PSAM��",0);
	display(2,1,"��ȴ�......",0);

	MSG_LOG("PSAM init9999\r\n");
#ifdef _debug_CPU_
	debugstring("---PsamInitialize init:\r\n");
	MSG_LOG("PSAMJTB:%d",gPSAMin);
#endif
	

#ifdef BUS_PRO_JSB
	ii = SamColdReset(gPSAMin, 38400, SAM_VOL3, buffer);
#elif defined qPBOC_BUS 
	ii = SamColdReset(gPSAMin, 9600, SAM_VOL3, buffer);//ʹ��PBOC
#else
	ii = SamColdReset(gPSAMin, 9600, SAM_VOL3, buffer);
#endif  //#ifdef BUS_PRO_JSB

#ifdef NO_MOBILE_24G_
	if(ii == 0){
#if ((defined BUS_CPU_)&&(!defined NO_CPU_PSAM_))
		return ST_ERROR;
#else
#if defined qPBOC_BUS || defined _CPU_24G_
		gPSAMin = PSAM_GJ;
#else
		gPSAMin = PSAM_YD;
#endif

#ifdef BUS_PRO_JSB
		ii = SamColdReset(gPSAMin, 38400, SAM_VOL3, buffer);
#else
		ii = SamColdReset(gPSAMin, 9600, SAM_VOL3, buffer);
#endif
		if(ii == 0){
			return ST_ERROR;
		}
#endif //#ifdef BUS_CPU_
	}
	
	MSG_LOG("gPSAMin:%d\r\n",gPSAMin);
	MSG_LOG("PSAM art:");
	BCD_LOG(buffer, ii, 1);

	
	delayxms(5);
	
	if(Read_Binary(gPSAMin, 0x16, 0, 6, buffer)==0) 
	{
		MSG_LOG("Read_Binary error\r\n");
		BCD_LOG(buffer, 8, 1);
		return ST_ERROR;
	}
	
	MSG_LOG("read Binary:");
	BCD_LOG(buffer,6,1);

	memcpy(PsamNo,buffer,6);
#ifdef BUS_CPU_
	memcpy(G24PsamNo,buffer,6);
#endif
	
	delayxms(10);
	
	MSG_LOG("Select File ");

#ifdef BUS_PRO_JSB
	if(Select_File(gPSAMin, 0x1001)==0) //0x3f01
	{
		MSG_LOG("Select_File error\r\n");
		return ST_ERROR;
	}
	
	#ifndef BUS_MZT
		memcpy(buffer, "\xBD\xA8\xC9\xE8\xB2\xBF", 6);		//���貿psam��������ͨӦ�ñ�ʶ
		if(Select_FileF(gPSAMin, buffer, 6) == 0){
			debugstring("Select_File Union error\r\n");
			return ST_ERROR;
		}
		if(Read_Binary(gPSAMin, 0x17, 0, 25, buffer)==0)		//��0x17�ļ�25�ֽ�
			//if(Read_Binary(SAM1, 0x19, 0, 25, buffer)==0) 
		{
			debugstring("GJ_Read_Binary17 error\r\n");
			debugdata(buffer, 8, 1);
			return ST_ERROR;
		}
	#ifdef _debug_CPU_
		debugstring("GJ_read 17 Binary:");
		debugdata(buffer,25,1);	//01000000000000000100000000000000012011122220161220
		debugstring("OK\r\n");
	#endif
	#endif


#ifdef BUS_CPU_
	  memcpy((unsigned char*)&SamPubInf_LL, buffer, sizeof(sam_pub));	//��һ���ֽ�����Կ����
#endif


#else	
	if(Select_File(gPSAMin, 0x1003)==0) //0x1003 
	{
		MSG_LOG("Select_File error\r\n");
		return ST_ERROR;
	}
	MSG_LOG("PSAM PIN:\r\n");
	memcpy(buffer, "\x12\x34\x56", 3);

	if(Verify_private_PIN(gPSAMin, 3, buffer) == 0){
		MSG_LOG("verify PIN ERROR!!\r\n");
		return ST_ERROR;
	}
#endif
//	delayxms(10);
	MSG_LOG("OK\r\n");

	gPsamErrorFlag &= 0xFE;//PSAM������
	
//	PsamInitialize_SLZR();//��������ͬʱˢ���貿�͹�˾��
#endif //#ifdef NO_MOBILE_24G_
	return ST_OK;
}

unsigned char GetDatEnd(unsigned int x)
{
	unsigned char i=0;//,pos=0;
	unsigned short temp1;
	unsigned char buffer[50];
//	unsigned char buffer1[20];
	unsigned int temp2;

	int curp,headp;
	
	curp = Get_Record_point((unsigned char*)&headp, 0);
	
	temp2=0;
	temp2=curp - headp;
	if(temp2 == 0)
		goto gde1;		//����ǿհ����򲻽��д���
	for(i=0;i<9;i++)
		memcpy((unsigned char*)&hisFCB[9-i],(unsigned char*)&hisFCB[8-i],21);

	memset(buffer,0,50);

	memcpy(hisFCB[0].hisName,(unsigned char*)&SysTime,7);//����������ʱ����
	memcpy((unsigned char *)&hisFCB[0].strAddr,(unsigned char*)&headp,4);
	
	memcpy((unsigned char *)&hisFCB[0].endAddr,(unsigned char*)&curp,4);

	temp2=0;
	temp2=curp - headp;
	memcpy((unsigned char *)&hisFCB[0].fileLength,(unsigned char*)&temp2,4);//�ļ�����
	
	memcpy(hisFCB[0].fileSno,pFistVary.fileSno,2);//�ļ����к�

	for(i=0;i<10;i++)
	{
		memcpy(buffer,(unsigned char*)&hisFCB[i],21);
		sysfewrite(BIT_HISDATA_GATHER+i*21,21,buffer);
	}

	temp1 =0;
	memcpy((unsigned char*)&temp1 ,pFistVary.fileSno,2);
	temp1+=1;
	memcpy(pFistVary.fileSno,(unsigned char*)&temp1,2);//�ļ����кż�1

	sysfewrite(BIT_GET_FILESNO,2,(unsigned char*)&temp1);

	ComSumeFile.SumMoney=0;
	ComSumeFile.SumMti=0;
	ComSumeFile.SumTi=0;
	ComSumeFile.BR_curE2pointer = 0;
	sysfewrite(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	lib_clr_Record_point();
//	MSG_LOG("Get dat end!!!\r\n");
gde1:
//	MSG_LOG("Get dat end 11!!!\r\n");
	x /= 4;
// 	if(x > 1)
// 		x -= 1;
	pFistVary.BlackListNum=x;
	sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
	temp2 = pFistVary.BlackListNum;
	for(i=0;i<16;i++)
		gBlackBlockNum[i] = 0;
	for(i=0;i<16;i++){
		if(temp2 > 1000){
			gBlackBlockNum[i] = 1000;
			temp2-=1000;
		}
		else{
			gBlackBlockNum[i] = temp2;
			break;
		}
	}
	
// #ifdef BUS_HANDAN_
// 	Write_Head_Record(1);
// #endif
	
	fewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);

	MSG_LOG("get current data end!\r\n");
	MSG_LOG("FileSno (char):");
	BCD_LOG(pFistVary.fileSno,2,1);
	
	SwitchBusClose(1);
#ifndef NO_MOBILE_SERVISE
	if(gMobilREC_Start == gMobilREC_End){
		memset((unsigned char*)&gMobilStyle, 0, sizeof(stMobilStyle));//��Ҫ����ǩ��
		sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
	}
	else
		uploadSettleData();//�������
#endif
	return ST_OK;
}

#ifdef SelMsg_enable
extern void SendmsgEnd(void);
#endif
void ResetPos(void)
{
	unsigned char i;
	unsigned char buff[64];

	
	cls();
	display(0,0,Card_Resetpos,DIS_CENTER|DIS_CONVERT);
	display(2,1,Card_HitResetpos,0);
	display(6,0,Card_HitSure, 0);
	while(1){
		i = getkey(1);
		if(i == KEY_ENT)
			break;
		else if(i == KEY_ESC)
			return;
		delayxms(50);
	}
#ifndef NO_MOBILE_24G_
	clr_mobile_record();
#endif
	memset((unsigned char*)&ComSumeFile,0,COMSUM_MESSAGE_FILE_LEN);//���������¼
	sysfewrite(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	clr_Record_point();

	memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
	sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
	
#ifndef NO_GJ_SERVISE
	delallBlacInbus();
#endif 
	
#ifndef NO_GJ_SERVISE
	ClrGPRSFE();
#endif
	memset(buff,0,50);
	sysfewrite(BIT_INTERVAL_STAR,1,buff);
	sysfewrite(BIT_INTERVAL_END,1,buff);
	memset(buff,0xFF,50);
	for(i=0;i<7;i++)
		sysfewrite(BIT_INTERVAL_DATA+(i*LAST_LIST_LEN),LAST_LIST_LEN,buff);

	memset(gBusVerInfo.busBLKVer, 0, 2);//���������
#ifdef _GPS_ENABLE_
	GPS_PAR_CLR();//���GPS��Ϣ��������վ��
#endif
#ifdef SelMsg_enable
	for(i=0; i<32; i++){
		memset(buff, 0xff, 32);
		sysfewrite(BIT_GPSDiaoDuCMD_INFO+(i*32), 32, buff);
	}
	SendmsgEnd();
#endif
#ifdef FengDuan_BU_
	flasherase(ERASE32KBYTESECTOR, FLASH_FD_Ticket_START);
#endif




	
	ClientPort = GJ_PORT;
	ClientIP = GJ_IP;
	
	sysfewrite(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
	sysfewrite(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
	saveCpuInfo(7, CLOUD_BUSSINESSNO);//�̻���
	savesysbupara();
	memset(NewPriceTab.lineVer,0,2);
	saveCpuInfo(1, buff);//
	
#ifdef BUS_MZT
	memset(buff,0,sizeof(buff));
	flasherase(ERASE4KBYTESECTOR,FLASH_QRCTAC_START);
	flasherase(ERASE4KBYTESECTOR,FLASH_MOBILE_START);
	sysfewrite(BIT_TACVER,2,buff);
	sysfewrite(BIT_WHTVER,2,buff);
	sysfewrite(BIT_QR_BLK_VER,2,buff);
	sysfewrite(BIT_SPKVER,2,buff);

#endif
	format_disk_RW();	//��ʽ���ļ�ϵͳ

	if(gchn_eng == 'E')
		display(3,0,"complete!",DIS_CENTER|DIS_Cls);
	else
		display(3,0,"�������,����!",DIS_CENTER|DIS_Cls);
	restore_flag = 3;
	delayxs(1);
	ReStartCPU();
}

void get_BR_Point(unsigned int *Hadd, unsigned int *Cadd);
//�˺���ÿ�ο�������һ�ξ����ˡ���getPOSPara�����е���
void saveHisaddr(void)
{
	unsigned int index, headaddr, curadd;
	stBackRecPointer flnme;
	unsigned char flag=0;

	get_BR_Point(&headaddr, &curadd);
	GetDateTime();
	index = BCD2int((unsigned char*)&SysTime.day, 1);//�������ڵ�ֵ

	MSG_LOG("save his index:");
	BCD_LOG((unsigned char*)&SysTime, 7, 1);
	BCD_LOG((unsigned char*)&SysTime.day, 1, 1);
	BCD_LOG((unsigned char*)&index, 4, 1);

	if(getsysfile(index, (unsigned char*)&flnme) != 0){		//�����м�¼���������ǲ��ǽ����
		if(memcmp(flnme.dateTIme, (unsigned char*)&SysTime, 4) == 0){//�����Ѿ����ˣ����ٸ���
			flag = 1;
		}
	}
	if(flag == 0){
		savesysfile(index, headaddr, headaddr);
	}
}

void debugHisAddr(void)
{
	unsigned int i;
	char disbuf[32][32];
	stBackRecPointer flnme;

	for(i=0; i<32; i++){
		if(getsysfile(i, (unsigned char*)&flnme) != 0){
			sprintf(disbuf[i], "%02d:H=%08X E=%08X", i, flnme.startAddr, flnme.endAddr);
		}
		else{
			sprintf(disbuf[i], "%02d:No His Files!", i);
		}
	}
	for(i=0; i<32; i++){
		MSG_LOG(disbuf[i]);
		MSG_LOG("\r\n");
	}
}

//---------------------------------------------------
//���������еı��ݱ�,����writeBackRec ������ʹ��,��PROLIB���С�
void save_bit_bak_addr(unsigned int haddr, unsigned int eaddr)
{
	unsigned char wbuf[16];
	unsigned char rbuf[16];
	unsigned int i;

	if(haddr >= FLASH2_BAKR_LEN)
		haddr = 0;
	memcpy(wbuf, (unsigned char*)&haddr, 4);
	memcpy(wbuf+4, (unsigned char*)&eaddr, 4);
	i = __cpuCrc32(wbuf, 8);
	memcpy(wbuf+8, (unsigned char*)&i, 4);
	for(i=0; i<3; i++){
		MSG_LOG("save_bit_bak_addr!\r\n");
		BCD_LOG(wbuf, 12, 1);

		fewrite(BIT_BAK_ADDR, 12, wbuf);
		memset(rbuf, 0, 12);
		feread(BIT_BAK_ADDR, 12, rbuf);
		if(memcmp(wbuf, rbuf, 12) == 0)
			break;
	}
}

unsigned char get_bit_bak_addr(unsigned int *haddr, unsigned int *eaddr)
{
	unsigned char rbuf[16];
	unsigned int i, t, j;
	for(j=0; j<3; j++){
		feread(BIT_BAK_ADDR, 12, rbuf);
		i = __cpuCrc32(rbuf, 8);
		memcpy((unsigned char*)&t, rbuf+8, 4);
		if(i == t){
			MSG_LOG("get bit bakaddr OK!\r\n");
			BCD_LOG(rbuf, 12, 1);
			memcpy((unsigned char*)&i, rbuf, 4);
			*haddr = i;
			memcpy((unsigned char*)&i, rbuf+4, 4);
			*eaddr = i;
			return ST_OK;
		}
	}
	MSG_LOG("get bit bakaddr ERROR!\r\n");
	BCD_LOG(rbuf, 12, 1);
	return ST_ERROR;
}

//���������ĵ�ַ
void get_BR_Point(unsigned int *Hadd, unsigned int *Cadd)
{
	stBackRecPointer filn;

	if(get_bit_bak_addr(Hadd, Cadd) == ST_OK){//�������ж�ȡ��ȷ
		return;
	}

	if(getsysfile(0, (unsigned char*)&filn) == 0){//û�ҵ�
		filn.startAddr = 0;
		filn.endAddr = 0;
	}

	if(filn.endAddr%RECORD_LEN){	//��ǰд���ַ��������д�볤�ȵ�������
		filn.endAddr += (RECORD_LEN-(filn.endAddr%RECORD_LEN));
	}

	MSG_LOG("get BR Point1:");
	BCD_LOG((unsigned char *)&filn, sizeof(stBackRecPointer), 1);

	if(filn.startAddr > FLASH2_BAKR_LEN){
		filn.startAddr = 0;
	}
	if(filn.endAddr > FLASH2_BAKR_LEN){
		filn.endAddr = 0;
	}
	*Hadd = filn.startAddr;
	*Cadd = filn.endAddr;

	MSG_LOG("get BR Point2:");
	BCD_LOG((unsigned char *)&filn, sizeof(stBackRecPointer), 1);

}

//�������ʽ��ǰ������
void clr_Record_point(void)
{
//	unsigned char buff[70];
	unsigned int crc32;
//	stStaeInfo stainto;

	MSG_LOG("Clr Record Point!!\r\n");

	lib_clr_Record_point();
	
	crc32 = FLASH_backRec_Pointer;//������ݼ�¼ָ��
	flasherase(ERASE64KBYTESECTOR, crc32);
	crc32 += 0x10000;
	flasherase(ERASE64KBYTESECTOR, crc32);

#ifndef NO_GJ_SERVISE	
	memset((unsigned char*)&tcpipPacket, 0, sizeof(tcpipPacket));
	sysfewrite(BIT_TCPIP_PACKET, 10, (unsigned char*)&tcpipPacket);
#endif

// 	memset((unsigned char*)&stainto, 0, sizeof(stStaeInfo));
// 	memcpy(stainto.DataTime, (unsigned char*)&SysTime, 7);
// 	crc32 = GenerateCRC32((unsigned char*)&stainto, sizeof(stStaeInfo)-4);
// 	stainto.sCrc32 = crc32;
// 	sysfewrite(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);
	
	memset((unsigned char*)&ComSumeFile, 0, sizeof(Parameter3));
	sysfewrite(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	return;
}

#define SYS_INT_STR "xxA5"
void savesysbupara(void)
{
	stSYSBUPARA sysbupara;

	MSG_LOG("��ʼ��FLASH�̶�����:%d\r\n", sizeof(stPricetable));
	BCD_LOG(pFistVary.DeviceNo, 9, 1);
	BCD_LOG(pFistVary.LineNo, 3, 1);
#ifndef NO_GJ_SERVISE
	MSG_LOG("IP:%08X Port:%d\r\n", ClientIP, ClientPort);
#endif	

	memset((unsigned char*)&sysbupara, 0xff, sizeof(stSYSBUPARA));
	memcpy(sysbupara.firstFlag, SYS_INT_STR, 4);
	memcpy(sysbupara.deviceNo, pFistVary.DeviceNo, 9);
	memcpy(sysbupara.lineNo, pFistVary.LineNo, 3);

	memcpy(sysbupara.priceTab, (unsigned char*)&NewPriceTab, sizeof(stPricetable));
#ifndef NO_GJ_SERVISE
	memcpy(sysbupara.severIP, (unsigned char*)&ClientIP, 4);
	memcpy(sysbupara.severPort, (unsigned char*)&ClientPort, 2);
#endif

	BCD_LOG((unsigned char*)&sysbupara,  sizeof(stSYSBUPARA), 1);

	sys_bu_para_save((unsigned char*)&sysbupara, sizeof(stSYSBUPARA));
}
void reflashFerom(stSYSBUPARA *sysbp)
{
	unsigned char buffer[20];
	sysferead(BIT_IS_FIRST_TIME,4,buffer);
	if(memcmp(buffer, SYS_INT_STR, 4) != 0)
		goto refl;
	sysferead(BIT_DEVICE_NO, 9, buffer);
	if(memcmp(buffer, sysbp->deviceNo, 8) != 0)
		goto refl;
	sysferead(BIT_LINENO, 3, buffer);
	if(memcmp(buffer, sysbp->lineNo, 3) != 0)
		goto refl;
	else
		return;//����Ҫ����
refl:
//	MSG_LOG("ˢ��FE����:");
//	BCD_LOG(sysbp->lineNo, 3, 1);
	memcpy(buffer, SYS_INT_STR, 4);
	sysfewrite(BIT_IS_FIRST_TIME, 4, buffer);
	sysfewrite(BIT_LINENO, 3, sysbp->lineNo);
	sysfewrite(BIT_DEVICE_NO, 9, sysbp->deviceNo);
	sysfewrite(BIT_PRICETAB,512,sysbp->priceTab);
}

// void getbusticketVer(void)
// {
// 	unsigned int i=0;
// 	stLoadPtable tempPrice;
// 	
// 	i = FLASH_TEMP_PRICE;
// 	flashread(i, (unsigned char*)&tempPrice, sizeof(stLoadPtable));
// 	if(time_invalid(tempPrice.startDateTime) == 1){	//����ʱ����Ч
// 		memcpy(gBusTicketV.busticketVer, tempPrice.lineVer, 2);
// 	}
// 	else{
// 		memcpy(gBusTicketV.busticketVer, NewPriceTab.lineVer, 2);
// 	}
// 
// 	MSG_LOG(" busLine ticket Ver:");
// 	BCD_LOG(gBusTicketV.busticketVer, 2, 1);
// }
void getbusticketVer(void)
{
	stCpuInfo cpuinfo;
	unsigned int offset;

	offset = ((unsigned char*)&cpuinfo.linever) - ((unsigned char*)&cpuinfo);
	cpuInfoRead(gBusTicketV.busticketVer, offset, 2);
#ifdef _debug_
	debugstring(" busLine ticket Ver:");
	debugdata(gBusTicketV.busticketVer, 2, 1);
#endif
}
//��·��15������
void file_15_line(unsigned char *file_15)
{
	
	unsigned char i,uctemp;
	unsigned char xor = 0;	
//	unsigned char ret;
	unsigned char buff_price[5];
#ifdef _debug_
	unsigned int uitemp;
	char disbuff[64];
#endif	
#ifdef _debug_
	debugstring("file_15_line  ��·��------\r\n");
#endif
	xor = 0;
	for (i=0;i<127;i++)		//ǰ127���У��
	{
		xor ^= file_15[i];
	}
	
#ifdef _debug_
	debugdata(file_15+127,1,1);
	debugstring("calc xor =");
	debugdata(&xor,1,1);
#endif
	
	if (xor != file_15[127])
	{
#ifdef _debug_
		debugstring("xor error return!\r\n");
#endif
		return ;
	}

	for(i=0; i<32; i++)
		NewPriceTab.rate[i] = 104;	//Ĭ�����еĿ�����ֹˢ��

	
	
	pFistVary.LineNo[0]=file_15[0];//��·��  BCD��
	pFistVary.LineNo[1]=file_15[1];//
	pFistVary.LineNo[2]=file_15[2];//
	if (memcmp(file_15+3,"\x00\x00",2) != 0)
	{
		memset(NewPriceTab.busPrice, 0, 4);
		memcpy(NewPriceTab.busPrice,file_15+3,2);//Ʊ��
	}else{
#ifdef _debug_
		debugstring("file_15+3 000000000   error\r\n");
#endif
	}
	
	memset(NewPriceTab.plusPrice, 0, 4);
	memcpy(NewPriceTab.plusPrice,file_15+5,2);//���ӷ�
	
#ifdef _debug_
	if (file_15[7] == 1)
	{
		debugstring("01����ʱ��\r\n");
	}else{
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"%d:��%dʱ��\r\n",file_15[7],file_15[7]);
		debugstring(disbuff);
	}
	
	debugstring("LineNo:");
	debugdata(pFistVary.LineNo,3,1);
	debugstring("NewPriceTab.busPrice:");
	debugdata(NewPriceTab.busPrice,4,1);
	debugstring("NewPriceTab.plusPrice:");
	debugdata(NewPriceTab.plusPrice,4,1);	
	debugstring("��ʱ��Ʊ�۱�:\r\n");
	debugdata(file_15+8,15,1);
	for (i=0;i<3;i++)
	{
		debugdata(file_15+8+i*5,5,1);
	}
	debugstring("end=----\r\n");
#endif
	
	
	for (i=0;i<20;i++)
	{
		memcpy(buff_price,file_15+23+i*5,5);
//		debugdata(buff_price,5,1);
		if ( memcmp(buff_price,"\x00\x00\x00\x00\x00",5) == 0)
		{
			continue;
		}else{
#ifdef _debug_
			debugdata(file_15+23+i*5,5,1);
#endif
			uctemp = buff_price[0];
			if (uctemp >= 0x40)
			{
				uctemp -= 0x40;
			}
			NewPriceTab.rate[uctemp] = buff_price[1];
			NewPriceTab.voice[uctemp] = buff_price[2];
			NewPriceTab.plusrate[uctemp] = buff_price[3];
			NewPriceTab.cardControl[uctemp] = buff_price[4];
			
		}
		
	}
	
	//	memcpy(pFistVary.LineNoOld, pFistVary.LineNo, 3);//�ɵ���·��
//	Get_SerialNumF4(NewPriceTab.deviceNo);
	memset(NewPriceTab.lineVer ,0,2);
	memcpy(NewPriceTab.lineVer, file_15+123, 2);
	memcpy(NewPriceTab.disprice,file_15+125,2);
	over_turn(2,NewPriceTab.disprice);
	memcpy(NewPriceTab.Time_price,file_15+7,16);
#ifdef _debug_
	uitemp = 0;
	memcpy((unsigned char *)&uitemp,NewPriceTab.disprice,2);
	sprintf(disbuff,"��ʾ::%d  YUAN\r\n\x0",uitemp);
	debugstring(disbuff);
	debugstring("�ֶ�Ϊ::::\r\n");
	debugdata(NewPriceTab.Time_price ,16,1);
	debugstring("Ʊ�۰汾Ϊ::::\r\n");
	debugdata(NewPriceTab.lineVer ,2,1);
#endif
	
	
	SetAudio();
#ifdef _debug_
	debugstring("LineNo1:");
	debugdata(pFistVary.LineNo,3,1);
#endif
	sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
	sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);
#ifdef _debug_
	debugstring("NewPriceTab:\r\n");
	debugdata((unsigned char*)&NewPriceTab,256,1);
#endif
	
//	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
	
	savesysbupara();
	saveCpuInfo(1,buff_price);
	F_FLAG_VARY.set_line_status=0;

#ifdef _debug_price
	debugstring("file_15_line Deal Over\r\n");
#endif
	
	return;
}

//���ڸ�����GPRS��̨���ص�Ʊ����Ϣ
unsigned char checkNewPrice(void)
{
	#if 0
	unsigned int i=0;
	unsigned int itemp,temp2;
	stLoadPtable tempPrice;
	unsigned char cnpBuf[16];
	unsigned char buffer[256];
	
	//�����µ�Ʊ����Ϣ
#ifdef _debug_sys_info_
	debugstring("read Time\r\n");
#endif
	GetDateTime();

#ifdef _debug_
	debugdata((unsigned char*)&SysTime, 7, 1);
#endif

	i = FLASH_TEMP_PRICE;
	flashread(i, (unsigned char*)&tempPrice, sizeof(stLoadPtable));
	memset(tempPrice.startDateTime+4, 0x01, 3);
#ifdef _debug_price
	debugstring("�µ�Ʊ����Ϣ:");
	debugdata((unsigned char*)&tempPrice, sizeof(stLoadPtable), 1);
#endif
	if(time_invalid(tempPrice.startDateTime) == 1){
		memcpy((unsigned char *)&itemp, tempPrice.startDateTime, 4);
		over_turn(4, (unsigned char*)&itemp);
		
		memcpy((unsigned char *)&temp2, (unsigned char*)&SysTime.year_h, 4);
		over_turn(4, (unsigned char*)&temp2);
#ifdef _debug_price
		debugstring("�µ�Ʊ����Ϣdate:");
		debugdata((unsigned char *)&itemp, 4, 1);
		debugdata((unsigned char *)&temp2, 4, 1);
		if (tempPrice.air_flag)
		{
			memset(buffer,0,sizeof(buffer));
			memcpy((unsigned char*)&itemp, NewPriceTab.plusPrice, 4);
			sprintf((char *)buffer,"�յ�������::%d\r\n",itemp);
			debugstring((char *)buffer);
		}else{
			debugstring("�յ��ѹر�-----\r\n");
		}
#endif

		Get_SerialNumF4(cnpBuf);

		if (memcmp(cnpBuf, tempPrice.SerialNo, 4) != 0)
		{
			i = FLASH_TEMP_PRICE;
			flasherase(ERASE4KBYTESECTOR, i);//�����ʱ��
#ifdef _debug_price
			debugstring("�·���ST�źͱ�����һ��=======\r\n");
#endif
			return 0;
		}

		BCD2Ascii(tempPrice.deviceNo, cnpBuf, 4);
		if(memcmp(cnpBuf, pFistVary.DeviceNo, 8) != 0){//��Ҫ�����豸��
#ifdef _debug_price
			debugstring(" �����豸:");
			debugdata(cnpBuf, 8, 1);
			debugstring("old DeviceNo::");
			debugdata(pFistVary.DeviceNo, 8, 1);
#endif
			if (memcmp(cnpBuf,"\x00\x00\x00\x00\x00\x00\x00\x00",8) != 0)
			{
				memcpy(pFistVary.DeviceNo, cnpBuf, 8);
				pFistVary.DeviceNo[8] = 0;
				sysfewrite(BIT_DEVICE_NO,9,pFistVary.DeviceNo);
				savesysbupara();
				saveCpuInfo(2,buffer);
			}else{
#ifdef _debug_price
				debugstring("ALL 0000000000000 error!!!\r\n");
#endif			
			}
		}
		
		if(memcpy(pFistVary.LineNo, tempPrice.LineNo+2, 2) != 0){
#ifdef _debug_price
			debugstring("  ������·:");
			debugdata(pFistVary.LineNo, 3, 1);
			debugdata(tempPrice.LineNo, 4, 1);
#endif
			if(memcmp(tempPrice.LineNo+2,"\x00\x00",2) != 0)
			{
				memcpy(pFistVary.LineNo, tempPrice.LineNo+2, 2);
				pFistVary.LineNo[2] = 0;
			}else{
#ifdef _debug_price
				debugstring("ALL 00000 error!!!\r\n");
#endif			
			}
		}
#ifdef _debug_price
		debugstring("bussiness:");
		debugdata(tempPrice.bussiness,2,1);
#endif


		if(temp2 >= itemp)
		{
#ifdef _debug_price
			debugstring("��Ҫ����:");
#endif
//			memcpy(NewPriceTab.citycode, tempPrice.citycode, 2);
			memset(NewPriceTab.hanye, 0, 2);
			memcpy(NewPriceTab.startDateTime, tempPrice.startDateTime, 7);
			memcpy(NewPriceTab.lineVer, tempPrice.lineVer, 2);
			memcpy(gBusTicketV.busticketVer, tempPrice.lineVer, 2);


			if (tempPrice.price_byte != 0)	//��������������·������Ϣ
			{


				i = FLASH_TEMP_PRICE +sizeof(stLoadPtable);
				flashread(i, buffer, tempPrice.price_byte);
#ifdef _debug_price
				debugstring("read:line infor");
				debugdata(buffer,tempPrice.price_byte,1);
#endif
				file_15_line(buffer);
			}
#ifdef _debug_
			debugstring("ͷ--------:\r\n");
			debugdata(pFistVary.LineNo,3,1);
#endif
#ifdef _debug_
			debugstring("NewPriceTab:\r\n");
			debugdata((unsigned char*)&NewPriceTab,256,1);
#endif
			
			
			memcpy(&temp2,NewPriceTab.plusPrice,4);
			
			
#ifdef _debug_price
			debugstring("plusPrice");
			debugdata((unsigned char *)&temp2,4,1);
			debugstring("�ı�־PlusSta=1------\r\n");
#endif
			F_FLAG_VARY.PlusSta = tempPrice.air_flag;
			fewrite(BIT_F_FLAG_VARY,LEN_F_FLAG_VARY,(unsigned char*)&F_FLAG_VARY);
			
#ifdef _debug_price
			debugstring("read:PlusSta=");
			debugdata(&F_FLAG_VARY.PlusSta,1,1);
#endif
			
			sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
			sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
			sysfewrite(BIT_DEVICE_NO,9,pFistVary.DeviceNo);
			
			saveCpuInfo(1,buffer);
			saveCpuInfo(2, buffer);
			savesysbupara();		
			restore_flag = 3;
			
			i = FLASH_TEMP_PRICE;
			flasherase(ERASE4KBYTESECTOR, i);//�����ʱ��
			
			return 1;
		}
	}
	#endif
	return 0;
}


void saveCpuInfo(unsigned char mode, unsigned char *dat)
{
	stCpuInfo cpuinfo;
	unsigned int Crc32;
	
	cpuInfoRead((unsigned char *)&cpuinfo, 0, sizeof(stCpuInfo));

	MSG_LOG("read cpu ROM:");
	BCD_LOG((unsigned char*)&cpuinfo, sizeof(stCpuInfo), 1);

	switch(mode){
	case 1:
		memcpy(cpuinfo.LineNo, pFistVary.LineNo, 3);
		memcpy(cpuinfo.buPrice, NewPriceTab.busPrice, 4);
		memcpy(cpuinfo.rate, NewPriceTab.rate, 32);
		memcpy(cpuinfo.voice, NewPriceTab.voice, 32);
		memcpy(cpuinfo.linever, NewPriceTab.lineVer, 2);
		memcpy(cpuinfo.plusMonthPrice, NewPriceTab.plusMonthPrice, 4);
		break;
	case 6:
		memcpy(cpuinfo.UserKeyA, dat, 16);
		Crc32 = GenerateCRC32(dat, 16);
		memcpy(cpuinfo.UserKeyACrc32, (unsigned char*)&Crc32, 4);
		memcpy(cpuinfo.UserKeyB, dat+16, 16);
		Crc32 = GenerateCRC32(dat+16, 16);
		memcpy(cpuinfo.UserKeyBCrc32, (unsigned char*)&Crc32, 4);
		break;
	case 7://�̻����
		if((memcmp(cpuinfo.BussinessNo, dat, 2) == 0)&&(cpuinfo.catalogPOST == cardMessage.catalogPOST)){//�̻���ͬ,Ŀ¼��ͬ�������ٴα���
			return;
		}
		memcpy(cpuinfo.BussinessNo, dat, 2);
		cpuinfo.catalogPOST = cardMessage.catalogPOST;
		Crc32 = GenerateCRC32(dat, 2);
		memcpy(cpuinfo.BussinessNo+2, (unsigned char*)&Crc32, 4);
		break;
	case 2:
		memcpy(cpuinfo.deviceNO, pFistVary.DeviceNo, 8);
		break;
	case 9://stopflag
		cpuinfo.stopflag = dat[0];
		break;
	case 8://����APN
		break;
	case 10://����SSID
		memcpy(cpuinfo.wifi_SSID, dat, 10);
		Crc32 = GenerateCRC32(cpuinfo.wifi_SSID, 30);
		memcpy(cpuinfo.wifi_CrC32, (unsigned char*)&Crc32, 4);
		break;
	case 11://����wifi_KEK
		memcpy(cpuinfo.wifi_KEK, dat, 20);
		Crc32 = GenerateCRC32(cpuinfo.wifi_SSID, 30);
		memcpy(cpuinfo.wifi_CrC32, (unsigned char*)&Crc32, 4);
		break;
	case 12://�ڷֶ��շ��б�����ǰ�Ż����Ǻ��Ż��ı�ʶ,��������ͨ��Ӳ���жϡ�
		if(cpuinfo.FDModeFlag == dat[0]){//һ��������Ҫ�ٴα���
			return;
		}
		cpuinfo.FDModeFlag = dat[0];
		Crc32 = GenerateCRC32(cpuinfo.wifi_SSID, 30);
		memcpy(cpuinfo.wifi_CrC32, (unsigned char*)&Crc32, 4);
		break;
	default:
		return;
	}
	Crc32 = GenerateCRC32((unsigned char*)&cpuinfo, (sizeof(stCpuInfo) - 4));
	memcpy(cpuinfo.Crc32, (unsigned char*)&Crc32, 4);

	MSG_LOG("save cpu ROM:");
	BCD_LOG((unsigned char*)&cpuinfo, sizeof(stCpuInfo), 1);

	cpuInfoWrite((unsigned char*)&cpuinfo, sizeof(stCpuInfo));
}

void getCpuInfo(stCpuInfo *inP)
{
	stCpuInfo cpuinfo;
	unsigned int Crc32, i;
	
	cpuInfoRead((unsigned char *)&cpuinfo, 0, sizeof(stCpuInfo));

	MSG_LOG("get cpu ROM:");
	BCD_LOG((unsigned char*)&cpuinfo, sizeof(stCpuInfo), 1);
	
	Crc32 = 0;
	Crc32 = GenerateCRC32((unsigned char*)&cpuinfo, (sizeof(stCpuInfo) - 4));
	memcpy((unsigned char*)&i, cpuinfo.Crc32, 4);
	if(Crc32 != i){//û��
		memcpy(cpuinfo.LineNo, pFistVary.LineNo, 3);
		memcpy(cpuinfo.buPrice, NewPriceTab.busPrice, 4);
		memcpy(cpuinfo.rate, NewPriceTab.rate, 32);
		memcpy(cpuinfo.deviceNO, pFistVary.DeviceNo, 8);
		memset(cpuinfo.reservee, 0xFF, sizeof(cpuinfo.reservee));
		memset(cpuinfo.UserKeyA, 0xFF, 16);
		memset(cpuinfo.UserKeyACrc32, 0xFF, 4);
		memset(cpuinfo.UserKeyB, 0xFF, 16);
		memset(cpuinfo.UserKeyBCrc32, 0xFF, 4);
		memset(cpuinfo.BussinessNo, 0xFF, 6);
		memset(cpuinfo.Reserver, 0xFF, sizeof(cpuinfo.Reserver));
		cpuinfo.catalogPOST = 0;
		Crc32 = GenerateCRC32((unsigned char*)&cpuinfo, (sizeof(stCpuInfo) - 4));
		memcpy(cpuinfo.Crc32, (unsigned char*)&Crc32, 4);
		cpuInfoWrite((unsigned char*)&cpuinfo, sizeof(stCpuInfo));
	}
	else{//��Ч����
		i = 0;
		if(memcmp(pFistVary.LineNo, cpuinfo.LineNo, 3)!=0){
			sysfewrite(BIT_LINENO,3,cpuinfo.LineNo);
			i++;
		}
		if(memcmp(pFistVary.DeviceNo, cpuinfo.deviceNO, 8)!=0){
			sysfewrite(BIT_DEVICE_NO,8,cpuinfo.deviceNO);
			i++;
		}
#ifdef KEYBOARD
		if(KeyBoardStyle != KEYBOARD_INPUT){	//����Ӽ������룬���ó��ڽ�����Ʊ�ۡ�
#endif		
			if(memcmp(NewPriceTab.busPrice, cpuinfo.buPrice, 4)!=0){
				memcpy(NewPriceTab.busPrice, cpuinfo.buPrice, 4);
				memcpy(NewPriceTab.rate, cpuinfo.rate, 32);
				sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);
				i++;
			}
			memcpy(NewPriceTab.busPrice, cpuinfo.buPrice, 4);
#ifdef KEYBOARD
		}
#endif
		memcpy(NewPriceTab.rate, cpuinfo.rate, 32);
		memcpy(NewPriceTab.plusMonthPrice, cpuinfo.plusMonthPrice, 4);
		memcpy(pFistVary.LineNo, cpuinfo.LineNo, 3);
		memcpy(pFistVary.DeviceNo, cpuinfo.deviceNO, 8);
#ifdef AutoAudio_line
		memcpy(NewPriceTab.voice, cpuinfo.voice, 32);
#endif

		MSG_LOG("get cpu ROM NewPriceTab.voice:");
		BCD_LOG(NewPriceTab.voice, 32, 1);
		if(i > 0)
			savesysbupara();
		memcpy((unsigned char*)inP, (unsigned char*)&cpuinfo, sizeof(stCpuInfo));
	}
}
#ifdef KEYBOARD
void keyBoardStart(void)
{
	unsigned int ksi;
	keyboard_open();
	memset(Keybuffer,0x20,5);
	// 	memset(NewPriceTab.busPrice, 0, 4);
	// 	memset(NewPriceTab.plusPrice, 0, 4);
	// 	Keybuffer[5]='0';
	memcpy((unsigned char*)&ksi, NewPriceTab.busPrice, 4);
	
	KeyDis(ksi,Keybuffer);
	LedDisplay(Keybuffer);
	memset(Keybuffer,0x20,5);
 	Keybuffer[5]='0';
}
#elif defined _GPS_Station_ExKey_enable_
extern void Ex_keyBoardStart(void);
#endif

void SysParInit(void)
{
	unsigned int i=0, indexn=0;
	unsigned char buffer[256];
	unsigned char IsFirstTime[5];
	unsigned char flag = 0;
	stSYSBUPARA sysbupara;
	
	MSG_LOG("---sysinit---\r\n");

	sysferead(BIT_CHN_ENG, 1, (unsigned char*)&gchn_eng);
#ifdef _AREA_ENG_
	gchn_eng = 'E';
#else
	gchn_eng = 'C';
#endif
	cls();
	if(gchn_eng == 'E'){
		display(0,0,"checking pos",1);
		display(2,0,"PLS wait...", 0);
	}
	else{
		display(0,0,"ϵͳ��ʼ��",0);
		display(2,1,"��ȴ�......",0);
	}
	qpoc_first(); //�ж�
	mzt_qpoc_init();
	qpoc_init_singe();
	init_delay_send();//��ʱ������
#ifdef CARD_SOUND
	cardSound=0;
	card_ser=0;
#endif
	
	sys_bu_para_load((unsigned char *)&sysbupara, sizeof(stSYSBUPARA));
	memcpy(IsFirstTime, sysbupara.firstFlag,4);

	MSG_LOG("---sys bu para---\r\n");
	BCD_LOG((unsigned char*)&sysbupara,  sizeof(stSYSBUPARA), 1);
	MSG_LOG("firstflag:");
	BCD_LOG(sysbupara.firstFlag, 4, 1);
	MSG_LOG("device:");
	BCD_LOG(sysbupara.deviceNo, 9, 1);
	MSG_LOG("lineno:");
	BCD_LOG(sysbupara.lineNo, 3, 1);
	MSG_LOG("sev IP:");
	BCD_LOG(sysbupara.severIP, 4, 1);
	MSG_LOG("sev PORT:");
	BCD_LOG(sysbupara.severPort, 2, 1);
	MSG_LOG("Price Table:");
	for(i=0;i<2;i++)//32
	{
		BCD_LOG(sysbupara.priceTab+(16*i), 16, 1);
	}	
	
	if(memcmp(sysbupara.firstFlag, SYS_INT_STR, 4) == 0){
		flag = 1;//FLASH����Ч
		goto NoFristRun;
	}
	else{
		Delay(100);
		sys_bu_para_load((unsigned char *)&sysbupara, sizeof(stSYSBUPARA));
		memcpy(IsFirstTime, sysbupara.firstFlag,4);

		if(memcmp(sysbupara.firstFlag, SYS_INT_STR, 4) == 0){
			flag = 1;//FLASH����Ч
			goto NoFristRun;
		}
	}
	
	MSG_LOG(" Judge KKKKK:\r\n");
	memset(buffer,0,4);
	sysferead(BIT_IS_FIRST_TIME,4,buffer);
	memcpy(IsFirstTime, buffer,4);
	
	MSG_LOG(" Judge IsFirstTime:\r\n");
	BCD_LOG(IsFirstTime, 4, 1);
	BCD_LOG(buffer, 4, 1);

	if (memcmp(IsFirstTime, SYS_INT_STR, 4) != 0) //YT05
	{
		sysferead(BIT_IS_FIRST_TIME,4,buffer);
		memcpy(IsFirstTime, buffer,4);
	}
	else
	{
		goto NoFristRun;
	}
	if(memcmp(IsFirstTime, SYS_INT_STR, 2) != 0) //YT01
	{
		MSG_LOG("first run init SYStem Pra!!!\r\n");
		MSG_LOG(" Judge IsFirstTime:\r\n");
		BCD_LOG(IsFirstTime, 4, 1);
//������վ״̬
// 		memset((unsigned char*)&F_FLAG_VARY, 0, LEN_F_FLAG_VARY);
// 		F_FLAG_VARY.stop_flag = 1;
// 		sysfewrite(BIT_F_FLAG_VARY,LEN_F_FLAG_VARY,(unsigned char*)&F_FLAG_VARY);
//������ˮ��Ϣ
		memset((unsigned char*)&POINT_MESSAGE,0,sizeof(Parameter2));
		sysfewrite(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN,(unsigned char*)&POINT_MESSAGE);
//MSG_LOG(" system 2\r\n");
//������Ϣ�ļ�	
		memset((unsigned char*)&ComSumeFile,0,sizeof(Parameter3));
		sysfewrite(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
		clr_Record_point();
		saveCpuInfo(7,CLOUD_BUSSINESSNO);//�̻���  00009998
		PRICE_init_frist();
//����TAC����
//		MAC_po=0;
//		sysfewrite(BIT_MAC_PO,1,(unsigned char *)&MAC_po);
//
//		memset(DriveCardNo,0,4);
//		sysfewrite(BIT_DRICE_CARD,4,DriveCardNo);
//		clr_dog();
//		memset(&BAK_WG,0,3);
// 		sysfewrite(BIT_BAK_WBUFFER_PO,3,(unsigned char *)&BAK_WG);
//������Ϣ		
		memset(buffer,0,50);

//������
		sysfewrite(BIT_BLACK_SUM,4,buffer);
		sysfewrite(BIT_BLACK_2BLOK,32,buffer);
		i = FLASH_BLK_START;
		flasherase(ERASE64KBYTESECTOR, i);
//��·��
//		memcpy(buffer, "\x00\x11\x01", 3);
//		sysfewrite(BIT_LINENO,3,buffer);
//�豸���
//		memcpy(buffer, "000000110", 9);
//		sysfewrite(BIT_DEVICE_NO,9,buffer);
//��������ˢ��
		memset(buffer,0,100);
		sysfewrite(BIT_INTERVAL_STAR,1,buffer);
		sysfewrite(BIT_INTERVAL_END,1,buffer);
		memset(buffer,0xFF,100);
		for(i=0;i<7;i++)
			sysfewrite(BIT_INTERVAL_DATA+(i*LAST_LIST_LEN),LAST_LIST_LEN,buffer);
//����ˢ��		
		for(i=0;i<ERROR_DETAIL_SUM;i++)
		{
			memset((unsigned char *)&ErrorDetail[i],0,ERROR_DETAIL_LEN);
			sysfewrite(BIT_ERROR_DETAIL+i*ERROR_DETAIL_LEN,ERROR_DETAIL_LEN,(unsigned char *)&ErrorDetail[i]);
		}
		error_pointer=0;
		sysfewrite(BIT_ERROR_POINT,1,(unsigned char *)&error_pointer);
//��ʷ��
		memset(buffer,0,100);
		for(i=0;i<10;i++)
			sysfewrite(BIT_HISDATA_GATHER+i*21,21,buffer);
//ת�˲��� 20070606
		buffer[2] = 0x5A;
		buffer[2] ^= buffer[0];
		buffer[2] ^= buffer[1];
		sysfewrite(BIT_ZCYH, 3, buffer);
		
		pFistVary.fileSno[0] = 1;
		pFistVary.fileSno[1] = 0;
		fewrite(BIT_GET_FILESNO,2,pFistVary.fileSno);
//		memcpy(sysbupara.deviceNo, "000000010", 9);
//		memcpy(sysbupara.lineNo, "\x00\x01\x01", 3);
//		memset(sysbupara.priceTab, 0, 512);
#ifndef NO_GJ_SERVISE
		ClientIP = GJ_IP;//0x0E9EC8DA;//58.61.47.85//ʮ��218.200.158.14
		ClientPort= GJ_PORT;//8818//1199
		memcpy(sysbupara.severIP, (unsigned char*)&ClientIP, 4);
		memcpy(sysbupara.severPort, (unsigned char*)&ClientPort, 2);
#endif
		memcpy(sysbupara.firstFlag, SYS_INT_STR, 4);
		flag = 1;
		sys_bu_para_save((unsigned char*)&sysbupara, sizeof(stSYSBUPARA));
//��ϵͳ�״�������ʶ
		memcpy(buffer, SYS_INT_STR, 4);
   		sysfewrite(BIT_IS_FIRST_TIME, 4, buffer);
//GPS Par Int
		memset(buffer,0,100);
		sysfewrite(BIT_TCP_REV_SUM,4,buffer);
#ifdef _GPS_ENABLE_
		GPS_PAR_CLR();
#endif
#ifndef NO_MOBILE_24G_
		clr_mobile_record();
		first_clr_mobile_para();
#endif
#ifndef NO_GJ_SERVISE
		ClrGPRSFE();
#endif
	} 
NoFristRun:
	
	sysferead(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	
//���б�־	
	sysferead(BIT_F_FLAG_VARY,sizeof(fFlagVary_1),(unsigned char*)&F_FLAG_VARY);
//������ˮ��Ϣ
	sysferead(BIT_POINT_MESSAGE,sizeof(Parameter2),(unsigned char*)&POINT_MESSAGE);
//������Ϣ�ļ�	
		MSG_LOG(" system 11\r\n");
	i = Get_Record_point(buffer, 0);

	MSG_LOG("cur pointer:%04X\r\n", i);
	MSG_LOG("Head :");
	BCD_LOG(buffer, 4, 1);

	MSG_LOG("ComsumFile:");
	BCD_LOG((unsigned char*)&ComSumeFile, COMSUM_MESSAGE_FILE_LEN, 1);

//����TAC����
//	sysferead(BIT_MAC_PO,1,(unsigned char *)&MAC_po);
	sysferead(BIT_DRICE_CARD,4,DriveCardNo);

	MSG_LOG("DriveCardNo:");
	BCD_LOG(DriveCardNo, 4, 0);
//	sysferead(BIT_BAK_WBUFFER_PO,3,(unsigned char *)&BAK_WG);
//������Ϣ
//	MSG_LOG("NewPriceTab:\r\n");

	sysferead(BIT_STUFFER_NO,4,StufferNO);

	MSG_LOG("stufferNO:");
	BCD_LOG(StufferNO, 4, 0);
	
	if(flag == 1)//FLASH ��Ч
	{
		MSG_LOG("F ��Ч\r\n");

		memcpy((unsigned char*)&NewPriceTab, sysbupara.priceTab, sizeof(stPricetable));

		BCD_LOG((unsigned char*)&NewPriceTab, sizeof(stPricetable), 1);

	}
	else{
			sysferead(BIT_PRICETAB,sizeof(stPricetable),(unsigned char*)&NewPriceTab);
			BCD_LOG((unsigned char*)&NewPriceTab, sizeof(stPricetable), 1);
	}
	//��·��
	if(flag == 1)//flash ��Ч
	{
		memcpy(pFistVary.LineNo, sysbupara.lineNo, 3);
	}
	else
		sysferead(BIT_LINENO,3,pFistVary.LineNo);
	
	//�豸���
	if(flag == 1)//flash ��Ч
	{
		memcpy(pFistVary.DeviceNo, sysbupara.deviceNo, 9);
#ifndef NO_GJ_SERVISE
		memcpy((unsigned char *)&ClientIP, sysbupara.severIP, 4);
		memcpy((unsigned char*)&ClientPort, sysbupara.severPort, 2);
#endif
	}
	else{
		sysferead(BIT_DEVICE_NO,9,pFistVary.DeviceNo);
#ifndef NO_GJ_SERVISE
		sysferead(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
		sysferead(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
#endif
	}

	MSG_LOG("DeviceNo:");
	BCD_LOG(pFistVary.DeviceNo, 9, 0);
	
	if(checkNewPrice() == 1)
		memcpy(sysbupara.priceTab, (unsigned char*)&NewPriceTab, sizeof(stPricetable));

#ifdef MOBILE_TONGCHUAN_
	ClientIP = GJ_IP;
	ClientPort = GJ_PORT;
#elif defined BUS_XINYI
	ClientIP = GJ_IP;
	ClientPort = GJ_PORT;
#elif defined IP_OF_SLZR_
	ClientIP = GJ_IP;
	ClientPort = GJ_PORT;
#endif

//����������
 	sysferead(BIT_BLACK_SUM,4,buffer);
	sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
	// 

 	MSG_LOG("BlackListNum:::");
	BCD_LOG(buffer,4,1);
	BCD_LOG((unsigned char*)&gBlackBlockNum[0], 32, 1);

// 	MSG_LOG("blackVer:");
// 	BCD_LOG((unsigned char*)&BLACK_ADD, 8, 1);
 	memcpy((unsigned char*)&pFistVary.BlackListNum,buffer,4); //����������
//	if(pFistVary.BlackListNum_1 > 3500)
//		pFistVary.BlackListNum_1 = 0;
//	if(pFistVary.BlackListNum_2 > 3500)
//		pFistVary.BlackListNum_2 = 0;
//ˢ������
	for(i=0;i<ERROR_DETAIL_SUM;i++)
	{
		memset(buffer,0,ERROR_DETAIL_LEN);
		sysferead(BIT_ERROR_DETAIL+i*ERROR_DETAIL_LEN,ERROR_DETAIL_LEN,buffer);
		memcpy((unsigned char *)&ErrorDetail[i],buffer,ERROR_DETAIL_LEN);
	}
	memset(buffer,0,2);
	sysferead(BIT_ERROR_POINT,1,buffer);
	error_pointer=buffer[0];
	//����ʷ�ɼ��ļ�
	for(i=0;i<10;i++)
	{
		sysferead(BIT_HISDATA_GATHER+i*21,21,buffer);
		
		memcpy((unsigned char*)&hisFCB[i],buffer,21);

// 		MSG_LOG("HIS Buff:");
// 		BCD_LOG(buffer, 20, 1);
// 		MSG_LOG("HIS Length:");
// 		BCD_LOG((unsigned char*)&hisFCB[i].fileLength, 4, 1);
// 		MSG_LOG("HIS strAddr:");
// 		BCD_LOG((unsigned char*)&hisFCB[i].strAddr, 4, 1);
// 		MSG_LOG("HIS endAddr:");
// 		BCD_LOG((unsigned char*)&hisFCB[i].endAddr, 4, 1);
// 		MSG_LOG("HIS fileSno:");
// 		BCD_LOG(hisFCB[i].fileSno, 2, 1);
	
	}
	//����������ˢ������
	sysferead(BIT_INTERVAL_STAR,1,(unsigned char *)&MothDelayTime.startptr);
	sysferead(BIT_INTERVAL_END,1,(unsigned char *)&MothDelayTime.endptr);
	
	MSG_LOG("interval  Starptr=%02X Endptr==%02X\r\n", MothDelayTime.startptr, MothDelayTime.endptr);

	for(i=0;i<LAST_LIST_LEN;i++)
	{
		sysferead((BIT_INTERVAL_DATA+i*7), 7, MothDelayTime.LastList[i]);
	}
//	cardMessage.ProcessFlag=0;
	
	feread(BIT_GET_FILESNO,2,pFistVary.fileSno);

	memset(buffer,0,5);

	gErrorFlag = 0;
	gErrortimes[0] = 0;
	gErrortimes[1] = 0;

	c_serial=0;
	
	if(checkBusInfo() ==0){//���������ȷ���ɳ�ʼ������
		if(flag != 1) //��Ч���������еĳ�ʼ��
		{
			savesysbupara();
		}
		else{//��Ч�����Ƿ���Ҫˢ������
			reflashFerom(&sysbupara);
		}
	}

	
	memcpy(buffer, SYS_INT_STR, 4);//��Ϊ���ϰ汾���������ڴ棬���԰汾����ʱҪ���������ǰû�õĴ�����
	
	if(memcmp(IsFirstTime+2, buffer+2, 2) != 0){//�ȽϺ������汾
		if(memcmp(buffer+2, "A5", 2) == 0){//��YYA5�汾��Ҫ������������
			for(i=5; i<16; i++){//�������Ŀ顣ǰ���汾ֻ�õ�ǰ5�顣�������Ҫ������Ȼ�������⡣
				indexn = (i*0x10000);
				flasherase(ERASE64KBYTESECTOR, indexn);
				delayxms(2);
				display(6, 0, "���������",0);
			}
		}
		savesysbupara();
	}

#ifndef NO_GJ_SERVISE
	loadGPRSFE();
#endif
	
	face_DisTime = 0;
	ghisCur=0xffffffff;
	USART1mode = 0;
//��ʼ��GPS����
#ifdef _GPS_ENABLE_
	gps_par_init();
#endif
#ifndef NO_GJ_SERVISE
	gprs_par_init();
#endif
	
#ifdef GPRS_SOUD_ENABLE
	ucMC323Mode = 0;
#endif

	getCpuInfo((stCpuInfo*)buffer);	//ȡCPU��FLASH����Ĳ�������
	
	getbusticketVer();
#ifdef _CPU_24G_
	G24init();
#endif
#ifdef FengDuan_BU_	//��ʼ���ֶ��շѵı���
	initFDpara_ALL();
#endif

#ifdef KEYBOARD
	//���ж��Ƿ����ⲿ����Ĭ�����ⲿ����, ����д����̺�û�д����̵Ļ��ã����ﲻ��Ҫ��ʼ��
//	KeyBoardStyle = KEYBOARD_INPUT;

	getCpuPrice();
	keyBoardStart();
#endif

#ifdef _GPS_Station_ExKey_enable_
	Ex_keyBoardStart();
#endif

#ifdef Center_Ctrl_BU_ENABEL
	BUCtrl_Get_Flag();
#endif

#ifndef NO_GJ_SERVISE	//������ʱ����IP���������ȷ�����ó�Ĭ��ֵ��
	aoutMcheckIP();
#endif
	MSG_LOG("sysinit END \r\n");

	return;
}

unsigned char GetDateTime(void)
{
	unsigned char i=0, ii;
	unsigned char buf[10];

#ifdef _GPS_ENABLE_
#ifndef _GPS_TEST_
	if((gpsTimeOK == 0xe5)&&(time_invalid((unsigned char*)&SysTime) != 0)){//GPSʱ����Ч
		gpsTimeOK = 0;
		return ST_OK;
	}
#endif
#endif
	for(i=0; i<3; i++) 
	{
		timeread((unsigned char*)&SysTime);
		memcpy(buf, (unsigned char*)&SysTime, 7);
		if(time_invalid(buf) == 0)
			continue;
		
		for(ii=0;ii<7;ii++){
			if((buf[i] & 0x0f) > 0x09)
				break;
			if(((buf[i] >> 4) & 0x0f) > 0x09)
				break;
		}
		if(ii < 7)
			continue;
		return ST_OK;
	}
	return ST_ERROR;
}

unsigned char SL3DES_MiKEY(void)
{
	unsigned char buffer[30];
	unsigned char pos;
	unsigned char tempbuf[10];

	pos=0;
	memcpy(buffer+pos, cardMessage.CityCode, CITY_CODE_LEN);	//���д���	02
	pos+=CITY_CODE_LEN;
	memcpy(buffer+pos, (unsigned char*)&c_serial ,4);				//�����к�	04
	pos+=4;
	memcpy(buffer+pos, cardMessage.PublishBicker, 2);			//������ˮ(BCD����λ)	02
	pos+=2;
	

	MSG_LOG("3DESGetKey\r\n");
	MSG_LOG("����:");
	BCD_LOG(buffer, 8, 1);

#ifdef BUS_ANSHUN_
	Lib3DESKey(buffer,tempbuf, 1);//��˳
#elif defined BUS_XINYI	//����
	Lib3DESKey(buffer,tempbuf, 3);//���� 3
#else
	Lib3DESKey(buffer,tempbuf, 0);
#endif
	
	MSG_LOG("����:");
	BCD_LOG(tempbuf, 8, 1);
	MSG_LOG("�����8�ֽڣ�ȡǰ6�ֽ�\r\n");
	
	memcpy(cardMessage.MoneyKEYA,  tempbuf, 6);				//����Ǯ��,��Ϣ2 6
	memcpy(cardMessage.RecodeKEYA, tempbuf, 6);			//���׼�¼	03 04 05
	memcpy(cardMessage.MonthKEYA,  tempbuf, 6);			//��Ʊ���� 7 8
	memcpy(cardMessage.PublichKEYA, tempbuf, 6);
	

	MSG_LOG(" Money KEYA:\r\n");
	BCD_LOG(cardMessage.MoneyKEYA, 6, 1);
	MSG_LOG(" Publich KEYA:\r\n");
	BCD_LOG(cardMessage.PublichKEYA, 6, 1);
	MSG_LOG(" Recode KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);
	MSG_LOG(" Month KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);

	return ST_OK;
}

#ifdef BUS_ANLONG_
unsigned char ANLONG_mikey(void)
{
	unsigned char keySeed[8];
	unsigned char keytemp[6];
	unsigned char i;
//����A������������������ǰ4�ֽ����+����ǰ2�ֽ�
	memcpy(keySeed, "\x47\x53\x26\x32\x85\x77\x13\x25", 8);
	memcpy(keytemp, (unsigned char*)&c_serial, 4);
	keytemp[4] = keytemp[0];
	keytemp[5] = keytemp[1];
	for(i=0; i<4; i++){
		keytemp[i] ^= keySeed[i];
	}
	memcpy(cardMessage.MoneyKEYA,  keytemp, 6);			//����Ǯ��,��Ϣ2 6
	memcpy(cardMessage.RecodeKEYA, keytemp, 6);			//���׼�¼	03 04 05
	memcpy(cardMessage.MonthKEYA,  keytemp, 6);			//��Ʊ���� 7 8
	memcpy(cardMessage.PublichKEYA, keytemp, 6);

	MSG_LOG(" Money KEY:\r\n");
	BCD_LOG(keytemp, 6, 1);

	return ST_OK;
}
#endif
//mode = 0x01 KEYA of user card
//mode = 0x11 KEYA of KEY card
unsigned char Cloud_miKey(unsigned char mode)
{					
	unsigned char buffer[30];
	unsigned char pos;
	unsigned char tempbuf[10];
	unsigned char Key3DES[16];	  
#ifndef BUS_YICHUN
	stCpuInfo cpuinfo;
	unsigned int Crc32, itemp;
#endif

#ifdef BUS_ANLONG_
	if(gMCardCand == CARDSTYLE_ANLONG)//�����Ͽ�
		return ANLONG_mikey();
#endif //#ifdef BUS_ANLONG_


	//(������+���д���+�̻����+"A1A2A3A4A5A6A7A8") 
	//(������+���д���+�̻�����+������ˮ+"A1A2A3A4"
	pos=0;
	memcpy(buffer+pos, (unsigned char*)&c_serial, 4); //������
	pos += 4;
	memcpy(buffer+pos, cardMessage.CityCode, CITY_CODE_LEN);	//���д���	02
	pos+=CITY_CODE_LEN;
	memcpy(buffer+pos, cardMessage.TradeCode, 2);			//�̻����	02
	pos+=2;
	if(mode == 0x01){
		memcpy(buffer+pos, cardMessage.PublishBicker, 4);
		pos+= 4;
		memcpy(buffer+pos, "\xA1\xA2\xA3\xA4", 4);
	}
	else if(mode == 0x11){
		memcpy(buffer+pos, "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8", 8);
	}
	else{
		memcpy(buffer+pos, cardMessage.PublishBicker, 4);
		pos+= 4;
		memcpy(buffer+pos, "\xA1\xA2\xA3\xA4", 4);
	}
	

	MSG_LOG("3DESGetKey\r\n");
	MSG_LOG("����:");
	BCD_LOG(buffer, 16, 1);

	if((mode&0xF0) == 0){//�����û���
#ifdef BUS_YICHUN
		memset(Key3DES, 0x5A, 16);//�˴�KEY
#else
		getCpuInfo(&cpuinfo);

		MSG_LOG("UserKeyA:");
		BCD_LOG(cpuinfo.UserKeyA, 16, 1);
		MSG_LOG("UserKeyA:");
		BCD_LOG(cpuinfo.UserKeyB, 16, 1);

		Crc32 = GenerateCRC32(cpuinfo.UserKeyA, 16);
		memcpy((unsigned char *)&itemp, cpuinfo.UserKeyACrc32, 4);
		if(Crc32 != itemp){//û��KEY
			MSG_LOG("CRC A Error:\r\n");
			BCD_LOG((unsigned char*)&Crc32, 4, 1);
			BCD_LOG(cpuinfo.UserKeyACrc32, 4, 1);
			if(gchn_eng == 'E')
				MessageBox(1, "Key Error,Please load Key!");
			else
				MessageBox(1, "����Կ��Ϣ����������Կ!");
			delayxs(1);
			restore_flag = 3;
			return ST_ERROR;
		}
		memcpy(Key3DES, cpuinfo.UserKeyA, 16);
#endif
		Lib3DESCloudKey(buffer, tempbuf, Key3DES);
		
	}
	else{//������Կ������Կ
		Lib3DESKey(buffer, tempbuf, 2);
	}

	MSG_LOG("����:");
	BCD_LOG(tempbuf, 8, 1);
	MSG_LOG("�����8�ֽڣ�ȡǰ6�ֽ�\r\n");

	memcpy(cardMessage.MoneyKEYA,  tempbuf, 6);			//����Ǯ��,��Ϣ2 6
	memcpy(cardMessage.RecodeKEYA, tempbuf, 6);			//���׼�¼	03 04 05
	memcpy(cardMessage.MonthKEYA,  tempbuf, 6);			//��Ʊ���� 7 8
	memcpy(cardMessage.PublichKEYA, tempbuf, 6);
	
	MSG_LOG(" Money KEY:\r\n");
	BCD_LOG(tempbuf, 6, 1);

	return ST_OK;	
}

unsigned char getMiKey_SLZR(void)
{					  
#ifdef _M1KEY_3DES_
	SL3DES_MiKEY();
	return ST_OK;
#elif defined _M1KEY_CLOUD_
	unsigned char i;
	if(cardMessage.card_catalog == CARD_KEY_BUS)
		i = Cloud_miKey(0x11);
	else
		i = Cloud_miKey(0x01);
	return i;
#else
	unsigned char pos;
	unsigned char buffer[30];
	unsigned char tempbuf[10];
	
//	if(Select_File(gPSAMin, 0x3f01)==0){
//		error(ERR_SELECT_MW_ADF3,0);
//		return ST_ERROR;
//	}
	
	if(gPsamErrorFlag&1){
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"error:",1);
			display(2,0,"PSAM1 error!",0);
		}
		else
		{
			display(0,0,"����:",1);
			display(2,0,"����PSAM����!",0);
		}
		restore_flag = 3;
		delayxs(1);
		return ST_ERROR;	 
	}

	MSG_LOG("get mi Key2\r\n");

	pos=0;
	memcpy(buffer+pos, cardMessage.CityCode, CITY_CODE_LEN);//���д���	02
	pos+=CITY_CODE_LEN;
	memcpy(tempbuf, (unsigned char*)&c_serial ,4);//�����к�	04

	buffer[pos++] = tempbuf[0];
	buffer[pos++] = tempbuf[1];
	buffer[pos++] = tempbuf[2];
	buffer[pos++] = tempbuf[3];

	memcpy(buffer+pos, cardMessage.PublishBicker, 2);//������ˮ(BCD����λ)	02
	pos+=2;
	memcpy(buffer+pos, cardMessage.CardCheckCode, CARD_CHECH_CODE_LEN);	//mac		04		
	pos+=CARD_CHECH_CODE_LEN;
	
	buffer[pos++]=0x02;	//����Ǯ����
	buffer[pos++]=0x03;	//Ǯ����¼��			//���׼�¼����
	buffer[pos++]=0x07;	//������Ϣ��

	pos=0;

	MSG_LOG("get Key2:\r\n");
	BCD_LOG(buffer, 15, 1);

	pos = LibGetMKey(gPSAMin, buffer, 0);

	if(pos == 0) {
#ifdef _DEBUG
		error(ERR_GET_KEY,0);
#endif
		return ST_ERROR;
	}

//	MSG_LOG("ȡ��Կ��������:\r\n");
//	BCD_LOG(30,30,buffer);

	memcpy(cardMessage.MoneyKEYA,  buffer, 6);				//����Ǯ��,��Ϣ2 6
	memcpy(cardMessage.RecodeKEYA, buffer+6, 6);			//���׼�¼	03 04 05
	memcpy(cardMessage.MonthKEYA,  buffer+12, 6);			//��Ʊ���� 7 8
	memcpy(cardMessage.PublichKEYA, buffer, 6);

	MSG_LOG(" Money KEYA:\r\n");
	BCD_LOG(cardMessage.MoneyKEYA, 6, 1);
	MSG_LOG(" Publich KEYA:\r\n");
	BCD_LOG(cardMessage.PublichKEYA, 6, 1);
	MSG_LOG(" Recode KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);
	MSG_LOG(" Month KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);

	return ST_OK;
#endif
}
unsigned char getMiKey(void)
{						
#ifdef _M1KEY_3DES_
	SL3DES_MiKEY();
	return ST_OK;
#elif defined _M1KEY_CLOUD_
	unsigned char i;
	if(cardMessage.card_catalog == CARD_KEY_BUS)
		i = Cloud_miKey(0x11);
	else
		i = Cloud_miKey(0x01);
	return i;
#else
	unsigned char pos;
	unsigned char buffer[30];
	unsigned char tempbuf[10];
	unsigned char mode;

//	if(Select_File(gPSAMin, 0x3f01)==0){
//		error(ERR_SELECT_MW_ADF3,0);
//		return ST_ERROR;
//	}
	
	if(gPsamErrorFlag&1){
		cls();
		if(gchn_eng == 'E'){
			display(0,0,"error:",1);
			display(2,0,"PSAM1 error!",0);
		}
		else
		{
			display(0,0,"����:",1);
			display(2,0,"����PSAM����!",0);
		}
		restore_flag = 3;
		delayxs(1);
		return ST_ERROR;	 
	}

	MSG_LOG("get mi Key\r\n");

	pos=0;

	memcpy(buffer+pos, cardMessage.CityCode, CITY_CODE_LEN);			//���д���	02

	pos+=CITY_CODE_LEN;
	
#ifdef NO_CPU_PSAM_
	if(gMCardCand == CARDSTYLE_CPU)
		memcpy(tempbuf, cardMessage.PublishBicker, 4);//CPU�� ������û��ȡ������ʹ�õ�����ˮ��
	else
		memcpy(tempbuf, (unsigned char*)&c_serial ,4);//�����к�	04
#else
	memcpy(tempbuf, (unsigned char*)&c_serial ,4);//�����к�	04
#endif
	
	buffer[pos++] = tempbuf[0];
	buffer[pos++] = tempbuf[1];
	buffer[pos++] = tempbuf[2];
	buffer[pos++] = tempbuf[3];

#ifdef BUS_PRO_JSB
	if(memcmp(cardMessage.TradeCode, "\xAB\xCD", 2) == 0){//���ſ���Կ����
		mode = 2;
	}
	else{
		mode = 1;
	}
#else
	mode = 0;
#endif
	memcpy(buffer+pos, cardMessage.PublishBicker, 2);					//������ˮ(BCD����λ)	02
	pos+=2;
	memcpy(buffer+pos, cardMessage.CardCheckCode, CARD_CHECH_CODE_LEN);	//mac		04		
	pos+=CARD_CHECH_CODE_LEN;
	
	buffer[pos++]=0x02;	//����Ǯ����
	buffer[pos++]=0x03;	//Ǯ����¼��			//���׼�¼����
	buffer[pos++]=0x07;	//������Ϣ��

	pos=0;

	MSG_LOG("get Key1:\r\n");
	BCD_LOG(buffer, 15, 1);
	BCD_LOG((unsigned char*)&gPSAMin, 1, 1);
	
	if(gMCardCand == CARDSTYLE_CPU){	//CPU���ڴ�ȡ��Կ
		mode=mode;
#ifdef BUS_PRO_JSB
		MSG_LOG("get Key_A:\r\n");
		pos = Get_MifKey(gPSAMin, 0x01, 0x01, 15, buffer);//JSB
#else
		MSG_LOG("get Key_B:\r\n");
		pos = Get_MifKey(gPSAMin, 0x10, 0x40, 15, buffer);//SLZR
#endif
	}
	else{	//M1��ȡ��Կ
#ifdef _old_PSAM_GET_KEY_
		MSG_LOG("get Key_C:\r\n");
		pos = Get_MifKey(gPSAMin, 0x10, 0x40, 15, buffer);//SLZR KEYA  ��������ˮ
#else
		MSG_LOG("get Key_D:\r\n");
		pos = LibGetMKey(gPSAMin, buffer, mode);
#endif
	}


// #ifdef BUS_BAOJI_//Card_typeM_Min_is0_
// 	mode=mode;
// 	pos = Get_MifKey(gPSAMin, 0x10, 0x40, 15, buffer);//SLZR KEYA
// #elif defined NO_CPU_PSAM_ 
// 	if(gMCardCand == CARDSTYLE_CPU){
// 		mode=mode;
// #ifdef BUS_PRO_JSB
// 		pos = Get_MifKey(gPSAMin, 0x01, 0x01, 15, buffer);//JSB KEYB
// #else
// 		pos = Get_MifKey(gPSAMin, 0x10, 0x40, 15, buffer);//SLZR KEYA
// #endif
// 	}
// 	else{
// 		pos = LibGetMKey(gPSAMin, buffer, mode);
// 	}
// #elif defined _XMSHDZ_24G_
// 	if(gMCardCand == CARDSTYLE_CPU){
// 		mode=mode;
// 		pos = Get_MifKey(gPSAMin, 0x10, 0x40, 15, buffer);//SLZR KEYA
// 	}
// 	else{
// 		pos = LibGetMKey(gPSAMin, buffer, mode);
// 	}
// #else
// #ifdef _old_PSAM_GET_KEY_
// 	pos = Get_MifKey(gPSAMin, 0x10, 0x40, 15, buffer);//SLZR KEYA
// #else
// 	pos = LibGetMKey(gPSAMin, buffer, mode);
// #endif
// #endif

// 	for(mode=0x01; mode<0xF0; mode++){	������
// 		pos = Get_MifKey(gPSAMin, 0x01, mode, 15, buffer);//JSB KEYB
// 		if(pos != 0)
// 			break;
// 		MSG_LOG("SW:");
// 		BCD_LOG(buffer, 2, 1);
// 		sprintf((char*)tempbuf, "%02X", mode);
// 		display(0,0,(char*)tempbuf, 0);
// 	}
	if(pos == 0) {
		sprintf((char*)tempbuf, "%02X%02X", buffer[0], buffer[1]);
		miniDispstr(6,0,(char*)tempbuf, 0);
		return ST_ERROR;
	}

//	MSG_LOG("ȡ��Կ��������:\r\n");
//	BCD_LOG(30,30,buffer);

	memcpy(cardMessage.MoneyKEYA,  buffer, 6);				//����Ǯ��,��Ϣ2 6
	memcpy(cardMessage.RecodeKEYA, buffer+6, 6);			//���׼�¼	03 04 05
	memcpy(cardMessage.MonthKEYA,  buffer+12, 6);			//��Ʊ���� 7 8
	memcpy(cardMessage.PublichKEYA, buffer, 6);

	MSG_LOG(" Money KEYA:\r\n");
	BCD_LOG(cardMessage.MoneyKEYA, 6, 1);
	MSG_LOG(" Publich KEYA:\r\n");
	BCD_LOG(cardMessage.PublichKEYA, 6, 1);
	MSG_LOG(" Recode KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);
	MSG_LOG(" Month KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);
	return ST_OK;
#endif
}
void save_h_month(void)
{
	unsigned char ii=0;
	unsigned char mcount;
	unsigned char temp[100];


	MSG_LOG("do %s\r\n",__FUNCTION__);
	
	if(MothDelayTime.endptr >= LAST_LIST_LEN)
		MothDelayTime.endptr = LAST_LIST_LEN-1;
	if(MothDelayTime.startptr >= LAST_LIST_LEN)
		MothDelayTime.startptr = 0;
	mcount=0;
	if(MothDelayTime.endptr >=MothDelayTime.startptr)										//����û����
	{
		for(ii=MothDelayTime.startptr;ii<MothDelayTime.endptr ;ii++)
		{
			if(!memcmp(MothDelayTime.LastList[ii],(unsigned char *)&c_serial,4))
			{
				memset(MothDelayTime.LastList[ii],0,7);
				temp[mcount]=ii;
				mcount++;
			}
		}
	}
	else 
	{
		for(ii=MothDelayTime.startptr;ii<LAST_LIST_LEN ;ii++)								//��������������
		{
			if(!memcmp(MothDelayTime.LastList[ii],(unsigned char *)&c_serial,4))
			{
				memset(MothDelayTime.LastList[ii],0,7);
				temp[mcount]=ii;
				mcount++;
			}
		}
		for(ii=0;ii<MothDelayTime.endptr;ii++)
		{
			if(!memcmp(MothDelayTime.LastList[ii],(unsigned char *)&c_serial,4))
			{
				memset(MothDelayTime.LastList[ii],0,7);
				temp[mcount]=ii;
				mcount++;
			}
		}
	}

	for(ii=0;ii<mcount;ii++)
		sysfewrite(BIT_INTERVAL_DATA+(temp[ii]*7),7,MothDelayTime.LastList[temp[ii]]);//�Ѹ���д������

	memcpy(MothDelayTime.LastList[MothDelayTime.endptr],(unsigned char *)&c_serial,4);
	
	MothDelayTime.LastList[MothDelayTime.endptr][4]=SysTime.day;
	MothDelayTime.LastList[MothDelayTime.endptr][5]=SysTime.hours;
	MothDelayTime.LastList[MothDelayTime.endptr][6]=SysTime.minutes;

	sysfewrite(BIT_INTERVAL_DATA+(MothDelayTime.endptr*7),7,MothDelayTime.LastList[MothDelayTime.endptr]);
	
	MothDelayTime.endptr++;
	
	if(MothDelayTime.endptr==LAST_LIST_LEN)
		MothDelayTime.endptr=0; 
	
	if(MothDelayTime.startptr==MothDelayTime.endptr)
		MothDelayTime.startptr++;
	if(MothDelayTime.startptr==LAST_LIST_LEN)
		MothDelayTime.startptr=0;

	sysfewrite(BIT_INTERVAL_STAR,1,&MothDelayTime.startptr);		//д������
	sysfewrite(BIT_INTERVAL_END,1,&MothDelayTime.endptr);
}

unsigned int cal_space(void)
{
	int headp, curp;

	curp = Get_Record_point((unsigned char*)&headp, 0x5a);
	if(curp == -2)
		return 0;										
	else 
		return 0xff;
}

unsigned char month_decide(void)
{
	unsigned char i=0;
	unsigned char buffer[3];
	unsigned char temp=0;
	int summin=0;
	int sT1,sT2;
	unsigned char MWTime;
	//clr_dog();
	//get_date(buffer);
	temp=SysTime.day;

	//get_time(buffer);
	memcpy(buffer,&SysTime.hours,3);
	buffer[2]=temp;

	MWTime = Waittime();
	if(MWTime == 0)
		return 0;

	MSG_LOG("MWTime:%d\r\n", MWTime);
	MSG_LOG("month_decide::c_serial:%08X\r\n", c_serial);

	sT1 = (((buffer[0]>>4)*10+(buffer[0]&0x0f))*60+(buffer[1]>>4)*10+(buffer[1]&0x0f));

	if(MothDelayTime.endptr>=MothDelayTime.startptr)
	{
		for(i=MothDelayTime.startptr;i<MothDelayTime.endptr;i++)
		{
// 			MSG_LOG("M111othDelayTime.LastList:");
// 			BCD_LOG(MothDelayTime.LastList[i], 7, 1);

			if(!memcmp(MothDelayTime.LastList[i],(unsigned char *)&c_serial,4))
			{
				if(buffer[2]!=MothDelayTime.LastList[i][4])
					continue;
				else
				{
					sT2 = (((MothDelayTime.LastList[i][5]>>4)*10+(MothDelayTime.LastList[i][5]&0x0f))*60+
							(MothDelayTime.LastList[i][6]>>4)*10+(MothDelayTime.LastList[i][6]&0x0f));
					summin = sT1 - sT2;
					if(summin<MWTime)
						return 1;
				}
			}
		}
	}
	else
	{
		for(i=MothDelayTime.startptr;i<50;i++)
		{
// 			MSG_LOG("M222othDelayTime.LastList:");
// 			BCD_LOG(MothDelayTime.LastList[i], 7, 1);

			if(!memcmp(MothDelayTime.LastList[i],(unsigned char *)&c_serial,4))
			{
				if(buffer[2]!=MothDelayTime.LastList[i][4])
					continue;
				else
				{
					sT2 = (((MothDelayTime.LastList[i][5]>>4)*10+(MothDelayTime.LastList[i][5]&0x0f))*60+
						(MothDelayTime.LastList[i][6]>>4)*10+(MothDelayTime.LastList[i][6]&0x0f));
					summin = sT1 - sT2;
					if(summin<MWTime)
						return 1;
				}
			}

		}
		for(i=0;i<MothDelayTime.endptr;i++)
		{
// 			MSG_LOG("M333othDelayTime.LastList:");
// 			BCD_LOG(MothDelayTime.LastList[i], 7, 1);

			if(!memcmp(MothDelayTime.LastList[i],(unsigned char *)&c_serial,4))
			{
				if(buffer[2]!=MothDelayTime.LastList[i][4])
					continue;
				else
				{
					sT2 = (((MothDelayTime.LastList[i][5]>>4)*10+(MothDelayTime.LastList[i][5]&0x0f))*60+
						(MothDelayTime.LastList[i][6]>>4)*10+(MothDelayTime.LastList[i][6]&0x0f));
					summin = sT1 - sT2;
					if(summin<MWTime)
						return 1;
				}
			}

		}
	}
	return 0;
}
#ifdef _TEST_YANPIAO_
///////////////////////////ƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱ
void disp_dipsj(void)
{
	cls();
	display(0,0,"�� Ʊ ϵ ͳ", DIS_CENTER|DIS_CONVERT);
	display(3, 0, "��ˢ��!", DIS_CENTER);
}
#endif

void restore_disp(void)
{
#ifdef FengDuan_BU_
	FengDuan_disp();
#else	//#ifdef FengDuan_BU_
#ifdef _GPS_DIAODU_
	unsigned char zline[10];
#endif
#ifdef BUS_YICHUN
	unsigned char ccb[16];
#endif

	unsigned char buffer[50];
	unsigned int mmoney;
	unsigned int imm,pos;
	unsigned short ii=0;
#ifdef MOBILE_SHIYAN_
	unsigned short uiTemp;
#endif

#ifdef _TEST_YANPIAO_
	///////////////////////////ƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱ
	disp_dipsj();
	return;
#endif
	ii=ii;


#ifdef _GPS_ENABLE_
#ifdef _GPS_CAIDIAN_
	cls();
	display(0,0," �� �� �� ", DIS_CENTER|DIS_CONVERT);
	return;
#endif
#ifdef _GPS_TRA_REC_
	cls();
	display(0,0," ��¼�켣 ", DIS_CENTER|DIS_CONVERT);
	gGpsDisEN = 'G';
#endif
	if(gGpsDisEN == 'G'){
		if((restore_flag&1) != 0){
// 			strcpy((char*)buffer, SYS_HEAD_STR);
// 			memcpy((unsigned char*)&ii, gBusVerInfo.busProVer, 2);
// 			sprintf((char*)buffer+30, " V%d.%02d", (ii/100), (ii%100));
// 			strcat((char*)buffer, (char*)(buffer+30));
			cls();
// 			if(gchn_eng == 'E')
// 				display(0,0,SYS_HEAD_STREn, DIS_CONVERT|DIS_CENTER);
// 			else
// 				display(0,0,(char*)buffer, DIS_CONVERT|DIS_CENTER);
		}
		return;
	}
#endif

	if((restore_flag&1) != 0){
		cls();
//		pos = 0;
		memset(buffer,0,50);

		strcpy((char*)buffer, Conn(_Font_Line_Str,":"));
		pos=strlen((char*)buffer);


		BCD2Ascii(pFistVary.LineNo, buffer+pos, 2);
		pos+=4;
		buffer[pos]='-';
		pos+=1;

		BCD2Ascii(pFistVary.LineNo+2, buffer+pos, 1);
		pos+=2;

		memcpy(buffer+pos, " ����:", 6);
		pos+=6;
		memcpy(buffer+pos, pFistVary.DeviceNo, 8);
		pos+=8;

		display(3,1,(char*)buffer,DIS_CENTER);


		memset(buffer,0,50);
		getProVer();

		strcpy((char*)buffer, _Font_Main_title);

#ifdef _DriverKey_signin_
		if(BUflag != 0){
			display(0,0,"��ˢ˾����", DIS_CONVERT|DIS_CENTER);
		}
		else
#endif
			display(0,0,(char*)buffer, DIS_CONVERT|DIS_CENTER);
		
		memcpy((unsigned char*)&ii, gBusVerInfo.busProVer, 2);
#if 0
		sprintf((char*)buffer, "V%d.%02d", (ii/100), (ii%100));
#else
		sprintf((char*)buffer, "V%04X", ii);		//PFQUAN  X�µ�ͨѶƽ̨
		
#endif
		miniDispstr(15, 0, (char*)buffer, 0);

		if(F_FLAG_VARY.stop_flag)
		{
			memset(buffer,0,50);
			display_3232(6,4,_Font_PauseCard_Str,DIS_CENTER);
			LED_Dis3("------");
#ifdef KEYBOARD
			SendCommandkey('C','0');
#endif
		}
		else
		{
			imm = 0;
			memset(buffer,0,50);
			getCpuPrice();
			memcpy((unsigned char*)&mmoney, NewPriceTab.busPrice, 4);
			if(F_FLAG_VARY.PlusSta){
				memcpy((unsigned char*)&imm, NewPriceTab.plusPrice, 4);
			}
			mmoney += imm;			
			sprintf((char*)buffer, "%d.%02d"/*_Font_Fare_Dis*/, mmoney/100, mmoney%100);
			display_AsciiDigit(5,1,(char*)buffer,DIS_CENTER, Dis_Font64);//display_3232(6,1,(char*)buffer,DIS_CENTER);
			display_3232(7, 14+(2*strlen((char*)buffer))+(strlen((char*)buffer)%2), "Ԫ", 0); //64Ҫ��2��32�Ĳ��óˣ�48��..
#ifdef KEYBOARD
			sprintf((char*)buffer, "---%d.%02d", mmoney/100, mmoney%100);
			LED_Dis3((char*)buffer);
			if(mmoney == 0)
				display(12,0,"������Ʊ��",DIS_CENTER|DIS_ClsLine);
#else
			sprintf((char*)buffer, "---%d.%02d", mmoney/100, mmoney%100);
			LED_Dis3((char*)buffer);
#endif
#ifdef BUS_YICHUN
			memcpy((unsigned char*)&imm, DriveCardNo, 4);
			sysferead(BIT_GPS_MESSAGE, 10, ccb);
			if(ccb[0] == RECORD_DEAL){
				sprintf((char*)&buffer, "ID:%02X%02X%02X->", StufferNO[1],StufferNO[2],StufferNO[3]);
			}
			else{
				sprintf((char*)&buffer, "ID:%02X%02X%02X", StufferNO[1],StufferNO[2],StufferNO[3]);
			}
			miniDispstr(13, 6, (char*)buffer, 0);
#elif defined BUS_Cloud_
#ifdef _Counter_SWcard_times
			ii = getSCtimes(0);
			imm = getSCtimes(1);
			sprintf((char*)&buffer, "SC:%d/%d  ", ii, imm);
#ifdef _GPS_ENABLE_
			miniDispstr(13, 8, (char*)buffer, 0);	
#else
			miniDispstr(13, 2, (char*)buffer, 0);
#endif	//#ifdef _GPS_ENABLE_
// #else
// 			sprintf((char*)&buffer, "ID:%02X%02X%02X", StufferNO[1],StufferNO[2],StufferNO[3]);
#endif	//#ifdef _Counter_SWcard_times
#endif	//#elif defined BUS_Cloud_
		}
#ifdef BUS_GUANGXI
		getposSN(irda_rxBuf, 0);
		irda_rxBuf[16] = 0;		
		pFistVary.DeviceNo[8] = 0;
		sprintf((char*)buffer, "�豸:%s-%s", (char*)irda_rxBuf+8, (char*)pFistVary.DeviceNo);//���к�ȡ��8λ		
		display(12,1,(char*)buffer,DIS_CENTER);
#endif
	}
// #ifdef _GPRS_Zhuangwang_
// 	miniDispstr(0, 21, "Z", DIS_RIGHT);
// #elif defined BUS_HANDAN_
// 	miniDispstr(0, 21, "N", DIS_RIGHT);
// #endif

#endif //#ifdef FengDuan_BU_
#ifdef _debug_
	if (KeyBoardStyle == KEYBOARD_INPUT)		//���Ͻ� 
		display(0, 26, (char *)"F",0);
	else
		display(0, 26, (char *)"G",0);
#else
	if (KeyBoardStyle == KEYBOARD_INPUT)		//���Ͻ� 
		display(0, 28, (char *)"F",0);
	else
		display(0, 28, (char *)"G",0);

#endif
#ifdef qPBOC_BUS
		
		if (Sign_Infor.ISOK ==0)
		{
			memset(buffer,0,50);
			display(12,4,"��������ǩ��",DIS_CENTER);
			
		}
		else
		{
		//	memset(buffer,0,50);
		//	display(12,4,"�Ƽ�ʹ������������",DIS_CENTER);
		}
#endif
}
unsigned char CheckSno(unsigned char mode,unsigned char ID,unsigned char *ptr)
{
	unsigned char i=0,temp=0;
	unsigned pos=ST_OK;
	unsigned char buffer[32];
	//clr_dog();

	temp=error_pointer;
	for(i=0;i<error_pointer;i++)
	{
		if((mode==1)||(mode==ERROR_MONTH)){
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONTH) continue;
		}
		else if((mode==0)||(mode==ERROR_MONEY)){
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONEY) continue;
		}
		else{
			if(ErrorDetail[i].effFlag!=IS_EFFECT) continue;
		}

		if(memcmp(ErrorDetail[i].Cardno,(unsigned char *)&c_serial,4)==0)
		{
			if(ErrorDetail[i].ErrorStep!=ID)
			{
				error_pointer=i;
				pos=ST_ERROR;
				break;
			}
			return ST_ERROR;
		}
	}

	if(error_pointer==ERROR_DETAIL_LEN)
	{
		for(i=0;i<ERROR_DETAIL_SUM-1;i++)
			memcpy((unsigned char *)&ErrorDetail[i],(unsigned char *)&ErrorDetail[i+1],ERROR_DETAIL_LEN);
		error_pointer=(ERROR_DETAIL_SUM-1);
	}
	
	if((mode==1)||(mode==ERROR_MONTH))
	{
		ErrorDetail[error_pointer].effFlag=IS_EFFECT_MONTH;
		if(pos==ST_OK)
		{
			memcpy(ErrorDetail[error_pointer].TollValue,(unsigned char *)&s_sum1,4);
			memcpy(ErrorDetail[error_pointer].AferToll,(unsigned char *)&a_sum1,4);
		}
	}
	else if((mode==0)||(mode==ERROR_MONEY))
	{
		
		ErrorDetail[error_pointer].effFlag=IS_EFFECT_MONEY;
		if(pos==ST_OK)
		{
			memcpy(ErrorDetail[error_pointer].TollValue,(unsigned char *)&s_sum1,4);
			memcpy(ErrorDetail[error_pointer].AferToll,(unsigned char *)&a_sum1,4);
		}
	}
	else
		ErrorDetail[error_pointer].effFlag=IS_EFFECT;

	switch(ID)
	{
	case OPER_BLACK:
	case OPER_PUBLIC_MESSAGE:
	case OPER_RECORD_PROFILE:
		memcpy(ErrorDetail[error_pointer].ToChange,ptr,16);	
		break;
	default:
		break;
	}
	memcpy(ErrorDetail[error_pointer].Cardno,&c_serial,4);
	
	ErrorDetail[error_pointer].ErrorStep=ID;
	if(pos==ST_OK)
		error_pointer+=1;
	else
		error_pointer=temp;

	//д����������FE2PROM��
	for(i=0;i<ERROR_DETAIL_SUM;i++)
	{
		memset(buffer,0,32);
		memcpy(buffer,(unsigned char *)&ErrorDetail[i],ERROR_DETAIL_LEN);
		sysfewrite(BIT_ERROR_DETAIL+i*ERROR_DETAIL_LEN,ERROR_DETAIL_LEN,buffer);
	}
	memset(buffer,0,2);
	buffer[0]=error_pointer;
	sysfewrite(BIT_ERROR_POINT,1,buffer);
	return pos;
}

void ErrorOper(unsigned char flag)
{
	unsigned char i=0,ii=0;
	unsigned char buffer[32];
	//clr_dog();
	for(i=0;i<error_pointer;i++)
	{
		if(ErrorDetail[i].effFlag!=flag) continue;
		else
		{
			if(memcmp(ErrorDetail[i].Cardno,(unsigned char *)&c_serial,4)==0)
			{
				for(ii=i;ii<error_pointer;ii++)
					memcpy((unsigned char *)&ErrorDetail[ii],(unsigned char *)&ErrorDetail[ii+1],ERROR_DETAIL_LEN);
				error_pointer-=1;
				memset((unsigned char *)&ErrorDetail[error_pointer],0,ERROR_DETAIL_LEN);
			}
		}
	}
	//д����������FE2PROM��
	for(i=0;i<ERROR_DETAIL_SUM;i++)
	{
		memset(buffer,0,32);
		memcpy(buffer,(unsigned char *)&ErrorDetail[i],ERROR_DETAIL_LEN);
		sysfewrite(BIT_ERROR_DETAIL+i*ERROR_DETAIL_LEN,ERROR_DETAIL_LEN,buffer);
	}
	
	memset(buffer,0,2);
	buffer[0]=error_pointer;
	sysfewrite(BIT_ERROR_POINT,1,buffer);
}

//��ƱǮ�����״���
unsigned char PurseProcessYear(unsigned char mode)
{
	unsigned char i=0;
	unsigned char buffer[60];
	unsigned char buffer_9[16],buffer_10[16];
	unsigned long temp1, temp;
	unsigned char moneyflag=0;
//Ǯ������
	MSG_LOG("Year Purse Process_1\n");
	if(MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MonthKEYA,cardMessage.dodMonthSector*4)==0)
	{
		return ST_ERROR; 
	}
	
	MSG_LOG("Year Purse Process	3\n");

	//����ƱǮ������
	if(MifareRead(cardMessage.dodMonthSector*4+1, buffer_9)==0){
#ifdef _DEBUG
		error(ERR_MIFARE_READ, cardMessage.publicMoneySector*4+1);
#endif
		return ST_ERROR;
	}

	MSG_LOG("mm1:");
	BCD_LOG(buffer_9, 16, 1);

	if(MifareRead(cardMessage.dodMonthSector*4+2, buffer_10)==0){
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.publicMoneySector*4+2);
#endif
		return ST_ERROR;
	}

	MSG_LOG("mm2:");
	BCD_LOG(buffer_10, 16, 1);
	
	if(MifIsPurse(buffer_9)==ST_ERROR)  //Ǯ������У��
	{
		gucRestorFlag |= RES_MONEY_Z;
#if(0)
		MSG_LOG("Year Money1 check is error!\r\n");
#endif
		
	}
	if(MifIsPurse(buffer_10)==ST_ERROR)    //Ǯ������У��
	{
		gucRestorFlag |= RES_MONEY_F;
#if(0)
		MSG_LOG(" Money2 check is error!\r\n");
#endif
	}
	if((gucRestorFlag & RES_MONEY_Z) && (gucRestorFlag & RES_MONEY_F))
		return RET_MONEY_SN_EROR;
	memcpy((unsigned char*)&temp ,buffer_9,4);
	memcpy((unsigned char*)&temp1 ,buffer_10,4);
	if(temp !=temp1)
	{
		gucRestorFlag |= RES_MONEY_D;
	}

	MSG_LOG("Year gucRestorFLag:");
	BCD_LOG((unsigned char*)&gucRestorFlag,1,1);

	if(gucRestorFlag & RES_PUBLIC_FLAG){
		if((gucRestorFlag & RES_MONEY_D) == 0){	//Ǯ����Ǯ��
			memcpy(buffer, (unsigned char*)&CARD_PUBLIC1, 8);
			for(i=0;i<8;i++){
				buffer[i] = ~buffer[i];//����
			}
			memcpy((unsigned char*)&CARD_PUBLIC, buffer, 8);
			moneyflag = 0;	//Ǯ��
		}
		else if((gucRestorFlag & RES_MONEY_D) && (gucRestorFlag & RES_MONEY_Z)){//Ǯ������ȷ
			memcpy(buffer, (unsigned char*)&CARD_PUBLIC1, 8);
			for(i=0;i<8;i++){
				buffer[i] = ~buffer[i];//����
			}
			memcpy((unsigned char*)&CARD_PUBLIC, buffer, 8);
			moneyflag = 1;	//Ǯ��
		}
		else if((gucRestorFlag & RES_MONEY_D) && ((gucRestorFlag & RES_MONEY_Z) == 0)){
			if(cardSound==0xaa){
				a_sum1 = temp;
				if(temp1 > temp)
					s_sum1 = temp1 - temp;
				goto Continue_money_puchase;
			}
			else 
				moneyflag = 1;//Ǯ��
		}
	}
	else{
		if(cardSound==0xaa){
			a_sum1 = temp;
			if(temp1 > temp)
				s_sum1 = temp1 - temp;
			goto Continue_money_puchase;
		}
		if(gucRestorFlag & RES_MONEY_Z){
			moneyflag = 1;//Ǯ��
		}
		else{
			moneyflag = 0;//Ǯ��
		}
	}
	if(gucRestorFlag & RES_PUBLIC_Z){
		memcpy(buffer, (unsigned char*)&CARD_PUBLIC1, 8);
		for(i=0;i<8;i++){
			buffer[i] = ~buffer[i];//����
		}
		memcpy((unsigned char*)&CARD_PUBLIC, buffer, 8);
	}

	MSG_LOG("Year CARD_PUBLIC:");
	BCD_LOG((unsigned char*)&CARD_PUBLIC,8,1);

	//���������Ϊ�����Ȼָ�Ǯ������
	if((moneyflag == 0) && (gucRestorFlag & RES_MONEY_D)){//���������ȣ��Ȱ�Ǯ���������ݵ������ٿ�Ǯ
		MSG_LOG("�ָ�����...\r\n");
	
		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MonthKEYA,cardMessage.dodMonthSector*4)==0)
		{
			//		MSG_LOG("MifareAuthKey error\r\n ");
			return ST_ERROR; 
		}
		memset(buffer+50, 0, 4);
		for(i=0;i<3;i++){
			if(MifareValueDebit(PICC_DECREMENT, cardMessage.dodMonthSector*4+1, buffer+50))	//change Ǯ��
				break;
		}
		if(i==3)																			//ʧ��
			return ST_ERROR;
		if(MifareTransfer(cardMessage.dodMonthSector*4+2)==0)
			return ST_ERROR;
	}

	if(mode == 100)
		return ST_OK;
//--------д���̱�־1--------

	MSG_LOG("Year public d:");
	BCD_LOG((unsigned char*)&CARD_PUBLIC, 8, 1);

	if(PurchasOk_Money(0) == ST_ERROR)
		return ST_ERROR;
	
	if(moneyflag == 0)
		memcpy((unsigned char*)&a_sum1, buffer_9, 4);
	else
		memcpy((unsigned char*)&a_sum1, buffer_10, 4);

	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MonthKEYA,cardMessage.dodMonthSector*4)==0)
	{
		//		MSG_LOG("MifareAuthKey error\r\n ");
		return ST_ERROR; 
	}

	s_sum1=1; //��ƱǮ����һ��ֻ��1
	//�û�ԭ��
//	memcpy((unsigned char *)a_sum1.charbuffer, (unsigned char *)buffer_9, 4);			//�������

	MSG_LOG("��ԭ��:%u\r\n", a_sum1);
	
	//CardPro_100:
	//	if((a_sum1.longbuffer>(4*s_sum1.longbuffer))&&(a_sum1.longbuffer<(6*s_sum1.longbuffer))) 
//	if((a_sum1.longbuffer<300) && (a_sum1.longbuffer>=s_sum1.longbuffer))				//С��3Ԫ�򾯸�
//		Audio(CARD_FULLMONEY_SOUND);													//���ֵ

	MSG_LOG("s_sum1=%u\r\n", s_sum1);

	if (a_sum1 >= 1000000) 
	{
		return 21;
	}

	if(a_sum1 >= s_sum1)
		a_sum1 -= s_sum1 ;										//�������
	else{
// 		SoundMessage(SOUND_FLING_MONEY);//��Ͷ��,����Ҫȥ��Ǯ�ģ��������ﲻ����ʾ
// 		MessageBox(0, "����!");
		return 3;
	}
	
	memcpy(buffer_9+4, (unsigned char*)&s_sum1, 4);
	
	if(mode == 100)
		return ST_OK;

#ifdef _debugif
	MSG_LOG("s_sum1:");
	BCD_LOG(buffer_9+4, 4, 1);
#endif
	memcpy((unsigned char *)buffer, (unsigned char *)buffer_9+4, 4);					//���ѽ��
	if(moneyflag == 0){
		for(i=0;i<3;i++){
			if(MifareValueDebit(PICC_DECREMENT, cardMessage.dodMonthSector*4+1, buffer))	//change Ǯ��
				break;
		}
		if(i==3)																			//ʧ��
			return ST_ERROR;
	}
	else{
		for(i=0;i<3;i++){
			if(MifareValueDebit(PICC_DECREMENT, cardMessage.dodMonthSector*4+2, buffer))	//change Ǯ��
				break;
		}
		if(i==3)																			//ʧ��
			return ST_ERROR;
	}
	
	MifareTransfer(cardMessage.dodMonthSector*4+1);
	for(i=0; i<3; i++)
	{
		if(MifareRead(cardMessage.dodMonthSector*4+1, buffer))
		{
			if(memcmp((unsigned char *)buffer, (unsigned char *)&a_sum1, 4)==0){
				if(MifIsPurse(buffer)==ST_OK)
				{
					goto Continue_money_puchase;
				}
			}
		}
	}
	return 6;
Continue_money_puchase:
	//--------д���̱�־2--------
	MSG_LOG("д���̱�־2\r\n");

	if(PurchasOk_Money(1) == ST_ERROR)
		return 6;
	
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MonthKEYA,cardMessage.dodMonthSector*4)==0)
	{
		//		MSG_LOG("MifareAuthKey error\r\n ");
		return ST_ERROR; 
	}

	for(i=0;i<3;i++)
	{
		memset(buffer, 0, 4);
		if(MifareValueDebit(PICC_DECREMENT, cardMessage.dodMonthSector*4+1, buffer))
			goto mey_step9a;
	}
	return 7;
mey_step9a:
	for(i=0;i<1;i++)
	{
		if(MifareTransfer(cardMessage.dodMonthSector*4+2))
			break;
	}
	for(i=0;i<3;i++)
	{
		if(MifareRead(cardMessage.dodMonthSector*4+2,buffer+50))
		{
			if(memcmp(buffer_9,buffer+50,16)==0)
			{
//					MSG_LOG("xxxx11��\r\n");
//					BCD_LOG(16, 16, buffer_9);
//					BCD_LOG(16,16, buffer+50);
				return 5;
			}
		}
	}
	return 8;
}


extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
//��ƽ̨Ʊ�۴���
unsigned char checkNewPrice_new(void)
{
	unsigned int i = 0, addr;
	stLoadPtable tempPrice;
	unsigned char cnpBuf[16];
	unsigned char buff_voice[32];
	typedef struct {

		unsigned char tcardrate;		//�ۿ�1(1B)
		unsigned char tlianshua;	//�����ۿ�

	}loadRate;
	loadRate *larp;
	unsigned char j;

	//�����µ�Ʊ����Ϣ
#if defined _debug_sys_info_ ||defined _debug_price
	debugstring("NEW PRICE\r\n");
#endif
	GetDateTime();

#ifdef _debug_price
	debugdata((unsigned char*)&SysTime, 7, 1);
#endif

	i = FLASH_TEMP_START;
	flashread(i, (unsigned char*)&tempPrice, sizeof(stLoadPtable));

#ifdef _debug_price
	debugstring("�µ�Ʊ����Ϣ:");
	debugdata((unsigned char*)&tempPrice, sizeof(stLoadPtable), 1);
#endif




	if (memcmp(pFistVary.LineNo, tempPrice.LineNo, 2) != 0) {
#ifdef _debug_price
		debugstring("  ������·:");
		debugdata(pFistVary.LineNo, 3, 1);
		debugdata(tempPrice.LineNo, 4, 1);
#endif
		memcpy(pFistVary.LineNo, tempPrice.LineNo, 3);
		//pFistVary.LineNo[2] = 1;
	}



	//	memcpy(gBusTicketV.busticketVer, tempPrice.lineVer, 2);
	memset(NewPriceTab.busPrice, 0, 4);
	memcpy(NewPriceTab.busPrice, tempPrice.busPrice, 2);
	//over_turn(4, NewPriceTab.busPrice);
	memset(NewPriceTab.plusPrice, 0, 4);
	memcpy(NewPriceTab.plusPrice, tempPrice.plusPrice, 2);
	//over_turn(4, NewPriceTab.plusPrice);
	// debugdata(NewPriceTab.busPrice,4,1);

	memset(NewPriceTab.rate, 204, 32);//��ֹ����

	larp = (loadRate*)cnpBuf;
	addr = FLASH_TEMP_START + sizeof(stLoadPtable);	//��ͷ��Ϣ����Ǹ������ۿ���
	for (j = 1; j < 32; j++)		////ȡ����������ۿ���
	{

		flashread(addr, cnpBuf, 2);
#ifdef _debug_price
		debugstring("cnpbuf:");
		debugdata(cnpBuf, 2, 1);
#endif				
		addr += 2;
		NewPriceTab.rate[j] = larp->tcardrate;
		NewPriceTab.plusrate[j] = larp->tlianshua;

	}
	//����
	flashread(addr, buff_voice, 32);

	memcpy(NewPriceTab.voice, buff_voice, 32);

	sysfewrite(BIT_PRICETAB, sizeof(stPricetable), (unsigned char*)&NewPriceTab);
	savesysbupara();
		MSG_LOG("xxxxxxxx-NewPriceTab.lineVer:");
			BCD_LOG(NewPriceTab.lineVer, 2, 1);
	saveCpuInfo(1, cnpBuf);
	i = FLASH_TEMP_START;
	flasherase(ERASE4KBYTESECTOR, i);//�����ʱ��

	return 1;
}
extern unsigned char CardType; //�������� ����/���
unsigned int get_s_sum1(unsigned char mode)
{
	unsigned int temp_s, i;
	int price=0,plus=0;
	unsigned char price_ratio,plus_ratio;
	unsigned char ctype;
	
	getCpuPrice();
	getCpuRate();

	MSG_LOG("do %s\r\n",__FUNCTION__);

#ifdef BUS_NanjingDZ_	//��ͨ�������������ڶ���ˢ����ȫ��
	if((cardMessage.card_catalog == CARD_NORMAL_BUS)||(cardMessage.card_catalog == CARD_ZILU_BUS)){
		if(month_decide()){
			memcpy((unsigned char*)&price, NewPriceTab.busPrice, 4);	//Ʊ��
			return price;
		}

	}
#endif
	/*
#ifdef BUS_GUANGXI
	if(CardType==autoregionCard)	//��������	
	{
	}
	else if(CardType==YiDiCard)		//��ؿ�
	{}
	else						
	{}
#endif
 */



	memcpy((unsigned char*)&price, NewPriceTab.busPrice, 4);	//Ʊ��
	memcpy((unsigned char*)&plus, NewPriceTab.plusPrice, 4);	//����Ʊ��
#ifdef FengDuan_BU_
	memcpy((unsigned char*)&price,(unsigned char*)&s_sum1,4); //ȡ�ۿ�
#endif
#ifdef BUS_PRO_JTB
	if((gMCardCand == CARDSTYLE_JTB) &&(CardType==YiDiCard))//��ؿ� ���Ż�
	{
		MSG_LOG("��ؽ�ͨ������-\r\n");
		return price;
	}
#endif


	
	

#ifdef BUS_MZT	//��ͨ�������������ڶ���ˢ����ȫ��

	ctype = cardMessage.card_catalog;
	
	if (cardMessage.Zicard_catalog &0x80)
	{
		if(ctype >= 0x40)
			ctype = ctype-0x40;
		
	}
		
	

#ifndef MZT_ST  //��̨Ҫ�����⣬Ҫ�����꿨 zѧ����ÿ��ˢ���������꿨�ۿۣ�û��ʱ������ 
	MSG_LOG("ctype:%02X\r\n",ctype);
	if ((ctype !=CARD_NORMAL_BUS)&&\
		(ctype !=CARD_QR_BUS)&&\
		(ctype !=CARD_QPBOC_BUS)&&\
		(ctype !=CARD_UNION_BUS)
		)
	{

		if(month_decide()){
			MSG_LOG("�ڶ���,����ͨ���ۿ���--\r\n");
			ctype = 1;		//����ͨ��

			second_money_flag = 0x5a;
		}
	}
#endif 

#endif



	if(ctype >= 0x40)
		ctype = ctype-0x40;
	else
		ctype = ctype;



	price_ratio = NewPriceTab.rate[ctype];			//Ʊ���ۿ�
	plus_ratio = NewPriceTab.plusrate[ctype];//���и��ӷ�ʱ�����������ۿ���
	
	MSG_LOG("Ctype0:%02X nprice:%d price_ratio:%d plus_ratio:%d plus:%d\r\n", ctype, price,price_ratio, plus_ratio, plus);

	if(price_ratio > 100){//�������Ʊ����������ۿ�һ���Ǵ���0��
		ctype = CARD_NORMAL;
		if(ctype >= 0x40)
			ctype = ctype-0x40;
		price_ratio = NewPriceTab.rate[ctype];			//Ʊ���ۿ�
	}

	if(price_ratio>100){
		price_ratio = 100;
	//	return 0xFFFFFFFF;
	}
#ifndef NEED_AIR_BUS_CARD //����Ҫˢ�յ����ĵط�û�и����ۿ�,�����ۿ۶�Ϊ100
	plus_ratio = 100;
#endif

	MSG_LOG("Ctype0:%02X nprice:%d price_ratio:%d plus_ratio:%d plus:%d\r\n", ctype, price,price_ratio, plus_ratio, plus);

	//���Ʊ��
	//�������Ʊ��Ǯ����ͨ�����ۿ�
	temp_s = price * price_ratio /10;
	i = temp_s%10;
	temp_s /= 10;
	if(i > 5)	//��������
		temp_s++;
	
	if((plus_ratio>0)&&(plus_ratio<=100))
	{
		if(F_FLAG_VARY.PlusSta)
			temp_s += plus*plus_ratio/100;
	}

	MSG_LOG("temp_s:%d Ԫ\r\n", temp_s);
// #ifdef _debug_
// 	temp_s = 1;
// #endif

	MSG_LOG("temp_s:%d Ԫ\r\n", temp_s);

	return temp_s;
}

unsigned int get_Month_s_sum1(void)
{
	unsigned int temp_s;
	
#ifdef BUS_Cloud_	  
	unsigned char ctype;
	unsigned int i;
#endif
									
	temp_s = 1;
#ifdef BUS_Cloud_
	if(cardMessage.card_catalog >= 0x40)
		ctype = cardMessage.card_catalog-0x40;
	else
		ctype = cardMessage.card_catalog;

	memcpy((unsigned char*)&i, NewPriceTab.plusMonthPrice, 4);

	MSG_LOG("get_Month_s_sum1 0:");
	BCD_LOG(NewPriceTab.plusMonthPrice, 4, 1);

	if(NewPriceTab.rate[ctype] == 101){
		if(i < 5)
			temp_s += i; //�Ӹ��ӵ���
	}
	else if((NewPriceTab.rate[ctype] >= 111)&&(NewPriceTab.rate[ctype] <= 119)){//110�Ͳ���Ҫ����
		if(i < 5){
			temp_s += ((NewPriceTab.rate[ctype]-110)*i); //�Ӹ��ӵ���
		}
	}
#endif

	MSG_LOG("get_Month_s_sum1 temp_s:");
	BCD_LOG((unsigned char*)&temp_s, 4, 1);

	return temp_s;
}

//Ǯ�������Ϸ�ʽ
unsigned char PurseProcess(unsigned char mode)		
{  //mode=0
	unsigned char i=0,b1,b2;
	unsigned char buffer[60];
	unsigned char buffer_9[16];
	unsigned char Check_TRANSFER=ST_OK;
	unsigned char buffer_10[16];
	unsigned char block9=ST_OK;
	unsigned char block10=ST_ERROR;
//	unsigned int price,plus;
//	unsigned char price_ratio,plus_ratio;//,r;
	unsigned char block910=ST_OK;
	unsigned int eemoney=0, moneyB2, blockB2;//,temp10,tempp;

#ifdef _debug_
	debugstring("Purse Process s\r\n");
#endif
	//���������
	for(i=0;i<error_pointer;i++)
	{
		if(ErrorDetail[i].effFlag!=IS_EFFECT_MONEY) continue;
		else
		{
			if(memcmp(ErrorDetail[i].Cardno,&c_serial,4)==0)
			{
				switch(ErrorDetail[i].ErrorStep)
				{
				case OPER_TRANSFER_NO:		
					{
						memcpy((unsigned char *)&eemoney,ErrorDetail[i].AferToll,4);
						memcpy((unsigned char *)&s_sum1, ErrorDetail[i].TollValue, 4);
						Check_TRANSFER=ST_ERROR;
						break;
					}
				case OPER_RECORD_PROFILE:		
				case OPER_PUBLIC_MESSAGE:
				case OPER_PUBLIC_MESSAGE_W:
					return 5;						
				default:
					break;
				}
			}
		}
	}
#ifdef _debug_
	debugstring("read Money Block:;");
#endif
	//	for(i=0;i<3;i++){
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MoneyKEYA,cardMessage.publicMoneySector*4)==0)
	{
		debugstring("Purse Process MifareAuthKey error return\r\n ");
		return ST_ERROR;
	}
	// 		debugstring("Purse Process MifareAuthKey error\r\n ");
	// 		InitRC531();
	// 		profile(0);
	// 	}
	// 	if(i>=3){
	// 		debugstring("Purse Process MifareAuthKey error return\r\n ");
	// 		return ST_ERROR;
	// 	}
	//��Ǯ����Ϣ��
	// 	if(MifareRead(cardMessage.publicMoneySector*4,buffer)==0)
	// 	{
	// #ifdef _DEBUG
	// 		error(ERR_MIFARE_READ,cardMessage.publicMoneySector*4);
	// #endif
	// 		return ST_ERROR;
	// 	}


	//��Ǯ��������
	if(MifareRead(cardMessage.publicMoneySector*4+1,buffer)==0)
	{
		debugstring("MifareRead money*4+1 ERROR!\r\n");
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("Ǯ����������\r\n");
	debugdata(buffer,16,1);
#endif
	memcpy(buffer_9,buffer,16);
		
	if(MifareRead(cardMessage.publicMoneySector*4+2,buffer)==0)
	{
		debugstring("MifareRead money*4+2 ERROR!\r\n");
		return ST_ERROR;
	}
	memcpy(buffer_10,buffer,16);

#ifdef _debug_
	debugstring("Ǯ���鸱����\r\n");
	debugdata(buffer,16,1);
#endif

	if(MifIsPurse(buffer_9)==ST_ERROR) 
	{
		block910=ST_ERROR;
		block9=ST_ERROR;
	}
	else{
		block9=ST_OK;
		block910=ST_OK;
	}
	if(MifIsPurse(buffer_10)==ST_ERROR) 	
	{
		block10=ST_ERROR;
		block910=ST_OK;
	}
	else{
		block10=ST_OK;
	}
	
	if((block9==ST_ERROR)&&(block10==ST_ERROR))
		return RET_MONEY_SN_EROR;
	
	if((block9 == ST_OK)&&(block10 == ST_OK)){
		block910=ST_OK;
		for(i=0;i<4;i++)
		{
			if(buffer_9[3-i]>buffer_10[3-i])
			{
				block910=ST_ERROR;
				break;
			}
			if(buffer_9[3-i]<buffer_10[3-i])
			{
				block910=ST_OK;
				break;
			}
		}
	}

	if(block910==ST_ERROR) 
		memcpy(buffer_9,buffer_10,4);

	if(block910==ST_ERROR){
		b1 = cardMessage.publicMoneySector*4+2;
		b2 = cardMessage.publicMoneySector*4+1;
		memcpy((unsigned char*)&moneyB2, buffer_9, 4);
		blockB2 = block9;//�ڶ����Ƿ��Ǻõġ�
	}
	else{
		b1 = cardMessage.publicMoneySector*4+1;
		b2 = cardMessage.publicMoneySector*4+2;
		memcpy((unsigned char*)&moneyB2, buffer_10, 4);
		blockB2 = block10;
	}

	memcpy((unsigned char*)&a_sum1, buffer_9, 4);
#ifndef FengDuan_BU_  //�ֶ��շѵģ�s_sum1�Ѿ���ֵ�ˡ�
	s_sum1 = get_s_sum1(mode);
#endif

	//�û�ԭ��
#ifdef _debug_
	debugstring("��ԭ��:");
	debugdata((unsigned char *)&a_sum1, 4, 1);
	debugstring("moneyB2:");
	debugdata((unsigned char *)&moneyB2, 4, 1);
#endif

	if(mode==100) return ST_OK;
	
	if(Check_TRANSFER==ST_ERROR)
	{
		if(eemoney>=a_sum1)
			goto mey_step90;
	}
	
	gucSoudnorm = 0;
#ifdef BUS_JIUJIANG_ //��Ǯ�����С��3Ԫʱ	���ֵ
	if((a_sum1>=s_sum1)&&(a_sum1<300)){
		gucSoudnorm = 1;
	}
#elif defined BUS_LICHUANG_
	eemoney = s_sum1*4;
	if((a_sum1>=s_sum1)&&(a_sum1<eemoney)){
		gucSoudnorm = 1;
	}
#else
	if((a_sum1>=s_sum1)&&(a_sum1<500)){
		gucSoudnorm = 1;
	}
#endif
	
#ifndef BuS_PRO_Burma_
	if((s_sum1 == 0) || (s_sum1 > 200000))
		return 22;
#ifdef BUS_HEZHOU_
	if (a_sum1 >= 1000000) 
#else
		if (a_sum1 >= 10000000) 
#endif
		{
			return 21;
		}
#endif
	
	if(a_sum1 >= s_sum1)
		a_sum1 -= s_sum1;										//�������
	else{
		led_on(LED_RED);
#ifdef BuS_PRO_Burma_
		MessageBox(0, " ticket balance is lack!");
		beep(5,50,50);
#elif defined _AREA_ENG_
		MessageBox(0, "ticket balance is lack!");
		beep(5,50,50);
#else
		MessageBox(0, "����!");
		SoundMessage(SOUND_FLING_MONEY);//��Ͷ��
#endif
		return 3;
	}
	if(s_sum1 == 0)
		return 5;

#ifdef _debug_
	debugstring("��b1:");
	debugdata((unsigned char *)&b1, 1, 1);
	debugstring("��b2:");
	debugdata((unsigned char *)&b2, 1, 1);
#endif
	
// 
// 	for(i=0;i<3;i++)
// 	{
// 		memcpy(buffer,(unsigned char*)&s_sum1,4);
// 		if(MifareValueDebit(PICC_DECREMENT,b1,buffer))
// 			goto mey_step9;
// 	}
	memcpy(buffer,(unsigned char*)&s_sum1,4);
	for(i=0;i<3;i++)//�ָ�����
	{
		if(block910==ST_ERROR)
		{
			//memcpy(buffer,buffer_9+4,4);
			if(MifareValueDebit(PICC_DECREMENT,cardMessage.publicMoneySector*4+2,buffer))
				goto mey_step9;
		}
		else
		{
			//	memcpy(buffer,buffer_9+4,4);
			if(MifareValueDebit(PICC_DECREMENT,cardMessage.publicMoneySector*4+1,buffer))
				goto mey_step9;
		}
	}
	debugstring("MifareValueDebit ERROR!\r\n");
	return ST_ERROR;
mey_step9:
	for(i=0;i<1;i++)
	{
		if(MifareTransfer(b1))
			break;
	}
	for(i=0;i<3;i++)
	{
		if(MifareRead(b1,buffer))
		{
			if(memcmp(buffer,(unsigned char*)&a_sum1,4)==0)
				goto mey_step90;
		}
	}
	if(i==3)
	{
		CheckSno(ERROR_MONEY,OPER_TRANSFER_NO,buffer);
		return 4;
	}
mey_step90:
	eemoney = moneyB2-a_sum1;//��Ǯ����Ҫ����ֵ
	if(blockB2 == ST_ERROR){//����˱����Ǻõģ�ֻ������һ���ָ�
		for(i=0;i<3;i++)
		{
			memset(buffer, 0, sizeof(buffer));
			if(MifareValueDebit(PICC_DECREMENT,b1,buffer))
				break;
		}
	}
	else{//�˱�û�����⣬�������м�ֵ
		for(i=0;i<3;i++)
		{
			if(MifareValueDebit(PICC_DECREMENT,b2,(unsigned char*)&eemoney))
				break;
		}
	}

	for(i=0;i<1;i++)
	{
		if(MifareTransfer(b2))
			break;
	}
	
#ifdef _debug_
	debugstring("Purse over!\r\n");
#endif
	ErrorOper(IS_EFFECT_MONEY);
	return 5;
}
extern unsigned char dis_overdate;	//��ʾ�������� 
//cMode: 0 - M1���� 1 - �ֻ����� 2 - CPU��
void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde)
{
	unsigned char disp[50];
	char buffer1[20];
	unsigned char pos;
	
#ifdef _New_Bu_mode_
	led_off(0xFF);
#endif
	led_on(LED_GREEN);
	
	restore_flag=3;
// 	if(memcmp(cardMessage.PublishBicker,(unsigned char*)&last_card_no ,4))
// 	{
// 		memcpy((unsigned char*)&last_card_no,cardMessage.PublishBicker,4);
// 		pFistVary.count=1;
// 	}
// 	else
// 	{
// 		pFistVary.count++;
// 	}
// 
// 	tmp = pFistVary.count;
	cls();

	pos = 0;
	if (gMCardCand == CARDSTYLE_QPBOC|| gMCardCand ==CARDSTYLE_UNPAY_ODA)
	{
		switch(emv_get_pay_channel())
		{
		case ODA:
		case UICS:
			display(0,0,"ODAˢ��", DIS_CENTER);
			break;
		case E_cash:
			display(0,0,"�����ֽ�ˢ��", DIS_CENTER);
			break;
		case PAY_Online:
			display(0,0,"����PASSˢ��", DIS_CENTER);
			break;
		default:
			break;
			
			
			
		}
		
	}
		else if (gMCardCand == CARDSTYLE_QR_PBOC)
	{
		display(0,0,"������ά��", DIS_CENTER);
	}
	else if (gMCardCand == CARDSTYLE_EWM)
	{
		display(0,0,"��ά��", DIS_CENTER);
	}
	else{
		display(0,0,"ˢ���ɹ�", DIS_CENTER);

	}
	pos+=2;
// 	display(0,0,"ˢ���ɹ�", DIS_CENTER);
// 	pos+=2;
	
	memset(disp,0,50);


	MSG_LOG("����:0x%02X\r\n",cardMessage.card_catalog);


	if(dmode==ID_REC_TOLL)
	{
		memset(buffer1, 0, sizeof(buffer1));
// 		if(gchn_eng == 'E'){
// 			if(getCardtype(buffer1, cardMessage.card_catalog) == ST_ERROR){
// 				sprintf(buffer1, "%d", cardMessage.card_catalog);
// 			}
// 			display(0, 1, buffer1, DIS_CENTER);
// 		}
// 		else{
		strcpy(buffer1, Conn(_Font_showCardtype, ":"));
		if(getCardtype(buffer1+5, cardMessage.card_catalog) == ST_ERROR){
			sprintf(buffer1+5, "%d", cardMessage.card_catalog);
		}
		if ((dis_overdate>=0) &&(dis_overdate<=15))
		{
			memset(buffer1,0,sizeof(buffer1));
			sprintf(buffer1,"%d������",dis_overdate+1);
		//	display_3232(pos, 0, buffer1, DIS_CENTER);
		}
		display_3232(pos, 0, buffer1, 0);
		pos += 2;
		pos += 2;
//		}

		if (gMCardCand != CARDSTYLE_QR &&emv_get_pay_channel()!=PAY_Online )
		{
			if(gchn_eng == 'E')
				sprintf(buffer1, "Balance:%d.%02d", remM/100, remM%100);
			else
				sprintf(buffer1, "���:%d.%02dԪ", remM/100, remM%100);
			display_3232(pos, 0, buffer1, 0);
			pos += 2;
			pos += 2;
		}
		if (pucM == 0)
		{
			memset(buffer1,0,sizeof(buffer1));
			strcpy(buffer1,"���");
			display_3232(pos, 0, buffer1, DIS_CENTER);
				}
				else{
					if(gchn_eng == 'E')
						sprintf(buffer1, "Deduct :%d.%02d", pucM/100, pucM%100);
					else
						sprintf(buffer1, "�ۿ�:%d.%02dԪ", pucM/100, pucM%100);
#ifdef _debug_trans
					printf("%s\r\n",buffer1);
#endif
					display_3232(pos, 0, buffer1, 0);
					
		}

		pos += 2;
		pos += 2;

		sprintf(buffer1, "%d.%02d", pucM/100, pucM%100);
		LED_Dis3(buffer1);
#ifdef MOBILE_XIANYANG_
		if(cardMessage.card_catalog == CARD_STUDENT_BUS){
			if(gUserTimes.Ttimes > 800)
				display(6,0,"ˢ���ѵ�800��",DIS_CENTER);
			else{
				sprintf(buffer1, "ʹ�ô���:%d�� ", gUserTimes.Ttimes);
				display(6,0,buffer1,0);
			}
		}
#endif
	}
	else if(dmode==ID_REC_MON)  
	{
		strcpy(buffer1, Conn(_Font_showCardtype, ":"));
		if(getCardtype(buffer1+5, cardMessage.card_catalog) == ST_ERROR){
			sprintf(buffer1+5, "%d", cardMessage.card_catalog);
		}
		display_3232(pos, 0, buffer1, 0);
		pos += 2;
		pos += 2;

		if(s_sum1 == 0){
			if(cardMessage.card_catalog == CARD_DRIVER_BUS){
				if(gchn_eng == 'E')
					display_3232(pos,0,"Staff card",DIS_CENTER);
				else
					display_3232(pos,0,"˾����",DIS_CENTER);
			}
			else{
				getCardtype((char*)buffer1, cardMessage.card_catalog);
				display_3232(pos,0,buffer1, DIS_CENTER);
			}
			pos += 2;
			pos += 2;
			display_3232(pos,0,Card_Welcome,DIS_CENTER);
			pos += 2;
			pos += 2;
		}
		else
		{
			if(gchn_eng == 'E')
				sprintf(buffer1, "Balance:%d", remM);
			else
				sprintf(buffer1, "���:%d", remM);
			display_3232(pos, 0, buffer1, 0);
			pos += 2;
			pos += 2;

			if(gchn_eng == 'E')
				sprintf(buffer1, "Deduct :%4d", pucM);
			else
				sprintf(buffer1, "�۴�:%d", pucM);
			display_3232(pos, 0, buffer1, 0);
			pos += 2;
			pos += 2;
			
			sprintf(buffer1, "C%2d", pucM);
			LED_Dis3(buffer1);
		}
	}
	else
	{
		if(gchn_eng == 'E'){
			display_3232(pos,5,"oldster card",DIS_CENTER);
			pos += 2;
			pos += 2;
			display_3232(pos,2,"WELCOME!",0);
			pos += 2;
			pos += 2;
		}
		else{
			display_3232(pos,5,"���˿�B",DIS_CENTER);
			pos += 2;
			pos += 2;
			display_3232(pos,2,"��ӭ�˳�!",0);
			pos += 2;
			pos += 2;
		}
	}

	if(gchn_eng != 'E'){
	if(gucEndTimeFlag==1){
		display(pos, 0, "  �ÿ���Ҫ����  ", 1);
		pos += 2;
		beep(4, 50, 50);
	}
	}
}

//void PleaseYearCheck(stcomtime *CurrerData,stcomtime *EndData)
//{
//
//	unsigned int uiTemp;
//
//	unsigned int TNowData,TEndData;
//
//	stcomtime YearCheckDate;
//	
//	uiTemp =calendar2days(CurrerData);
//
//	uiTemp+=30;
//
//	days2calendar(&YearCheckDate,uiTemp);
//
//	memcpy(TNowData.charbuffer,(unsigned char *)&YearCheckDate.year_h,4);
//
//	memcpy(TEndData.charbuffer,&EndData->year_h,4);
//
//	MSG_LOG("����������ʾ�ж�:\r\n");
//
//	BCD_LOG(4,4,TNowData.charbuffer);
//
//	BCD_LOG(4,4,TEndData.charbuffer);
//
//	if (TNowData.longbuffer >= TEndData.longbuffer) 
//	{
//		//������ʾ������
//		audio(AUDIO_No,100);
//
//		MSG_LOG("������\r\n");
//
// 	}
//
//}

unsigned int IsBlackBlock(unsigned char *c_card_no, unsigned char block)
{
	unsigned int i_start=0;
	unsigned int i_end;
	unsigned int i_cur, addr;
	unsigned char i;
	unsigned char temp[BLK_SNO_LEN];
	unsigned int invalid_point;
	unsigned char buffer[BLK_SNO_LEN];

	if(gBlackBlockNum[block]==0)
		return 0xffffffff;
	memcpy(temp,c_card_no,BLK_SNO_LEN);
	i_end=gBlackBlockNum[block]-1;


	MSG_LOG("black  --  current Card NO.:");
	BCD_LOG(temp,BLK_SNO_LEN,1);
	MSG_LOG("\r\n");
	BCD_LOG((unsigned char *)&i_end,4,1);

	while(1)
	{
		i_cur=i_start+(i_end-i_start)/2;
		invalid_point=i_cur*BLK_SNO_LEN;
		if(block == 0)
			addr = FLASH_BLK_START+(block*4096)+14+invalid_point;//��ǰ14�ֽ��ǰ汾��,�����Ҳ������
		else
			addr = FLASH_BLK_START+(block*4096)+invalid_point;//
		
		flashread(addr, buffer, BLK_SNO_LEN);

		MSG_LOG("invalid_point:%08X\r\n", invalid_point);
		MSG_LOG("i_cur:%08X\r\n", i_cur);
		BCD_LOG(buffer,BLK_SNO_LEN,1);

		for(i=0;i<BLK_SNO_LEN;i++)
		{
			//if(temp[BLK_SNO_LEN-1-i]==buffer[BLK_SNO_LEN-1-i])		//������ǰ����
			if(temp[i]==buffer[i])										//ǰ����������
				continue;
			//else if(temp[BLK_SNO_LEN-1-i]>buffer[BLK_SNO_LEN-1-i])	//������ǰ����
			else if(temp[i]>buffer[i])									//ǰ����������
			{
				if(i_end==i_cur)
					return 0xffffffff;
				else
				{
					i_start=i_cur+1;
					break;
				}
			}
			else
			{
				if(i_start==i_cur)
					return 0xffffffff;
				else
				{
					i_end=i_cur;
					break;
				}
			}
		}
		if(i==BLK_SNO_LEN){
// 			MSG_LOG("addr:");
// 			BCD_LOG((unsigned char*)&addr, 4, 1);
			return addr;
		}
	}
}

unsigned int IsInBlackList(unsigned char *c_card_no)
{
	unsigned char i;
	unsigned int addr;

	for(i=0; i<16; i++){
		MSG_LOG("is BlacK:%d|| %d\r\n", i, gBlackBlockNum[i]);

		if(gBlackBlockNum[i] == 0)
			break;
		addr = IsBlackBlock(c_card_no, i);
		if(addr < FLASH_BLK_END)
			return addr;
	}
	return 0xffffffff;
}

#ifdef BUS_ANLONG_
extern unsigned char Card_typeProcessANLONG(void);
#endif//#ifdef BUS_ANLONG_

#ifdef BUS_GUILIN_
	extern unsigned char Card_typeProcess_GuiLin(void);
#endif
extern unsigned char Card_typeProcess_nomal(unsigned char mode);
unsigned char Card_typeProcess(void)	
{
	unsigned char i;
#ifdef BUS_GUILIN_
	i = Card_typeProcess_GuiLin();
#elif defined BUS_MZT
	i = Card_typeProcess_MZT(0);
#else
	i = Card_typeProcess_nomal(0);
	#ifdef BUS_ANLONG_
		MSG_LOG("C RI:");
		BCD_LOG((unsigned char*)&i, 1, 1);
		if(i == 0xA2)
			return Card_typeProcessANLONG();
	#endif
#endif
	getCpuDeviceNO(pFistVary.DeviceNo);
	return i;
}
			   
void wRiteControlRecord(unsigned char ID)
{
	unsigned char rRecord[RECORD_LEN];
	
	a_sum1 = 0;
	s_sum1 = 0;

	BuildRecorde(ID, rRecord);
	WriteRecord(rRecord, 0);
	return;
}


#ifdef BUS_Cloud_
void set_line_cloud(void)
{
	unsigned char i,j, flag;
	unsigned char s_no;
	unsigned char buffer1[150];
	unsigned short temp1=0;
	unsigned char Key[20];

	cls();
	if(gchn_eng == 'E')
		display(4,3,"PLS wait... ",0);
	else
		display(4,3,"���Ե�......",0);
	
	InitRC531();
	profile(0);

	flag = 0;
	memset(buffer1,0,120);
#ifdef BUS_PRO_JSB
 	memcpy(Key, cardMessage.PublishKEYA, 6);
#else
	memcpy(Key, cardMessage.RecodeKEYA, 6);
#endif

	MSG_LOG("set Line No: Key: RecodeKEYA:");
	BCD_LOG(Key, 6, 1);
	BCD_LOG((unsigned char*)&c_serial, 4, 1);

	s_no=0;	
	for(i=0;i<5;i++){
		if(cardMessage.dealRecordSector[s_no] > 15){
			cardMessage.dealRecordSector[s_no] = 3+s_no;
		}
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,Key,cardMessage.dealRecordSector[s_no]*4);

		MSG_LOG("set Line No 2 cardMessage.dealRecordSector[s_no]:");
		BCD_LOG(cardMessage.dealRecordSector+s_no, 1, 1);

		if(MifareRead(cardMessage.dealRecordSector[s_no]*4,buffer1)!=0){
			break;
		}
		
		for(j=0; j<5; j++){
			if(profile(0) != ST_ERROR)
			{
				break;
			}
			else{
				InitRC531();
			}
		}
	}
	if(i>=3){
		MessageBox(1, "��Կ����!");
		return;
	}


	MSG_LOG("set Line No 3 buf:");
	BCD_LOG(buffer1, 16, 1);

	memcpy((unsigned char*)&temp1, buffer1, 2);

	sprintf((char*)buffer1+60,"%4d",temp1);

	memcpy(pFistVary.LineNoOld, pFistVary.LineNo, 3);//�ɵ���·��
	memcpy(pFistVary.LineNo, buffer1, 3);//��·��

	memset(NewPriceTab.busPrice, 0, 4);
	memcpy(NewPriceTab.busPrice,buffer1+3,2);//Ʊ��
	
	memset(NewPriceTab.plusPrice, 0, 4);
	memcpy(NewPriceTab.plusPrice,buffer1+5,2);//���ӷ�
	memset(NewPriceTab.plusMonthPrice, 0, 4);
	memcpy(NewPriceTab.plusMonthPrice,buffer1+9,2);//��Ʊ���ӷ�
	if(memcmp(buffer1+0x0b, "��ʱ", 4) == 0)
		flag = 1;//POS��������·����

	if (MifareRead(cardMessage.dealRecordSector[s_no]*4+1,buffer1) ==0){
		return;
	}

	MSG_LOG("set Line No 4 buf:");
	BCD_LOG(buffer1, 16, 1);	

	for(i=0;i<8;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*i];//41H~47H��˿Ϳ���Ʊ���ۿ��ʡ����ӷ��ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*i+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	if (MifareRead(cardMessage.dealRecordSector[s_no]*4+2,buffer1) ==0){
		memcpy(pFistVary.LineNo, pFistVary.LineNoOld, 3);//�ɵ���·��
		return;
	}

	MSG_LOG("set Line No 5 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=8;i<16;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*(i-8)];//48H~4FH��˿Ϳ���Ʊ���ۿ��ʡ����ӷ��ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*(i-8)+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	
	if(flag == 0){//POS��ֻд��ǰ�����飬����Ҫ���������
	s_no++;	
	if(cardMessage.dealRecordSector[s_no] > 15){
		cardMessage.dealRecordSector[s_no] = 3+s_no;
	}
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,Key,cardMessage.dealRecordSector[s_no]*4)==0)
	{
		memcpy(pFistVary.LineNo, pFistVary.LineNoOld, 3);//�ɵ���·��
		MSG_LOG("MifareAuthKey error\r\n ");
		return ; 
	}

	MSG_LOG("set Line No 6 Key:");
	BCD_LOG(Key, 6, 1);

	
	if(MifareRead(cardMessage.dealRecordSector[s_no]*4,buffer1)==0){
		memcpy(pFistVary.LineNo, pFistVary.LineNoOld, 3);//�ɵ���·��
		return;
	}

	MSG_LOG("set Line No 7 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=16;i<24;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*(i-16)];//50H~57H��˿Ϳ���Ʊ���ۿ��ʡ����ӷ��ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*(i-16)+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	
	//	if(MifareRead(cardMessage.dealRecordSector[s_no]*4+1,buffer1)==0) return;
	MifareRead(cardMessage.dealRecordSector[s_no]*4+1,buffer1);
	MSG_LOG("set Line No 8 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=24;i<32;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*(i-24)];//58H~5FH��˿Ϳ���Ʊ���ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*(i-24)+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	}//if(flag == 0){//POS��ֻд��ǰ�����飬����Ҫ���������
	else{
		for(i=16;i<32;i++)
		{
			NewPriceTab.rate[i]=100;//
			NewPriceTab.plusrate[i]=0;//
		}
	}//if(flag == 0){//POS��ֻд��ǰ�����飬����Ҫ���������

	s_no = 2;		//��5������������
	if(cardMessage.dealRecordSector[s_no] > 15){
		cardMessage.dealRecordSector[s_no] = 3+s_no;
	}
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,Key,cardMessage.dealRecordSector[s_no]*4)==0)
	{
		MSG_LOG("MifareAuthKey 5 error\r\n ");
		return ; 
	}
	
	if(MifareRead(cardMessage.dealRecordSector[s_no]*4,buffer1)==0){
		return;
	}
	

	MSG_LOG("set Line No 5-0 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=0;i<16;i++)
	{
		NewPriceTab.voice[i]=buffer1[i];//40H~4FH��˿Ϳ�������
	}
	
	if(MifareRead(cardMessage.dealRecordSector[s_no]*4+1,buffer1)==0){
		return;
	}

	MSG_LOG("set Line No 5-1 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=0;i<16;i++)
	{
		NewPriceTab.voice[i+16]=buffer1[i+16];//50H~5FH��˿Ϳ�������
	}

	memcpy(NewPriceTab.citycode, "\x07\x55", 2);
	
	memset(NewPriceTab.deviceNo, 0, 4);

	Get_SerialNumF4(NewPriceTab.deviceNo);
	memcpy(NewPriceTab.lineVer, "\x00\x00", 2);

	memcpy(buffer1,pFistVary.LineNo,3);
	sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
	
	sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);

	BCD_LOG((unsigned char*)&NewPriceTab,256,1);

	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��

	savesysbupara();
	saveCpuInfo(1, buffer1);
	F_FLAG_VARY.set_line_status=0;
	restore_flag=1;
	if(gchn_eng == 'E')
		display(4,3,"complete!",0);
	else
		display(4,3,"�������!",0);
	a_sum1 = 0;
	s_sum1 = 0;
	beep(1, 100, 10);
	SoundMessage(SOUND_DI_BUS);
	BuildRecorde(ID_REC_LINENO, buffer1);
	WriteRecord(buffer1, 0);
//	ControlDis(cardMessage.card_catalog);
	delayxs(1);

	MSG_LOG("Set line END!!!\r\n");

}
#endif

#ifdef FengDuan_BU_
extern void debugFDinfo(void);
extern stFDLineHead gFdLinehead;
void set_FengDuan_line(void)
{
	unsigned int outdly=0;
	char disbuf[32];
	unsigned char M1buf[16], M1buf2[16], M1buf3[16];
	unsigned int Cindex, wsaddr, windex, useFDs;
	unsigned short Pnum, ustmp;
	unsigned char sector;
	unsigned char block,ret,allstation, i, kk, tt;

	cls();

#ifdef _AREA_ENG_
	display(0,0," Price Card ", DIS_CENTER|DIS_CONVERT);
	display(2, 0, "PLS Swip Card", DIS_CENTER);
#else
	display(0,0," �ֶ�Ʊ�� ", DIS_CENTER|DIS_CONVERT);
	display(2, 0, "��ˢ��1�ſ�", DIS_CENTER);
#endif
	Cindex = 1;
	allstation = 0;
	Pnum = 0;
	windex = 0;
	useFDs = 0;
	
	while(1){
		if(outdly++ > 200){
			return;
		}
		if((outdly % 10) == 0){
			sprintf(disbuf, " %d ", (20-(outdly/10)));;
			display(6, 0, disbuf, DIS_CENTER);
		}
		delayxms(50);
		if(profile(0) == ST_ERROR)
			continue;
		
		sector = 1;
		block = sector*4;
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial, cardMessage.PublishKEYA, block);

		MSG_LOG("set FDLine:");

		if(MifareRead(block, M1buf) ==0)//0-4
		{
			MSG_LOG("read 1 error\r\n ");
			profile(0);
		}

		MSG_LOG("0_4:");
		BCD_LOG(M1buf, 16, 1);

		if(MiBlockInvalid(1, 15, M1buf) == ST_ERROR){
#ifndef _AREA_ENG_
			sprintf(disbuf, "%d��У���!", block);
			display(4, 0, disbuf,DIS_CENTER);
#endif
			continue;	//У���,�˿���Ϊ�Ǵ����.
		}

		MSG_LOG("OK!\r\n");

		if(M1buf[13] != CARD_FENDUAN_Line){
#ifndef _AREA_ENG_
			sprintf(disbuf, "%d��,����·��!", M1buf[13]);
			display(4, 0, disbuf,DIS_CENTER);
#endif
			continue;	//У���,�˿���Ϊ�Ǵ����.
		}
		
		block += 2;
		if(MifareRead(block, M1buf) ==0)//0-4
		{
			MSG_LOG("read 1 error\r\n ");
			profile(0);
		}

		MSG_LOG("Read 2_6:");
		BCD_LOG(M1buf, 16, 1);

		if(MiBlockInvalid(1, 15, M1buf) == ST_ERROR){
#ifdef _AREA_ENG_
			sprintf(disbuf, "%d��У���!", block);
			display(4, 0, disbuf,DIS_CENTER);
#endif
			continue;	//У���,�˿���Ϊ�Ǵ����.
		}

		MSG_LOG("OK!\r\n");

		if(Cindex != M1buf[3]){
#ifdef _AREA_ENG_
			sprintf(disbuf, "Don't Swip No %d!", M1buf[3]);
			display(4, 0, disbuf,DIS_CENTER);
#else
			sprintf(disbuf, "��Ҫˢ%d�ſ�!", M1buf[3]);
			display(4, 0, disbuf,DIS_CENTER);
#endif
			continue;	//������Ǵ��.
		}
		if((Pnum == 0) || (allstation == 0) || (Cindex == 1)){//��һ�ſ�,������·��ż�Ʊ��
			memcpy(pFistVary.LineNo, M1buf, 3);
			Pnum = BCD2int(M1buf+4, 2);
			allstation = M1buf[6];//��·��վ��
			memcpy((unsigned char*)&ustmp, M1buf+7, 2);
			over_turn(2, (unsigned char*)&ustmp);	//���ӷ�
			memset(NewPriceTab.plusPrice, 0, 4);
			memcpy(NewPriceTab.plusPrice, (unsigned char*)&ustmp, 2);

			memcpy((unsigned char*)&ustmp, M1buf+12, 2);
			over_turn(2, (unsigned char*)&ustmp);	//����·ȫ��Ʊ��
			memset(NewPriceTab.busPrice, 0, 4);
			memcpy(NewPriceTab.busPrice, (unsigned char*)&ustmp, 2);

			MSG_LOG("first Card_lineinfo:%04X  %d\r\n", Pnum, allstation);
			BCD_LOG(NewPriceTab.plusPrice, 4, 1);
			BCD_LOG(NewPriceTab.busPrice, 4, 1);

			wsaddr = FLASH_FD_Ticket_START;
			flasherase(ERASE32KBYTESECTOR, wsaddr);//����
			memcpy(disbuf, "����·��ϢOK\x0\x0\x0\x0", 16);
			i = CRC8_Tab((unsigned char*)&SysTime, 7);	//���㵱ǰ��CRC8��Ϊ�汾��
			if(i == 0) i = 1;	//����Ϊ0
			disbuf[12] = i;
			flashwrite(wsaddr, (unsigned char*)disbuf, 16);
			wsaddr += 16;
			over_turn(2, M1buf+7);//���ӷ�
			over_turn(2, M1buf+10);//����·�����������ʱ��
			over_turn(2, M1buf+12);//����·ȫ��Ʊ��
			flashwrite(wsaddr, M1buf, 16);		//��·ͷ
			
			memcpy((unsigned char*)&gFdLinehead, M1buf, sizeof(stFDLineHead));

			MSG_LOG("first Card_lineinfo write END\r\n");

			if(Cindex == 1){//����ǵ�һ�ſ�,��Ҫ�����������ۿ���,�ڵ�2����.
				windex = 0;
				useFDs = 0;
				memset(NewPriceTab.rate, 100, 32);
				memset(NewPriceTab.plusrate, 0, 32);
				memset(NewPriceTab.cardControl, 0, 32);
#ifdef AutoAudio_line
				memset(NewPriceTab.voice, 0, 32);
#else
				memset(NewPriceTab.voice, Audio_DONG, 32);
#endif
				sector = 2;
				block = sector*4;
				MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial, cardMessage.PublishKEYA, block);

				MSG_LOG("read 2 secoter1:");
				if(MifareRead(block, M1buf) ==0)//2-0
				{
					MSG_LOG("read 1 error\r\n ");
					profile(0);
				}

				MSG_LOG("0_4:");
				BCD_LOG(M1buf, 16, 1);

				if(MiBlockInvalid(1, 15, M1buf) == ST_ERROR){
#ifndef _AREA_ENG_

					sprintf(disbuf, "%d��У���!", block);
					display(4, 0, disbuf,DIS_CENTER);
#endif
					continue;	//У���,�˿���Ϊ�Ǵ����.
				}

				MSG_LOG("OK!\r\n");
				MSG_LOG("read 2 secoter2:");

				block++;
				if(MifareRead(block, M1buf2) ==0)//2-1
				{
					MSG_LOG("read 1 error\r\n ");
					profile(0);
				}

				MSG_LOG("0_4:");
				BCD_LOG(M1buf2, 16, 1);

				if(MiBlockInvalid(1, 15, M1buf2) == ST_ERROR){
#ifdef _AREA_ENG_
					sprintf(disbuf, "%d��У���!", block);
					display(4, 0, disbuf,DIS_CENTER);
#endif
					continue;	//У���,�˿���Ϊ�Ǵ����.
				}

				MSG_LOG("OK!\r\n");
				MSG_LOG("read 2 secoter3:");

				block++;
				if(MifareRead(block, M1buf3) ==0)//2-2
				{
					MSG_LOG("read 1 error\r\n ");
					profile(0);
				}

				MSG_LOG("0_4:");
				BCD_LOG(M1buf3, 16, 1);

				if(MiBlockInvalid(1, 15, M1buf3) == ST_ERROR){
#ifndef _AREA_ENG_
					sprintf(disbuf, "%d��У���!", block);
					display(4, 0, disbuf,DIS_CENTER);
#endif
					continue;	//У���,�˿���Ϊ�Ǵ����.
				}

				MSG_LOG("OK!\r\n");
	
				for(i=0; i<16; i++){
					ret = BCD2int(M1buf+i, 1);	//������BCD��
					if(ret > (CARD_NORMAL+31))//�������
						continue;
					if(ret >= 0x40)
						ret -= 0x40;
					NewPriceTab.rate[ret] = M1buf2[i];	//��������16������
					NewPriceTab.voice[ret] = M1buf3[i];
				}
				//��·��Ϣ��.д�������.

				MSG_LOG("LineNo1:");
				BCD_LOG(pFistVary.LineNo,3,1);

				memcpy(M1buf,pFistVary.LineNo,3);
				sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
				sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);

				BCD_LOG((unsigned char*)&NewPriceTab,256,1);

				sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
				savesysbupara();
				saveCpuInfo(1, M1buf);
				F_FLAG_VARY.set_line_status=0;
				restore_flag=3;
			}
		}
		
		//������ֶ���Ϣ
		for(i=3; i<16; i++){//��3����5
			for(ret = 0; ret<5; ret++){
				block = i*4;
				MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial, cardMessage.PublishKEYA, i*4);
				if(MifareRead(block, M1buf) ==0)
				{
					MSG_LOG("read 333 error\r\n ");
					profile(0);
					continue;
				}
				block++;
				if(MifareRead(block, M1buf2) ==0)
				{
					MSG_LOG("read 333 error\r\n ");
					profile(0);
					continue;
				}
				block++;
				if(MifareRead(block, M1buf3) ==0)
				{
					MSG_LOG("read 333 error\r\n ");
					profile(0);
					continue;
				}
				break;
			}
			if(ret >= 5){
#ifndef _AREA_ENG_

				sprintf(disbuf, "%d��������!", i);
				display(4, 0, disbuf,DIS_CENTER);
#endif
				break;	//У���,�˿���Ϊ�Ǵ����.
			}
			

			MSG_LOG("read m1buf:\r\n");
			BCD_LOG(M1buf,16,1);
			BCD_LOG(M1buf2,16,1);
			BCD_LOG(M1buf3,16,1);

			block = i*4;
			if(MiBlockInvalid(1, 15, M1buf) == ST_ERROR){
#ifndef _AREA_ENG_
				sprintf(disbuf, "%d��У���!", (block));
				display(4, 0, disbuf,DIS_CENTER);
#endif
				break;	//У���,�˿���Ϊ�Ǵ����.
			}
			if(MiBlockInvalid(1, 15, M1buf2) == ST_ERROR){
#ifndef _AREA_ENG_

				sprintf(disbuf, "%d��У���!", (block+1));
				display(4, 0, disbuf,DIS_CENTER);
#endif
				break;	//У���,�˿���Ϊ�Ǵ����.
			}
			if(MiBlockInvalid(1, 15, M1buf3) == ST_ERROR){
#ifndef _AREA_ENG_
				sprintf(disbuf, "%d��У���!", (block+2));
				display(4, 0, disbuf,DIS_CENTER);
#endif
				break;	//У���,�˿���Ϊ�Ǵ����.
			}
			for(tt=0; tt<15; tt++){//һ����5��,3�����15��
				if(tt==5)
					memcpy(M1buf, M1buf2, 16);
				else if(tt==10)
					memcpy(M1buf, M1buf3, 16);
				kk = (tt%5);

				MSG_LOG(":::");
				BCD_LOG(M1buf+(kk*3), 3, 0);

				over_turn(2, M1buf+(kk*3)+1);//Ʊ�۷�
				useFDs++;
				for(ret=0; ret<M1buf[kk*3]; ret++){
					wsaddr = FLASH_FD_Ticket_START+32;
					wsaddr += (windex*2);
					flashwrite(wsaddr, M1buf+(kk*3)+1, 2);
					windex++;
				}
				if(useFDs >= Pnum){//���
					audio(Audio_DONG);
#ifdef _AREA_ENG_
					display(4, 0, "   Successfull    ", 0);
#else
					display(4, 0, "   �������!    ", 0);
#endif
					end_card();
#ifdef _debug_
					debugFDinfo();
#endif
#ifdef FengDuan_BU_
					FDParChange_QM(FDParChange_Line);
#endif

					return;
				}
			}
		}
	}

}
#endif
void set_line(void)
{
	unsigned char i,j;
	unsigned char s_no;
	unsigned char buffer1[150];
#ifdef line_older_	
	unsigned short temp1=0;
#endif
	unsigned char Key[20];

	cls();
	if(gchn_eng == 'E')
		display(4,3,"PLS wait... ",0);
	else
		display(4,3,"���Ե�......",0);
	
// 	InitRC531();
// 	profile(0);

	memset(buffer1,0,120);
#ifdef BUS_PRO_JSB
#ifndef line_older_//�Ž������ڵĿ��ƿ�Ҳȫ��ȡ��Կ
	memcpy(Key, cardMessage.RecodeKEYA, 6);
#else
 	memcpy(Key, cardMessage.PublishKEYA, 6);  	//����·��   
#endif
#else
	memcpy(Key, cardMessage.RecodeKEYA, 6);
#endif

	MSG_LOG("set Line No: Key: RecodeKEYA:");
	BCD_LOG(Key, 6, 1);
	BCD_LOG((unsigned char*)&c_serial, 4, 1);

	s_no=3;
	for(i=0;i<5;i++){
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,Key,s_no*4);

	MSG_LOG("set Line No 2 s_no:");
	BCD_LOG(Key, 6, 1);

		if(MifareRead(s_no*4,buffer1)!=0){
			break;
		}
		
		for(j=0; j<5; j++){
			if(profile(0) != ST_ERROR)
			{
				break;
			}
			else{
				InitRC531();
			}
		}
	}
	if(i>=3)
		return;

	MSG_LOG("set Line No 3 buf:");
	BCD_LOG(buffer1, 16, 1);

	memcpy(pFistVary.LineNoOld, pFistVary.LineNo, 3);//�ɵ���·��
#ifndef line_older_	//����·��
	pFistVary.LineNo[0]=buffer1[0];//��·��  BCD��
	pFistVary.LineNo[1]=buffer1[1];//
#else
	memcpy((unsigned char*)&temp1, buffer1, 2);	//����·�� 
	sprintf((char*)buffer1+60,"%4d",temp1);

	pFistVary.LineNo[0]=(((buffer1[60]-'0')&0x0f)<<4)+((buffer1[61]-'0')&0x0f);//��·��
	pFistVary.LineNo[1]=(((buffer1[62]-'0')&0x0f)<<4)+((buffer1[63]-'0')&0x0f);//
#endif

	if(buffer1[2] == 0)
		buffer1[2] = 1;
	sprintf((char*)buffer1+60,"%2d",buffer1[2]);
	pFistVary.LineNo[2]=(((buffer1[60]-'0')&0x0f)<<4)+((buffer1[61]-'0')&0x0f);//��·�ֺ�
//	pFistVary.LineNo[2] = 0;
	
	memset(NewPriceTab.busPrice, 0, 4);
	memcpy(NewPriceTab.busPrice,buffer1+3,2);//Ʊ��
	
	memset(NewPriceTab.plusPrice, 0, 4);
	memcpy(NewPriceTab.plusPrice,buffer1+5,2);//���ӷ�

	if (MifareRead(s_no*4+1,buffer1) ==0){
		return;
	}

	MSG_LOG("set Line No 4 buf:");
	BCD_LOG(buffer1, 16, 1);
	
	for(i=0;i<8;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*i];//40H~47H��˿Ϳ���Ʊ���ۿ��ʡ����ӷ��ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*i+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	if (MifareRead(s_no*4+2,buffer1) ==0){
		memcpy(pFistVary.LineNo, pFistVary.LineNoOld, 3);//�ɵ���·��
		return;
	}

	MSG_LOG("set Line No 5 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=8;i<16;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*(i-8)];//48H~4FH��˿Ϳ���Ʊ���ۿ��ʡ����ӷ��ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*(i-8)+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}

	s_no++;	
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,Key,s_no*4)==0)
	{
		memcpy(pFistVary.LineNo, pFistVary.LineNoOld, 3);//�ɵ���·��
		MSG_LOG("MifareAuthKey error\r\n ");
		return ; 
	}

	MSG_LOG("set Line No 6 Key:");
	BCD_LOG(Key, 6, 1);
	
	if(MifareRead(s_no*4,buffer1)==0){
		memcpy(pFistVary.LineNo, pFistVary.LineNoOld, 3);//�ɵ���·��
		return;
	}

	MSG_LOG("set Line No 7 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=16;i<24;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*(i-16)];//50H~57H��˿Ϳ���Ʊ���ۿ��ʡ����ӷ��ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*(i-16)+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	
//	if(MifareRead(s_no*4+1,buffer1)==0) return;
	MifareRead(s_no*4+1,buffer1);

	MSG_LOG("set Line No 8 buf:");
	BCD_LOG(buffer1, 16, 1);

	for(i=24;i<32;i++)
	{
		NewPriceTab.rate[i]=buffer1[2*(i-24)];//58H~5FH��˿Ϳ���Ʊ���ۿ���
		NewPriceTab.plusrate[i]=buffer1[2*(i-24)+1];//40H~47H���ӷ��ۿ��� �ָ�Ϊ�Ƿ���ˢ־
	}
	
	s_no+=1;		//��������������
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,Key,s_no*4)==0)
	{
		MSG_LOG("MifareAuthKey error\r\n ");
		return ; 
	}
	
	if(MifareRead(s_no*4,buffer1)==0) return;
	
	for(i=0;i<16;i++)
	{
		NewPriceTab.voice[i]=i;//40H~4FH��˿Ϳ�������
	}
	
	if(MifareRead(s_no*4+1,buffer1)==0) return;
	for(i=0;i<16;i++)
	{
		NewPriceTab.voice[i+16]=i+16;//50H~5FH��˿Ϳ�������
	}

	memcpy(NewPriceTab.citycode, "\x07\x55", 2);
	
	memset(NewPriceTab.deviceNo, 0, 4);
#ifdef BUS_SERRIER_No
	getposSN(Key, 0);//ȡPOS���ն˱��
	Ascii2BCD(Key+10, NewPriceTab.deviceNo+1, 6);//Address:�ն˻����
#else 
	Get_SerialNumF4(NewPriceTab.deviceNo);
//Ascii2BCD(pFistVary.DeviceNo, NewPriceTab.deviceNo, 8);
#endif
	memcpy(NewPriceTab.lineVer, "\x00\x00", 2);

	SetAudio();

	MSG_LOG("LineNo1:");
	BCD_LOG(pFistVary.LineNo,3,1);

	memcpy(buffer1,pFistVary.LineNo,3);
	sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
	
	sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);

	BCD_LOG((unsigned char*)&NewPriceTab,256,1);

	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��

	savesysbupara();
	saveCpuInfo(1, buffer1);
	F_FLAG_VARY.set_line_status=0;
	restore_flag=1;
	if(gchn_eng == 'E')
		display(4,3,"complete!",0);
	else
		display(4,3,"�������!",0);
	

	MSG_LOG("LineNo2:");
	BCD_LOG(pFistVary.LineNo,3,1);

	a_sum1 = 0;
	s_sum1 = 0;
	beep(1, 100, 10);
	SoundMessage(SOUND_DI_BUS);
// #ifdef BUS_HANDAN_
// 	Write_Head_Record(1);
// #else //#ifdef BUS_HANDAN_
	BuildRecorde(ID_REC_LINENO, buffer1);

	MSG_LOG("LineNo3:");
	BCD_LOG(pFistVary.LineNo,3,1);

	WriteRecord(buffer1, 0);
//#endif	//#ifdef BUS_HANDAN_

	MSG_LOG("LineNo4:");
	BCD_LOG(pFistVary.LineNo,3,1);
	MSG_LOG("Set line END!!!\r\n");

}

void sysSaveDevice(unsigned char *dbuf)
{
	sysfewrite(BIT_DEVICE_NO,9,pFistVary.DeviceNo);
	savesysbupara();
	saveCpuInfo(2, dbuf);
}

void set_device(void)
{
	unsigned char i;
	unsigned char buffer[20];
	unsigned char ret;
	
	memcpy(pFistVary.DeviceNo_1, pFistVary.DeviceNo, 8);

Startt:
	i=0;
	memset(buffer,0,10);

	memcpy(buffer,pFistVary.DeviceNo,8);
	if(ValidDevNo(pFistVary.DeviceNo) == 1){
		memset(pFistVary.DeviceNo, '0', 9);
	}
	cls();
	if(gchn_eng == 'E')
		display(0,2,"set device NO.",DIS_CENTER);
	else
		display(0,2,"�����豸��",DIS_CENTER);
	display(2,4,(char*)buffer,0);
	while(1)
	{
		delayxms(10);
		memset(buffer,0,10);
		buffer[0]=pFistVary.DeviceNo[i];
		display(2,4+i,(char*)buffer,DIS_CONVERT);
		ret =getkey(1);
		switch(ret)
		{
#ifndef _New_Bu_mode_
		case UP:
			if((pFistVary.DeviceNo[i]<='0')||(pFistVary.DeviceNo[i]>'9'))
				pFistVary.DeviceNo[i]='9';
			else if((pFistVary.DeviceNo[i]<='9')&&(pFistVary.DeviceNo[i]>'0'))
				pFistVary.DeviceNo[i]--;

			break;
		case F2:  
			i++;
			if(i==8)
				i = 0;
			memset(buffer,0,10);
			memcpy(buffer,pFistVary.DeviceNo,8);
			display(2,4,(char*)buffer,0);
			break;
		case F1:
			if(i>0)
				i--;
			else
				i = 7;
			memset(buffer,0,10);
			memcpy(buffer,pFistVary.DeviceNo,8);
			display(2,4,(char*)buffer,0);
			break;
		case DOWN: 
#else
		case UP:
#endif  
			if((pFistVary.DeviceNo[i]<'9')&&(pFistVary.DeviceNo[i]>='0'))
				pFistVary.DeviceNo[i]++;
			else if(pFistVary.DeviceNo[i] == '9'){
				pFistVary.DeviceNo[i] = 'A';
			}
			else if((pFistVary.DeviceNo[i]>='A')&&(pFistVary.DeviceNo[i]<'Z'))
				pFistVary.DeviceNo[i]++;
			else if(pFistVary.DeviceNo[i] == 'Z'){
				pFistVary.DeviceNo[i] = '0';
			}
			else 
				pFistVary.DeviceNo[i]='0';
			break;
		case ESC:
			memcpy(pFistVary.DeviceNo, pFistVary.DeviceNo_1, 8);
			goto SetupDevi_exit;
		case ENTER:
#ifdef _New_Bu_mode_
			i++;
			memset(buffer,0,10);
			memcpy(buffer,pFistVary.DeviceNo,8);
			display(2,4,(char*)buffer,0);
#else
			i = 8;
#endif
			if(i >= 8){
				ret = ASCII2ZBcod((char*)pFistVary.DeviceNo, buffer, 4);
// 				printf("DeviceNo = %s\r\n", (char*)pFistVary.DeviceNo);
// 				printf("ret = %d\r\n", ret);
// 				debugdata(buffer, 10, 1);

				if(ret > 4){//���벻��ȷ
					MessageBox(0, "����λ�����࣬���������롣");
					memset(buffer,0,10);
					memcpy(buffer,pFistVary.DeviceNo,9);
					delayxms(500);
					goto Startt;
				}
				memset(buffer,0,10);
				memcpy(buffer,pFistVary.DeviceNo,9);
				sysSaveDevice(buffer);
#ifdef FengDuan_BU_
				FDParChange_QM(FDParChange_Device);
#endif

// #ifdef BUS_HANDAN_
// 				Write_Head_Record(1);
// #else
				wRiteControlRecord(ID_REC_DEVSET);
//#endif
					gsl8583Style.ISOK=0;
#ifdef OTHER_GPRS
				gprs_other_sndbuinfo();
#endif
#ifndef NO_GJ_SERVISE
				if(GPRSLinkProcess == 0x15){
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
				}
#endif
				goto SetupDevi_exit;
			}
			break;
		default:
			break;

		}
	}
SetupDevi_exit:
		;
}

void ControlDis(unsigned char type)
{
	char buffer[20];
	unsigned int temp;

	cls();
	memset(buffer,0,20);
	if(gchn_eng == 'E')
		strcpy(buffer,"NO.:");
	else
		strcpy(buffer,"����:");
	memcpy((unsigned char*)&temp,cardMessage.PublishBicker,4);
	
//	over_turn(4, (unsigned char*)&temp);
	sprintf(buffer+strlen(buffer),"%010u",temp);
	//	memcpy(buffer+5,BCD2ASC(temp.charbuffer,4),8);
	display(3,0,buffer,DIS_CENTER);
	led_on(LED_GREEN);
	switch(type) 
	{
	case CARD_DATA_GATHER:
		if(gchn_eng == 'E')
			display(0,2,"Data collection",DIS_CENTER);
		else
			display(0,2,"���ݲɼ���",1);
		beep(1, 80, 20);
		break;
	case CARD_LINE_SET:
		if(gchn_eng == 'E')
			display(0,2,"line set card",DIS_CENTER);
		else
			display(0,2,"��·Ʊ�ۿ�",1);
		beep(1, 80, 20);
		break;
	case CARD_TIME_SET:
		if(gchn_eng == 'E')
			display(0,2,"time set card",DIS_CENTER);
		else
			display(0,2,"ʱ�����ÿ�",1);
		beep(1, 80, 20);
		break;
	case CARD_LEV_BUS:
		if(gchn_eng == 'E')
			display(0,2,"depart card",DIS_CENTER);
		else
#ifdef MOBILE_TONGCHUAN_
			display(0,3,"����˾����",DIS_CENTER);
#else
			display(0,3,"������", 1);
#endif
		beep(1, 80, 20);
		break;
	case CARD_ARV_BUS:
		if(gchn_eng == 'E')
			display(0,2,"stop card",DIS_CENTER);
		else
			display(0,3,"��վ��",1);
		beep(1, 80, 20);
		break;
	case CARD_CLERK_BUS:
		if(gchn_eng == 'E')
			display(0,2,"manager card",DIS_CENTER);
		else
			display(0,4,"վ��Ա��",1);
		beep(1, 80, 20);
		break;
	case CARD_DEV_SET:
		if(gchn_eng == 'E')
			display(0,2,"set NO. card",DIS_CENTER);
		else
			display(0,3,"�豸���ÿ�",1);
		beep(1, 80, 20);
		break;
#ifdef BUS_YICHUN
	case CARD_COUPON_BUS:
#endif
#ifdef BUS_WUHAN_
	case CARD_WH_QIYE_DRIVER:
#endif
	case CARD_DRIVER_BUS:
		if(gchn_eng == 'E')
			display(0,2,"driver card",DIS_CENTER);
		else
			display(0,5,"˾����",1);
#ifdef BUS_Cloud_
		memset(buffer, 0, 20);
		if(gchn_eng != 'E')
			strcpy(buffer, "Ա����:");
		else
			strcpy(buffer, "ID:");
		BCD2Ascii(StufferNO, (unsigned char*)buffer+strlen(buffer), 4);
		display(5, 0, buffer, DIS_CENTER);
#endif
		beep(1, 80, 20);
		break;
	case CARD_AIR_BUS:
		display(0,5,"�յ��շѿ�",DIS_CENTER);
		beep(1, 80, 20);
		break;
	default:
		break;
	}

}
extern void GpsLichengClear(void);
extern unsigned int distance[2];//����ʼվ�ľ���
unsigned char DriverCardDeal(void)
{
#ifdef _DriverKey_signin_
	if(BUflag == 0){
#ifdef MOBILE_SHIYAN_
		return ST_ERROR;
#elif defined BUS_YICHUN
		return ST_ERROR; //�˴�Ա��˾��������Ҫ����Ʊ��Ǯ
#else
		FreeConsume();
		return ST_OK;
#endif
	}
#else
	unsigned int i;
#endif //#ifdef _DriverKey_signin_

	BUflag = 0;
	memset(gBusVerInfo.newbusBLKVer, 0, 2);
	if(F_FLAG_VARY.stop_flag)
	{	//����
#ifdef MOBILE_TONGCHUAN_
		audio(Audio_STUFF);
#endif
#ifdef BUS_YICHUN
		audio(Audio_zuanxian);//ǩ�����
#endif

		memcpy(DriveCardNo, cardMessage.PublishBicker, 4);
		memcpy(StufferNO, cardMessage.stuffNO, 4);
#ifdef BUS_YICHUN
		YD_BuildGPSMsginfo(StufferNO+1);
#endif
		beep(2, 50, 50);
		led_on(LED_RED);
		sysfewrite(BIT_DRICE_CARD,4,DriveCardNo);
		sysfewrite(BIT_STUFFER_NO,4,StufferNO);
		ControlDis(cardMessage.card_catalog);
		//���ĵ�վ״̬��־
		SwitchBusClose(0);
		wRiteControlRecord(ID_REC_LEVBUS);				//��������¼
		wRiteControlRecord(ID_REC_DRIVENO);				//˾������¼
#ifdef KEYBOARD
		keyBoardStart();
#endif

#ifdef _GPS_ENABLE_
		if((gGpsInfo.ucValid != GPSINVALID)&&(distance[0]<500||distance[1]<500))//���������յ�
		{
			GpsLichengClear();// �������������
			WriteGPSRec(GPS_MainIn);//��վ��¼
		}
		else
		{
			
			MessageBox(0,"������վ ����GPS��Ч");
			delayxms(1000);
			//debugstring("������վ ����GPS��Ч\r\n");
		}
		
#endif
		return ST_OK;		
	}
	else
	{//��վ
#ifndef _DriverKey_signin_
		i = memcmp(DriveCardNo, cardMessage.PublishBicker, 4);
		if(i != 0){//����ͬһ��˾����,����
#ifdef MOBILE_SHIYAN_
			return ST_ERROR;
#elif defined BUS_YICHUN
			return ST_ERROR; //�˴�Ա��˾��������Ҫ����Ʊ��Ǯ
#else
			FreeConsume();
			return ST_OK;
#endif
		}
#endif 

		memcpy(DriveCardNo, cardMessage.PublishBicker, 4);
		memcpy(StufferNO, cardMessage.stuffNO, 4);
		sysfewrite(BIT_DRICE_CARD,4,DriveCardNo);
		sysfewrite(BIT_STUFFER_NO,4,StufferNO);
#ifdef MOBILE_TONGCHUAN_
	#ifdef AUDIO_SOUND_OLD
			audio_old(Audio_DONG);
	#else
			audio(Audio_DONG);
	#endif
			SoundMessage(SOUND_DI_BUS);
			delayxms(100);
	#ifdef AUDIO_SOUND_OLD
			audio_old(Audio_DONG);
	#else
			audio(Audio_DONG);
	#endif
#elif defined BUS_YICHUN
		audio(Audio_zuanxian);//ǩ�����
		YD_BuildGPSMsginfo(StufferNO+1);
#else
		SoundMessage(SOUND_DI_BUS);
#endif
		ControlDis(cardMessage.card_catalog);
		wRiteControlRecord(ID_REC_DRIVENO);				//˾������¼
#ifndef _BU_NO_STOP_
		SwitchBusClose(1);
		wRiteControlRecord(ID_REC_ARVBUS);			//��վ����¼
#else
		SwitchBusClose(0);
		beep(2, 100, 300);
		led_on(LED_RED);
#endif
#ifdef _GPS_ENABLE_
		if((gGpsInfo.ucValid != GPSINVALID)&&(distance[0]<500||distance[1]<500))//���������յ�
			WriteGPSRec(GPS_MainIn);
		else
		{
			MessageBox(0,"������վ ����GPS��Ч");
			debugstring("������վ ����GPS��Ч\r\n");
			delayxms(1000);
		}
			WriteGPSRec(GPS_LICHENG);//
			WriteGPSRec(GPS_LiCheng30);//
#endif
#ifndef NO_GJ_SERVISE
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
#endif			
#ifndef NO_MOBILE_SERVISE
		if(gMobilREC_Start == gMobilREC_End){
			memset((unsigned char*)&gMobilStyle, 0, sizeof(stMobilStyle));//��Ҫ����ǩ��
			sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
		}
		else{
			uploadSettleData();//�������
		}
#endif
		return ST_OK;	
	}
}
void ShowSetInfo(void);
extern void set_line_cpu(unsigned char mode);
//������ⳬʱ��ʱ�Ļ�׼ֵ����Ϊ407 CPU��103��2.3������407Ҫ���������ó�23�������������ʹ��Ĭ��ֵ
extern void setIrdaBaseDly(unsigned int bdly);
void ControlMagane(void)
{
//#ifdef BUS_Cloud_
//	char disbuf[20];
//#endif
//	unsigned int i;


	MSG_LOG("--control manage---\r\n");

	restore_flag = 3;
#ifdef Card_typeM_Min_is0_
#ifndef BUS_EZOU_
	switch(cardMessage.card_catalog)
	{
	case CARD_ARV_BUS:
		break;
	case CARD_LEV_BUS:
	case CARD_DEV_SET:
	case CARD_LINE_SET:
	case CARD_TIME_SET:
		if(!F_FLAG_VARY.stop_flag)
		{
			MessageBox(1, Card_NodealCard);
			led_on(LED_RED);
			end_card();
			return;
		}
		break;
	case CARD_CLERK_BUS:
	case CARD_DRIVER_BUS:
	default:
		break;
	}
#endif
#else //#ifndef BUS_Cloud_
#ifndef BUS_Cloud_
	switch(cardMessage.card_catalog)
	{
	case CARD_ARV_BUS:
#ifdef BUS_HANDAN_
		if(F_FLAG_VARY.stop_flag)
		{
			led_on(LED_RED);
			return;
		}
#endif
		break;
	case CARD_LEV_BUS:
#ifndef BUS_HANDAN_
		break;
#endif
	case CARD_DEV_SET:
	case CARD_LINE_SET:
	case CARD_TIME_SET:
	case CARD_AIR_BUS:
// 		if(!F_FLAG_VARY.stop_flag)
// 		{
// 			MessageBox(1, Card_NodealCard);
// 			led_on(LED_RED);
// 			
// 			end_card();
// 			return;
// 		}
		break;
	case CARD_CLERK_BUS:
	case CARD_DRIVER_BUS:
	default:
		break;
	}
#endif //#ifndef BUS_Cloud_

#endif//#ifdef Card_typeM_Min_is0_
	
	switch(cardMessage.card_catalog)
	{
		//#ifdef BUS_PRO_JSB
	case CARD_DATA_GATHER:
		beep(1,100,10);
		setIrdaBaseDly(23);
#ifdef NO_GJ_SERVISE
		USART2mode = _TEST_Irda;
// #ifdef BUS_HANDAN_
// 		get_data(1);
// #else
		get_data(0);
//#endif
		USART2mode = _TEST_GPRS;
#else
		USART2mode = _TEST_Irda;
// #ifdef BUS_HANDAN_
// 		get_data(1);
// #else
		get_data(0);
//#endif
#ifdef WIFI_TLH_
		GPRS_INIT(115200);
#else
		GPRS_INIT(9600);
#endif
		USART2mode = _TEST_GPRS;
#endif			
		restore_flag=1;
		break;
#ifdef BUS_Cloud_
	case CARD_Business:
// 		beep(1, 100, 10);
// 		saveCpuInfo(7, cardMessage.TradeCode);//�̻���
// 		cls();
// 		display(0,0,"�̻����ÿ�", DIS_CENTER);
// 		display(3,0,"�̻���Ϣ�������", 0);
// 		memset(disbuf, 0, 20);
// 		strcpy(disbuf, "�̻���:");
// 		BCD2Ascii(cardMessage.TradeCode, (unsigned char*)disbuf+7, 2);
// 		display(5, 0, disbuf, 0);
		break;
#endif
	case CARD_DEV_SET:
		beep(1,100,10);
#ifdef BUS_JIUJIANG_
		ShowSetInfo();
#else
		ShowSetInfo();
//		set_device();
#endif
//		wRiteControlRecord(cardMessage.card_catalog);
		break;
	case CARD_LINE_SET:
#ifdef BUS_Cloud_
		F_FLAG_VARY.set_line_status = 1;
#elif defined BUS_EZOU_ || defined BUS_KUITUN_
		F_FLAG_VARY.set_line_status = 1;	//����Ҫˢվ��Ա��
#endif
		if(F_FLAG_VARY.set_line_status){
#ifdef BUS_Cloud_
			set_line_cloud();
#else
#ifdef BUS_PRO_JTB
			if(	gMCardCand== CARDSTYLE_JTB)
				set_line_cpu(0);
			else
#endif
				set_line();
				
#endif
#ifdef OTHER_GPRS
			gprs_other_sndbuinfo();
#endif
			SetAudio();//��������ǹ̶����ģ���������·������Ҫ���³�ʼ��.����������Ҫ�ٵ���һ��

//			wRiteControlRecord(ID_REC_LINENO);
// 			SoundMessage(SOUND_DI_BUS);
// 			ControlDis(cardMessage.card_catalog);
		}
		else{
			cls();
			
			if(gchn_eng == 'E')
				display(2,2,"Show manager card",0);
			else
				display(2,2,"��ˢվ��Ա��",0);
		}
		break;
	case CARD_FENDUAN_Line:
#ifdef FengDuan_BU_
		set_FengDuan_line();
#endif
		break;
	case CARD_TIME_SET:	
		beep(1,100,10);
		SYSsetdatetime();
		break;
	case CARD_ARV_BUS:	//�����ĵ�վ�� ,ֻ�ܵ�վ
#ifdef BUS_BAOJI_
		if(!F_FLAG_VARY.stop_flag){
			SwitchBusClose(1);
			wRiteControlRecord(ID_REC_ARVBUS);
			ControlDis(CARD_ARV_BUS);	
			SoundMessage(SOUND_DI_BUS);				
		}
		break;
	case CARD_LEV_BUS:	//�����ķ���,ֻ�ܷ���
		if(F_FLAG_VARY.stop_flag){
			//memcpy(DriveCardNo, cardMessage.PublishBicker, 4);
			//fewrite(BIT_DRICE_CARD,4,DriveCardNo);
			SwitchBusClose(0);
			wRiteControlRecord(ID_REC_LEVBUS);
			ControlDis(CARD_LEV_BUS);
			SoundMessage(SOUND_DI_BUS);	
		}
		break;
#else		//#ifdef BUS_BAOJI_
	case CARD_LEV_BUS:
//		MSG_LOG("bb2");
		memset(gBusVerInfo.newbusBLKVer, 0, 2);
		if(F_FLAG_VARY.stop_flag)
		{	//����
//		MSG_LOG("bb3");
			SoundMessage(SOUND_DI_BUS);
			memcpy(DriveCardNo, cardMessage.PublishBicker, 4);
			sysfewrite(BIT_DRICE_CARD,4,DriveCardNo);
			ControlDis(cardMessage.card_catalog);
			//���ĵ�վ״̬��־
			SwitchBusClose(0);
			wRiteControlRecord(ID_REC_LEVBUS);				//��������¼
			wRiteControlRecord(ID_REC_DRIVENO);				//˾������¼
#ifdef _GPS_ENABLE_
			WriteGPSRec(0xE0);
#endif
#ifdef KEYBOARD
			keyBoardStart();
#endif
		}
		else
		{//��վ
//		MSG_LOG("bb4");
			SoundMessage(SOUND_DI_BUS);
			ControlDis(cardMessage.card_catalog);
			SwitchBusClose(1);
			wRiteControlRecord(ID_REC_DRIVENO);				//˾������¼
			wRiteControlRecord(ID_REC_ARVBUS);			//��վ����¼
#ifndef NO_GJ_SERVISE			
			if(GPRSLinkProcess == GPRS_SENDING_CMD)
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
#endif			
#ifndef NO_MOBILE_SERVISE
			if(gMobilREC_Start == gMobilREC_End){
				memset((unsigned char*)&gMobilStyle, 0, sizeof(stMobilStyle));//��Ҫ����ǩ��
				sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
			}
			else{
				uploadSettleData();//�������
			}
#endif
#ifdef _GPS_ENABLE_
			WriteGPSRec(0xE0);
#endif
		}
#ifdef OTHER_GPRS
		gprs_other_sndbuinfo();
#endif
//-----------------------
#warning "�÷���������USB�ɼ�" 
			main_usb_collect();	
//			return;
//-----------------------
		break;
#endif	//#ifdef BUS_BAOJI_
	case CARD_DRIVER_BUS:
#ifdef BUS_BAOJI_ //�������ô˷���
		if(F_FLAG_VARY.stop_flag)
		{	//����
			SoundMessage(SOUND_DI_BUS);
			memcpy(DriveCardNo, cardMessage.PublishBicker, 4);
			fewrite(BIT_DRICE_CARD,4,DriveCardNo);
			ControlDis(cardMessage.card_catalog);
			//���ĵ�վ״̬��־
			wRiteControlRecord(ID_REC_DRIVENO);				//˾������¼
		}
#else
		DriverCardDeal();
		
#ifdef OTHER_GPRS
		gprs_other_sndbuinfo();
#endif

#endif
		break;
	case CARD_CLERK_BUS:	
		if(F_FLAG_VARY.stop_flag)
		{
			ControlDis(CARD_CLERK_BUS);
			restore_flag=1;
			wRiteControlRecord(ID_REC_CLERK);
			F_FLAG_VARY.set_line_status=1;
			SoundMessage(SOUND_DI_BUS);
		}
		break;
	case CARD_AIR_BUS:
		ControlDis(CARD_AIR_BUS);
		if(F_FLAG_VARY.PlusSta == 0)
			F_FLAG_VARY.PlusSta = 1;
		else
			F_FLAG_VARY.PlusSta = 0;
		if(F_FLAG_VARY.PlusSta == 0){
			display(6,0,"�յ��ѹر�",DIS_CENTER);
		}
		else{
			display(6,0,"�յ��Ѵ�",DIS_CENTER);
		}
		restore_flag=1;
		fewrite(BIT_F_FLAG_VARY,LEN_F_FLAG_VARY,(unsigned char*)&F_FLAG_VARY);
		wRiteControlRecord(ID_REC_AIRCON);
		break;
	}
	end_card();
	restore_flag =1;
	return;	
}
void save_normal(void)
{
 	memcpy(PCard_delay.CardSno,(unsigned char *)&c_serial,4);
 	PCard_delay.Sec=SysTime.seconds;
}
unsigned char Delay_Normal(void)
{
#if NORM_DLY_TIME==0
	return 0;
#else
	char i;
	if(memcmp(PCard_delay.CardSno,(unsigned char *)&c_serial,4))
	{
		memset(PCard_delay.CardSno,0,4);
		return 0;
	}
	i=SysTime.seconds-PCard_delay.Sec;
	if(i>NORM_DLY_TIME)
	{
		memset(PCard_delay.CardSno,0,4);
		return 0;
	}
	else return 1;
#endif
}
void disp_no_swipe(void)
{
	char dnssbuf[20];

	if(F_FLAG_VARY.stop_flag)
		return;
	
	restore_flag=1;
	led_off(LED_GREEN|LED_RED|LED_BLUE|LED_YELLOW);
	led_on(LED_RED);
	cls();

	getCardtype(dnssbuf, cardMessage.card_catalog);
	display(0,0,dnssbuf, DIS_CENTER);
	if(gchn_eng == 'E')
		display(4,1,"card constants!",DIS_CENTER);
	else	
		display(4,1,"���ܴ��˳˳�!",DIS_CENTER);
// #if (defined(BUS_HANDAN_) || defined(BUS_PINGXIANG_) || defined(BUS_EZOU_))
// 	return;
// #else
#ifdef ZongKong_CPU_ //�пص�CPU��������
	audio(Audio_DingDang);
#elif defined SOUND_New_B
	audio(Audio_NODRen);
#elif defined BUS_NanjingDZ_
 	audio(Audio_FLING_MONEY);	//���еĳ��򶼲���ʾ��Ͷ���ˡ�
#endif
//#endif
}

unsigned char MoneyResultManage(unsigned char mode);
unsigned char YearResultManage(unsigned char mode)
{
	unsigned char pabuf[72];
	unsigned char ucRnt, mothFlag;
	unsigned int ii;
	
	mode=mode;
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return ST_ERROR;
	}	
	
	if(F_FLAG_VARY.stop_flag) 
		return ST_ERROR;
	
	mothFlag = cardMessage.card_catalog;
	if(mothFlag >= 0x40)
		mothFlag -= 0x40;
	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
	{
		disp_no_swipe();
		return ST_OK;
	}

	if(Delay_Normal())
		return ST_ERROR;
	
	ucRnt = PurseProcessYear(0);															//�����׽���

	MSG_LOG("Year Purse Process = ");
	BCD_LOG(&ucRnt,1, 1);

	switch(ucRnt)    //96ms
	{
	case ST_ERROR:						//��ͨ����
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//����Ǯ��������
		error(ERR_MONEY_SN, 0);
		return ST_OK;
	case 22:
		display(0,0,"��ʾ:",DIS_CONVERT|DIS_Cls);
		display(3,0,"Ʊ�۲���ȷ!", DIS_CENTER);
		return ST_OK;
	case 2:
	case 3:																				//����
		if(MoneyResultManage(3)==ST_ERROR)
			return ST_ERROR;
		else
			return ST_OK;
	case 6:
#ifdef CARD_SOUND
		card_ser = c_serial;
		cardSound = 0xaa;
#endif
		break;
	case 5:
	case 7:
	case 8:
		dis_sum2 = a_sum1;
		
#ifdef CARD_SOUND
		cardSound=0; 
#endif
		save_h_month();

		money_msg(ID_REC_MON, a_sum1, s_sum1, 0);
		//if(gucSoudnorm == 0){
		//if(cardMessage.oddMonthSector!=0xff)
		//	audio(SetupAudio[CARD_NORMAL_BUS]);//����Ʊ��Ǯ����ͨ����
		//else
			SoundMessage(SOUND_DEAL);
		//			}
		//			else
		//				SoundMessage(SOUND_CHARGE_MONEY);	//��ʾ���ֵ
			
		BuildRecorde(ID_REC_MON, pabuf);
		card_ser = c_serial;
		ComSumeFile.SumMti += s_sum1; //��ǰ�����ܶ�
		ii = ((unsigned char*)&ComSumeFile.SumMti)-((unsigned char*)&ComSumeFile);
		sysfewrite(BIT_COMSUME_FILE+ii, 4 ,(unsigned char*)&ComSumeFile.SumMti);
		WriteRecord(pabuf, 0);
//			PleaseYearCheck((struct com_time *)nowTime.charbuffer,(struct com_time *)endTime.charbuffer);
		return ST_OK;
	default:
		break;
	}		
	return ST_ERROR;
}

unsigned char MoneyResultManage(unsigned char mode)
{
	unsigned char pabuf[RECORD_LEN];
	unsigned char ucRnt, mothFlag;
	unsigned int ii;
	
	mode=mode;
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return ST_ERROR;
	}	
	
	if(F_FLAG_VARY.stop_flag) 
		return ST_ERROR;
	
	mothFlag = cardMessage.card_catalog;
	if(cardMessage.card_catalog >= 0x40)
		mothFlag -= 0x40;
// #ifdef BUS_NanjingDZ_
// 	if((cardMessage.card_catalog != CARD_NORMAL_BUS)&&(cardMessage.card_catalog != CARD_ZILU_BUS))
// 	{
// #endif
// 	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
// 	{
// 		disp_no_swipe();
// 		return ST_OK;	
// 	}
// #ifdef BUS_NanjingDZ_
// 	}
// #endif

	if(Delay_Normal())
		return ST_ERROR;
#ifdef _Counter_SWcard_times	//ˢ������������
	ii = checkSCtimes();
	if(ii == ST_ERROR){//������ˢ����
		cls();
		restore_flag = 1;
		MessageBox(0, "��������,�뷢��!");
		beep(8, 50, 50);	//û�к��ʵ�����
		return ST_OK;
	}
#endif

	ucRnt = PurseProcess(mode);															//�����׽���

	MSG_LOG("Purse Process = ");
	BCD_LOG(&ucRnt,1, 1);

	switch(ucRnt)    //96ms
	{
	case ST_ERROR:						//��ͨ����
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//����Ǯ��������
		error(ERR_MONEY_SN, 0);
		return ST_OK;
	case 22:
		if(gchn_eng == 'E'){
			display(0,0,"hint:", DIS_Cls);
			display(2,0,"price is error",DIS_CENTER);
		}
		else{
			display(0,0,"��ʾ:", DIS_Cls);
			display(2,0,"Ʊ����Ϣ����ȷ",DIS_CENTER);
		}
		return ST_OK;
	case 2:
	case 3://����
#ifdef BUS_YICHUN
		beep(3, 200, 500);
#endif
		led_on(LED_RED);
		return ST_OK;
	case 4:
	case 6:
#ifdef CARD_SOUND
#ifdef _AREA_ENG_
		beep(3, 100, 50);
#else
#ifndef MOBILE_SHIYAN_
#ifndef BUS_PINGXIANG_
#ifdef AUDIO_SOUND_OLD
		audio_old(Audio_TRY_AGAIN);		//����ˢ

#else
		audio(Audio_TRY_AGAIN);		//����ˢ
#endif
#endif 
#endif
#endif
		card_ser = c_serial;
		cardSound = 0xaa;
#endif
		break;
	case 5:
	case 7:
	case 8:
// #ifdef _PURSE_OLD_STYLE//����Ϊ�Ϸ�ʽ����Ҫд�빫����Ϣ
// #ifndef BUS_BAOJI_		//�����Ŀ��Ͳ�д�����ˡ�
// 		PurchasOk_Money(1);
// #endif 
// #endif
		if(gchn_eng == 'E')
			audio(Audio_DONG);
		else
			Sound_cityCMD(mode);
#ifdef CARD_SOUND
		cardSound=0;
#endif
		Save_delayCardinfo(mode);

		money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
		
		ComSumeFile.SumMoney+=s_sum1; //��ǰ�����ܶ�
		BuildRecorde(ID_REC_TOLL, pabuf);
		card_ser = c_serial;
		ii = ((unsigned char*)&ComSumeFile.SumMoney)-((unsigned char*)&ComSumeFile);
		sysfewrite(BIT_COMSUME_FILE+ii, 4 ,(unsigned char*)&ComSumeFile.SumMoney);
		
		MSG_LOG("money WriteRecord !!\r\n");
		WriteRecord(pabuf, 0);

		MSG_LOG("money puchase ok!!\r\n");

		
#ifdef BUS_HBYS_//����ͳ��
		save_month_tongji(ID_REC_TOLL);
#endif
//			PleaseYearCheck((struct com_time *)nowTime.charbuffer,(struct com_time *)endTime.charbuffer);
		return ST_OK;
	default:
		break;
	}		
	return ST_ERROR;
}

void dis_time_mini(unsigned char x, stcomtime *time)
{
	unsigned int pos;
	unsigned char buff[30];
	pos = 0;
	memset(buff,0,17);
	BCD2Ascii((unsigned char*)&time->year_h, buff+pos, 1); pos+=2;
	BCD2Ascii((unsigned char*)&time->year_l, buff+pos, 1); pos+=2;
	buff[pos++] = '-';
	BCD2Ascii((unsigned char*)&time->month, buff+pos, 1); pos+=2;
	buff[pos++] = '-';
	BCD2Ascii((unsigned char*)&time->day, buff+pos, 1); pos+=2;
	buff[pos++] = ' ';
	BCD2Ascii((unsigned char*)&time->hours, buff+pos, 1); pos+=2;
	buff[pos++] = ':';
	BCD2Ascii((unsigned char*)&time->minutes, buff+pos, 1); pos+=2;
	buff[pos++] = ':';
	BCD2Ascii((unsigned char*)&time->seconds, buff+pos, 1); pos+=2;
	buff[pos++] = 0;

	display(x, 1, (char*)buff, DIS_CENTER);
}
extern unsigned char RC531ReadReg( unsigned char reg );
void face_time(void)
{
	static unsigned int DisDly=0;
	unsigned int temp;
#ifdef _GPS_ENABLE_
	unsigned char t;
#endif
//	unsigned short sret;
//#ifdef MOBILE_SHIYAN_
//	char buff[20];
//#endif
//	if(F_FLAG_VARY.stop_flag)
//		return;
	if(DisDly++ < 100)//
		return;
	
	if(g24GDisFlash > 0){
		g24GDisFlash--;
		face_DisTime = 0;
		if(g24GDisFlash == 0){//�������ӳ���ʾ
			restore_flag = 3;
			restore_disp();
		//	led_off(0xff);
		}
	}

	DisDly = 0;
	GetDateTime();
//	if(==ST_OK)
//	{
// #ifdef _GPS_ENABLE_
// 		if(gGpsDisEN != 'G')
// #endif
		dis_time_mini(14, (stcomtime*)&SysTime);//��ʾʱ��
		
//	}
	
	face_DisTime ++;
#ifndef NO_M1_Purse
	if((face_DisTime%270) == 0)
	{
		show_err();
#ifndef _GPS_CAIDIAN_
#ifndef _GPS_TRA_REC_
//		delayxms(10);
		disinit();
//		face_DisTime =0;
		restore_flag = 3;
#endif
#endif
// #ifdef _FASH_FATs_SYS_
// 		close_rfile_Wrecord();
// 		open_rfile_Wrecord();
// 		flasherase(ERASE4KBYTESECTOR,FLASH_RECORD_POINT);
// 		temp = 0;
// 		save_bit_tmprec_point(&temp);
// #endif

		audio_reset();
		face_DisTime ++;
	}

/*	if((face_DisTime%10) == 0){//
		if(RC531FIFOLevelCheck() == ST_ERROR){//��֤����
// 			debugdata((unsigned char*)&temp, 4, 1);
// 			debugstring("531init E ");
			delayxms(100);
 			ReStartCPU();
		}
//	   	debugstring("|");
	}*/
	
#endif
// #ifdef _New_Bu_mode_ 
// 	if((face_DisTime%2) == 0)
// 	{
// 		led_convert(LED_GREEN|LED_YELLOW|LED_BLUE|LED_RED);
// 	}
// #endif
#ifndef NO_MOBILE_24G_
	if((face_DisTime%1300)==0){//���G24��ͷ��״̬
		Check24Card();
	}
#elif defined _CPU_24G_
	if((face_DisTime%1300)==0){//���G24��ͷ��״̬
		Check24Card();
	}
#endif

	temp = ((SysTime.hours>>4)&0x0f)*10+(SysTime.hours & 0x0f);
	temp *= 60;
	temp += ((SysTime.minutes>>4)&0x0f)*10+(SysTime.minutes & 0x0f);
	if((temp < 1050)&&(temp > 390)){//17:30
		if(gucBLight>0){
			gucBLight--;
			if(gucBLight==0){
#if _backLight_on_
				dislight(0);
#endif
				;
			}
		}
	}
	else{
		BlightOpen();	//����17:30���ⳣ��
	}
	
	temp = 0;
	memcpy((unsigned char*)&temp, (unsigned char*)&SysTime.hours, 3);
	over_turn(3, (unsigned char*)&temp);//����ʱ��
	if((temp == 0x00003000)||	//00:30:00
		(temp == 0x00010000)||	//01:00:00
		(temp == 0x00060000)||	//06:00:00
		(temp == 0x00110000)){	//11:00:00
		ReStartCPU();
	}

#ifdef _GPS_ENABLE_
	t = 1;
#ifdef FengDuan_BU_
	if(gFDMode == FDMODE_NORMAL)//���Ż�����ʾGPS״̬
		t = 0;
#endif
	showGps(t);
#endif
}
//���ʱ���Ƿ���ȷ
//�ж�ʱ���Ƿ���ȷ
unsigned char IF_TIME_OK(void)
{
	unsigned short Y;
	if(GetDateTime()==ST_ERROR)
		return ST_ERROR;
	memcpy((unsigned char*)&Y,(unsigned char*)&SysTime,2);
	if (Y>0x5020||Y<0x1620)//ʱ��������2050�����С��2015��  ����
	{
		return ST_ERROR;
	}else if (SysTime.month>0x12||SysTime.day>0x31||SysTime.minutes>0x60||SysTime.seconds>0x60)//��Ϊ�Ǵ��
	{
		return ST_ERROR;
	}
	return ST_OK;
	
}
unsigned char Time_OK;
unsigned char check_time(void)
{
	//	unsigned char  ret;
#ifdef _debug_
			return ST_OK;//OK
#endif
	if(Time_OK!=LogOK)//û��¼
	{
		if (IF_TIME_OK()==ST_ERROR){
			
			cls();
			beep(4,50,50);
			display(0,0,"����:",0);
			display(2,0,"     ʱ�䲻��!",0);
			display(4,0,"  ������ʱ��",0);
			restore_flag=0;
			g24GDisFlash=3;
			return ST_ERROR;
		}
		else{
			
			return ST_OK;//OK
			
		}
	}
	return ST_OK;
}
//��ʾ�ǵ��Եİ汾
void Dis_Debug(void)
{
	//��ʾ �ǵ�����Ϣ
#if defined _GPS_TEST_ || defined _debug_ || defined _debug_CPU_ || defined _Money_purchase_test

	miniDispstr(1,37,"BUG",0);

#endif
}
void main_face(void)
{

	
	if(restore_flag) //DriveCardNo DeviceNo pFistVary.LineNo
	{
		led_off(LED_GREEN|LED_RED|LED_BLUE|LED_YELLOW);
		
		restore_disp();
		restore_flag = 0;
		Dis_Debug();
	}
	face_time();
}

void Find_History(unsigned int Index,unsigned char *buff)
{
	unsigned int addr;
	unsigned int curp;

	if((ghisCur <= FLASH2_START1)||(ghisCur >= FLASH2_START2END)){
		writeBackRec(buff, 0, 0xA5);
	}
	curp = ghisCur;

	addr=(Index+1) * RECORD_LEN;
	MSG_LOG("index:%d\n\r",Index);
	
	MSG_LOG("History:addr:");
	BCD_LOG((unsigned char*)&addr, 4, 1);
	MSG_LOG("History:ghisCur:");
	BCD_LOG((unsigned char*)&ghisCur, 4, 1);
	if(addr>(curp-FLASH2_START1))
	{
		addr=FLASH2_START2END-(addr-(curp-FLASH2_START1));
	}
	else
		addr=curp-addr;

	MSG_LOG("History:addr:0x%04X\r\n",addr);
	MSG_LOG("History:ghisCur:0x%04X\r\n",ghisCur);
	

	flashread(addr, buff, RECORD_LEN);



}

void dis_g24Record_GJ(unsigned char *Trecord, unsigned int Index)
{
	unsigned char buffer[50];
	unsigned int temp;
	MobileDEALRECODE historyCheck;

	memcpy((unsigned char*)&historyCheck, Trecord, RECORD_LEN);

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
	if(temp > 200000)
		temp %= 10000;
	if(gchn_eng == 'E')
		sprintf((char*)buffer, "balance:%d.%02d", temp/100, temp%100);
	else
		sprintf((char*)buffer, "���:%d.%02d", temp/100, temp%100);
	display(4,0,(char*)buffer,0);
	dis_time_mini(6, (stcomtime*)&historyCheck.date);
	
	sprintf((char*)buffer, "%d ", Index+1);
	miniDispstr(7, (21-strlen((char*)buffer)), (char*)buffer, 0);
}
#ifdef _card_guard_
void dis_kq_rec(unsigned char *ret, unsigned int Index);
#endif

#ifdef qPBOC_BUS
extern unsigned char qPbocDisp_history(unsigned char mode, unsigned int Index);
#endif
#ifdef CS456_ASCii_
	extern void disCS456_ASCIIRec(unsigned char *RRebuf);
#endif
	
extern unsigned char qrDisp_history(unsigned char mode, unsigned int Index);

unsigned char Disp_history(unsigned char mode, unsigned int Index)
{
	unsigned char buffer[RECORD_LEN*2];
	char disbuf[30];
	char btff[20];
	unsigned int temp;//,temp1;
	
// #ifdef BUS_HANDAN_
// 	DEALRECODE_32B historyCheck;
// #else
#ifdef QR_CODE_COM
	DEALRECODE_QRC *rRecordDeal;
unsigned long long ultemp=0;
#endif

 	RECORD_GPS gpsRec ;



	DEALRECODE historyCheck;


//#endif
	

	if(qPbocDisp_history(mode, Index) == ST_OK){
		MSG_LOG("return EA\r\n");
		return 0xEA;
	}
	if(qrDisp_history(mode, Index) == ST_OK){
		MSG_LOG("return qr EA\r\n");
		return 0xEA;
	}

	Find_History(Index,buffer);

#ifdef _debug_
	debugstring("find:");
	debugdata((unsigned char*)&Index, 4, 1);
	debugdata((unsigned char*)&historyCheck, RECORD_LEN, 1);
	debugdata((unsigned char*)&historyCheck.rDealType, 1, 1);
#endif





// #ifdef BUS_PRO_JTB	
// 	if(memcmp(buffer+RECORD_JTB_LEN-4,"\xAA\xAA\xAA\xAA",4)==0){//��ͨ����
// 	
// 		Disp_historyJTB(buffer,Index);
// 		
// 		return ST_OK;
// 	}else
// #endif
//	{//��ǰ�ϵ�����
		//DEALRECODE historyCheck;
		memcpy((unsigned char*)&historyCheck,buffer,RECORD_LEN);
//	}


	MSG_LOG("find:");
	BCD_LOG((unsigned char*)&Index, 4, 1);
	BCD_LOG((unsigned char*)&historyCheck, 64, 1);
	BCD_LOG((unsigned char*)&historyCheck.rDealType, 1, 1);

	memset(buffer, 0xff, 70);
	if(memcmp(buffer, (unsigned char*)&historyCheck, RECORD_LEN) == 0){
		return 0xEE;
	}

#ifdef QR_CODE_COM
	
	
	if (historyCheck.rDealType==ID_REC_QRC_SLZR) 
	{
		rRecordDeal = (DEALRECODE_QRC *)&historyCheck;
	}
#endif

	temp=0;
	switch((historyCheck.rDealType))
	{
#ifdef BUS_SYBAOSHI_
	case ID_REC_DXBAOSI:
		cls();
		display(0,0,Conn(_Font_showCardNo, ":"),0);
		memset(btff, 0, 20);
		BCD2Ascii(historyCheck.rPublishNumb, (unsigned char*)btff, 4);
		display(3, 0, btff, DIS_CENTER);
		dis_time_mini(6, (stcomtime*)&historyCheck.rDealTime);
		
		sprintf((char*)buffer, "%d ", Index+1);
		miniDispstr(7, (21-strlen((char*)buffer)), (char*)buffer, 0);
		return ST_OK;
#endif
#ifdef CS456_ASCii_
	case ID_REC_CS456_ASCii:
		disCS456_ASCIIRec((unsigned char*)&historyCheck);
		sprintf((char*)buffer, "%d ", Index+1);
		miniDispstr(7, (21-strlen((char*)buffer)), (char*)buffer, 0);
		return ST_OK;
#endif
#ifdef _debug_
	case ID_REC_History:
#endif
	case ID_REC_TOLL:
	case ID_REC_MOBILE:
		cls();
#ifdef ZongKong_CPU_
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
		if(temp == 0){
			strcpy((char*)buffer, _Font_showCardtype);
			sprintf((char*)buffer+strlen((char*)buffer), ":%d", historyCheck.rCardType);
			display(2,0,(char*)buffer,0);
			sprintf((char*)buffer, _Font_KouKuandian, 0, 0);
			display(4,0,(char*)buffer,0);
		}
		else{
			memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
			sprintf((char*)buffer, _Font_KouKuandian, temp/100, temp%100);
			display(2,0,(char*)buffer,0);
			memset(buffer,0,20);
			temp=0;
			memcpy((unsigned char*)&temp, historyCheck.rAfterMoney, 4);
			
			sprintf((char*)buffer, _Font_Balancedian, temp/100, temp%100);
			display(4,0,(char*)buffer,0);
		}
#else
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
		sprintf((char*)buffer, _Font_KouKuandian, temp/100, temp%100);
		memset(btff, 0, 20);//���뿨��
//		getCardtype(btff, historyCheck.rCardType);
//		strcat((char*)buffer, btff);

		display(2,0,(char*)buffer,0);
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp, historyCheck.rAfterMoney, 4);
		
		sprintf((char*)buffer, _Font_Balancedian, temp/100, temp%100);
		display(4,0,(char*)buffer,0);
#endif
		
		//dis_time_mini(6, (stcomtime*)&historyCheck.rDealTime);
		break;
#ifdef QR_CODE_COM
		

	case ID_REC_QRC_SLZR:

		cls();


		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,rRecordDeal->rDealMoney,3);
		sprintf((char*)buffer, _Font_KouKuandian, temp/100, temp%100);
		memset(btff, 0, 20);//���뿨��
		//		getCardtype(btff, historyCheck.rCardType);
		//		strcat((char*)buffer, btff);
		
		display(2,0,(char*)buffer,0);

		break;
#endif

	case ID_REC_MON:
		cls();
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,historyCheck.rAfterMoney, 4);

	//	memcpy(buffer,(unsigned char *)"�۴�:1",6);
		//sprintf(buffer+5,"%lu",temp.longbuffer);
		temp = 0;
		memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
		if(temp == 0){
			if(historyCheck.rCardType==10)
				strcat((char*)buffer, "˾����");
			else
				getCardtype((char*)buffer, historyCheck.rCardType);
			display(3,0,(char*)buffer, DIS_CENTER);
		}
		else{
			temp=0;
			memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
			sprintf((char*)buffer, _Font_KouCIdian, temp);

			memset(btff, 0, 20);//���뿨��
//			getCardtype(btff, historyCheck.rCardType);
//			strcat((char*)buffer, btff);
			display(2,0,(char*)buffer,0);

			memset(buffer,0,20);
			memcpy((unsigned char*)&temp, historyCheck.rAfterMoney, 4);
			sprintf((char*)buffer, _Font_BalanceCI, temp);
			display(4,0,(char*)buffer,0);
		}
		break;
// 	case ID_REC_MOBILE:
// 		dis_g24Record_GJ((unsigned char*)&historyCheck, Index);
// 		return ST_OK;
#ifdef _card_guard_
	case 85://���ڼ�¼
		dis_kq_rec((unsigned char*)&historyCheck, Index);
		return ST_OK;
#endif
	case ID_REC_CLERK:
		cls();
		display(3, 0, Card_Name_ZWYcard, DIS_CENTER);
		break;
	case ID_REC_DRIVENO:
		cls();
		display(3, 0, Card_Name_Driver, DIS_CENTER);
		break;
	case ID_REC_LEVBUS:
		cls();
		display(3, 0, Card_Name_departcard, DIS_CENTER);
		break;
	case ID_REC_ARVBUS:
		cls();
		display(3, 0, Card_Name_stopcard, DIS_CENTER);
		break;
	case 0x50:		//pfquan 
		cls();
		display(3, 0, Card_Name_Blkcard, DIS_CENTER);
		break;
		//gpsjilu 
	case ID_REC_GPS:
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy((unsigned char*)&temp,gpsRec.LiCeng,4);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		sprintf(btff,"�����:%d.%dKM",temp/1000,temp%1000);
		display(2,0,btff,0);
		
		break;
	case ID_REC_GPS_30:
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy((unsigned char*)&temp,gpsRec.LiCeng,4);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		sprintf(btff,"�����:%d.%dKM",temp/1000,temp%1000);
		display(2,0,btff,0);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		break;
	case  ID_REC_GPS_JZ:
	case ID_REC_GPS_OF:
	case ID_REC_GPS_IN:
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		sprintf(btff,"��վ:%02d,%02d,%X",gpsRec.RunDirect,gpsRec.stationNo,historyCheck.rDealType);
		display(2,0,btff,0);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		break;
	case ID_REC_GPS_JF:
	case ID_REC_GPS_OZ:
	case ID_REC_GPS_OUT:
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		sprintf(btff,"��վ:%02d,%02d,%X",gpsRec.RunDirect,gpsRec.stationNo,historyCheck.rDealType);
		display(2,0,btff,0);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		break;
	case ID_REC_GPS_ST:
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		strcpy(btff,"��վ��¼:");
		display(2,0,btff,0);
		sprintf(btff,"->%02d,%02d,%X",gpsRec.RunDirect,gpsRec.stationNo,historyCheck.rDealType);
		display(4,0,btff,0);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		break;
	case ID_REC_GPS_YUE://Խվ��¼
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		strcpy(btff,"Խվ��¼:");
		display(2,0,btff,0);
		sprintf(btff,"->%02d,%02d,%02dkm,%X",gpsRec.RunDirect,gpsRec.stationNo,gpsRec.MaxSpeed/100,historyCheck.rDealType);
		display(4,0,btff,0);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		break;
	case ID_REC_OverSpeed://���ټ�¼
		cls();
		memcpy((unsigned char*)&gpsRec,(unsigned char*)&historyCheck,64);
		memcpy(historyCheck.rPublishNumb,gpsRec.ulCardNo,4);
		strcpy(btff,"���ټ�¼:");
		display(2,0,btff,0);
		//����վ�㣬�ٶ�
		sprintf(btff,"->%02d,%02d,%02dkm,%X",gpsRec.RunDirect,gpsRec.stationNo,gpsRec.MaxSpeed/100,historyCheck.rDealType);
		display(4,0,btff,0);
		memcpy(historyCheck.rDealTime,gpsRec.DealTime,7);
		dis_time_mini(6, (stcomtime*)gpsRec.STARTDealTime);
		break;
	default:
		return ST_ERROR;
	}
#ifdef _debug_

	memset(buffer,0,30);
	sprintf((char*)buffer, "%d ", Index+1);
	miniDispstr(6, 37, (char*)buffer, 0);
#endif

	MSG_LOG("kh:%d\r\n",historyCheck.rCardCand);
	BCD_LOG(historyCheck.rUnionCardNum,10,1);

	temp=0;
	if((historyCheck.rCardCand==CARDSTYLE_JTB)||(historyCheck.rCardCand==CARDSTYLE_CPU) || (historyCheck.rCardCand==CARDSTYLE_24CPU))//��ͨ���� ���߱���
	{
// 		memcpy(buffer,historyCheck.rProVer,6);
// 		memcpy(buffer+6,historyCheck.rPublishNumb,4);
 		memset((unsigned char*)disbuf,0,22);
// 		memcpy(disbuf,"����:",5);
		strcpy((char*)disbuf, _Font_showCardNo);
		BCD2Ascii(historyCheck.rUnionCardNum+2,  (unsigned char *)(disbuf + 5), 8);
		display(0,0,disbuf,0);

		dis_time_mini(6, (stcomtime*)&historyCheck.rDealTime);

	}
#ifdef QR_CODE_COM
	else if (historyCheck.rDealType == ID_REC_QRC_SLZR)	//��ά��
	{	
		memset((unsigned char*)disbuf,0,22);
		// 		memcpy(disbuf,"����:",5);
		strcpy((char*)disbuf, _Font_showCardNo);
		BCD2Ascii(rRecordDeal->rPublishNumb,  (unsigned char *)(disbuf + 5), 10);
		
		display(0,0,disbuf,0);

		dis_time_mini(6, (stcomtime*)rRecordDeal->rDealTime);


	}
#endif

	else
	{
		memset((unsigned char*)buffer,0,sizeof(buffer));
		MSG_LOG("M1:longlong%d\r\n",sizeof(unsigned long long));
		ultemp = 0;
		memcpy((unsigned char*)&ultemp, historyCheck.rUnionCardNum, 5);

		MSG_LOG("ultemp:%10llu\r\n",ultemp);
		strcpy((char*)buffer, _Font_showCardNo);
		sprintf((char*)(buffer+5),"%10llu", ultemp);
		display(0,0,(char*)buffer,0);
		dis_time_mini(6, (stcomtime*)&historyCheck.rDealTime);

	}
#ifdef _debug_
	if(historyCheck.rDealType==ID_REC_History)
		miniDispstr(6,10,"History",DIS_CENTER);
#endif
	display(6, 0,"ʱ��:",0);
// 	sprintf(disbuf, "����:%02X%02X��%02X��%02X��", historyCheck.rDealTime[0],historyCheck.rDealTime[1],historyCheck.rDealTime[2],historyCheck.rDealTime[3]);
// 	display(6, 0, disbuf, 0);//, (stcomtime*)historyCheck.rDealTime);
// 	sprintf(disbuf, "ʱ��:%02Xʱ%02X��%02X��", historyCheck.rDealTime[4],historyCheck.rDealTime[5],historyCheck.rDealTime[6]);
// 	display(8, 0, disbuf, 0);

	return ST_OK;
}

/*		�����ʷ��¼  mode 1 ���� 0 ����; pos ��ǰ��¼��  alllength ȫ����¼����     */
unsigned char check_history(char mode,unsigned int *pos,unsigned int alllength)
{
	unsigned char temp;
	unsigned int ipos;
	
	ipos=*pos;
	while (1)
	{
		clr_wdt();
		temp=Disp_history(mode, ipos);
		if(temp==ST_ERROR)
		{
			if(mode==1)
			{
				ipos++;
				if(ipos>=alllength)
				{
					return ST_ERROR;
				}
			}
			else if (mode==0)
			{
				if(ipos==0)
				{
					return ST_ERROR;
				}
				ipos--;
			}
		}
		else if(temp == 0xee)
			return temp;
#ifdef qPBOC_BUS
		else if(temp == 0xEA){//���п�
			if(mode == 1){
				ipos += 1;
				*pos=ipos;
			}
			else{
				if(ipos > 1)
					ipos -= 1;
				else
					ipos = 0;
				*pos=ipos;
			}
			return ST_OK;
		}
#endif
		else
		{
			*pos=ipos;
			return ST_OK;
		}
	}
}

void ClearBlkList(void)
{
	unsigned char i;
	cls();
	display(0,0,"��ȫ���������",DIS_CENTER|DIS_CONVERT);
	display(2, 0, "ȷ����?",DIS_CENTER);

	display(6, 0, "ȡ��        ȷ��", 0);
	while(1){
		i = getkey(1);
		if(i == KEY_ENT)
			break;
		else if(i == KEY_ESC)
			return;
		delayxms(50);
	}
	
#ifndef NO_GJ_SERVISE
	memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
	sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
	delallBlacInbus();
#endif
	memset((unsigned char*)&gBusblkDownPro, 0, sizeof(gBusDownPro));
	display(4, 0, "   ������!!     ", 0);
	display(6, 0, "   ������!    ", 0);
	beep(1,10,10);
	delayxs(1);
	ReStartCPU();
	return;
}
#ifdef BUS_Cloud_
void loadingKey(void)
{
	int outdly=0;
	unsigned char M1Buff[3][16];
	unsigned char buff[64];
	unsigned char i,block;
	unsigned int crc32;
	
	cls();
	display(0,0,Card_LoadKey,DIS_CENTER);
	display(3,0,Card_SWKeyCard,DIS_CENTER);
	while(1){
		if(outdly++ > 100)
			return;
		if(profile(0) != ST_ERROR)
			break;
		if((outdly%10)==0)
			InitRC531();
		delayxms(50);
		if(getkey(1) == KEY_ESC)
			return;
	}
	
	display(3,0,Card_getCardinfo,DIS_CENTER);
	outdly = Card_typeProcess();//ȡ����Կ
	if(outdly != ST_OK)
		return;
	if(cardMessage.card_catalog != CARD_KEY_BUS){
		MessageBox(1, Card_CardtypeE);
		delayxs(2);
		return;
	}

	
	if(cardMessage.dealRecordSector[0] > 15){
		cardMessage.dealRecordSector[0] = 3;
	}
	block = cardMessage.dealRecordSector[0]*4;
	
	if(MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA, block)==0){
		MSG_LOG("initKEY MifareAuthKey1 error: 12");
		MSG_LOG("||");
		BCD_LOG(cardMessage.RecodeKEYA, 6, 1);
		MessageBox(1, Card_AuthenticateErr);
		delayxs(2);
		return;
	}
	for(i=0; i<3; i++){
		if(MifareRead((block+i),M1Buff[i]) ==0){
			MessageBox(1, Conn(Card_getCardinfo, "����!"));
			delayxs(2);
			return;
		}
	}

	MSG_LOG("KEY:\r\n");
	BCD_LOG(M1Buff[0], 16, 1);
	BCD_LOG(M1Buff[1], 16, 1);
	BCD_LOG(M1Buff[2], 16, 1);

	crc32 = Getcrc16(M1Buff[0], 16, 0xFF, 0xFF);

	MSG_LOG("KEYC1:\r\n");
	BCD_LOG((unsigned char*)&crc32, 4, 1);

	if(memcmp((unsigned char*)&crc32, M1Buff[2], 4) != 0){
		MessageBox(1, Conn(Card_getCardinfo, "2����!"));
		delayxs(2);
		return;
	}
	crc32 = Getcrc16(M1Buff[1], 16, 0xFF, 0xFF);

	MSG_LOG("KEYC2:\r\n");
	BCD_LOG((unsigned char*)&crc32, 4, 1);

	if(memcmp((unsigned char*)&crc32, M1Buff[2]+8, 4) != 0){
		MessageBox(1, Conn(Card_getCardinfo, "3����!"));
		BCD_LOG((unsigned char*)&crc32, 4, 1);
		delayxs(2);
		return;
	}
	memcpy(buff, M1Buff[0], 16);
	memcpy(buff+16, M1Buff[1], 16);
	saveCpuInfo(6, buff);
	
	MifareHalt();
	display(3,0,Card_LoadKEyOK,DIS_CENTER);
	beep(1, 100, 100);
	delayxs(2);
}
#endif

#ifdef NO_GJ_SERVISE
void SetSevice(void)
{
	return;
}
#else
extern void SetSevice(void);
#endif
extern void setMobilesv(void);
extern void DownKEK(void);
#ifdef BUS_SYBAOSHI_
extern void SetWaitTime(void);
#endif
#ifdef WIFI_TLH_
extern void setWiFIKEK(void);
extern void setWiFISSID(void);
#endif
//�˵����ã�����ÿ�ζ�����
#ifdef QR_CODE_COM
	extern void set_QR_para(void);
#endif
void usb_main(void)
{
	main_usb_collect();
}


//������·��
void set_LineNo_Menu(void)
{
	unsigned char i;
	unsigned char buffer[20];
//	unsigned tempbuff[2];
	//	unsigned char disbufer[20];
	unsigned char ret,dispflag=0;
	unsigned int outdly=0;
	
	
	i=0;
	
	cls();
	display(0,0,"������·��",0);

	
	dispflag = 1;
	memset(buffer,0,sizeof(buffer));
	BCD2Ascii(pFistVary.LineNo,buffer,3);
	display(3, 7, (const char *)buffer, 0);


	while(1)
	{
		delayxms(50);
		
		if(dispflag){
			dispflag = 0;
			display(3, 7, (const char *)buffer, 0);
			buffer[8] = buffer[i];
			display(3, 7+i, (const char *)(buffer+8), 1);			
		}
		ret =getkey(1);

		if(ret == 0){
			if(outdly++ > 400)
				return;
			continue;
		}

		outdly =0;


		switch(ret)
		{
#ifndef _New_Bu_mode_
		case UP:
			if((buffer[i]<='0')||(buffer[i]>'9'))
				buffer[i]='9';
			else if((buffer[i]<='9')&&(buffer[i]>'0'))
				buffer[i]--;
			dispflag = 1;
			break;
		case F2:  
			i++;
			if(i >= 2)
				i = 0;
			dispflag = 1;
			break;
		case F1:
			if(i>0)
				i--;
			else
				i = 1;
			dispflag = 1;
			break;
		case DOWN: 
#else
		case UP:
#endif  
			if((buffer[i]<'0')||(buffer[i]>='9'))
				buffer[i]='0';
			else if((buffer[i]<'9')&&(buffer[i]>='0'))
				buffer[i]++;
			dispflag = 1;
			break;
		case ESC:
			return;
		case ENTER:
			dispflag = 1;
#ifdef _New_Bu_mode_
			i++;
#else
			i = 2;
#endif
			
			if(i >= 6){
				display(5,5,"�������!",0);
				//				memset(buffer,0,10);
				Ascii2BCD(buffer,pFistVary.LineNo,6);
				
				sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
				sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
				
				savesysbupara();
					gsl8583Style.ISOK=0;
				memcpy(NewPriceTab.lineVer, "\x00\x00", 2);
				saveCpuInfo(1,buffer);
				return ;
			}
			break;

		default:
			break;
		
		}
	}


}
void set_bussinessNO(void)
{
	unsigned char i;
	unsigned char buffer[20];
	unsigned char ret;
	unsigned char buff_buss[2];
	unsigned char buff_asc[4];
	unsigned char buff_asc_1[4];
	i=0;
	SYSgetbussinessNO(buff_buss);
	BCD2Ascii(buff_buss,buff_asc,2);
	cls();
	if(gchn_eng == 'E')
		display(0,2,"set device NO.",DIS_CENTER);
	else
		display(0,2,"�����̻���",DIS_CENTER);
	memset(buffer,0,10);
	memcpy(buff_asc_1,buff_asc,4);
	
	memcpy(buffer,buff_asc,4);
	
	display(2,4,(char*)buffer,0);
	while(1)
	{
		delayxms(50);
		memset(buffer,0,10);
		buffer[0]=buff_asc[i];
		display(2,4+i,(char*)buffer,DIS_CONVERT);
		ret =getkey(1);
		switch(ret)
		{
#ifndef _New_Bu_mode_
		case UP:
			if((buff_asc[i]<='0')||(buff_asc[i]>'9'))
				buff_asc[i]='9';
			else if((buff_asc[i]<='9')&&(buff_asc[i]>'0'))
				buff_asc[i]--;
			
			break;
		case F2:  
			i++;
			if(i==4)
				i = 0;
			memset(buffer,0,10);
			memcpy(buffer,buff_asc,4);
			display(2,4,(char*)buffer,0);
			break;
		case F1:
			if(i>0)
				i--;
			else
				i = 3;
			memset(buffer,0,10);
			memcpy(buffer,buff_asc,4);
			display(2,4,(char*)buffer,0);
			break;
		case DOWN: 
#else
		case UP:
#endif  
			if((buff_asc[i]<'0')||(buff_asc[i]>='9'))
				buff_asc[i]='0';
			else if((buff_asc[i]<'9')&&(buff_asc[i]>='0'))
				buff_asc[i]++;
			break;
		case ESC:
			memcpy(buff_asc, buff_asc_1, 4);
			goto SetupDevi_exit;
		case ENTER:
#ifdef _New_Bu_mode_
			i++;
			memset(buffer,0,10);
			memcpy(buffer,buff_asc,4);
			display(2,4,(char*)buffer,0);
#else
			i = 4;
#endif
			if(i >= 4){
				
				
				goto SetupDevi_exit;
			}
			break;
		default:
			break;
			
		}
	}
SetupDevi_exit:
	
	Ascii2BCD(buff_asc,buff_buss,4);
	saveCpuInfo(7, buff_buss);//�̻���  00009998
	gsl8583Style.ISOK = 0;	//��ǩ��
	
}

void ShowSetInfo(void)
{
	unsigned char ucMENU_NUM;
	char menu[12][20];
	void (*function[12])(void);
	
#ifdef MOBILE_SHIYAN_
	strcpy(menu[0], _Font_DTset);
	strcpy(menu[1], _Font_YDIPset);
	strcpy(menu[2], _Font_GJIPset);
	strcpy(menu[3], _Font_KEKDown);
	strcpy(menu[4], _Font_CLRBLK);
	function[0] = SYSsetdatetime;
	function[1] = setMobilesv;
	function[2] = SetSevice;
	function[3] = DownKEK;
	function[4] = ClearBlkList;
	ucMENU_NUM = 5;
#else
#ifdef NO_MOBILE_24G_
	ucMENU_NUM = 0;
	strcpy(menu[ucMENU_NUM], _Font_DTset);
	function[ucMENU_NUM++] = SYSsetdatetime;
//#ifndef BUS_HANDAN_
	strcpy(menu[ucMENU_NUM], _Font_SetDEV);
	function[ucMENU_NUM++] = set_device;
//#endif
#ifndef NO_GJ_SERVISE
#ifdef FengDuan_BU_
	if(gFDMode == FDMODE_GPS){	//�ֶ��շѣ����Ż�����Ҫ������
#endif
	strcpy(menu[ucMENU_NUM], _Font_GJIPset);
	function[ucMENU_NUM++] = SetSevice;
#ifdef FengDuan_BU_
	}
#endif

	function[ucMENU_NUM] = set_LineNo_Menu;
	strcpy(menu[ucMENU_NUM++], "������·��� ");

	strcpy(menu[ucMENU_NUM], "�����̻�    ");
	function[ucMENU_NUM++] = set_bussinessNO;

		strcpy(menu[ucMENU_NUM], "������������ ");
	function[ucMENU_NUM++] = set_pboc_menu;

	strcpy(menu[ucMENU_NUM], "¼���������� ");
	function[ucMENU_NUM++] = LOAD_qpoc;
#endif
#ifdef BUS_Cloud_
	function[ucMENU_NUM] = loadingKey;
	strcpy(menu[ucMENU_NUM++], _Font_Load_Key);
#elif defined BUS_SYBAOSHI_
	function[ucMENU_NUM] = SetWaitTime;
	strcpy(menu[ucMENU_NUM++], "ˢ���ӳ�ʱ�� ");
#else
	function[ucMENU_NUM] = ClearBlkList;
	strcpy(menu[ucMENU_NUM++], _Font_CLRBLK);
#endif
	function[ucMENU_NUM] = usb_main;
	strcpy(menu[ucMENU_NUM++], "U�̲���      ");

#ifdef QR_CODE_COM
// 	function[ucMENU_NUM] = set_QR_para;
// 	strcpy(menu[ucMENU_NUM++], "��ʼ����ά��");
#endif

#ifdef WIFI_TLH_
	function[ucMENU_NUM] = setWiFISSID;
	strcpy(menu[ucMENU_NUM++], _Font_WIFI_SSID);
	function[ucMENU_NUM] = setWiFIKEK;
	strcpy(menu[ucMENU_NUM++], _Font_WIFI_PSW);
#endif
#ifdef _Counter_SWcard_times
	function[ucMENU_NUM] = setSCtimesMax;
	strcpy(menu[ucMENU_NUM++], "���������� ");
#endif
#ifdef FengDuan_BU_
	function[ucMENU_NUM] = SETCpuFDMode;
	strcpy(menu[ucMENU_NUM++], DeviceType);
#endif
 	function[ucMENU_NUM] = ResetPos;
 	strcpy(menu[ucMENU_NUM++], _Font_ResetPos);
#else
	strcpy(menu[0], "����ʱ��     ");
	strcpy(menu[1], "�����豸��   ");
	strcpy(menu[2], "�ƶ�IP����   ");
	strcpy(menu[3], "����������   ");
	strcpy(menu[4], "KEK����      ");
	strcpy(menu[5], "���ú�����   ");
	strcpy(menu[6], "�ָ�����״̬ ");
	function[0] = SYSsetdatetime;
	function[1] = set_device;
	function[2] = setMobilesv;
	function[3] = SetSevice;
	function[4] = DownKEK;
	function[5] = ClearBlkList;
	function[6] = ResetPos;
	ucMENU_NUM = 7;
#endif
#endif
	
	MenuFrame(menu, function, _Font_MainSet, ucMENU_NUM, (100));
}

int getpassword(void)//up down up ent
{
	unsigned char i=0;
	unsigned char ret;
	unsigned char disbuf[20];
#if ((defined _debug_)||(!defined PASSWORD_CARD_))
	unsigned char check[9];
#ifdef _debug_
	debugstring("����������----\r\n");
	return 1;
#endif
	
	memset(check, 0, 9);
	memset(disbuf, 0, 9);
	
	display(2, 0, _Font_InputCODE, DIS_CENTER|DIS_Cls);
	while(1){
		delayxms(20);
		ret = getkey(1);
		if(ret == 0){
			continue;
		}
		switch (ret)
		{
#ifndef _New_Bu_mode_
		case F1:
			ret = '1';
			break;
		case F2:
			ret = '2';
			break;
		case UP:
			ret = '3';
			break;
		case DOWN:
#else
		case UP:
#endif
			ret = '4';
			break;
		case ESC:
			ret = '5';
			break;
		case ENTER:
			ret = '6';
			break;
		default:
			break;
		}
		check[i] = ret;
		i++;
		memset(disbuf, '*', i);
		display(4, 0, (char*)disbuf, DIS_CENTER);
		if(i >= 4){
#ifdef _New_Bu_mode_
			if(memcmp(check, "4465",4)==0)//D D Ent CSE
				return 1;
			else if(memcmp(check, "5464", 4) == 0)//C D E D
				return 2;
			else if(memcmp(check, "5465", 4) == 0)//C D E C
				return 3;
#else
			if(memcmp(check, "3436",4)==0)//������OK
				return 1;
			else if(memcmp(check, "1256", 4) == 0)//F1 F2 ESC ENT
				return 2;
#endif
			else
				goto inerror;
		}
	}
inerror:
	
	display(2, 0, _Font_Inputerror, DIS_CENTER);
	delayxs(1);
	return 0;
#else
	cls();
	display(0,0,_Font_MainSet,DIS_CENTER|DIS_CONVERT);
	display(3,0, Conn(_Font_swipeCard, "..."),DIS_CENTER);
	display(5,0, " 20 ", DIS_CENTER);
	i = 0;
	while(1){
		if(getkey(1) == KEY_ESC)
			return 0;
		delayxms(50);
		i++;
		if(i >= 200)
			return 0;
		if(i % 10 == 0){
			sprintf((char*)disbuf, " %d ", 20 - i/10);
			display(5,0, (char*)disbuf, DIS_CENTER);
		}
		if (profile(0) == ST_ERROR)
			continue;
		ret = Card_typeProcess_nomal(0xA1);
		if(ret == CONTROL_CARD)
			break;
	}
	if((cardMessage.card_catalog == CARD_LINE_SET)||(cardMessage.card_catalog == CARD_CLERK_BUS)||
		(cardMessage.card_catalog == CARD_LEV_BUS)||(cardMessage.card_catalog == CARD_ARV_BUS)||
		(cardMessage.card_catalog == CARD_DEV_SET))
	{
		beep(1, 100, 100);
		return 1;
	}
	else 
		return 0;
#endif //#ifdef _debug_
}

//�汾�ţ��ļ�����ʱ��7+0000+�汾2 �ϰ汾��û������ʱ��
//���к�4+�豸��4+����汾��12+�������汾12+GPS�ļ��汾12+�����ļ��汾12+��·��3+1����
void GetBuIfor(unsigned char *buinfo)
{
	unsigned int index=0;
	
	Get_SerialNumF4(buinfo+index);
	index += 4;//���к�
	Ascii2BCD(pFistVary.DeviceNo, buinfo+index, 8);
	index += 4;//�豸��
	
	getProVer();
	memset(buinfo+index, 0, 12);
	memcpy(buinfo+index+10, gBusVerInfo.busProVer, 2);//����汾
//	over_turn(2, buinfo+index+10);
	index += 12;

	memset(buinfo+index, 0, 12);
	if(memcmp(gBusVerInfo.busBLKVer, "\xff\xff", 2) == 0)
		memset(gBusVerInfo.busBLKVer, 0, 2);
	memcpy(buinfo+index+10, gBusVerInfo.busBLKVer, 2);//�������汾2B
//	over_turn(2, buinfo+index+10);
	index += 12;
	
	memset(buinfo+index, 0, 12);
	memcpy(buinfo+index+10, gBusVerInfo.busLineVer, 2);//��λ������Ϣ�汾2B
//	over_turn(2, buinfo+index+10);
	index += 12;
	
	memset(buinfo+index, 0, 12);
	memcpy(buinfo+index+10, gBusVerInfo.busVoiceVer, 2);//��Կ��Ϣ�汾2B
//	over_turn(2, buinfo+index+10);
	index += 12;
	memcpy(buinfo+index, pFistVary.LineNo, 3);
	index += 3;
	buinfo[index++] = 0;
}


#ifdef FengDuan_BU_
/*
void debugFDinfo(void)//n(n-1)/2
{
	unsigned int Cindex, windex, wsaddr, useFDs;
	unsigned short ustmp, inexx, stion;
	char disbuf[16];
	
//	unsigned char buff[130];
	inexx = 0;
	stion = BCD2int((unsigned char*)&gFdLinehead.StationNum, 1);
	debugstring("-----�ֶ���Ϣ--------\r\n");
	debugdata(gFdLinehead.LineNo,sizeof(gFdLinehead),1);
	debugdata((unsigned char*)&stion,2,1);
	for(Cindex=0; Cindex<stion; Cindex++){//10�������
		
		delayxms(1);
		debugstring("cindex:");
		debugdata((unsigned char*)&Cindex,1,1);
		for(windex=0; windex<stion; windex++){//10�������
			debugstring("windex:");
			debugdata((unsigned char*)&windex,1,1);
			wsaddr = FLASH_FD_Ticket_START+32;
			
 			if(Cindex <=windex)
				break;
			useFDs = Cindex*(Cindex-1);
			useFDs /= 2;
			useFDs += windex;
			wsaddr += (useFDs*2);
			if(Cindex==1)
			{
				flashread(FLASH_FD_Ticket_START+16, (unsigned char*)disbuf,16);
			//	debugstring(LineInfo[0].StationInfo[0].name);
		//	debugdata(disbuf+14,1,1);//�𲽼�
			//	debugstring("\r\n");
				
			}
			flashread(wsaddr, (unsigned char*)&ustmp, 2);
			//sprintf((char*)disbuf, "%02d-%02d:%d:||", windex, Cindex, ustmp/100);
			//debugstring((char*)disbuf);
			
			inexx++;
			if((inexx%Cindex)==0)
			{
			
				debugstring(LineInfo[0].StationInfo[inexx].name);
				debugstring("\r\n");
			
				
				inexx=0;
			}
			
			
		}
	}
	debugstring("����\r\n");
	debugdata((unsigned char*)&Cindex,1,1);
	debugdata((unsigned char*)&windex,1,1);
	debugdata((unsigned char*)&stion,1,1);
}*/
void debugFDinfo(void)//n(n-1)/2
{
	unsigned int Cindex, windex, wsaddr, useFDs;
	unsigned short ustmp, inexx, stion;
	char disbuf[16];
	
	//	unsigned char buff[130];
	inexx = 0;
	stion = BCD2int((unsigned char*)&gFdLinehead.StationNum, 1);
	debugstring("-----�ֶ���Ϣ--------\r\n");
	debugdata(gFdLinehead.LineNo,sizeof(gFdLinehead),1);
	debugdata((unsigned char*)&stion,2,1);
	for(Cindex=0; Cindex<stion; Cindex++){//10�������
		clr_wdt();	
		for(windex=0; windex<stion; windex++){//10�������
			
			wsaddr = FLASH_FD_Ticket_START+32;
			
			if(Cindex <=windex)
				break;
			useFDs = Cindex*(Cindex-1);
			useFDs /= 2;
			useFDs += windex;
			wsaddr += (useFDs*2);
			if(Cindex==1)
			{
				debugstring(LineInfo[0].StationInfo[0].name);
				debugstring("\r\n");
			}
			flashread(wsaddr, (unsigned char*)&ustmp, 2);
			sprintf((char*)disbuf, "%02d-%02d:%d:%x||", Cindex, windex, ustmp/100,wsaddr);
			debugstring((char*)disbuf);
			
			inexx++;
			if((inexx%Cindex)==0)
			{
				debugstring(LineInfo[0].StationInfo[inexx].name);
				debugstring("\r\n");
				
				inexx=0;
			}
			
			
		}
	}
}
#endif
void debugBusinfo(void)
{
	char buff[128];
	unsigned int i, t;
	unsigned int add,addr;
	unsigned char k;
	unsigned char buffer[10];
	
	cls();
	display(0,0,_Font_Rs232Debug,DIS_CENTER|DIS_CONVERT);
	display(2,0,_Font_HitDebuginfo,0);

#ifdef qPBOC_BUS
	debugstring("PKI�ڹ�Կ��Ϣ:\r\n");
	checkSamKey(1); //Qpboc�������ӹ�Կ��PSAM���������Ƭ�е�ֵ������
	debugstring("\r\n----------------\r\n");
#endif
	i = Get_Record_point((unsigned char *)&t, 0);
	debugstring("ͷָ��:");
	debugdata((unsigned char*)&t, 4, 1);
	debugstring("��ǰָ��:");
	debugdata((unsigned char*)&i, 4, 1);
	debugstring("��·��Ϣ:");
	memcpy((unsigned char*)&add, NewPriceTab.busPrice, 4);
	sprintf(buff, "Ʊ��:%d \r\n", add);
	debugstring(buff);
	memcpy((unsigned char*)&add, NewPriceTab.plusPrice, 4);
	sprintf(buff, "����Ʊ��:%d \r\n", add);
	debugstring(buff);
	if(F_FLAG_VARY.PlusSta == 1)
		debugstring("�������ѿ���!\r\n");
	else
		debugstring("�������ѹر�!\r\n");

	for(i=0;i<32;i++){
		sprintf(buff, "����:%d �ۿ�:%d ����:%d ����:%d\r\n", (i+0x40), NewPriceTab.rate[i], NewPriceTab.plusrate[i], NewPriceTab.voice[i]);
		debugstring(buff);
	}
	
#ifdef _GPS_ENABLE_
	memset(buff, 0, 20);
	debugstring("��·վ����Ϣ:\r\n�汾��:");
	memcpy(buff, gBusVerInfo.busLineVer, 2);
	debugstring(buff);
	sprintf(buff, "\r\n����վ��:%d ����:\r\n", LineInfo[0].ucStationMax);
	debugstring(buff);
	if(LineInfo[0].ucStationMax <= STATION_MAX_COUNT){
		for(i=0; i<LineInfo[0].ucStationMax; i++){
			clr_wdt();
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ucId, 1, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLatitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLongitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiangIN, 4, 0);
			debugstring("||");
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLatitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLongitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiang, 4, 0);
			debugstring("||");
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLatitudeOUT, 4, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLongitudeOUT, 4, 0);
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiangOUT, 4, 0);
			debugstring(LineInfo[0].StationInfo[i].name);
			debugstring("->");
			debugdata((unsigned char*)&LineInfo[0].StationInfo[i].Sstyle, 1, 1);
		}
		sprintf(buff, "����վ��:%d ����:\r\n", LineInfo[1].ucStationMax);
		debugstring(buff);
		for(i=0; i<LineInfo[1].ucStationMax; i++){
			clr_wdt();
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ucId, 1, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLatitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLongitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiangIN, 4, 0);
			debugstring("||");
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLatitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLongitude, 4, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiang, 4, 0);
			debugstring("||");
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLatitudeOUT, 4, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLongitudeOUT, 4, 0);
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiangOUT, 4, 0);
			debugstring(LineInfo[1].StationInfo[i].name);
			debugstring("->");
			debugdata((unsigned char*)&LineInfo[1].StationInfo[i].Sstyle, 1, 1);
		}
	}
	if(gGPsPar.EnableGps == 0)
		debugstring("GPS��ֹ ��·վ����Ϣ����!\r\n");
	else
		debugstring("GPS����!\r\n");
#endif
#ifndef NO_MOBILE_24G_
	debugstring("�ƶ�ͷָ��:");
	debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
	debugstring("�ƶ���ǰָ��:");
	debugdata((unsigned char*)&gMobilREC_End, 4, 1);
#endif
	debugstring("�������汾��:");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	sprintf(buff, "����������:%d \r\n", pFistVary.BlackListNum);
	debugstring(buff);
// 	for(t=0; t< 16; t++){
// 		sprintf(buff, "��:%d|%d \r\n", t, gBlackBlockNum[t]);
// 		debugstring(buff);
// 		if(t==0)
// 			add = FLASH_BLK_START+14+(t*4096);
// 		else
// 			add = FLASH_BLK_START+(t*4096);
// 		for(i=0; i<gBlackBlockNum[t]; i++){
// 			flashread(add, (unsigned char*)buff, BLK_SNO_LEN);
// 			debugdata((unsigned char*)buff, BLK_SNO_LEN, 0);
// 			debugstring("||");
// 			add += 4;
// 			sprintf(buff, " i:%d ",i);
// 			display(6,0,buff,DIS_CENTER);
// 			clr_wdt();
// 		}
// 	}
	
	
		sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //������һ�������������������
		debugstring("������\r\n");
		debugdata((unsigned char*)&pFistVary.BlackListNum,4,1);
		i=0;
		while(1)
		{
			for(k=0;k<16;k++)
			{
			if (k==0)
			{
				addr=FLASH_BLK_START+14;
			}else{
				addr=k*4096+FLASH_BLK_START;
			}
			debugstring("\r\n");
			debugstring("K=");
			debugdata((unsigned char*)&k,1,1);
			debugdata((unsigned char*)&gBlackBlockNum[0],32,1);
			debugdata((unsigned char*)&gBlackBlockNum[k],2,1);
			for (i=1;i<=gBlackBlockNum[k];i++)
			{
				debugdata((unsigned char*)&i,2,0);
				
				clr_wdt();
				
				flashread(addr, buffer, BLK_SNO_LEN);
				addr=(addr+BLK_SNO_LEN);
				debugstring("||");
				debugdata(buffer,BLK_SNO_LEN,0);
				debugstring(",");
				sprintf(buff, " i:%d ",i);
				
			}
			clr_wdt();
			
		}
		break;
		}
		
	debugHisAddr();
#ifdef FengDuan_BU_
	debugFDinfo();
#endif
#ifdef _TTSSound_enable_
	debugADsoundInfo();
#endif

	display(0,0,_Font_Complete,DIS_CENTER|DIS_CONVERT);
	delayxs(1);
}
void debug_voice(void)
{
	
	char disbuf[20];
	unsigned char ret,cout=1;
	unsigned char  disFlag=0;
	cls();
	display(0,0,"��������",DIS_CENTER);
	disFlag=1;
	cout=0;
	while(1){
		ret=getkey(1);
		delayxms(100);
		if(ret==UP||ret==DOWN){
			if (cout>0)
				cout--;
			disFlag=1;
		}	
		if(ret==ENTER){
			if(cout<31)
				cout++;
			else
				cout=0;
			disFlag=1;
		}
		if(ret==ESC)
			break;
		if(disFlag){
#ifndef Card_typeM_Min_is0_
			if(cout==0)//��˾�Ŀ�û�п���0��
				cout=1;
#endif
			cls();
			display(0,0,"��������",DIS_CENTER);
			delayxms(200);
			getCardtype(disbuf,cout);
			cardMessage.card_catalog=cout;
			SoundMessage(SOUND_DEAL);
#ifndef Card_typeM_Min_is0_
			
			sprintf(disbuf+strlen(disbuf), "->����:%d",cout+0x40);
#else
			sprintf(disbuf+strlen(disbuf), "->����:%d",cout);
#endif
			display(3,0,disbuf,DIS_CENTER);
			delayxms(600);
			disFlag=0;
			
		}
		
		
	}
}
void set_main(void)
{
	unsigned char ret;
	ret = getpassword();
	if(ret == 2){
		debugBusinfo();
		return;
	}
	else if(ret==3){//��������
		debug_voice();
		return;
	}
	else if(ret != 1)
		return;
	ShowSetInfo();
}
void showGPRStR(void)
{
#ifndef NO_MOBILE_24G_
	unsigned int ltemp1;
	unsigned int sendenda;
#endif
	unsigned int ltemp;
	int headp, curp;

	char buffer[50];
	cls();
	curp = Get_Record_point((unsigned char*)&headp, 0);
	ltemp=0;
	ltemp=curp - headp;
	ltemp/=RECORD_LEN;
	sprintf(buffer,_Font_ICcard,ltemp);
// #ifndef NO_GJ_SERVISE
// 	if(tcpipPacket.packetStatus==RECORD_DEAL)
// 	{
// 		strcat(buffer, "|1");
// 	}
// 	else
// 	{
// 		strcat(buffer, "|0");
// 	}
// #else
// 	strcat(buffer, "|0");
// #endif	
	display(0,0,buffer,0);
#ifndef NO_MOBILE_24G_
	if(memcmp(mobilsendStyle.Single, "���ܴ���", 8) == 0){
		sendenda = mobilsendStyle.EndAddr;
	}
	else{
		sendenda = gMobilREC_End;//��Ҫ���㵽����Ϊֹ
	}
	ltemp = (sendenda+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
	ltemp /= MOBILE_REC_LEN;
	if(gchn_eng == 'E')
		sprintf(buffer,"mobile:%d",ltemp);
	else
		sprintf(buffer,"�ֻ���:%d",ltemp);
	display(2,0,buffer,0);
	 if((memcmp(mobilsendStyle.Single, "���ڷ���", 8) == 0)||
		 (memcmp(mobilsendStyle.Single, "���ܴ���", 8) == 0)){
		 ltemp1 = (sendenda+LEN_M_REC-mobilsendStyle.SendAddr)%LEN_M_REC;
		 ltemp1 /= MOBILE_REC_LEN;
	 }
	 else
		 ltemp1 = ltemp;
	 if(gchn_eng == 'E')
		sprintf(buffer,"upload:%d",(ltemp-ltemp1));
	else
		sprintf(buffer,"���ϴ�:%d",(ltemp-ltemp1));
	display(4,0,buffer,0);
	memset(buffer, 0, 20);
	if(gchn_eng == 'E'){
		memcpy(buffer, "batch No.:", 10);
		memcpy(buffer+10, gMobileParameter.posPiCiNo, 6);
	}
	else{
		memcpy(buffer, "���κ�:", 7);
		memcpy(buffer+7, gMobileParameter.posPiCiNo, 6);
	}
	display(6,0,buffer,0);
#endif
	
// #ifdef qPBOC_BUS
// 	curp = qPbocGet_Record_point((unsigned char*)&headp, 0);
// 	ltemp=0;
// 	ltemp=(curp - headp)/256;
// 	sprintf(buffer,"������:%d",ltemp);
// 	display(2,0,buffer,0);
// #endif

	while(getkey(1)==0)
	{
		if(profile(0)!=ST_ERROR)
			break;//2003.9.15
		delayxms(100);
	}
}
#ifdef _GPS_ENABLE_
void showGPSStation(void)
{
	char disbuf[20];
	unsigned char i=0;
	unsigned char derect=0;
	unsigned char keyret;

	cls();
	if(LineInfo[0].uiVer == 0){
		if(gchn_eng == 'E')
			display(3,0,"no line GPS info",DIS_CENTER);
		else
			display(3,0,"����·վ����Ϣ",DIS_CENTER);
		delayxs(3);
		return;
	}
	sprintf(disbuf, "L:%02d%02d Ver:%d ", LineInfo[derect].lineNo[1], LineInfo[derect].lineNo[2], LineInfo[derect].uiVer);
	if(derect == 0)
		strcat(disbuf, "+");
	else
		strcat(disbuf, "-");
	display(0,0,disbuf, DIS_CENTER);
	while(1){
		if(gchn_eng == 'E')
			sprintf(disbuf,"LONG.%d", LineInfo[derect].StationInfo[i].ulLongitude);
		else
			sprintf(disbuf,"����:%d", LineInfo[derect].StationInfo[i].ulLongitude);
		display(2,0,disbuf, 0);
		if(gchn_eng == 'E')
			sprintf(disbuf,"lat.:%d", LineInfo[derect].StationInfo[i].ulLatitude);
		else
			sprintf(disbuf,"γ��:%d", LineInfo[derect].StationInfo[i].ulLatitude);
		display(4,0,disbuf, 0);
		sprintf(disbuf, "%02d:", i);
		strcat(disbuf, LineInfo[derect].StationInfo[i].name);
		display(6,0,"                ",0);
		display(6,0,disbuf,0);
		while(1){
			keyret = getkey(1);
			if(keyret == KEY_ESC)
				return;
//			else if(keyret == KEY_DOWN){
//				i++;
//				if(i >= LineInfo[derect].ucStationMax)
//					i = 0;
//				break;
//			}
//			else if(keyret == KEY_UP){
//				if(i>0)
//					i--;
//				else
//					i = LineInfo[derect].ucStationMax-1;
//				break;
//			}
			else if(keyret == UP){
				derect = 0;
				i = 0;
				cls();
				sprintf(disbuf, "L:%02d%02d Ver:%d ", LineInfo[derect].lineNo[1], LineInfo[derect].lineNo[2], LineInfo[derect].uiVer);
				if(derect == 0)
					strcat(disbuf, "+");
				else
					strcat(disbuf, "-");
				display(0,0,disbuf, DIS_CENTER);
				break;
			}
			else if(keyret == DOWN){
				derect = 1;
				i = 0;
				cls();
				sprintf(disbuf, "L:%02d%02d Ver:%d ", LineInfo[derect].lineNo[1], LineInfo[derect].lineNo[2], LineInfo[derect].uiVer);
				if(derect == 0)
					strcat(disbuf, "+");
				else
					strcat(disbuf, "-");
				display(0,0,disbuf, DIS_CENTER);
				break;
			}
			delayxms(50);
		}
	}
	
}
void showvoiceVer(void)
{
	char disbuf[20];

	cls();
	if(gSoundInfo.uiVer == 0){
		if(gchn_eng == 'E')
			display(3,0,"no sound info",DIS_CENTER);
		else
			display(3,0,"��������Ϣ",DIS_CENTER);
		delayxs(3);
		return;
	}
	if(gchn_eng == 'E')
		sprintf(disbuf, "sound ver:%04d", gSoundInfo.uiVer);
	else
		sprintf(disbuf, "�����汾:%04d", gSoundInfo.uiVer);
	display(3, 0, disbuf, 0);
	while(1){
		if(getkey(1) == KEY_ESC)
			break;
		delayxms(50);
		if(profile(0) !=ST_ERROR)
			break;
	}
}
void show_gprs_info(void)
{
	unsigned char ucMENU_NUM;
	
	char menu[3][20];
	void (*function[3])(void);
	
	ucMENU_NUM = 3;
	strcpy(menu[0], "GPRS������Ϣ ");
	strcpy(menu[1], "GPSվ����Ϣ  ");
	strcpy(menu[2], "GPS������Ϣ  ");
	
	function[0] = showGPRStR;
	function[1] = showGPSStation;
	function[2] = showvoiceVer;
	MenuFrame(menu, function, "  --GPRS�˵�--  ", ucMENU_NUM, (100));
}
#endif //#ifdef _GPS_ENABLE_
void Showcard(void)
{
	unsigned int indexn;
	unsigned int index_his;
	unsigned int startadd;
	unsigned char ctemp;
	unsigned char ii;
	unsigned int outdly=0;
	int headp, curp;

	display(0,2,_Font_RECqurey,DIS_CENTER);

 	curp = Get_Record_point((unsigned char *)&headp, 0);
// 
 	indexn=curp - headp;
 	indexn/=RECORD_LEN;
	indexn = 250;
//	index_his=0;
	if(indexn==0)
	{
		cls();
		display(3,4, _Font_NOREC,DIS_CENTER);
		delayxs(1);
		return;
// 		if(indexn < 50)
// 			indexn = 50;
	}
	startadd=0;
	while (1)		//Ѱ�ҵ�һ�����Ѽ�¼
	{
		clr_wdt();
		cls();
		display(3,0,_Font_NOREC,DIS_CENTER);
		ctemp=Disp_history(1, startadd);
		if(ctemp==ST_ERROR)
		{
			startadd++;
			if(startadd>=indexn)
			{
				cls();
				display(3,4, _Font_NOREC,DIS_CENTER);
				delayxs(1);
				return;
			}
		}
		else if(ctemp == 0xee){
			cls();
			display(3,4, _Font_NOREC,DIS_CENTER);
			delayxs(1);
			return;
		}
#ifdef qPBOC_BUS
		else if(ctemp == 0xEA){
			startadd += 1;
			break;
		}
#endif
		else
			break;
	}
//	display(4,14, "��",0);

	index_his=startadd;
	while(1)
	{
		if(outdly++ > 1000)
			return;
		if(profile(0)!=ST_ERROR) return;//2003.9.15
		delayxms(10);
		ii=getkey(1);
		if(ii == 0)
			continue;

		MSG_LOG("index_his=%d,startadd=%d\r\n",index_his,startadd);
		
		outdly = 0;
		if(ii==ESC)
			return;
		else if(ii==DOWN)//2003.9.15
		{
			if(index_his>startadd)
				index_his--;
			if(check_history(0,&index_his,startadd)==ST_ERROR)
			{
				index_his++;
			}
		}
		else if(ii==UP)
		{
			if(index_his<indexn-1)
				index_his++;
			ctemp = check_history(1,&index_his,indexn);
			if((ctemp==ST_ERROR)||(ctemp==0xee))
			{
				index_his--;
				indexn=index_his+1;
			}
		}
		else if(ii==ENTER){
			if(index_his>0)
				index_his--;
			if(check_history(0,&index_his,startadd)==ST_ERROR)
			{
				index_his++;
			}
		}

// 		if(index_his==indexn-1)//���������������
// 			display(4,14,"��",0);
// 		else if(index_his==startadd)
// 			display(4,14,"��",0);
// 		else 
// 			display(4,14,"��",0);
	}
}

#if 0
void Get_SerialNumF4(unsigned char *ser)
{
	unsigned int Device_Serial0;//, Device_Serial1, Device_Serial2;
	unsigned char buff[20];
	unsigned char cmac[4];
	unsigned char kbuf[16];
	unsigned char radon[8];
	//	unsigned char i;
	
// 	Ascii2BCD("A0F4DA4E",ser,8);
// 
// 	return ;


	Device_Serial0 = *(unsigned int*)(0x1FFF7A10);      //12 Bytes Serial Number
	memcpy(buff, (unsigned char*)&Device_Serial0, 4);
	Device_Serial0 = *(unsigned int*)(0x1FFF7A14);      //12 Bytes Serial Number
	memcpy(buff+4, (unsigned char*)&Device_Serial0, 4);
	Device_Serial0 = *(unsigned int*)(0x1FFF7A18);      //12 Bytes Serial Number
	memcpy(buff+8, (unsigned char*)&Device_Serial0, 4);
	
	memcpy(buff+12, "\xA2\xB3\xC4\xD5", 4);
	
	memset(kbuf, 0x11, 16);
	memset(radon, 0x22, 8);
	CountMac(kbuf, radon, buff, 16, cmac);
	
	memcpy(ser, cmac, 4);
}
#endif
//ȡ�Զ������к�,ȡ��8λ
void Get_SerialNumF4_Self(unsigned char *oSn)
{
	unsigned char buf[32];
	getposSN(buf, 0);
	Ascii2BCD(buf+8, oSn, 8);
}

extern unsigned char GprsCardList[16];
void showBUNo(void)
{
	char disbuf[30][30];
	unsigned char buff[32];
	unsigned short kk;
	unsigned int i,index;
	unsigned char  uct;
#ifdef FengDuan_BU_
	unsigned int addr;
#endif
	index = 0;
	for(i=0; i<30; i++)
		memset(disbuf[i], 0,sizeof(disbuf[i]));

	strcpy(disbuf[index], Card_deviceNO);
	memcpy(disbuf[index]+strlen(disbuf[index]), pFistVary.DeviceNo, 8);
	index++;

	Get_SerialNumF4(buff);//Get_SerialNumF4_Self(buff);//
	strcpy(disbuf[index], Card_SerialNO);
	BCD2Ascii(buff, (unsigned char*)disbuf[index]+7, 4);
	
	index++;

// 	getposSN(buff,0);										//���к�
// 	memcpy(disbuf[index],buff,16);
// 	index++;

	memcpy((unsigned char*)&kk, gBusVerInfo.busBLKVer, 2);

	SYSgetbussinessNO((unsigned char*)buff);


	if(gchn_eng != 'E'){
	sprintf(disbuf[index++], "�̻�:%02X%02X", buff[0], buff[1]);
	sprintf(disbuf[index], "�������汾:%X ", kk);
	index++;

#ifdef Center_Ctrl_BU_ENABEL
	BUCtrl_Get_Flag();
	if((gBuCtrlFlag & BUCTRL_FLAG_qPBOC) != 0)
		sprintf(disbuf[index], "����������:%02X", gBuCtrlFlag);
	else
		sprintf(disbuf[index], "����������:%02X", gBuCtrlFlag);
	index++;
#endif
	}
	//˾������
	memcpy((unsigned char *)&i,DriveCardNo,4);
	sprintf(disbuf[index],"˾����:%08d",i);
		index++;
	strcpy(disbuf[index], _Font_Line_Str);
	sprintf(disbuf[index]+strlen(_Font_Line_Str), ":%02X%02X-%02X", pFistVary.LineNo[0], pFistVary.LineNo[1], pFistVary.LineNo[2]);
	index++;
	
	if(gchn_eng != 'E'){
	getbusticketVer();
	memcpy((unsigned char*)&kk, gBusTicketV.busticketVer, 2);
//	over_turn(2, (unsigned char*)&kk);
#ifdef FengDuan_BU_
	sprintf(disbuf[index], "��·�汾:%02d-%02X", kk,gFdLinehead.CXulieNo);
#else
	sprintf(disbuf[index], "��·�汾:%02x", kk);
#endif
	index++;
	}
#ifdef FengDuan_BU_
	addr = FLASH_FD_Ticket_START;
	addr+=16;	//��־
	flashread(addr, buff, 16);//16 ͷ
	MSG_LOG("get stationPrice:");
	BCD_LOG(buff, 16, 1);
	i = buff[14];	//�𲽼ۣ���Ϊ��λ��0.1
	i *= 10;
	strcpy(disbuf[index], StarPrice);
	sprintf(disbuf[index]+strlen(StarPrice), ":%d.%02d", i/100, i%100), 
		index++;
#endif
	memcpy((unsigned char*)&i, NewPriceTab.busPrice, 4);
	strcpy(disbuf[index], _Font_Fare_Str);
	sprintf(disbuf[index]+strlen(_Font_Fare_Str), ":%d.%02d", i/100, i%100);
	index++;
	
	if(gchn_eng != 'E'){

	for(kk=1; kk<31; kk++){
		
		cardMessage.Zicard_catalog =0;
		if(getCardtype(disbuf[index], kk+0x40) == ST_ERROR){
			continue;
		}
#ifdef AutoAudio_line
		uct = NewPriceTab.voice[kk];
#else
		uct = NewPriceTab.rate[kk];
#endif
		if((NewPriceTab.rate[kk] == 102)||(NewPriceTab.rate[kk] == 202)){
#ifdef AutoAudio_line
			sprintf(disbuf[index]+strlen(disbuf[index]), ":���|%02X", uct);
#else
			sprintf(disbuf[index]+strlen(disbuf[index]), ":���|%d ", uct);
#endif
		}
		else if((NewPriceTab.rate[kk] == 101)||(NewPriceTab.rate[kk] == 201)){
#ifdef AutoAudio_line
			sprintf(disbuf[index]+strlen(disbuf[index]), ":��Ʊ|%02X", uct);
#else
			sprintf(disbuf[index]+strlen(disbuf[index]), ":��Ʊ|%d ", uct);
#endif
		}
		else if((NewPriceTab.rate[kk] == 103)||(NewPriceTab.rate[kk] == 203)){
#ifdef AutoAudio_line
			sprintf(disbuf[index]+strlen(disbuf[index]), ":Ǯ��|%02X", uct);
#else
			sprintf(disbuf[index]+strlen(disbuf[index]), ":Ǯ��|%d ", uct);
#endif
		}
		else if((NewPriceTab.rate[kk] == 104)||(NewPriceTab.rate[kk] == 204)){
#ifdef BUS_BAOJI_
			sprintf(disbuf[index]+strlen(disbuf[index]), ":���|%02X", uct);
#else
			strcat(disbuf[index], ":��ֹ����");
#endif
		}
		else if((NewPriceTab.rate[kk]>=110) && (NewPriceTab.rate[kk]<=119))
		{//�۴κ���Ҫ�۸��ӷѣ�����Ʊ����101Ҫ��һ�θ��ӷѣ�110���Ӹ��ӷѣ�111��һ�Σ�112�����Σ����ε�119.
			sprintf(disbuf[index]+strlen(disbuf[index]), ":��Ʊ|%02X", NewPriceTab.voice[kk]);
		}
		else if(NewPriceTab.rate[kk] <= 100){
			memcpy((unsigned char*)&i, NewPriceTab.busPrice, 4);
			i *= NewPriceTab.rate[kk];
			i/=100;
			sprintf(disbuf[index]+strlen(disbuf[index]), ":%d%%|%02X", NewPriceTab.rate[kk], NewPriceTab.voice[kk]);
		}
		else{
			strcat(disbuf[index], ":��ֹ����");
		}
		index++;
		if(index >= 20)
			break;
	}

	}//if(gchn_eng != 'E')
#ifdef _debug_
	debugstring("showBUNo:\r\n");
	for (i=0;i<index;i++)
	{
		disbuf[i][29]=0;
		debugstring(disbuf[i]);
		debugstring("\r\n");
	}
#endif
	dispageInfo_L(index, disbuf);
}


#define MOD531 0x31	///ʹ�õ���531ģ��
#define MOD522 0x22	///ʹ�õ���522ģ��

unsigned char getRFmode(void)
{
	//return MOD522;	//
	return MOD531;	
}


extern int getgprslibVer(void);		//��ȡGPRS��汾��
extern int getprolibVer(void);		//��ȡ��汾�š�  prolib ��
extern unsigned char SpeedLimitMode;//
void showVer(void)
{
#ifdef _GPS_ENABLE_
	unsigned char dtb[32];
	unsigned short ii2;
#endif
	char buff[60];
	unsigned int outdly=0;
	unsigned short ii;
	unsigned char k;
#ifdef _TTSSound_enable_33
	int ret;
#endif
	unsigned int ato=0, afree=0;

	cls();

	memset(buff, 0, sizeof(buff));
	if(gchn_eng == 'E')
		display(0, 0, "software VER:", 0);
	else
		display(0, 0,  "�汾��:", 0);


	k = getRFmode();		//�����ͷ������ʾ��22=RC522��RC520����31=RC531��
//	k=0x22;
	memcpy((unsigned char*)&ii, gBusVerInfo.busProVer, 2);

	sprintf((char*)buff, "V%04X", ii);		//PFQUAN  X�µ�ͨѶƽ̨

	miniDispstr(0, 10, buff, 0);

	strcpy(buff, SYS_cityName);
#ifdef _New_Bu_mode_
	strcat(buff, "E-");
#else
	strcat(buff,"B-");//ST
#endif

#ifndef NO_GJ_SERVISE
#ifdef GPRS_MG323_
	strcat(buff,"G");
#elif defined CDMA_MC323_
	strcat(buff, "C");
#elif defined WIFI_TLH_
	strcat(buff, "W");
#elif defined OTHER_GPRS
	#ifdef COM2RJ45_
		strcat(buff, "T");	//͸��
	#else
		strcat(buff, "O");//��Э��
	#endif
#else
	strcat(buff, "N");		//Ӧ��û��
#endif
#else
	strcat(buff, "N");		//û�����ߴ���
#endif

#ifdef _GPS_ENABLE_
#ifdef _GPS_DIAODU_
	strcat(buff, "S");
#else
	strcat(buff, "S");
#endif
#else
	strcat(buff, "N");
#endif
#if !defined NO_MOBILE_24G_ || defined _CPU_24G_
	#ifdef _XMSHDZ_24G_
		strcat(buff, "4");
	#elif defined ZKXL_24G_
		strcat(buff, "3");
	#else
		strcat(buff, "2");
	#endif
#else
	strcat(buff, "N");
#endif

#ifdef BUS_PRO_JSB//���貿��
	strcat(buff, "J");
#elif defined _M1KEY_3DES_ //3DES��Կ 
	strcat(buff, "Y");
#elif defined _M1KEY_CLOUD_
	strcat(buff, "Y");	//��ƽ̨��Կ
#else
	strcat(buff, "S");	//��˾PSAM��Կ
#endif

#ifdef qPBOC_BUS
	strcat(buff, "P");//������qPBOC����
#else
	strcat(buff, "N");
#endif

#ifdef FengDuan_BU_
	strcat(buff, "C");//��CAN����
#else
	strcat(buff, "N");
#endif

	strcat(buff, "HMZ");	//����Ա���� 
	
//	sprintf((char*)buff+40, "_V%d.%02d", (ii/100), (ii%100));
//	sprintf((char*)buff+40, "_V%04X", ii);
	strcat(buff, buff+40);
	miniDispstr(2, 0, buff, DIS_CENTER);	//���ػ��汾��
	
#ifdef _GPS_ENABLE_	 
	miniDispstr(3, 0,  SOFT_VER_TIME, DIS_CENTER);
	memset(buff, 0, 20);
	
	memcpy((unsigned char*)&ii, gBusVerInfo.busVoiceVer, 2);

	sprintf((char*)buff, "Voice:V%d.%02d", (ii/100), (ii%100));
	miniDispstr(4, 0, buff, 0);
#ifdef _TTSSound_enable_
	ret = getADSoundFileVer(dtb, dtb+10, dtb+20);
	if(ret == 0){
		memcpy((unsigned char*)&ii, dtb, 2);//��������汾��
		memcpy((unsigned char*)&ii2, dtb+10, 2);//��·�ţ���·�Ų�һ������Ҳ�����á�
		sprintf((char*)buff, "AD Ver:%d Line:%d", ii, ii2);
	}
	else{
		strcpy((char*)buff, "AD None");
	}
	miniDispstr(5, 0, (char*)buff, 0);
#endif
	
	//display(5, 0,  "վ��汾��:", 0);
	memcpy((unsigned char*)&ii, gBusVerInfo.busLineVer, 2);
if(SpeedLimitMode==SpeedLimit_New)
	sprintf((char*)buff, "ZD_V:%d.%02d_New", (ii/100), (ii%100));
else
	sprintf((char*)buff, "ZD_V:%d.%02d_Old", (ii/100), (ii%100));
	miniDispstr(7, 0, buff, 0);
#ifdef _New_Bu_mode_
	ii = getlibVer();
	sprintf(buff, "LibVer:%d.%02d", (ii/100), (ii%100));
	miniDispstr(6, 0, buff, 0);
#endif
#else
	miniDispstr(3, 0,  "VerDate:", 0);
	miniDispstr(4, 0,  SOFT_VER_TIME, DIS_CENTER);
	memset(buff, 0, 20);
	
	memcpy((unsigned char*)&ii, gBusVerInfo.busProVer, 2);
	sprintf(buff, "ProVer:%d.%02d", (ii/100), (ii%100));
	miniDispstr(5, 0, buff, 0);
#ifdef _New_Bu_mode_
	memset(buff, 0, sizeof(buff));
	ii = getlibVer();
	sprintf(buff, "Lib:%d.%02d,Pro:%d,GPRS:%d\x0", (ii/100), (ii%100),getprolibVer(),getgprslibVer());
	miniDispstr(6, 0, buff, 0);
#endif
#endif
	memcpy((unsigned char*)&ii, gBusVerInfo.busBLKVer, 2);
	over_turn(2,(unsigned char*)&ii);

	//sprintf(buff, "BLK:%d", ii);
	if(ii==0||ii==0xFFFF){
		strcpy((char*)buff, "BLAK None");
	}else{
		sprintf((char*)buff, "BLK:%04d->(%04d)", ii,pFistVary.BlackListNum);
	}
	miniDispstr(5, 0, buff, 0);

	k = get_flahFatfree(&ato, &afree);
	
	sprintf(buff,"Flash:%02dM FATA:%dM,FreeFAT:%dM", ugflash_capacity/1024/1024,ato/1024, afree/1024);
	
	miniDispstr(7, 0, buff, 0);


	memcpy((unsigned char*)&ii, gBusVerInfo.Linux4GVer, 2);
	sprintf(buff, "LinuxVer:%04X", ii);
	miniDispstr(9, 0, buff, 0);
	while(1){
		k = getkey(1);
		if(profile(0)!=ST_ERROR)
			return;
		delayxms(50);
		if(outdly++ > 400)
			return;
		if((k == KEY_ESC)||(k == KEY_ENT))
			return;
		else if(k == KEY_UP)
			outdly = 0;
	}
}
#ifndef NO_GJ_SERVISE
void showIP(void)
{
	
	char disbuf[20][20];
	unsigned char tempb[20];
	unsigned char index=0;
#ifndef NO_MOBILE_24G_	  
	unsigned char i;
#endif
	
	for(index=0; index<20; index++)
		memset(disbuf[index], 0, 20);
	index = 0;
#ifdef WIFI_TLH_
	strcpy(disbuf[index], "MAC��ַ:");
	index++;
	strcpy(disbuf[index], "   ");
	BCD2Ascii(gWIfiIpConfig, (unsigned char*)disbuf[index]+1, 6);
	index++;
#endif
	
	strcpy(disbuf[index], _Font_Server_IP);
	index++;
	memcpy(tempb, (unsigned char*)&ClientIP, 4);
	sprintf(disbuf[index], " %d.%d.%d.%d", tempb[0],tempb[1],tempb[2],tempb[3]);
	index++;
	strcpy(disbuf[index], _Font_Server_PORT);
	sprintf(disbuf[index]+strlen(_Font_Server_PORT)-1, ":%u", ClientPort);
	index++;
	if(sizeof(NETAPN) < 12)
		strcpy(disbuf[index], "APN:");
	strcat(disbuf[index], NETAPN);
	index++;
#ifdef WIFI_TLH_
	strcpy(disbuf[index], "����IP:");
	index++;
	sprintf(disbuf[index], " %d.%d.%d.%d", gWIfiIpConfig[6],gWIfiIpConfig[7],gWIfiIpConfig[8],gWIfiIpConfig[9]);
	index++;
	strcpy(disbuf[index], "��������:");
	index++;
	sprintf(disbuf[index], " %d.%d.%d.%d", gWIfiIpConfig[10],gWIfiIpConfig[11],gWIfiIpConfig[12],gWIfiIpConfig[13]);
	index++;
	strcpy(disbuf[index], "Ĭ������:");
	index++;
	sprintf(disbuf[index], " %d.%d.%d.%d", gWIfiIpConfig[14],gWIfiIpConfig[15],gWIfiIpConfig[16],gWIfiIpConfig[17]);
	index++;
#endif
	
#ifndef NO_MOBILE_24G_
	strcpy(disbuf[index], "�ƶ�IP:");
	index++;

	memcpy(disbuf[index], gMobileParameter.manageIP, 15);
	index++;
	strcpy(disbuf[index], "����˿�:");
	index++;
	i = strlen((char*)gMobileParameter.managePort);
	if(i >= 6)
		memcpy(disbuf[index], (char*)gMobileParameter.managePort, 5);
	else
		memcpy(disbuf[index], gMobileParameter.managePort, i);
	index++;

	strcpy(disbuf[index], "���׶˿�:");
	index++;
	i = strlen((char*)gMobileParameter.purPort);
	if(i >= 6)
		memcpy(disbuf[index], (char*)gMobileParameter.purPort, 5);
	else
		memcpy(disbuf[index], (char*)gMobileParameter.purPort, i);
	index++;
	
#endif
	dispageInfo(index, disbuf);
}
#endif

void StatsMoney(void)
{
	char disbuf[32];
	stStaeInfo stainto;
	unsigned int i,index;
	unsigned int itemp,outdly=0;
	unsigned char ret;
	
	for(i=0;i<3;i++){
		sysferead(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);//ÿ�����һ����ʷָ���ͳ������ ����ʱ��7+��ֵ�ۼƽ��4+�����ۼƽ��4+�ۿ�����2+����11+CRC32(4)
		
		itemp = GenerateCRC32((unsigned char*)&stainto, (sizeof(stStaeInfo)-4));

// 		MSG_LOG("RS stStaeInfo:");
// 		BCD_LOG((unsigned char*)&stainto, sizeof(stStaeInfo), 1);
// 		BCD_LOG((unsigned char*)&itemp, 4, 1);
		
		if(stainto.sCrc32 == itemp)
			break;
	}
	if(i >= 3){
		memset((unsigned char*)&stainto, 0, sizeof(stStaeInfo));
		MSG_LOG("bͳ�Ƽ�������\r\n");
	}
	
	for(index=0; index<8; index++){//�Ҵ˲���Ա�ı���λ��
		if(memcmp(stainto.infoBuf[index].cardNo, DriveCardNo, 4) == 0){
			break;
		}
	}
	
#ifdef _debug_d
	MSG_LOG("Drive CardNo:");
	BCD_LOG((unsigned char*)&DriveCardNo, 4, 0);
	MSG_LOG("check index2:");
	BCD_LOG((unsigned char*)&index, 1, 0);
	MSG_LOG("::");
	if(index < 8){
		BCD_LOG((unsigned char*)&stainto.infoBuf[index], sizeof(stSatone), 1);
		BCD_LOG((unsigned char*)stainto.infoBuf[index].cardNo, 4, 1);
		BCD_LOG((unsigned char*)&stainto.infoBuf[index].chargeA, 4, 1);
		BCD_LOG((unsigned char*)&stainto.infoBuf[index].RseverA, 4, 1);
		BCD_LOG((unsigned char*)&stainto.infoBuf[index].SaleA, 4, 1);
	}
#endif
	
	cls();
	
	if(index >= 8){
		display(3, 0, _Font_NOREC, DIS_CENTER);
		memset(disbuf, 0, 20);
#ifdef BUS_YICHUN
		sprintf(disbuf, "Ա����:%02X%02X%02X", StufferNO[1],StufferNO[2],StufferNO[3]);
#else
		memcpy((unsigned char*)&i, DriveCardNo, 4);
		sprintf(disbuf, "ID:%06d ", i);
#endif
		display(0,0,disbuf,0);
	}
	else{
		memset(disbuf, 0, 20);
		
#ifdef BUS_YICHUN
		sprintf(disbuf, "Ա����:%02X%02X%02X", StufferNO[1],StufferNO[2],StufferNO[3]);
#else
		memcpy((unsigned char*)&i, stainto.infoBuf[index].cardNo, 4);
		sprintf(disbuf, "ID:%06d ", i);
#endif
		display(0,0,disbuf,0);
		sprintf(disbuf, _Font_KouKuandian, stainto.infoBuf[index].chargeA/100, stainto.infoBuf[index].chargeA%100);
		display(2,0,disbuf,0);
		
		sprintf(disbuf, _Font_KouCIdian, stainto.infoBuf[index].RseverA);
		display(4,0,disbuf,0);
		
		sprintf(disbuf,_Font_SwipTimes, stainto.infoBuf[index].SaleA);
		display(6,0,disbuf,0);
		
// 		sprintf(disbuf, "����:%02X%02X-%02X-%02X ", stainto.DataTime[0],\
// 			stainto.DataTime[1], stainto.DataTime[2], stainto.DataTime[3]);
// 		display(6,0,disbuf,0);
	}
	i = 0;
	while(1){
		if(outdly++ > 200)
			return;
		delayxms(50);
		ret = getkey(1);
		if(ret == 0)
			continue;
		outdly = 0;
		if((ret == KEY_ESC)||(ret == KEY_ENT)){
			return;
		}
	}
}
#ifdef _GPS_NEED_REC_
extern stNSpeedInfo SpeedInfo[2];//������Ϣ���� ��������0�� ����    1�鷴��
void ShowLimintSpeed(void)//��ʾ������Ϣ
{	
	unsigned char i,Index=0;
	char disbuf[50][20];//��ʾ
	unsigned short speed;
	for(i=0;i<50;i++)
	{
		memset(disbuf[i],0,20);
	}
	for(i=0; i<SpeedInfo[0].ucStationMax; i++){
		
		
		//SpeedInfo[0].SpeedPInfo[i].usRDNo;//վ��
		//SpeedInfo[0].SpeedPInfo[i].usTIme1;//�ٶ�
		memcpy((unsigned char*)&speed,(unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usTIme1,2);
		over_turn(2,(unsigned char*)&speed);
		sprintf(disbuf[i],"����+:%02d,%02dkm",SpeedInfo[0].SpeedPInfo[i].usRDNo,speed/100);
		Index++;
	}
	
	i=0;
	for(i=0; i<SpeedInfo[1].ucStationMax; i++){
		
		
		//		SpeedInfo[1].SpeedPInfo[i].usRDNo;//վ��
		//		SpeedInfo[1].SpeedPInfo[i].usTIme1;//�ٶ�
		
		memcpy((unsigned char*)&speed,(unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usTIme1,2);
		over_turn(2,(unsigned char*)&speed);	
		sprintf(disbuf[Index],"����-:%02d,%02dkm",SpeedInfo[1].SpeedPInfo[i].usRDNo,speed/100);
		Index++;
	}
	
	dispageInfo(Index-1,disbuf);
}
#endif
#ifdef SelMsg_enable
//��ʾû�д��Ķ���
extern void disUNMsg(void);
#endif
extern void showmobilePra(void);
extern void show24gCard(void);
void ShowQu(void)
{
	unsigned char ucMENU_NUM;
	
	char menu[8][20];
	void (*function[8])(void);
	
	ucMENU_NUM = 0;

#ifdef NO_MOBILE_24G_
#ifdef NO_GJ_SERVISE
	strcpy(menu[0], _Font_Records);
	strcpy(menu[1], _Font_Device_Info);
	strcpy(menu[2], _Font_HW_Ver);
	strcpy(menu[3], _Font_un_upload);
	function[0] = Showcard;
	function[1] = showBUNo;
	function[2] = showVer;
	function[3] = showGPRStR;
	ucMENU_NUM = 4;
#else
	strcpy(menu[ucMENU_NUM], _Font_Records);
	function[ucMENU_NUM++] = Showcard;
	strcpy(menu[ucMENU_NUM], _Font_Device_Info);
	function[ucMENU_NUM++] = showBUNo;

// 	strcpy(menu[ucMENU_NUM], _Font_Statistics);
// 	function[ucMENU_NUM++] = StatsMoney;

	strcpy(menu[ucMENU_NUM], _Font_Server_IP);
	function[ucMENU_NUM++] = showIP;
	strcpy(menu[ucMENU_NUM], _Font_HW_Ver);
	function[ucMENU_NUM++] = showVer;
#ifdef _GPS_NEED_REC_
	strcpy(menu[ucMENU_NUM], "������Ϣ");
	function[ucMENU_NUM++] = ShowLimintSpeed;
#endif
	strcpy(menu[ucMENU_NUM], _Font_un_upload);
	function[ucMENU_NUM++] = showGPRStR;
#ifdef QR_CODE_COM
	strcpy(menu[ucMENU_NUM], "�汾��Ϣ");
	function[ucMENU_NUM++] = show_QR_Ver;
#endif
	#ifdef qPBOC_BUS
	strcpy(menu[ucMENU_NUM], "�鿴�������� ");
	function[ucMENU_NUM++] = Show_pboc_minu;
#endif
#endif 
#else
	strcpy(menu[0], "ˢ����¼ ");
	strcpy(menu[1], "�ֻ���¼ ");
	strcpy(menu[2], "�豸��Ϣ ");
	strcpy(menu[3], "��ѯIP   ");//ˢ��ͳ��
	strcpy(menu[4], "��ѯ���� ");
	strcpy(menu[5], "��ͷ״̬ ");
	strcpy(menu[6], "�� �� �� ");
	strcpy(menu[7], "δ����¼ ");
	function[0] = Showcard;
	function[1] = show24gCard;
	function[2] = showBUNo;
	function[3] = showIP;
	function[4] = showmobilePra;
	function[5] = G24ReadStyle;
	function[6] = showVer;
	function[7] = showGPRStR;
#endif
	
	MenuFrame(menu, function,  _Font_Query_menu, ucMENU_NUM, (100));//"  --query menu--  "
}

void com_control(void)
{
	unsigned int addr, i;
	int head;

// 	com_init(COM2, 115200);
// 	DEBUG_It();
	if(getpassword() != 1)
		return;
	i = Get_Record_point((unsigned char*)&head, 0);
	i -= head;
	addr = head;
	USART1mode = 1;
	com_up_data((unsigned char*)&addr, i);
	USART1mode = 0;
	if(addr != head){//˵����ǰ���ݼ����ɼ�
		GetDatEnd(0);
	}
		
// #ifdef _GPS_ENABLE_
// 	com_init(GPS_COM, 9600);
// 	COM_it_enable(COM1);//2.4G����Ҫ�ж�
// 	gps_config_hw();
// 	delayxms(100);
// 	gps_config_hw();
// #else
// 	com_init(COM1, 115200);
// 	COM_it_enable(COM1);//2.4G����Ҫ�ж�
// #endif
	
}

#ifndef NO_MOBILE_SERVISE
void ShowUPMobile(void)
{
	if(gMobilREC_Start != gMobilREC_End)
		uploadSettleData();//�������
}
#endif

#ifdef _TTSSound_enable_
extern void setM50TTS(unsigned char vi);
void VolumeChg(void)
{
	char buff[100];
	unsigned int outdly=0;
	unsigned char i;
	unsigned char vvv;

	sysferead(BIT_GPSSOUND_V, 1, (unsigned char*)&vvv);
	while(1){
		cls();
		display(0,0,Conn("    ", _Font_Volume), DIS_CENTER|DIS_CONVERT);

		display(6,0,"<-          ->", 0);
		memset(buff, 0, 100);
		sprintf(buff, "%d", vvv);
		display(3,0,buff, DIS_CENTER);
//		sprintf(buff, "��������", vvv);
//		MSG_LOG(buff);
		sysfewrite(BIT_GPSSOUND_V, 1, (unsigned char*)&vvv);
		setM50TTS(vvv);
		SoundFreeManage("��������");

		while(1){
			if(outdly++ > 200){
#ifndef _New_Bu_mode_
				XF_Reset_LOW();
#endif
				return;
			}
			delayxms(50);
			clr_wdt();
			i = getkey(1);
			if(i == 0)
				continue;
			outdly = 0;
			if(i == KEY_ESC){
#ifndef _New_Bu_mode_
				XF_Reset_LOW();
#endif
				return;
			}
			else if((i == KEY_DOWN)||(i == KEY_ENT)){
				if(vvv >= 15)
					vvv = 0;
				else
					vvv++;
				break;
			}
			else if(i == KEY_UP){
				if(vvv == 0)
					vvv = 15;
				else
					vvv--;
				break;
			}
		}
	}
}
#endif


#ifdef _FASH_FATs_SYS_
extern void upFileCmd(void);
void DealupFlieCmd(void)
{
	USART1mode = 2;
	
	close_rfile_Wrecord();
	
	upFileCmd();
	USART1mode = 0;
	
	open_rfile_Wrecord();
	
	
}
#endif
void ShowMainMenu(void)
{
	unsigned char ucMENU_NUM;
	
	char menu[6][20];
	void (*function[6])(void);

	setCUM_PER_SCREEN(16);//����һ������ʾ������
#ifdef NO_MOBILE_24G_
#ifdef _TTSSound_enable_
	ucMENU_NUM = 0;
	strcpy(menu[ucMENU_NUM], _Font_query_info);
	function[ucMENU_NUM++] = ShowQu;
	
#ifdef BUS_LICHUANG_
	if(F_FLAG_VARY.stop_flag){//��վʱ����ʾ���ò˵�	
#endif
	strcpy(menu[ucMENU_NUM], _Font_MSG_SETUP);
	function[ucMENU_NUM++] = set_main;
	strcpy(menu[ucMENU_NUM], _Font_PC_Cnnect);
	function[ucMENU_NUM++] = com_control;
#ifdef _FASH_FATs_SYS_
	strcpy(menu[ucMENU_NUM], "�ļ�ͨѶ ");
	function[ucMENU_NUM++] = DealupFlieCmd;
#endif

#ifdef FengDuan_BU_
	if(gFDMode == FDMODE_GPS){
#endif
	strcpy(menu[ucMENU_NUM], _Font_Volume);
	function[ucMENU_NUM++] = VolumeChg;
#ifdef FengDuan_BU_
	}
#endif

#ifdef BUS_LICHUANG_
	}
#endif//��վʱ����ʾ���ò˵�
	
#else
	ucMENU_NUM = 0;
	strcpy(menu[ucMENU_NUM], _Font_query_info);
	function[ucMENU_NUM++] = ShowQu;
#if !defined BUS_JIUJIANG_ && !defined BUS_HANDAN_
	strcpy(menu[ucMENU_NUM], _Font_MSG_SETUP);
	function[ucMENU_NUM++] = set_main;
#endif
#ifdef _FASH_FATs_SYS_
	strcpy(menu[ucMENU_NUM], "�ļ�ͨѶ ");
	function[ucMENU_NUM++] = DealupFlieCmd;
#endif
	strcpy(menu[ucMENU_NUM], _Font_PC_Cnnect);
	function[ucMENU_NUM++] = com_control;
#endif
#else
	ucMENU_NUM = 4;
	strcpy(menu[0], _Font_query_info);
	strcpy(menu[1], _Font_MSG_SETUP);
	strcpy(menu[2], _Font_UPData);
	strcpy(menu[3], _Font_PC_Cnnect);
	function[0] = ShowQu;
	function[1] = set_main;
	function[2] = ShowUPMobile;
	function[3] = com_control;
#endif
	MenuFrame(menu, function, _Font_OptionsMenu, ucMENU_NUM, (100));

}
#ifndef NO_MOBILE_SERVISE
void debug_mobile_data(void)
{	
	unsigned int indexn;
	unsigned int index_his;
	unsigned int startadd;
	unsigned char ii;
	unsigned char buff[200];

	sysferead(BIT_SendSeril, 5, buff);
	BCD_LOG(buff, 5, 1);
	BCD_LOG((unsigned char*)&mobilsendStyle, sizeof(stMobilSend), 1);

	indexn=(gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
	indexn/=MOBILE_REC_LEN;
	
	startadd = FLASH_M_REC_START;
	index_his = gMobilREC_Start;
	for(ii=0; ii<indexn; ii++){
		startadd = FLASH_M_REC_START+index_his;
		flashread(startadd, buff, MOBILE_REC_LEN);
		BCD_LOG(buff, MOBILE_REC_LEN, 1);
		index_his += MOBILE_REC_LEN;
		if(index_his > LEN_M_REC)
			index_his = 0;
	}
}
#endif
extern void delayxmstt(unsigned int xms);
extern void gprs_snd(unsigned int len, unsigned char *dat);
extern void GPRSModeBaudChage(void);

extern int findSamKey(unsigned char ver);

#ifdef KEY_PAD
extern volatile unsigned char gKeyMode;	//��ǰKEYPAD�Ĺ���״̬
extern unsigned char GPRS_Ring_ing(void);
#endif
#ifdef SelMsg_enable
extern void addSelMsg(char *SelNo, char *msg, unsigned char len);
extern void debugSelMsg(void);
#endif


extern volatile unsigned short gprsIndex;
extern void CountMac_CUP(unsigned char *Key,unsigned char *random,unsigned char *Sdat,unsigned int len ,unsigned char *Result);

/*==========================================================================*
 * FUNCTION : CRC16 calculation
 * PURPOSE  :
 * CALLS    :
 * CALLED BY:
 * ARGUMENTS: unsigned char   *Msg :
 *            unsigned short  len  :
 * RETURN   : unsigned short :
 * COMMENTS : Copy
 * CREATOR  : Li Xi Dong               DATE: 2004-09-11 11:15
 *==========================================================================*/
static unsigned short CRC16_Table[256] = {
	 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
short CRC16(unsigned char *ptr, int pSize, unsigned char *pCRC)
{
	unsigned int i;
	unsigned char tmp[2]={0};
	unsigned short crc;
	unsigned short st;

	printf(" CRC16 \r\n");
	debugdata((unsigned char*)ptr, pSize, 1);

	tmp[0] = 0x00;//pCRC[0];
	tmp[1] = 0x00;//pCRC[1];
	memcpy((unsigned char*)&st, tmp, 2);

	crc = st;
	for (i=0; i<pSize; i++){
//		over_turn(2, (unsigned char*)&crc);
		st = CRC16_Table[(crc ^ *ptr++) & 0xff];
		over_turn(2, (unsigned char*)&st);
		crc = ((crc >> 8) & 0xff) ^ st;
	}
	if (pCRC != NULL)
	{
		pCRC[0] = (char)((crc & 0xff00)>>8);
		pCRC[1] = (char)(crc & 0x00ff);
	}
	
	printf("crc:%04X\r\n", crc);
	return (crc);
}

//crc-16 Forward��CRC16=X^16+X^15+X^5+1  [0x8021]
unsigned int GetCrcWord_8021(unsigned char* ptr,int length)
{
	union crc
	{
		unsigned short ui;
		char   chr[2];
	}crcTemp;
	//	crc crcTemp;
	unsigned char i;
	char ct;
	
	crcTemp.ui=0;
	
	while(length-- != 0)
	{
		crcTemp.ui = (unsigned int)(crcTemp.ui ^(unsigned int)((*ptr++)<<8));
		for ( i=0; i<8; i++)
		{
			if ((crcTemp.ui & 0x8000) != 0)
				crcTemp.ui = (unsigned int)((crcTemp.ui >> 1) ^ 0x2180);
			else
				crcTemp.ui = (unsigned int)(crcTemp.ui >> 1);//<< 1);
		}
	}
	
	//�ߵ�λ����
	ct = 	crcTemp.chr[0];
	crcTemp.chr[0] = crcTemp.chr[1];
	crcTemp.chr[1] = ct;
	
	return crcTemp.ui;
}
#if defined _debug_|| defined _debug_DVR
void Int_Tes(void)
{//��ʼ������
	memcpy(pFistVary.LineNo,"\x00\x01\x00",3);
		memcpy(NewPriceTab.busPrice,"\x01\x00\x00\x00",4);
	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
	savesysbupara();
	saveCpuInfo(1,pFistVary.LineNo);
}
extern void DisIcon8(unsigned char x, unsigned char y,unsigned char *disbuf);//��ʾ16*16��ͼ��
extern unsigned char  CountPointRead(void);
extern unsigned char getDafang_CPUKey(unsigned char *Sdata, unsigned char *Rkey);
extern void ERR_DIS(char *dis,unsigned char *dat);//��ʾ����

void key_test_test(void)
{
	unsigned int ret=0;
//	unsigned char buffer[32];


	unsigned int temp=0;
	unsigned char buffer[100];
	MSG_LOG("do %s\r\n",__FUNCTION__);
	temp = 100;
	
	memset(NewPriceTab.rate,100,32);
	NewPriceTab.rate[CARD_NORMAL-0x40] = 100;
	NewPriceTab.rate[CARD_STUDENT-0x40] = 100;
	NewPriceTab.rate[CARD_OLDMAN-0x40] = 50;//Audio_BIRTHDAY;//Audio_OLDMAN;
	NewPriceTab.rate[CARD_STUFF-0x40] = 102;

	NewPriceTab.rate[CARD_Love_BUS-0x40] = 102;
	
	memcpy(NewPriceTab.busPrice,&temp,4);
	sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);
	
	memcpy(pFistVary.LineNo,"\x00\x55\x01",3);
	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
	
	memset(NewPriceTab.lineVer ,0,2);
	
	savesysbupara();
	saveCpuInfo(1, buffer);
	SwitchBusClose(0);
	

/*
	unsigned char buff[2];
	MSG_LOG("do %s\r\n",__FUNCTION__);
	buff[0]= 0;
	buff[1 ] = 0;
	sysfewrite(BIT_TACVER,2,buff);
	sysfewrite(BIT_WHTVER,2,buff);
	sysfewrite(BIT_QR_BLK_VER,2,buff);
	sysfewrite(BIT_SPKVER,2,buff);
	flasherase(ERASE4KBYTESECTOR,FLASH_QRCTAC_START);
	flasherase(ERASE4KBYTESECTOR,FLASH_MOBILE_START);//��Կ
*/

/*

	unsigned char buff[500];
	unsigned int i=0;


	for (i=0;i<3;i++)
	{
		Ascii2BCD("D2F8D0D0CAFDBEDD0000000000613030303030303030303030303030303030306223670100004522768F2312000134B9955DDD8E41690107140103900000010A010000097527DB8E15BA00000000000000000000000000CEA1EBCA01AC00000000007C00003000014543433030315933010013000F00017CF720170921182837009998000100010013003CD000000000E05A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A5A"\
					,buff,256*2);

		WriteRecord(buff,0);
		WriteRecord(buff+128,0);


		
		memset(buff,i+100,128);
		WriteRecord(buff,0);

	}
*/

/*

	Ascii2BCD("6210000000109102",buffer,16);
	ret =   IsInBlackList(buffer);
	MSG_LOG("ret=%08x\r\n",ret);

	Ascii2BCD("6210000000000010",buffer,16);
	ret =   IsInBlackList(buffer);
	MSG_LOG("ret=%08x\r\n",ret);

	Ascii2BCD("6210000000106345",buffer,16);
	ret =   IsInBlackList(buffer);
	MSG_LOG("ret=%08x\r\n",ret);
	
	Ascii2BCD("6210000000109103",buffer,16);
	ret =   IsInBlackList(buffer);
	MSG_LOG("ret=%08x\r\n",ret);
	*/

// 	gsl8583Style.ISOK = 0;
// 	delallBlacInbus();

//	MSG_LOG("BLK_SNO_LEN:%d\r\n",BLK_SNO_LEN);

// 	ClrGPRSFE();


	//182.137.15.10	8095

/*
	buffer[0] = 182;
	buffer[1] = 137;
	buffer[2] = 15;
	buffer[3] = 10;
	memcpy(&ClientIP,buffer,4);
	ClientPort = 8095;
	sysfewrite(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
	sysfewrite(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
	savesysbupara();
*/

	


	
}

void teflasgh(void)
{
	unsigned char ret;
	unsigned char buff[8], bl=4;
	char s[9];
	memcpy(s, "000A1234\x0", 9);
	printf("----s:%s\r\n", s);
	ret = ASCII2ZBcod(s, buff, bl);
	printf("A2Z:%d\r\n", ret);
	debugdata(buff, bl, 1);

	ret = ZBcod2ASCII(buff, bl, s);
	printf("Z2A:%d\r\n", ret);
	debugdata(buff, bl, 1);
	printf("s:%s\r\n", s);
	debugdata((unsigned char*)s, 8, 1);
	
	printf("----s:%s\r\n", s);
	ret = ASCII2ZBcod(s, buff, bl);
	printf("A2Z:%d\r\n", ret);
	debugdata(buff, bl, 1);
	
	ret = ZBcod2ASCII(buff, bl, s);
	printf("Z2A:%d\r\n", ret);
	debugdata(buff, bl, 1);
	printf("s:%s\r\n", s);
	debugdata((unsigned char*)s, 8, 1);
}

#endif


void set_key_line(void)
{

	unsigned char buffer[256];

	unsigned int temp;
	unsigned short crc16 = 0;
	unsigned char uc = 0;
	unsigned int i = 0;

	unsigned char buff[130];
	unsigned int addr;
	unsigned char md5out[16];
	unsigned char okey[8];
	unsigned char disbuf[8];


		// ����������¼
// 	save_repurse_infor(FeRC_Dlelt, NULL);
// 	return;
#if 0
	down_kek_ic();
	return;
#endif
//   temp=400000;
// sysfewrite(BIT_PBOC_NS_8583, 4, (unsigned char*)&temp);
#if 0
		save_repurse_infor(FeRC_Dlelt, NULL);
		saveMobileParameter(1, "952510741110015");		//�̻�
	
	Ascii2BCD("6000090000", buffer, 10);
	saveMobileParameter(5, buffer);		//TPDU
	
	saveMobileParameter(6, "10171020");		//�豸��
	

			   //3EA419EA2945836468A7739B1591D0DF
	Ascii2BCD("A43BE0019E4002EFF43E326B345ECE6E", buffer, 32);
//	Ascii2BCD("57C8F7CD1FC4F44619C70B1C29E354C8", buffer, 32);
	saveMobileParameter(2, buffer);		//kek����
	
	// 	139.199.161.164 2020
	//219.141.123.150
// 	buffer[0] = 182;
// 	buffer[1] = 131;
// 	buffer[2] = 1;
// 	buffer[3] = 218;
// 	memcpy(&ClientIP, buffer, 4);
// 	ClientPort = 7788;
// 	sysfewrite(BIT_GPRS_IP, 4, (unsigned char*)&ClientIP);
// 	sysfewrite(BIT_GPRS_PORT, 2, (unsigned char*)&ClientPort);
// 	savesysbupara();
	
	
	//������202.101.25.188:20140
	//���ţ�101.231.141.158:30000	
	buffer[0] = 182;
	buffer[1] = 131;
	buffer[2] = 1;
	buffer[3] = 218;
	temp = 7788;
	saveMobileParameter(3, buffer);
	saveMobileParameter(4, (unsigned char *)&temp);

	return;
#endif
#if 0	
	save_repurse_infor(FeRC_Dlelt, NULL);
	MSG_LOG("len:%d\r\n", sizeof(stMobileParameter));
	saveMobileParameter(1, "952510741110015");		//�̻�
	
	Ascii2BCD("6000090000", buffer, 10);
	saveMobileParameter(5, buffer);		//TPDU
	
	saveMobileParameter(6, "10170978");		//�豸��
	

			   //3EA419EA2945836468A7739B1591D0DF
	Ascii2BCD("3EA419EA2945836468A7739B1591E9DF", buffer, 32);
//	Ascii2BCD("57C8F7CD1FC4F44619C70B1C29E354C8", buffer, 32);
	saveMobileParameter(2, buffer);		//kek����
	
	// 	139.199.161.164 2020
	//219.141.123.150
// 	buffer[0] = 182;
// 	buffer[1] = 131;
// 	buffer[2] = 1;
// 	buffer[3] = 218;
// 	memcpy(&ClientIP, buffer, 4);
// 	ClientPort = 7788;
// 	sysfewrite(BIT_GPRS_IP, 4, (unsigned char*)&ClientIP);
// 	sysfewrite(BIT_GPRS_PORT, 2, (unsigned char*)&ClientPort);
// 	savesysbupara();
	
	
	//������202.101.25.188:20140
	//���ţ�101.231.141.158:30000	
	buffer[0] = 182;
	buffer[1] = 131;
	buffer[2] = 1;
	buffer[3] = 218;
	temp = 7788;
	saveMobileParameter(3, buffer);
	saveMobileParameter(4, (unsigned char *)&temp);

	return;

#endif
#if 0	// �ɵ��������Բ���
	MSG_LOG("len:%d\r\n", sizeof(stMobileParameter));
	saveMobileParameter(1, "936522341118801");		//�̻�

	Ascii2BCD("6006010000", buffer, 10);
	saveMobileParameter(5, buffer);		//TPDU

	saveMobileParameter(6, "28800788");		//�豸��

	Ascii2BCD("4CF8ADDC29167F15674CBCBAE3406B51", buffer, 32);
	saveMobileParameter(2, buffer);		//kek����

										// 	139.199.161.164 2020
										//219.141.123.150
	buffer[0] = 219;
	buffer[1] = 141;
	buffer[2] = 123;
	buffer[3] = 150;
	memcpy(&ClientIP, buffer, 4);
	ClientPort = 7902;
	sysfewrite(BIT_GPRS_IP, 4, (unsigned char*)&ClientIP);
	sysfewrite(BIT_GPRS_PORT, 2, (unsigned char*)&ClientPort);
	savesysbupara();


	//������202.101.25.188:20140
	//���ţ�101.231.141.158:30000	
	buffer[0] = 101;
	buffer[1] = 231;
	buffer[2] = 141;
	buffer[3] = 158;
	temp = 30000;
	saveMobileParameter(3, buffer);
	saveMobileParameter(4, (unsigned char *)&temp);
#else

// 	MSG_LOG("len:%d\r\n", sizeof(stMobileParameter));
// 	saveMobileParameter(1, "898430170110369");		//�̻�
// 
// 	Ascii2BCD("6000870000", buffer, 10);
// 	saveMobileParameter(5, buffer);		//TPDU
// 
// 	saveMobileParameter(6, "01397906");		//�豸��

#if 0// û�������ĳ˳���
	Ascii2BCD("98765432101234567890123456789012", buffer, 32);
	saveMobileParameter(7, buffer);		//��֤��Կ����
	Ascii2BCD("44444444444444444444444444444444", buffer, 32);
	saveMobileParameter(2, buffer);		//KEK����

	i = BIT_UNPAY_BUS_SM2VERSION;
	sysfewrite(i, BIT_UNPAY_BUS_SM2VERSION_LEN, "\x00\x00");
	i = BIT_UNPAY_BUS_SM2NUM;
	sysfewrite(i, BIT_UNPAY_BUS_SM2NUM_LEN, "\x00\x05");
	i = BIT_UNPAY_BUS_SM2VAL;
	// 100112A00000011250100028040 01140 47CECF10F7852579E380CCA2A5E59D61410D3BDEDCFF263D7CB1BCC8952D6E87B1931E6C5E9B5AE29730564CFA358FBB3853D2AB214CFACAF3CAECFA3FAC8500 BCF75B1BC52280B96F795177E9F28B694A364B37F798988B1CD4D563A85952C5EF93F7741C67230CBB741380DF57AEB641227D27B1F4B27C48776F10569E8A79
	sysfewrite(i, 144, "\x10\x01\x12\xA0\x00\x00\x01\x12\x50\x10\x00\x28\x04\x00\x11\x40\x47\xCE\xCF\x10\xF7\x85\x25\x79\xE3\x80\xCC\xA2\xA5\xE5\x9D\x61\x41\x0D\x3B\xDE\xDC\xFF\x26\x3D\x7C\xB1\xBC\xC8\x95\x2D\x6E\x87\xB1\x93\x1E\x6C\x5E\x9B\x5A\xE2\x97\x30\x56\x4C\xFA\x35\x8F\xBB\x38\x53\xD2\xAB\x21\x4C\xFA\xCA\xF3\xCA\xEC\xFA\x3F\xAC\x85\x00\xBC\xF7\x5B\x1B\xC5\x22\x80\xB9\x6F\x79\x51\x77\xE9\xF2\x8B\x69\x4A\x36\x4B\x37\xF7\x98\x98\x8B\x1C\xD4\xD5\x63\xA8\x59\x52\xC5\xEF\x93\xF7\x74\x1C\x67\x23\x0C\xBB\x74\x13\x80\xDF\x57\xAE\xB6\x41\x22\x7D\x27\xB1\xF4\xB2\x7C\x48\x77\x6F\x10\x56\x9E\x8A\x79");
	i += 144;
	// 100212A00000011250100028040 01140 26216568ADB16A673C6B7ABCC6C4849FAF33458F101F01A5BBABD481FD73BA121F24E72DA1490BF08FB5ED909D319949D41B9100228FE1C5511D1ABDEBD8E844 3E1C894A5AA4DAC4D23A59F49E1444206E673EC657AB58C8DAF1ECAC7F4A9A4BBA3FC726AA7A3ED1FB36B392C37A11AC193FA07D364272C85A7ABB397F23F711
	sysfewrite(i, 144, "\x10\x02\x12\xA0\x00\x00\x01\x12\x50\x10\x00\x28\x04\x00\x11\x40\x26\x21\x65\x68\xAD\xB1\x6A\x67\x3C\x6B\x7A\xBC\xC6\xC4\x84\x9F\xAF\x33\x45\x8F\x10\x1F\x01\xA5\xBB\xAB\xD4\x81\xFD\x73\xBA\x12\x1F\x24\xE7\x2D\xA1\x49\x0B\xF0\x8F\xB5\xED\x90\x9D\x31\x99\x49\xD4\x1B\x91\x00\x22\x8F\xE1\xC5\x51\x1D\x1A\xBD\xEB\xD8\xE8\x44\x3E\x1C\x89\x4A\x5A\xA4\xDA\xC4\xD2\x3A\x59\xF4\x9E\x14\x44\x20\x6E\x67\x3E\xC6\x57\xAB\x58\xC8\xDA\xF1\xEC\xAC\x7F\x4A\x9A\x4B\xBA\x3F\xC7\x26\xAA\x7A\x3E\xD1\xFB\x36\xB3\x92\xC3\x7A\x11\xAC\x19\x3F\xA0\x7D\x36\x42\x72\xC8\x5A\x7A\xBB\x39\x7F\x23\xF7\x11");
	i += 144;
	// 100312A00000011250100028040 01140 905E3CF019303B57851DF6398482AE80C0E192AAE9F272D61D7C354358C447BB65BFFC09DE599369928237F0BCF03B4A54E5D2957605E2699277E3C4E042B344 89F73DF6D069D75DA00203985E649E21337EC1C05384A26E924A51B1072A085BBA692050D3D73CDADD264B2B4A0D62BF8AF837215FD643F783F2403B22ACCE82
	sysfewrite(i, 144, "\x10\x03\x12\xA0\x00\x00\x01\x12\x50\x10\x00\x28\x04\x00\x11\x40\x90\x5E\x3C\xF0\x19\x30\x3B\x57\x85\x1D\xF6\x39\x84\x82\xAE\x80\xC0\xE1\x92\xAA\xE9\xF2\x72\xD6\x1D\x7C\x35\x43\x58\xC4\x47\xBB\x65\xBF\xFC\x09\xDE\x59\x93\x69\x92\x82\x37\xF0\xBC\xF0\x3B\x4A\x54\xE5\xD2\x95\x76\x05\xE2\x69\x92\x77\xE3\xC4\xE0\x42\xB3\x44\x89\xF7\x3D\xF6\xD0\x69\xD7\x5D\xA0\x02\x03\x98\x5E\x64\x9E\x21\x33\x7E\xC1\xC0\x53\x84\xA2\x6E\x92\x4A\x51\xB1\x07\x2A\x08\x5B\xBA\x69\x20\x50\xD3\xD7\x3C\xDA\xDD\x26\x4B\x2B\x4A\x0D\x62\xBF\x8A\xF8\x37\x21\x5F\xD6\x43\xF7\x83\xF2\x40\x3B\x22\xAC\xCE\x82");
	i += 144;
	// 100412A00000011250100028040 01140 56C18DD352AFFBB4C9260870C132B02BF38F17C7480EB3104B372B15AFD0DC8363C683790E374B9FDC54784668B6FAE3DC34F005635C2DD79BCF917EB8313578 7889CE694B4A16E8D68F08106C8B3D4EE4EDA45F33EE051EBCC22CC73E3568953E79BC0FFD71443851A5622FE09BCC49C04F059F5B1522016F79EF32B258030C
	sysfewrite(i, 144, "\x10\x04\x12\xA0\x00\x00\x01\x12\x50\x10\x00\x28\x04\x00\x11\x40\x56\xC1\x8D\xD3\x52\xAF\xFB\xB4\xC9\x26\x08\x70\xC1\x32\xB0\x2B\xF3\x8F\x17\xC7\x48\x0E\xB3\x10\x4B\x37\x2B\x15\xAF\xD0\xDC\x83\x63\xC6\x83\x79\x0E\x37\x4B\x9F\xDC\x54\x78\x46\x68\xB6\xFA\xE3\xDC\x34\xF0\x05\x63\x5C\x2D\xD7\x9B\xCF\x91\x7E\xB8\x31\x35\x78\x78\x89\xCE\x69\x4B\x4A\x16\xE8\xD6\x8F\x08\x10\x6C\x8B\x3D\x4E\xE4\xED\xA4\x5F\x33\xEE\x05\x1E\xBC\xC2\x2C\xC7\x3E\x35\x68\x95\x3E\x79\xBC\x0F\xFD\x71\x44\x38\x51\xA5\x62\x2F\xE0\x9B\xCC\x49\xC0\x4F\x05\x9F\x5B\x15\x22\x01\x6F\x79\xEF\x32\xB2\x58\x03\x0C");
	i += 144;
	// 100512A00000011250100028040 01140 2FDCEB05E3EAB9BB7348E4C2E7E93BE33A4601A506ED7BAB2191FA4F803E927B9BF137EDC1AD5AB219FFDE91ACDCF8B04050A7380D72E501BA2713FED0E92462 E7C66577C60322BA5A5BB373A0F98F7BED0B0785CAB193DC0C4BB65395E7B1CD7B9CEFFEEB98031333B69396C83A512EDDF07BF18069D7063D126134EB129619
	sysfewrite(i, 144, "\x10\x05\x12\xA0\x00\x00\x01\x12\x50\x10\x00\x28\x04\x00\x11\x40\x2F\xDC\xEB\x05\xE3\xEA\xB9\xBB\x73\x48\xE4\xC2\xE7\xE9\x3B\xE3\x3A\x46\x01\xA5\x06\xED\x7B\xAB\x21\x91\xFA\x4F\x80\x3E\x92\x7B\x9B\xF1\x37\xED\xC1\xAD\x5A\xB2\x19\xFF\xDE\x91\xAC\xDC\xF8\xB0\x40\x50\xA7\x38\x0D\x72\xE5\x01\xBA\x27\x13\xFE\xD0\xE9\x24\x62\xE7\xC6\x65\x77\xC6\x03\x22\xBA\x5A\x5B\xB3\x73\xA0\xF9\x8F\x7B\xED\x0B\x07\x85\xCA\xB1\x93\xDC\x0C\x4B\xB6\x53\x95\xE7\xB1\xCD\x7B\x9C\xEF\xFE\xEB\x98\x03\x13\x33\xB6\x93\x96\xC8\x3A\x51\x2E\xDD\xF0\x7B\xF1\x80\x69\xD7\x06\x3D\x12\x61\x34\xEB\x12\x96\x19");
	i += 144;
#endif

	//������202.101.25.188:20140
	//���ţ�101.231.141.158:30000
	// �Ϻ�����183.131.180.56:9210
	buffer[0] = 222;
	buffer[1] = 240;
#if 0
	buffer[2] = 182;
	buffer[3] = 134;
#else
	buffer[2] = 185;
	buffer[3] = 183;
#endif
	temp = 5903;	// 5803
	saveMobileParameter(3, buffer);
	saveMobileParameter(4, (unsigned char *)&temp);

#endif


	temp = 2;

	memset(NewPriceTab.rate, 100, 32);
	// 	NewPriceTab.rate[CARD_NORMAL-0x40] = 90;
	// 	NewPriceTab.rate[CARD_COUPON-0x40] = 101;
	// 	NewPriceTab.rate[CARD_STUDENT-0x40] = 50;
	// 	NewPriceTab.rate[CARD_OLDMAN-0x40] = 101;//Audio_BIRTHDAY;//Audio_OLDMAN;
	// 	NewPriceTab.rate[CARD_STUFF-0x40] = 101;
	// 	NewPriceTab.rate[CARD_RELATION_BUS-0x40]=100;
	// 	NewPriceTab.rate[CARD_HANDICAPPED-0x40] = 100;
	// 	NewPriceTab.rate[CARD_qPBOC_BUS-0x40] = 90;

	memcpy(NewPriceTab.busPrice, &temp, 4);
	sysfewrite(BIT_PRICETAB, sizeof(stPricetable), (unsigned char*)&NewPriceTab);

//	memcpy(pFistVary.LineNo, "\x99\x99\x01", 3);  //������· �Ž�
//	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��

	//	memset(NewPriceTab.lineVer ,0,2);

	savesysbupara();
	saveCpuInfo(1, buffer);
	// 	
	SwitchBusClose(0);

	// ����������¼
	save_repurse_infor(FeRC_Dlelt, NULL);

#if 1
	// ��ʼ��ODA��¼��ַ
//	save_ODA_infor(FeRC_Init, NULL);
#if 0
	sysferead(BIT_PBOC_NS_8583, 4, (unsigned char*)&temp);
	temp += 300000;
	if (temp >= 999999) {
		temp = 333333;
	}
	sysfewrite(BIT_PBOC_NS_8583, 4, (unsigned char*)&temp);
#endif

#endif
}

void PRICE_init_frist(void)
{
	unsigned int temp;
	unsigned char buffer[100];
	temp = 100;
	
	memset(NewPriceTab.rate, 100, 32);
	// 	NewPriceTab.rate[CARD_NORMAL-0x40] = 90;
	// 	NewPriceTab.rate[CARD_COUPON-0x40] = 101;
	// 	NewPriceTab.rate[CARD_STUDENT-0x40] = 50;
	// 	NewPriceTab.rate[CARD_OLDMAN-0x40] = 101;//Audio_BIRTHDAY;//Audio_OLDMAN;
	// 	NewPriceTab.rate[CARD_STUFF-0x40] = 101;
	// 	NewPriceTab.rate[CARD_RELATION_BUS-0x40]=100;
	// 	NewPriceTab.rate[CARD_HANDICAPPED-0x40] = 100;
	// 	NewPriceTab.rate[CARD_qPBOC_BUS-0x40] = 90;
	
	memcpy(NewPriceTab.busPrice, &temp, 4);
	sysfewrite(BIT_PRICETAB, sizeof(stPricetable), (unsigned char*)&NewPriceTab);
	
	memcpy(pFistVary.LineNo, "\x00\x00\x00", 3);  //������· �Ž�
	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
	savesysbupara();
	saveCpuInfo(1, buffer);
}


#ifdef _New_Bu_mode_
void NewBU_main_key(void)
{
	unsigned char k;
//	unsigned short tt;

#ifdef _TTSSound_enable_
	unsigned char buff[100];
#endif
	k = getkey(1);
	if(k!=0)
		BlightOpen();
	else
		return;

	switch(k) 
	{
	case ESC:
// 		audio_reset();
// 		BUflag = 0;
// 		disinit();
// 		InitRC531();
#ifdef _debug_
		debugstring("��������---\r\n");
	//	ReStartCPU();
#endif
		restore_flag = 3;
		
		break;
	case ENTER:
#ifdef _DriverKey_signin_
		if(BUflag != 5){
			BUflag = 5;
			restore_flag = 3;
			break;
		}
		else
			BUflag = 0;
#endif
		BUflag = 0;
		ShowMainMenu();
		restore_flag = 3;
		break;
	case UP:
	case DOWN:
#ifdef _TTSSound_enable_
		strcpy((char	*)buff, "��������");
		SoundFreeManage((char*)buff);
#endif

// #ifdef _GPS_ENABLE_
// 		if(gGpsDisEN == 'G'){
// 			gGpsDisEN = 0;
// 			restore_flag = 3;
// 		}
// 		else{
// 			gGpsDisEN = 'G';
// 			restore_flag = 3;
// 		} 
// #endif
	 
#if defined _debug_|| defined _debug_DVR
		set_key_line();
	//	key_test_test();
#endif
		//main_usb_collect();
// #ifdef _BlueTooth_COM
// 		Bluetooth_main();	//�������Ӳ���
// #endif
		break;
	default:
		break;
	}
}
#endif

void main_key(void)
{

#ifdef _New_Bu_mode_
	NewBU_main_key();
#else //#ifdef _New_Bu_mode_
	unsigned char k;
#ifdef _TTSSound_enable_
	unsigned char buff[100];
#endif
	k = getkey(1);
	if(k!=0)
		BlightOpen();

	switch(k) 
	{
	case ESC:
#ifdef KEY_PAD
		if(gKeyMode == 0xF9){//�Ҷϵ绰
#ifdef GPRS_SOUD_ENABLE
			ucMC323Mode = MC323_Ring_END;
#endif
			gKeyMode = 0;
			gKeyRestorDis = 3;
			restore_flag = 3;
			break;
		}
#ifdef GPRS_SOUD_ENABLE
		if((ucMC323Mode&0xF0) == 0xA0){
			ucMC323Mode = MC323_Ring_END;
			
			gKeyMode = 0;
			gKeyRestorDis = 3;
			restore_flag = 3;
			break;
		}
#endif
#endif
		BUflag = 0;
		audio_reset();
// 		disinit();
// 		InitRC531();
		restore_flag = 3;
		break;
	case F1:
//		key_test_test();
//		addSelMsg("13590292897", "\x20\x12\x07\x11\x12\x34\x56", 7);
// 		MSG_LOG("gprsIndex:");
// 		BCD_LOG((unsigned char*)&gprsIndex, 2, 1);
#ifdef _GPS_TRA_REC_
		ComSumeFile.BR_curE2pointer = 0;
		sysfewrite(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
		break;
#endif
	case ENTER:
#ifdef KEY_PAD
		if(gKeyMode == 0xF9){//�����绰
#ifdef GPRS_SOUD_ENABLE
			memcpy(buff, "ATA\r\n",5);
			gprs_snd(5, buff);
			gKeyMode = 0;
			GPRS_Ring_ing();
#endif
			break;
		}
#endif//#ifdef KEY_PAD
		BUflag = 0;
		ShowMainMenu();
		restore_flag = 3;
		break;
	case UP:
		BUflag = 0;
		break;
	case DOWN:
#ifdef _DriverKey_signin_
		if(BUflag != 5){
			BUflag = 5;
			restore_flag = 3;
		}
		else
			BUflag = 0;
		restore_flag = 3;
#endif
#ifdef _GPS_ENABLE_
		if(gGpsDisEN == 'G'){
			gGpsDisEN = 0;
			restore_flag = 3;
		}
		else{
			gGpsDisEN = 'G';
			restore_flag = 3;
		}
#endif
		break;
	case F2:
//		debugSelMsg();
#ifdef _DriverKey_signin_
		if(BUflag != 5){
			BUflag = 5;
		}
		else
			BUflag = 0;
		restore_flag = 3;
#endif

#ifdef _TTSSound_enable_
		strcpy((char*)buff, "��������");
		SoundFreeManage((char*)buff);
#endif
#ifdef _GPS_TRA_REC_
		USART2mode = _TEST_Irda;
		get_data(0);
#ifdef WIFI_TLH_
		GPRS_INIT(115200);
#else
		GPRS_INIT(9600);
#endif
		USART2mode = _TEST_GPRS;
		restore_flag = 1;
#endif
		break;
	default:
		break;
	}
#endif //#ifdef _New_Bu_mode_
//=============С���̴���==============
#ifdef KEYBOARD
	if(F_FLAG_VARY.stop_flag==0)
	{
		if(KeyDeal() == ST_OK){
			KeyBoardStyle = KEYBOARD_INPUT;
		}
	}
#endif
}

unsigned char getIrdaType(void)
{
// #ifdef BUS_HANDAN_
// 	return 1;	//1������û�����к��ж� �������·��ĳ�����Ҫ����4
// #else
	return 0;
//#endif
}
extern int cmp_time(unsigned char *scrt, unsigned char *dest);
extern int cmp_data(unsigned char *scrt, unsigned char *dest);//�ڴ��������Ҷ�Ӧ�����ڵļ�¼
int get_hisdat_allinfo(unsigned char *stime, unsigned char *etime, unsigned char *buffer)
{
	unsigned int BR_HeadAddress;
	unsigned int BR_c_cur_po;
	unsigned int addr1;
	unsigned int startaddr=0xffffffff,endaddr=0xffffffff;
	int its,ite;
// #ifdef BUS_HANDAN_
// 	DEALRECODE_32B hisrec1, hisrec2;
// #else
	DEALRECODE hisrec1, hisrec2;
//#endif
	
	its = 0;
	ite = 0;
	get_BR_Point(&BR_HeadAddress, &BR_c_cur_po);
	
	addr1 = FLASH2_START1+BR_c_cur_po;
	if(addr1 == FLASH2_START1)
		addr1 = FLASH2_START2END-RECORD_LEN;//ָ�����µ�һ��
	else
		addr1 -= RECORD_LEN;

	MSG_LOG("find0:");
	BCD_LOG(stime, 4, 1);
	BCD_LOG(etime, 4, 1);

	memset((unsigned char *)&hisrec2, 0xff, RECORD_LEN);
	while(1){
		clr_wdt();
		flashread(addr1,(unsigned char*)&hisrec1, RECORD_LEN);

		MSG_LOG("find1:");
		BCD_LOG((unsigned char*)&addr1, 4, 1);
		BCD_LOG((unsigned char*)&hisrec1, RECORD_LEN, 1);

		if(memcmp((unsigned char*)&hisrec1, (unsigned char*)&hisrec2, RECORD_LEN) == 0)//��ȫFFFFF��
			break;
		if((hisrec1.rDealType>0x07)&&(hisrec1.rDealType<0x20)){
			its = 0;
			ite = 0;
		}
		else{
			if(time_invalid(hisrec1.rDealTime) == 1){
				its = cmp_data(hisrec1.rDealTime, stime);
				ite = cmp_data(hisrec1.rDealTime, etime);
			}
			else{
				its = 0;
				ite = 0;
			}
		}
		if(its < 0)
			break;
		else{
			startaddr= addr1;
		}
		if(ite<=0){
			if(endaddr==0xffffffff)
				endaddr = addr1;
		}
		else if(ite > 0){
			endaddr=0xffffffff;
		}
		
		if(addr1 == FLASH2_START1)
			addr1 = FLASH2_START2END-RECORD_LEN;//ָ�����µ�һ��
		else
			addr1 -= RECORD_LEN;
	}
	
	if((startaddr == 0xffffffff)||(endaddr == 0xffffffff)){
		MSG_LOG("find Error1\r\n");
		return -1;
	}
	else {
		MSG_LOG("find:");
		BCD_LOG((unsigned char*)&startaddr, 4, 1);
		BCD_LOG((unsigned char*)&endaddr, 4, 1);

		memcpy(buffer, (unsigned char*)&startaddr, 4);
		its = (endaddr+0x100000-startaddr)%0x100000;
		its += RECORD_LEN;//��Ҫ�������һ����
		memcpy(buffer+4, (unsigned char*)&its, 4);
		BCD2Ascii((unsigned char*)&SysTime.year_h, buffer+8, 7);
		memcpy(buffer+22, pFistVary.DeviceNo, 9);

		MSG_LOG("buffer:");
		BCD_LOG(buffer, 8, 1);

		return 0;
	}
}

//ͨ����Ϣ������ʷ��¼
int get_hisdat_info(unsigned char *stime, unsigned char *etime, unsigned char *buffer)
{
	unsigned int headaddr, endaddr, its;

	MSG_LOG("find his info:");
	BCD_LOG(stime, 4, 1);
	BCD_LOG(etime, 4, 1);

	
	if(getHisAddr(stime, etime, &headaddr, &endaddr) != 0){
		return -1;
	}
	
	MSG_LOG("find:");
	BCD_LOG((unsigned char*)&headaddr, 4, 1);
	BCD_LOG((unsigned char*)&endaddr, 4, 1);

	its = (endaddr+0x100000-headaddr)%0x100000;

	headaddr += FLASH2_START1;

	memcpy(buffer, (unsigned char*)&headaddr, 4);
	memcpy(buffer+4, (unsigned char*)&its, 4);
	BCD2Ascii((unsigned char*)&SysTime.year_h, buffer+8, 7);
	memcpy(buffer+22, pFistVary.DeviceNo, 9);

	MSG_LOG("buffer:");
	BCD_LOG(buffer, 8, 1);

	return 0;
}

void get_current_info(unsigned char *buffer)
{
	unsigned char pos=0;
	unsigned int temp1;
	unsigned int crup;
	unsigned int length;
	pos=0;
	BCD2Ascii((unsigned char*)&SysTime, buffer, 7); //ʱ������
	pos+=14;
	
	pFistVary.DeviceNo[8] = '0';	//�豸�ŵ�9λû���ã���������д�ɡ�0����������һλ���������.
	memcpy(buffer+pos,pFistVary.DeviceNo,9);//�豸��
	pos+=9;
	
	temp1 = 0;
	memcpy((unsigned char*)&temp1,pFistVary.fileSno,2);//�ļ���
	sprintf((char*)buffer+pos,"%05u",temp1);//�ɼ������к�	(ASCII)
	pos+=5;
	
	if((!ComSumeFile.SumMoney)&&(!ComSumeFile.SumMti))
		buffer[pos]=0;
	else
		buffer[pos]=1;//���ѱ�־
	pos+=1;
	
	length=ComSumeFile.SumMoney;//�����ܽ��
	memcpy(buffer+pos,(unsigned char*)&length,4);
	pos+=4;
	
	length=0;
	length=ComSumeFile.SumMti;//�����ܴ���
	memcpy(buffer+pos,(unsigned char*)&length,4);
	pos+=4;
	
	crup = Get_Record_point((unsigned char *)&temp1, 0);

#ifdef NO_GJ_SERVISE
	temp1 = 64;//����ɼ�����ͷ��ʼ
#endif
	length=temp1;
	memcpy(buffer+pos, (unsigned char*)&length, 3);//��ǰ�ļ�����ʼҳ��
	pos+=3;
	
	length=0;
	length=(crup-temp1);
	//over_turn(4,length.charbuffer);
// #ifdef BUS_HANDAN_
// 	length += 96;
// #endif
// 	length = 10240;
	memcpy(buffer+pos,(unsigned char*)&length,4);
	return;
}
void get_current_info_more(unsigned char *buffer,unsigned char *Rom2Info)
{
	unsigned char pos=0;
//#ifdef qPBOC_BUS
//	unsigned int ltemp;
//#endif
	unsigned int temp1;
	unsigned int crup;
	unsigned int length;
	
	sysferead(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	pos=0;
	BCD2Ascii((unsigned char*)&SysTime, buffer, 7); //ʱ������
	pos+=14;
	
	pFistVary.DeviceNo[8] = '0';	//�豸�ŵ�9λû���ã���������д�ɡ�0����������һλ���������.
	memcpy(buffer+pos,pFistVary.DeviceNo,9);//�豸��
	pos+=9;
	
	temp1 = 0;
	memcpy((unsigned char*)&temp1,pFistVary.fileSno,2);//�ļ���
	sprintf((char*)buffer+pos,"%05u",temp1);//�ɼ������к�	(ASCII)
	pos+=5;
	
	if((!ComSumeFile.SumMoney)&&(!ComSumeFile.SumMti))
		buffer[pos]=0;
	else
		buffer[pos]=1;//���ѱ�־
	pos+=1;
	
	length=ComSumeFile.SumMoney;//�����ܽ��
	memcpy(buffer+pos,(unsigned char*)&length,4);
	pos+=4;
	
	length=0;
	length=ComSumeFile.SumMti;//�����ܴ���
	memcpy(buffer+pos,(unsigned char*)&length,4);
	pos+=4;
	
	crup = Get_Record_point((unsigned char *)&temp1, 0);
#ifdef NO_GJ_SERVISE
	temp1 = 64;//����ɼ�����ͷ��ʼ
#endif
	length=temp1;
	memcpy(buffer+pos, (unsigned char*)&length, 3);//��ǰ�ļ�����ʼҳ��
	pos+=3;
	
	length=0;
	length=(crup-temp1);
	//over_turn(4,length.charbuffer);
	//	length += 96;
	//	length = 10240;
	memcpy(buffer+pos,(unsigned char*)&length,4);
	
	
// #ifdef qPBOC_BUS
// 	crup = qPbocGet_Record_point((unsigned char*)&temp1, 0);
// 	ltemp=0;
// 	ltemp=(crup - temp1);
// 	memcpy(Rom2Info, (unsigned char*)&temp1, 4);
// 	memcpy(Rom2Info+4, (unsigned char*)&ltemp, 4);
// #else
	memset(Rom2Info, 0, 8);//û��������¼
//#endif
	return;
}

void get_history_info(unsigned char file_no, unsigned char *buffer)
{
	unsigned char fileNo;
	unsigned char pos=0;
	unsigned int temp1;
	unsigned int length;
	
	fileNo=file_no-1;
	
	BCD2Ascii(hisFCB[fileNo].hisName, buffer, 7); //ʱ������
	pos = 14;

	pFistVary.DeviceNo[8] = '0';	//�豸�ŵ�9λû���ã���������д�ɡ�0����������һλ���������.
	memcpy(buffer+pos,pFistVary.DeviceNo,9);//�豸��
	pos+=9;
	temp1=0;
	memcpy((unsigned char*)&temp1,hisFCB[fileNo].fileSno,2);
	sprintf((char*)buffer+pos,"%05u",temp1);//�ļ����к�
	pos+=5;
	length = FLASH2_START1;//hisFCB[fileNo].strAddr;//�ļ���ʼ��ַ
	memcpy(buffer+pos,(unsigned char*)&length,3);
	
	pos+=3;
	length = 0x100000;//hisFCB[fileNo].fileLength;
	memcpy(buffer+pos,(unsigned char*)&length ,4);//�ļ�����
	
#ifdef _debugif
	MSG_LOG("History Information!!");
	BCD_LOG(buffer,35,1);
#endif
	return;
}
unsigned char GetHisFileSum(void)
{
	return 1;
// 	unsigned char i;
// 	for(i=0;i<10;i++)
// 	{
// 		if(hisFCB[i].fileLength)
// 			continue;
// 		else
// 			break;
// 	}
// 	return i;
}

unsigned char MonthManage(void)
{
	unsigned char i=0, ji;
	unsigned char buffer[25];
	unsigned char date[3];
	unsigned char dateSum=0;
	unsigned char buffer_29[16],buffer_30[16];
	unsigned int temp10, uii, ui30, uinow;
	unsigned int temp20=0,s_sumTmp;
	unsigned int SUMa=0;
	unsigned int SUMb=0;
	unsigned int SUMc=0;
	unsigned long Pm=0;
	unsigned char buffer_33[16];
	unsigned char buffer_34[16];
	unsigned char block29=ST_ERROR;
	unsigned char block30=ST_ERROR;
	unsigned char block2930=ST_ERROR;
	unsigned char block33=ST_ERROR;
	unsigned char block34=ST_ERROR;
	unsigned char block3334=ST_ERROR;
	unsigned char Check_Month=ST_OK;

	//clr_dog();
	if(F_FLAG_VARY.stop_flag)
		return ST_ERROR;
		
	for(i=0;i<error_pointer;i++)
	{
		if(ErrorDetail[i].effFlag!=IS_EFFECT_MONTH) continue;
		else
		{
			if(!memcmp(ErrorDetail[i].Cardno,&c_serial,4))
			{
				switch(ErrorDetail[i].ErrorStep)
				{
					case OPER_TRANSFER_NO:		
					{
						memcpy((unsigned char*)&temp20,ErrorDetail[i].AferToll,4);
						Check_Month=ST_ERROR;
						break;
					}
					case OPER_RECORD_PROFILE:		
					case OPER_PUBLIC_MESSAGE_W:
					case OPER_PUBLIC_MESSAGE:
						return 5;						
					default:
						break;
				}
			}
		}
	}
	i = cardMessage.card_catalog;
	if(i >= 0x40)
		i -= 0x40;

	if((NewPriceTab.rate[i]==202)||(NewPriceTab.rate[i]==102))////�������
	{
		a_sum1=0;		
		s_sum1=0;
		dis_sum2 = 0;
		ErrorOper(IS_EFFECT_MONTH);
		return 5;
	}

/*	if((cardMessage.card_catalog==CARD_OLDMAN_BUS)||(cardMessage.card_catalog==CARD_STUFF_BUS))
	{
		a_sum2.longbuffer=0l;		//�������
		s_sum2.longbuffer=0l;

		CARD_RECORD.CardRecord.dealOldMoney=a_sum2.longbuffer;
		memcpy(CARD_RECORD.CardRecord.dealMoney,s_sum2.charbuffer,3);
		goto month_step10;
	}
*/
	memcpy(date,&SysTime.year_l,3);          //������
	dateSum=(((date[1]&0xf0)>>4)*10)+(date[1]&0x0f);//��ǰ�·ݵ�ֵ
	
	MSG_LOG("Month MMM 1 !!!\r\n");
	BCD_LOG(date, 3, 1);
	BCD_LOG((unsigned char *)&c_serial, 4, 1);
	BCD_LOG(cardMessage.MonthKEYA, 6, 1);
	BCD_LOG(&cardMessage.oddMonthSector, 1, 1);

	//if(profile(1)==ST_ERROR) return ST_ERROR;

	if(MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MonthKEYA,cardMessage.oddMonthSector*4)==0)
	{
		return ST_ERROR; 
	}

	//��Ǯ����Ϣ��
	if(MifareRead(cardMessage.oddMonthSector*4+1,buffer)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.publicMoneySector*4+1);
#endif
		return ST_ERROR;
	}
	memcpy(buffer_29,buffer,16);
	

	MSG_LOG("7-29:");
	BCD_LOG(buffer, 16, 1);

	if(MiBlockInvalid(1,15,buffer_29)==ST_ERROR) 
	{
#ifdef _DEBUG
		error(ERR_CARD_GESHI,cardMessage.oddMonthSector*4+1);
#endif
		return ST_ERROR;
	}
	if(MifareRead(cardMessage.oddMonthSector*4+2,buffer)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.oddMonthSector*4+2);
#endif
		return ST_ERROR;
	}
	memcpy(buffer_30,buffer,16);
	

	MSG_LOG("7-30:");
	BCD_LOG(buffer, 16, 1);

	if(MiBlockInvalid(1,15,buffer_30)==ST_ERROR) 
	{
#ifdef _DEBUG
		error(ERR_CARD_GESHI,cardMessage.oddMonthSector*4+2);	
#endif
		return ST_ERROR;
	}

	MSG_LOG("Month MMM 4 !!!\r\n");

	
	//if(profile(1)==ST_ERROR) return ST_ERROR;
	if(MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.MonthKEYA,cardMessage.dodMonthSector*4)==0)
	{
		return ST_ERROR; 
	}
	//��Ǯ����Ϣ��
	if(MifareRead(cardMessage.dodMonthSector*4+2,buffer)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.publicMoneySector*4);
#endif
		return ST_ERROR;
	}
	memcpy(buffer_34,buffer,16);

	MSG_LOG("8-0:");
	BCD_LOG(buffer, 16, 1);

	if(MifareRead(cardMessage.dodMonthSector*4+1,buffer)==0)
	{
#ifdef _DEBUG
		error(ERR_MIFARE_READ,cardMessage.dodMonthSector*4+1);
#endif
		return ST_ERROR;
	}
	memcpy(buffer_33,buffer,16);

	MSG_LOG("8-1:");
	BCD_LOG(buffer, 16, 1);
	
	block3334=ST_OK;
	if(MifIsPurse(buffer_33)==ST_ERROR) 
	{
		block3334=ST_ERROR;
		
		goto month_step6;
	}
	else 
		block33=ST_OK;
	if(MifIsPurse(buffer_34)==ST_ERROR) 	
	{
		block3334=ST_OK;
		goto month_step6;
	}
	else 
		block34=ST_OK;

	MSG_LOG("Month MMM 8 !!!\r\n");

	if((block33==ST_ERROR)&&(block34==ST_ERROR)) return 120;
	for(i=0;i<4;i++)
	{
		//clr_dog();
		if(buffer_33[3-i]>buffer_34[3-i])
		{
			block3334=ST_ERROR;
			break;
		}
		if(buffer_33[3-i]<buffer_34[3-i])
		{
			block3334=ST_OK;
			break;
		}
	}	
month_step6:
//	block3334 = ST_ERROR;
	if(block3334==ST_ERROR) 
		memcpy(buffer_33,buffer_34,4);
	
	s_sum1 = get_Month_s_sum1();

	memcpy((unsigned char*)&a_sum1,buffer_33,4);
	s_sumTmp = s_sum1;
	

	MSG_LOG("Month MMM 9 !!!\r\n");
	MSG_LOG("a_sum1:%d  s_sum1:%d\r\n", a_sum1, s_sum1);

	gucSoudnorm=0;
	if((a_sum1>=1)&&(a_sum1<=3))		//��Ʊ������3��
	{
		gucSoudnorm=1;
	}
	
	if(Check_Month==ST_ERROR)
	{
		if(temp20 >= a_sum1)//�ڶ���ˢ��
			goto month_step10;
	}

	if(cardMessage.Zicard_catalog == 1){//Ϊ��Ʊ
	
		MSG_LOG("Month year!!!\r\n");

		uii = 0;
		memcpy((unsigned char*)&uii, buffer_29, 3);//��Ч��
		over_turn(4, (unsigned char*)&uii);
		ui30 = 0;
		memcpy((unsigned char*)&ui30, buffer_30, 3);//��Ч��
		over_turn(4, (unsigned char*)&ui30);
		uinow = 0;
		memcpy((unsigned char*)&uinow, (unsigned char*)&SysTime.year_l, 3);
		over_turn(4, (unsigned char*)&uinow);
		if(uii < ui30){
			if(uinow < uii){
				block29=ST_OK;
				block2930=ST_OK;//���껹��
			}
			else if(uinow < ui30){
				block30=ST_OK;//û����һ����
			}
			else
				return 100;
		}
		else{
			if(uinow < ui30){
				block30=ST_OK;
				block2930=ST_OK;//���껹��
			}
			else if(uinow < uii){
				block29=ST_OK;//û����һ����
			}
			else
				return 100;
		}
		if(block29 == ST_OK){
			temp10 = 0;
			memcpy((unsigned char*)&temp10, buffer_29+13, 2);//��ǰ����ܵ���
			SUMb = temp10;
			SUMa = 0;
			if(block2930 == ST_OK){//��һ�껹��
				temp10 = 0;
				memcpy((unsigned char*)&temp10, buffer_30+13, 2);//��ǰ����ܵ���
				SUMb += temp10;
				SUMa = temp10;
			}
			SUMc = SUMb;
		}
		else{
			temp10 = 0;
			memcpy((unsigned char*)&temp10, buffer_30+13, 2);//��ǰ����ܵ���
			SUMb = temp10;
			SUMa = 0;
			if(block2930 == ST_OK){//��һ�껹��
				temp10 = 0;
				memcpy((unsigned char*)&temp10, buffer_29+13, 2);//��ǰ����ܵ���
				SUMb += temp10;
				SUMa = temp10;
			}
			SUMc = SUMb;
		}
	}
	else if(cardMessage.Zicard_catalog == 2){//Ϊ��Ʊ
		MSG_LOG("Month Quarter!!!\r\n");

		if(date[0]==buffer_29[0])//
		{
			block29=ST_OK;
			if(date[0]<buffer_30[0])
			{
				block2930=ST_OK;//���껹��
			}
		}
		else if(date[0]==buffer_30[0]) 
		{
			block30=ST_OK;
			if(date[0]<buffer_29[0])
				block2930=ST_OK;//���껹��
		}
		else 
		{
			return 100;	   //����û�г伾Ʊ
		}

		if(block29==ST_OK)
		{
			uii = 0;
			ji = ((dateSum-1)/3);//���� dataSum=0 - 12
			memcpy((unsigned char*)&uii, buffer_29+1+(ji*3), 3);

			MSG_LOG("��29 :%d | %d\r\n", i, uii);

			if(uii == 0) //����û�г��
			{
				return 100;
			}
			for(i=ji;i<4;i++){
				memcpy((unsigned char*)&uii, buffer_29+1+(i*3), 3);
				SUMb+=uii;

				MSG_LOG("�����ܵ�1: %d\r\n", SUMb);
			}
			temp10 = 0;
			memcpy((unsigned char*)&temp10,buffer_29+13,2);
			SUMc=temp10 ;			//�������µ�����
			
			if(block2930==ST_OK)
			{
				for(i=0;i<4;i++){
					memcpy((unsigned char*)&uii, buffer_30+1+(i*3), 3);
					SUMb+=uii;
					MSG_LOG("�����ܵ�1: %d\r\n", SUMb);
				}
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
				SUMc+=temp10;
			}
			memcpy((unsigned char*)&uii, buffer_29+1+(ji*3), 3);
			SUMa=SUMb-uii;
		}
		else if(block30==ST_OK)
		{
			uii = 0;
			ji = ((dateSum-1)/3);//���� dataSum=0 - 12
			memcpy((unsigned char*)&uii, buffer_30+1+(ji*3), 3);

			MSG_LOG("��30 :%d | %d\r\n", i, uii);
			if(uii == 0) //����û�г��
			{
				return 100;
			}
			for(i=ji;i<4;i++){
				memcpy((unsigned char*)&uii, buffer_30+1+(i*3), 3);
				SUMb+=uii;
				MSG_LOG("�����ܵ�3: %d\r\n", SUMb);
			}
			temp10 = 0;
			memcpy((unsigned char*)&temp10,buffer_30+13,2);
			SUMc=temp10 ;			//�������µ�����
			
			if(block2930==ST_OK)
			{
				for(i=0;i<4;i++){
					memcpy((unsigned char*)&uii, buffer_29+1+(i*3), 3);
					SUMb+=uii;
					MSG_LOG("�����ܵ�4: %d\r\n", SUMb);

				}
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_29+13,2);
				SUMc+=temp10;
			}
			memcpy((unsigned char*)&uii, buffer_30+1+(ji*3), 3);
			SUMa=SUMb-uii;
		}
		if((SUMb<SUMa)||(SUMb>SUMc)||(SUMa>SUMc)) 
		{
			MSG_LOG("����������\r\n");
			return 100;	
		}

	}
	else{//����Ʊ
		MSG_LOG("Month Month!!!\r\n");

#ifdef BUS_HANDAN_		//���������λ�� ʮ����������0x0D,���Ե�ǰ��ҲҪ��ʮ��������
		i=(((date[0]&0xf0)>>4)*10)+(date[0]&0x0f);
		date[0] = i;
#endif
		if(date[0]==buffer_29[0]) 
		{
			block29=ST_OK;
			if(date[0]<buffer_30[0])
			{
				block2930=ST_OK;
			}
		}
		else if(date[0]==buffer_30[0]) 
		{
			block30=ST_OK;
			if(date[0]<buffer_29[0])
				block2930=ST_OK;
		}
		else 
		{
			return 100;	   //��Ʊ�黵�ˣ�ȥ��Ǯ								 
		}

		if(block29==ST_OK)
		{
			if(!buffer_29[dateSum]) 
			{
				return 100;
			}
			for(i=dateSum;i<13;i++)
				SUMb+=buffer_29[i];
			temp10 = 0;
			memcpy((unsigned char*)&temp10,buffer_29+13,2);
			SUMc=temp10 ;			//�������µ�����

			if(block2930==ST_OK)
			{
				for(i=1;i<13;i++)
					SUMb+=buffer_30[i];
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
				SUMc+=temp10;
			}
			SUMa=SUMb-buffer_29[dateSum];	
		}
		else if(block30==ST_OK)
		{
			if(!buffer_30[dateSum]) 
			{
				return 100;
			}
			for(i=dateSum;i<13;i++)
				SUMb+=buffer_30[i];
			
			temp10 = 0;
			memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
			SUMc=temp10 ;
			if(block2930==ST_OK)
			{
				for(i=1;i<13;i++)
					SUMb+=buffer_29[i];
				
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_29+13,2);
				SUMc+=temp10 ;
			}
			SUMa=SUMb-buffer_30[dateSum];
		}
		if((SUMb<SUMa)||(SUMb>SUMc)||(SUMa>SUMc)) 
		{
			return 100;	
		}
	}
	Pm=a_sum1;
	

	MSG_LOG("SUMa:");
	BCD_LOG((unsigned char*)&SUMa, 4, 1);//�������Ժ�ĵ���
	MSG_LOG("SUMb:");
	BCD_LOG((unsigned char*)&SUMb, 4, 1);//���¼��Ժ����
	MSG_LOG("SUMc:");
	BCD_LOG((unsigned char*)&SUMc, 4, 1);

	MSG_LOG("Month MMM 6 !!!\r\n");
	BCD_LOG((unsigned char*)&a_sum1, 4, 1);


	if(Pm<=SUMa)//��ʱ���Ѿ�����
		return 100;
	
	if(s_sum1 > 3){
		return 121;
	}

    if((Pm>=SUMa)&&(Pm<=SUMb))
	{	
		if(a_sum1>=s_sum1)
		{
			a_sum1-=s_sum1;
			dis_sum2=a_sum1-SUMa;
		}
		else 
		{
			return 100;
		}
	}
	else if(Pm>SUMb)//������ ���¼��Ժ�������������ǰ�ġ�
	{
		s_sum1=Pm-SUMb+s_sumTmp;
		if(a_sum1>=s_sum1)
		{
			a_sum1-=s_sum1;
			if(a_sum1 > SUMa)
				dis_sum2=a_sum1-SUMa;
			else
				dis_sum2=a_sum1;
		}
		else
		{
			return 100;
		}
	}
	else 
		return ST_ERROR;
	

	MSG_LOG("Month MMM s_sum1:");
	BCD_LOG((unsigned char*)&s_sum1, 4, 1);

	for(i=0;i<3;i++)
	{
		//clr_dog();
		memcpy(buffer, (unsigned char*)&s_sum1, 4);
		if(block3334==ST_ERROR)
		{
			if(MifareValueDebit(PICC_DECREMENT,cardMessage.dodMonthSector*4+2,buffer))
				break;
		}
		else
		{
			if(MifareValueDebit(PICC_DECREMENT,cardMessage.dodMonthSector*4+1,buffer))
				break;
		}
	}
	if(i >= 3)
		return ST_ERROR;

	for(i=0;i<1;i++)
	{
		if(block3334==ST_ERROR)
		{
			if(MifareTransfer(cardMessage.dodMonthSector*4+1))
				break;
		}
		else
		{
			if(MifareTransfer(cardMessage.dodMonthSector*4+2))
				break;
		}
	}
	
	if(block3334==ST_ERROR)
	{
		for(i=0;i<3;i++)
		{
			//clr_dog();
			if(MifareRead(cardMessage.dodMonthSector*4+1,buffer))
			{
				if(memcmp(buffer,(unsigned char*)&a_sum1,4)==0)
					break;
			}
		}
		if(i>=3)
		{
			CheckSno(ERROR_MONTH,OPER_TRANSFER_NO,buffer);
			return 4;					
		}
	}
	else
	{
		for(i=0;i<3;i++)
		{
			//clr_dog();
			if(MifareRead(cardMessage.dodMonthSector*4+2,buffer))
			{
				if(memcmp(buffer,(unsigned char*)&a_sum1,4)==0)
					break;
			}
		}
		if(i>=3)
		{
			CheckSno(ERROR_MONTH,OPER_TRANSFER_NO,buffer);
			return 4;					
		}

	}
month_step10:
	s_sum1 = s_sumTmp;
	ErrorOper(IS_EFFECT_MONTH);
	return 5;
}

unsigned char MonthResultManage(void)
{
	unsigned char rRecord[RECORD_LEN];
	unsigned char i=0, mothFlag;
	unsigned int ii;
//	unsigned char pabuf[50];

	//clr_dog();

	MSG_LOG("MonthResultManage Programer!\r\n");

	mothFlag = cardMessage.card_catalog;
	if(mothFlag >= 0x40)
		mothFlag -= 0x40;
	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
	{
		for(i=0;i<error_pointer;i++)
		{
			if(ErrorDetail[i].effFlag!=IS_EFFECT_MONTH) continue;
			else
			{
				if(memcmp(ErrorDetail[i].Cardno,&c_serial,4)==0)
				{
					goto kousum_continue;		
				}
			}
		}
		disp_no_swipe();
		return ST_OK;	
	}

	for(i=0;i<error_pointer;i++)
	{
		if(ErrorDetail[i].effFlag!=IS_EFFECT_MONEY) continue;
		else
		{
			if(memcmp(ErrorDetail[i].Cardno,&c_serial,4)==0)
			{
				if(MoneyResultManage(3)==ST_ERROR)
					return ST_ERROR;
				else
					return ST_OK;
			}
		}
		
	}

#ifdef _Counter_SWcard_times	//ˢ������������
	ii = checkSCtimes();
	if(ii == ST_ERROR){//������ˢ����
		cls();
		restore_flag = 1;
		MessageBox(0, "��������,�뷢��!");
		beep(8, 50, 50);	//û�к��ʵ�����
		return ST_OK;
	}
#endif

kousum_continue:
	i = cardMessage.card_catalog;
	if(i >= 0x40)
		i -= 0x40;
#ifndef BUS_BAOJI_
	if(NewPriceTab.rate[i]==203)//��ֹʹ����Ʊ
	{
		if(MoneyResultManage(3)==ST_ERROR)
			return ST_ERROR;
		else
			return ST_OK;
	}
#endif


	MSG_LOG("MonthResultManage Programer 3 !!!\r\n");
	BCD_LOG(&i, 1, 1);

	
	i = MonthManage();

	MSG_LOG("MonthResultManage Programer 2 !!!\r\n");
	BCD_LOG(&i, 1, 1);

	switch(i)
	{
	case 5:
		{
// 			if(PurchasOk(1)==ST_ERROR) 
// 			{
// #ifdef CARD_SOUND
// 				card_ser=c_serial;
// 				cardSound=0xaa;
// #endif
// //				SoundMessage(Audio_TRY_AGAIN);
// 				return ST_ERROR;
// 			}
#ifdef CARD_SOUND
			cardSound=0;
#endif
			SoundMessage(SOUND_DEAL);
			BuildRecorde(ID_REC_MON, rRecord);
			money_msg(ID_REC_MON, dis_sum2, s_sum1, 0);
			
			//if(cardMessage.card_catalog != CARD_COUPON)
#ifndef BUS_LAIZOUAT_
			save_h_month();
#endif
			WriteRecord(rRecord, 0);
			ComSumeFile.SumMti += s_sum1;
			ii = ((unsigned char*)&ComSumeFile.SumMti)-((unsigned char*)&ComSumeFile);
			sysfewrite(BIT_COMSUME_FILE+ii, 4 ,(unsigned char*)&ComSumeFile.SumMti);
			
#ifdef BUS_HBYS_//����ͳ��
			save_month_tongji(ID_REC_MON);
#endif
			return ST_OK;
		}
	case 120:
		error(ERR_MONTH_BAD,0);
		return ST_OK;
	case 121://�۴�������
		cls();
		MessageBox(1, "����Ʊ�۴���!");
		return ST_OK;
	case 100:
		if(MoneyResultManage(3)==ST_ERROR) return ST_ERROR;
		else
			return ST_OK;
	case 4:
#ifdef CARD_SOUND
		card_ser=c_serial;
		cardSound=0xaa;
#endif
//		SoundMessage(Audio_TRY_AGAIN);
	default:
		break;
	}
	return ST_ERROR;
}

unsigned char OperBlackCard(void)
{
	unsigned char rRecordDeal[70];
	unsigned char i;
	unsigned char buffer[40];
	unsigned char buffer1[16];
	//clr_dog();

	MSG_LOG("Ente OperBlackCard 1 !!\r\n");
	
	for(i=0;i<error_pointer;i++)
	{
		if(ErrorDetail[i].effFlag!=IS_EFFECT) continue;
		else
		{
			if(!memcmp(ErrorDetail[i].Cardno,(unsigned char *)&c_serial,4))
			{
				switch(ErrorDetail[i].ErrorStep)
				{
				case OPER_BLACK:	
					memcpy(buffer,ErrorDetail[i].ToChange,16);
					goto error_black;
				default:
					break;
				}
			}
		}
	}
	
	if(cardMessage.publicMessageSector >= 16)
	{
		cardSound=0;
//		WriteRecord((unsigned char*)&rRecordDeal, 0);
		ErrorOper(IS_EFFECT);
		return ST_OK;
	}

	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
	{
		return ST_ERROR; 
	}
	
	//��������Ϣ
	if(MifareRead(cardMessage.publicMessageSector*4,buffer)==0)
	{
		return ST_ERROR;
	}

	buffer[6]=0x04;
	buffer[15]=MiBlockInvalid(0,15,buffer);
error_black:
	memcpy(buffer1,buffer,16);
	
	if(MifareWrite(cardMessage.publicMessageSector*4,buffer)==0) return ST_ERROR;
	for(i=0;i<16;i++)
		buffer[i]=~buffer1[i];
	if(MifareWrite(cardMessage.publicMessageSector*4+1,buffer)==0) return  ST_ERROR;
	
	BuildRecorde(ID_REC_BLKLIST, rRecordDeal);

	MSG_LOG("�û���¼1\r\n");
	BCD_LOG((unsigned char*)&rRecordDeal,RECORD_LEN,1);			

#ifdef CARD_SOUND
	cardSound=0;
#endif
	WriteRecord((unsigned char*)&rRecordDeal, 0);
	ErrorOper(IS_EFFECT);
	return ST_OK;
}

void DisRetry(void)
{
#ifdef BUS_PINGXIANG_
	return;
#else
	static unsigned int ucFrePresentC;
	unsigned int tt;

	tt = 20;
#ifndef NO_MOBILE_24G_
	if(g24PurError == 0xaa){// ��ֵҪ��һЩ
		tt = 80;
	}
#elif defined _CPU_24G_
	if(g24PurError == 0xaa){// ��ֵҪ��һЩ
		tt = 1160;
	}
#endif

	ucFrePresentC++;
	if(ucFrePresentC==tt)
	{
		cls();
		beep(3,100,50);
		display(3,2,PLS_Swipe_again,DIS_CENTER|DIS_CONVERT);
	}
	else if(ucFrePresentC>(tt*2))
	{
		cls();
		ucFrePresentC=0;
		beep(3,100,50);
		display(3,2,PLS_Swipe_again,DIS_CENTER|DIS_CONVERT);
	}
	BlightOpen();
#endif
}
void DisRetry2(void)
{
#ifdef BUS_PINGXIANG_
	return;
#else
	static unsigned int ucFrePresentC;
	
	
#if defined _debug_ || defined _debug_trans
	char disbuff[16];
#endif
	
	ucFrePresentC++;
	if(ucFrePresentC==120)
	{
		cls();
		beep(3, 100, 200);
		display(3,2,PLS_Swipe_again,DIS_CENTER|DIS_CONVERT);
#if defined _debug_ || defined _debug_trans
		memset(disbuff,0,10);
		sprintf(disbuff,"%02X",cardSound);
		printf("%s%02X\r\n",PLS_Swipe_again,cardSound);
		display(5,0,disbuff,0);
		audio(Audio_TRY_AGAIN);		//����ˢ
		
#else
		audio(Audio_TRY_AGAIN);		//����ˢ
		
#endif
	}
	else if(ucFrePresentC>250)
	{
		cls();
		beep(3, 100, 200);
		ucFrePresentC=0;
		display(3,2,PLS_Swipe_again,DIS_CENTER|DIS_CONVERT);
	}
	BlightOpen();
#endif
}

unsigned char FreeConsume(void)
{
	unsigned char buffer[RECORD_LEN];
	unsigned char mothFlag;
	unsigned int ii;
	
#ifdef CARD_SOUND
	cardSound = 0;
#endif
	
	if(F_FLAG_VARY.stop_flag)
		return ST_ERROR;

	restore_flag = 1;
	mothFlag = cardMessage.card_catalog;
	if(mothFlag >= 0x40)
		mothFlag -= 0x40;
// 	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
// 	{
// 		disp_no_swipe();
// 		return ST_OK;
// 	}

	s_sum1 = 0;
	dis_sum2 = 0;
	a_sum1 = 0;

	money_msg(ID_REC_MON, 0, 0, 0);
#ifdef _AREA_ENG_
	SoundMessage(SOUND_DEAL);
#else
	if(cardMessage.card_catalog == CARD_DRIVER_BUS)
		audio(Audio_STUFF);
	else
		SoundMessage(SOUND_DEAL);
#endif
#ifdef BUS_LICHUANG_	//�质˾����������ˢ
	if(cardMessage.card_catalog != CARD_DRIVER_BUS)
#endif
		save_h_month();	//"�¿����Żݿ�,��������ʹ��!"

	BuildRecorde(ID_REC_MON, buffer);
	WriteRecord(buffer, 0);
	ComSumeFile.SumMti += s_sum1;
	ii = ((unsigned char*)&ComSumeFile.SumMti)-((unsigned char*)&ComSumeFile);
	sysfewrite(BIT_COMSUME_FILE+ii, 4 ,(unsigned char*)&ComSumeFile.SumMti);
	
#ifdef BUS_HBYS_//����ͳ��
	save_month_tongji(ID_REC_MON);
#endif
	end_card();
	return ST_OK;
}
unsigned char checkLineNo(void)
{
	unsigned char dat;
	unsigned char i;

	if(memcmp(pFistVary.LineNo, "\x00\x00", 2) ==0)
		return ST_ERROR;
	for(i=0;i<2;i++){
		dat = ((pFistVary.LineNo[i] & 0xF0)>>4);
		if(dat > 9)
			return ST_ERROR;
		dat = (pFistVary.LineNo[i]&0x0F);
		if(dat > 9)
			return ST_ERROR;
	}
	return ST_OK;
}
//0 ��ȷ ��������
unsigned char checkBusInfo(void)
{
	stCpuInfo cpuinfo;
	unsigned char flag=0;
//#ifndef KEYBOARD
//	unsigned char ret=0;
//	unsigned char i;
//	unsigned int mmoney;
//#endif
	//����豸��
	if((memcmp(pFistVary.DeviceNo, "00000000", 8)==0)||\
		(ValidDevNo(pFistVary.DeviceNo) == 1)){
		sysferead(BIT_DEVICE_NO,9,pFistVary.DeviceNo);
		if((memcmp(pFistVary.DeviceNo, "00000000", 8)==0)||(ValidDevNo(pFistVary.DeviceNo) == 1))
			return 0xf1;
		else
			flag = 1;
	}
	//�����·��
	if(checkLineNo() == ST_ERROR){
		getCpuInfo(&cpuinfo);
		if(checkLineNo() == ST_ERROR)
			return 0xf2;
		else
			flag = 1;
	}
// 
// #ifndef KEYBOARD
// 	//���Ʊ����Ϣ,����ֻ��鵽6�ſ�
// 	ret = 0;
// 	
// 	memcpy((unsigned char*)&mmoney, NewPriceTab.busPrice, 4);
// #ifdef BUS_PRO_JSB
// 	for(i=0;i<7;i++)
// #else
// 	for(i=1;i<4;i++)
// #endif
// 	{
// 		if((mmoney > 10000) || (mmoney < 10)){
// 			ret = 1;
// 			break;
// 		}
// 		if(NewPriceTab.rate[i] == 0){
// 			ret = 1;
// 			break;
// 		}
// 	}
// 	if(ret == 1){//�ۿ��ʴ�
// 		sysferead(BIT_PRICETAB,sizeof(stPricetable),(unsigned char*)&NewPriceTab);
// 		return 1;
// 	}
// #endif

	if(flag == 1){
		savesysbupara();
		return 2;
	}
	return 0;
}

	
#ifdef BUS_SYBAOSHI_//ֻ���ֻ����ţ�û��M1������
#ifdef DX24G_FAR
unsigned int CScardDly;
#define CSDLY_TIME 40
#endif

void BuildRecorde_SYBAOSHI(unsigned char delType, unsigned char *recBuf)
{
	DEALRECODE *rRecordDeal;
	unsigned int temp;
	
	sysferead(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN,(unsigned char*)&POINT_MESSAGE);
	
	rRecordDeal = (DEALRECODE *)recBuf;
	
	memcpy(rRecordDeal->rMoneyDealSum, CARD_PUBLIC.MoneyDealSum, 2);
	memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&a_sum1, 4);
	temp =POINT_MESSAGE.MoneyDealPointer;
	POINT_MESSAGE.MoneyDealPointer++;
	if(POINT_MESSAGE.MoneyDealPointer > 0x3B9AC9FF)
		POINT_MESSAGE.MoneyDealPointer = 0;
	temp = POINT_MESSAGE.MoneyDealPointer;
	memcpy(rRecordDeal->rDealMoney, (unsigned char *)&s_sum1, 3);
	CountPoint(0);
	//	memcpy(rRecordDeal->rTradCode, cardMessage.TradeCode, 2);
	rRecordDeal->rRemain = 0xfe;
	rRecordDeal->rPublishNumb_a = 0;
	memcpy(rRecordDeal->rCardDealNumb, (unsigned char*)&temp, 4);
	rRecordDeal->rCardType = cardMessage.card_catalog;
	Get_SerialNumF4(rRecordDeal->rResever1);
//	memcpy(rRecordDeal->rResever1, "\xea\xef\xea\xef", 4);
	memcpy(rRecordDeal->rDealTime, (unsigned char*)&SysTime, 7);
	rRecordDeal->rDealType = delType;
	Ascii2BCD(pFistVary.DeviceNo, rRecordDeal->rDeviceNo, 8);
	memcpy(rRecordDeal->rDriverNo, DriveCardNo, 4);
	memcpy(rRecordDeal->rstuffernum, StufferNO, 4);
	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 2);
	memcpy(rRecordDeal->oldLineNO, pFistVary.LineNoOld, 2);
	memcpy(rRecordDeal->rPublishNumb, cardMessage.PublishBicker, 4);
	rRecordDeal->rCardCand = gMCardCand;

	memcpy(rRecordDeal->rBussinessNo, "\x01\x68", 2);
	memset(rRecordDeal->ruserTimes, 0xff, 2);
	memset(rRecordDeal->rResever, 0xff, sizeof(rRecordDeal->rResever));
	CountRecMac((unsigned char*)&rRecordDeal, 60, rRecordDeal->rTAC);

	MSG_LOG("BLD REC:");
	BCD_LOG(recBuf, 64, 1);
}

void G24DXBreaKCard(void)//�Ͽ�����:80 05 40 04 00 00 00
{
	unsigned char buffer[32];
	memcpy(buffer, "\x80\x05\x40\x04\x00\x00\x00", 7);
//	memcpy(buffer, "\x80\x05\x90\xB0\x00\x00\x00", 7);//�ر�RF
// 	MSG_LOG("����:");
// 	BCD_LOG(buffer, 7, 1);
	gDX24ComFlag = 0xaa;
	gDX24ComRindex = 0;
	com_snd(COM5, 7, buffer);
}

void G24DX_handler(unsigned char value)
{
	unsigned char ii;

//	BCD_LOG(&value, 1, 0);
//	com_snd(COM1, 1, &value);
	
	ii = value;
#ifdef DX24G_FAR
	if(gDX24GProFlag != 0x55){
		switch (gDX24ComRindex)
		{
		case 0:
			if (ii != 0xB0)
				return;
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			break;
		case 1:
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			break;
		case 2:
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			break;
		default:
			if(gDX24ComRindex >= 20){
				gDX24ComRindex = 0;
				gDX24ComFlag = 0xee;//���մ���
				break;
			}
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			if(gDX24ComRindex >= (gDX24ComRBuf[1]+2)){
				gDX24ComFlag = 0x55;//�������
			}
		}
	}
#else
	if(gDX24ComFlag == 0xaa){//��ͷ��Ϣ
		switch (gDX24ComRindex)
		{
		case 0:
			if (ii != 0x90)
				return;
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			break;
		case 1:
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			break;
		case 2:
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			break;
		default:
			if(gDX24ComRindex >= 512){
				gDX24ComRindex = 0;
				gDX24ComFlag = 0xee;//���մ���
				break;
			}
			gDX24ComRBuf[gDX24ComRindex++] = ii;
			if(gDX24ComRindex >= (gDX24ComRBuf[1]+2)){
				gDX24ComFlag = 0x55;//�������
			}
		}
		return;
	}
#endif
}

//��24G������ѭ��
unsigned char DXprofile(void)
{
	unsigned char buffer[100];
	unsigned short ss=0;

	if(gDX24GProFlag == 0){//ѯ��
#ifndef DX24G_FAR
		gDX24ComFlag = 0xaa;
		gDX24ComRindex = 0;
		memcpy(buffer, "\x80\x05\x40\x0A\x00\x00\x00", 7);
// 		MSG_LOG("����:");
// 		BCD_LOG(buffer, 7, 1);
		com_snd(COM5, 7, buffer);
#endif
		gDX24GProFlag = 1;
		return 0;
	}
	else{
		if(gDX24ComFlag == 0x55){
//			gDX24GProFlag = 0;
			gDX24ComFlag = 0; //902800606782000000000000000000000000000000000000000000000000000000000000000000009000

// 			MSG_LOG("ѯ������:");	//ѯ������:B009000863032800606782
// 			BCD_LOG(gDX24ComRBuf, gDX24ComRindex,1);

			memcpy((unsigned char*)&ss, gDX24ComRBuf+3, 2);//���ص�״̬��
#ifdef DX24G_FAR	//B009000863032800606782
			if((gDX24ComRBuf[1] == 9)&&(gDX24ComRindex >= 9)) //��B0��������Ϣ��09�����ݳ��ȣ�00�����ӳɹ���־��99 90 01 00 00 00 08 59������IDֵ��
#else
			if((memcmp(gDX24ComRBuf+gDX24ComRindex-2, "\x90\x00", 2) == 0)&&(gDX24ComRindex >= 8))
#endif
			{//�ɹ�

				MSG_LOG("cardNOOO:");
				BCD_LOG(gDX24ComRBuf, gDX24ComRindex,1);

#ifdef DX24G_FAR	//8�ֽ�ID�ţ�ֻȡ��4�ֽ�
				memcpy((unsigned char *)&c_serial, gDX24ComRBuf+7, 4);
				memcpy(cardMessage.PublishBicker, gDX24ComRBuf+7, 4);
#else
				memcpy((unsigned char *)&c_serial, gDX24ComRBuf+2, 4);
				memcpy(cardMessage.PublishBicker, gDX24ComRBuf+2, 4);
#endif
// 				MSG_LOG("c_serial:");
// 				BCD_LOG((unsigned char*)&c_serial, 4,1);

				if(month_decide()){
#ifdef DX24G_FAR
					if(CScardDly > CSDLY_TIME){
						CScardDly = CSDLY_TIME;
					}
					if(CScardDly != 0){
						CScardDly--;
						return 1;
					}
					CScardDly = CSDLY_TIME;
#endif
					restore_flag = 0;
#ifdef NO_M1_Purse
					g24GDisFlash = 100;
#else
					g24GDisFlash = 2;
#endif
					beep(3, 20, 50);
					cls();
					display(0, 0, "��ʾ:", 0);
					display(2, 0, "�Ѿ�ˢ��!", DIS_CENTER);
					memset(buffer, 0, 20);
					memcpy(buffer, "����:", 5);
					BCD2Ascii(cardMessage.PublishBicker, buffer+5, 4);
					display(4, 0, (char*)buffer, DIS_CENTER);
#ifndef DX24G_FAR
					G24DXBreaKCard();
					delayxs(1);
#endif
					return 1;
				}
#ifdef DX24G_FAR
				CScardDly = 0;
#endif
				gMCardCand = CARDSTYLE_24CT;
				GetDateTime();
#ifdef BAOSHI_LM
				a_sum1 = 0x55555555;
				s_sum1 = 0x55555555;
#else
				a_sum1 = 0xAAAAAAAA;
				s_sum1 = 0xAAAAAAAA;
#endif
				BuildRecorde_SYBAOSHI(ID_REC_DXBAOSI, buffer);
				WriteRecord(buffer, 0);
				audio(Audio_SuaKaOK);
				save_h_month();
				cls();
				display(0,0,"��ʾ:",0);
				display(2,0,"ˢ���ɹ�",DIS_CENTER);		  
				memset(buffer, 0, 20);
				memcpy(buffer, "����:", 5);
				BCD2Ascii(cardMessage.PublishBicker, buffer+5, 4);
				display(4, 0, (char*)buffer, DIS_CENTER);
#ifdef NO_M1_Purse
				g24GDisFlash = 100;
#else
				g24GDisFlash = 2;
#endif
				SetTcpPacketTTLTime(0);
				G24DXBreaKCard();
				delayxs(1);
				return 1;
			}
			gDX24ComRindex = 0;
		}
		else{
			gDX24GProFlag++;
#ifdef NO_M1_Purse
			if(gDX24GProFlag > 0x9FF)
#else
			if(gDX24GProFlag > 0x10)
#endif
			{
				gDX24GProFlag = 0;
 		//MSG_LOG("DX Out:");
			}
		}
	}
	return 0;
}
#endif

#ifdef Card_typeB_enable_
unsigned char profileB(unsigned char mode)
{
	unsigned char rxbuf[100];
	unsigned char ret;

	memset(rxbuf, 0, 50);
	ret = GetTypeBUID(rxbuf);
	//		ret = RC531ReqB(rxbuf);
	if(ret==0){
		return ST_ERROR;
	}
	memcpy((unsigned char*)&c_serial, rxbuf, 4);
	
	MSG_LOG("OK:");
	BCD_LOG(rxbuf, ret, 1);

	return ST_OK;
}

void disTypBprofile(void)
{
	char disbuf[32];

	cls();
	display(0, 0, " ˢ���ɹ� ", DIS_CENTER|DIS_CONVERT);
	memset(disbuf, 0, 32);
	strcpy(disbuf, "���:");
	BCD2Ascii((unsigned char*)&c_serial, (unsigned char*)disbuf+5, 4);
	display(3, 1, disbuf, 0);
	display(5, 1, "����:TypeB", 0);
	audio(Audio_SuaKaOK);
	led_on(LED_GREEN);
	delayxms(500);
	g24GDisFlash = 2;
	restore_flag = 0;
}
#endif

#ifdef Card_snd1111
extern void snd111cmd(void);
#endif
void main_card(void)
{
	unsigned char cASK=0;
	unsigned char resPonse=0;
	Card_Area=0;
#ifdef _TEST_YANPIAO_
	unsigned char pbuff[70];
#endif

#ifndef NEED_AIR_BUS_CARD
	F_FLAG_VARY.PlusSta = 1;//���ӷ�����
#endif

#ifdef NO_PlusSta_
	F_FLAG_VARY.PlusSta = 0;//û�и��ӷѹ���
#endif
#ifdef _TTSSound_enable_
#ifdef _TTS_CLOSE_M1_
	if(xfSounding == 0x55){//���ڱ�����ʱѯ��������̫�����ﲻȥѯ����
//		MSG_LOG("xfsonjdnd = 0x55\r\n");
		display(2, 1, "���ڱ�վ,�Ե�.", 0);
		delayxms(200);
		restore_flag = 3;
		face_DisTime = 0;
		return;
	}
#endif
#endif

#ifdef BUS_SYBAOSHI_//ֻ���ֻ����ţ�����M1������
	DXprofile();
#ifdef NO_M1_Purse
	return;
#endif
#endif
	
#ifndef NO_MOBILE_24G_
	if(g24PurError == 0xaa){//����ֻ�����������ˢ,����ˢ����IC��
#ifdef MOBILE_XIANYANG_
		DisRetry2();
		//�����ƶ��ֻ�����SAM-PASS��,ֻ���ڲ���,��û��ʹ��.
		resPonse = profile(0);
		if(resPonse == 0x20) //���M1��������
		{
			G24MainPASSCard();
		}
#endif
		G24MainCard();
		return;
	}
#elif defined _CPU_24G_
	if(g24PurError == 0xaa){//����ֻ�����������ˢ,����ˢ����IC��
		CPU24GMainCard();
		return;
	}
#endif

	if((cardSound==0xaa)||(cardSound==0xbb)||(cardSound==0xcc)){//aaǮ������ bb��Ʊ���� 
#ifdef MOBILE_SHIYAN_
		DisRetry();
#else
		DisRetry2();
#endif
	}

//	if((gmissflag == MISS_M_DPGRAM)||(gmissflag == MISS_M_DPGRAMOK)||(gmissflag == MISS_G_DPRO)){
// 	if((gmissflag == MISS_M_DPGRAM)||(gmissflag == MISS_M_DPGRAMOK)){
// 		profile(0);//ȥ���������ʱ����ֹͣ��timer0��
// 		return;
// 	}
//	BCD_LOG((unsigned char*)&resPonse, 1, 0);
	
	if(RC531offtime > 3){
		PcdRfReset(1);	//��ͨѯ����3�ο���һ�Ρ���
		RC531offtime = 0;
	}
	RC531offtime++;
	
#ifdef Card_typeB_enable_
	if(gCartTypeA0B1 == 1){
		resPonse = profileB(0);
		gCartTypeA0B1 = 0;
		if(resPonse == ST_OK)
			disTypBprofile();
		return;
	}
	else{
		resPonse = profile(0);
		gCartTypeA0B1 = 1;
	}
#else
	resPonse = profile(0);
#endif
	
// 	if(sound_dly == 0){
// 	}
// 	else{
//		debugstring("-");
//		delayxms(10);
//	}

 ///	printf(" resPonse:%d\r\n", resPonse);
// 	BCD_LOG((unsigned char*)&resPonse, 1, 1);
	//���ʱ�� 2016��5��9��11:48:54  @zzc
	if(resPonse != ST_ERROR){
		if(check_time()==ST_ERROR)
		{
			delayxms(200);
			return;
		}
	}


	if (resPonse == ST_ERROR) //���M1��������
	{
 #ifdef _CPU_24G_
		CPU24GMainCard();
 #endif
		return;
	}
	else if(resPonse == 0x20){//CPU 0x20,0x28
		cASK = resPonse;
#ifdef BUS_CPU_
		CPUMainCard();//CPU��,Ӧ�ò���2.4G��
		return;
#endif
//#ifdef qPBOC_BUS
//		cpuPBOCmain();
//		return;
//#endif
#ifdef MOBILE_XIANYANG_
//�����ƶ��ֻ�����SAM-PASS��,ֻ���ڲ���,��û��ʹ��.
		G24MainPASSCard();
		return;
#endif
	}
#ifdef _TEST_YANPIAO_
	///////////////////////////ƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱƱ
	GetDateTime();
	memcpy(cardMessage.PublishBicker, (unsigned char*)&c_serial, 4);
	cls();
	display(0,0,"ˢ���ɹ�",DIS_CENTER|DIS_CONVERT);
	display(3,0,"��ӭ����", DIS_CENTER);
	restore_flag = 3;
#ifdef AUDIO_SOUND_OLD
	audio_old(Audio_DONG);
#else
	audio(Audio_DONG);
#endif
	BuildRecorde(ID_REC_TOLL, pbuff);
	WriteRecord(pbuff, 0);
	end_card();
	
	return;
#endif


	MSG_LOG("--New Card:");
	BCD_LOG((unsigned char*)&c_serial, 4, 0);
	MSG_LOG(" resPonse:");
	BCD_LOG((unsigned char*)&resPonse, 1, 0);
	MSG_LOG("--\r\n");

	BlightOpen();


#ifdef BUS_MZT
	second_money_flag = 0;
#endif
	
	
	if (cardSound != 0)
	{
		if (c_serial != card_ser)
		{
#if defined _debug_ || defined _debug_trans
			printf("now:%08X,befor:%08X\r\n",c_serial,card_ser);
#endif
			return ;
		}
	}

	resPonse=Card_typeProcess();	 //120ms

	MSG_LOG("resPonse=");
	BCD_LOG(&resPonse, 1, 1);
	MSG_LOG("  �ۿ�:");
#ifdef BUS_PRO_JSB
	BCD_LOG((unsigned char*)&NewPriceTab.rate[cardMessage.card_catalog], 1, 1);
#else
	BCD_LOG((unsigned char*)&NewPriceTab.rate[cardMessage.card_catalog-0x40], 1, 1);
#endif


#ifdef _YiDa_Card_
	if(resPonse == 0xA3)
	{
		YiDaMainCard();
		return;
	}
#endif
	
	if((resPonse == 0xA2)&&(cASK == 0x20)){//��֤��������CPU��
#ifdef BUS_CPU_
		CPUMainCard();//CPU��,Ӧ�ò���2.4G��
#endif
#ifdef qPBOC_BUS
		cpuPBOCmain();
#endif
		return;
	}
#ifdef FengDuan_BU_
	if((resPonse == MONTH_CARD)||(resPonse == CARD_MONEY)||(resPonse == CARD_YEAR)||(resPonse == CARD_STUFF_BUS)){
		FengDuan_BUS_card();
		return;
	}
#endif	//#ifdef FengDuan_BU_
	if((cal_space()<5)&&(cardMessage.card_catalog!=CARD_DATA_GATHER))
	{
		cls();
		display(0, 0, Card_Worring, 0);
		display(2,0,EEisFull,DIS_CENTER);
		end_card();
		restore_flag = 3;
		return;
	}

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
	case CARD_CHAGEERR:
		restore_flag = 1;
		led_on(LED_RED);
		MessageBox(1, Card_Carderror);
	case ST_OK:
		end_card();
		break;
	case CARD_PUBLIC_BAD:		//����Ҫ�ָ�
		if(RepairPublicSector() == ST_ERROR)
			error(ERR_CARD_PUBLIC, 0);
		restore_flag=3;
		break;
	case CARD_NO_TIME:		
		if(F_FLAG_VARY.stop_flag) return;
		cls();
		display(0,0,Card_Worring,0);
		display(2,1,Expired_card,0);
#ifdef _AREA_ENG_
		beep(3, 100, 50);
#else
#ifdef BUS_YICHUN
		audio(Audio_YWJYu);//������
		beep(3, 100, 50);
#elif defined BUS_BAOJI_
		audio(Audio_YWJYu);//������
#else
#ifdef AUDIO_SOUND_OLD
		audio_old(Audio_FLING_MONEY);	//��Ͷ��
#else
		audio(Audio_FLING_MONEY);	//��Ͷ��
#endif
#endif
#endif
		restore_flag=3;
		end_card();
		break;
	case CARD_BLACK_PRO:
// #ifndef BUS_EZOU_	//���ݲ�д���������־
// 		OperBlackCard();
// #endif


		
		cls();
		display(2,0,Please_Coin,DIS_CENTER);
#ifdef _AREA_ENG_
		display(0,0,Card_Worring,1);
		beep(3, 100, 50);
#else
#ifdef BUS_HANDAN_
		display(0,0,"����:�ÿ��ѹ�ʧ!",0);
		SoundMessage(INVALID_CARD);
#elif defined Audio_GuaShi
		display(0,0,"����:��������!",0);
		audio(Audio_GuaShi);
#else
		display(0,0,"����:��������!",0);
		SoundMessage(INVALID_CARD);
#endif
#endif
		restore_flag=3;
		end_card();
		break;
	case CARD_forbid:
		MessageBox(1, Card_forbidCard);
		led_on(LED_RED);
		restore_flag=3;
		end_card();
		break;
	case CARD_BLACK:
		cls();
		display(2,0,Please_Coin,DIS_CENTER);
#ifdef _AREA_ENG_
		display(0,0,Card_Worring,1);
		beep(3, 100, 50);
#else
#ifdef BUS_HANDAN_
		display(0,0,"����:�ÿ��ѹ�ʧ!",0);
		SoundMessage(INVALID_CARD);
#elif defined Audio_GuaShi
		display(0,0,"����:��������!",0);
		audio(Audio_GuaShi);
#else
		display(0,0,"����:��������!",0);
		SoundMessage(INVALID_CARD);
#endif
#endif
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
	case CARD_HUIFU:
		error(ERR_CARD_PUBLIC,0);
		restore_flag=3;
		end_card();
		break;
	case CARD_YEAR:
		if(YearResultManage(0)==ST_ERROR) 
			break;   //��ƱǮ�������
		restore_flag=1;
		end_card();
		break;
	case CARD_MONEY:
		if(checkBusInfo() > 10)
		{
			cls();
			display(0,0,Card_Worring,1);
#ifdef _AREA_ENG_
			display(2,0,"pos info error!",DIS_CENTER);
#else
			display(3,2,"�豸��Ϣ����",0);
#endif
		}
		else{
			if(MoneyResultManage(0)==ST_ERROR)
				break;   //Ǯ����
		}
		end_card();

		g24GDisFlash = 2;
		restore_flag = 0;
#ifndef NO_GJ_SERVISE
		SetTcpPacketTTLTime(RECORD_TTL);
#endif
#ifndef NO_MOBILE_SERVISE
		gM_Senddll = 0;//RECORD24G_TTL;
#endif
		
#ifdef Card_snd1111
		display(6, 0, "ͨѶ��...", DIS_CENTER);
		for(resPonse=0; resPonse<10; resPonse++){
			snd111cmd();
			delayxs(1);
		}
#endif
		break;
	case MONTH_CARD:
		if(checkBusInfo() > 10)
		{
			cls();
			if(gchn_eng == 'E'){
				display(0,0,"warring:",1);
				display(2,0,"pos info error!",DIS_CENTER);
			}
			else
			{
				display(0,0,"��ʾ:",0);
				display(3,2,"�豸��Ϣ����",0);
			}
		}
		else{
			if(MonthResultManage()==ST_ERROR)
				break;	//��Ʊ�����´�
		}
		end_card();
		g24GDisFlash = 2;
		restore_flag=0;

#ifndef NO_GJ_SERVISE
		SetTcpPacketTTLTime(RECORD_TTL);
#endif
#ifndef NO_MOBILE_SERVISE
		gM_Senddll = 0;//RECORD24G_TTL;
#endif
#ifdef Card_snd1111
		for(resPonse=0; resPonse<10; resPonse++){
			snd111cmd();
			delayxs(1);
		}
#endif		
		break;

	case CARD_FREE:
		FreeConsume();
		end_card();
		break;

	case 150:
		restore_flag=3;
#if 1
		end_card();
#endif
		break;
	case CONTROL_CARD:
		ControlMagane();
		restore_flag=3;
#ifndef NO_GJ_SERVISE
		SetTcpPacketTTLTime(RECORD_TTL);
#endif
		break;
	default:
		break;
	}
	return;
}

#ifdef _card_guard_
void dis_kq_rec(unsigned char *ret, unsigned int Index)
{
	DEALRECODE *historyCheck;// = (RECORD_CONTROL*)&rRecordDeal;
	char buffer[40];
	unsigned int temp;

	cls();
	historyCheck = (DEALRECODE *)ret;

	sprintf((char*)buffer, "%d ", Index+1);
	miniDispstr(7, (21-strlen((char*)buffer)), (char*)buffer, 0);
	memset(buffer,0,20);
	temp=0;
	memcpy((unsigned char*)&temp, historyCheck->rPublishNumb, 4);
	display(0,0,"����:",0);
	sprintf((char*)buffer,"%05u", temp);
	display(3,0,(char*)buffer,DIS_CENTER);
	dis_time_mini(6, (stcomtime*)historyCheck->rDealTime);
}
#endif

void testXF(void)
{
//���ڹ���
#ifdef _card_guard_
	unsigned int DisDD=0;
	unsigned int temp;
	unsigned char rb[70];
	unsigned char ret,kk=0;
	int headp, curp;

	restore_flag = 1;

	while(1){
		clr_wdt();
		if(restore_flag) //DriveCardNo DeviceNo pFistVary.LineNo
		{
			led_off(LED_GREEN|LED_RED|LED_BLUE|LED_YELLOW);
			
			cls();
			display(0,0," ��������Ƽ� ",DIS_CONVERT|DIS_CENTER);
			display(3,0," �� ˢ �� ",DIS_CENTER);

			restore_flag = 0;
		}
		
		ret = getkey(1);
		if(ret == KEY_ENT){
			ShowQu();
			restore_flag = 1;
		}
		if((ret == KEY_F1)&&(kk == 0))
			kk = 1;
		else if((ret == KEY_UP)&&(kk == 1)){
			USART2mode = _TEST_Irda;
			get_data(0);
#ifdef WIFI_TLH_
			GPRS_INIT(115200);
#else
			GPRS_INIT(9600);
#endif
			USART2mode = _TEST_GPRS;
			restore_flag = 1;
		}
		else if(ret != 0)
			kk = 0;

		if(DisDD++ >= 30){
			DisDD = 0;
			if(GetDateTime()==ST_OK)
			{
				dis_time_mini(7, (stcomtime*)&SysTime);//��ʾʱ��
			}
			curp = Get_Record_point((unsigned char*)&headp, 0);
			temp = curp-headp;
			temp /= 64;
			sprintf((char*)rb, "%u ", temp);
			miniDispstr(6,1,(char*)rb, 0);
		}
		face_DisTime ++;
		if (face_DisTime>27000)
		{
			//		delayxms(10);
			disinit();
			InitRC531();//add V3.19 30/08/2010
			face_DisTime =0;
			restore_flag = 3;
		}
		if(profile(0) == ST_OK){//��ˢ��
//			MSG_LOG("ѭ����");
//			BCD_LOG((unsigned char*)&c_serial,4,1);

//			memcpy(rb, "\x35\x34\x11\x22\x33\x44", 6);//0����Ĭ����Կ
			memcpy(rb, "\x35\x34\x31\x31\x32\x32", 6);//0����Ĭ����Կ
			if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,rb,52)!=0)
			{
//				MSG_LOG("��֤��");
//				BCD_LOG(rb,6,1);

// 				MifareRead(53,rb);
// 				BCD_LOG(rb,16,1);
// 				MifareRead(54,rb);
// 				BCD_LOG(rb,16,1);

				if (MifareRead(52,rb) !=0) 
				{
// 					MSG_LOG("13������Ϣ��\r\n");
// 					BCD_LOG(rb,16,1);
					
					temp = str2int(rb, 5);
					cardMessage.card_catalog = 0;
					memcpy(cardMessage.PublishBicker, (unsigned char *)&temp, 4);
					BuildRecorde(85, rb);
					WriteRecord(rb, 0);
					
					cls();
#ifdef AUDIO_SOUND_OLD
					audio_old(Audio_DONG);
#else
					audio(Audio_DONG);
#endif
					display(0,0,"ˢ���ɹ�:", 0);
					sprintf((char*)rb, "%05d", temp);
					display(3,0,(char*)rb, DIS_CENTER);
					dis_time_mini(7, (stcomtime*)&SysTime);//��ʾʱ��
					beep(1,100,0);
					end_card();
					restore_flag = 1;
				}
			}
		}
	}
#endif
#ifdef _TTSSound_enable_
	char strbuf[50];
//	int i;
	memset(strbuf, 0, 50);
//	GetInSound('B', 0, strbuf);
//	voice_switch(voice2busIn);//(voice2board);//
//	if(strlen(strbuf) == 0)

	MSG_LOG("SoundOut_Test\r\n");

// 	strbuf[0] = 0xFD;
// 	strbuf[1] = 0;
// 	strbuf[2] = 30;
// 	strbuf[3] = 1;
// 	strbuf[4] = 0;
// 	strcpy((char*)strbuf+5, "��ӭʹ�������������ܹ���ϵͳ");
#ifdef BUS_NanjingDZ_
	strcpy((char*)strbuf, "��λ�˿�,��ӭʹ�� ��ˮ���� ����������.");
#else
	strcpy((char*)strbuf, "��ӭʹ�������������ܹ���ϵͳ.");
#endif
	SoundFreeManage(strbuf);
#else
	XF_Reset_LOW();
#endif
	//�����ػ�����BU��Ϣ
#ifdef OTHER_GPRS
	gprs_other_sndbuinfo();
#endif

	MSG_LOG("FE_END_ADDR:%d", BIT_FE_END_ADDR);

	return;
}

void PosStateInit(void)
{
	unsigned int itemp;
#ifdef BUS_SHAODONG_
	SD_DEALRECODE rRecordDeal;
	unsigned char temp_1[7],temp_2[7],temp_3[7];
	int heap, curp;
#endif
	memcpy((unsigned char*)&itemp, NewPriceTab.busPrice, 4);
	if(itemp > 10000){
		F_FLAG_VARY.stop_flag = 1;//Ʊ�۲���ȷ��ֹͣ�շ�
	}
	
#ifdef BUS_SHAODONG_
	curp = Get_Record_point((unsigned char*)&heap, 0);
	memset(rRecordDeal.rDeviceNo, '0', 64);
	memcpy(rRecordDeal.rDeviceNo, pFistVary.DeviceNo, 8);
	rRecordDeal.posHardW_flag = 'T';	//���ػ�Ӳ����ʶ��7S,7X,ST
	rRecordDeal.rDealType = 50;			//���������ڴ˶���Ϊ50
	Ascii2BCD(gBusVerInfo.busBLKVer, temp_1, 2);
	memcpy(rRecordDeal.busBLKVer, temp_1, 7);
	Ascii2BCD(gBusVerInfo.busProVer, temp_2, 2);
	memcpy(rRecordDeal.busProVer, temp_2, 7);
	Ascii2BCD(gBusVerInfo.busLineVer, temp_3, 2);
	memcpy(rRecordDeal.busLineVer, temp_3, 7);
	memcpy(rRecordDeal.CurPointer, (unsigned char*)&heap, 4);
	memcpy(rRecordDeal.CurRecordPointer, (unsigned char*)&curp, 4);
	memcpy(rRecordDeal.BlackListNum, (unsigned char*)&pFistVary.BlackListNum, 4);
	memcpy(rRecordDeal.rLineNo, pFistVary.LineNo, 2);
	memset(rRecordDeal.rResever, 0xff, 19);

	MSG_LOG(" rRecordDeal:\r\n");
	BCD_LOG(rRecordDeal.rDeviceNo, 64, 1);

	
	WriteRecord((unsigned char *)&rRecordDeal, 0);
#endif
//#ifdef BUS_PRO_JSB
//	return;
//#endif
	return;	
}
#ifdef _CPU_24G_
extern unsigned char CPU24GPsamInitialize(void);
#endif
//ȡǩ����Ϣ��������Ϣ ֻ�ڿ���ʱ����һ��
void getPOSPara(void)
{
	unsigned int BR_HeadAddress;
	unsigned int BR_c_cur_po;
	stStaeInfo stainto;
	unsigned int ii, crc32;

#ifndef BUS_MZT 
#ifdef BUS_CPU_
#ifdef BUS_PRO_JSB

	if(CPUPsamInitialize_JSB() == ST_OK)

#endif
	if(CPUPsamInitialize() == ST_OK)
		;
#elif defined _CPU_24G_
	if(CPU24GPsamInitialize() == ST_OK)
		;
#endif
#endif

//	gmissOldflag = gmissflag;//�����ϴ����������־,ִ���걾������󷵻���һ����
//	gmissflag = MISS_M_ActiveTest;
	//�����ʼ������
#ifndef NO_GJ_SERVISE
	gSendOverTime = 0;
#endif

#ifndef NO_GJ_SERVISE
#ifdef NO_MOBILE_SERVISE
	gmissflag = MISS_G_LOGINGJ;
#endif
#endif
	
	for(ii=0;ii<3;ii++){
		sysferead(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);//ÿ�����һ����ʷָ���ͳ������ ����ʱ��7+��ֵ�ۼƽ��4+�����ۼƽ��4+�ۿ�����2+����11+CRC32(4)
		
		crc32 = GenerateCRC32((unsigned char*)&stainto, (sizeof(stStaeInfo)-4));

		MSG_LOG("R stStaeInfo:");
		BCD_LOG((unsigned char*)&stainto, sizeof(stStaeInfo), 1);
		BCD_LOG((unsigned char*)&crc32, 4, 1);

		if(stainto.sCrc32 == crc32)
			break;
	}
	if(ii >= 3){
		memset((unsigned char*)&stainto, 0, sizeof(stStaeInfo));
		MSG_LOG("ͳ�Ƽ�������\r\n");
	}

	MSG_LOG("R SysTime:");
	BCD_LOG((unsigned char*)&SysTime, 7, 1);
	BCD_LOG(stainto.DataTime, 7, 1);

	if(memcmp((unsigned char*)&SysTime, stainto.DataTime, 4) != 0){//����û�и�����ʷͷָ��	
		MSG_LOG("R SysTime2:");
		BCD_LOG((unsigned char*)&SysTime, 7, 1);
		BCD_LOG(stainto.DataTime, 7, 1);

		get_BR_Point(&BR_HeadAddress, &BR_c_cur_po);
		savesysfile(0, BR_c_cur_po, BR_c_cur_po);//��ͷ����β		
		cls();
		
		memset((unsigned char*)&stainto, 0, sizeof(stStaeInfo));
		memcpy(stainto.DataTime, (unsigned char*)&SysTime, 7);
		crc32 = GenerateCRC32((unsigned char*)&stainto, sizeof(stStaeInfo)-4);
		stainto.sCrc32 = crc32;
		
		MSG_LOG("R SysTime3:");
		BCD_LOG((unsigned char*)&SysTime, 7, 1);
		BCD_LOG(stainto.DataTime, 7, 1);

		sysfewrite(BIS_PERDat_Info, sizeof(stStaeInfo), (unsigned char*)&stainto);
				
		memset(irda_rxBuf,0,50);
		sysfewrite(BIT_INTERVAL_STAR,1,irda_rxBuf);
		sysfewrite(BIT_INTERVAL_END,1,irda_rxBuf);
		memset(irda_rxBuf,0xFF,50);
		for(ii=0;ii<7;ii++)
			sysfewrite(BIT_INTERVAL_DATA+(ii*LAST_LIST_LEN),LAST_LIST_LEN,irda_rxBuf);

// 		MessageBox(0,"����ͳ��ָ��");
// 		beep(4,50,50);
// 		delayxs(2);
	}
	
	saveHisaddr();
	
}

#ifndef qPBOC_BUS
void getOtherFileVer(void)
{
	unsigned char buff[64];
	char Ver[64];
	char *p1,*p;
	unsigned short i;
															   
	memset(gBusVerInfo.busVoiceVer, 0, 2);
	flashread(FLASH_GPS_SOUND_START, (unsigned char*)buff, 32);

	MSG_LOG("OtheerFiles Ver:");
	MSG_LOG((char*)buff);

	p1 = strstr((char*)buff, "�ļ��汾");
	if(p1 == NULL)//�ļ�û�а汾�����Ϸ�
		return;
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

	MSG_LOG("Ver0:");
	MSG_LOG(Ver);
	MSG_LOG("||||\r\n");
 
	i = __atol(Ver);
	memcpy(gBusVerInfo.busVoiceVer, (unsigned char*)&i, 2);

	MSG_LOG("Ver1:");
	BCD_LOG(gBusVerInfo.busVoiceVer, 2, 1);
}
#endif


void DealGPSforMain(void)
{
	
#ifdef _GPS_ENABLE_
#ifdef FengDuan_BU_	//��������˷ֶΣ�
	if(gFDMode != FDMODE_NORMAL){//����Ǻ��Ż���������GPS
		main_Gps();
	}
#else
	main_Gps();
#endif	//#ifdef FengDuan_BU_
#endif
}

void DealGPRSforMain(void)
{	
#ifndef NO_GJ_SERVISE
#ifdef FengDuan_BU_	//��������˷ֶΣ�
	if(gFDMode != FDMODE_NORMAL){//����Ǻ��Ż���������GPRS
#ifdef WIFI_TLH_
		wifiTlh_main();
#else
		main_GpsRs();
#endif	//#ifdef WIFI_TLH_
	}
#else
#ifdef WIFI_TLH_
	wifiTlh_main();
#else
	main_GpsRs();
#endif	//#ifdef WIFI_TLH_
#endif	//#ifdef FengDuan_BU_	//��������˷ֶΣ�
#endif	//#ifndef NO_GJ_SERVISE
	
}

int main(void)
{

	MSG_LOG("TCP_IP_PACKET1:len:%d\r\n", sizeof(TCP_IP_PACKET1));
	MSG_LOG("TCP_IP_PACKET1:len:%d\r\n", sizeof(GPS_INFO));
	MSG_LOG("stDiaoDuinfo:len:%d\r\n", sizeof(stDiaoDuinfo));
	MSG_LOG("stStaeInfo:len:%d\r\n", sizeof(stStaeInfo));
	MSG_LOG("��������1::%d|7168\r\n", BIT_KeyPrice+4);//������������
	MSG_LOG("��������2::%d|8192\r\n", BIT_FE_END_ADDR);//������������
 
	intdev();
#ifdef _TTSSound_enable_
	xfSounding = 0;
	XF_Reset_LOW();
#endif
	
	BlightOpen();		//�򿪱���
//	MSG_LOG("-----main-----\r\n");
//	beep(2, 100, 50);
	GetDateTime();		//ȡϵͳʱ��
	gPsamErrorFlag = 0;
	BUflag = 0;

//	checkFlashDisFAT();
	
#ifdef FengDuan_BU_
	FD_Check_devMode();
#endif


//	check_tmp_record();	//�������flash������û�м�¼���еĻ���д �����ļ�	
	
	MSG_LOG("flash��С:%d\r\n",ugflash_capacity);
	
//	check_free_flash(5);	//ȷ��ʣ��5M�ռ�

// #ifdef QR_CODE_COM
// 	memset(irda_rxBuf,0,20);
// 	//ȡһ��ID
// 	find_BLK_QR(irda_rxBuf,irda_rxBuf,0);//��0  Ϊ��ȡһ�°汾����
// 	QR_BLK_Ver = get_QR_blkVer();
// #endif
	



	SysParInit();
	restore_flag = 3;
	
#ifdef qPBOC_BUS
	qPBOC_INIT();
#endif
	
	gPsamErrorFlag = 0;
#ifndef _M1KEY_3DES_
#ifndef _M1KEY_CLOUD_
#ifndef BUS_PRO_JSB
	
	PsamInitialize();
		
#endif

#endif
#endif

#ifndef NO_MOBILE_24G_
	getMobilePara();	//ȡ�ƶ���ز���
#else
	getPOSPara();
#endif
#ifdef BUS_PRO_JTB
 CPUPsamInitJTB();
#endif
#ifdef BUS_MZT
 PsamInitialize();
#endif

#ifndef NO_GJ_SERVISE	
#ifdef GPRS_MG323_
	GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
#elif defined CDMA_MC323_
	GPRSLinkProcess = GPRS_LINK_UP;//GPRS_MODE_NEED_CLOSE;
#elif defined WIFI_TLH_
	GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
#else
	GPRSLinkProcess = GPRS_MC55_CLOSE;
#endif
#endif

	if(gPsamErrorFlag){
		cls();
		if(gchn_eng == 'E')
			display(0,0,"Error:",1);
		else
			display(0,0,"����:",1);
		if(gPsamErrorFlag&1){
			if(gchn_eng == 'E')
				display(2,0, "PSAM1 error!",0);
			else
				display(2,0,"����PSAM����!",0);
		
			gErrorFlag |= ERROR_PSAM2;
		}
#ifndef NO_MOBILE_24G_
		if(gPsamErrorFlag&2){
			if(gchn_eng == 'E')
				display(4, 0, "PSAM2 error!",0);
			else
				display(4,0,"�ƶ�PSAM����!",0);
#ifdef _M1KEY_3DES_
			gPsamErrorFlag = 0;
#elif defined _M1KEY_CLOUD_
			gPsamErrorFlag = 0;
#endif
		}
#elif defined BUS_CPU_
		if(gPsamErrorFlag&2){
			if(gchn_eng == 'E')
				display(4, 0, "PSAM2 error!",0);
			else
				display(4,0,"��ͨ��PSAM����!",0);
#ifdef _M1KEY_3DES_
			gPsamErrorFlag = 0;
#elif defined _M1KEY_CLOUD_
			gPsamErrorFlag = 0;
#endif
		}
#elif defined _CPU_24G_
		if(gPsamErrorFlag&2){
			if(gchn_eng == 'E')
				display(4, 0, "PSAM2 error!",0);
			else
				display(4,0,"CPU��PSAM����!",0);
#ifdef _M1KEY_3DES_
			gPsamErrorFlag = 0;
#elif defined _M1KEY_CLOUD_
			gPsamErrorFlag = 0;
#endif
		}
#endif
		delayxs(1);
	}

// #ifdef qPBOC_BUS
// 	MSG_LOG("�ȵ�¼����\r\n");
// 	gmissflag = MISS_PBOC_LOGIN;
// #else
	MSG_LOG("�ȵ�¼����\r\n");
	gmissflag = MISS_G_LOGINGJ;
//#endif

//	setG24Reader();
#ifndef NO_MOBILE_24G_
	Rest24Card();
#elif defined _CPU_24G_
	RestCPU24Card();
#endif

#ifdef KEY_PAD
	KeyPadClr();//���KEY_PAD��LCD��ʾ
	gKeyRestorDis = 3;
#endif
	
#ifdef qPBOC_BUS
	qPbocDealPkey();
#elif defined _TTSSound_enable_
	DealADTmpFile();
#else//���������ļ���Ϣ
	getOtherFileVer();
#endif

// #ifdef _GPS_ENABLE_
// 	gps_config_hw();
// 	delayxms(100);
// 	gps_config_hw();
// #endif
//	PosStateInit();//�����Լ��¼,��ʱֻ���ܶ���
#ifndef NO_GJ_SERVISE
#ifndef GPRS_MG323_
#ifndef CDMA_MC323_
#ifndef WIFI_TLH_
	GPRS_close_Gt900(2000);
#endif
#endif
#endif

#ifdef Modem_BUS_
	gprs_snd(12, "GT$POWERON\r\n");
#else
#ifndef OTHER_GPRS
	gprs_4V_ON(); 
#endif
#endif
#ifdef GPRS_MG323_
	gprs_pwon_lo();
#elif defined CDMA_MC323_
	gprs_pwon_hi();
#endif
#endif 

	set_timer0(1000, 0);//1S�ӽ�һ�� GPRSר��
	set_timer0(5000, 1);

	SetAudio();

	//testXF();
#ifdef ZKXL_24G_
	comParaInit();
#elif defined _XMSHDZ_24G_
	OcomParaInit();
#endif

#ifndef NEED_AIR_BUS_CARD
	F_FLAG_VARY.PlusSta = 1;//���ӷ�����
#endif
#ifdef NO_PlusSta_
	F_FLAG_VARY.PlusSta = 0;//û�и��ӷѹ���
#endif

//	CheckHzk(0);
//	F_FLAG_VARY.stop_flag = 1;
	while(1){
#ifdef _BU_NO_STOP_
		F_FLAG_VARY.stop_flag = 0;
#endif
		clr_wdt();
		main_face();
		main_key();
		main_card();
			
		DealGPRSforMain();
 #ifdef QR_CODE_COM
 		QR_Code_Test_4500R();//�����ά�롣��ǰֻ����ʾ��
 #endif
		
		DealGPSforMain();
#ifdef FengDuan_BU_
		FengDuan_main();
#endif

#ifdef KEY_PAD
		KeyPad_main();
#endif
	}
}
 
// void mcm_rs232_LCD3232_F407(unsigned char mode)
// {
// //#warning "this function is not exist!!"	
// 	;
// }

