/**************************************************
* �ļ��� :CPUCARJTB.c
* ����   :��ͨ��CPU�������ļ�
* ����   :15/11/2015 @zzc	
**************************************************/

#include "CPUCARJTB.h"
#include "szct.h"
#include "E_GJGPRS.h"
#include "Mydes.h"
#if ((defined BUS_CPU_)||(defined _XMSHDZ_24G_)||(defined ZKXL_24G_))
#include "Cpucard.h"
//�����Ǻ�24G�ֻ������õı���
#ifdef NO_MOBILE_24G_
extern unsigned char G24PsamNo[6];
extern unsigned char g24PSAMDsno[4];//�ն����к�
extern typea_public_inf_zaozuang stuPubInfZaoZuang;
extern pboc_debit_capp_purchase stuDebitPurchase;
extern pboc_sam_init_purchase stuSamInitPurchase;
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
#else 
extern unsigned char G24PsamNo[6];
extern unsigned int g24GDisFlash;//24G������ˢ��
extern unsigned char g24PSAMDsno[4];//�ն����к�
extern typea_public_inf_zaozuang stuPubInfZaoZuang;
extern pboc_debit_capp_purchase stuDebitPurchase;
extern pboc_sam_init_purchase stuSamInitPurchase;
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
#endif
#endif
extern void SoundFreeManage(char *sstr);
//extern void dis_time_mini(unsigned char x, stcomtime *time);
extern void Sound_cityCMD(unsigned char sMode);
extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
//extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void BlightOpen(void);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
//extern void BuildRecordeHistory(unsigned char delType, unsigned char *recBuf);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void CountPoint(unsigned char mode);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char FreeConsume(void);
extern void disp_no_swipe(void);
extern void error(unsigned char code1,unsigned char Index);
extern void ControlDis(unsigned char type);
extern unsigned char Delay_Normal(void);
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
//��Щ����Ҫ������Ů��
extern unsigned char nnAudioflag;
extern unsigned char mode2or3;
//������:�ֻ���03\�մ￨ 04\������01\CPU��02
extern unsigned char gMCardCand;
extern stPricetable NewPriceTab;
extern Parameter5 cardMessage;
extern unsigned int a_sumR;//�ֻ�Ǯ���е���ʵ���
extern unsigned char DriveCardNo[4];
extern pFistVary_1 pFistVary;
extern Parameter3 ComSumeFile;
extern unsigned int c_serial;
extern unsigned char cardSound;			//ˢ��û�гɹ���־
extern unsigned int card_ser;
extern Parameter2 POINT_MESSAGE;
extern unsigned char gchn_eng;//��Ӣ�ı�־
extern unsigned char BUflag;//���ػ�״̬ 0x02 ��Ҫˢ˾���� 
extern unsigned char gucSoudnorm;
extern unsigned char CPU_Read_Money(unsigned char *remb, unsigned char mode);
extern unsigned char gPSAMin;//psam������λ��
extern unsigned char GetDateTime(void);
extern void deal2or3(unsigned char*sndd);
extern unsigned char getMiKey(void);
//888888888888888888888888
 stJTB1A	JTB_1A;//1A�ļ��ṹ
 stJTB15	jtb15file;//15
 stJTB16	jtb16file;//16
 stJTB17	jtb17file;//17
 stJTB18	jtb18file;//18
 stJTB1E	JTB_1E;//1A�ļ��ṹ
pboc_sam_init_purchaseJTB stuSamInitPurchaseJTB;
 unsigned char pursMode;//����ģʽ
unsigned char dealtype;//�۷�ģʽ
unsigned char CardType; //�������� ����/���
unsigned char BUS_Flag;//����״̬ һ��8λǰ��λ�ֱ��ʾ  1�Ƿ񷢳�2�Ƿ��ڳ�վ3�Ƿ�ˢ�˷�����
FeUnKnowREC FeuRec;	//

extern unsigned int get2stationPrice(unsigned char sta1, unsigned char sta2);
extern unsigned char gFDderict;

extern unsigned short WhiteLength;		////���ذ�������ʱ��¼������
extern stBusVerIfo gBusVerInfo;
extern void addStatMoney(unsigned char mode, unsigned int moneyv);
unsigned char PsamNumJTB[6];
//unsigned char Cpufile15[30];//tttttttttttttttttttttttttttttttttttt
//������ ����������� ȡ0x15�ļ��µķ���������ʶ��01-08 HEX����ǰ8λ��������Ƚ�
#define white_organization 1
const unsigned int WriteNUM[white_organization+1] = {
	0x04026110,0
};
						
sam_pubJTB SamPubInf_LLJTB;
#define PSAM_JTB   SAM3

