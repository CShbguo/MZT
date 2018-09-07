/********************************************
*GPRS����Э�鴦�����
*֧��JYS�Ĺ���Э����ճ���
*��д��KSLi 
*********************************************/
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
//#include "Board.h"

#include "..\include\include.h"
//#include "public.h"
#include "szct.h"
#include "Font.h"
#include "Gszct.h"
#include "E_Deal24GCard.h"
#include "E_Gmobile.h"

//#define _debug_blk

#define DDMAX_RCV_PACKET_NUM 2

extern stLineInfo LineInfo[2];//��·��Ϣ�������վ����Ϣ ��������0�� ����    1�鷴��

extern unsigned char StufferNO[4];
extern pFistVary_1 pFistVary;
extern stcomtime SysTime;
extern stPricetable NewPriceTab;
//extern unsigned char gbusblkbuf[30];//���س��򡢺���������·������ʱ�Ļ���
extern stbusdownPara gBusblkDownPro;//���������أ�ָ��gbusblkbuf
#ifdef _GPS_DIAODU_
extern stAlammessage galamessage;
#endif
//������ÿ������
extern unsigned short gBlackBlockNum[16];

extern stBusTicketV gBusTicketV;
extern stBusVerIfo gBusVerInfo;

extern stbusdownpro gBusDownPro;
extern stGPSdownLine gGPSDownLine;//��·��Ϣ
extern unsigned char restore_flag;
extern unsigned char gTTime[14];
unsigned char gmissOldflag;//�ϸ�����������־ 0x00 ��½�ƶ�,ǩ��.
unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.
extern unsigned char DriveCardNo[4];
extern Parameter3 ComSumeFile;
extern Parameter2 POINT_MESSAGE;
extern unsigned char GPRSLinkProcess;
extern unsigned char GprsCardList[16];
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char gchn_eng;//��Ӣ�ı�־
extern unsigned short gErrorFlag;//�����ʶ,���ֽڱ�ʾ�����ţ����ֽڱ�ʾ������� ���"*��������ʶ����"
extern unsigned int gErrortimes[2];//���������,��Ҫ������GPRS����
extern unsigned char gGPRS_Cmd_outtime;//GPRS����ʱ����

extern TCP_IP_PACKET1 tcpipPacket;
#ifdef _GPS_DIAODU_
//������Ϣ
extern stDiaoDuCMD gDiaoDuCMD;
//����Ӫ����Ϣ
extern unsigned int Build_GJ_ALAM(unsigned char *dat);
#endif
extern stDiaoDuinfo gDiaoDuinfo;
#ifdef KEY_PAD
extern volatile unsigned char gKeyRestorDis;//��Ҫ����ˢ��KEYPAD��ʾ
#endif
#ifdef SelMsg_enable
extern int findSelMsg(void);
extern void SelMsgInit(void);
extern void addSelMsg(char *SelNo, char *msg, unsigned char len);
#endif

extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern unsigned int ClientIP;
extern unsigned short ClientPort;
#ifdef GPRS_MG323_
extern unsigned char gTcpIpbusy;//ģ���������ڷ����ݣ�ģ�鹤���Ƿ����Ȼ���Ӧ���ٷ����ݡ��ڵȻ�Ӧʱ���ܷ��������ݡ�
extern volatile stGPrsDataStyle gGPRS_data_style;
extern unsigned char gMC55iSendbuff[256];//MC55i�������ݻ��壬ǰ8�ֽ�Ϊ����������Ч��+���ȣ�4��+����...
extern void gprs_DATA_snd(unsigned int len, unsigned char *dat);
#endif
extern unsigned short BCC( unsigned char *Bcc_pointer,unsigned short x);
extern void stopCCgprs(unsigned short mode);
//ȡ��¼ָ��
extern void clr_Record_point(void);
extern void savesysbupara(void);
extern void readmobilsendStyle(void);
extern unsigned char posnoIsOK(unsigned char *dat);
extern unsigned int Build8583Packge(unsigned char *revBuf, unsigned char mode);

extern unsigned char checkNewPrice(void);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char GetDateTime(void);
extern void Load_Line_info(void);
extern void getProVer(void);
#ifdef GPRS_MG323_
extern void initTCPIPnode(void);
#endif
extern unsigned char SYSgetbussinessNO(unsigned char *dat);
extern unsigned char gGPSdly;//GPS��ʱ
extern unsigned char isNetOK[2];//��·�����Ƿ�ɹ�;
extern void find_new_mission(void);
extern void find_G_new_mission(void);//������һ���һ��;
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat);
extern void MC55i_SISREAD(void);
extern void tcpipClose(unsigned char mode);

#ifdef _GPS_ENABLE_
extern unsigned int Build_GJ_GPS(unsigned char *dat);
#endif
extern void out_GPRS(void);
extern unsigned char gAPN_Style;//APN״̬����ר�������ǹ���
#define GPRSCOMMAX 1024
extern unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS���ջ���
extern volatile unsigned short gprsIndex;
extern unsigned char gprsErrorTimes;//���֣ţңңϣҷ��ش������
extern PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//����֡����


#ifndef NO_GJ_SERVISE
unsigned char gsendError;//���ʹ���������������ڳ����Ͷ������Ӧʱֹͣ����
unsigned short BlackLength;
unsigned int Sendpincount;//�ط���Ӧ������������
unsigned int gSendGLogin;//��½����
unsigned int gGprsResertTime;	//����GPRSģ�������GPRSģ�����û�յ�����10��������һ�Ρ�
#endif

extern unsigned char BUflag;//���ػ�״̬ 0x02 ��Ҫˢ˾���� 
volatile unsigned char gpacSNo;//�����

#ifdef SelMsg_enable
extern stSelMsg gsendMsg;
#endif

#ifdef CPU_JSB
//��Ч����0  ��Ч��������
unsigned int load_white_list(void);
#endif


unsigned char Xor8(const unsigned char *pData, unsigned long DataLen)
{
	unsigned char ret;
	unsigned long i;
	
	ret = 0;
	for(i=0; i<DataLen; i++)
	{
		ret ^= pData[i];		
	}	
	
	return ret;
}
void gprs_send_data_OTHER_CMD(unsigned char cmd, unsigned int len, unsigned char *dat)
{
	unsigned char buff[200];
	unsigned int pos=0;
//	unsigned int CRC;
	unsigned short i;
	
	memset(buff, '$', 4);//��ͷ
	pos = 4;
	
	i = len+9;
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//���ݳ��ȣ���������+Ӧ�ò����� �ĳ���,���ֽ���ǰ
	pos += 2;
	
	buff[pos++] = cmd;//����
	buff[pos++] = gpacSNo;//�����
	if (len !=0)
	{
		memcpy(buff+pos, dat, len);
		pos += len;
	}
	
// 	CRC = crc16_isr((unsigned char*)buff, pos);
// 	
// 	memcpy(buff+pos, (unsigned char*)&CRC, 2);
// 	//over_turn(2, buff+pos);
// 	pos += 2;
	buff[pos++] = Xor8(buff,pos);
#ifdef _debug_DVR
//	miniDispstr(12,0,"S",0);
	//beep(1,50,50);
//	delayxms(200);
//	miniDispstr(6,0," ",0);
	debugstring("Send DVR:");
	debugdata(buff,pos,1);
#endif
	com_snd(COM1, pos, (unsigned char*)buff);

	if (cmd == 0x02)
	{
		MSG_LOG("�洢����ʱ��----\r\n");
		sysfewrite(BIT_DVR_PACKET,pos,buff);
	}
	

}
#ifdef OTHER_GPRS
void deal_DVR_ack(void)
{
	unsigned char sendbuf[10];
	
	memset(sendbuf,0,10);
	sysfewrite(BIT_DVR_PACKET,10,sendbuf);
#ifdef _debug_
	debugstring("���DVR������::::\r\n");
#endif	
}
void gprs_other_sndbuinfo(void)
{
	unsigned char ddbuff[32];
	unsigned int pos = 0;
	
	memcpy(ddbuff+pos, pFistVary.DeviceNo, 8); pos += 8;
	Get_SerialNumF4(ddbuff+pos); pos += 4;
	memcpy(ddbuff+pos, StufferNO, 4); pos += 4;
	memcpy(ddbuff+pos, pFistVary.LineNo, 2); pos += 2;
	memcpy(ddbuff+pos,&SysTime.year_h,7); pos +=7;
	ddbuff[pos++] = F_FLAG_VARY.stop_flag;
	
	
	
	gprs_send_data_OTHER_CMD(0x02, pos, ddbuff);
}

void gprs_other_check_dvr(void)
{
	unsigned char ddbuff[32];
	unsigned int pos = 0;
	
// 	memcpy(ddbuff+pos, pFistVary.DeviceNo, 8); pos += 8;
// 	Get_SerialNumF4(ddbuff+pos); pos += 4;
// 	memcpy(ddbuff+pos, StufferNO, 4); pos += 4;
// 	memcpy(ddbuff+pos, pFistVary.LineNo, 2); pos += 2;
// 	memcpy(ddbuff+pos,&SysTime.year_h,7); pos +=7;
// 	ddbuff[pos++] = F_FLAG_VARY.stop_flag;
	
	
	
	gprs_send_data_OTHER_CMD(0x03, pos, ddbuff);

}

void Deal_DVR_Status(unsigned char *dat)
{
	// 	unsigned char *infor;
	// 	infor =( stDVRstation *)( dat+8);
	
	debugstring("NULL!!!!!!!!!!!!\r\n");
	;
}

//����04���dVR���ص�վ������
void Deal_station_infor(unsigned char * dat)
{
	unsigned int plen=0;
	// 	unsigned char resave=0;
	stDVRstation *infor;
#ifdef _debug_DVR
	unsigned char uctemp=0;
	char disbuff[32];
#endif
	
	
	memcpy((unsigned char*)&plen, dat+4, 2);
	over_turn(2, (unsigned char*)&plen);
#ifdef _debug_
	MSG_LOG("deal--04--Deal_station_infor\r\n");
	BCD_LOG(dat,plen,1);
#endif
	
	if (plen !=35)
	{
		debugstring("line infor����35�ֽ�!\r\n");
		return;
	}
	
	infor =( stDVRstation *)( dat+8);
	
	
#ifdef _debug_DVR
	uctemp = infor->ucDirect;
	cls();
	sprintf(disbuff,"Direct:%02d,%s,Now:%02d\x0\r\n",uctemp,uctemp==0 ?"UP":"DOWN",infor->ucStation);
	miniDispstr(0,0,disbuff,0);
	sprintf(disbuff,"Max:%02d,Middle:%02d\x0\r\n",infor->ucStationMax,infor->ucStaMiddle);
	miniDispstr(1,0,disbuff,0);
	memcpy(disbuff,infor->name,20);
	disbuff[20]=0;
	display(2,0,disbuff,0);
	
	sprintf(disbuff,"LineNO:%02X%02X\x0",infor->LineNO[0],infor->LineNO[1]);
	miniDispstr(4,0,disbuff,0);
	
	beep(1,50,50);
	delayxms(1000);
	
	restore_flag = 3;
	
	// 	MSG_LOG("����::����:%02d,%s,���:%02d\r\n",uctemp,uctemp==0 ?"����":"����",infor->ucStation);
	// 	MSG_LOG("����::�����%d,��ת���%d\r\n",infor->ucStationMax,infor->ucStaMiddle);
	// 	MSG_LOG("gFDStaIndex[gFDderict]%d\r\n",gFDStaIndex[gFDderict]);
#endif
	
	
	
}


#endif
//----------------��ӵ���������-----------------

void gprs_send_data_OTHER(unsigned char linkNum, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos=0;
	
#ifdef COM2RJ45_
	memcpy(buff, dat, len);
	pos = len;
#else		 
//	unsigned int CRC;
	unsigned short i;
	memset(buff, '$', 4);//��ͷ
	pos = 4;
	
	i = len+9;		//ֻ��һ��XOR
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//���ݳ��ȣ���������+Ӧ�ò����� �ĳ���,���ֽ���ǰ
	pos += 2;
	
	buff[pos++] = 0x01;//�������ݣ�ֱ�ӷ��͵���̨�����ֽڲ���Ҫ����
	buff[pos++] = gpacSNo;//�����
	if (len !=0)
	{
		memcpy(buff+pos, dat, len);
		pos += len;
	}
	
// 	CRC = crc16_isr((unsigned char*)buff, pos);	
// 	memcpy(buff+pos, (unsigned char*)&CRC, 2);
// 	//over_turn(2, buff+pos);
//	pos += 2;
	buff[pos++] = Xor8(buff,pos);
#endif

#ifdef _debug_DVR
//	miniDispstr(6,0,"S",0);
	//beep(1,50,50);
//	delayxms(200);
//	miniDispstr(6,0," ",0);
	debugstring("Send DvrData:");
	debugdata(buff,pos,1);
#endif

	com_snd(COM1, pos, buff);

}
//�����豸��������
void GPSDD_Deal(unsigned char mode, unsigned char pasn)
{
	unsigned char sendbuf[200];
	unsigned char pos=0;
//	unsigned int CRC;

	memset(sendbuf, '$', 4);
	pos = 4;
	switch(mode){
	case 0x21://ת���������ļ�
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 9; pos++;
		sendbuf[pos] = 0x21; pos++;
		sendbuf[pos] = pasn; pos++;//�����
		break;
	case 0x22://���ػ�ȡIC �����豸��Ϣ
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 0x1b; pos++;
		sendbuf[pos] = 0x22; pos++;
		sendbuf[pos] = pasn; pos++;//�����
		memcpy(sendbuf+pos, pFistVary.DeviceNo, 8); pos += 8;
		Get_SerialNumF4(sendbuf+pos); pos += 4;
		memcpy(sendbuf+pos, StufferNO, 4); pos += 4;
		memcpy(sendbuf+pos, pFistVary.LineNo, 2); pos += 2;
		break;
	case 0x23://ˢ���˴��ϴ���
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 11; pos++;
		sendbuf[pos] = 0x23; pos++;
		sendbuf[pos] = pasn; pos++;//�����
		sendbuf[pos++] = 0;//�ϳ�ˢ������
		sendbuf[pos++] = 0;//�³�ˢ������
		break;
	case 0x24://ˢ���˴������
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 9; pos++;
		sendbuf[pos] = 0x24; pos++;
		sendbuf[pos] = pasn; pos++;//�����
//		gOTGrc = 0;
		break;
	case 0x25://˾���ϰ��¼��Ӧ��
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 25; pos++;
		sendbuf[pos] = 0x25; pos++;
		sendbuf[pos] = pasn; pos++;//�����
		memcpy(sendbuf+pos, (unsigned char*)&SysTime, 7);
		pos += 7;
		if(F_FLAG_VARY.stop_flag){
			sendbuf[pos++] = 0;
		}
		else
			sendbuf[pos++] = 1;
		memset(sendbuf+pos, 0, 8);
		memcpy(sendbuf+pos+4, DriveCardNo, 4);
		over_turn(4, sendbuf+pos+4);
		pos += 8;
		break;

	default:
		break;
	}
// 	CRC = crc16_isr(sendbuf, pos);
// 	memcpy(sendbuf+pos, (unsigned char*)&CRC, 2);
// 	over_turn(2, sendbuf+pos);
// 	pos += 2;
	sendbuf[pos++] = Xor8(sendbuf,pos);

#ifdef _debug_
	debugstring("GPSDD_Deal Send:");
	debugdata(sendbuf,pos,1);
	com_snd(COM4, pos, sendbuf);
#else
	com_snd(COM1, pos, sendbuf);

#endif
}
#ifdef OTHER_GPRS
#ifdef COM2RJ45_	//ֱ����BDBDBD������
void GPRS_BDBD_Data_hand(unsigned char value)
{
	unsigned short plen;
	unsigned char ii;
	
	ii = value;
	//	debugdata((unsigned char*)&ii, 1, 0);
	switch (gprsIndex)
	{
	case 0:
	case 1:
	case 2:
		if(ii != 0xBD){
			gprsIndex = 0;
		}
		else{
			gprsRevBuf[gprsIndex++] = ii;
		}
		break;
// 	case 14://���ȸ�λ
// 		gprsRevBuf[gprsIndex++] = ii;
// 		break;
// 	case 15://���׵�λ
// 		gprsRevBuf[gprsIndex++] = ii;
// 		memcpy((unsigned char*)&plen, gprsRevBuf+14, 2);
// 		over_turn(2, (unsigned char*)&plen);
// 		if(plen > 2048){//���ȴ���
// 			gprsIndex = 0;
// 			memset(gprsRevBuf,0,10);
// 			break;
// 		}
// 		break;
	default:
		gprsRevBuf[gprsIndex++] = ii;
		if(gprsIndex <= 15)
			break;
		memcpy((unsigned char*)&plen, gprsRevBuf+14, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 1024){//���ȴ���
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		if(gprsIndex >= (plen+18)){//���ݽ������
#ifdef _debug_gprs
			debugstring("��:");
			debugdata(gprsRevBuf, gprsIndex,1);
#endif
			miniDispstr(6, 0, "RR_OKK", 0);

			gprsErrorTimes = 0;
			for(ii=0;ii<MAX_RCV_PACKET_NUM;ii++){
				if(gPacketRcvBuf[ii].ucItemStatus != 0xea)
					break;
			}
			if(ii>=MAX_RCV_PACKET_NUM){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);	//�����������˰�����
//					debugstring("����\r\n");
				return;
			}

			memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf, gprsIndex);
			gPacketRcvBuf[ii].LinkFrame.ucFrameType = 0x07;
			memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&gprsIndex, 2);
			gPacketRcvBuf[ii].ucItemStatus = 0xea;
#ifdef _debug_gprs
			debugstring("����:");
			debugdata((unsigned char*)&gPacketRcvBuf[ii].LinkFrame, gprsIndex+3, 1);
#endif

			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
		}
		break;
	}
}


#else

void DVR_test(void)
{
//	unsigned char ddbuff[32];
	//	unsigned int pos = 0;
	
	
	//MSG_LOG("test----\r\n");
//	gprs_send_data_OTHER_CMD(2,0,ddbuff);	//���Ͳ�������04  ����DVR��û�л�Ӧ
//	gprs_other_sndbuinfo();
	
}




void OTHER_GPRS_hand(unsigned char value)
{	
	unsigned short plen;
	unsigned char ii;

	ii = value;
//	debugdata((unsigned char*)&ii, 1, 0);
	switch (gprsIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if(ii != '$'){
			gprsIndex = 0;
		}
		else{
			gprsRevBuf[gprsIndex++] = ii;
		}
		break;
	case 4://���ȸ�λ
	case 5://���׵�λ
		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//���ȴ���
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		break;
	default:
		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//���ȴ���
#ifdef _debug_DVR
			debugstring("���ȴ���*************\r\n");
#endif
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		if(gprsIndex >= plen){//���ݽ������
#ifdef _debug_
			debugstring("��DVR_CMD:0x");
// 			debugdata(gprsRevBuf+6,1,1);
// 			debugdata(gprsRevBuf, gprsIndex,1);
#endif

#ifndef _debug_
			if (Xor8(gprsRevBuf,gprsIndex-1) != gprsRevBuf[gprsIndex-1])
			{
				debugstring("XOR error*************\r\n");

				gprsIndex = 0;
				memset(gprsRevBuf,0,10);
				break;
			}
#endif

#ifdef _debug_DVR
			miniDispstr(12,2,"R",0);
			beep(1,50,50);
			delayxms(100);
			miniDispstr(12,2," ",0);
#endif

#ifdef _debug_DVR
//			debugstring("----DVR--ready------\r\n");
#endif
			if(gprsRevBuf[6] == 0x21){//�Ǻ�̨����
				gprsErrorTimes = 0;
				for(ii=0;ii<MAX_RCV_PACKET_NUM;ii++){
					if(gPacketRcvBuf[ii].ucItemStatus != 0xea)
						break;
				}
				if(ii>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//�����������˰�����
//					debugstring("����\r\n");
					return;
				}
				memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf+8, plen-9);
				gPacketRcvBuf[ii].LinkFrame.ucFrameType = 0x07;
				memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&plen, 2);
				gPacketRcvBuf[ii].ucItemStatus = 0xea;
			}
			if((gprsRevBuf[6]==0x21))
				GPSDD_Deal(gprsRevBuf[6], gprsRevBuf[7]);
			else if ((gprsRevBuf[6]==0x02))
			{		//IC���������ϱ��豸��Ϣ����(0x02)
				
				deal_DVR_ack();
			}
			else if ((gprsRevBuf[6]==0x03))
			{		//IC���������ϱ��豸��Ϣ����(0x02)
				
				Deal_DVR_Status(gprsRevBuf);
			}

			else if ((gprsRevBuf[6]==0x04))
			{
				Deal_station_infor(gprsRevBuf);
			}
			else 
				gpacSNo++;//��������л�Ӧ���ͼӣ����潫���°�
#ifdef _debug_DVR
// 			delayxms(200);
//			miniDispstr(12,2," ",0);
#endif

			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
		}
		break;
	}
}
#endif
#endif
//----------------��ӵ��������ݡ�������������������
//--------------------------------------------------------
#ifdef _YiDa_Card_
extern void YD_GPSMsginfo(void);
#endif
#ifdef BUS_YICHUN
extern void checkDataSend(void);
#endif

//ÿһ���ӽ�һ��
void in_dispGprsProcess(void)
{
#ifndef NO_GJ_SERVISE
	char buff[32];
#ifdef OTHER_GPRS
	static unsigned int dvr_timer=0;
#endif
	gGprsResertTime++;
// 	sprintf(buff, "gGprsResertTime:%d", gGprsResertTime);
// 	debugstring(buff);
	if((gGprsResertTime >= 600)&&((GPRSLinkProcess&0xF0) != 0xF0)){
		gGprsResertTime = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
	}

	sprintf(buff, "%02X", GPRSLinkProcess);
	miniDispstr(15, 19, buff, DIS_RIGHT|DIS_CONVERT);
#ifdef NO_MOBILE_SERVISE	//���û���ƶ�ƽ̨����Ĭ���ƶ�ƽ̨���Ӵ���,��Ȼ�ᵼ�¼�⵽GPRS����ʱ������ģ��.
	gErrorFlag |= ERROR_CONNECT_MOBILE;
#endif

#ifndef NO_MOBILE_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))//���ֻ�ģ���ʼ�����
		miniDispstr(14, 19, "--", DIS_RIGHT|DIS_CONVERT);
	else{
		sprintf(buff, "%02X", gmissflag);
		miniDispstr(14, 19, buff, DIS_RIGHT|DIS_CONVERT);
	}
#elif defined _CPU_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))//���ֻ�ģ���ʼ�����
		miniDispstr(14, 19, "--", DIS_RIGHT|DIS_CONVERT);
	else{
		sprintf(buff, "%02X", gmissflag);
		miniDispstr(14, 19, buff, DIS_RIGHT|DIS_CONVERT);
	}
#else
	sprintf(buff, "%02X", gmissflag);
	miniDispstr(14, 19, buff, DIS_RIGHT|DIS_CONVERT);
#endif
#endif//#ifndef NO_GJ_SERVISE
#ifdef BUS_YICHUN
	YD_GPSMsginfo();
#endif
	if(BUflag > 0){
		BUflag--;
		if(BUflag == 0)
			restore_flag = 3;
	}

#ifdef OTHER_GPRS
	if ((++dvr_timer % 4) == 0)
	{

		//DVR_test();
		//dvr_timer = 0;
	}
#endif
	return;
}

unsigned char in_getGprsModeStyle(void)
{
	unsigned char inmmdd;

#ifdef GPRS_MG323_
	inmmdd = Gmode_MG323;
#elif defined CDMA_MC323_
	inmmdd = Gmode_MC323;
#elif defined OTHER_GPRS
	inmmdd = Gmode_OTHER;
#elif defined WIFI_TLH_
	inmmdd = Gmode_WIFI_TLH;
#elif defined ALL4G
	inmmdd = Gmode_ALL4G;
#else
	inmmdd = Gmode_900C;
#endif

#ifdef Modem_BUS_
	inmmdd |= Gmode_MODEM;
#endif

	return inmmdd;
}
extern unsigned char gprsSingle;//GPRS�ź�ǿ��
//ÿһ���ӽ�һ��
int in_sendGPs(void)
{
	#ifdef _GPS_NEED_REC_
	unsigned int t;
	unsigned char sndbuf[128];
	#endif
unsigned char static Dly=0;
if((gmissflag==MISS_G_UREC)&&(Dly++<10))//�����ݵ�ʱ���ӿ�  �����������
	return 0;
	Dly=0;
#ifdef _GPS_NEED_REC_

	if(gGPSdly > 50)
		gGPSdly = 8;

	if(gGPSdly>0)
		gGPSdly--;
	if(gGPSdly == 0){//10���ӷ�һ��GPS��Ϣ
		gGPSdly = 8;
		if((GPRSLinkProcess == TCPSTARTSTAT)||(GPRSLinkProcess == 0xA0)){//GPRS�Ѿ���½�����Է�����
			t = Build_GJ_GPS((unsigned char*)sndbuf);//����GPS��Ϣ����
			if(t != 0){
				gprs_send_data(1, t, (unsigned char*)sndbuf);
				return 1;
			}
		}
	}


#endif
#ifdef BUS_YICHUN
	checkDataSend();
#endif
	//��ʾ�ź�ͼ��
	ShowGprsSinal(2,14,gprsSingle);
	return 0;
}
//��GPRS��Դ 
void GPRS_open(void)
{
#ifdef BUS_JIUJIANG_
#ifdef CDMA_CASE_
	gprs_pwon_lo();
#else
	gprs_pwon_hi();
#endif
#else
	gprs_pwon_lo();
#endif
	gprsIndex = 0;
	gprsErrorTimes = 0;
}

//��GPRS��Դ,MC55I�Ŀ�������
void GPRS_MC55i_open(void)
{
#ifdef BUS_JIUJIANG_
#ifdef CDMA_CASE_
	gprs_pwon_hi();//�Ž��Ժ�Ļ���
#else
	gprs_pwon_lo();
#endif
#else
	gprs_pwon_hi();
#endif
	gprsIndex = 0;
	gprsErrorTimes = 0;
	
#ifdef _debug_gprs
	debugstring("GPRS_MC55i_open::\r");
#endif
}

void in_decgM_Senddll(void)
{
#ifndef NO_MOBILE_SERVISE
	if(gM_Senddll > 0)
		gM_Senddll--;//�����ݵ����ͷ���¼
	if(gM_Senddll > 50)
		gM_Senddll = 0;//�ȴ�ʱ�䲻����50
#else
	;
#endif
#ifndef NO_MOBILE_SERVISE
	if((g24PurError>0)&&(g24PurError<0xa0))
		g24PurError--;
#elif defined _CPU_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))
		g24PurError--;
#endif
	
#ifdef SelMsg_enable
	if(gsendMsg.TLL > 0)
		gsendMsg.TLL--;
#endif
}
unsigned char getGprsCom(void)
{
	return COM_GPRS;
}
unsigned int getDefaultIP(void)
{
	return GJ_IP;
}
unsigned int getCpuIPaddr(void)
{
#ifndef _debug_
	unsigned char gpsrsP[100];
#ifdef BUS_DAFANG
		ClientIP = getDefaultIP();
#else
#endif
	memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
	sprintf((char*)gpsrsP+60, "%03u.%03u.%03u.%03u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
	if((memcmp(gpsrsP+50, "\x00\x00\x00\x00", 4) == 0)||(memcmp(gpsrsP+50, "\xFF\xFF\xFF\xFF", 4) == 0)){
		ClientIP = getDefaultIP();
		//	MSG_LOG("IP����:");
		memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
		sprintf((char*)gpsrsP+60, "%03u.%03u.%03u.%03u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
	}
#ifndef _debug_
#endif
#endif
	return ClientIP;
}
unsigned short getDefaultPort(void)
{
	return GJ_PORT;
}
unsigned short getCpuPort(void)
{

#ifdef BUS_DAFANG
	ClientPort = getDefaultPort();
#else
	if((ClientPort == 0)||(ClientPort >= 9900)){
		//MSG_LOG("�˿ڳ���");
		ClientPort = getDefaultPort();
	}
#endif
	return ClientPort;
}


void getDefaultAPN(char *rAPN, unsigned char mode)//mode=0 ������1 �ƶ�����
{
	if(mode == 0){
		strcpy(rAPN, NETAPN);
	}
	else{
#ifdef _MOBILE_CMNET_
		strcpy(rAPN, "CMNET");
#elif defined _MOBILE_IP_CMPAY
		strcpy(rAPN, "CMPAY");
#else
		strcpy(rAPN, "CMPAY.HB");
#endif
	}
}

int in_buildSevece(char *gpsrsP)
{
#ifndef NO_GJ_SERVISE
	unsigned int t;
#endif
	int i=0;
// #ifdef _debug_gprs
// 		debugstring("gmissflag:");
// 		debugdata((unsigned char*)&gmissflag,1, 0);
// #endif
	if((gmissflag & 0xf0)==0x10){//��½�ƶ�IF1
#ifdef NO_MOBILE_SERVISE
		gmissflag = 0;
		set_timer0(1000, 1);
		return -2;
#else
// 			if(gmissOldflag != MISS_M_TradeBatch){
// 				if((gmissflag == MISS_M_SINGIN)&&(F_FLAG_VARY.stop_flag)){//����ǵ�վ״̬��������ǩ����
// 					gmissflag = MISS_G_FREE;
// 					set_timer0(500, 1);
// 					break;
// 				}
// 			}
		if(isNetOK[0] != 0){
			tcpipClose(5);
			//out_GPRS();
			return -1;
		}
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y')
		{
			GPRSLinkProcess = 16;
			out_GPRS();
			return -1;
		}
#endif
		memset((char *)gpsrsP, 0, 100);
//			strcpy((char*)gpsrsP, "AT%IPOPENX=1,\"TCP\",\"211.138.236.210\",26207\r\n");//��һ��tcp����
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"172.031.011.193\",36007\r\n");//��һ��IF2 tcp����
#ifdef CDMA_MC323_
		memcpy((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"", 19);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 19;
#elif defined GPRS_MG323_
		memcpy((char*)gpsrsP, "at^siss=0,address,\"socktcp://", 29);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 29;
#else
		memcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"", 17);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 17;
#endif
		if(checkIP((char*)gMobileParameter.purIP) == 0){ //if(memcmp(gMobileParameter.purIP, "\x00\x00\x00\x00\x00", 5) == 0)
#ifdef _MOBILE_IP_CHANGSHA_
			memcpy((char*)gpsrsP+i, "211.138.236.210", 15);//��ɳ
#endif
#ifdef _MOBILE_IP_HUBEI_
			memcpy((char*)gpsrsP+i, "172.031.011.193", 15);
#endif
#ifdef _MOBILE_IP_FUJIAN_
			memcpy((char*)gpsrsP+i, "218.207.183.082", 15);//��ɳ
#endif
#ifdef _MOBILE_IP_XiAN_
#ifdef MOBILE_XIANYANG_
			memcpy((char*)gpsrsP+i, "010.129.005.100", 15);//����
#else 
			memcpy((char*)gpsrsP+i, "211.137.133.038", 15);//ͭ��
#endif
#endif
#ifdef _MOBILE_IP_QINHAI
			memcpy((char*)gpsrsP+i, "218.206.165.003", 15);
#endif
			;
		}
		else
			memcpy(gpsrsP+i, gMobileParameter.purIP, 15);
		i+=15;
#ifdef GPRS_MG323_
		gpsrsP[i] = ':';
		i++;
#else
		memcpy(gpsrsP+i, "\",", 2);
		i += 2;
#endif
// 			debugstring("..........");
// 			debugstring((char*)gpsrsP);
// 			debugstring((char*)gMobileParameter.purIP);
//			if(memcmp(gMobileParameter.purPort, "\x00\x00\x00\x00", 4) == 0){
#ifdef _MOBILE_IP_HUBEI_
			memcpy((char*)gpsrsP+i, "36007", 5);
			i += 5;
#endif
#ifdef _MOBILE_IP_CHANGSHA_
			memcpy((char*)gpsrsP+i, "26207", 5);
			i += 5;
#endif
#ifdef _MOBILE_IP_FUJIAN_
			memcpy((char*)gpsrsP+i, "9092", 4);//����
			i += 4;
#endif
#ifdef _MOBILE_IP_XiAN_
			memcpy((char*)gpsrsP+i, "8000", 4);//��ɳ
			i += 4;
#endif
#ifdef _MOBILE_IP_QINHAI
			memcpy((char*)gpsrsP+i, "8000", 4);
			i += 4;
#endif
//			}
//			else{
#ifdef _MOBILE_IP_NOLOCK_
			t = strlen((char*)gMobileParameter.purPort);
			if(t>= 6)
				t = 5;
			memcpy(gpsrsP+i, gMobileParameter.purPort, t);
			i += t;
#endif
//				strcat((char*)gpsrsP, (char*)gMobileParameter.purPort);
//			}
#ifdef GPRS_MG323_
			memcpy((char*)gpsrsP+i, "\"\r\n", 3);
			i+=3;
#elif defined CDMA_MC323_
			memcpy((char*)gpsrsP+i, ",1000\r\n", 7);
			i+=7;
#else
			memcpy((char*)gpsrsP+i, "\r\n", 2);
			i+=2;
#endif
#endif
	}
	else if((gmissflag & 0xf0)==0x20){//��½�ƶ�IF2
#ifdef NO_MOBILE_SERVISE
		gmissflag = 0;
		set_timer0(1000, 1);
		return -1;
#else

		if(isNetOK[0] != 0){
			tcpipClose(5);
			set_timer0(1000, 1);
			return -1;
		}
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y')
		{
			GPRSLinkProcess = 16;
			out_GPRS();
			return -1;
		}
#endif
		memset(gpsrsP, 0, 100);
//			strcpy((char*)gpsrsP, "AT%IPOPENX=1,\"TCP\",\"211.138.236.210\",26202\r\n");//��һ��IF2 tcp����
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"211.138.236.210\",26202\r\n");//��һ��IF2 tcp����
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"172.31.11.193\",36002\r\n");//��һ��IF2 tcp����
#ifdef CDMA_MC323_
		memcpy((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"", 19);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 19;
#elif defined GPRS_MG323_
		memcpy((char*)gpsrsP, "at^siss=0,address,\"socktcp://", 29);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 29;
#else
		memcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"", 17);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 17;
#endif
		if(checkIP((char*)gMobileParameter.purIP) == 0){ //if(memcmp(gMobileParameter.purIP, "\x00\x00\x00\x00\x00", 5) == 0)
#ifdef _MOBILE_IP_CHANGSHA_
			memcpy((char*)gpsrsP+i, "211.138.236.210", 15);//��ɳ
#endif
#ifdef _MOBILE_IP_HUBEI_
			memcpy((char*)gpsrsP+i, "172.031.011.193", 15);
#endif
#ifdef _MOBILE_IP_FUJIAN_
			memcpy((char*)gpsrsP+i, "218.207.183.082", 15);//��ɳ
#endif
#ifdef _MOBILE_IP_XiAN_
#ifdef MOBILE_XIANYANG_
			memcpy((char*)gpsrsP+i, "010.129.005.100", 15);//����
#else 
			memcpy((char*)gpsrsP+i, "211.137.133.038", 15);//ͭ��
#endif
#endif
#ifdef _MOBILE_IP_QINHAI
			memcpy((char*)gpsrsP+i, "218.206.165.003", 15);
#endif
			;
		}
		else
			memcpy(gpsrsP+i, gMobileParameter.purIP, 15);
		i+=15;
#ifdef GPRS_MG323_
		gpsrsP[i] = ':';
		i++;
#else
		memcpy(gpsrsP+i, "\",", 2);
		i += 2;
#endif
//			if(memcmp(gMobileParameter.managePort, "\x00\x00\x00\x00", 4) == 0){
#ifdef _MOBILE_IP_HUBEI_
		memcpy((char*)gpsrsP+i, "36002", 5);
		i+=5;
#endif
#ifdef _MOBILE_IP_CHANGSHA_
		memcpy((char*)gpsrsP+i, "26202", 5);
		i+=5;
#endif
#ifdef _MOBILE_IP_FUJIAN_
		memcpy((char*)gpsrsP+i, "9091", 4);//����
		i+=4;
#endif
#ifdef _MOBILE_IP_XiAN_
		memcpy((char*)gpsrsP+i, "8001", 4);//��ɳ
		i+=4;
#endif
#ifdef _MOBILE_IP_QINHAI
		memcpy((char*)gpsrsP+i, "8002", 4);
		i+=4;
#endif
// 			}
// 			else{
#ifdef _MOBILE_IP_NOLOCK_
			t = strlen((char*)gMobileParameter.managePort);
			if(t >= 6)
				t = 5;
			memcpy(gpsrsP+i, gMobileParameter.managePort, t);
			i += t;
#endif
//				strcat((char*)gpsrsP, (char*)gMobileParameter.managePort);
// 			}
#ifdef GPRS_MG323_
			memcpy((char*)gpsrsP+i, "\"\r\n", 3);
			i+=3;
#elif defined CDMA_MC323_
			memcpy((char*)gpsrsP+i, ",1000\r\n", 7);
			i+=7;
#else
			memcpy((char*)gpsrsP+i, "\r\n", 2);
			i += 2;
#endif
#endif
	}
	else{// if((gmissflag & 0xf0)==MISS_GJ){//��½������������
#ifdef NO_GJ_SERVISE
		gmissflag = 0;
		set_timer0(1000, 1);
		i = -1;
#else//#ifdef NO_GJ_SERVISE
		if(isNetOK[0] != 0){
#ifdef _debug_gprs
			debugstring("in bsc 1:\r\n");
#endif
			set_timer0(1000, 1);
			tcpipClose(5);
 			return -1;
		}

#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'N')
		{
#ifdef _debug_gprs
			debugstring("in bsc 2:\r\n");
#endif
			GPRSLinkProcess = 16;
			out_GPRS();
			return -1;
		}
#endif
#ifdef _debug_gprs
		debugstring("in bsc 3:\r\n");
#endif
		memset((char *)gpsrsP, 0, 100);
#ifdef _debug_gprs
		debugstring("in bsc 4:\r\n");
#endif
//			strcpy((char*)gpsrsP, "AT%IPOPENX=1,\"TCP\",\"211.138.236.210\",26202\r\n");//��һ��IF2 tcp����
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"211.138.236.210\",26202\r\n");//��һ��IF2 tcp����
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"172.31.11.193\",36002\r\n");//��һ��IF2 tcp����
#ifdef CDMA_MC323_
		memcpy((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"", 19);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 19;
#elif defined GPRS_MG323_
		memcpy((char*)gpsrsP, "at^siss=0,address,\"socktcp://", 29);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i = 29;
#else
		memcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"", 17);//211.138.236.210\",26207\r\n");//��һ��tcp����
		i += 17;
#endif

//			ClientIP = getDefaultIP();//д��IP��ַ����
		memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
		sprintf((char*)gpsrsP+60, "%03u.%03u.%03u.%03u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
		if((memcmp(gpsrsP+50, "\x00\x00\x00\x00", 4) == 0)||(memcmp(gpsrsP+50, "\xFF\xFF\xFF\xFF", 4) == 0)){
			ClientIP = getDefaultIP();
			memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
			sprintf((char*)gpsrsP+60, "%03u.%03u.%03u.%03u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
		}
		memcpy(gpsrsP+i, gpsrsP+60, 15);
		i += 15;
#ifdef GPRS_MG323_
		gpsrsP[i] = ':';
		i++;
#else
		memcpy(gpsrsP+i, "\",", 2);
		i += 2;

#endif
		if((ClientPort == 0)||(ClientPort >= 9000)){
			ClientPort = getDefaultPort();
		}
		sprintf((char*)gpsrsP+50, "%u", ClientPort);
		t = strlen((char*)gpsrsP+50);
		if(t >= 6)
			t = 5;
		memcpy(gpsrsP+i, gpsrsP+50, t);
		i += t;
#ifdef GPRS_MG323_
		memcpy((char*)gpsrsP+i, "\"\r\n", 3);
		i+=3;
#elif defined CDMA_MC323_
		memcpy((char*)gpsrsP+i, ",1000\r\n", 7);
		i+=7;
#else
		memcpy((char*)gpsrsP+i, "\r\n", 2);
		i += 2;
#endif
		
#endif //#ifdef NO_GJ_SERVISE
	}
#ifdef _debug_gprs
	debugstring("in buildSevece:");
	debugstring(gpsrsP);
#endif
	return i;
}

void TaskRecWrite(void)
{
// #ifdef OTHER_GPRS
// 	unsigned short i=0;
// 	static unsigned char dvr_ack_outtime=0;		//���ػ���dvr���̵����ݣ�Ҫ��DVR�ظ��������и���ʱ�ȴ�
// 	#define DVR_OUTTIME	20
// 	#define DVR_STATUSTIME 40
// #endif

	

#ifndef NO_GJ_SERVISE
//	unsigned short i=0;
//	unsigned short sum;
	unsigned char buffer[1025];//[2100];
	unsigned int uilen;
//	int temp1;

//	sprintf((char*)buffer, "TaskRecWrite**%d:* ", gmobileErrTimes);
//	debugstring((char*)buffer);
#ifndef NO_MOBILE_SERVISE
	if(gmobileErrTimes >= 5){
		stopCCgprs(ERROR_CONNECT_MOBILE);
		gmobileErrTimes = 0;
	}
#endif
#ifdef SelMsg_enable
	if((gmissflag == MISS_G_LOGINGJ)&&(GPRSLinkProcess < 21)){
		find_G_new_mission();
		return;
	}
#endif
//	beep(1,100,100);
		//set_timer0(1000,1);
// #ifdef OTHER_GPRS
// 	dvr_ack_outtime++;
// 	if((dvr_ack_outtime% DVR_OUTTIME) == 0){
// 		sysferead(BIT_DVR_PACKET,6,buffer);
// 		if ((memcmp(buffer,"$$$$",4)) == 0)
// 		{
// 			memcpy((unsigned char *)&i,buffer+4,2);
// 			over_turn(2,(unsigned char *)&i);
// 			sysferead(BIT_DVR_PACKET,i,buffer);
// #ifdef _debug_
// 			com_snd(COM4, i, buffer);
// 			debugstring("\r\nsysread$$$$:");
// 			debugdata(buffer,i,1);
// #else
// 			com_snd(COM1, i, buffer);
// #endif
// 			gpacSNo = buffer[8];		//�����			
// 		}
// //		dvr_ack_outtime = 0;
// 		
// 	}
// 	if((dvr_ack_outtime% DVR_STATUSTIME) == 0){
// 	//	gprs_other_check_dvr();
// 	}
// 
// 	
// #endif


	if((GPRSLinkProcess > 14)&&((gmissflag == 0)||(gmissflag == MISS_G_FREE))){
#ifndef NO_MOBILE_SERVISE
		if((gmissflag == 0)||(gmissflag == MISS_G_FREE)){//û��������������
			find_new_mission();
		}
#endif	
		
		if((gmissflag == 0)||(gmissflag == MISS_G_FREE)||(gmissflag == MISS_G_LOGINGJ)){
			find_G_new_mission();
		}
		return;
	}

	if((GPRSLinkProcess != TCPSTARTSTAT)&&//��û����TCPIP
		(GPRSLinkProcess != GPRS_SENDING_CMD)){
		return;
	}

	if(isNetOK == 0){//GPRSLinkProcess = 21,�����Ѿ��Ͽ��ˣ���Ҫ��������
		GPRSLinkProcess = 20;
		return;
	}

	if(GPRSLinkProcess == GPRS_SENDING_CMD){//���ڵȴ��ϸ�����Ӧ��
#ifdef _debug_gprs
		debugstring("gSendOverTime:");
		debugdata((unsigned char*)&gSendOverTime, 1, 0);
#endif
		if(gSendOverTime > 0){
			gSendOverTime--;
#ifdef GPRS_MG323_
			if((gSendOverTime == 10)||(gSendOverTime == 5))//�쳬ʱ�ˣ�����ģ����û�����ݣ���ʱ������Ҳû���ϱ��ġ�
				MC55i_SISREAD();
#endif 
		}
		if(gSendOverTime == 0){//����Ӧǩ��ʱ
#ifdef _GPS_DIAODU_
			if(gmissflag == MISS_G_ALAM){
#ifdef _debug_
				debugstring("galamessage.style:");
				debugdata((unsigned char*)&galamessage.style, 1, 0);
#endif
				if(galamessage.style != 0x5A){
					gmissflag = 0;
#ifdef KEY_PAD
					gKeyRestorDis = 3;
#endif
#ifdef _debug_
					debugstring("��������ʧ��!\r\n");
#endif
				}
			}
#endif
			GPRSLinkProcess = TCPSTARTSTAT;
#ifndef NO_MOBILE_SERVISE
			if(gmissflag == MISS_M_TradeBatch){
				readmobilsendStyle();
				load_fe_SendSeril();
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
			}
#endif
			
#ifdef _debug_gprs
			debugstring("gGPRS_Cmd_outtime:");
			debugdata((unsigned char*)&gGPRS_Cmd_outtime, 1, 0);
#endif
			gGPRS_Cmd_outtime++;
			if(gGPRS_Cmd_outtime == 2){
				gmissflag = 0;
				gmissOldflag = 0;
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
			}
			if(gGPRS_Cmd_outtime > 4){//������մ������εĴ���
				if(gmissflag == MISS_M_ActiveTest)//����ǻ��죬����ִ��
				{
					gmissflag = 0;
					gmissOldflag = 0;
				}
				if(gmissflag & MISS_GJ){//����ǹ�������,��ֹͣ
					stopCCgprs(ERROR_BUS_CONNECT);
					gmissflag = 0;
					gmissOldflag = 0;
				}
				else {
					stopCCgprs(ERROR_CONNECT_MOBILE);
				}
#ifdef _debug_
				debugstring("���Ӵ���gGPRS_Cmd_outtime > 4,�Ͽ�---\r\n");
#endif
				gGPRS_Cmd_outtime = 0;
//				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
#ifdef WIFI_TLH_
				GPRSLinkProcess = GPRS_AT_WAIT;//GPRS_MODE_NEED_CLOSE;
#else
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
#ifdef GPRS_MG323_

				gGPRS_data_style.ucReadStyle=0;
#endif
#endif
			}
		}
		return;
	}
#ifdef _debug_f
	debugstring("gmissflag:");
	debugdata((unsigned char*)&gmissflag, 1, 0);
#endif
	
#ifndef NO_MOBILE_SERVISE
	if((gmissflag & MISS_IF1) == MISS_IF1){
		if(isNetOK[0] != 1){//û������IF1����������
#ifdef _debug_
			debugstring("û������IF1����������\r\n");
#endif
			if(isNetOK[0] != 0){
				if(gAPN_Style != 'Y')
					tcpipClose(5);
				else 
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
			}
			else
				GPRSLinkProcess = 20;
			return;
		}
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y'){
			tcpipClose(5);
			return;
		}
#endif

#ifdef _debug_
		debugstring("IF1:");
		debugdata((unsigned char*)&gmissflag, 1, 1);
#endif
		switch(gmissflag){
		case MISS_M_SINGIN:
			if(gmissOldflag != MISS_M_TradeBatch){
				if(F_FLAG_VARY.stop_flag){//����ǵ�վ״̬��������ǩ����
					gmissflag = 0;
					return;
				}
			}
			if(posnoIsOK(gMobileParameter.posNo) == 1)
			{
				gmissflag = MISS_M_DPARAM;
				return;
			}
			uilen = Build8583Packge(buffer, MISS_M_SINGIN);
//			strcpy((char*)buffer+1000, "000000000008000038000001C00010000089142620200909045A5A5A5A5A5A5A5A5A5A31303030323537333130303034363838383037333134313131313233390072D15600010180038000000001000000003230303930363033313333303030303032303039303833313130333434384646303130303030303230312020202020202020202020202020");
//			strcpy((char*)buffer+1000, "600000000008000038000001C00010000545162852200909045157515751575157515730303130323137333130303034343838383037333134353131343531310072D15600010180038000000001000000003230303930363239313131313131303032303039303930343131333835314646303130303030353530312020202020200000000000000000");
//			Ascii2BCD(buffer+1000, buffer, strlen((char*)buffer+1000));
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_Settlement:
			uilen = Build8583Packge(buffer, MISS_M_Settlement);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_TradeBatch:
			if(gMobilStyle.ISOK != 1){
				gmissflag = MISS_M_SINGIN;
				gmissOldflag = MISS_M_TradeBatch;
				gseflag = 0;
				return;
			}
			uilen = Build8583Packge(buffer, MISS_M_TradeBatch);
			if(uilen == 0){
				gmissflag = MISS_M_Settlement;
				return;//���������,�ٷ��ͻ���
			}
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		default:
			find_new_mission();//��Ѱ��������
			break;
		}
		return;
	}
	
	if((gmissflag & MISS_IF2) == MISS_IF2){
		if(isNetOK[0] != 2){//û������IF2����������
#ifdef _debug_
			debugstring("û������IF2����������\r\n");
#endif
			if(isNetOK[0] != 0){
				if(gAPN_Style != 'Y')
					tcpipClose(5);
				else 
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
			}
			else
				GPRSLinkProcess = 20;
			return;
		}
		
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y'){
			tcpipClose(5);
			return;
		}