#ifdef BUS_PRO_JTB
//��Ч����0  ��Ч��������
unsigned short WhiteLength;		////���ذ�������ʱ��¼������
//jsb_rec jsb_recbuff;		//
//��Ч����0  ��Ч��������
unsigned int load_white_list(void)
{
	unsigned char buftemp[32];
	unsigned int addr;
	unsigned short len;
#ifdef _debug_white
	unsigned int i;
#endif
#ifdef _debug_
	debugstring("���ذ�����:BCD::\r\n");
#endif
	
	addr = FLASH_WHITE_START;
	flashread(addr,buftemp,6);
#ifdef _debug_
	debugstring("read:");
	debugdata(buftemp,6,1);
#endif
	if (memcmp(buftemp,"��Ч",4)==0 )
	{
#ifdef _debug_
		debugstring("��������Ч=====\r\n");
#endif
		memcpy((unsigned char*)&len,buftemp+4,2);
		over_turn(2, (unsigned char *)&len);
		len = len/(WHITE_SNO_LEN);		//flash�д����4�ֽڵģ�ʵ����ǰ���ֽڣ�
		
		WhiteLength =len;
		
		if ( WhiteLength > WHITELEN)
		{
			WhiteLength = 0;
			len = 0;
			flasherase(ERASE4KBYTESECTOR,addr);
			memcpy(buftemp,"��Ч",4);
			memcpy(buftemp+4,&WhiteLength,2);
			flashwrite(addr,buftemp,6);
		}
		
	}else{
#ifdef _debug_
		debugstring("û�а�����::\r\n");
#endif
		WhiteLength = 0;
	}
	
	
#ifdef _debug_white
	if (WhiteLength != 0)
	{	
		debugstring("WhiteLength:");
		debugdata((unsigned char *)&WhiteLength,2,1);
		for (i=0;i<WhiteLength;i++)
		{
			addr = FLASH_WHITE_START + 6 + i*(WHITE_SNO_LEN);	//��ַ��4byte�������4�ֽڵģ���2�ֽ�
			flashread(addr,buftemp,WHITE_SNO_LEN);
			if (i%300 == 0)
			{
				clr_wdt();
			}
			debugdata(buftemp,WHITE_SNO_LEN,1);
		}
	}
#endif 
	
	
	
	return WhiteLength;	
	
}
unsigned char CPUPsamInitJTB(void)
{
	unsigned char i;
//	unsigned char ii=0;
	unsigned char buffer[256];
	gPsamErrorFlag |= 2;
	
	memset(PsamNumJTB, 0, 6);
#ifdef _debug_CPU_
	debugstring("---JTBPSAM init\r\n");
//	ii = PSAM_JTB;
//	MSG_LOG("PSAMJTB:%d",ii);
#endif
	cls();
	display(0,0,"���ڼ�⽻ͨ��PSAM��",0);
	display(2,1,"��ȴ�......",0);
	

	for(i=0;i<2;i++)
	{
		if( SamColdReset(PSAM_JTB, 9600, SAM_VOL3, buffer))       //;//
			break;
			/*	return ST_ERROR;*/
			MSG_LOG("��ʼ������\r\n");
		if(SamColdReset(PSAM_JTB, 38400, SAM_VOL3, buffer))
			break;
	}	
#ifdef _debug_CPU_
	debugstring("CPUPSAM art:");
//	debugdata(buffer, ii, 1);
#endif
	delayxms(5);
	if(Read_Binary(PSAM_JTB, 0x16, 0, 6, buffer)==0) 
	{
#ifdef _debug_CPU_
		debugstring("CPURead_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
#ifdef _debug_CPU_
	debugstring("CPUread 16 Binary:");
	debugdata(buffer,8,1);
#endif
	memcpy(PsamNumJTB,buffer,6);
	delayxms(100);
	
#ifdef _debug_CPU_
	debugstring("CPUSelect File \r\n");
#endif

	//��ǰ AID=ASCII��MOT.CPTSAM01��
	//A0 00 00 06 32 4D 4F 54 2E 43 50 53 41 4D 30 31

	memcpy(buffer, "\xA0\x00\x00\x06\x32\x4D\x4F\x54\x2E\x43\x50\x53\x41\x4D\x30\x31", 16);		//JTB����Ǯ��AID
#ifdef _debug_CPU_
	debugstring("***select AID1:");
	debugstring("SND_C:");
	debugdata(buffer, 16, 1);
#endif
	if(Select_FileF(PSAM_JTB, buffer, 16)==0)
	{
#ifdef _debug_
		debugstring("CPU PSAM Select_File1 error\r\n");
#endif
		return ST_ERROR;
	}

	if(Read_Binary(PSAM_JTB, 0x17, 0, 25, buffer)==0)		//��0x17�ļ�25�ֽ�
	{
		debugstring("GJ_Read_Binary17 error\r\n");
		debugdata(buffer, 8, 1);
		return ST_ERROR;
	}
#ifdef _debug_CPU_
	debugstring("JTB _read 17 Binary:");
	debugdata(buffer,25,1);	//01000000000000000100000000000000012011122220161220
	debugstring("OK\r\n");
#endif
	memcpy((unsigned char*)&SamPubInf_LLJTB, buffer, sizeof(SamPubInf_LLJTB));	//��һ���ֽ�����Կ����
	
	gPsamErrorFlag &= 0xFD;
	return ST_OK;
}

// void end_close_card(unsigned char moded)
// {
// 	unsigned int i;
// 	unsigned int sno1;
// 	unsigned char buffer[10];
// 	//	unsigned char key;
// 	
// 	sno1 = c_serial;
// 	i = 0;
// 	do {
// 		if(moded == 1){
// 			PcdRfReset(5);
// 			delayxms(10);
// 		}
// 		i++;
// 		if (GetTypeAUID(buffer)!=0) 
// 		{
// 			if (memcmp((unsigned char *)&sno1,buffer,4) ==0) 
// 			{
// 				i=0;
// 			}
// 		}
// 	} while(i<5);
// 	MifareHalt();
// 	return;
// }
extern void ERR_DIS(char *dis,unsigned char *dat);//��ʾ����

//HEXתBCD  Dec ��Ҫת��������  length ��Ҫת�����ֽ��� mode==1,�������ת��
int DectoBCD(unsigned int Dec, unsigned char *Bcd, int length,unsigned char mode)  //
{  //
	    int i;  
	     int temp;  
		 if(mode==1)
			over_turn(4,(unsigned char*)&Dec);

		    for(i=length-1; i>=0; i--)  
		     {  
				temp = Dec%100;  
				Bcd[i] = ((temp/10)<<4) + ((temp%10) & 0x0F);  
				Dec /= 100;  
			}  
			
		return 0;  
}  


// extern int getCpuLineNo(void);
// extern int getCpuDriveNo(void);
// extern unsigned char G24PsamNo[6];
// extern unsigned char BUS_Flag;//����״̬
// void addSCtimes(unsigned char atimes);
// void BuildRecordeJTB(unsigned char delType, unsigned char *recBuf)
// {
// 	DEALRECODE_JTB *rRecordDeal;
// 	//	unsigned int temp;
// 	//		unsigned char ttt;
// 	rRecordDeal = (DEALRECODE_JTB *)recBuf;
// 	memset(rRecordDeal->rPublishNumb,0,128);
// 	memcpy(rRecordDeal->rPublishNumb, cardMessage.PublishBicker, 10);//����
// 	rRecordDeal->rCardType = cardMessage.card_catalog;				//����
// 	rRecordDeal->rCardCand = gMCardCand;							//������
// 	memcpy(rRecordDeal->rSAMxulie,stuDebitPurchase.cTermSnq,4);		//PSAM������ˮ
// 	memcpy(rRecordDeal->rPSAMNO,PsamNumJTB,6);						//�ն˱���PSAM����
// #ifdef _NEW_Device
// 	ASCII2ZBcod((char*)pFistVary.DeviceNo,rRecordDeal->rDeviceNo, 4);	//�������豸�� ����
// #else
// 	Ascii2BCD(pFistVary.DeviceNo,rRecordDeal->rDeviceNo,8);
// #endif
// 	
// 	rRecordDeal->rDealType = delType;								//��������
// 	memcpy(rRecordDeal->rPiaoJia,NewPriceTab.busPrice,2);			//Ʊ��
// 	memcpy(rRecordDeal->rMoneyDealSum,stuInitPurchaseRet.cSnq,2);	//Ǯ�����״���
// 	memcpy(rRecordDeal->rDealMoney, (unsigned char *)&s_sum1, 3);	//���׽��
// 	memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&a_sum1, 4);	//�������
// 	memcpy(rRecordDeal->rDealTime,(unsigned char*)&SysTime,7);		//����ʱ��
// 	memcpy(rRecordDeal->rCPUTAC,stuDebitPurchase.cMac,4);			//TAC
// //	getCpuDriveNo();//
// 	memcpy(rRecordDeal->rDriverNo, (unsigned char*)&DriveCardNo, 4);//˾������
// 	getCpuLineNo();
// 	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 3);				//��·��
// #ifdef FengDuan_BU_
// 	rRecordDeal->rGPSzhandian=gFDucID;//վ���� 1
// 	rRecordDeal->rGPSfangxian=gFDderict;//GPS����  1
// #endif
// 	memcpy(rRecordDeal->rFactorFlg,jtb15file.cPublisherSign,8);		//��������15
// 	memcpy(rRecordDeal->rCityCode,jtb17file.Citycode,2);			//���д���15
// 	if(delType==ID_REC_HJY_REC)
// 		rRecordDeal->rHUIJIAOYI=1;										//�ҽ��ױ�ʶ15
// 	else
// 		rRecordDeal->rHUIJIAOYI=0;
// #ifdef FengDuan_BU_
// 
// 	//�������ϳ���Ϣ����¼������
// 	memcpy(rRecordDeal->rDealTimeOLD,JTB_1A.rPursTimeOn,7);			//��¼ʱ��
// 	memcpy(rRecordDeal->rPSAMNOOLD,JTB_1A.rPsamOn,6);				//��¼�ն�PSAM
// 	memcpy(rRecordDeal->rDriverNoOLD,JTB_1A.rDriverNum,4);			//��¼˾����HEX
// 	memcpy(rRecordDeal->rLineNoOLD,JTB_1A.rLineNum,2);				//��¼��·��
// 	memcpy(rRecordDeal->rCityCodeOLD,JTB_1A.rCityCodeON,2);			//��¼���д���
// 	memcpy(rRecordDeal->rFactorFlgOLD,JTB_1A.rFactoryOn,8);			//��¼��������
// 	Ascii2BCD(JTB_1A.rBusNum,rRecordDeal->rDeviceNoOLD,8);			//��¼������
// #endif
// 
// #ifdef _NEW_Device
// 	ASCII2ZBcod((char*)&JTB_1A.rBusNum,rRecordDeal->rDeviceNoOLD, 4);		//�������豸�� ����
// #else
// 	Ascii2BCD(JTB_1A.rBusNum,rRecordDeal->rDeviceNoOLD,8);
// #endif
// 	
// 	//
// #ifdef _debug_
// 	BUS_Flag|=BUS_TES;//���Լ�¼
// #else
// 	BUS_Flag&=BUS_CL_TES;
// #endif
// 	rRecordDeal->rBusFlag=BUS_Flag;
// 	//����14
// 	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);			//����汾
// 	memcpy(rRecordDeal->rTTTT,"\xAA\xAA\xAA\xAA",4);				//�������� ������ĸ�AA
// 	
// #ifdef _debug_
// 	debugstring("��֯��¼\r\n");
// 	debugdata(rRecordDeal->rPublishNumb,29,1);
// 	debugdata(rRecordDeal->rMoneyDealSum,40,1);
// 	debugdata(rRecordDeal->rDealTimeOLD,39,1);
// 	
// #endif
// 	if((delType == ID_REC_TOLL)||(delType == ID_REC_MON)){
// 		addStatMoney(delType, s_sum1);
// 
// #ifdef FengDuan_BU_
// 		if(BUS_Flag&BUS_ON)//�ϳ�
// 			addSCtimes(1);//ˢ��������1 
// 		else 
// 			addSCtimes(0);//ˢ��������1 
// #endif		
// 	}
// 	//	memcpy(recBuf, (unsigned char*)&rRecordDeal, 128);
// #ifdef _debug_
// 	debugstring("BLD REC:");
// 	debugdata((unsigned char*)&rRecordDeal,128,1);
// #endif
// 	
// }

// ��������64�ֽڳ���д128��,mode=0;����128��  mode=1,����128
unsigned char WriRecorJTB(unsigned char *rec,unsigned char mode)
{
	unsigned char i=0;
	unsigned char buff[RECORD_JTB_LEN+1];
	unsigned char buffer[RECORD_JTB_LEN+1];
	//	unsigned char *tempbuf;
	unsigned int temp;
	//	unsigned char len;
	
	memset(buff, 0x00, RECORD_JTB_LEN);
	memcpy(buffer, rec, RECORD_JTB_LEN);
	if((mode==1)&&(memcmp(rec+124,"\xAA\xAA\xAA\xAA",4)!=0))//����128��
	{
		MSG_LOG("������");
		//memcpy(buffer,rec,64);
		memset(buffer+64,0,60);
		memset(buffer+64+60,0xBB,4);
	}
	temp = Get_Record_point(buff, 0);
	
	if(tcpipPacket.packetStatus!=RECORD_DEAL){
		set_timer0(100, 0);//�ӿ�ȥ������
		tcpipPacket.packetStatus = 0;
		MSG_LOG("�ӿ��������");
	}else{
		;
	}
#ifdef _debug_
	debugstring("-------------------\r\ncur:");
	debugdata((unsigned char*)&temp, 4, 1);
	flashread(temp, buff, RECORD_JTB_LEN);
	debugdata(buff, RECORD_JTB_LEN, 1);
#endif
	
	for(i=0; i<3; i++) 
	{
		
		flashwrite(temp, buffer, RECORD_JTB_LEN);
#ifdef _debug_
		debugstring("WriteRecord : \r\n");		
		debugdata(buffer,RECORD_JTB_LEN,1);
#endif
		
		delay();
		flashread(temp, buff, RECORD_JTB_LEN);
#ifdef _debug_
		debugstring("WriteRecordCheck ulofset2: \r\n");
		debugdata(buff, RECORD_JTB_LEN, 1);
#endif
		clr_wdt();
		if(memcmp(buffer, buff, RECORD_JTB_LEN) == 0) 
		{
			writeBackRec(buffer,RECORD_LEN, 0);
			writeBackRec(buffer+RECORD_LEN, RECORD_LEN, 0);
			//clr_wdt();
			//writeBackRec(buffer+128, RECORD_LEN, 0);
			MSG_LOG("WRITRC_OK!");
			return ST_OK;
		}
		else{//д������дȫ0������дʵ���ݣ���д����һ���ݵ�ַ
#ifdef _debug_
			debugstring("write record ERROR!!!\r\n");
#endif
			memset(buff, 0x01, RECORD_JTB_LEN);
			flashwrite(temp, buff, RECORD_JTB_LEN);
			temp += RECORD_JTB_LEN;
			delay();
		}
	}
	cls();
	
	writeBackRec(buffer, RECORD_LEN, 0);
	writeBackRec(buffer+RECORD_LEN, RECORD_LEN, 0);
	//	writeBackRec(buffer+128, RECORD_LEN, 0);
	display(2, 2, "E2PROMд����,��Ҫά��!", 1);
	memset(buffer, 0, 20);
	memcpy(buffer, "ADDR:", 5);
	BCD2Ascii((unsigned char *)&temp, buffer+5, 4);
	display(6,0,(char*)buffer, 0);
	
	led_on(LED_RED|LED_GREEN);
	for(;;)
		delayxms(50);
}
#ifdef FengDuan_BU_
extern stLineInfo LineInfo[2];//��·��Ϣ�������վ����Ϣ ��������0�� ���� 1�鷴��
extern stRunStation RunStation;				//�����е�վ����Ϣ
//��֯1A�ļ�
//�ϳ���¼���³���¼ ������
extern stFDLineHead gFdLinehead;
extern unsigned char gFDucID;	
extern unsigned char gFDStaIndex[2];	//��ǰ���ػ�վ�����кŴ�0��ʼ,��վ���Ų�һ��
void Build_1A_Rec(unsigned char *Rvb,unsigned char mode){//01 �ϳ�  02  �³�

	unsigned char tembuff[20];
	unsigned int itm;
	stJTB1A JTB_1AR;//
	unsigned char i,k;
	
#ifdef _debug_CPU_
	debugstring("Build_1A_Rec:\r\n");
	debugdata(JTB_1A.rID,128,1);
	debugstring("15\r\n");
	debugdata(jtb15file.cPublisherSign,30,1);
	debugstring("16\r\n");
	debugdata((unsigned char*)&jtb16file.cCardType,55,1);
	debugstring("17\r\n");
	debugdata(jtb17file.intercode,60,1);
	debugstring("stuDebitPurchase\r\n");
	debugdata(stuDebitPurchase.cTermSnq,15,1);
#endif

	memcpy(JTB_1AR.rID,JTB_1A.rID,128);
//	memcpy(JTB_1AR.rID,"\x27\x02",2);
	if(mode==ON_BUS)//EP�ϳ���¼ ����Ǯ����
	{
//	JTB_1AR.rOk=1;
//	JTB_1AR.rpursFlag=1;//.........................				//���ױ�ʶ ��������  ���β�Ʊ �ĳ��³�״̬
//	JTB_1AR.rLock=0x00;//0xJTB_1AR.rLock;//............................	//01
	memcpy((unsigned char*)&itm,stuDebitPurchase.cTermSnq,4);
	DectoBCD(itm,tembuff,8,1);//...........................		//ת��8�ֽ�BCD
	memcpy(JTB_1AR.rPursSearl,tembuff,8);//...............		//������ˮBCD
	JTB_1AR.rPursState=1;										//����״̬?  �ϳ� �����³�
																//����Ҫ�����ϳ��³���¼
	memcpy(JTB_1AR.rCityCodeON,jtb17file.Citycode,2);			//�ϳ����д���2 BCD�����¼��߶�������
	memcpy(JTB_1AR.rFactoryOn,jtb15file.cPublisherSign,8);		//�ϳ�������ʶ8 BCD
	//�ϳ�վ��8 BCD����ͨ����û���ϳ���	
	memset(JTB_1AR.rPsamOn,0,8);
	memcpy(JTB_1AR.rPsamOn+2,G24PsamNo,6);						// �ϳ��ն�8  BCD
	memcpy(JTB_1AR.rPursTimeOn,(unsigned char*)&SysTime,7);		//�ϳ�ʱ�� BCD
	memcpy((unsigned char*)&itm,(unsigned char*)&s_sum1,4);
	over_turn(4,(unsigned char*)&itm);
	memcpy(JTB_1AR.rPrice,(unsigned char*)&itm,4);//���׽��

	
  //������ȡ���յ�վ�ļ۸�
	memset((unsigned char*)&itm,0,4);

//#ifdef FD_TES
	itm=200;
	over_turn(4,(unsigned char*)&itm);
	memcpy(JTB_1AR.rPriceMax,(unsigned char*)&itm,4);				//������ѽ��HEX(���)
	JTB_1AR.rStationOn=2;
	memcpy(JTB_1AR.rLineNum,pFistVary.LineNo,2);//..............//��·���HEX ����ˣ�
	memcpy(JTB_1AR.rBusNum,pFistVary.DeviceNo,8);//..........	//������  8 ascii
	
	memcpy(JTB_1AR.rDriverNum,DriveCardNo,4);					//�ϳ�˾����
	//	memcpy(JTB_1AR.rCwuyuanNum,JTB_1A.rCwuyuanNum,4);			//����Ա��
	JTB_1AR.rDirection=gFDderict;//memcpy(JTB_1AR.rDirection)����
	//�˴�
	memcpy(JTB_1AR.rTimeMax,"\x50\x00",2);//�ʱ��
	memcpy(JTB_1AR.rCiytCodeY,"\x03\x10",2);					//�ϳ����д����д���
	memcpy(Rvb,JTB_1AR.rID,128);
	BUS_Flag|=BUS_ON;//�ϳ�
	return;
//#endif
	if(RunStation.ucDirect == 1)//�����յ�Ϊ0��վ��
		i = 1;
	else	//�����յ�Ϊ���վ��
		i = LineInfo[0].StationInfo[LineInfo[0].ucStationMax-1].ucId;//�ָ�Ϊ�ɵ�ǰվ���յ�վ��Ʊ��
	
	MSG_LOG(" i::");
	BCD_LOG((unsigned char*)&i, 4, 1);
	MSG_LOG(" ucId::");
	BCD_LOG((unsigned char*)&LineInfo[0].StationInfo[LineInfo[0].ucStationMax-1].ucId, 1, 1);
	
	i = (((i>>4)*10)+(i&0x0F));	//վ����  ���������BCD��
	
	if(gFDderict<2)//������ȷ
		k = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//��1����Ϊ��Ŷ��Ǵ�1��ʼ�ġ�
	else
	{
		MSG_LOG("������");
		k = LineInfo[0].StationInfo[gFDucID-1].ucId;//�����ǽ�������
	}

		k = (((k>>4)*10)+(k&0x0F));	//վ����  ���������BCD��
	
		JTB_1AR.rStationOn=k;

	itm = get2stationPrice(i, k);//ȡ�ϳ����յ��Ʊ�� 
	if(itm>10000)//����
		memcpy((unsigned char*)&itm,gFdLinehead.TottalPrice,2);

	over_turn(4,(unsigned char*)&itm);
	memcpy(JTB_1AR.rPriceMax,(unsigned char*)&itm,4);				//������ѽ��HEX(���)

	memcpy(JTB_1AR.rLineNum,pFistVary.LineNo,2);//..............//��·���HEX ����ˣ�
	memcpy(JTB_1AR.rBusNum,pFistVary.DeviceNo,8);//..........	//������  8 ascii

	memcpy(JTB_1AR.rDriverNum,DriveCardNo,4);					//�ϳ�˾����
//	memcpy(JTB_1AR.rCwuyuanNum,JTB_1A.rCwuyuanNum,4);			//����Ա��
	JTB_1AR.rDirection=gFDderict;//memcpy(JTB_1AR.rDirection)����
	//�˴�


	memcpy(JTB_1AR.rTimeMax,gFdLinehead.NeedMaxTime,2);//�ʱ��
//	JTB_1AR.rStationOn=	LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//��1����Ϊ��Ŷ��Ǵ�1��ʼ�ġ�;		
	if((gFDderict < 2)&&(gFDStaIndex[gFDderict] < 100))
		memcpy(JTB_1AR.rName, LineInfo[gFDderict].StationInfo[gFDucID-1].name,10);//���10

	//����
		memcpy(JTB_1AR.rCiytCodeY,"\x03\x10",2);					//�ϳ����д����д���
		memcpy(Rvb,JTB_1AR.rID,128);
		BUS_Flag|=BUS_ON;//�ϳ�
	}
	else if(mode==OFF_BUS||mode==COVER_BUS){//�³���¼ 
	//	JTB_1AR.rOk=1;
	//	JTB_1AR.rpursFlag=1;//.........................				//���ױ�ʶ ��������
	//	JTB_1AR.rLock=0x00;//JTB_1AR.rLock;//............................			//Ӧ��������ʶ 0 û������ 1����Ӧ��
		memcpy((unsigned char*)&itm,stuDebitPurchase.cTermSnq,4);
		DectoBCD(itm,tembuff,8,1);//...........................		//ת��8�ֽ�BCD
		memcpy(JTB_1AR.rPursSearl,tembuff,8);//...............		//������ˮBCD
		JTB_1AR.rPursState=2;										//����״̬?
		//����Ҫ�����ϳ��³���¼

		memcpy(JTB_1AR.rCityCodeOff,jtb17file.Citycode,2);			//�³����д���2 BCD 
		memcpy(JTB_1AR.rFactoryOff,jtb15file.cPublisherSign,8);		//�³���������8 BCD
#ifdef FD_TES
	JTB_1AR.rStationOff=2;
#else
		k=LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//�³�վ��8 BCD����ͨ����û���ϳ���	;
		k = (((k>>4)*10)+(k&0x0F));	//վ����  ���������BCD��
		JTB_1AR.rStationOff=k;
#endif
		memset(JTB_1AR.rPsamOff,0,8);
		memcpy(JTB_1AR.rPsamOff+2,G24PsamNo,6);						// �³��ն�8  BCD

		memcpy(JTB_1AR.rPursTimeOff,(unsigned char*)&SysTime,7);	//�³���ʱ��   BCD

		memcpy((unsigned char*)&itm,(unsigned char*)&s_sum1,4);
		over_turn(4,(unsigned char*)&itm);
		memcpy(JTB_1AR.rPrice,(unsigned char*)&itm,4);//���׽��
	
		memset((unsigned char*)&itm,0,4);
		memcpy(JTB_1AR.rPriceMax,(unsigned char*)&itm,4);			//������ѽ��HEX(���) Ҫ����
		JTB_1AR.rDirection=gFDderict;//............................			//�����ʶHEX 
		memcpy(JTB_1AR.rLineNumOff,pFistVary.LineNo,2);//..............//��·���HEX ����ˣ�
		memcpy(JTB_1AR.rBusNumOff,pFistVary.DeviceNo,8);//..........	//������  8 ascii

		memcpy(JTB_1AR.rDriverNum,DriveCardNo,4);					//�³�˾����
		memcpy(JTB_1AR.rCiytCodeY,"\x03\x10",2);					//�ϳ����д���
		memcpy(Rvb,JTB_1AR.rID,128);
		BUS_Flag&=BUS_CL_ON;//�����ʶ
	}else{
		memcpy(Rvb,JTB_1A.rID,128);
	}
	MSG_LOG("��ע���:");
	BCD_LOG(JTB_1AR.rPriceMax,4,1);
	MSG_LOG("�ϳ�ʱ��:");
	BCD_LOG(JTB_1AR.rPursTimeOn,7,1);
	MSG_LOG("��������:");
	BCD_LOG((unsigned char*)&JTB_1AR.rpursFlag,1,1);
	MSG_LOG("������ˮ:");
	BCD_LOG(JTB_1AR.rPursSearl,8,1);
	MSG_LOG("����״̬:");
	BCD_LOG((unsigned char*)&JTB_1AR.rPursState,1,1);
	MSG_LOG("�ϳ���������:");
	BCD_LOG(JTB_1AR.rFactoryOn,8,1);
	MSG_LOG("�ϳ���·��:");
	BCD_LOG(JTB_1AR.rLineNum,2,1);
	MSG_LOG("�ϳ���վ��:");
	BCD_LOG((unsigned char*)&JTB_1AR.rStationOn,1,1);
	MSG_LOG("�ϳ����ƺ�:");
	BCD_LOG(JTB_1AR.rBusNum,8,1);
	MSG_LOG("�ϳ��ն˺�:");
	BCD_LOG(JTB_1AR.rPsamOn,8,1);
	MSG_LOG("����:");
	BCD_LOG((unsigned char*)&JTB_1AR.rDirection,1,1);
	MSG_LOG("�³����д���:");
	BCD_LOG(JTB_1AR.rCityCodeOff,2,1);
	MSG_LOG("�³���������:");
	BCD_LOG(JTB_1AR.rFactoryOff,8,1);
	MSG_LOG("�³���Ӫ����:");
	BCD_LOG(JTB_1AR.rYunYinCodeOff,2,1);
	MSG_LOG("�³���·��:");
	BCD_LOG(JTB_1AR.rLineNumOff,2,1);
	MSG_LOG("�³���վ��:");
	BCD_LOG((unsigned char*)&JTB_1AR.rStationOff,1,1);
	MSG_LOG("�³����ƺ�:");
	BCD_LOG(JTB_1AR.rBusNumOff,8,1);
	MSG_LOG("�³��ն˺�:");
	BCD_LOG(JTB_1AR.rPsamOff,8,1);
	MSG_LOG("�³���ʱ��:");
	BCD_LOG(JTB_1AR.rPursTimeOff,13,1);
	MSG_LOG("Ԥ����Ϣ:");
	MSG_LOG((char*)JTB_1AR.rName);
	BCD_LOG(JTB_1AR.rMileage,26,1);


		//Hex2BCD()

}
#endif

void Build_1E_Rec(unsigned char *Rvb,unsigned char mode){
//	unsigned char tembuff[20];
	unsigned int itm;
	stJTB1E JTB_1ER;//
	memset((unsigned char*)&JTB_1ER,0,48);

	JTB_1ER.eDealType=mode;		//�³���
	memcpy(JTB_1ER.eDeviceNum+2,PsamNumJTB,6);
	JTB_1ER.eTracode=02;		//��ҵ����
	memcpy(JTB_1ER.eLineNUm,pFistVary.LineNo,2);
	memcpy((unsigned char*)&itm,(unsigned char*)&s_sum1,4);
	over_turn(4,(unsigned char*)&itm);
	memcpy(JTB_1ER.eDmoney,(unsigned char*)&itm,4);//���׽��
	memcpy((unsigned char*)&itm,(unsigned char*)&a_sum1,4);
	over_turn(4,(unsigned char*)&itm);
	memcpy(JTB_1ER.eYuE,(unsigned char*)&itm,4);//���
	memcpy(JTB_1ER.eDealTime,(unsigned char*)&SysTime,7);
	memcpy(JTB_1ER.eCityco,jtb17file.Citycode,2);
	memcpy(JTB_1ER.eFactoryco,jtb15file.cPublisherSign,8);
#ifdef FengDuan_BU_
	memcpy(JTB_1ER.eTimeMax,gFdLinehead.NeedMaxTime,2);//�ʱ��
#endif
	
	itm=5;
	over_turn(4,(unsigned char*)&itm);
	memcpy(JTB_1ER.eErrPriceMax,(unsigned char*)&itm,4);				//������ѽ��HEX(���)
	//������ѽ��
	memcpy(Rvb,(unsigned char*)&JTB_1ER,48);
#ifdef _debug_
	debugstring("Build_1E\r\n");
	debugdata(Rvb,48,1);
#endif
}
#ifdef FengDuan_BU_
unsigned int CouCpu_Sum(void)
{
//	unsigned char cbuff[16];
//	int  t;
	unsigned char zdbh, czdbh;
	//���϶������
	if(dealtype==1||dealtype==3){//�ϳ�
		s_sum1=0;//��ʼ��0������
	}
	else if (dealtype==2||dealtype==4)
	{
#if 1
		MSG_LOG("�³�");
		zdbh = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;	//��1����Ϊ��Ŷ��Ǵ�1��ʼ�ġ�
		if(gFDderict == 0){//����
			czdbh = LineInfo[0].StationInfo[JTB_1A.rStationOn-1].ucId;
		}
		else{//����
			czdbh = LineInfo[0].StationInfo[JTB_1A.rStationOn-1].ucId;
		}
		//��������·��Ϣ����������BCD�롣
		zdbh = (zdbh>>4)*10+(zdbh&0x0F);
		czdbh = (czdbh>>4)*10+(czdbh&0x0F);
		s_sum1 = get2stationPrice(zdbh, czdbh);
#endif

#ifdef _debug_
		s_sum1=1;
#endif
	}
	else if(dealtype==8){//��Ʊ
		//��Ʊ
		memcpy((unsigned char*)&s_sum1,JTB_1A.rPriceMax,4);//��ע���
		over_turn(4,(unsigned char*)&s_sum1);
	}else{
#ifdef _debug_
		return 1;
#endif
		return 0xFFFF;
	}
	return s_sum1;
}
#endif
/*
unsigned char CpuBlackMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0xFA;
	
	Scmd.P1 = 0x05;//0x01;//
	
	Scmd.P2 = 0x00;
	Scmd.LC = len;
	
	//memset(Scmd.DATA, 0, 0x24);
	//Scmd.DATA[10] = 0x05;
	//Scmd.DATA[18] = 0x01;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_
	debugstring("CpuCardSetMac scmd:");
	debugdata(pData, len, 1);
#endif
	IccCommand(PsamID, &Scmd, &Rcmd);
#ifdef _debug_
	debugstring("CpuCardSetMac rcmd:");
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
unsigned char CpuCardSetMac2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0x1A;
	
	Scmd.P1 = 0x45;//0x01;//
	
	Scmd.P2 = 0x02;
	Scmd.LC = len;
	
	//memset(Scmd.DATA, 0, 0x24);
	//Scmd.DATA[10] = 0x05;
	//Scmd.DATA[18] = 0x01;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_
	debugstring("MAC=scmd:");
	debugdata(&Scmd.CLA, len+5, 1);
#endif
	IccCommand(PsamID, &Scmd, &Rcmd);
#ifdef _debug_
	debugstring("CpuCardSetMac rcmd:");
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
*/
extern unsigned char PsamNo[6];
extern unsigned char CpuCardSetMac2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuBlackMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern void CountMac_CUP(unsigned char *Key,unsigned char *random,unsigned char *Sdat,unsigned int len ,unsigned char *Result);
//extern void Mac(unsigned char *Key,unsigned char *pInData1,unsigned char *pInData2,int data2len,unsigned char *initData, unsigned char *mac);
unsigned char  operat_Black(void)
{
	
	unsigned char i;
	unsigned char ret;
	unsigned char revbuf[256];
	unsigned char sndbuf[56];
	unsigned char Rader[10];


	i=13;
	memcpy(sndbuf, "\x00\xA4\x04\x00\x08\xA0\x00\x00\x06\x32\x01\x01\x05", i);		//JTB����Ǯ��AID
	//memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//�д˿���Ҫ���ļ���������ȡ����ʱ��Ҫ����һ�¡�
//	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
	if(ret == 0)
		return ST_ERROR;

	memcpy(sndbuf, "\02\x00\x84\x00\x00\x08", 6); i = 6;
	deal2or3(sndbuf);	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_
	debugstring("�����:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return 4;
	else{
		if(memcmp(revbuf+1, "\x94\x01", 2)==0)
			return 3;
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}
	memset(Rader,0,10);
	memcpy(Rader,revbuf+1,4);


//	memcpy(sndbuf,stuPubInfZaoZuang.cPublisherSign,8);
	memcpy(sndbuf,cardMessage.PublishBicker+2,8);//���ź�8�ֽ�
	i =8;
	memcpy(sndbuf+i,cardMessage.FactorFlg,8);//������������
	i+=8;
	//memcpy(sndbuf+i,cardMessage.CityCode,2);i+=2;
	//memcpy(sndbuf+i,"\xFF\x00\x00\x00\x00\x00",6);i+=6;
		
#ifdef _debug_CPU_
	debugstring("PSAM_MAC��ʼ��ָ��:");
	debugdata((unsigned char *)&i,4,1);
	debugdata(sndbuf,i,1);
#endif
	if (CpuCardSetMac2(PSAM_JTB,revbuf,sndbuf,i) == ST_ERROR){	//801A450110
		return ST_ERROR;
	}

	memcpy(sndbuf,Rader,4);i=8;//��Կ
	memcpy(sndbuf+i,"\x84\x1E\x00\x00\x04\x80\x00\x00",8);i +=8;
	
	if(CpuBlackMac(PSAM_JTB,revbuf,sndbuf,i)==ST_ERROR)			//80FA050010
		return ST_ERROR;
#ifdef _debug_
	debugstring("MAC1:");
	debugdata(revbuf,10,1);
#endif	
	//if(CpuBlackMac(gPSAMin,revbuf,sndbuf,i)==ST_ERROR)
	//	return ST_ERROR;
	//CountMAC();

	memcpy(sndbuf, "\02\x84\x1e\x00\x00\x04", 6); i = 6;
	memcpy(sndbuf+i,revbuf,4);i+=4;//MAC
	deal2or3(sndbuf);
	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_
	debugstring("����:");
	debugdata(&ret,1,1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return 4;
	else{
		if(memcmp(revbuf+1, "\x94\x01", 2)==0)
			return 3;
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}
//	memcpy(sndbuf, "\x02\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 15); // ѡ��3F01
	memcpy(sndbuf, "\x00\xA4\x04\x00\x08\xA0\x00\x00\x06\x32\x01\x01\x05", i);		//JTB����Ǯ��AID
//	deal2or3(sndbuf);
#ifdef _debug_CPU_
	debugstring("select AID1:");
	debugdata(sndbuf, 15, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, 15);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	BuildRecorde(ID_REC_BLKLIST,revbuf);
	WriteRecord(revbuf, 0);
//	WriRecorJTB(revbuf);
//	WriRecorJTB(revbuf,0);
	return ST_OK;
}
//mode==1  д�����磬mode==2ɾ�������¼  3 ����Ƿ��м�¼��Ҫת��      �������δ���  �ϵ�û��¼�����
void Set_UnknowREC(unsigned char mode,unsigned char *data){

	//FeUnKnowREC buffer;
	unsigned int CRC;
	unsigned char i;

	switch(mode)
	{
		case FeRC_Write://д��¼
			memcpy(FeuRec.falg,"��Ч",4);
			memcpy(FeuRec.RCE,data,RECORD_JTB_LEN);
			CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
			memcpy(FeuRec.CRC,(unsigned char*)&CRC,4);
			sysfewrite(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
			MSG_LOG("д�����¼\r\n ");
			BCD_LOG(FeuRec.falg,sizeof(FeUnKnowREC),1);
			break;
		case FeRC_Dlelt://ɾ��
			memset((unsigned char*)&FeuRec,0,sizeof(FeUnKnowREC));
			CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
			memcpy(FeuRec.CRC,(unsigned char*)&CRC,4);
			sysfewrite(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
			MSG_LOG("ɾ�������¼\r\n ");
			BCD_LOG(FeuRec.falg,sizeof(FeUnKnowREC),1);
			break;
		case FeRC_Check://�鿴�Ƿ��м�¼
		
			for(i=0;i<5;i++)
			{
				sysferead(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
				CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
				if(memcmp((unsigned char*)&CRC,FeuRec.CRC,4)==0)//��ȷ
					break;
			}
			
			MSG_LOG("FeuREC:\r\n");
			BCD_LOG((unsigned char*)&i,1,1);
			BCD_LOG(FeuRec.falg,136,1);

			if(memcmp(FeuRec.falg,"��Ч",4)==0){//�м�¼
				WriteRecord(FeuRec.RCE,0);
				//	WriRecorJTB(FeuRec.RCE);		//д����¼
				//ת�������
				memset((unsigned char*)&FeuRec,0,sizeof(FeUnKnowREC));
				CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
				memcpy(FeuRec.CRC,(unsigned char*)&CRC,4);
				sysfewrite(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
				MSG_LOG("��¼ת��\r\n");
				return;
			}
			MSG_LOG("û�м�¼\r\n");
			break;
		default:
			break;
	}

	/*
	if(mode==FeRC_Write){
		memcpy(FeuRec.falg,"��Ч",4);
		memcpy(FeuRec.RCE,data,RECORD_JTB_LEN);
		CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
		memcpy(FeuRec.CRC,(unsigned char*)&CRC,4);
		sysfewrite(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
		MSG_LOG("д�����¼\r\n ");
		BCD_LOG(FeuRec.falg,sizeof(FeUnKnowREC),1);
	}
	else if (mode==FeRC_Dlelt)//ɾ��
	{
		
		memset((unsigned char*)&FeuRec,0,sizeof(FeUnKnowREC));
		CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
		memcpy(FeuRec.CRC,(unsigned char*)&CRC,4);
		sysfewrite(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
		MSG_LOG("ɾ�������¼\r\n ");
		BCD_LOG(FeuRec.falg,sizeof(FeUnKnowREC),1);
	}else if (mode==FeRC_Check)//�ж��Ƿ��м�¼��Ҫת��
	{
		
		for(i=0;i<5;i++)
		{
			sysferead(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
			CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
			if(memcmp((unsigned char*)&CRC,FeuRec.CRC,4)==0)//��ȷ
				break;
		}

		MSG_LOG("FeuREC:\r\n");
		BCD_LOG((unsigned char*)&i,1,1);
		BCD_LOG(FeuRec.falg,136,1);

		if(memcmp(FeuRec.falg,"��Ч",4)==0){//�м�¼
			WriRecorJTB(FeuRec.RCE,0);
		//	WriRecorJTB(FeuRec.RCE);		//д����¼
		//ת�������
		memset((unsigned char*)&FeuRec,0,sizeof(FeUnKnowREC));
		CRC=__cpuCrc32(FeuRec.falg,sizeof(FeUnKnowREC)-4);
		memcpy(FeuRec.CRC,(unsigned char*)&CRC,4);
		sysfewrite(BIT_UNKNOW_RECORD,sizeof(FeUnKnowREC),(unsigned char*)&FeuRec);
		MSG_LOG("��¼ת��\r\n");
		return;
		}
		MSG_LOG("û�м�¼\r\n");
	}
		*/

}





extern void Save_delayCardinfo(unsigned char Cmode);
unsigned char CPUDealCard_JTB(unsigned char mode, unsigned char cool)
{
	unsigned int uitemp1;
	unsigned char i;
	unsigned char sndbuf[250];
	unsigned char revbuf[200];
	unsigned char TACC[4]={0};
	//unsigned char PSAMDsno[4];
	unsigned int revLen,ret;
	unsigned int value;
	unsigned short ii;
	//Recode_ *rp=0;
	unsigned int uitemp=0;
	unsigned char temrev[10];
#ifdef FengDuan_BU_	
	unsigned short toall;
	unsigned char disbuf[20];
#endif

	BCD_LOG((unsigned char *)cardMessage.card_catalog,1,1);	
#ifdef _debug_CPU_
	debugstring("CPUDealCard====JTB====\r\n");
#endif
	gMCardCand= CARDSTYLE_JTB;
	if(F_FLAG_VARY.stop_flag==1) 
		return ST_ERROR;

//	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
//	{
//		disp_no_swipe();
//		return ST_OK;	
//	}

	if (CPU_Read_Money((unsigned char*)&value, 2) != 0){
		MessageBox(1,"��������");
		beep(2,50,50);
		g24GDisFlash=5;
		restore_flag=0;
		return ST_ERROR;
	}
	over_turn(4, (unsigned char*)&value);
	a_sum1 = value;		//���
#ifdef FengDuan_BU_
	memcpy((unsigned char*)&toall,gFdLinehead.TottalPrice,2);
	if(gFDMode == FDMODE_NORMAL&&JTB_1A.rPursState!=1){//����ֻ���³�
		MessageBox(0,"�뵽ǰ���ϳ�");
		end_close_card(1);
		restore_flag=3;
		return ST_ERROR;
	}
	else if (gFDMode==FDMODE_GPS)//�����빻����ȫ������
	{
		if(value<toall){
			MessageBox(0,"����");
			memset(disbuf,0,20);
			sprintf((char*)disbuf,"���:%d,%02dԪ",value/100,value%100);
			display(4,0,(char*)disbuf,DIS_CENTER);
			end_close_card(1);
			restore_flag=3;
			return ST_ERROR;
		}	//�ڳ�վ������ˢ�˷�����  �������ϳ���
		else if((BUS_Flag&BUS_Star)&&(BUS_Flag&BUS_InStation)){
			MessageBox(0,"�ѷ���");
			display(4,0,"����ˢ��!",DIS_CENTER);
			end_close_card(1);
			restore_flag=3;
			return ST_ERROR;
		}
	}

#endif

// 	if(cardSound == 0xaa){
// 		if(card_ser!=c_serial){
// 			Set_UnknowREC(FeRC_Check,revbuf);//��ˢ����Ŀ�������,Ҫ����һ�����ļ�¼��������
// 			card_ser = 0;
// 			cardSound = 0;
// 		}
// 	}

/*
	memcpy(sndbuf,"\x00\xB2\x01\xC4\x17",5);//ȡ�ϱʼ�¼ 18�ļ�
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
#ifdef _debug_CPU_
	debugstring("ȡ18��¼�ļ�----\r\nsnd:\r\n");
	debugdata(sndbuf, 5, 1);
	debugdata(revbuf, ret, 1);
#endif
	
	
	memcpy(sndbuf, "\x02\x80\x5A\x00\x09\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
	memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
	over_turn(2, (unsigned char*)&ii);
	ii += 1;
	over_turn(2, (unsigned char*)&ii);
	memcpy(sndbuf+6, (unsigned char*)&ii, 2);
	sndbuf[8] = 8;
#ifdef _debug_CPU_
	display(0,0,"�Ű�",0);
	delayxms(500);
	beep(3,50,50);
	debugstring("����:");
	debugdata(stuInitPurchaseRet.cSnq, 2, 1);
	debugdata(sndbuf, 9, 1);
#endif
	deal2or3(sndbuf);
	revLen = RfPassthrough(revbuf, 2, sndbuf, 9);
	// 	if(revLen == 0)
	// 		return ST_ERROR;
#ifdef _debug_CPU_
	debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
*/


	if(cardSound == 0xaa){//��Ҫ��ˢ
		if(value < a_sumR){//�ϴ��Ѿ��۳ɹ�
			memcpy(sndbuf, "\x02\x80\x5A\x00\x09\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(sndbuf+6, (unsigned char*)&ii, 2);
			sndbuf[8] = 8;
#if defined _debug_CPU_ || defined _debug_tryagain
			display(0,0,"�Ű�",0);
			delayxms(500);
			beep(3,50,50);
			debugstring("����:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(sndbuf, 9, 1);
#endif
			deal2or3(sndbuf);
			revLen = RfPassthrough(revbuf, 2, sndbuf, 9);
			if(revLen == 0)
				return ST_ERROR;
#if defined _debug_CPU_ || defined _debug_tryagain
			debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(revLen == 0)//���س�ʱ,����
				return ST_ERROR;
			if((memcmp(revbuf+revLen-2, "\x90\x00", 2)!=0) &&\
				(memcmp(revbuf+revLen-2, "\x6C\x08", 2)!=0))
				goto CPUMAC2deal;//ȡ���ɹ�02 69 85 
			
			memcpy(TACC, revbuf+revLen-6, 4);//���ص�TAC��
			memcpy(sndbuf, revbuf+revLen-10, 4);//��MAC2��

			
			//�����������Ʊ����M1����ȥ��һ�Σ���Щ���������Ѿ����ˣ�Ϊ�˷�ֹд��¼�����⣬����ȡֵ
			//��ʵǰ����ˢ���ƺ��ˣ���ôû�б�ҪҪ��
		//	
		
			
			goto CPUMAC2deal0;
		}	

	}
	a_sumR = value;		//��֮ǰ���
	//8050 0102 0B||��Կ��ʶ�� [1]��0x01��||���׽��[4](0x00000001)||�ն˻����[6]
	//8050 0302 0B  ��������
	i=5;
	if(mode!=MONTH_CARD)
		memcpy(sndbuf, "\x80\x50\x03\x02\x0B", i);//11 INITIALIZE FOR PURCHASE����
	else
		memcpy(sndbuf, "\x80\x50\x01\x02\x0B", i);//11 INITIALIZE FOR PURCHASE����
	sndbuf[i++] = SamPubInf_LLJTB.kid;	//��Կ��


	if(mode== MONEY_CARD){//
		s_sum1 = get_s_sum1(0);//ȡ�����ѽ��
		if((s_sum1 == 0) || (s_sum1 > 9900))
			return 22;
		
	}else if (mode == FREE_CARD)
	{
		s_sum1 = 0;
	}else
	{
		s_sum1=1;
	}

	



	MSG_LOG("���ѽ��:%d,���:%d\r\n",s_sum1,a_sum1);
	
	if(s_sum1 > 9900||a_sum1>500000)
		return 22;


	uitemp = s_sum1;
	over_turn(4, (unsigned char*)&uitemp);
	memcpy(sndbuf+i, (unsigned char*)&uitemp, 4); i+=4;
	memcpy(sndbuf+i, PsamNumJTB, 6); i += 6;
#ifdef _debug_CPU_
	debugstring("*** init for purchase:");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0){
		ERR_DIS("ERROR 9",revbuf);
		return ST_ERROR;
	}
	else{
		if(memcmp(revbuf+ret-2, "\x94\x01", 2)==0){
			if(cool == 4)//��Ʊ��ˢ����������ʾ����
				return 4;
			else
				return 3;			//����
		}
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
			ERR_DIS("ERROR 9",revbuf+ret-2);
			return 5;
		}
	}	
	memcpy((unsigned char*)&stuInitPurchaseRet, revbuf+1, sizeof(stuInitPurchaseRet));
#ifdef _debug_CPU_
	debugstring("8050ָ���:::\r\n");
	debugdata((unsigned char*)&stuInitPurchaseRet, sizeof(stuInitPurchaseRet), 1);
	debugstring("�����::");
	debugdata(stuInitPurchaseRet.cBalance,4,1);
	debugstring("��Կ�汾���㷨��ʶ:");
	debugdata(&stuInitPurchaseRet.cKeyVer,2,1);
#endif
	//�ж�͸֧���
	uitemp1=0;
	memcpy((unsigned char*)&uitemp1,stuInitPurchaseRet.cOverdraftLimit,3);
	over_turn(3,(unsigned char*)&uitemp1);
	if(a_sum1<=uitemp1)//�ܾ�����
		return 3;


	if(GetDateTime())
		return ST_ERROR;
	memcpy(stuSamInitPurchaseJTB.cDateTime,(unsigned char*)&SysTime,7);
	memcpy(stuSamInitPurchaseJTB.cRand,stuInitPurchaseRet.cRand,4);
	memcpy(stuSamInitPurchaseJTB.cSnq,stuInitPurchaseRet.cSnq,2);
	memcpy(stuSamInitPurchaseJTB.cMoney,(unsigned char*)&uitemp,4);
	if(mode!=MONTH_CARD)
	stuSamInitPurchaseJTB.cType=0x09;//��������
	else
	stuSamInitPurchaseJTB.cType=0x06;//��������
	stuSamInitPurchaseJTB.cKeyVer=stuInitPurchaseRet.cKeyVer;
	stuSamInitPurchaseJTB.cKeyEn=stuInitPurchaseRet.cKeyEn;

	memcpy(stuSamInitPurchaseJTB.cPublisherSign,cardMessage.PublishBicker+2,8);//���ź�8�ֽ�
	memcpy(stuSamInitPurchaseJTB.cKeyFactor,cardMessage.FactorFlg,8);//������������
	
	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);//���
	over_turn(4, (unsigned char*)&a_sum1);
	
#ifdef _debug_CPU_
	MSG_LOG("a_sum:%d.%dԪ\r\n",a_sum1/100,a_sum1%100);
#endif

 #ifdef _debug_CPU_
	debugstring("JTB MAC1����ָ��:");
	debugdata(cardMessage.PublishBicker,10,1);
	debugdata(cardMessage.FactorFlg,8,1);
	debugdata((unsigned char*)&stuSamInitPurchaseJTB,sizeof(stuSamInitPurchaseJTB), 1);//+12
 #endif	

		if(CpuCardSetMac(PSAM_JTB,revbuf, (unsigned char*)&stuSamInitPurchaseJTB,0x24) == ST_ERROR)
		{
			return 6;
		}

#ifdef _debug_CPU_
	debugstring("MAC OK!:\r\n");
	debugstring("8070ָ���:::\r\n");
	debugdata(revbuf, 9, 1);
#endif	
		memcpy(temrev,revbuf,8);//�ն����к�+MAC
		memcpy(stuDebitPurchase.cTermSnq,revbuf,4);//�ն˽�����ˮ
		memcpy(stuDebitPurchase.cDateTime,stuSamInitPurchaseJTB.cDateTime,7);
#ifdef _debug_CPU_
		debugstring("stuDebitPurchase\r\n");
		debugdata(stuDebitPurchase.cTermSnq,15,1);
#endif

#ifdef FengDuan_BU_
 		if(CardType==LocakCard)//���ؿ�
		{
			//����0X1A�ļ�80DC 02 D0 80 2702 7D
			memcpy(sndbuf,"\x80\xDC\x02\xC8\x80",5);
			//�ۿ�
			i=5;
			Build_1A_Rec(sndbuf+i,dealtype);//��֯1A�ļ�
			i+=128;
			ret = RfPassthrough(revbuf, 2, sndbuf, i);
			
#ifdef _debug_CPU_
			debugstring("����19:");
			debugdata(sndbuf, i, 1);
			debugdata(revbuf, 10, 1);
#endif
			if(ret==0){
				ERR_DIS("EP ERRO A3",revbuf);
				return ST_ERROR;
			}
			if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
				ERR_DIS("ERROR A3",revbuf+ret-2);
				return ST_ERROR;
			}

		}
		else{//��ؿ�
			//UP_ECH:
			//����0X1A�ļ�80DC 02 D0 80 2702 7D
			memcpy(sndbuf,"\x80\xDC\x02\xD0\x80",5);
			//�ۿ�
			i=5;
			Build_1A_Rec(sndbuf+i,dealtype);//��֯1A�ļ�
			i+=128;
			ret = RfPassthrough(revbuf, 2, sndbuf, i);
			
#ifdef _debug_CPU_
			debugstring("����1A:");
			debugdata(sndbuf, i, 1);
			debugdata(revbuf, 10, 1);
#endif
			if(ret==0){
				ERR_DIS("ERROR A2",revbuf);
				return ST_ERROR;
			}
			if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
				ERR_DIS("ERROR A2",revbuf+ret-2);
				return ST_ERROR;
			}

		}
#endif//FengDuan_BU_

	if(mode!=MONTH_CARD)//��Ʊ����Ҫ����
	{
		//����0X1E�ļ�80DC 00 F0 
		memcpy(sndbuf,"\x80\xDC\x00\xF0\x30",5);
		//�ۿ�
		i=5;
		Build_1E_Rec(sndbuf+i,dealtype);//��֯1E�ļ�
		i+=48;
		ret = RfPassthrough(revbuf, 2, sndbuf, i);
		
#ifdef _debug_CPU_
		debugstring("����1E:");
		debugdata(sndbuf, i, 1);
		debugdata(revbuf, 10, 1);
#endif
		if(ret==0){
			ERR_DIS("ERROR A1",revbuf);
			return ST_ERROR;
		}
		if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
			ERR_DIS("ERROR A1",revbuf+ret-2);
			 return ST_ERROR;
		}
	}
		BuildRecorde(ID_REC_HJY_REC, revbuf);//�����ɼ�¼
		Set_UnknowREC(FeRC_Write,revbuf);//д������
		//*****************************
		i=5;
		memcpy(sndbuf, "\x80\x54\x01\x00\x0F", i);//11 INITIALIZE FOR PURCHASE����
		memcpy(sndbuf+i,temrev,4);//�ն˽������к�
		i+=4;
		memcpy(sndbuf+i,stuSamInitPurchaseJTB.cDateTime,7);
		i+=7;
		memcpy(sndbuf+i,temrev+4,4);//MAC1
		i+=4;	
		ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
		debugstring("�۷�!:");
		debugdata(sndbuf, i, 1);
		debugdata(revbuf, ret, 1);
#endif
		if(ret==0){
			cardSound = 0xaa;//������Ҫ��ˢ��
			card_ser = c_serial;
			audio(Audio_TRY_AGAIN);
			return 7;
		}
		if(memcmp(revbuf+ret-2, "\x93\x02", 2)==0)
			return 9;//�Ƿ���
		if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
				//audio(Audio_TRY_AGAIN);
		//	cardSound = 0xaa;//������Ҫ��ˢ��
		//	card_ser = c_serial;
			return ST_ERROR;
		}			 
		memcpy(sndbuf,revbuf+1+4,4);//MAC2
		memcpy(stuDebitPurchase.cMac,revbuf+1,4);//���׵�MAC
		memcpy(cardMessage.PbocTac ,revbuf+1, 4);	// TAC��
		a_sum1-=s_sum1;

CPUMAC2deal0:
#ifdef _debug_CPU_
	debugstring("У��MAC2::\r\n");
#endif
// 		if(memcmp(jtb15file.cFCI,"\xEF\xDF",2)!=0)//���㷨�Ĳ��������
// 		{
			if(CPUMAC2(PSAM_JTB,revbuf, sndbuf, 4)==ST_ERROR)//У��MAC2
				ERR_DIS("ERROR A5",revbuf);
//		}

CPUMAC2deal:
// 	memcpy(cardMessage.PbocTac, TACC, 4);	// TAC��
// 	memcpy(jsb_recbuff.TAC,cardMessage.PbocTac,4);
	
#ifdef _debug_CPU_
	debugstring("����MAC2:");
	debugdata(revbuf, 10, 1);
#endif
	
	//	display(2,1,"���ѳɹ�",DIS_CENTER);
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
	//	Sound_cityCMD(0);
	SoundMessage(SOUND_DEAL);


	BuildRecorde(ID_REC_TOLL, revbuf);
	WriteRecord(revbuf,0);
//	BuildRecordeHistory(ID_REC_History,revbuf);
//	WriteRecord(revbuf,0);
	
	Set_UnknowREC(FeRC_Dlelt,revbuf);//ɾ������ļ�¼
#ifdef FengDuan_BU_
	if(dealtype==8){//��Ҫ��Ʊ������
		MSG_LOG("�������\r\n");
		return PursAgian;
	}
#endif

//	debugstring((char*)revbuf);


	card_ser = 0;
	cardSound = 0;
	Save_delayCardinfo(mode);
	gMCardCand=0;
	CardType = 0;
	

	return ST_OK;//���ѳɹ�
}
//���Ұ�����
unsigned char IsWriteNUM(unsigned int Write)
{
		unsigned char i;
		MSG_LOG("���Ұ�����\r\n");
	for(i=0; i<white_organization; i++){
		BCD_LOG((unsigned char*)&Write,4,1);
		BCD_LOG((unsigned char*)&WriteNUM[i],4,1);
		if(Write == WriteNUM[i])
			return ST_OK;
		}
	return ST_ERROR;
}
unsigned char IF_15_OK(unsigned char *Data){

//	unsigned int ret=0;
	unsigned short s1;
	unsigned int uitemp=0;
	unsigned int NowTime;
	unsigned int start_time=0,end_time=0;
	
//	unsigned char i=0;
	//	unsigned int value;	
//	unsigned char sndbuf[256];
//	unsigned char revbuf[150];
//	unsigned int temp;
	unsigned char disbuf[2];
	memcpy(cardMessage.PublishBicker, jtb15file.cApp_sn,10);
	memcpy((unsigned char*)&NowTime, (unsigned char *)&SysTime, 4);//��ǰ����
	over_turn(4, (unsigned char*)&NowTime);
	memcpy((unsigned char*)&start_time, jtb15file.cIssueDate, 4);//��������
	over_turn(4, (unsigned char*)&start_time);
	memcpy((unsigned char*)&end_time, jtb15file.cValidDate, 4);//��Ч����
	over_turn(4, (unsigned char*)&end_time);
	memcpy(cardMessage.FactorFlg,jtb15file.cPublisherSign,8);
	memset(disbuf,0xff,2);
	if(jtb15file.cStarFlag==0||jtb15file.cStarFlag==2){//0 δ���ã�1 ���� 2 ͣ��
		disbuf[0]=0;
		disbuf[1]=jtb15file.cStarFlag;
			ERR_DIS("ERROR 2.1",disbuf);
		return ST_ERROR;//δ����
	}
	//�жϸ��˻����ñ�ʶ
	if (jtb15file.cAppType!=02)//���˻�Ӧ�ñ�ʶ��
	{
		disbuf[1]=jtb15file.cAppType;
		disbuf[0]=0x00;
		ERR_DIS("ERROR 2.2",disbuf);
		return ST_ERROR;//δ����
	}
	uitemp=BCD2int((unsigned char*)&start_time,4);
	//�ж�����ʱ��
	if(memcmp((unsigned char*)&start_time,"\x00\x00\x00\x00",4)==0||memcmp((unsigned char*)&start_time,"\xFF\xFF\xFF\xFF",4)==0||memcmp((unsigned char*)&end_time,"\x00\x00\x00\x00",4)==0||memcmp((unsigned char*)&end_time,"\xFF\xFF\xFF\xFF",4)==0)
	{	memcpy(disbuf,(unsigned char*)&start_time,2);
		ERR_DIS("ERROR 2.3",disbuf);
		 return ST_ERROR;
	}
	//�������������ж�
	if(((uitemp%4==0)&&(uitemp%100!=0))||(uitemp%400==0))  
	{
		//����
		if(memcmp((unsigned char*)&start_time,"\x29",1)>0&&memcmp((unsigned char*)&start_time+1,"\x02",1)==0){
			//MessageBox(1,"�������ڸ�ʽ����");
			memcpy(disbuf,(unsigned char*)&start_time,2);
			MSG_LOG("����");
			ERR_DIS("ERROR 2.3",disbuf);
			return ST_ERROR;
		}
	}
	else{
		//ƽ��
		if(memcmp((unsigned char*)&start_time,"\x28",1)>0&&memcmp((unsigned char*)&start_time+1,"\x02",1)==0){
			MSG_LOG("ƽ��");
			MessageBox(1,"ERROR 2.3");
			return ST_ERROR;
		}
		
	}
	//���������ж�
	uitemp=BCD2int((unsigned char*)&end_time,4);
	//��Ч����
	if(((uitemp%4==0)&&(uitemp%100!=0))||(uitemp%400==0))  
	{
		//����
		if(memcmp((unsigned char*)&end_time,"\x29",1)>0&&memcmp((unsigned char*)&end_time+1,"\x02",1)==0){
			MessageBox(1,"ERROR 2.4");
			return ST_ERROR;
		}
	}
	else{
		//ƽ��
		if(memcmp((unsigned char*)&end_time,"\x28",1)>0&&memcmp((unsigned char*)&end_time+1,"\x02",1)==0){
			
			MessageBox(1,"ERROR 2.4");
			return ST_ERROR;
		}
		
	}

	//�ж��Ƿ����
	if((start_time>NowTime)||(NowTime > end_time)){//����ʱ���ж� ����ҲҪͨ��

		return CARD_NO_TIME;//���ڻ�ĩ����
	}

	
#ifdef BUS_GUANGXI
	//��������ʱͨ������������˳�������ж�
	s1 = 0;
	memcpy((unsigned char*)&s1, cardMessage.FactorFlg, 2);
	over_turn(2, (unsigned char*)&s1);
	//if(memcmp(cardMessage.FactorFlg,Factory_NUM,4)==0)//���ؿ�
	if((s1>=0x0402)&&(s1<=0x0415))//�����Ŀ�
	{
		CardType=LocakCard;//���ؿ� 
		MSG_LOG("���ؿ�\r\n");
	}
	else{
		CardType=YiDiCard;//��ؿ�
		MSG_LOG("��ؿ�\r\n");
	}
#else
 	if(memcmp(cardMessage.FactorFlg,Factory_NUM,4)==0)//���ؿ�
	{
 		CardType=LocakCard;//���ؿ� 
		MSG_LOG("���ؿ�\r\n");
	}
	else{
 		CardType=YiDiCard;//��ؿ�
		MSG_LOG("��ؿ�\r\n");
	}
#endif
	

	//CardType=YiDiCard;//��ؿ�
#if 0
	//�жϿ��Ļ��������Ƿ��ڰ�������
	memcpy((unsigned char*)&temp,cardMessage.FactorFlg,4);//ȡ�������������ǰ�ĸ��ֽ�
	over_turn(4,(unsigned char*)&temp);
	if(IsWriteNUM(temp))
			return NO_WriteNUM;//���ڰ�������

	if(IsInWhileList(cardMessage.FactorFlg)!=ST_ERROR)//��
		return NO_WriteNUM;//���ڰ�������

#endif


	return ST_OK;
}
#ifdef FengDuan_BU_
unsigned char IF_1A_OK(unsigned char *Data){

//	unsigned int ret=0;
//	unsigned char i=0;
//	unsigned int temp;
	unsigned char disbuf[2];
	unsigned int temp;
	unsigned short i;
#ifdef _debug_CPU_
	debugstring("JTB_1A\r\n");
	debugdata((unsigned char*)&JTB_1A,128,1);
#endif
	memset(disbuf,0,2);
	//1A�ļ�
	if(memcmp(JTB_1A.rID,"\x27\x02",2)!=0){//�ж�1A
		memcpy(disbuf,JTB_1A.rID,2);
		ERR_DIS("ERROR 7.1",disbuf);
	//	return ST_ERROR;
	}
	if(JTB_1A.rlen!=0x7D){//
		disbuf[1]=JTB_1A.rlen;
		ERR_DIS("EP_ERRO 7.2",disbuf);
		return ST_ERROR;
	}
	if(JTB_1A.rOk!=1){//
		disbuf[1]=JTB_1A.rOk;
		ERR_DIS("ERROR 7.3",disbuf);
		return ST_ERROR;
	}
	if(JTB_1A.rpursFlag!=1){
		disbuf[1]=JTB_1A.rpursFlag;
		ERR_DIS("ERROR 7.4",disbuf);
		return ST_ERROR;
	}
	if(JTB_1A.rLock!=0){
		disbuf[1]=JTB_1A.rLock;
		ERR_DIS("ERROR 7.5",disbuf);
		return ST_ERROR;
	}
	
	memcpy((unsigned char*)&temp,JTB_1A.rPriceMax,4);
//�жϿ۷ѷ�ʽ
	switch(JTB_1A.rPursState)
	{	

	case 1://δ��� ��Ҫ��Ʊ ��ؿ� 
		//�����³�ʱ��

		if(CardType!=LocakCard&&memcmp(JTB_1A.rCiytCodeY,"\x03\x10",2)!=0){//������Ǳ����ϳ���  ֱ�����ϳ�����
			dealtype=ON_BUS;//�ϳ�
			MSG_LOG("���EP�ϳ���");
		}
		else{//�����ϵĳ�
			memcpy((unsigned char*)&i, JTB_1A.rTimeMax, 2);//��������ʱ�䳤 ��λ�֡�
			i *= 60;
			if((DataTimeDec((unsigned char*)&SysTime, JTB_1A.rPursTimeOn)> i)&&temp!=0)//�����ʱ����
			{
				MSG_LOG("EP��Ʊ");
				dealtype=COVER_BUS;//EP��Ʊ
			}
			else{
				MSG_LOG("EP�³�");
				dealtype=OFF_BUS;//�³�
			}
		}
		break;
	case 2:
		dealtype=ON_BUS;//�ϳ�
		MSG_LOG("�ϳ�");
		break;
	default:
		dealtype=ON_BUS;//�ϳ�
		MSG_LOG("Ĭ���ϳ�");
		break;
		
	}
	
	return ST_OK;
}
#endif
unsigned char IF_1E_OK(unsigned char *Data)
{
//	unsigned short i;
	unsigned char disbuf[2];
//	unsigned int temp;

	//�ж�1E
	if(JTB_1E.eDealType>10){
		disbuf[0]=JTB_1E.eDealType;
		disbuf[1]=0;
		ERR_DIS("��1E�������ʹ�",disbuf);
		return ST_ERROR;
		}
//��������
#if 0
	if(JTB_1E.eDealType==1||JTB_1E.eDealType==3)//��δ��ɵ�����
	{

			MSG_LOG("EP��Ʊ");
			dealtype=COVER_BUS;//EP��Ʊ

	}else{
			dealtype=ON_Nomal;
	}
#endif//��ʱ����Ʊ

		dealtype=ON_Nomal;//ȥ����
		return ST_OK;
}

unsigned int IsWhiteBlock(unsigned char *c_card_no )
{
	unsigned int i_start=0;
	unsigned int i_end;
	unsigned int i_cur, addr;
	unsigned char i;
	unsigned char temp[WHITE_SNO_LEN];
	unsigned int invalid_point;
	unsigned char buffer[WHITE_SNO_LEN];
#ifdef _debug_white
	char disbuff[64];
#endif
	
	memcpy(temp,c_card_no,WHITE_SNO_LEN);
	
	i_end = WhiteLength -1;
	
#ifdef _debug_white
	debugstring("White==  current Card NO.:");
	debugdata(temp,WHITE_SNO_LEN,1);
	debugstring("\r\n");
	debugdata((unsigned char *)&i_end,4,1);
#endif 
	
	while(1)
	{
		i_cur=i_start+(i_end-i_start)/WHITE_SNO_LEN;
		invalid_point=i_cur*WHITE_SNO_LEN;			//
	
		addr = FLASH_WHITE_START + 6 +invalid_point;//
		
		
#ifdef _debug_white
// 		debugstring("invalid_point:");
// 		debugdata((unsigned char *)&invalid_point, 4, 1);
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"start:%d\r\n",i_start);
		debugstring(disbuff);
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"i_cur:%d\r\n",i_cur);
		debugstring(disbuff);
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"i_end:%d\r\n",i_end);
		debugstring(disbuff);
#endif
		flashread(addr, buffer, WHITE_SNO_LEN);
#ifdef _debug_white
		debugstring("read:");
		debugdata(buffer,WHITE_SNO_LEN,1);
#endif
		for(i=0;i<WHITE_SNO_LEN;i++)
		{
			if(temp[i]==buffer[i]){
#ifdef _debug_white
				debugstring("Y");
#endif
				continue;
			}
			else if(temp[i]>buffer[i])

			{
				if(i_end==i_cur)
					return 0xffffffff;
				else
				{
					i_start=i_cur+1;
#ifdef _debug_white
					debugstring("����---\r\n");
#endif

					break;
				}
			}
			else
			{
				if(i_start==i_cur)
					return 0xffffffff;
				else
				{
#ifdef _debug_white
					debugstring("��ǰ---\r\n");
#endif

					i_end=i_cur;
					break;
				}
			}
		}
		if(i==WHITE_SNO_LEN){
			return addr;
		}
	}
}
//0 ����   1 ��
unsigned int IsInWhileList(unsigned char *c_card_no)
{
//	unsigned char i;
	unsigned int addr;

#ifdef _debug_CPU_
	debugstring("city_code::");
	debugdata(c_card_no,2,1);
	debugstring("WhiteLength:");
	debugdata((unsigned char *)&WhiteLength,2,1);
#endif
	
	if (memcmp(c_card_no,Factory_NUM,WHITE_SNO_LEN) == 0)
	{
		return 1;
	}else{
		if (WhiteLength == 0)
			return 0;

		addr = IsWhiteBlock(c_card_no);
#ifdef _debug_white
		debugstring("addr:::");
		debugdata((unsigned char *)&addr,4,1);
#endif
		if(addr < FLASH_WHITE_END)
			return 1;
	}	
	
	return 0;
}
extern void file_15_line(unsigned char *file_15);
extern void set_line(void);
void set_line_cpu(unsigned char mode)
{
	//	unsigned char i,uctemp;
	unsigned char sndbuf[100];
	unsigned char revbuf[256];
	unsigned char ret;
#ifdef _debug_CPU_
	//	char disbuff[64];
#endif	
	
	
	cls();
	if(gchn_eng == 'E')
		display(4,3,"PLS wait... ",0);
	else
		display(4,3,"���Ե�......",0);
	
		memcpy(sndbuf,"\x00\xB0\x83\x00\x80",5);//   ���貿�Ŀ���128����
#ifdef _debug_CPU_
		debugstring("��03��·�ļ�:\r\n");
		debugdata(sndbuf, 5, 1);
#endif
		ret = RfPassthrough(revbuf, 2, sndbuf, 5);
		
		if(ret == 0)
			return ;
		if(memcmp(revbuf+ret-2, "\x90\x00", 2) !=0){
			return;
		}
#ifdef _debug_CPU_
		debugdata((unsigned char*)&ret, 1, 1);
		debugdata(revbuf, ret, 1);
#endif
	file_15_line(revbuf+1);
	
	restore_flag=1;
	display(2,3,"��·��",DIS_CENTER | DIS_Cls);
	display(4,3,"�������!",DIS_CENTER);
	
#ifdef _debug_
	debugstring("LineNo2:");
	debugdata(pFistVary.LineNo,3,1);
#endif
	
	a_sum1 = 0;
	s_sum1 = 0;
	beep(1, 100, 100);
	SoundMessage(SOUND_DI_BUS);
	
	memset(revbuf,0,sizeof(revbuf));
	
// 	memset(&jsb_recbuff,0,sizeof(jsb_rec));
// 	memcpy(jsb_recbuff.deal_time,&SysTime.year_h,7);
	BuildRecorde(ID_REC_LINENO, revbuf);
#ifdef _debug_
	debugstring("LineNo3:");
	debugdata(pFistVary.LineNo,3,1);
#endif
	WriteRecord(revbuf, 0);
	//#endif	//#ifdef BUS_HANDAN_
#ifdef _debug_
	debugstring("LineNo4:");
	debugdata(pFistVary.LineNo,3,1);
#endif
	//	ControlDis(cardMessage.card_catalog);
#ifdef _debug_
	debugstring("Set line END!!!\r\n");
#endif
	
	end_close_card(1);
	
}
//00��ء�01����
unsigned char CPUcardType_JTB(unsigned char mode)
{
	unsigned int ret=0;
	unsigned int uitemp=0;
	unsigned int start_time=0,end_time=0;

	unsigned char i=0;
//	unsigned int value;	
	unsigned char sndbuf[256];
	unsigned char revbuf[150];

	MSG_LOG("Cpu_JTB----\r\n");

#ifndef BUS_MZT 
	memcpy(sndbuf,"\xe0\x50",2);
	ret = RfPassthrough(revbuf, 2, sndbuf, 2);
	mode2or3 = 2;
	delayxms(2);
#ifdef _debug_CPU_
	debugstring("CPU start!\r\n");
	debugdata(revbuf, ret, 1);
#endif
#endif


	i=19;
    memcpy(sndbuf,"\x00\xA4\x04\x00\x0E\x32\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31",i);//2PAY.SYS.DDF01
#ifdef _debug_CPU_
	debugstring("***select PPSE:2PAY.SYS.DDF01\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C:");
	debugdata(revbuf, ret, 1);
#endif
//	if(ret == 0)//û��Ҳ��
//		return ST_ERROR;

	i=13;
	memcpy(sndbuf, "\x00\xA4\x04\x00\x08\xA0\x00\x00\x06\x32\x01\x01\x05", i);		//JTB����Ǯ��AID
#ifdef _debug_CPU_
	debugstring("***select AID1:");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_//ѡ��Ӧ�÷��ص���������Դ
	debugstring("RCV_C:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0){
		//MessageBox(1,"ѡ��AIDʧ��");
	//	beep(2,50,50);
		g24GDisFlash=5;
		restore_flag=0;
		return NO_JTB_UNION;
	}
	
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
	{
		MSG_LOG("�Ҳ���Ӧ��----\r\n");
		//ERR_DIS("ERROR 1 ",revbuf+ret-2);
		return NO_JTB_UNION;
	}

	gMCardCand= CARDSTYLE_JTB;


	if(gPsamErrorFlag&2){
// 		cls();
// 		if(gchn_eng == 'E'){
// 			display(4, 0, "PSAM2 error!",0);
// 		}
// 		else{
// 			display(0,0,"����:",1);
// 			display(3,0,"��ͨ��PSAM����!",0);
// 		}
 		restore_flag = 3;
		MSG_LOG("û�н�ͨ��PSAM\r\n");
		return NO_JTB_UNION;
// 		delayxs(1);
// 		return ST_ERROR;
	}
	
	i=5;
	memcpy(sndbuf,"\x00\xB0\x95\x00\x1E",i);//����Ӧ����Ϣ�ļ� 30�ֽ�
#ifdef _debug_CPU_
	debugstring("***READ 95:\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C:");
	debugdata(revbuf, ret, 1);
#endif

	if(ret == 0){
	//	ERR_DIS("��1A�ļ�ʧ��",revbuf);
		return ST_ERROR;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
	{
		//ERR_DIS("ERROR 2",revbuf+ret-2);
		return ST_ERROR;
	}
	//	jtb15file = (stJTB15 *)(revbuf+1);
	memcpy((unsigned char*)&jtb15file,revbuf+1,30);
	memcpy(cardMessage.PublishBicker, jtb15file.cApp_sn,10);
	memcpy((unsigned char*)&uitemp, (unsigned char *)&SysTime, 4);//��ǰ����
	over_turn(4, (unsigned char*)&uitemp);
	memcpy((unsigned char*)&start_time, jtb15file.cIssueDate, 4);//��������
	over_turn(4, (unsigned char*)&start_time);
	memcpy((unsigned char*)&end_time, jtb15file.cValidDate, 4);//��Ч����
	over_turn(4, (unsigned char*)&end_time);
	memcpy(cardMessage.FactorFlg,jtb15file.cPublisherSign,8);
	if(memcmp(revbuf+1+4,"\xAA\xAA\xBB\xBB",4)==0)//����
	{
		//�ǹ�˾�Ŀ� Ҫ���¶� 40���ֽ�
		
		// 		s="0001090310AAAABBBB0001460103100109000000012016050820501231000000000000009999999900";
		// 		Ascii2BCD(s,revbuf,strlen(s));
		
		i=5;
		memcpy(sndbuf,"\x00\xB0\x95\x00\x28",i);//����Ӧ����Ϣ�ļ� 30�ֽ�
		ret = RfPassthrough(revbuf, 2, sndbuf, i);
		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		{
			ERR_DIS("ERROR 2",revbuf+ret-2);
			return ST_ERROR;
		}
		cardMessage.card_catalog=revbuf[1+4+4+2];//����
		memcpy(cardMessage.PublishBicker, revbuf+13,8);
		memcpy(cardMessage.PublishBicker+8,"\xBB\xBB",2);
		//memcpy(StufferNO,revbuf+36,4);
		memcpy(cardMessage.stuffNO,revbuf+36,4);
		MSG_LOG("����");
		BCD_LOG((unsigned char*)&cardMessage.card_catalog,1,1);
		return CONTROL_CARD;
	}
#ifdef _debug_CPU_
	debugstring("����:");
	debugdata(cardMessage.PublishBicker,10,1);
	debugstring("����������ʶ:");
	debugdata(cardMessage.FactorFlg,8,1);
	debugstring("���ñ�ʶ:");
	debugdata((unsigned char *)&jtb15file.cStarFlag,1,1);
	debugstring("���������Զ����ʶ:");
	debugdata(jtb15file.cFCI,2,1);
	MSG_LOG("��������::%08X\r\n",start_time);
	MSG_LOG("��ǰ����::%08X\r\n",uitemp);
	MSG_LOG("��Ч����::%08X\r\n",end_time);

#endif
	  ret=IF_15_OK(revbuf+1);//�ж�15�ļ�
	  if(ret!=ST_OK)
		  return ret;
//

#if 0//������ͨ ��ʱû�к�����
	if(IsInBlackList(stuPubInfZaoZuang.cPublisherSign) < FLASH_BLK_END){
#ifdef _debug_
		debugstring("lock card:-----\r\n");
#endif
		return CARD_BLACK_PRO;
		
	}
#endif//

// 	i=5;
// 	memcpy(sndbuf,"\x00\xB0\x96\x00\x37",i);//�ֿ��˻�����Ϣ 55�ֽ�
// #ifdef _debug_CPU_
// 	debugstring("***READ 16:\r\n");
// 	debugstring("SND_C:");
// 	debugdata(sndbuf, i, 1);
// #endif
// 	ret = RfPassthrough(revbuf, 2, sndbuf, i);
// #ifdef _debug_CPU_
// 	debugstring("RCV_C16:");
// 	debugdata(revbuf, ret, 1);
// #endif	
// 	if(ret == 0){
// 	//	ERR_DIS("��16�ļ�ʧ��",revbuf);
// 		return ST_ERROR;
// 	}
// 	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
// 	{
// 		ERR_DIS("��16�ļ�ʧ��",revbuf+ret-2);
// 		MSG_LOG("16�ļ�������");
// 		return ST_ERROR;
// 	}
// //	jtb16file = (stJTB16 *)(revbuf+1);
// 	memcpy((unsigned char*)&jtb16file,revbuf+1,55);
#ifdef _debug_CPU_
	debugstring("������ʶ:");
	debugdata((unsigned char*)&jtb16file.cCardType,1,1);
	debugstring("����ְ����ʶ:");
	debugdata((unsigned char*)&jtb16file.cStuffcode,1,1);
	debugstring("֤������:");
	debugdata(jtb16file.cName,20,1);
	debugstring("֤����:");
	debugdata(jtb16file.cCredent,32,1);
	debugstring("֤������:");
	debugdata((unsigned char*)&jtb16file.cCretype,1,1);
	
#endif
	//88888888888888888888888888888888888
	i=5;
	memcpy(sndbuf,"\x00\xB0\x97\x00\x3C",i);//������Ϣ�ļ�
#ifdef _debug_CPU_
	debugstring("***READ 17:\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C17:");
	debugdata(revbuf, ret, 1);
#endif
	
	if(ret == 0){
	//	ERR_DIS("��17�ļ�ʧ��",revbuf);
		return ST_ERROR;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
	{
		ERR_DIS("EP ERRO 4",revbuf+ret-2);
		MSG_LOG("�Ҳ���Ӧ��----\r\n");
		return ST_ERROR;
	}
	memcpy((unsigned char*)&jtb17file,revbuf+1,60);
#ifdef _debug_CPU_
	debugstring("���ʴ���:");
	debugdata(jtb17file.intercode,4,1);
	debugstring("ʡ������:");
	debugdata(jtb17file.provcode,2,1);
	debugstring("���д���:");
	debugdata(jtb17file.Citycode,2,1);
	debugstring("��ͨ����:");
	debugdata(jtb17file.car_type,2,1);//Ҫ�Ի�ͨ�����ж�
	debugstring("������:");
	debugdata((unsigned char*)&jtb17file.card_catalog,1,1);
	debugstring("Ԥ��:");
	debugdata(jtb17file.reser,49,1);

#endif


#ifdef _M1KEY_CLOUD_
	cardMessage.card_catalog = CARD_NORMAL_BUS;
#else
	cardMessage.card_catalog = CARD_UNION_BUS;
#endif
	cardMessage.Zicard_catalog = 0;

	MSG_LOG("��ͨ������̶�:0x%04X\r\n",cardMessage.card_catalog);

//��18�ļ�88888888888888888888
	i=5;
	memcpy(sndbuf,"\x00\xB2\x01\xC4\x00",i);//18�ļ�
#ifdef _debug_CPU_
	debugstring("***READ 18:\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C18:");
	debugdata(revbuf, ret, 1); 
#endif

//�¿� �п���û�м�¼   ���ش���� ����������
// 	if(ret == 0){
// 		//	ERR_DIS("��1E�ļ�ʧ��",revbuf);
// 		return ST_ERROR;
// 	}
// 	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0){
// 		
// 		ERR_DIS("��18�ļ�ʧ��",revbuf+ret-2);
// 		MSG_LOG("�Ҳ���Ӧ��----\r\n");
// 		return ST_ERROR;
// 	}

	memcpy((unsigned char*)&jtb18file,revbuf+1,23);//18�ļ�����
	//88888888888888888
	i=5;
	memcpy(sndbuf,"\x00\xB2\x01\xF4\x00",i);//1E�ļ�
#ifdef _debug_CPU_
	debugstring("***READ 1E:\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C1E:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0){
		//	ERR_DIS("��1E�ļ�ʧ��",revbuf);
		return ST_ERROR;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0){
		
		ERR_DIS("EP ERRO 6",revbuf+ret-2);
		MSG_LOG("�Ҳ���Ӧ��----\r\n");
		return ST_ERROR;
	}
	memcpy((unsigned char*)&JTB_1E,revbuf+1,48);//1E�ļ�����
#ifdef FengDuan_BU_
	if(CardType==LocakCard)//��15�ļ�����ж��Ƿ�Ϊ���ؿ�
		goto LOCAK;//���ؿ� 
#endif	
	
	if(CardType==YiDiCard)//��ؿ� 
	{

		if (memcmp(jtb17file.car_type,"\x00\x01",2)==0)//��ͨ��
		{
			//if(!(jtb17file.card_catalog!=0||jtb17file.card_catalog<5||jtb17file.card_catalog==0xFF)) ����Ǵ�ġ�
			if((jtb17file.card_catalog==0)||(jtb17file.card_catalog>5)||(jtb17file.card_catalog==0xFF))
			{
				ERR_DIS("ERROR 4.2",jtb17file.car_type);
				return ST_ERROR;
			}
		}
		else{
				ERR_DIS("ERROR 4.1",jtb17file.car_type);
			//	MSG_LOG("���ؿ�\r\n");
				return ST_ERROR;
		}
	}	 
//�жϻ�ͨ����
	//�ж��Żݿ���
	 //
//������ͨ�ļ����� 
#ifdef FengDuan_BU_  
	i=5;
	memcpy(sndbuf,"\x00\xB2\x02\xD4\x00",i);//1A�ļ�
#ifdef _debug_CPU_
	debugstring("***READ 1A:\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C1A:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0){
	//	ERR_DIS("��1A�ļ�ʧ��",revbuf);

		return ST_ERROR;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0){
		ERR_DIS("ERROR 7",revbuf+ret-2);
		MSG_LOG("�Ҳ���Ӧ��----\r\n");
		return ST_ERROR;
	}
	memcpy(JTB_1A.rID,revbuf+1,128);
	goto END;

LOCAK://������������
	//�����ļ�
	i=5;
	memcpy(sndbuf,"\x00\xB2\x02\xCC\x00",i);//19�ļ�
#ifdef _debug_CPU_
	debugstring("***READ 19:\r\n");
	debugstring("SND_C:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("RCV_C19:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0){
		//	ERR_DIS("��1A�ļ�ʧ��",revbuf);
		
		return ST_ERROR;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0){
		ERR_DIS("��19�ļ�ʧ��",revbuf+ret-2);
		MSG_LOG("�Ҳ���Ӧ��----\r\n");
		delayxs(1);
		return ST_ERROR;
	}
	memcpy(JTB_1A.rID,revbuf+1,128);
END:
#else//#define FengDuan_BU_

//�ж� һƱ��
	ret=IF_1E_OK((unsigned char*)&JTB_1E);
	if(ret!=ST_OK)
		return ret;
#endif
#ifdef FengDuan_BU_

	  ret=IF_1A_OK(JTB_1A.rID);
 	if(ret!=ST_OK)
		 return ret;
#endif



	ret=cardMessage.card_catalog;
	BCD_LOG((unsigned char *)cardMessage.card_catalog,1,1);
	if(cardMessage.card_catalog >= 0x80){
		ret = cardMessage.card_catalog - 0x80;
	}
	else if(cardMessage.card_catalog >= 0x40)
	{
		ret = cardMessage.card_catalog-0x40;
	}else{

		ret = cardMessage.card_catalog;

	}
	


	
	
	if((NewPriceTab.rate[ret]==204)||(NewPriceTab.rate[ret]==104))//��ֹ
	{
		return CARD_forbid;	
	}	
	
	if((NewPriceTab.rate[ret]==202)||(NewPriceTab.rate[ret] ==102)){//�������
		
		if ( ret  != CARD_NORMAL_BUS)
		{
			if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
			{
				return CARD_MONEY;
				
			}
		}
		return CARD_FREE;	
		
	}	

	return CARD_MONEY;
}
/*
extern unsigned char CardType; //�������� ����/���
int getCartypeJTB(char *cardD, unsigned char type)
{
	
	if(type == 1){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == 2){
		strcpy(cardD, "ѧ����");
	}
	else if(type == 3){
		strcpy(cardD, "���꿨");
	}
	else if(type == 4){
		strcpy(cardD, "���Կ�");
	}else if (type==5)
	{
		strcpy(cardD, "���˿�");
 	}
	//else if (type==CARD_JTB)
// 	{
// 		if(CardType==LocakCard)
// 			strcpy(cardD, "����");
// 		else
// 			strcpy(cardD, "��ͨ��");
// 	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
extern unsigned int ghisCur;//��ʷ��ǰָ��
extern void dis_time_mini(unsigned char x, stcomtime *time);
unsigned char Disp_historyJTB(unsigned char *data,unsigned int Index)
{
	unsigned char buffer[80];
	char btff[20];
	unsigned int temp;//,temp1;
	DEALRECODE_JTB historyCheck;

	 char  tembuf[22];
//	unsigned int addr;
//	int curp;
	memcpy((unsigned char*)&historyCheck,data,RECORD_JTB_LEN);
	if (memcmp(historyCheck.rTTTT,"\xAA\xAA\xAA\xAA",4)!=0)//�Ӹ��жϣ���֪��Ϊɶ ��ʱ�����
	{
		return ST_ERROR;
	}
	switch((historyCheck.rDealType))
	{
	case ID_REC_HJY_REC:
	case ID_REC_TOLL:
		cls();
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
		sprintf((char*)buffer,"�ۿ�:%d.%02d ",temp/100, temp%100);
		memset(btff, 0, 20);//���뿨��
		getCartypeJTB(btff, historyCheck.rCardType);
		strcat((char*)buffer, btff);
		display(2,0,(char*)buffer,0);
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp, historyCheck.rAfterMoney, 4);
			sprintf((char*)buffer, "���:%d.%02d", temp/100, temp%100);
		display(4,0,(char*)buffer,0);
		
		dis_time_mini(6, (stcomtime*)&historyCheck.rDealTime);
		break;
	case ID_REC_MON:
		cls();
		memset(buffer,0,20);

		temp = 0;
		memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
		if(temp == 0){
			getCartypeJTB((char*)buffer, historyCheck.rCardType);
			display(3,0,(char*)buffer, DIS_CENTER);
		}
		else{
			temp=0;
			memcpy((unsigned char*)&temp,historyCheck.rDealMoney,3);
				sprintf((char*)buffer, "�۴�:%d ",temp);
			memset(btff, 0, 20);//���뿨��
			getCartypeJTB(btff, historyCheck.rCardType);
			strcat((char*)buffer, btff);
			display(2,0,(char*)buffer,0);

			memset(buffer,0,20);
			memcpy((unsigned char*)&temp, historyCheck.rAfterMoney, 4);
			if(gchn_eng == 'E')
				sprintf((char*)buffer, "balance:%d", temp);
			else
				sprintf((char*)buffer, "���:%d", temp);
			display(4,0,(char*)buffer,0);
		}
		break;

	case ID_REC_CLERK:
		cls();
		display(3, 0, "վ��Ա��", DIS_CENTER);
		break;

	case ID_REC_ARVBUS:
		cls();
		display(3, 0, "��վ��¼", DIS_CENTER);
		break;
	case ID_REC_LINESET:
			  cls();
		display(3, 0, "��·��", DIS_CENTER);
		break;
	case ID_REC_BLKLIST	:
		cls();
		display(3, 0, "�ڿ���¼", DIS_CENTER);
		break;
	default:
		return ST_ERROR;
	}
	if(historyCheck.rBusFlag&BUS_TES&&historyCheck.rDealType!=ID_REC_HJY_REC)//��������
		sprintf((char*)buffer, "TES_REC    ->%d ", Index+1);
	else if(historyCheck.rDealType==ID_REC_HJY_REC)
		sprintf((char*)buffer, "UN_Know    ->%d ", Index+1);
	else
		sprintf((char*)buffer, "->%d ", Index+1);
	miniDispstr(7, (21-strlen((char*)buffer)), (char*)buffer, 0);
	memset(buffer,0,20);
	temp=0;
	//memcpy((unsigned char*)&tembuf, historyCheck.rPublishNumb,8);
	memset(tembuf,0,22);
	BCD2Ascii(historyCheck.rPublishNumb,(unsigned char*)tembuf,10);
	//over_turn(8,(unsigned char*)&tembuf);	
//	sprintf((char*)buffer,"%16x",tembuf);
	strcpy((char*)&buffer,tembuf);
	miniDispstr(0,1,(char*)buffer,0);
	dis_time_mini(6, (stcomtime*)historyCheck.rDealTime);
	return ST_OK;

}

//��֯���͵Ĺ�����¼��
unsigned int Build_GJRec_SendJTB(unsigned char cnum, unsigned int *headp)//
{
	unsigned int len;
//	unsigned int temp;
	unsigned short crc16;
	unsigned int addr;
	unsigned char disbuf[256];
//	unsigned int i;
	DEALRECODE_JTB *Record;
//	unsigned char buffer[130];
	unsigned char dat[500];
	unsigned char cmpbuf[100];
	memset(cmpbuf,0,64);
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
#ifdef BUS_SYBAOSHI_
	dat[len++] = 0x05; //��Ӧʱ��Ҫ�����ֻ�����Ͷ������ݡ�
#else
	dat[len++] = 0x03;
#endif
	
	dat[len++] = 0x01;//GnCode
	
	sysferead(FE_PACKSERIAL, 2, dat+len);	//P1,p2�����к�
	memcpy((unsigned char*)&tcpipPacket.Serial, dat+len, 2);
	len += 2;

	memcpy(dat+len, (unsigned char**)&headp, 2);//p1,p2,p3,p4 ���ڱ���������¼�ĵ�ַ
	len += 2;
	
	dat[len++] = 0x00;//����=64�ֽ�
	dat[len++] = 0x80;
	
	addr = *headp;
//	for(i=0; i<cnum; i++){//ֻ����һ��
	//	flashread(addr, disbuf, RECORD_LEN);
		flashread(addr, disbuf, RECORD_JTB_LEN);//�ȶ�128���ֽڳ���

		if(memcmp(disbuf, "��������", 8) == 0){//
			memcpy(dat+len, disbuf+8, (RECORD_LEN-8));
			len += (RECORD_LEN-8);//56
			
			if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//������д��ȫ0��
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}

			flashread(addr, dat+len, RECORD_LEN);
			len += RECORD_LEN;//64
			
			if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//������д��ȫ0��
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}

			flashread(addr, dat+len, 30);
			len += 30;//30          56+64+30 = 150
		}
		else{//128���ֽ�
			Record=(DEALRECODE_JTB*)disbuf;//128���ֽ�һ����¼
#ifdef _debug_		
			debugstring("rtttt");
			debugdata((unsigned char*)Record,128,1);
			debugdata(Record->rTTTT,4,1);
#endif
			if ((memcmp(Record->rTTTT,"\xAA\xAA\xAA\xAA",4)!=0)&&(memcmp(Record->rTTTT,"\xBB\xBB\xBB\xBB",4)!=0))//����������ĸ��ֽ����ĸ�AA
			{
				MSG_LOG("�ɵļ�¼���쳣��¼\r\n");
				memcpy(disbuf+64,cmpbuf,60);//��������ǰ�ɵļ�¼ ����64���ֽڲ��� ���Ҳ��������ǰ��
				memset(disbuf+64+60,0xBB,4);//������4��B
				memcpy(dat+len,disbuf,128);
				len+=RECORD_JTB_LEN;//����
			}
			else{//����ʽ��¼ �����ǰ������64���ֽ�
				MSG_LOG("������¼\r\n");
				memcpy(dat+len,disbuf,128);
				if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
					memset(disbuf, 0, RECORD_LEN);
					flashwrite(addr, disbuf, RECORD_LEN);//0-64������д��ȫ0��
				}
				addr += RECORD_LEN;
				len+=RECORD_JTB_LEN;//����
			}	
		}

		if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
			memset(disbuf, 0, RECORD_LEN);
			flashwrite(addr, disbuf, RECORD_LEN);//64-128������д��ȫ0��
		}
#ifdef _debug_gprs
		if(addr < FLASH2_START1)
			debugstring("���ɵ�ǰ���Ͱ�:");
		else
			debugstring("������ʷ���Ͱ�:");
			//debugdata(dat+len, RECORD_JTB_LEN, 1);
#endif	
		addr += RECORD_LEN;
		if(addr >= FLASH2_START2END){
			addr = FLASH2_START1;
		}
		if(addr >= FLASH2_START1)
			*headp = addr;
//	}

	dat[GCMD_Ln] = ((len - 16)/256);
	dat[GCMD_Ln+1] = ((len - 16)%256);//���¼��㳤��

	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;

	tcpipPacket.packetStatus = RECORD_DEAL;
	tcpipPacket.length = len;//Ҫ���͵ĳ���
	memcpy(tcpipPacket.tcpBuffer, dat, len);//Ҫ���͵�����
	sysfewrite(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket);
#ifdef _debug_gprs
	debugstring("���ɷ��Ͱ����:");
	debugdata((unsigned char*)&*headp, 4, 1);
	debugdata((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
	debugdata((unsigned char*)&tcpipPacket.length, 2, 1);
	debugdata((unsigned char*)&tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
#endif
	return len;
}*/
#endif  //#ifdef BUS_PRO_JTB