#endif
		switch(gmissflag){
		case MISS_M_DPARAM:
			uilen = Build8583Packge(buffer, MISS_M_DPARAM);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_DPGRAM:
			gmissflag = 0;	//�����ֻ�POSPƽ̨���س���
			gmissOldflag = 0;	//�����ֻ�POSPƽ̨���س���
			break;
// 			uilen = Build8583Packge(buffer, MISS_M_DPGRAM);
// 			gprs_send_data(1, uilen, buffer);
// 			GPRSLinkProcess = GPRS_SENDING_CMD;
// 			gSendOverTime = DE_SendOverTime;
// 			break;
		case MISS_M_DPGRAMOK:
			gmissflag = 0;	//�����ֻ�POSPƽ̨���س���
			gmissOldflag = 0;	//�����ֻ�POSPƽ̨���س���
			break;
// 			uilen = Build8583Packge(buffer, MISS_M_DPGRAMOK);
// 			gprs_send_data(1, uilen, buffer);
// 			GPRSLinkProcess = GPRS_SENDING_CMD;
// 			gSendOverTime = DE_SendOverTime;
// 			break;
		case MISS_M_KEK:
			uilen = Build8583Packge(buffer, MISS_M_KEK);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_ActiveTest:
			uilen = Build8583Packge(buffer, MISS_M_ActiveTest);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		default:
			find_new_mission();//��Ѱ��������
			break;
		}
		return;
	}
#else //#ifndef NO_MOBILE_SERVISE
	if(((gmissflag & MISS_IF2) == MISS_IF2)||((gmissflag & MISS_IF1) == MISS_IF1)){
		gmissflag = 0;
		return;
	}
#endif//#ifndef NO_MOBILE_SERVISE
	else if((gmissflag & MISS_GJ) == MISS_GJ){
		if(isNetOK[0] != 3){//û������IF2����������
#ifdef _debug_
			debugstring("û�����ӹ�������������\r\n");
#endif
			if(isNetOK[0] != 0)
				tcpipClose(5);
			else
				GPRSLinkProcess = 20;
			return;
		}
#ifndef _MOBILE_CMNET_
#ifndef WIFI_TLH_
		if(gAPN_Style != 'N'){
			tcpipClose(5);
			return;
		}
#endif
#endif
		switch(gmissflag){
		case MISS_G_TOO:
		case MISS_G_LOGINGJ:
		case MISS_G_DBLKD:
		case MISS_G_DBLKI:
		case MISS_G_UREC:
		case MISS_G_DPRO:
		case MISS_G_DLine:
		case MISS_G_DSound:
		case MISS_G_DPROOK:
		case MISS_G_DLineOK:
		case MISS_G_DSoundOK:
		case MISS_G_HART:
		case MISS_G_GPS:
		case MISS_G_ALAM:
			uilen = BuildGJPackge(buffer, gmissflag);
			if(uilen == 0){
				gmissflag = MISS_G_FREE;
				break;
			}
// 			if(gGPSdly < 3){
// 				gGPSdly = 3;//Ҫ�����ݣ�GPS��Ϣ�ӳٷ���
// 			}
#ifdef _debug_gprs
			debugstring("TaskRecWrite_snd data:");
			debugdata(buffer, uilen, 1);
#endif
			gprs_send_data(1, uilen, buffer);
			if((gmissflag == MISS_G_HART)||(gmissflag == MISS_G_GPS)){
				gmissflag = MISS_G_FREE;
				return;//����Ҫ��Ӧ��
			}
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
		case MISS_G_FREE:
			break;
		default:
			gmissflag = 0;
			break;
		}
	}
#ifndef NO_MOBILE_SERVISE
	if((gmissflag == 0)||(gmissflag == MISS_G_FREE)){//û��������������
		find_new_mission();
	}
#endif

	if((gmissflag == 0)||(gmissflag == MISS_G_FREE)||\
		(gmissflag == MISS_G_LOGINGJ)){
		find_G_new_mission();
	}
//	debugstring("TASK END ");
#endif
	return;
}
void SetTcpPacketTTLTime(unsigned char val)
{
#ifndef NO_GJ_SERVISE
#ifdef WIFI_TLH_
	tcpipPacket.TTL = 0;
#else
	tcpipPacket.TTL = val;
#endif
	if(tcpipPacket.packetStatus!=RECORD_DEAL)
		tcpipPacket.packetStatus = 0;
//	tcpipClose(0);
#endif	//#ifndef NO_GJ_SERVISE
}


#ifndef NO_GJ_SERVISE
void aoutMcheckIP(void)
{							  
	unsigned char Ipbuf[50];
	memcpy(Ipbuf, (unsigned char*)&ClientIP, 4);
	sprintf((char*)Ipbuf+5, "%03u.%03u.%03u.%03u", Ipbuf[0],Ipbuf[1],Ipbuf[2],Ipbuf[3]);
	if((checkIP((char*)Ipbuf+5) == 0)&&(GJ_IP != 0x64646464)){	//IP����ȷ����IP�ó�Ĭ��ֵ
		ClientIP = GJ_IP;
		ClientPort = GJ_PORT;
		sysfewrite(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
		sysfewrite(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
		savesysbupara();
	}

}

unsigned char SetupClientIP(void)
{
	unsigned char i;
	unsigned int tm;
	unsigned char x,col;
	unsigned char buffer[2][20];
	unsigned char Ipbuf[50];
	unsigned int uitemp;
//	unsigned long ultempIP;
	
	memcpy(Ipbuf, (unsigned char*)&ClientIP, 4);
	sprintf((char*)Ipbuf+5, "%03u.%03u.%03u.%03u", Ipbuf[0],Ipbuf[1],Ipbuf[2],Ipbuf[3]);
	if(checkIP((char*)Ipbuf+5) == 0){
		ClientIP = GJ_IP;
	}

	memset(Ipbuf, 0, 4);
	tm=ClientIP;//ClientIP;
ip_start:
//	ultempIP=ClientIP;
	i=0;
	x=0;
	memset(buffer[0],0,20);
	memcpy(Ipbuf, (unsigned char*)&tm, 4);
	sprintf((char*)buffer[0],"%03u",Ipbuf[0]);
	i+=3;
	buffer[0][i]='.';
	i++;
	sprintf((char*)buffer[0]+i,"%03u",Ipbuf[1]);
	i+=3;
	buffer[0][i]='.';
	i++;
	sprintf((char*)buffer[0]+i,"%03u",Ipbuf[2]);
	i+=3;
	buffer[0][i]='.';
	i++;
	sprintf((char*)buffer[0]+i,"%03u",Ipbuf[3]);

	cls();
	if(gchn_eng == 'E')
		display(0,1,(char *)"sevice IP:",1);
	else
		display(0,1,(char *)"���÷�����IP:",1);
	display(2,1,(char*)buffer[0],0);
	buffer[0][18]=buffer[0][0];
	display(2,1,(char*)buffer[0]+18,1);
//	display(4,1,(char *)"���÷�����PORT:",1);
	memset(buffer[1], 0, 20);
	sprintf((char*)buffer[1], "%04d", ClientPort);
	buffer[1][4] = 0;
	display(4, 1, (char*)buffer[1], 0);
	col = 0;
	while(1)
	{
		delayxms(50);
		i=getkey(1);
		switch(i)
		{
#ifndef _New_Bu_mode_
		case KEY_F1:
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x>0) x--;
				else{
					col = 1;
					x = 3;
				}
			}
			else{
				if(x>0) x--;
				else{
					col = 0;
					x = 14;
				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x--;
			}

			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_F2:
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x<14) x++;
				else{
					col = 1;
					x = 0;
				}
			}
			else{
				if(x<3) x++;
				else{
					col = 0;
					x = 0;
				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x++;
			}
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_DOWN:
			if((buffer[col][x]<0x30)||(buffer[col][x]>0x39)) buffer[col][x]=0x30;
			if(buffer[col][x]==0x30) buffer[col][x]=0x39;
			else if(buffer[col][x]>0x30) buffer[col][x]--;
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
#endif
		case KEY_UP:
			if((buffer[col][x]<0x30)||(buffer[col][x]>0x39)) buffer[col][x]=0x39;
			if(buffer[col][x]==0x39) buffer[col][x]=0x30;
			else if(buffer[col][x]<0x39) buffer[col][x]++;
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_ESC:		//-10
//			ClientIP=tm;
			return 1;
		case KEY_ENT:		//ok
#ifdef _New_Bu_mode_
			
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x<14) x++;
				else{
					col = 1;
					x = 0;
				}
			}
			else{
				if(x<4) 
					x++;
				// 				else{
				// 					col = 0;
				// 					x = 0;
				// 				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x++;
			}
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
#else
		col = 1; 
		x = 4;
#endif
			if((col == 1)&&(x >= 4))
			{
				uitemp=(buffer[0][0]-0x30)*100+(buffer[0][1]-0x30)*10+(buffer[0][2]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][4]-0x30)*100+(buffer[0][5]-0x30)*10+(buffer[0][6]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][8]-0x30)*100+(buffer[0][9]-0x30)*10+(buffer[0][10]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][12]-0x30)*100+(buffer[0][13]-0x30)*10+(buffer[0][14]-0x30);
				if(uitemp>255) goto ip_error;
				
				Ipbuf[0]=(buffer[0][0]-0x30)*100+(buffer[0][1]-0x30)*10+(buffer[0][2]-0x30);
				Ipbuf[1]=(buffer[0][4]-0x30)*100+(buffer[0][5]-0x30)*10+(buffer[0][6]-0x30);
				Ipbuf[2]=(buffer[0][8]-0x30)*100+(buffer[0][9]-0x30)*10+(buffer[0][10]-0x30);
				Ipbuf[3]=(buffer[0][12]-0x30)*100+(buffer[0][13]-0x30)*10+(buffer[0][14]-0x30);
				memcpy((unsigned char*)&tm, Ipbuf, 4);		
				ClientIP=tm; 
				//debugdata((unsigned char *)&ClientIP, 4, 1);
				ClientPort = (buffer[1][0]-0x30)*1000+(buffer[1][1]-0x30)*100+(buffer[1][2]-0x30)*10+(buffer[1][3]-0x30);
				//debugdata((unsigned char*)&ClientPort, 2, 1);
				i=0;
				if(ClientIP<0x00FFFFFFl) i |= 0x10;
				if((ClientPort<100)||(ClientPort>9999)) i |= 0x20;
				if(i&0x30) {
					cls();
					if(gchn_eng == 'E'){
						display(0,0,(char *)"input error:",0);
						display(2,1,"data illegal",0);
					}
					else{
						display(0,0,"�������:",0);
						display(2,1,"IP��ַ���Ϸ�",0);
					}
					delayxs(1);
					goto ip_start;
				}

				sysfewrite(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
				sysfewrite(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
				cls();
				if(gchn_eng == 'E'){
					display(2,1,"complete!",0);
				}
				else
					display(2,4,"�������",0);
				savesysbupara();
				saveCpuInfo(3, Ipbuf);
				return 0; 
ip_error:
				cls();
				if(gchn_eng == 'E'){
					display(0,0,(char *)"input error:",0);
					display(2,1,"data illegal",0);
				}
				else{
					display(0,0,"�������:",0);
					display(2,1,"����ֵҪС��256",0);
				}
				goto ip_start;
			}
//			else
//			{
//				buffer[18]=buffer[x];
//				Display(2,1+x,buffer+18,0);
//				x++;
//				if((x==3)||(x==7)||(x==11)) x++;
//
//				buffer[18]=buffer[x];
//				Display(2,1+x,buffer+18,1);
//
//			}
			break;
		default:
			break;
		}
	}
}
/*
#ifdef WIFI_TLH_
void setWiFISSID(void)
{
	
	unsigned char oldSSID[10];
	unsigned char SSIDbuf[64];
	char buffer[2];
	unsigned char CMFlag;//�������ͣ�0���ݣ�1��д��2Сд
	unsigned char Slen, index, disflag, ret;

	disflag = 1;
	CMFlag = 0;

	memset(SSIDbuf, 0, sizeof(SSIDbuf));
	getWifiInfo(SSIDbuf, SSIDbuf+20);
	SSIDbuf[10] = 0; //���10���ֽ� 
	memcpy(oldSSID, SSIDbuf, 10);
	Slen = strlen((char*)SSIDbuf);
	index = Slen-1;
	
	cls();
	display(0,0," ����SSID ", DIS_CENTER|DIS_CONVERT);

	while(1)
	{
		delayxms(10);
		if(disflag == 1){
			miniDispstr(3, 0, "                      ", 0);
			miniDispstr(3,2,(char*)SSIDbuf,0);
			buffer[0] = SSIDbuf[index];
			buffer[1] = 0;
			if(buffer[0] == 0)
				buffer[0] = ' ';
			miniDispstr(3, 2+index, buffer, DIS_CONVERT);
			if(CMFlag == 0)
				display(6, 0, "F1-����  F2-����", 0);
			else if(CMFlag == 1)
				display(6, 0, "F1-��д  F2-����", 0);
			else if(CMFlag == 2)
				display(6, 0, "F1-Сд  F2-����", 0);
			disflag = 0;
		}

		ret =getkey(1);
		switch(ret)
		{
		case UP:
			if(CMFlag == 0){
				if((SSIDbuf[index]<='0')||(SSIDbuf[index]>'9'))
					SSIDbuf[index]='9';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<='A')||(SSIDbuf[index]>'Z'))
					SSIDbuf[index]='Z';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<='a')||(SSIDbuf[index]>'z'))
					SSIDbuf[index]='z';
				else
					SSIDbuf[index]--;
			}
			disflag = 1;
			break;
		case DOWN:			
			if(CMFlag == 0){
				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
					SSIDbuf[index]='0';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
					SSIDbuf[index]='A';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
					SSIDbuf[index]='a';
				else
					SSIDbuf[index]++;
			}
			disflag = 1;
			break;
		case ESC:
			if(index > 0){
				SSIDbuf[index] = 0;
				index--;
				disflag = 1;
			}
			else
				return;
			break;
		case F2:
			if(SSIDbuf[index] == 0)
				break;
			if(index >= 9)
				index = 0;
			else
				index++;
// 			if(CMFlag == 0){
// 				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
// 					SSIDbuf[index]='0';
// 				if((SSIDbuf[index]+3) > '9')
// 					SSIDbuf[index] = '9';
// 				else
// 					SSIDbuf[index] += 3;
// 			}
// 			else if(CMFlag == 1){
// 				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
// 					SSIDbuf[index]='A';
// 				if((SSIDbuf[index]+10) > 'Z')
// 					SSIDbuf[index] = 'Z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
// 			else if(CMFlag == 2){
// 				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
// 					SSIDbuf[index]='a';
// 				if((SSIDbuf[index]+10) > 'z')
// 					SSIDbuf[index] = 'z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
			
			disflag = 1;
			break;
		case F1:
			if(CMFlag >= 2)
				CMFlag = 0;
			else
				CMFlag++;
			disflag = 1;
			break;
		case ENTER:
#ifdef _debug_
			debugdata(SSIDbuf, 10, 1);
#endif
			if(memcmp(SSIDbuf, oldSSID, 10) != 0){
				saveCpuInfo(10, SSIDbuf);
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//tcpipClose(0);
			}
			return;
		default:
			break;
			
		}
	}
	
}

void setWiFIKEK(void)
{

	unsigned char oldKEK[20];
	unsigned char SSIDbuf[64];
	char buffer[2];
	unsigned char CMFlag;//�������ͣ�0���ݣ�1��д��2Сд
	unsigned char Slen, index, disflag, ret;

	disflag = 1;
	CMFlag = 0;

	memset(SSIDbuf, 0, sizeof(SSIDbuf));
	getWifiInfo(SSIDbuf+30, SSIDbuf);
	SSIDbuf[20] = 0; //���10���ֽ� 
	memcpy(oldKEK, SSIDbuf, 20);
	Slen = strlen((char*)SSIDbuf);
	index = Slen-1;
	
	cls();
	display(0,0," ������������ ", DIS_CENTER|DIS_CONVERT);

	while(1)
	{
		delayxms(10);
		if(disflag == 1){
			miniDispstr(3,0,"                      ",0);
			miniDispstr(3,2,(char*)SSIDbuf,0);
			buffer[0] = SSIDbuf[index];
			buffer[1] = 0;
			if(buffer[0] == 0)
				buffer[0] = ' ';
			miniDispstr(3, 2+index, buffer, DIS_CONVERT);
			if(CMFlag == 0)
				display(6, 0, "F1-����  F2-����", 0);
			else if(CMFlag == 1)
				display(6, 0, "F1-��д  F2-����", 0);
			else if(CMFlag == 2)
				display(6, 0, "F1-Сд  F2-����", 0);
			disflag = 0;
		}

		ret =getkey(1);
		switch(ret)
		{
		case UP:
			if(CMFlag == 0){
				if((SSIDbuf[index]<='0')||(SSIDbuf[index]>'9'))
					SSIDbuf[index]='9';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<='A')||(SSIDbuf[index]>'Z'))
					SSIDbuf[index]='Z';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<='a')||(SSIDbuf[index]>'z'))
					SSIDbuf[index]='z';
				else
					SSIDbuf[index]--;
			}
			disflag = 1;
			break;
		case DOWN:			
			if(CMFlag == 0){
				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
					SSIDbuf[index]='0';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
					SSIDbuf[index]='A';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
					SSIDbuf[index]='a';
				else
					SSIDbuf[index]++;
			}
			disflag = 1;
			break;
		case ESC:
			if(index > 0){
				SSIDbuf[index] = 0;
				index--;
				disflag = 1;
			}
			else
				return;
			break;
		case F2:
			if(SSIDbuf[index] == 0)
				break;
			if(index >= 19)
				index = 0;
			else
				index++;
// 			if(CMFlag == 0){
// 				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
// 					SSIDbuf[index]='0';
// 				if((SSIDbuf[index]+3) > '9')
// 					SSIDbuf[index] = '9';
// 				else
// 					SSIDbuf[index] += 3;
// 			}
// 			else if(CMFlag == 1){
// 				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
// 					SSIDbuf[index]='A';
// 				if((SSIDbuf[index]+10) > 'Z')
// 					SSIDbuf[index] = 'Z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
// 			else if(CMFlag == 2){
// 				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
// 					SSIDbuf[index]='a';
// 				if((SSIDbuf[index]+10) > 'z')
// 					SSIDbuf[index] = 'z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
			
			disflag = 1;
			break;
		case F1:
			if(CMFlag >= 2)
				CMFlag = 0;
			else
				CMFlag++;
			disflag = 1;
			break;
		case ENTER:
			if(memcmp(oldKEK, SSIDbuf, 20)!= 0){
				saveCpuInfo(11, SSIDbuf);
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//tcpipClose(0);
			}
			return;
		default:
			break;
			
		}
	}

}
#endif

  	*/



void SetSevice(void)
{
//	unsigned int ll;
	unsigned char buff[30];
	unsigned char bu[5];
	if(SetupClientIP()==0){
		memcpy(bu, (unsigned char*)&ClientIP, 4);
		sprintf((char*)buff, "%03d.%03d.%03d.%03d%04d", bu[0],bu[1],bu[2],bu[3],ClientPort);
		if(gErrorFlag&ERROR_BUS_CONNECT){
			gErrortimes[1] = 0;
		}
		if((GPRSLinkProcess >= 20)&&(GPRSLinkProcess < 0xB0)){
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
			gSendOverTime = 0;
			gmissflag = 0;
		}
	}
}
//---------------------------------------------------------------------------------

void delallBlacInbus(void)
{
#ifdef NO_GJ_SERVISE
	return;//û�й���ƽ̨�������
#else
	unsigned int ulTemp;
	unsigned char pp[64];

	if(gBusblkDownPro.Miss_PPack != 0)//�������غ�����
		return;
	
	memset(gBusVerInfo.busBLKVer, 0xff, 2);//�汾������
	ulTemp = FLASH_BLK_START;
	flasherase(ERASE64KBYTESECTOR, ulTemp);
	BlackLength =0;
	pFistVary.BlackListNum=0;
	//	pFistVary.BlackListNum_1=0;
	//	pFistVary.BlackListNum_2=0;
	
	// 	memset(pp, 0xff, 14);
	// 	memcpy(pp, gBusVerInfo.busBLKVer, 2);
	// 	memcpy(pp+2, (unsigned char*)&BlackLength, 4);
	// 	flashwrite(ulTemp, pp, 14);//д��������汾������
	
	memset(pp, 0x00, 32);
	memset((unsigned char *)&gBlackBlockNum, 0, 32);
	sysfewrite(BIT_BLACK_SUM, 4, (unsigned char *)&pFistVary.BlackListNum);
	sysfewrite(BIT_BLACK_2BLOK, 32, pp);
#endif	
}
unsigned int Build_GJ_HART(unsigned char *dat)
{
	memset(dat, 0xee, 20);
	return 2;
}

unsigned int Build_GJ_His_ACHO3003(unsigned char *dat, unsigned char *sndbuf)
{
	unsigned int len;
	unsigned short crc16=0;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x30;//CmdCode
	dat[len++] = 0x03;
	
	dat[len++] = 1;//GnCode
	
	memset(dat+len, 0, 4);//P1p2p3p4
	len+=4;
	
	dat[len++] = 0x00;//����=00�ֽ�
	dat[len++] = 0x18;

	memcpy(dat+len, sndbuf, 0x18);
	len += 0x18;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;	
}
extern void getbusticketVer(void);
extern void getCpuDeviceNO(unsigned char *dat);
//���͵�½����Ϣ����
//0x11:���ջ����ָ�� -2001: ���ջ���������ն˻�����ȡ�ն˻�������Ϣ��0x12 ������Ϣ
//0x21:�ն˻���ָ֪�� -���ͻ�����Ϣ,0x22:�ն˻���ָ֪�� - ����������Ϣ
//0x01:�ն�ǩ��ָ�� -1001:�ն˻���¼���ջ�
//BDBDBD00FBD8D810010100000000003C1001010000004A020020110428095944000000000000000007550101490132300000000000000000000000000000000000000000000000000000000059DA
unsigned int Build_GJ_LOGIN(unsigned char *dat, unsigned char mode)
{
#ifndef NO_MOBILE_24G_
	unsigned int sendenda;
	unsigned int ltemp1;
	unsigned char *ptrp;
#else
	int headp, curp;
#endif
	unsigned int ltemp;
	unsigned int i;
	unsigned int len;
	unsigned short crc16;

	unsigned char disbuf[50];
	
	getCpuDeviceNO(pFistVary.DeviceNo);
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//

#ifdef BUS_SERRIER_No
	getposSN((unsigned char*)disbuf, 0);//ȡPOS���ն˱��
	dat[len++] = 0;//һ��4�ֽڣ�ǰһ�ֽڲ�0
	Ascii2BCD(disbuf+10, dat+len, 6);//Address:�ն˻����
	len += 3;
#else 
	Get_SerialNumF4(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
#endif

	if((mode == 0x21)||(mode == 0x22)){//�ն˻���ָ֪�� -2002: �ն˻����������ն˻�������Ϣ����
		dat[len++] = 0x20;//CmdCode
		dat[len++] = 0x02;
	}
	else if(mode == 0x01){//�ն�ǩ��ָ�� -1001:�ն˻���¼���ջ�
		dat[len++] = 0x10;//CmdCode
		dat[len++] = 0x01;
	}
	else{// if((mode == 0x11)||(mode == 0x12)){//�ն˻���ָ֪�� -2002: �ն˻����������ն˻�������Ϣ����
		dat[len++] = 0x20;//CmdCode
		dat[len++] = 0x01;
	}
	
	dat[len++] = (mode&0x0F);//GnCode  ȡMODE�ĵ�4λ

	memcpy(dat+len, "BU", 2);//�豸���� P1 P2
	len += 2;
	memset(dat+len, 0x01, 2);//P3��P4
	len+=2;
	
	if((mode & 0x0F) == 0x01){//��Ҫ���ͻ�����Ϣ
		dat[len++] = 0x00;//����=60�ֽ�
		dat[len++] = 0x3C;
		
		dat[len++] = 0;
		memcpy(dat+len, pFistVary.LineNo, 2);//��·3B,��̨��6λBCD�������ߣ�����֧��
		len += 2;
#ifdef ZBCD_DRICENO
		ASCII2ZBcod((char *)pFistVary.DeviceNo, dat+len, 4);//����4B
#else
		Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);//����4B
#endif
		
		len +=4;
#ifdef BUS_Cloud_
		memcpy(dat+len, StufferNO, 4);//��Ա4B
#else
		memcpy(dat+len, DriveCardNo, 4);//��Ա4B
#endif
		len +=4;

		memcpy(dat+len, (unsigned char*)&SysTime.year_h, 7);
		len += 7;
		
		Ascii2BCD(GprsCardList, dat+len, 8);	//���ﴫ���ֻ�����
		len += 4;
		Ascii2BCD(GprsCardList+8, dat+len, 8);
		len += 4;
// #ifdef _GPS_ENABLE_  //���þ�γ����
// 		memcpy(dat+len, (unsigned char*)&gGpsInfo.Longitude, 4);//����4B
// 		len += 4;
// 		memcpy(dat+len, (unsigned char*)&gGpsInfo.Latitude, 4);//+γ��4B
// 		len += 4;
// #else
// 		memset(dat+len, 0, 4);//����4B
// 		len += 4;
// 		memset(dat+len, 0, 4);//+γ��4B
// 		len += 4;
// #endif
		
		memcpy(dat+len, PCityCode, 2);//���д���2B
		len += 2;
#ifdef CPU_JSB
		if (load_white_list() == 0)
		{
			dat[len] = 0xaa;		//���ߺ�̨Ҫ������
		}else{
			dat[len] = 0;
		}
		len ++;
		 dat[len++] = 0;
#else
		memcpy(dat+len, "\x01\x01", 2);//��ҵ����2B
		len += 2;
#endif
		getProVer();
		memcpy(dat+len, gBusVerInfo.busProVer, 2);//����汾
		over_turn(2, dat+len);
#ifdef _debug_gprs
		debugstring("busProVer:");
		debugdata(gBusVerInfo.busProVer, 2, 1);
#endif
		len += 2;

		if(memcmp(gBusVerInfo.busBLKVer, "\xff\xff", 2) == 0)
			memset(gBusVerInfo.busBLKVer, 0, 2);
		memcpy(dat+len, gBusVerInfo.busBLKVer, 2);//�������汾2B
#ifdef _debug_gprs
		debugstring("buBLKVer:");
		debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
		len += 2;
		memcpy(dat+len, gBusVerInfo.busVoiceVer, 2);//��Կ��Ϣ�汾2B
		over_turn(2, dat+len);

		len += 2;

		memcpy(dat+len, gBusVerInfo.busLineVer, 2);//��λ������Ϣ�汾2B
		over_turn(2, dat+len);
		len += 2;
		getbusticketVer();
		memcpy(dat+len, gBusTicketV.busticketVer, 2);//Ʊ����Ϣ�汾2B
		len += 2;

#ifdef CDMA_CASE_
		memcpy(dat+len, "7S", 2);//2B
#elif defined Card_snd1111
		memcpy(dat+len, "7X", 2);//2B
#elif defined Modem_BUS_
		memcpy(dat+len, "7X", 2);//2B
#elif defined _New_Bu_mode_
		memcpy(dat+len, "ET", 2);//2B
#else
		memcpy(dat+len, "ST", 2);//2B
#endif
		len += 2;
		
		memset(disbuf, 0, 50);
#ifndef NO_MOBILE_24G_
		if((gmissNotTrad&0xF0) == 0xA0){//���Ͳ��ɹ�
			memcpy(disbuf, "UErr,", 5);
		}
		else{
			sendenda = mobilsendStyle.EndAddr;
			
			ltemp = (sendenda-mobilsendStyle.StartAddr);//���ڷ��͵�����
			ltemp /= MOBILE_REC_LEN;
			ltemp1 = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;//��ǰ��
			ltemp1 /= MOBILE_REC_LEN;
			sprintf((char*)disbuf, "%d/%d,",ltemp,(ltemp+ltemp1));
		}
		i = strlen((char*)disbuf);
		if(gMobilStyle.ISOK == 1){//�Ƿ��Ѿ�ǩ���ɹ�
			disbuf[i++] = 'Y';
		}
		else{
			disbuf[i++] = 'N';
		}
		disbuf[i++] = ',';
		BCD2Ascii((unsigned char*)&gG24Ver, disbuf+i, 1);
		i+= 2;
		disbuf[i++] = ',';
		
		ptrp = (unsigned char*)(0x00103FE0);//POS�����к�
		memcpy(disbuf+i, ptrp+12, 4);
#else
		
		curp = Get_Record_point((unsigned char*)&headp, 0);
		ltemp=(curp - headp)/RECORD_LEN;
		
		sprintf((char*)disbuf, "%d",ltemp);
		i = strlen((char*)disbuf);
		disbuf[i++] = ',';
		if(F_FLAG_VARY.stop_flag){
			disbuf[i] = 'P';
		}
		else{
			disbuf[i] = 'R';
		}
		i = strlen((char*)disbuf);
		disbuf[i++] = 0;//���������
#endif
		
		memcpy(dat+len, disbuf, 15);//�������
		len += 15;
		SYSgetbussinessNO(dat+len);//���ֽ��̻����

		len += 2;
		dat[len] = 0;  //����
		len += 1;
	}
	else{
		dat[len++] = 0x00;//����=00�ֽ�
		dat[len++] = 0x00;
	}
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}

//���س���Ӧ��
unsigned int Build_GJ_DLine(unsigned char *dat)
{
	unsigned int len;
	unsigned int temp;
	unsigned short crc16=0;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x04;
	
	dat[len++] = gGPSDownLine.Miss_style;//GnCode
	
	memcpy(dat+len, gGPSDownLine.Miss_PPack, 2);//p1,p2�յ��İ����
	over_turn(2, dat+len);
	len += 2;
	
	memcpy((unsigned char *)&temp, gGPSDownLine.MISS_MAXPack, 4);//������
	memcpy((unsigned char *)&crc16, gGPSDownLine.Miss_PPack, 2);//��ǰ����
#ifdef debug_GJ_TLVDeal_
	debugstring("Build_GJ_DLINE:");
	debugdata((unsigned char *)&temp, 4, 1);
	debugdata((unsigned char *)&crc16, 2, 1);
	
#endif
	if(crc16 < temp)
		crc16 += 1;//��Ҫ��һ����
	else
		crc16 = 0xffff;//������ɣ�����Ҫ������
	if(temp == 0)
		crc16 = 1;
	
	memcpy(dat+len, (unsigned char*)&crc16, 2);//P3��P4 Ҫ����İ���� ���ΪFFFF,�������ն˴��ͳ���
	over_turn(2, dat+len);
	len+=2;
	
	dat[len++] = 0x00;//����=00�ֽ�
	dat[len++] = 0x00;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}

unsigned int Build_GJ_UREC(unsigned char *sbuf)
{
	unsigned short crc16; 
	//	unsigned char buff[10];
	if(tcpipPacket.packetStatus == RECORD_DEAL){//����Ҫ���͵�����
		if((tcpipPacket.length > 0)&&(memcmp(tcpipPacket.tcpBuffer, "\xBD\xBD\xBD", 3) == 0)){
			// 			Get_SerialNumF4(buff);
			// 			if(memcmp(tcpipPacket.tcpBuffer+3, buff, 4) != 0){
			// 				memcpy(tcpipPacket.tcpBuffer+3, buff, 4);
			// 			}
			crc16 = Getcrc16(tcpipPacket.tcpBuffer, tcpipPacket.length-2, 0xFF, 0xFF);//���¼���У����
			memcpy(tcpipPacket.tcpBuffer+tcpipPacket.length-2, (unsigned char*)&crc16, 2);//CRC32 У����
			
			memcpy(sbuf, tcpipPacket.tcpBuffer, tcpipPacket.length);
#ifdef _debug_
			debugstring("Build Snd rec:");
			debugdata(tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
#endif
			return (unsigned int)tcpipPacket.length;
		}
		tcpipPacket.packetStatus = 0;//�˼�¼��������
		sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
	}
	return 0;
}
//���س���Ӧ��
unsigned int Build_GJ_DPro(unsigned char *dat)
{
	unsigned int len;
	unsigned int temp;
	unsigned short crc16=0;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x02;
	
	dat[len++] = gBusDownPro.Miss_style;//GnCode
	
	memcpy(dat+len, gBusDownPro.Miss_PPack, 2);//p1,p2�յ��İ����
	over_turn(2, dat+len);
	len += 2;
	
	memcpy((unsigned char *)&temp, gBusDownPro.MISS_MAXPack, 4);//������
	memcpy((unsigned char *)&crc16, gBusDownPro.Miss_PPack, 2);//��ǰ����
	if(crc16 < temp)
		crc16 += 1;//��Ҫ��һ����
	else
		crc16 = 0xffff;//������ɣ�����Ҫ������
	if(temp == 0)
		crc16 = 1;

#ifdef debug_GJ_TLVDeal_
	debugstring("Build_GJ_DPro:\r\n");
	debugstring("MISS_MAXPack::");
	debugdata((unsigned char *)&temp, 4, 1);
	debugstring("��Ҫ�İ�::");
	debugdata((unsigned char *)&crc16, 2, 1);
#endif


	memcpy(dat+len, (unsigned char*)&crc16, 2);//P3��P4 Ҫ����İ���� ���ΪFFFF,�������ն˴��ͳ���
	over_turn(2, dat+len);
	len+=2;
	
	dat[len++] = 0x00;//����=00�ֽ�
	dat[len++] = 0x00;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}
//������Ӧ��
unsigned int Buid_GJ_DBLK(unsigned char *dat)
{
	unsigned int len;
	unsigned int temp;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x01;
	
	dat[len++] = gBusblkDownPro.Miss_style;//GnCode
	
	memcpy(dat+len, (unsigned char*)&gBusblkDownPro.Miss_PPack, 2);//p1,p2�յ��İ����
	over_turn(2, dat+len);
	len += 2;
#ifdef _debug_blk
// 	debugstring("Buid_GJ_DBLK ��ͷ��");
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
#endif

	temp = gBusblkDownPro.MISS_MAXPack;//������
	crc16 = gBusblkDownPro.Miss_PPack;//��ǰ����

	if(crc16 < temp)
		crc16 += 1;//��Ҫ��һ����
	else
		crc16 = 0xffff;//������ɣ�����Ҫ������


#ifdef _debug_blk
	debugstring("Build_GJ_DPro:");
	debugstring("MISS_MAXPack::");
	debugdata((unsigned char *)&temp, 4, 1);
	debugstring("��Ҫ�İ�::");
	debugdata((unsigned char *)&crc16, 2, 1);
#endif

	memcpy(dat+len, (unsigned char*)&crc16, 2);//P3��P4 Ҫ����İ���� ���ΪFFFF,�������ն˴��ͳ���
	over_turn(2, dat+len);
	len+=2;
	
	dat[len++] = 0x00;//����=00�ֽ�
	dat[len++] = 0x00;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}

unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode)
{
	unsigned int len = 0;
	//	unsigned int blen;
	
#ifdef _debug_
	debugstring("BuildGJPackge mode:");
	debugdata((unsigned char*)&mode, 1, 1);
#endif
	switch (mode)
	{
	case MISS_G_TOO:
		len = Build_GJ_LOGIN(revBuf, 0x22);
		break;
	case MISS_G_LOGINGJ://������½
		len = Build_GJ_LOGIN(revBuf, 0x01);
		break;
	case MISS_G_DBLKD://�º�����
	case MISS_G_DBLKI://�º�����
		debugstring("MISS_G_DBLKD\r\n");
		//		len = Buid_GJ_DBLK(revBuf, mode);
		break;
	case MISS_G_UREC://�ϴ���¼
		len = Build_GJ_UREC(revBuf);
		break;
#ifdef _GPS_DIAODU_
	case MISS_G_GPS:
		len = Build_GJ_GPS(revBuf);
		break;
	case MISS_G_ALAM:
		len = Build_GJ_ALAM(revBuf);
#ifdef _debug_
		debugstring("galamessage.reSendTimes:");
		debugdata((unsigned char*)&galamessage.reSendTimes, 1, 0);
#endif
		galamessage.reSendTimes++;
		if((galamessage.reSendTimes > 3)||(len == 0)){//�������Σ�ȡ��
			galamessage.style = 0xE1;
		}
		break;
#endif
	case MISS_G_DPRO://��������
	case MISS_G_DLine://��·����������
	case MISS_G_DSound://��������������
		debugstring("MISS_D_DPRO\r\n");
		// 		len = Build_GJ_DPro(revBuf, mode);
		break;
	case MISS_G_DPROOK:
	case MISS_G_DLineOK:
	case MISS_G_DSoundOK:
		debugstring("MISS_G_DPROOK\r\n");
		break;
	case MISS_G_HART://��������
		//		len = Build_GJ_HART(revBuf);
		debugstring("MISS_G_HART\r\n");
		break;
	default:
		len = 0;
		break;
	}
	return len;
}

//��֯���͵Ĺ�����¼��
unsigned int Build_GJRec_Send(unsigned char cnum, unsigned int *headp)//
{
	unsigned int len;
//	unsigned int temp;
	unsigned short crc16;
	unsigned int addr;
	unsigned char disbuf[256];
//	unsigned int i;
	unsigned char dat[500];

	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
	dat[len++] = 0x03;
	
	dat[len++] = 0x01;//GnCode
	
	
	sysferead(FE_PACKSERIAL, 2, dat+len);	//P1,p2�����к�
	memcpy((unsigned char*)&tcpipPacket.Serial, dat+len, 2);
	len += 2;

	memcpy(dat+len, (unsigned char**)&headp, 2);//p1,p2,p3,p4 ���ڱ���������¼�ĵ�ַ
	len += 2;
	
	dat[len++] = 0x00;//����=64�ֽ�
	dat[len++] = 0x40;
	
	addr = *headp;
//	for(i=0; i<cnum; i++){//ֻ����һ��
		flashread(addr, disbuf, RECORD_LEN);
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
		else{
			memcpy(dat+len, disbuf, RECORD_LEN);
			len+=RECORD_LEN;
		}

#ifdef _debug_gprs
		if(addr < FLASH2_START1)
			debugstring("���ɵ�ǰ���Ͱ�:");
		else
			debugstring("������ʷ���Ͱ�:");
		debugdata(dat+len, RECORD_LEN, 1);
#endif
		if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
			memset(disbuf, 0, RECORD_LEN);
			flashwrite(addr, disbuf, RECORD_LEN);//������д��ȫ0��
		}
		
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
#ifdef debug_GJ_TLVDeal_
	debugstring("���ɷ��Ͱ����:");
	debugdata((unsigned char*)&*headp, 4, 1);
	debugdata((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
	debugdata((unsigned char*)&tcpipPacket.length, 2, 1);
	debugdata((unsigned char*)&tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
#endif
	return len;
}


unsigned int Build_GJ_LINE_ACHO(unsigned char *dat, unsigned char Gncode, stLoadPtable *pp)
{
	unsigned int len;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	memcpy(dat+len, "\x40\x05", 2);
	len += 2;
	
	dat[len++] = Gncode;//GnCode
	
	memset(dat+len, 0, 4);//p1,p2�յ��İ����
	len += 4;
	
	dat[len++] = 0x00;//����=12�ֽ�
	dat[len++] = 12;
	memcpy(dat+len, pp->bussiness, 2);
	len += 2;
	memcpy(dat+len, pp->lineVer, 2);
	len += 2;
	memcpy(dat+len, pp->LineNo, 4);
	len += 4;
	memcpy(dat+len, pp->deviceNo, 4);
	len += 4;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}
#ifdef _GPS_DIAODU_
//mode = 0���ɹ� ������ʧ��
unsigned int Build_DIAODU_Echo(unsigned char *dat, unsigned short cmdcode, unsigned char Gncode, unsigned char mode, unsigned char *p1p2p3)
{
	unsigned int len;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&cmdcode, 2);
	len += 2;
	
	dat[len++] = Gncode;//GnCode
	
	memcpy(dat+len, p1p2p3, 3);//p1,p2,p3���Ȼ���
	len += 3;
	dat[len++] = 0x01;//���ر��

	dat[len++] = 0x00;//����=02�ֽ�
	dat[len++] = 0x05;
	dat[len++] = gDiaoDuCMD.ucCMD;
	dat[len++] = gDiaoDuCMD.ZLseiral[0];
	dat[len++] = gDiaoDuCMD.ZLseiral[1];
	if(mode == 0){
		dat[len++] = 0x90;//
		dat[len++] = 0x00;
	}
	else{
		dat[len++] = 0x10;//
		dat[len++] = 0x00;
	}

	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}
#endif
//mode = 0���ɹ� ������ʧ��
unsigned int Build_Echo(unsigned char *dat, unsigned short cmdcode, unsigned char Gncode, unsigned char mode)
{
	unsigned int len;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&cmdcode, 2);
	len += 2;
	
	dat[len++] = Gncode;//GnCode
	
	memset(dat+len, 0, 4);//p1,p2�յ��İ����
	len += 4;
	
	dat[len++] = 0x00;//����=02�ֽ�
	dat[len++] = 0x02;
	
	if(mode == 0){
		dat[len++] = 0x90;//
		dat[len++] = 0x00;
	}
	else{
		dat[len++] = 0x10;//
		dat[len++] = 0x00;
	}

	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}
extern unsigned int Build_GJRec_SendJTB(unsigned char cnum, unsigned int *headp);//;
void GJRec_Send(void)
{
	struct{
		unsigned int shadd;
		unsigned int sLen;
		unsigned char sdate[8];
		unsigned int scrc32;
	}hisda;
	unsigned int sum;
	unsigned int curp, headp;
	
	if(tcpipPacket.packetStatus==RECORD_DEAL)
	{
		return;
	}
	else
	{
		if(tcpipPacket.packetStatus>0)
			tcpipPacket.packetStatus--;
		if((tcpipPacket.packetStatus == 0) || (tcpipPacket.packetStatus > 60))
			tcpipPacket.packetStatus = 0;
		else
			return;
		//�ȴ����Ƿ�����ʷ��¼����
		sysferead(BIT_HISREC_SND, 20, (unsigned char *)&hisda);
		sum = __cpuCrc32((unsigned char *)&hisda, 16);
#ifdef _debug_
		debugstring("GJRec Send his data:");
		debugdata((unsigned char *)&hisda, 20, 1);
#endif
		if((hisda.scrc32 == sum)&&(hisda.sLen != 0)&&(hisda.sLen <= (FLASH2_BAKR_LEN/RECORD_LEN))&&(hisda.shadd < FLASH2_BAKR_LEN)){//����ʷ��¼
			headp = FLASH2_START1+hisda.shadd;
			sum = 1;//��Э�飬һ�δ�һ��
		//	Build_GJRec_Send(sum, &headp);

// #ifdef BUS_PRO_JTB
// 			Build_GJRec_SendJTB(sum, &headp);//
// #else
			Build_GJRec_Send(sum, &headp);
//#endif
			hisda.shadd = headp-FLASH2_START1;
			if(hisda.sLen >= 1)
				hisda.sLen -= 1;
			else
				hisda.sLen = 0;
			hisda.scrc32 = __cpuCrc32((unsigned char *)&hisda, 16);
#ifdef _debug_
			debugstring("GJRec Send his END:");
			debugdata((unsigned char *)&hisda, 20, 1);
#endif
			sysfewrite(BIT_HISREC_SND, 20, (unsigned char*)&hisda);
			return;
		}

		//---------------------------------------

		curp = Get_Record_point((unsigned char*)&headp, 0);
	//�����¼������û�м�¼�ɷ�������E2PROM��û�з��ɹ��ļ�¼
		if(curp>headp)
		{
#ifdef _debug_
			debugstring("Read Record From E2prom to TCP/IP !\r\n");
#endif
// 			sum=0;
// 			sum=(unsigned int)(curp - headp);
// 		
// 			if(sum > 384){//�����ܳ�����384���� 32�ֽ����12����64�ֽ����6��
// 				sum = (384/RECORD_LEN);
// 			}
// 			else
// 				sum /= RECORD_LEN;

			sum = 1;//��Э�飬һ�δ�һ��
// #ifdef BUS_PRO_JTB
// 			Build_GJRec_SendJTB(sum, &headp);//
// #else
			Build_GJRec_Send(sum, &headp);
//#endif
			
		}
		else{
			if(curp >= (FLASH1_START+FLASH_RCLR)){//0x3E800�����¼��ַ�Ѿ��ﵽ��40000����64�ֽڣ��������ô����������¿�ʼ
				//clr_Record_point();
				lib_clr_Record_point();//ԭ���õ��� clr_Record_point();   ���������ѱ������ĵ�ַҲ�������
				
			}
			tcpipPacket.packetStatus = 40;
		}
	}
}

void find_G_new_mission(void)//������һ���һ��
{
	unsigned short usTemp=0;
#ifdef _debug_
	unsigned char btf[20];
#endif

#ifdef SelMsg_enable
	if(findSelMsg() == 0){//�ж���Ϣ��Ҫ����
		gmissflag = MISS_G_MSG1;
		return;
	}
#endif

	if(gErrorFlag&ERROR_BUS_CONNECT){
#ifdef _debug_
		sprintf((char*)btf, "��:%u", gErrortimes[1]);
		debugstring((char*)btf);
#endif
		if(gErrortimes[1] > 0){
			if((gmissflag & MISS_GJ)!=0)
				gmissflag = 0;
			return;//�ϴ����Ӵ���,ʱ��û����������.
		}
	}
	
	gErrorFlag &= (~ERROR_BUS_CONNECT);// ��������ʶ
#ifdef BUS_JIUJIANG_
	if(tcpipPacket.TTL != 0){	//��ˢ�˿���û��ʱ�䣬������GPRS����
		gmissflag = 0;
		return;
	}
#endif

	memcpy((unsigned char*)&usTemp, gBusVerInfo.newbusBLKVer, 2);
	if(memcmp(gBusVerInfo.newbusBLKVer,"OK",2)!=0){//�������İ汾��.û��ȡ��
#ifdef _debug_gprs
		debugstring("��½��Ӧ���������ԣ��ط���Ӧ:");
		debugdata(gBusVerInfo.newbusBLKVer, 2, 1);
//		debugdata(btf, 7, 1);
#endif
		gmissflag = MISS_G_LOGINGJ;
		return;
	}
	//���س�����Ҫ��̨��������̨����汾��Ҫ��̨����������������������ж�����λ��

	if(BlackLength>BLACKSUM){//�����������������º�����
#ifdef _debug_
		debugstring("�����������������º�����\r\n");
#endif
		memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
		sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
		delallBlacInbus();
	}
	//������������Ҳ�Ǻ�̨�������ء�
	
#ifdef _GPS_DIAODU_
	if(galamessage.style == 0x5A)//5A ��ʾ��Ҫ����GPS��������Ϣ
	{
		gmissflag = MISS_G_ALAM;
		set_timer0(100,0);
		return;
	}
#endif
	if(tcpipPacket.packetStatus == RECORD_DEAL){//�м�¼����Ҫ����
		if(tcpipPacket.TTL == 0){
			gmissflag = MISS_G_UREC;//��������
			set_timer0(100, 0);//�ӿ���뷢������
		}
	}
	else{
		if(tcpipPacket.TTL == 0)
			GJRec_Send();//����GPRS��������
		Sendpincount++;
		gSendGLogin++;
		gmissflag = MISS_G_FREE;//������
		if(gSendGLogin > SEND_PIN_COUNT_){
			gSendGLogin = 0;
			gmissflag = MISS_G_TOO;
		}
	}
}

void load_black_list(void)
{
	unsigned int addr;
	unsigned short ii=0;
	unsigned char i;
#ifdef _debug_blk

	unsigned int j=0;
	unsigned char buffer[20];
#endif

#ifdef _debug_
	debugstring("���غ�����:::\r\n");
#endif
	sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
	sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		// 
#ifdef _debug_blk
	debugstring("BlackListNum:::");
	debugdata((unsigned char*)&pFistVary.BlackListNum,4,1); //����������
	debugdata((unsigned char*)&gBlackBlockNum[0],32,1);
#endif

	addr = FLASH_BLK_START;
	flashread(addr, gBusVerInfo.busBLKVer, 2);//�������汾
#ifdef _debug_blk
	debugstring("gBusVerInfo.busBLKVer:");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
	memcpy((unsigned char*)&ii, gBusVerInfo.busBLKVer, 2);
	
	for(i=0; i<16; i++){
#ifdef _debug_blk
		debugstring("BIT_BLACK_2BLOK:::");
		debugdata((unsigned char*)&gBlackBlockNum[i],2,1); //����������
#endif
		if(gBlackBlockNum[i] == 0xffff)
			gBlackBlockNum[i] = 0;
		if(gBlackBlockNum[i] > BLACKLEN)		//16�飬
		{
			gBlackBlockNum[i] = 0;
			ii = 0;
		}
	}

#ifdef _debug_blk
	for(i=0; i<16; i++){
		if(gBlackBlockNum[i] == 0)
			break;
		memset(buffer,0,4);
		debugdata(&i,1,0);
		debugstring("------");
		debugdata((unsigned char *)&gBlackBlockNum[i],2,0);
		debugstring("------");
		debugstring("\r\n");
		for (j=0;j<gBlackBlockNum[i];j++)
		{
			if(i == 0)
				addr = FLASH_BLK_START+(i*4096)+14+j*BLK_SNO_LEN;//��ǰ14�ֽ��ǰ汾��,�����Ҳ������
			else
				addr = FLASH_BLK_START+(i*4096)+j*BLK_SNO_LEN;//
			flashread(addr, buffer, BLK_SNO_LEN);
			debugdata(buffer,BLK_SNO_LEN,1);
			if (j%300 == 0)
			{
				clr_wdt();
			}
		}
	}
#endif


	if((ii == 0)||(ii == 0xffff)||(pFistVary.BlackListNum >= BLACKSUM)){//�������汾����ȷ����������ȷ����0��������
		delallBlacInbus();
		pFistVary.BlackListNum = 0;
	}
	
	if(pFistVary.BlackListNum > 0){
		if(gBlackBlockNum[0] == 0){
			ii = pFistVary.BlackListNum/BLACKLEN;	//16��
			for(i=0; i<ii; i++){
				gBlackBlockNum[i] = BLACKLEN;
			}
			sysfewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
			gBlackBlockNum[i] = pFistVary.BlackListNum%(BLACKLEN);
		}
	}

	addr+=14;
//	temp = pFistVary.BlackListNum*4;//����������
	BlackLength = pFistVary.BlackListNum;
#ifdef _debug_blk
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	debugstring("pFistVary.BlackListNum:");
	debugdata((unsigned char*)&pFistVary.BlackListNum, 4, 1);
	debugdata((unsigned char*)&gBlackBlockNum, 32, 1);
#endif

}


void ClrGPRSFE(void)
{
	unsigned int addr;

	addr = FLASH_BLK_START;
	flasherase(ERASE64KBYTESECTOR, addr);

	memset((unsigned char*)&tcpipPacket, 0, sizeof(tcpipPacket));
	sysfewrite(BIT_TCPIP_PACKET, 10, (unsigned char*)&tcpipPacket);

	memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));//������Ϣ����ȷ����ʼ��
	sysfewrite(BIT_BUSDownPRO_INFO, 27, (unsigned char*)&gBusDownPro);//ȡ�ϴ����ز�����,��������

	memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));//������Ϣ����ȷ����ʼ��
	sysfewrite(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//ȡ�ϴ����ز�����,��������
}

void loadGPRSFE(void)
{
	unsigned int i;//,j;
	unsigned short j;
//	unsigned char *ptrp;
	
	sysferead(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket.packetStatus);
#ifdef debug_GJ_TLVDeal_
	debugstring("tcpippacet packetStatus:");
	debugdata((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
	debugstring("tcpippacet len:");
	debugdata((unsigned char*)&tcpipPacket.length, 2, 1);
	debugstring("tcpippacet Serial:");
	debugdata((unsigned char*)&tcpipPacket.Serial, 2, 1);
#endif

	memset(gBusVerInfo.busLineVer, 0, 2);
	memset(gBusVerInfo.busVoiceVer, 0, 2);
	
	getProVer();
	
	sysferead(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//ȡ�ϴ����ز�����,��������
	sysferead(BIT_BUSDownPRO_INFO, sizeof(stbusdownpro), (unsigned char*)&gBusDownPro);//ȡ�ϴ����ز�����,��������
#ifdef debug_GJ_TLVDeal_
	debugstring("gBusDownPro:");
	debugdata((unsigned char*)&gBusDownPro, sizeof(stbusdownpro), 1);
#endif
	if(((gBusDownPro.Miss_style != 0x01) && (gBusDownPro.Miss_style != 0x02))||(gBusDownPro.MISS_flag != 0xEA)){
		memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));//������Ϣ����ȷ����ʼ��:EA 01 E2 02 00 00 02 00 F0 E1 02 00 20 11 05 16 10 43 30 00 00 00 00 00 00 00 00 
	}
	if(gBusDownPro.MISS_flag == 0xEA){
		memcpy((unsigned char*)&j, gBusDownPro.Miss_PPack, 2);
		memcpy((unsigned char*)&i, gBusDownPro.MISS_MAXPack, 4);
#ifdef debug_GJ_TLVDeal_
		debugstring("gBusDownPro 2:");
		debugdata((unsigned char*)&j, 2, 1);
		debugdata((unsigned char*)&i, 4, 1);
#endif
		if((j == 0) ||(j > i))
			memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));
	}
#ifdef debug_GJ_TLVDeal_
	debugstring("gBusDownPro 3:");
	debugdata((unsigned char*)&gBusDownPro, sizeof(stbusdownpro), 1);
#endif
	
//	memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));//���gBusblkDownPro ��gBusblkDownProָ��gbusblkbuf
	sysferead(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�������
#ifdef _debug_blk
	debugstring("������������������:::-----------\r\n");
	debugstring("Miss_cmdCode::");
	debugdata((unsigned char*)gBusblkDownPro.Miss_cmdCode, 1, 1);
	debugstring("Miss_style::");
	debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
	debugstring("Miss_PPack:::");
	debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
	debugstring("MISS_MAXPack:");
	debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
	debugstring("Miss_ver:::::");
	debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
	debugstring("Miss_ALL_LEn:");
	debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
	debugstring("proLen:::::::");

#endif
	load_black_list();

#ifdef CPU_JSB
	j= load_white_list();
#ifdef _debug_
	debugstring("��������:");
	debugdata((unsigned char *)&j,2,1);
	
#endif
#endif

	memset(gBusVerInfo.newbusBLKVer, 0, 2);
	memset(gBusVerInfo.newbusLineVer, 0, 2);
	memset(gBusVerInfo.newbusProVer, 0, 2);
	memset(gBusVerInfo.newbusVoiceVer, 0, 2);
	memset(gBusTicketV.newbusticketVer, 0, 2);
	
	gprsIndex = 0;
	Sendpincount = 0;
	gSendGLogin = 0;
	gsendError = 0;
	gGPRS_Cmd_outtime = 0;
#ifdef SelMsg_enable
	SelMsgInit();
#endif
}
void saveBlackInfo(void)
{
	unsigned int ulTemp;
	unsigned char i;
	unsigned char buf[20];
	unsigned char btt[14];

//	temp1 = BlackLength*4;
	
	ulTemp = FLASH_BLK_START;
/*	flasherase(ERASE64KBYTESECTOR, ulTemp);*/  //ֻд�汾�ţ�������

	memset(btt, 0xff, 14);
	memcpy(btt, gBusVerInfo.busBLKVer, 2);
	memcpy(btt+2, (unsigned char *)&BlackLength, 4);

	for(i=0;i<3;i++){
		flashwrite(ulTemp, btt, 14);//д��汾��
		flashread(ulTemp, buf, 14);
//	debugdata(buf, 14, 1);
		if(memcmp(buf, btt, 14) == 0)
			break;
	}
// 	ulTemp+=14;
// 	for(uiTemp=0;uiTemp<temp1;){
// 		flashwrite(ulTemp, blkBuf+uiTemp, 128);
// 		uiTemp+=128;
// 		ulTemp+=128;
// 	}
// 	sysfewrite(BIT_BLACK_SUM, 2, (unsigned char *)&BlackLength);//д�����������
// 	pFistVary.BlackListNum =BlackLength;
#ifdef _debug_
	debugstring("д���������\r\n");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	MSG_LOG("ˢ�º�ĺ�����:%d\r\n",BlackLength);
	
	ulTemp = FLASH_BLK_START;
	flashread(ulTemp, gBusVerInfo.busBLKVer, 2);
	debugstring("�����汾�ţ�");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
}

//ֻ��������
unsigned char BlackUpdataAddOrDel(unsigned char *ulCardNo,unsigned int blkLen,unsigned char *blkBuf)
{
	unsigned short i_start=0;
	unsigned short i_end;
	unsigned short i_cur;
	unsigned short invalid_point;
	unsigned char flag =0 ;
	unsigned long long ulTempBlackNo;
	unsigned long long ulTempCardNo;
	unsigned char tembuff[10];

	unsigned int i;
	memset(tembuff,0,10);
//���ƺ���������
#ifdef BUS_PRO_JTB
	memcpy((unsigned char *)&ulTempCardNo,ulCardNo+2,8);
#else
	memcpy((unsigned char *)&ulTempCardNo,ulCardNo,BLK_SNO_LEN);
#endif
//	memcpy((unsigned char *)&ulTempCardNo,ulCardNo,BLK_SNO_LEN);

	i_end =blkLen;
	
	if (blkLen ==0) 
	{
		memset(blkBuf,0,BLK_SNO_LEN);
	}
#ifdef _debug_
	MSG_LOG("��BlackLength:%d\r\n",BlackLength);
#endif

	if(BlackLength >= BLACKSUM)
		return 0;
#ifdef _debug_
	debugstring("����:");
	debugdata(ulCardNo, BLK_SNO_LEN, 1);
#endif
	if(blkLen == 0){
		memcpy(blkBuf,ulCardNo,BLK_SNO_LEN);
		return 1;
	}

	while(1)
	{
		clr_wdt();
		i_cur=i_start+(i_end-i_start)/2;
		invalid_point=i_cur*BLK_SNO_LEN;
	//	memcpy((unsigned char *)ulTempBlackNo,blkBuf+invalid_point,BLK_SNO_LEN);
#ifdef BUS_PRO_JTB//��ͨ����10���ֽ�  �ȽϺ���8���ֽ�
		memcpy((unsigned char *)&ulTempBlackNo,blkBuf+invalid_point+2,8);
#else
		memcpy((unsigned char *)ulTempBlackNo,blkBuf+invalid_point,BLK_SNO_LEN);
#endif
		
// #ifdef _debug_
// 	debugstring("invalid_point:");
// 	debugdata((unsigned char *)&invalid_point,2,1);
// 	debugstring("i_cur:");
// 	debugdata((unsigned char *)&i_cur,2,1);
// 	debugdata((unsigned char *)&ulTempBlackNo,8,1);
// #endif
			if (ulTempBlackNo ==ulTempCardNo) 
			{
				flag =0;
				break;
			}
			else if (ulTempCardNo > ulTempBlackNo) 
			{
				if(i_start==i_end)
				{	
						flag =1;
						i_cur =i_start;
						break;
				}
				else
				{
					i_start=i_cur+1;
					if(i_start == i_end){
						flag =1;
						break;
					}
				}
			}
			else if(ulTempCardNo < ulTempBlackNo) 
			{
				if(i_start==i_cur)
				{
					flag =1;
					break;
				}
				else
				{
					i_end=i_cur;//-1
				}
			
			}

		}

//��Ӵ���
// #ifdef _debug_
// 	debugstring("invalid_point:");
// 	debugdata((unsigned char *)&invalid_point,2,1);
// 	debugstring("i_start:");
// 	debugdata((unsigned char *)&i_start,2,1);
// 	debugstring("i_cur:");
// 	debugdata((unsigned char *)&i_cur,2,1);
// 	debugstring("i_end:");
// 	debugdata((unsigned char *)&i_end,2,1);
// 	debugdata((unsigned char *)&ulTempBlackNo,8,1);
// #endif
	if (flag ==1) 
	{
#ifdef _debug_
		debugstring("�ƶ������� ");
#endif			
		if (ulTempCardNo < ulTempBlackNo) //38.39
		{
#ifdef _debug_
			debugstring("===1\r\n");
			debugdata((unsigned char*)&i, 4, 1);
#endif
		}
		else if(ulTempCardNo > ulTempBlackNo)//38<39
		{
#ifdef _debug_
			debugstring("===2\r\n");
#endif			
			i_cur++;
		}
		else{
			return 0;
		}
		i = 4096-(i_cur+1)*BLK_SNO_LEN;
		memmove(blkBuf+(i_cur+1)*BLK_SNO_LEN,blkBuf+i_cur*BLK_SNO_LEN,i);
		memcpy(blkBuf+i_cur*BLK_SNO_LEN,ulCardNo,BLK_SNO_LEN);
		return 1;

	}
	else
	{
		flag =0;
		return 0;
	}
}

//ɾ��������
unsigned char DealGPrsBLOckDEC(unsigned char block, unsigned char *blkBuff)
{
	stAddBLKTEMP *blkTemp;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned int addr;
	unsigned int uit;
	unsigned char i, ucBcc;
#ifdef debug_GJ_TLVDeal_
	char disbuff[64];
#endif

	addr = FLASH_TEMP_START;
	while(addr < FLASH_TEMP_END){
		flashread(addr, buff, 14);
#ifdef debug_GJ_TLVDeal_
		debugstring("��ʱ������:");
		debugdata(buff, 14, 1);
#endif
		memset(buffT, 0xff, 20);
		if(memcmp(buff, buffT, 14) == 0){//��FFFFFF�ˣ�����
#ifdef debug_GJ_TLVDeal_
			debugstring("��ʱ��������:::\r\n");
#endif
			return ST_OK;
		}
		
		clr_wdt();
		blkTemp = (stAddBLKTEMP *)buff;

		memset(buffT,0,sizeof(buffT));
		if(memcmp(blkTemp->index, buffT, 14) == 0){//14�ֽڶ�Ϊ0 ˵�����λ��ԭ����һ��������Ȼ�����ˣ������ˡ������ǿ���ַ����
		//ȫ0 ^�Ľ��Ҳ��0��������ʱ�������һ������һ�������Ĵ����ˣ�����Ҳ�����жϣ�ֱ������
#ifdef _debug_blk
			debugstring("�մ�����---==\r\n");
#endif
			addr += 14;
			continue;
		}
		ucBcc = 0;
		for(i=0;i<13;i++)
			ucBcc ^= buff[i];
		if(ucBcc != blkTemp->BCC){
#ifdef debug_GJ_TLVDeal_
			debugstring("��ʱ������BCC�� ,ɾ��\r\n");
#endif
			flashwrite(addr,buffT,14);	//���
			addr += 14;
			continue;
		}
		if((blkTemp->DorI == BLK_ADD)){
#ifdef debug_GJ_TLVDeal_
			debugstring("���Ǽ�,������һ��;;;\r\n");
#endif
			addr += 14;
			continue;
		}
		uit = 0;
		memcpy((unsigned char*)&uit, blkTemp->index, 3); // �жϿ��
		if((uit >= (FLASH_BLK_START+((block) * 4096))) &&(uit < (FLASH_BLK_START+((block+1) * 4096)))){//�ڴ˿�ķ�Χ��
//			uit = uit - FLASH_BLK_START;
#ifdef debug_GJ_TLVDeal_
			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"�˼���������:::%d��\r\n",block);
			debugstring(disbuff);
#endif
			if(block == 0)
				uit = BinSCH(gBlackBlockNum[block], blkBuff+14, blkTemp->CardNo, BLK_SNO_LEN);//��һ��ǰ14�ֽ�Ϊ�汾��
			else
				uit = BinSCH(gBlackBlockNum[block], blkBuff, blkTemp->CardNo, BLK_SNO_LEN);
#ifdef debug_GJ_TLVDeal_
// 			MSG_LOG("ԭ��������:::");
// 			BCD_LOG(blkBuff+uit,4,1);
// 			BCD_LOG(blkBuff+uit+14,4,1);
			debugstring("ɾ��������1:");
			debugdata((unsigned char*)&uit, 4, 1);
			debugdata((unsigned char*)&gBlackBlockNum[block], 2, 1);
			
#endif
			if(uit == 0xffffffff){
				MSG_LOG("��Ȼ�Ҳ���!���������⣬�ǲ���Ҫɾ����!\r\n");
				addr += 14;
				continue;
			}
			if (block == 0)
			{
				memmove(blkBuff+14+uit, blkBuff+14+uit+BLK_SNO_LEN, ((gBlackBlockNum[block]*BLK_SNO_LEN)-uit));//2B���ţ�����14�ֽ�
			}else{
				memmove(blkBuff+uit, blkBuff+uit+BLK_SNO_LEN, ((gBlackBlockNum[block]*BLK_SNO_LEN)-uit));//???????
			}
			if(gBlackBlockNum[block] > 0)
				gBlackBlockNum[block]--;
			if(BlackLength > 0)
				BlackLength--;
			memset(buffT, 0, 14);
			flashwrite(addr, buffT, 14);
#ifdef debug_GJ_TLVDeal_
			
			debugstring("ɾ��һ��##############\r\n");
			debugstring("ɾ��������2:");
			debugdata((unsigned char*)&uit, 4, 1);
			debugdata((unsigned char*)&gBlackBlockNum[block], 2, 1);		
#endif
		}
		addr += 14;
	}
	return ST_OK;
}
//���Ӻ�����
unsigned char DealGPrsBLOckADD(unsigned char block, unsigned char *blkBuff)
{
	stAddBLKTEMP *blkTemp;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned int addr;
	unsigned char i, ucBcc;
	
#ifdef debug_GJ_TLVDeal_
	debugstring("���������:::\r\n");
#endif
	addr = FLASH_TEMP_START;
	while(addr < FLASH_TEMP_END){
		flashread(addr, buff, 14);
#ifdef debug_GJ_TLVDeal_
		debugstring("��ʱ������:");
		debugdata((unsigned char*)&addr, 4, 0);
		debugstring("||");
		debugdata(buff, 14, 1);
#endif
		clr_wdt();


		blkTemp = (stAddBLKTEMP *)buff;

		memset(buffT, 0xff, 20);
		if(memcmp(buff, buffT, 14) == 0){//��FFFFFF�ˣ�����
			return 0;
		}

		memset(buffT,0,sizeof(buffT));
		if(memcmp(blkTemp->index, buffT, 14) == 0){//14�ֽڶ�Ϊ0 ˵�����λ��ԭ����һ��������Ȼ�����ˣ������ˡ������ǿ���ַ����
			//ȫ0 ^�Ľ��Ҳ��0��������ʱ�������һ������һ�������Ĵ����ˣ�����Ҳ�����жϣ�ֱ������
#ifdef _debug_blk
			debugstring("�մ�����---==\r\n");
#endif
			addr += 14;
			continue;
		}

		
		ucBcc = 0;
		for(i=0;i<13;i++)
			ucBcc ^= buff[i];
// #ifdef _debug_blk
// 		debugstring("ucBcc:");
// 		debugdata(&ucBcc,1,1);
// 		debugstring("buff[13]:");
// 		debugdata(&buff[13],1,1);
// #endif
		if(ucBcc != blkTemp->BCC){
#ifdef debug_GJ_TLVDeal_
			debugstring("��ʱ������BCC�� ,ɾ��\r\n");
#endif
			flashwrite(addr,buffT,14);	//���
			addr += 14;
			continue;
		}
		if((blkTemp->DorI == BLK_DEC)){
#ifdef debug_GJ_TLVDeal_
			debugstring("������,������һ��;;;\r\n");
#endif
			addr += 14;
			continue;
		}
 		if(gBlackBlockNum[block] < BLACKLEN){//�˿��пռ䣬���Դ���

			#ifdef debug_GJ_TLVDeal_
			debugstring("�˿��пռ䣬���Դ���:::\r\n");
#endif
			if(block == 0){
				if(BlackUpdataAddOrDel(blkTemp->CardNo, gBlackBlockNum[block], blkBuff+14) == 1){
					gBlackBlockNum[block]++;
					BlackLength++;
#ifdef _debug_blk
					debugstring("����һ��##############\r\n");
#endif
				}
			}
			else{
				if(BlackUpdataAddOrDel(blkTemp->CardNo, gBlackBlockNum[block], blkBuff) == 1){
					gBlackBlockNum[block]++;
					BlackLength++;
#ifdef _debug_blk
					debugstring("����һ��##############\r\n");
#endif
				}
			}
			memset(buffT, 0, 14);
			flashwrite(addr, buffT, 14);
#ifdef _debug_blk
			debugstring("addr:::++++14:##############\r\n");
#endif
			addr +=14;
		}
		else{
#ifdef _debug_blk
			debugstring("�˿��Ѿ�����\r\n");
#endif
			return 1;//�˿��Ѿ�����
		}
	}
	return 1;
}




//������ʱ���������������� �ȴ���ɾ�����������ٴ������ӵ������������鵥������
unsigned char TempDealBlack(void)
{
	unsigned char blkTempbuff[5120];
	unsigned int addr, uiT;
	unsigned char block, tt;
	unsigned char addflag = 1;//�����1������������


	
#ifdef _debug_blk
	unsigned char buff[15];
	stAddBLKTEMP *blkTemp;
#endif


	
#ifdef debug_GJ_TLVDeal_
	unsigned int uitemp =0;
	char disbuff[64];
	debugstring("����������:::\r\n");
#endif

#ifdef _debug_blk
	addr = FLASH_TEMP_START;
	while(addr<FLASH_TEMP_END){
		clr_wdt();
		flashread(addr, buff, 14);
		blkTemp = (stAddBLKTEMP *)buff;
		uitemp = 0;
		memcpy(&uitemp ,blkTemp->index,3); 
		uitemp = (uitemp -FLASH_BLK_START)/4096;
		memset(disbuff,0,sizeof(disbuff));
		if (blkTemp->DorI == BLK_DEC)
		{
			sprintf(disbuff,"BLOCK:%02d||��%d||||",uitemp, blkTemp->DorI);
		}else{
			sprintf(disbuff,"BLOCK:%02d||��%d||||",uitemp, blkTemp->DorI);
		}
		BCD2Ascii(blkTemp->CardNo,(unsigned char *)disbuff+16,8);
		debugstring(disbuff);
		debugstring("\r\n");
		memset(blkTempbuff,0xff,sizeof(stAddBLKTEMP));
		if(memcmp(buff, blkTempbuff, sizeof(stAddBLKTEMP)) == 0)
			break;	//ֹͣ
		addr += 14;
		}
#endif



	addflag = 1;
	for(block=0; block<16; block++){//�ӵ�0�鿪ʼ����
		clr_wdt();
		addr = FLASH_BLK_START+(block*4096);//ÿ��4K
		tt = 0;
		if(block == 0){//����ǰ��İ汾��
			flashread(addr, blkTempbuff, 14);
			addr += 14;
			tt = 14;
		}
		for(uiT=0; uiT<(16); uiT++){
			flashread(addr, blkTempbuff+tt+(uiT*256), 256);
			addr += 256;
		}
#ifdef debug_GJ_TLVDeal_
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"gBlackBlockNum[%d]=%d\r\n",block,gBlackBlockNum[block]);
		debugstring(disbuff);
#endif
		if(gBlackBlockNum[block] == 0){//��һ������ʼû�к������ˣ�
			if(addflag == 0){//����û�����ӵģ��������
#ifdef debug_GJ_TLVDeal_
				debugstring("Temp ST_OK 4 \r\n");
#endif
				return ST_OK;
			}
		}
		else{
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp BLOckDEC 4 \r\n");
			debugstring("�������111::::\r\n");
#endif
			DealGPrsBLOckDEC(block, blkTempbuff);//����ʱ���У�����������еĿ��ţ����������ɾ��
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp BLOckDEC OK \r\n");
#endif
		}
		
		if(gBlackBlockNum[block]< BLACKLEN){//��ʣ��ռ�.�������ӵĺ�����BLACKLEN/16
			
#ifdef debug_GJ_TLVDeal_
			debugstring("��һ���пռ���::\r\n");
			debugstring("��������:111:::\r\n");
#endif
			if(addflag){//�������
#ifdef debug_GJ_TLVDeal_
				debugstring("��������::");
				memset(disbuff,0,sizeof(disbuff));
				sprintf(disbuff,"block:%d,%d��::\r\n",block,gBlackBlockNum[block]);
				debugstring(disbuff);
// 				debugstring("Temp Deal Black 2 ");
// 				debugdata((unsigned char*)&block, 1, 1);
#endif
				addflag = DealGPrsBLOckADD(block, blkTempbuff);
#ifdef _debug_blk
				if(addflag==1)debugstring("��������!!!\r\n");
#endif
			}
		}
		
		addr = FLASH_BLK_START+(block*4096);//ÿ��4K �˿�ɾ�������Ӷ��Ѿ�������  д������
		flasherase(ERASE4KBYTESECTOR, addr);
		if(block == 0){
			memset(blkTempbuff, 0xFF, 14);//��һ���ǰ汾�ţ����ں���д�룬����������д��FF
		}
#ifdef debug_GJ_TLVDeal_
		debugstring("Temp Deal Black 3 ");
#endif
		for(uiT=0; uiT<(16); uiT++){
			flashwrite(addr, blkTempbuff+(uiT*256), 256);
			addr += 256;
		}
		pFistVary.BlackListNum = BlackLength;
		
		sysfewrite(BIT_BLACK_2BLOK, sizeof(gBlackBlockNum), (unsigned char*)&gBlackBlockNum[0]);
		sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //������һ�������������������
		
	}
#ifdef debug_GJ_TLVDeal_
	debugstring("Temp Deal Black 4 ");
#endif
	return ST_OK;
}
//������յ��ĺ�������ȫ������ֱ�ӷŵ������������������ŵ���ʱ��
void DealGPRSBlack(unsigned char len, unsigned char *dat)
{
	unsigned int addr;
	unsigned int i,itemp=0;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned char cardNo[20];
	stAddBLKTEMP *blacktemp;
#ifdef _debug_blk
	char disbuff[50];
#endif

	if((gBusblkDownPro.Miss_style == 2)){//ȫ������ ֱ��д��
#ifdef _debug_blk
		debugstring("deal GPRS black  ALLL\r\n");
//		debugdata(dat,len*10,1);
#endif
		sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //����������
		sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		for(i=0;i<16;i++)
		{
			if(gBlackBlockNum[i] > BLACKLEN)		//ԭ1000�������۰�
				gBlackBlockNum[i] = 0;
		}
		BlackLength = pFistVary.BlackListNum;
		itemp = BlackLength/BLACKLEN; //������һ����

		if(itemp>=16)
			return;

		if(itemp == 0)
			addr = FLASH_BLK_START+14+(itemp*4096)+(gBlackBlockNum[itemp]*BLK_SNO_LEN); // ��Ҫд��������ĵ�ַ ǰ14�ֽ��ǰ汾��
		else
			addr = FLASH_BLK_START+(itemp*4096)+(gBlackBlockNum[itemp]*BLK_SNO_LEN); // ��Ҫд��������ĵ�ַ
		memset(buff, 0, 20);
		for(i=0; i<len; i++){//len Ҫ����ĺ���������
			clr_wdt();
			if(memcmp(dat+(i*10), buff, 8) == 0){
				break;	//���ȫ��0������Ϊ�Ѿ�û�к�������
			}
			else{
				flashwrite(addr, dat+(i*10), BLK_SNO_LEN);//���������
				BlackLength++;
				pFistVary.BlackListNum=BlackLength;
				itemp = (BlackLength-1)/BLACKLEN;
				if(itemp < 16)
					gBlackBlockNum[itemp] = (((BlackLength-1)%(BLACKLEN))+1);
				addr+=BLK_SNO_LEN;
			}
		}
#ifdef _debug_blk
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"ͳ������:%d\r\n",BlackLength);
		debugstring(disbuff);
		debugstring("gBlackBlockNum:");
		for (i=0;i<16;i++)
		{
			MSG_LOG("%d ",gBlackBlockNum[i]);
		}
		debugstring("\r\n");
#endif
		sysfewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //������һ�������������������
	}
	else{//����������
		addr = FLASH_TEMP_START;//��FLAHS����ӡ�
		memset(buffT, 0xff, 20);
		while(addr<FLASH_TEMP_END){
			clr_wdt();
			flashread(addr, buff, 14);
			if(memcmp(buff, buffT, 14) == 0)
				break;	//�˴���д��
			addr += 14;
		}
		if(addr >= FLASH_TEMP_END){
			debugstring("д���������:");
			debugdata((unsigned char*)&addr, 4, 1);
			return;
		}
		for(i=0; i<len; i++){//len Ҫ����ĺ���������
			clr_wdt();
			memset(buff, 0, sizeof(buff));
			if(memcmp(dat+(i*10), buff, 8) == 0){
				sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
				return;	//���ȫ��0������Ϊ�Ѿ�û�к�������
			}
// #ifdef _debug_blk
// 			debugstring("addr:");
// 			debugdata((unsigned char*)&addr, 4, 1);
// #endif
			clr_wdt();
			memcpy(cardNo, dat+(i*10), 10);//ÿ��������10������ɣ������ֽ� 0ɾ 1��
			itemp = IsInBlackList(cardNo);
#ifdef _debug_blk
			debugstring("cardNo:");
			debugdata(cardNo, 10, 1);
			debugdata((unsigned char*)&itemp, 4,1);
#endif
			memset(buff,0xff,14);
			blacktemp = (stAddBLKTEMP *)buff;
			if(itemp < FLASH_BLK_END){//�Ѿ����ں�������
				if(cardNo[9] == BLK_DEC){//��
					MSG_LOG("��_");
					blacktemp->DorI = BLK_DEC;
					memcpy(blacktemp->CardNo, cardNo, BLK_SNO_LEN);
				}//���ӾͲ���
			}
			else{//�����ں�������,ֻ������
				if(cardNo[9] == BLK_ADD){//��
					MSG_LOG("��_");
					blacktemp->DorI = BLK_ADD;
					memcpy(buff+4, cardNo, BLK_SNO_LEN);
				}//���ӾͲ���
			}

			memcpy(blacktemp->index, (unsigned char*)&itemp, 3);
			blacktemp->reserved[1] = 0xFF;
			
			for (itemp=0; itemp<13; itemp++)
			{
				blacktemp->BCC ^= buff[itemp];
			}
			flashwrite(addr, buff, 14);//д��
			addr += 14;
#ifdef _debug_blk
			MSG_LOG("дbuff:");
			debugdata(buff, 14, 1);;
#endif

		}
	}
}

#endif //#ifndef NO_GJ_SERVISE 

//extern void MobileCOMDeal(unsigned char val);
#ifndef NO_MOBILE_24G_
//��COM5����
void G24Card_handler(unsigned char value)
{
	unsigned char ii;
	
	ii = value;
	
	if(g24ComFlag == 0xaa){//��ͷ��Ϣ
		switch (g24ComRindex)
		{
		case 0:
			if (ii != 0x02)
				return;
			com1Rlen = 0;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 1:
			com1Rlen = ii*256;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 2:
			com1Rlen += ii;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		default:
			if(g24ComRindex >= 512){
				g24ComFlag = 0xee;//���մ���
				break;
			}
			g24ComRBuf[g24ComRindex++] = ii;
			if(g24ComRindex >= com1Rlen+5){
				g24ComFlag = 0x55;//�������
			}
		}
		return;
	}
	
	// 	if(gmissflag == MISS_COM_PARA){ 
	// 		MobileCOMDeal(ii);//������ƶ���������,���ƶ��Ĵ���ʽ
	// 		return;
	// 	}
}
#elif defined _CPU_24G_
void G24Card_handler(unsigned char value)
{
	unsigned char ii;
	
	ii = value;
	
	if(g24ComFlag == 0xaa){//��ͷ��Ϣ
		switch (g24ComRindex)
		{
		case 0:
			if (ii != 0x02)
				return;
			com1Rlen = 0;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 1:
			com1Rlen = ii*256;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 2:
			com1Rlen += ii;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		default:
			if(g24ComRindex >= 512){
				g24ComFlag = 0xee;//���մ���
				break;
			}
			g24ComRBuf[g24ComRindex++] = ii;
			if(g24ComRindex >= com1Rlen+5){
				g24ComFlag = 0x55;//�������
			}
		}
		return;
	}
	
	// 	if(gmissflag == MISS_COM_PARA){ 
	// 		MobileCOMDeal(ii);//������ƶ���������,���ƶ��Ĵ���ʽ
	// 		return;
	// 	}
}
#endif

extern unsigned char Time_OK;
//һ��������Ĵ�С
#define CMD_G_SIZE 256
//����ǵ�½��Ӧ����Ҫ�� gBusVerInfo.newbusBLKVer ֵ�ó� 1������᲻ͣ�ķ���½��Ϣ
unsigned char GJDataDeal(unsigned char *pakege)
{
#ifndef NO_GJ_SERVISE 
	unsigned short CmdCode=0;//ָ����
	//	unsigned short GnCode=0;//ָ�����
	unsigned short PallLen=0;//�������ճ��ȣ���ʱ���ذ�
	unsigned short Alen=0;//�˰��ܳ���
	unsigned short Tack=0, proLen=0;
	unsigned short crc16;

	unsigned char buftemp[300];
	unsigned char blktemp[32];
	unsigned char hadware[2];
//	unsigned char Tver[14];//Ӧ��Ĳ����汾
	unsigned char *p;
	unsigned short nestpLen;//�������ĳ���
	unsigned char *nestpP;//�Ƿ����ذ�������о�ָ����һ����
	unsigned int index,itemp, i, addr=0;
	unsigned char bbb;//
	stLoadPtable *pprice;//
	int iret;
	unsigned short ustemp=0;

#ifdef debug_GJ_TLVDeal_
	char disbuff[50];
#endif

#ifdef debug_GJ_TLVDeal_
	debugstring("GJdeal IF2 CMD:");
#endif
	Sendpincount = 0;

	memset(hadware, 0, 2);
	index = 0;
	p = pakege;
	nestpP = 0;

	memcpy((unsigned char*)&Alen, p+1, 2);//�����򳤶�
	nestpLen = 0;
	PallLen = Alen;
	p += 3;//ǰ�����ֽ��� ֡���ͺ�֡���ȣ��������޹�
#ifdef _debug_
	debugdata(p, PallLen, 1);
	debugstring("Miss_cmdCode::::");
	debugdata((unsigned char *)&gBusblkDownPro.Miss_cmdCode,2,1);
#endif
	if(memcmp(p+index, "\xBD\xBD\xBD" , 3) != 0)
	{
#ifdef debug_GJ_TLVDeal_
		debugstring("STX Error!!");
		debugdata(p, 10, 1);
#endif
		return 0;
	}
	index += 3;//������Դ��ַ 4�ֽ�

	gGprsResertTime = 0;//���յ����ݣ���0
//	memcpy((unsigned char *)&, p+index, 4);
	//ָ����(CmdCode)
	memcpy((unsigned char*)&CmdCode, p+GCMD_CmdCode, 2);
	over_turn(2, (unsigned char*)&CmdCode);
	//.ָ�����(GnCode)
//	GnCode = p[GCMD_GnCode];
	memcpy((unsigned char*)&Alen, p+GCMD_Ln, 2);//�����򳤶�
	over_turn(2, (unsigned char*)&Alen);

	crc16 = Getcrc16(p, (Alen+GCMD_Data), 0xFF, 0xFF);
	memcpy((unsigned char*)&proLen, (p+GCMD_Data+Alen), 2);//CRCֵ

// #ifdef debug_GJ_TLVDeal_
// 	debugstring("crc16:");
// 	debugdata((unsigned char*)&crc16, 4, 1);
// 	debugstring("crc16proLen:");
// 	debugdata((unsigned char*)&proLen, 4, 1);
// 	debugstring("Alen:");
// 	debugdata((unsigned char*)&Alen, 2, 1);
// 	debugstring("CmdCode:");
// 	debugdata((unsigned char*)&CmdCode, 2, 1);
// 	debugstring("GnCode:");
// 	debugdata(p+GCMD_GnCode, 1, 1);
// 	debugstring("GCMD_Data:");
// 	debugdata(p+GCMD_Data, Alen, 1);
// #endif



	if((Alen+21) < PallLen){//������������
		nestpP = pakege+Alen+21;
		nestpLen = (PallLen +3 - Alen - 21);
#ifdef debug_GJ_TLVDeal_
		debugstring("  ������һ����::");
		debugdata(nestpP, nestpLen, 1);
#endif 
	}
	
	Tack = crc16;
	over_turn(2, (unsigned char*)&Tack);
	if((crc16 != proLen)&&(Tack != proLen)){
#ifdef debug_GJ_TLVDeal_
		debugstring("crc16 ERROR!!!\r\n");
#endif
		if(CmdCode == 0x4002){
			Alen = Build_GJ_DPro(buftemp);
			gprs_send_data(1, Alen, buftemp);//����Ӧ��
		}
		goto GJdataDealEnd;
	}

	gSendGLogin = 0;//��Ӧ�𣬲���������

	switch(CmdCode)
	{
	case 0x1001://�ն�ǩ�� Ӧ��
#ifdef debug_GJ_TLVDeal_
		debugstring("�ն�ǩ��:");
		debugdata(p+GCMD_Data, 10, 1);
#endif
		memcpy((unsigned char*)&Tack, p+GCMD_Data+7, 2);
#ifdef debug_GJ_TLVDeal_
		debugstring("Tack:");
		debugdata((unsigned char*)&Tack,2,1);
#endif
		if(Tack == 0x0090){
			if(time_invalid(p+GCMD_Data) == 1){//ʱ����Ч��Уʱ
#ifdef debug_GJ_TLVDeal_
				debugstring("ʱ����Ч\r\n");
#endif
		
				GetDateTime();
				if(memcmp((unsigned char*)&SysTime.year_h, p+GCMD_Data, 5) != 0){
#ifdef debug_GJ_TLVDeal_
					debugstring("У׼:");
					debugdata(p+GCMD_Data, 7, 1);
#endif
					timewrite(p+GCMD_Data);
					Time_OK=LogOK;
				}
			}
			//memcpy(gBusVerInfo.newbusBLKVer, "\x00\x01", 2);//���յ��ĺ�������־��Ϊ1,�ж��Ƿ���Ҫ���·���½����
				memcpy(gBusVerInfo.newbusBLKVer, "OK", 2);//���յ��ĺ�������־��Ϊ1,�ж��Ƿ���Ҫ���·���½����
			gmissflag = 0;
#ifdef debug_GJ_TLVDeal_
			debugstring("ǩ���ɹ�========\r\n");
#endif
		}
		else{
#ifdef debug_GJ_TLVDeal_
			MSG_LOG("���̨ע��%04X\r\n",Tack);
#endif
			goto GJdataDealEnd;
		}
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;//��½ʧ�ܣ����������·���½����
	case 0x1102://�ն�ǩ��
		memcpy((unsigned char*)&Tack, p+GCMD_Data, 2);
		if(Tack == 0x0090){
			gmissflag = 0;
			stopCCgprs(ERROR_BUS_CONNECT);//ǩ�˳ɹ�,��ʱ����ʱ�������µ�½
		}
		gmissflag = 0;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;//��½ʧ�ܣ����������·���½����
	case 0x2001://���ջ����
#ifdef debug_GJ_TLVDeal_
		debugstring("���ջ����:");
		debugdata(p+GCMD_Data, 10, 1);
#endif
		if(time_invalid(p+GCMD_Data) == 1){//ʱ����Ч��Уʱ
			GetDateTime();
			if(memcmp((unsigned char*)&SysTime.year_h, p+GCMD_Data, 5) != 0){
#ifdef debug_GJ_TLVDeal_
				debugstring("У׼:");
				debugdata(p+GCMD_Data, 7, 1);
#endif
				timewrite(p+GCMD_Data);
			}
		}
		i = Build_GJ_LOGIN(buftemp, (0x10|p[GCMD_GnCode]));
		gprs_send_data(1, i, buftemp);//����Ӧ��
		if(gmissflag == 0x4C){//������ػ����ڷ���֪ͨ����ΪOK
			gmissflag = MISS_G_FREE;//�ÿ��У�
		}
		goto GJdataDealEnd;
//		break;
	case 0x3101://���ݵ�������
		break;
	case 0x3002://���ݶ������
		break;
	case 0x3003://�ɼ���ʷ��¼//index,itemp, i, addr=0;
		addr = 0;	//��ʼ��ַ
		index = 0;	//��¼������
		Get_SerialNumF4(buftemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("snd 3003:1:");
		debugdata(p, Alen, 1);
		debugdata(buftemp, 4, 1);
		debugdata(p+GCMD_Data+6, 4, 1);
#endif
		if(memcmp(buftemp, p+GCMD_Data+6, 4) != 0){//Ψһ�Ų�һ��
			memcpy(blktemp, p+GCMD_Data, 6);//�����������̻���� 2byte (BCD)+������� 4Byte (BCD)+�豸���4Byte (HEX)+��ʼ���� 4Byte (BCD)+ ��ֹ���� 4Byte (BCD)
			memcpy(blktemp+6, buftemp, 4);
			memset(blktemp+10, 0, 12);
			memcpy(blktemp+22, "\x20\x00", 2);
			goto GJDEAL3003;
		}
#ifdef ZBCD_DRICENO
		ASCII2ZBcod((char *)pFistVary.DeviceNo, blktemp, 4);
#else
		Ascii2BCD(pFistVary.DeviceNo, blktemp, 8);
#endif

		if(memcmp(blktemp, p+GCMD_Data+2, 4) != 0){//�豸�Ų�һ��
			memcpy(blktemp, p+GCMD_Data, 6);//�����������̻���� 2byte (BCD)+������� 4Byte (BCD)+�豸���4Byte (HEX)+��ʼ���� 4Byte (BCD)+ ��ֹ���� 4Byte (BCD)
			memcpy(blktemp+6, buftemp, 4);
			memset(blktemp+10, 0, 12);
			memcpy(blktemp+22, "\x10\x00", 2);
			goto GJDEAL3003;
		}
 
		iret = getHisAddr(p+GCMD_Data+10, p+GCMD_Data+14, &addr, &itemp);	//����ԭ�������⣬�Ѿ��޸�150302  quan
		if((iret != 0)||(addr == itemp)){//û�м�¼
			memcpy(blktemp, p+GCMD_Data, 6);
			memcpy(blktemp+6, buftemp, 4);
			memset(blktemp+10, 0, 12);
			memcpy(blktemp+22, "\x30\x00", 2);
			goto GJDEAL3003;
		}
		else{//�м�¼
			i = ((itemp+FLASH2_BAKR_LEN-addr)%FLASH2_BAKR_LEN);
			i /= RECORD_LEN;
#ifdef _debug_
			debugstring("w hisrec 1:");
			debugdata((unsigned char*)&addr, 4, 1);
			debugdata((unsigned char*)&itemp, 4, 1);
			debugdata((unsigned char*)&i, 4, 1);
#endif
			memcpy(blktemp, (unsigned char*)&addr, 4);//�Ȱ�Ҫ��������д������
			memcpy(blktemp+4, (unsigned char*)&i, 4);
			memcpy(blktemp+8, (unsigned char*)&SysTime, 7);
			blktemp[15] = 0;
			index = __cpuCrc32(blktemp, 16);
			memcpy(blktemp+16, (unsigned char*)&index, 4);
#ifdef _debug_
			debugstring("w hisrec 3003:");
			debugdata(blktemp, 20, 1);
#endif
			sysfewrite(BIT_HISREC_SND, 20, blktemp);
			

			memcpy(blktemp, p+GCMD_Data, 6);
			memcpy(blktemp+6, buftemp, 4);
			memcpy(blktemp+10, (unsigned char*)&addr, 4);
			memcpy(blktemp+14, (unsigned char*)&itemp, 4);
			memcpy(blktemp+18, (unsigned char*)&i, 4);
			memcpy(blktemp+22, "\x90\x00", 2);
		}
		
GJDEAL3003:
		i = Build_GJ_His_ACHO3003(buftemp, blktemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("snd 3003::");
		debugdata(buftemp, i, 1);
#endif
		gprs_send_data(1, i, buftemp);//����Ӧ��
		break;
	case 0x4001://�������´�

#ifdef debug_GJ_TLVDeal_
		debugstring("�������´�:");
		debugdata(p+GCMD_GnCode, 1, 1);
#endif

		if(memcmp(gBusblkDownPro.Miss_cmdCode, "\x40\x05", 2) == 0){//���������ʷ�
#ifdef debug_GJ_TLVDeal_
			debugstring("���������ʷ�\r\n");
#endif
			break;
		}
// 		if(gBusDownPro.MISS_flag == 0xEA){//���ڸ��³�����ͣ�º�����
// #ifdef debug_GJ_TLVDeal_
// 			debugstring("���س����У�������ͣ\r\n");
// #endif
// 			break;
// 		}
		
		gBusblkDownPro.Miss_time = GPRS_Login_dly;
		if((p[GCMD_GnCode] == 1)||(p[GCMD_GnCode] == 3)){//��ͷ
			memcpy((unsigned char*)&crc16, p+GCMD_Ln, 2);
			over_turn(2, (unsigned char*)&crc16);
#ifdef debug_GJ_TLVDeal_
			debugstring("�������´�2:");
			debugdata((unsigned char*)&crc16, 2, 1);
#endif
			if(crc16 != 0x0A){
				debugstring("������ͷ���ȴ���\r\n");
				return 0;
			}
			memcpy(gBusblkDownPro.Miss_cmdCode, "\x40\x01", 2);
			gBusblkDownPro.Miss_style = p[GCMD_GnCode];

			if(gBusblkDownPro.Miss_PPack >= gBusblkDownPro.MISS_MAXPack) //��ǰ�������������غ�����
				gBusblkDownPro.Miss_PPack = 0;
			if((gBusblkDownPro.Miss_PPack > 0) && (memcmp(gBusblkDownPro.Miss_ver, p+GCMD_Data+4, 2) != 0))//�汾����ȷ���������غ�����
				gBusblkDownPro.Miss_PPack = 0;

			memcpy(blktemp, p+GCMD_Data, 4);
			over_turn(4, blktemp);
			memcpy((unsigned char*)&gBusblkDownPro.MISS_MAXPack, blktemp, 4);

			memcpy(gBusblkDownPro.Miss_ver, p+GCMD_Data+4, 2);
			memcpy((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, p+GCMD_Data+6, 4);
			over_turn(4, (unsigned char*)&gBusblkDownPro.Miss_ALL_LEn);
#ifdef debug_GJ_TLVDeal_
			debugstring("������ ��ͷ:\r\n");
			debugstring("Miss_style::");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
			debugstring("Miss_PPack:::");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
			debugstring("MISS_MAXPack:");
			debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
			debugstring("Miss_ver:::::");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
			debugstring("Miss_ALL_LEn:");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
			debugstring("proLen:::::::");
			debugdata((unsigned char*)&proLen, 2, 1);
#endif
			if((gBusblkDownPro.Miss_ALL_LEn == 0)||(gBusblkDownPro.MISS_MAXPack == 0)||
				(memcmp(gBusVerInfo.busBLKVer, gBusblkDownPro.Miss_ver, 2) == 0))//����µİ汾�źͳ��ػ��Ѿ��е���һ���ģ�������
			{
				memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
				sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
				goto BLKSENDECHO;
			}

			if(p[GCMD_GnCode] == 1){//�����ȫ�����ص�ͷ�������������
#ifdef _debug_blk
				debugstring("ȫ�����ذ�ͷ======\r\n");
				
#endif

				if(gBusblkDownPro.Miss_PPack == 0){
#ifdef _debug_blk
					debugstring("�������======\r\n");
#endif
					delallBlacInbus();//������ں�����
					memcpy(gBusVerInfo.busBLKVer, "\x00\x01", 2);
					restore_flag = 3;
				}
				addr = FLASH_TEMP_START;
				flasherase(ERASE64KBYTESECTOR, addr);//�����ʱ��
			}
			else{//�����������أ������ʱ��
#ifdef _debug_blk
				debugstring("���������ذ�ͷ======\r\n");
#endif
				restore_flag = 3;
				addr = FLASH_TEMP_START;
				flasherase(ERASE64KBYTESECTOR, addr);//�����ʱ��
			}
			sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
		}
		else if((p[GCMD_GnCode] == 2)||(p[GCMD_GnCode] == 4)){//����
			if(Alen < 10)//���ݳ��Ȳ���ȷ��������
				break;
			memcpy(gBusblkDownPro.Miss_cmdCode, "\x40\x01", 2);
			gBusblkDownPro.Miss_style = p[GCMD_GnCode];

			memcpy((unsigned char*)&proLen, p+GCMD_CmdPara, 2);//�����
			over_turn(2, (unsigned char*)&proLen);
#ifdef debug_GJ_TLVDeal_
			debugstring("������ ����:\r\n");
			debugstring("Miss_style::");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
			debugstring("MISS_MAXPack:");
			debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
			debugstring("Miss_ver:::::");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
			debugstring("Miss_ALL_LEn:");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
			debugstring("Miss_PPack:::::::::");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
			debugstring("�·���proLen:::::::");
			debugdata((unsigned char*)&proLen, 2, 1);
			
			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"alen:%d,����:%d\r\n",Alen,Alen/10);
			debugstring(disbuff);
#endif
			

//*********************************************
			if (proLen != gBusblkDownPro.Miss_PPack +1)		//��������Ҫ����һ������
			{
				MSG_LOG("������Ҫ�İ�\r\n");
				goto BLKSENDECHO;		//�����Ҫ��һ���������Ǻ�̨û���յ��ҵĻ�Ӧ��Ȼ�󻹻���ҷ���ǰ���ģ����ʱ�����������
			}	
			if(p[GCMD_GnCode] == 2){						//ȫ������Ҫ���汾�Ƿ�һ�£������·���ʱ�����ж�����ţ���Ϊ������ͷ�İ汾�Ͱ���İ汾�ǲ�һ����
				memcpy(buftemp,p+GCMD_CmdPara+2,2);			//P��ӡ001E 
				if (memcmp(gBusblkDownPro.Miss_ver,buftemp, 2) != 0)	//�����еİ汾�� 
				{
					over_turn(2,buftemp);//�����ǲ��Ǹߵ͵ߵ���()
					if(memcmp(gBusblkDownPro.Miss_ver,buftemp, 2) != 0)
					{
						MSG_LOG("����汾�Ų�һ��\r\n");
						BCD_LOG(buftemp,2,1);
						BCD_LOG(gBusblkDownPro.Miss_ver,2,1);
						delallBlacInbus();//������ں�����
						//memcpy(gBusVerInfo.busBLKVer, "\x00\x01", 2);
						//restore_flag = 3;
						
						sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
						memset(gBusVerInfo.newbusBLKVer, 0, 2);//���µ�½
						gmissflag = MISS_G_FREE;
						break;
					}
				}
			}
//********************************************************

			DealGPRSBlack(Alen/10, p+GCMD_Data); // �������������

			gBusblkDownPro.Miss_PPack = proLen;			//�������ز���,���°���

#ifdef debug_GJ_TLVDeal_
			MSG_LOG("BlackLength:%d\r\n",BlackLength);
#endif
			sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
			
			sprintf((char*)buftemp, "B:%d%% ",  ((proLen*100)/gBusblkDownPro.MISS_MAXPack));
			//display(5, 0, (char*)buftemp, DIS_CENTER);
			miniDispstr(12, 7, (char*)buftemp, DIS_CENTER);

			if(proLen == gBusblkDownPro.MISS_MAXPack)//�������
			{
#ifdef debug_GJ_TLVDeal_
				debugstring("����������:");
				debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
				if((gBusblkDownPro.Miss_style == 1)||(gBusblkDownPro.Miss_style == 2)){//��ȫ������
#ifdef debug_GJ_TLVDeal_
					debugstring("������ȫ������1");
					debugdata((unsigned char*)&BlackLength, 2, 1);
#endif
					if((gBusblkDownPro.Miss_ALL_LEn == 0)||(gBusblkDownPro.MISS_MAXPack == 0)||
						(BlackLength+20 < (gBusblkDownPro.Miss_ALL_LEn))){//�����ȫ�����أ��������������Ծ���������
#ifdef debug_GJ_TLVDeal_
						debugstring("�����������ش���2\r\n");
#endif
						memset(gBusVerInfo.busBLKVer, 0, 2);
						BlackLength = 0;
						memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
						saveBlackInfo();
// 						if(GPRSLinkProcess == GPRS_SENDING_CMD)
// 							GPRSLinkProcess = TCPSTARTSTAT;
						goto BLKSENDECHO;
					}
				}
				else{//����������,Ҫ��������������
					TempDealBlack();
				}


				addr = FLASH_TEMP_START;
				flasherase(ERASE4KBYTESECTOR,addr);
#ifdef _debug_blk
				debugstring("����:FLASH_TEMP_START---\r\n");
#endif
				memcpy(gBusVerInfo.busBLKVer, gBusblkDownPro.Miss_ver, 2);//�º������汾�������ڵġ�
				memcpy(gBusVerInfo.newbusBLKVer, gBusblkDownPro.Miss_ver, 2);//�º������汾�������ڵġ�
				memset(gBusVerInfo.newbusBLKVer, 0, 2);//���µ�½
				gmissflag = MISS_G_FREE;
				gmissOldflag = 0;
				restore_flag = 3;//ˢ�½���
				memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
				saveBlackInfo();  //ֻ���°汾��
#ifdef debug_GJ_TLVDeal_
				debugstring("�������������:");
				debugstring("\r\n");
#endif
			}
		}
		else if (p[GCMD_GnCode] == 5){
			
			memcpy((unsigned char*)&crc16, p+GCMD_Ln, 2);
			over_turn(2, (unsigned char*)&crc16);
#ifdef _debug_white
			debugstring("����������len:");//�ֽ���
			debugdata((unsigned char*)&crc16, 2, 1);
#endif
			addr = FLASH_WHITE_START;
			flasherase(ERASE4KBYTESECTOR, addr);
			memcpy(buftemp,"��Ч", 4);
			flashwrite(addr,buftemp,4);
			addr +=4;
			flashwrite(addr, p+GCMD_Ln, crc16+2);//����bai����
#ifdef _debug_white
			load_white_list();
#endif
		}




BLKSENDECHO:
#ifdef debug_GJ_TLVDeal_
// 		debugstring("������###��");
// 		debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
// 		debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
// 		debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
// 		debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
// 		debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
// 		debugdata((unsigned char*)&proLen, 2, 1);
#endif
		gBusblkDownPro.Miss_style = p[GCMD_GnCode]; //��Ӧ��Ҫ�������
		sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//�����������
		Alen = Buid_GJ_DBLK(buftemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("������ACHO:");
		debugdata(buftemp, Alen, 1);
#endif
		gprs_send_data(1, Alen, buftemp);//����Ӧ��
		break;
	case 0x4002://���س�������
		if(memcmp(gBusblkDownPro.Miss_cmdCode, "\x40\x05", 2) == 0){//���������ʷ�
			break;
		}

		if(p[GCMD_GnCode] == 0x01){//��ͷ
#ifdef debug_GJ_TLVDeal_
			debugstring("��ͷ:");
			debugdata((unsigned char*)&gBusDownPro, sizeof(stbusdownpro), 1);
#endif
			if(gBusDownPro.MISS_flag == 0xEA){//��ǰ����������Ч
				memcpy(blktemp, gBusDownPro.MISS_MAXPack, 4);
				over_turn(4, blktemp);
				memcpy(blktemp+4, gBusDownPro.Miss_ver, 2);
				over_turn(2, blktemp+4);
				memcpy(blktemp+6, gBusDownPro.Miss_ALL_LEn, 4);
				over_turn(4, blktemp+6);
				
#ifdef debug_GJ_TLVDeal_
				debugdata(p+GCMD_Data, 10, 1);
				debugstring("blktemp:");
				debugdata(blktemp, 10, 1);
#endif
//				if((memcmp(blktemp, p+GCMD_Data, 4)!=0)||(memcmp(blktemp+4, p+GCMD_Data+4, 2)!=0)||(memcmp(blktemp+6, p+GCMD_Data+6, 4)!=0))//�����š��汾�š����ֽ�����һ�£���ͷ���ط�������
				if(memcmp(blktemp, p+GCMD_Data, 10) != 0)
				{
					memcpy(gBusDownPro.MISS_MAXPack, p+GCMD_Data, 4);
					over_turn(4, gBusDownPro.MISS_MAXPack);
					memcpy(gBusDownPro.Miss_ver, p+GCMD_Data+4, 2);
					over_turn(2, gBusDownPro.Miss_ver);
					memcpy(gBusDownPro.Miss_ALL_LEn, p+GCMD_Data+6, 4);
					over_turn(4, gBusDownPro.Miss_ALL_LEn);
					memcpy(gBusDownPro.Miss_StartData, p+GCMD_Data+10, 7);
					gBusDownPro.MISS_flag = 0xEA;
					gBusDownPro.Miss_style = p[GCMD_GnCode];
					memcpy(gBusDownPro.Miss_PPack, "\x00\x00", 2);
				}
			}
			else{
				memcpy(gBusDownPro.MISS_MAXPack, p+GCMD_Data, 4);
				over_turn(4, gBusDownPro.MISS_MAXPack);
				memcpy(gBusDownPro.Miss_ver, p+GCMD_Data+4, 2);
				over_turn(2, gBusDownPro.Miss_ver);
				memcpy(gBusDownPro.Miss_ALL_LEn, p+GCMD_Data+6, 4);
				over_turn(4, gBusDownPro.Miss_ALL_LEn);
				memcpy(gBusDownPro.Miss_StartData, p+GCMD_Data+10, 7);
				gBusDownPro.MISS_flag = 0xEA;
				gBusDownPro.Miss_style = p[GCMD_GnCode];
				memcpy(gBusDownPro.Miss_PPack, "\x00\x00", 2);
			}
#ifdef debug_GJ_TLVDeal_
			memset(buftemp,0,50);
			memcpy(&itemp,gBusDownPro.MISS_MAXPack,4);
			sprintf((char *)&buftemp,"MISS_MAXPack:%d\r\n",itemp);
			debugstring((char *)buftemp);
			memset(buftemp,0,50);
			itemp = 0;
			memcpy(&itemp,gBusDownPro.Miss_ver,2);
			sprintf((char *)&buftemp,"Miss_ver:%d\r\n",itemp);
			debugstring((char *)buftemp);
			memset(buftemp,0,50);
			memcpy(&itemp,gBusDownPro.Miss_ALL_LEn,4);
			sprintf((char *)&buftemp,"Miss_ALL_LEn:%d\r\n",itemp);
			debugstring((char *)buftemp);
			memset(buftemp,0,50);
			itemp = 0;
			memcpy(&itemp,gBusDownPro.Miss_PPack,2);
			sprintf((char *)&buftemp,"Miss_PPack:%d\r\n",itemp);
			debugstring((char *)buftemp);
#endif
			memcpy((unsigned char*)&i, gBusDownPro.MISS_MAXPack, 4);
			itemp = 0;
			memcpy((unsigned char*)&itemp, gBusDownPro.Miss_PPack, 2);
			
			if((i == itemp)&&(itemp > 0)){//�Ѿ����������
				goto DoenPEND;
			}
		}
		else if(p[GCMD_GnCode] == 0x02){//����
			if(Alen < 10)//���ݳ��Ȳ���ȷ��������
				break;
			memcpy((unsigned char *)&Tack, p+GCMD_CmdPara, 2);//�����
			over_turn(2, (unsigned char *)&Tack);
			memcpy((unsigned char *)&crc16, p+GCMD_CmdPara+2, 2);//����汾��
			over_turn(2, (unsigned char *)&crc16);
			if(memcmp((unsigned char*)&crc16, gBusDownPro.Miss_ver, 2) != 0){//���صİ汾�Ų�һ��
#ifdef debug_GJ_TLVDeal_
				debugstring("���غ��������ذ汾��һ��\r\n");
#endif
				memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));//���´�
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				goto PROSENDECHO;
			}
			if (Tack == 1)//��һ����
			{
				addr = FLASH_PRO_START;//д��ĵ�ַ ������ʼ��ַ
				for(i=0; i<4; i++){
#ifdef debug_GJ_TLVDeal_
					debugstring("��մ�����:");
					debugdata((unsigned char*)&addr, 4, 1);
#endif
					flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 256K
					addr += 0x10000;//��һ��64K�Ŀ�
				}
			}
			memcpy((unsigned char*)&Alen, gBusDownPro.Miss_PPack, 2);//��һ�θ��µİ����
			if(Tack > (Alen+1)){//������
				memcpy(gBusDownPro.Miss_PPack, "\x00\x00", 2);//���´�
#ifdef debug_GJ_TLVDeal_
				debugstring("���´�:");
				debugdata((unsigned char*)&Tack, 2, 1);
				debugdata((unsigned char*)&Alen, 2, 1);
#endif
#ifdef GPRS_MG323_
				initTCPIPnode();
#endif
//				memset((unsigned char*)&gBusDownPro.MISS_flag, 0, sizeof(stbusdownpro));
				goto PROSENDECHO;
			}
			addr = (FLASH_PRO_START + ((Tack-1) * CMD_G_SIZE));//д��ĵ�ַ������ų���һ�������صĴ�С
			//��ҪУ��CRC16
			memcpy((unsigned char*)&itemp, gBusDownPro.MISS_MAXPack, 4);
// 			debugstring("Tack:");
// 			debugdata((unsigned char*)&Tack, 2, 1);
// 			debugstring("itemp:");
// 			debugdata((unsigned char*)&itemp, 4, 1);
			sprintf((char*)buftemp, "P:%d%% ",  ((Tack*100)/itemp));
			//display(5, 0, (char*)buftemp, DIS_CENTER);
			miniDispstr(12, 7, (char*)buftemp, DIS_CENTER);
			
			if(addr > FLASH_PRO_END){//��ַ�����˰���д�룬��������˰�����
				restore_flag = 1;
				goto PROSENDECHO;
			}
			else{
				memcpy((unsigned char *)&Alen, p+GCMD_Ln, 2);
				over_turn(2, (unsigned char*)&Alen);

				
#ifdef debug_GJ_TLVDeal_
			debugstring("400202::����:::\r\n");
			memset(buftemp,0,50);
			memcpy(&itemp,gBusDownPro.MISS_MAXPack,4);
			sprintf((char *)&buftemp,"MISS_MAXPack:%d\r\n",itemp);
			debugstring((char *)buftemp);
			memset(buftemp,0,50);
			itemp = 0;
			memcpy(&itemp,gBusDownPro.Miss_ver,2);
			sprintf((char *)&buftemp,"Miss_ver:%d\r\n",itemp);
			debugstring((char *)buftemp);
			memset(buftemp,0,50);
			memcpy(&itemp,gBusDownPro.Miss_ALL_LEn,4);
			sprintf((char *)&buftemp,"Miss_ALL_LEn:%d\r\n",itemp);
			debugstring((char *)buftemp);
			memset(buftemp,0,50);
			itemp = 0;
			memcpy(&itemp,gBusDownPro.Miss_PPack,2);
			sprintf((char *)&buftemp,"Miss_PPack:%d\r\n",itemp);
			debugstring((char *)buftemp);
#endif

#ifdef debug_GJ_TLVDeal_
				debugstring("addr:");
				debugdata((unsigned char*)&addr, 4, 1);
				memset(buftemp,0,50);
				sprintf((char *)&buftemp,"Alen:%d\r\n",Alen);
				debugstring((char *)buftemp);

#endif
				for(bbb =0;bbb<3;bbb++){
					flashwrite(addr, p+GCMD_Data, CMD_G_SIZE);//д��
					flashread(addr, buftemp, CMD_G_SIZE);
					if(memcmp(p+GCMD_Data, buftemp, CMD_G_SIZE) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("����д�����\r\n");
						debugdata(p+GCMD_Data, proLen, 1);
						debugdata(buftemp, proLen, 1);
#endif
						continue;
					}
				}

				if(bbb >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("��ͷ��ʼ\r\n");
#endif
					addr = FLASH_PRO_START;//д��ĵ�ַ ������ʼ��ַ
					
					memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));
#ifdef GPRS_MG323_
					initTCPIPnode();
#endif
					goto PROSENDECHO;//д�����ȫ����������
				}
		
				memcpy((unsigned char*)&Alen, gBusDownPro.MISS_MAXPack, 4);//��һ�θ��µİ����
				if(Tack == Alen)//�������
				{
DoenPEND:
#ifdef debug_GJ_TLVDeal_
					debugstring("��������:");
					debugdata((unsigned char*)&gBusDownPro.Miss_style, 1, 1);
#endif
					memset(buftemp, 0, 64);
					memcpy(buftemp,"������Ҫ����", 12);
					memcpy(buftemp+12,gBusDownPro.Miss_ver, 2);
					memcpy(buftemp+14, gBusDownPro.Miss_StartData, 7);
#ifdef debug_GJ_TLVDeal_
					debugstring("�����������:");
					debugdata(buftemp, 21, 1);
#endif
#ifdef _AREA_ENG_
					display(3,0,"Restart.....",DIS_CENTER);
#else
					display(3,0,"���,������...",DIS_CENTER);
#endif
					addr = FLASH_PRO_FLAG;
					
#ifdef debug_GJ_TLVDeal_
					debugstring("�����������2:");
					debugdata(buftemp, 34, 1);
#endif
					flashwrite(addr, buftemp, 64);
					
					memcpy(gBusDownPro.Miss_PPack, p+GCMD_CmdPara, 2);//��¼��ǰ���ڸ��µİ����к�
					over_turn(2, gBusDownPro.Miss_PPack);
					
					gBusDownPro.Miss_style = p[GCMD_GnCode];//�����Ӧ��Ҫ�ã�����������������µġ�
					Alen = Build_GJ_DPro(buftemp);
					gprs_send_data(1, Alen, buftemp);//����Ӧ��
#ifdef GPRS_MG323_
					while(1){
						clr_wdt();
						if(gGPRS_data_style.ucSendstyle == GPRS_SISW){
#ifdef _debug_gprs
							debugstring("����:");
							debugdata((unsigned char*)&gGPRS_data_style.uiNum, 4, 1);
#endif
							//		if(memcmp(gMC55iSendbuff, "������Ч", 8)==0){//�����Ƿ���Ч����Ӧ�÷�
							//			memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
							gprs_DATA_snd(gGPRS_data_style.uiNum, gMC55iSendbuff+12);//�����ж��в��ܷ�������
							memset(gMC55iSendbuff, 0, 50);
							//		}
							
							gGPRS_data_style.ucSendstyle = 0;
							gGPRS_data_style.uiNum = 0;
							break;
						}
					}
#endif
					
					memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));
					sysfewrite(BIT_BUSDownPRO_INFO, 27, (unsigned char*)&gBusDownPro);//�����������

					beep(1,10,10);
					delayxms(400);
					ReStartCPU();//������ɣ������豸�Ա���³���
					while(1);
				}
			}
			memcpy(gBusDownPro.Miss_PPack, p+GCMD_CmdPara, 2);//��¼��ǰ���ڸ��µİ����к�
			over_turn(2, gBusDownPro.Miss_PPack);
		}
PROSENDECHO:
		gBusDownPro.Miss_Time = GPRS_Login_dly;//�������40�룬û�����յ�����������Ϣ�������µ�½
		gBusDownPro.Miss_style = p[GCMD_GnCode];//�����Ӧ��Ҫ�ã�����������������µġ�
		sysfewrite(BIT_BUSDownPRO_INFO, 27, (unsigned char*)&gBusDownPro);//

		Alen = Build_GJ_DPro(buftemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("��Ӧ:::::\r\n");
		debugdata(buftemp,Alen,1);
#endif
		gprs_send_data(1, Alen, buftemp);//����Ӧ��
		break;
	case 0x4005://�ʷ���Ϣ����
#ifdef _debug_price
		debugstring("case 0x4005:!!!\r\n");
#endif
		if(p[GCMD_GnCode] != 3){//��ȫ����һ����
			break;	
		}
		pprice = (stLoadPtable*)(p+GCMD_Data);
		
#ifdef BUS_SERRIER_No
		memset(blktemp, 0, 4);
		getposSN((unsigned char*)buftemp, 0);//ȡPOS���ն˱��
		Ascii2BCD(buftemp+10, blktemp+1, 6);//Address:�ն˻����
#else 
		//		Get_SerialNumF4(blktemp);
		Ascii2BCD(pFistVary.DeviceNo, blktemp, 8);
#endif
		
		addr = FLASH_TEMP_PRICE;
		flasherase(ERASE4KBYTESECTOR, addr);	//�ȱ���������������·��Ϣ  checkNewPrice�����д�����Ϣ
		for(i=0; i<3; i++){
			flashwrite(addr, p+GCMD_Data, Alen);
			flashread(addr, buftemp, Alen);
			if(memcmp(buftemp, p+GCMD_Data, Alen) == 0)
				break;
		}
		if(i >= 3){
			debugstring(" ������·��Ϣд�����!\r\n");
		}

		checkNewPrice(); //����Ƿ�Ҫ������Ҫ����  
		
		Alen = Build_GJ_LINE_ACHO(buftemp, p[GCMD_GnCode], pprice);
		gprs_send_data(1, Alen, buftemp);//����Ӧ��
#ifdef _debug_price
		debugstring("ACK:::::");
		debugdata(buftemp,Alen,1);
		
#endif
		
		memset(gBusVerInfo.newbusBLKVer, 0, 2);//���µ�½
		gmissflag = MISS_G_FREE;
		gmissOldflag = 0;
		restore_flag = 3;
		break;
		
	case 0x4004://��վ������Ϣ����
#ifdef debug_GJ_TLVDeal_
		debugstring("case 0x4004:=====\r\n");
#endif
		
		if(p[GCMD_GnCode] == 0x01){//��ͷ
#ifdef debug_GJ_TLVDeal_
			debugstring("��վ���ٰ�ͷ:");
			debugdata((unsigned char*)&gGPSDownLine, sizeof(stGPSdownLine), 1);
#endif
			if(gGPSDownLine.MISS_flag == 0xEA){//��ǰ����������Ч
				memcpy(blktemp, gGPSDownLine.MISS_MAXPack, 4);
				over_turn(4, blktemp);
				memcpy(blktemp+4, gGPSDownLine.Miss_ver, 2);
				over_turn(2, blktemp+4);
				memcpy(blktemp+6, gGPSDownLine.Miss_Line, 2);
				over_turn(2, blktemp+6);
				
#ifdef debug_GJ_TLVDeal_
				debugdata(p+GCMD_Data, 10, 1);
				debugstring("blktemp:");
				debugdata(blktemp, 10, 1);
#endif
				if((memcmp(blktemp, p+GCMD_Data, 4)!=0)||(memcmp(blktemp+4, p+GCMD_Data+4, 2)!=0))//�����š��汾��
				{
#ifdef debug_GJ_TLVDeal_
					debugstring("Restart!!\r\n");
#endif
					memcpy(gGPSDownLine.MISS_MAXPack, p+GCMD_Data, 4);
					over_turn(4, gGPSDownLine.MISS_MAXPack);
					memcpy(gGPSDownLine.Miss_ver, p+GCMD_Data+4, 2);
					over_turn(2, gGPSDownLine.Miss_ver);
					memcpy(gGPSDownLine.Miss_Line, p+GCMD_Data+8, 2);//��·���ȡ��2�ֽ�
					gGPSDownLine.MISS_flag = 0xEA;
					gGPSDownLine.Miss_style = p[GCMD_GnCode];
					memcpy(gGPSDownLine.Miss_PPack, "\x00\x00", 2);
				}
			}
			else{
#ifdef debug_GJ_TLVDeal_
				debugstring("Continue!!\r\n");
#endif
				memcpy(gGPSDownLine.MISS_MAXPack, p+GCMD_Data, 4);
				over_turn(4, gGPSDownLine.MISS_MAXPack);
				memcpy(gGPSDownLine.Miss_ver, p+GCMD_Data+4, 2);
				over_turn(2, gGPSDownLine.Miss_ver);
				memcpy(gGPSDownLine.Miss_Line, p+GCMD_Data+8, 2);//��·���ȡ��2�ֽ�
				gGPSDownLine.MISS_flag = 0xEA;
				gGPSDownLine.Miss_style = p[GCMD_GnCode];
				memcpy(gGPSDownLine.Miss_PPack, "\x00\x00", 2);
			}
#ifdef debug_GJ_TLVDeal_
			debugstring("MISS_MAXPack:");
			debugdata(gGPSDownLine.MISS_MAXPack, 4, 1);
			debugstring("Miss_ver:");
			debugdata(gGPSDownLine.Miss_ver, 2, 1);
			debugstring("Miss_Line:");
			debugdata(gGPSDownLine.Miss_Line, 2, 1);
			debugstring("Miss_PPack:");
			debugdata(gGPSDownLine.Miss_PPack, 2, 1);
#endif
		}
		else if(p[GCMD_GnCode] == 0x02){//����
			if(Alen < 10)//���ݳ��Ȳ���ȷ��������
				break;
			memcpy((unsigned char *)&Tack, p+GCMD_CmdPara, 2);//�����
			over_turn(2, (unsigned char *)&Tack);
			memcpy((unsigned char *)&crc16, p+GCMD_CmdPara+2, 2);//�汾��
			over_turn(2, (unsigned char *)&crc16);
			if(memcmp((unsigned char*)&crc16, gGPSDownLine.Miss_ver, 2) != 0){//���صİ汾�Ų�һ��
#ifdef debug_GJ_TLVDeal_
				debugstring("��վ���ٰ汾��һ��\r\n");
#endif
				memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));//���´�
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				goto PROSENDECHO2;
			}
			if (Tack == 1)//��һ����
			{
				addr = FLASH_GPS_LINE_START;//д��ĵ�ַ ������ʼ��ַ
#ifdef debug_GJ_TLVDeal_
				debugstring("��մ�����:");
				debugdata((unsigned char*)&addr, 4, 1);
#endif
				flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 64K
				addr += 0x10000;//��һ��64K�Ŀ�
				Delay(2);
				flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 64K
			}
			memcpy((unsigned char*)&Alen, gGPSDownLine.Miss_PPack, 2);//��һ�θ��µİ����
			if(Tack < Alen){//���ڸ��µ���ű��ϴθ��µ����С����Ϊ����
				memcpy(gGPSDownLine.Miss_PPack, "\x00\x00", 2);//���´�
//				memset((unsigned char*)&gGPSDownLine.MISS_flag, 0, sizeof(stbusdownpro));
				goto PROSENDECHO2;
			}
			addr = (FLASH_GPS_LINE_START + ((Tack-1) * CMD_G_SIZE));//д��ĵ�ַ������ų���һ�������صĴ�С
			//��ҪУ��CRC16
			memcpy((unsigned char*)&itemp, gGPSDownLine.MISS_MAXPack, 4);
// 			debugstring("Tack:");
// 			debugdata((unsigned char*)&Tack, 2, 1);
// 			debugstring("itemp:");
// 			debugdata((unsigned char*)&itemp, 4, 1);
			sprintf((char*)buftemp, "L:%d%% ",  ((Tack*100)/itemp));
			//display(5, 0, (char*)buftemp, DIS_CENTER);
			miniDispstr(12, 7, (char*)buftemp, DIS_CENTER);
			
			if(addr > FLASH_GPS_LENE_END){//��ַ�����˰���д�룬��������˰�����
				restore_flag = 1;
				goto PROSENDECHO2;
			}
			else{
				memcpy((unsigned char *)&Alen, p+GCMD_Ln, 2);
				over_turn(2, (unsigned char*)&Alen);

				
#ifdef debug_GJ_TLVDeal_
				debugstring("MISS_MAXPack:");
				debugdata(gGPSDownLine.MISS_MAXPack, 4, 1);
				debugstring("MISS_MAXPack:");
				debugdata(gGPSDownLine.Miss_ver, 2, 1);
				debugstring("Miss_Line:");
				debugdata(gGPSDownLine.Miss_Line, 2, 1);
				debugstring("Miss_PPack:");
				debugdata(gGPSDownLine.Miss_PPack, 2, 1);
#endif

#ifdef debug_GJ_TLVDeal_
				debugstring("addr:");
				debugdata((unsigned char*)&addr, 4, 1);
				debugstring("Alen:");
				debugdata((unsigned char*)&Alen, 2, 1);
#endif
				for(bbb =0;bbb<3;bbb++){
					flashwrite(addr, p+GCMD_Data, CMD_G_SIZE);//д��
					flashread(addr, buftemp, CMD_G_SIZE);
					if(memcmp(p+GCMD_Data, buftemp, CMD_G_SIZE) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("����д�����\r\n");
						debugdata(p+GCMD_Data, proLen, 1);
						debugdata(buftemp, proLen, 1);
#endif
						continue;
					}
				}

				if(bbb >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("��ͷ��ʼ\r\n");
#endif
					addr = FLASH_GPS_LINE_START;//д��ĵ�ַ ������ʼ��ַ
					
					memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));
					
					goto PROSENDECHO2;//д�����ȫ����������
				}
		
				memcpy((unsigned char*)&Alen, gGPSDownLine.MISS_MAXPack, 4);//��һ�θ��µİ����
				if(Tack == Alen)//�������
				{
#ifdef debug_GJ_TLVDeal_
					debugstring("��������:");
					debugdata((unsigned char*)&gGPSDownLine.Miss_style, 1, 1);
#endif
#ifdef debug_GJ_TLVDeal_
					debugstring("��·��Ϣ�������:");
					debugdata((unsigned char*)&gGPSDownLine, 27, 1);
#endif
					
					gGPSDownLine.Miss_style = p[GCMD_GnCode];//�����Ӧ��Ҫ�ã�����������������µġ�
					Alen = Build_GJ_DLine(buftemp);
					gprs_send_data(1, Alen, buftemp);//����Ӧ��
					
					flashwrite(FLASH_GPS_LENE_VER, gGPSDownLine.Miss_ver, 2);
					//gBusVerInfo.busLineVer
					memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));
					sysfewrite(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//�����������

					beep(1,10,10);
					delayxms(400);
					ReStartCPU();//������ɣ������豸�Ա���³���
					while(1);
				}
			}
			memcpy(gGPSDownLine.Miss_PPack, p+GCMD_CmdPara, 2);//��¼��ǰ���ڸ��µİ����к�
			over_turn(2, gGPSDownLine.Miss_PPack);
		}
PROSENDECHO2:
		gGPSDownLine.Miss_Time = GPRS_Login_dly;//�������40�룬û�����յ�����������Ϣ�������µ�½
		gGPSDownLine.Miss_style = p[GCMD_GnCode];//�����Ӧ��Ҫ�ã�����������������µġ�
		sysfewrite(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//

		Alen = Build_GJ_DLine(buftemp);
		gprs_send_data(1, Alen, buftemp);//����Ӧ��
		break;
	case 0x4003://GPS�ն˳�������
	case 0x4006://���ɼ���Ϣ����
		break;
	case 0x5002://������Ϣ����
		memcpy((unsigned char*)&Tack, p+GCMD_Data, 2);
		if(Tack != 0x0090){
#ifdef debug_GJ_TLVDeal_
			debugstring("����Ϣ��Ӧʧ��:");
			sprintf((char*)buftemp, "ACK:%02X%02X\r\n", (Tack>>8), (Tack&0xFF));
			debugstring((char*)buftemp);
#endif
			gmissflag = MISS_G_ALAM;
			goto GJdataDealEnd;
		}
		else{
#ifdef _GPS_DIAODU_
//			galamessage.alamFlag = 0;  ����������ڸ�ƽ̨�ϴα�����Ϣ
			if(galamessage.style == 0x5A)//���ڷ��� 
				galamessage.style = 0xE0;//��ɷ��ͱ�־
			else
				galamessage.style = 0;
#endif
			gmissflag = MISS_G_FREE;//�ÿ��У�
#ifdef KEY_PAD
			gKeyRestorDis = 3;		
#endif
		}
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x2002://�ն˻���֪
	case 0x5001://GPS��λ���ݴ���
	case 0x5003://���ػ��������ݴ���
	case 0x5004://5004û����,��ǰ�����д����ݣ������ϲ���������¼����
	case 0x5005://������ʯ��¼�ش������ֻ��ż���������
#ifdef debug_GJ_TLVDeal_
		debugstring("Alen 5003|5005:");
		debugdata((unsigned char*)&Alen, 2,1);
		debugdata(p+GCMD_Data+Alen-2, 2,1);
#endif
//		����1000 ΪCRC16 ����ȷ
//		����7007 ϵͳ��æ����������
//		����8008 ϵͳ�����쳣���󣬲��ܴ洢����
//		����4000 ���ܵ���ͨѶͷ����ȷ

		memcpy((unsigned char*)&Tack, p+GCMD_Data+Alen-2, 2);
		if((Tack == 0x0010)||(Tack == 0x0040)){
// 			memset((unsigned char*)&tcpipPacket, 0, 5);
// 			sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
			miniDispstr(12,0,"5003ERROR",0);
			beep(3,100,100);
			delayxms(500);
		}
		else if(Tack != 0x0090){
#ifdef debug_GJ_TLVDeal_
			debugstring("�ϴ���¼��Ӧʧ��:");
			sprintf((char*)buftemp, "ACK:%02X%02X\r\n", (Tack>>8), (Tack&0xFF));
			debugstring((char*)buftemp);
#endif
			if(CmdCode == 0x5001)
				gmissflag = MISS_G_GPS;
			else if((CmdCode == 0x5003)||(CmdCode == 0x5005))
				gmissflag = MISS_G_UREC;
			else
				gmissflag = MISS_G_FREE;//�ÿ��У�
//			stopCCgprs(ERROR_BUS_CONNECT);
			goto GJdataDealEnd;
		}
		else{
			if(CmdCode == 0x5003){
				memcpy(&ustemp,p+GCMD_CmdPara,2);
				// 				debugstring("P1P2:");
				// 				debugdata((unsigned char *)&ustemp,2,1);
				// 				debugstring("P1P2:");
				// 				debugdata((unsigned char *)&(tcpipPacket.Serial),2,1);
				
				if (tcpipPacket.Serial == ustemp)	//���ź͸մ����һ��
				{
#ifdef debug_GJ_TLVDeal_
					debugstring("DEL record======\r\n");
#endif
					
					memset((unsigned char*)&tcpipPacket, 0, 5);
					sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
					ustemp++;
					sysfewrite(FE_PACKSERIAL, 2, (unsigned char*)&ustemp);
				}

			}
			gmissflag = MISS_G_FREE;//�ÿ��У�
		}
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x7001://�����������
	case 0x7002://��·�������
		break;
	case 0x6001://����ָ�����
#ifdef _GPS_DIAODU_
		memcpy((unsigned char*)&gDiaoDuCMD, p+GCMD_Data, 3);//��ȡǰ��λ����Ȼ��������������Ϣ������������ı��Ͳ�������������ֽ�
		memcpy(gDiaoDuCMD.Date, (unsigned char*)&SysTime.year_h, 4);
		if((gDiaoDuCMD.ucCMD == 0x10)||(gDiaoDuCMD.ucCMD == 0x11)){//������Ϣ
			sysferead(BIT_GPSChildLine, 2, hadware);
			memcpy((unsigned char*)&gDiaoDuCMD, p+GCMD_Data, 15);
			if(memcmp(hadware, gDiaoDuCMD.ucZiLine+2, 2)!=0){//�������·��һ��,�������÷���·�����¶�ȡ������Ϣ
#ifdef _debug_gprs
				debugstring("����·��һ��:");
				debugdata((unsigned char*)&gDiaoDuCMD.ucZiLine, 4, 1);
				debugdata(hadware, 2, 1);
#endif
				sysfewrite(BIT_GPSChildLine, 2, gDiaoDuCMD.ucZiLine+2);//��������·���
				savesysbupara();
				Load_Line_info();//���¶�ȡ������Ϣ
			}
			gDiaoDuCMD.ucDDstyle = 0x10;
		}
		else if((gDiaoDuCMD.ucCMD == 0x13)||(gDiaoDuCMD.ucCMD == 0x14)){//������������Ϣ,��Ҫ���Ᵽ��
			memcpy((unsigned char*)&Alen, p+GCMD_Ln, 2);//�����򳤶�
			over_turn(2, (unsigned char*)&Alen);//��ȡǰ��λ����Ȼ��������������Ϣ������������ı��Ͳ�������������ֽ�,����������������
			if(Alen > 64)
				Alen = 64;
			p[GCMD_Data+Alen] = 0;//���������
			Alen++;
			sysfewrite(BIT_GPS_MESSAGE, (Alen-3), p+GCMD_Data+3);
			sysfewrite(BIT_GPS_MESSAGE+65, 7, (unsigned char*)&SysTime);//д������ʱ��
		}
#ifdef _debug_gprs
		debugstring("����ָ��:");
		debugdata((unsigned char*)&gDiaoDuCMD, sizeof(stDiaoDuCMD), 1);
#endif 
		gDiaoDuCMD.ucStartYorN = 0;
		Save_DIAODUCMD_info();
#ifdef KEY_PAD
		gKeyRestorDis = 3;//ˢ�°�������ʾ
#endif
		Alen = Build_DIAODU_Echo(buftemp, 0x0160, 0x02, 0, p+GCMD_CmdPara);
		gprs_send_data(1, Alen, buftemp);//����Ӧ��
#endif
		break;
	case 0x7003://ʱ������
		if(time_invalid(p+GCMD_Data) == 1){//ʱ����Ч��Уʱ
			timewrite(p+GCMD_Data);
		}
		Alen = Build_Echo(buftemp, 0x0370, p[GCMD_GnCode], 0);
		gprs_send_data(1, Alen, buftemp);//����Ӧ��
		break;
	case 0x7004://����IP�ͽڵ�����
		break;
	case 0x7005://���ػ���������ʱ������
		break;
	case 0x7006://GPS����������ʱ������
		break;
	}
GJdataDealEnd:
	if((nestpP != 0) && (nestpLen > 0))//���ذ�����Ҫ�Ƶ�ǰ��
	{
		pakege[0] = 0x01;//��������
		memcpy(pakege+1, (unsigned char*)&nestpLen, 2);//ʣ��ĳ���
		memmove(pakege+3, nestpP, nestpLen);
		return 1;
	}
	else
		return 0;
#else
	return 0;
#endif
}

