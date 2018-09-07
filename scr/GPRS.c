
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"

#include "szct.h"

//#define _debug_gprs
//#define _debug_time	//


//V3 : ����4Gģ��
#define SOFT_VAL 4	//��汾



extern unsigned int str2int(unsigned char *pstr, unsigned char len);
extern unsigned char checkIP(char *IPb);

extern void savesysbupara(void);
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern unsigned char SPI_Snd_dat(unsigned short len, unsigned char *dat, unsigned char cmd);
extern unsigned char GetDateTime(void);
//extern unsigned char GprsSend(unsigned short serialno,unsigned char cmd,unsigned short len,unsigned char *buff);
extern unsigned char DealStationBTM(unsigned int len, unsigned char *info);
extern void UPData_Line_Info_gprs(unsigned char *info);
extern void gprs_snd(unsigned int len, char *dat);
extern void uploadMobilePara(void);
extern void first_clr_mobile_para(void);
extern unsigned char GJDataDeal(unsigned char *pakege);
extern void getMobilePara(void);
extern void stopCCgprs(unsigned short mode);
extern void find_G_new_mission(void);
extern void KeyPad_restordis(void);
extern void in_deal_ring(void);
extern int in_dealSoundIn(void);
extern int in_checkRingstyle(void);
extern int in_sendGPs(void);
extern void GPRS_MC55i_open(void);
extern void GPRS_open(void);
extern unsigned char in_getGprsModeStyle(void);
extern void in_dispGprsProcess(void);

extern void sendmsgHead(void);
extern void sendmsgBody(void);
extern void SendmsgEnd(void);

extern void in_gErrortimes(unsigned char val, unsigned char mode);
extern void in_decgM_Senddll(void);
extern void in_IF2TLVDeal(unsigned char sindex);
extern void in_IF18583Deal(unsigned char sindex);
extern void set_KEK_SN(unsigned char val);
extern int checkKEK(void);

extern void powerOff4V0(void);
extern void powerON4V0(unsigned int baund);

extern unsigned char gmissOldflag;
extern volatile unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.

extern stbusdownPara gBusblkDownPro;//����������
extern stbusdownpro gBusDownPro;
extern stGPSdownLine gGPSDownLine;
extern pFistVary_1 pFistVary;
extern stcomtime SysTime;
extern unsigned int a_sum1,s_sum1;
extern stBusVerIfo gBusVerInfo;
extern unsigned short gErrorFlag;//�����ʶ,���ֽڱ�ʾ�����ţ����ֽڱ�ʾ������� ���"*��������ʶ����"
extern unsigned int gErrortimes[2];//���������,��Ҫ������GPRS����

extern unsigned char gchn_eng;//��Ӣ�ı�־
extern volatile unsigned char gpacSNo;//�����
//GPRS����״̬
volatile unsigned char gAPN_Style;//APN״̬����ר�������ǹ���
unsigned char gGPSdly;//GPS��ʱ
unsigned short gSendOverTime;//���ͺ���Ӧ��ʱ������
unsigned char omissDly;
extern unsigned int gSendGLogin;//��½���� �������ѯ�ź�

TCP_IP_PACKET1 tcpipPacket;//500
volatile unsigned char GPRSLinkProcess;
										//gprs���ж���Ϣ ÿ�β���ǰ��λ	
PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//����֡����	//���Ͷ��л���

volatile unsigned int CDMA_REV_LEN;//��Ҫ�������ݵĳ���,�����0xffffffffΪû��������Ҫ���ա�

unsigned char gTcpIpbusy;//ģ���������ڷ����ݣ�ģ�鹤���Ƿ����Ȼ���Ӧ���ٷ����ݡ��ڵȻ�Ӧʱ���ܷ��������ݡ�
volatile unsigned int guiMC323_REV_LEN;//��Ҫ�������ݵĳ���,�����0xffffffffΪû��������Ҫ���ա�
unsigned char gTcpIpbusydly;//�ղ���������ʱ����
unsigned char gMC55iSendbuff[256];//MC55i�������ݻ��壬ǰ8�ֽ�Ϊ����������Ч��
unsigned char tcpipsdnode;
stTcpIpSp gTcpIPsd[4];//�����ݴ棬���ܳ���256�ֽ�һ����
volatile stGPrsDataStyle gGPRS_data_style;

#define GPRSCOMMAX 1024+64
unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS���ջ���
volatile unsigned short gprsIndex;

#define GPRS_CLEAR_INDEX	0x55	//����������������
#define GPRS_DATA_OK		0xaa	//����׼����
#define GPRS_GOON			0		//����������
volatile unsigned char gprsReady=GPRS_GOON;//�յ�GPRS�ش�������,��Ҫ����.

unsigned int ClientIP;
unsigned short ClientPort;

unsigned char gprsSingle;//GPRS�ź�ǿ��
unsigned char gprsAnswer;//�ǣУң�ģ���Ӧ�ϣ˱�־
unsigned char gprsSendDlyflag;//ģ�鷢���Ƿ���ʱ��־
unsigned char isNetOK[2];//��·�����Ƿ�ɹ�

unsigned char gprsErrorTimes;//���֣ţңңϣҷ��ش������
unsigned char gprsSNDerro;//�ͺ�̨ͨѶ���ֵĴ���
unsigned char gTTime[14];//�������ķ�����������ʱ��
unsigned char gGPRS_Cmd_outtime;//GPRS����ʱ����
unsigned char GprsCardList[16];


#define  ChinaMobile   1
#define  ChinaUnicom   2
#define  ChinaTelecom  3 
unsigned char ugISMI_type;	///ȫ��ͨʱָʾ��ǰ������Ӫ��

extern unsigned char restore_flag;

extern unsigned short getCpuPort(void);
extern unsigned int getCpuIPaddr(void);
extern void GPRSSASK(unsigned char *dat, unsigned char cmd, unsigned int len);



unsigned short BCC( unsigned char *Bcc_pointer,unsigned short x)					//����BCCУ����
{
	unsigned char data;
	unsigned char k,h;
	unsigned char BCC_L=0;
	unsigned char BCC_H=0;
	unsigned short  BCC=0;
	unsigned short i,j;

	for(i=0;i<x;i++)
	{
		data= *(Bcc_pointer+i);
		for(j=0;j<8;j++)
		{
			if(BCC_H&0x01) k=1;
			else k=0;
			BCC_H=BCC_H>>1;                 //������ֽڡ�
			if(BCC_L&0x01) h=1;
			else h=0;
			BCC_L=BCC_L>>1;
			if(k) BCC_L=BCC_L|0x80;
			else BCC_L=BCC_L&0X7f;
			if(h) BCC_H=BCC_H|0x80;
			else BCC_H=BCC_H&0x7f;
			if(data&0x01) h=1;
			else h=0;
			data=data>>1;
			if(h)
			{
				BCC_H=BCC_H^0x80;
				goto bcc_1;
			}
			else
			{
bcc_1:
			if(BCC_H&0x80)
			{
				BCC_H=BCC_H^0x04;
				BCC_L=BCC_L^0x08;
			}
			}   
		}
	}
	BCC=0;
	BCC=BCC|BCC_H;
	BCC=BCC<<8;
	BCC=BCC|BCC_L;
	return(BCC);
}

//ֻ���ڷ��͸�MODEMģ�飬ST�ĳ��ػ�Ӧ���ò��ϡ�
void gprs_ModSend(unsigned char Scom, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos=0;
	unsigned int CRC;
	unsigned short i;
	
	memset(buff, '$', 4);//��ͷ
	pos = 4;
	
	i = len+10;
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//���ݳ��ȣ���������+Ӧ�ò����� �ĳ���,���ֽ���ǰ
	pos += 2;
	
	buff[pos++] = 0x01;//�������ݣ�ֱ�ӷ��͵���̨�����ֽڲ���Ҫ����
	buff[pos++] = gpacSNo;//�����
	memcpy(buff+pos, dat, len);
	pos += len;
	
	CRC = 0xFEFE;//crc16_isr((unsigned char*)buff, pos);
	
	memcpy(buff+pos, (unsigned char*)&CRC, 2);
	//over_turn(2, buff+pos);
	pos += 2;
	com_snd(Scom, pos, (unsigned char*)buff);
}

extern unsigned char getGprsCom(void);
void gprs_snd(unsigned int len, char *dat)
{
	unsigned char sndcom;
	unsigned char sndGMode;

	sndcom = getGprsCom();
	
	sndGMode = in_getGprsModeStyle();
	sndGMode &= 0xF0;
	if(sndGMode == 0x80){//���͸�ģ�飬��Ҫ��װ
		gprs_ModSend(sndcom, len, (unsigned char*)dat);
	}
	else{
		clr_wdt();
#ifdef _debug_gprs
// 		debugstring("Send:");
//  		debugstring(dat);
// 		debugstring("\r\n");
#endif

		com_snd(sndcom, len, (unsigned char*)dat);
	}
}

void initTCPIPnode(void)
{
	memset((unsigned char*)&gTcpIPsd[0], 0, 10);
	memset((unsigned char*)&gTcpIPsd[1], 0, 10);
	memset((unsigned char*)&gTcpIPsd[2], 0, 10);
	memset((unsigned char*)&gTcpIPsd[3], 0, 10);
	
	tcpipsdnode = 0;
	
}
void TcpIPSd_SAVE(unsigned char *dat, unsigned int len)
{
	unsigned char i;

#ifdef _debug_gprs
	debugstring("TcpIpsd save --\r\n");
#endif

	if((len > 1024)||(len == 0)){
#ifdef _debug_gprs
		debugstring("TcpIpsd Save Data too long!!\r\n");
#endif
		return;
	}//BDBDBD226868752001020000000000072012070508265618E6
	for(i=0; i<4; i++){//����ͬ�����ݰ�
		if(gTcpIPsd[i].T == 0xE501){//
			if(memcmp(gTcpIPsd[i].V+GCMD_CmdCode, dat+GCMD_CmdCode, 2) == 0){
// 				debugstring("����ͬ��������ǣ�\r\n");
// 				debugdata((unsigned char *)gTcpIPsd[i].V, 10, 1);
// 				debugdata((unsigned char *)dat, 10, 1);
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if(i >= 4){//û���ҵ���ͬ��
		for(i=0;i<4;i++){//�ҿ���ط�
			if(gTcpIPsd[i].T != 0xE501){//�ձ�
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if(i>=4){//û���ҵ����еģ������
		if(tcpipsdnode > 0)//
			i = tcpipsdnode-1;
		else
			i = 3;

		gTcpIPsd[i].T = 0xE501;
		gTcpIPsd[i].L = len;
		memcpy(gTcpIPsd[i].V, dat, len);
	}
#ifdef _debug_gprs
// 	for(i=0;i<4;i++){
		debugstring("TcpIPSd_SAVE:");
		debugdata((unsigned char *)&gTcpIPsd[i], gTcpIPsd[i].L +4, 1);
// 	}
#endif
}

//ȡ��������Ҫ��������
unsigned short TcpIPSd_Pick(unsigned char *dat)
{
	unsigned char i;
	unsigned short rLen;
	
	rLen = 0xFFFF;
	if(tcpipsdnode > 3)
		tcpipsdnode = 0;
	for(i=0; i<4; i++){
		if(gTcpIPsd[tcpipsdnode].T == 0xE501){
			rLen = gTcpIPsd[tcpipsdnode].L;
			memcpy(dat, gTcpIPsd[tcpipsdnode].V, gTcpIPsd[tcpipsdnode].L);
			memset((unsigned char*)&gTcpIPsd[tcpipsdnode], 0, 10);//���
			tcpipsdnode++;
#ifdef _debug_gprs
			debugstring("TCPip_pick OK:");
			debugdata((unsigned char*)&rLen, 2, 1);
#endif
			break;
		}
		else{
			tcpipsdnode++;
			if(tcpipsdnode>3)
				tcpipsdnode = 0;
		}
	}
#ifdef _debug_gprs
	if(rLen != 0xFFFF)
	{
		debugstring("TcpIPSd_Pick:");
		debugdata(dat, rLen, 1);
	}
#endif
	return rLen;
}

//�������УǣУңӰ���linkNum = ���͵���·ѡ��
void gprs_send_data_MG323(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
	char buff[50];

	if(len != 0)
		TcpIPSd_SAVE(dat, len);//����

	if(gTcpIpbusy == TcpIPBusy){//���ܷ��ͣ���ȴ�������
//		debugstring("@");
		if(gTcpIpbusydly == 0){
			gTcpIpbusy = TcpIPFree;
#ifdef _debug_gprs
			debugstring("()");
#endif
		}
		return;
	}
	if ((gGPRS_data_style.ucReadStyle == GPRS_SISR)||(gGPRS_data_style.ucReadStyle == GPRS_SISRing)){//����ȡ���ݣ����ܷ�����
#ifdef _debug_gprs
//		debugstring("{}");
#endif
		return;
	}

	gTcpIpbusydly = 5;
	len = TcpIPSd_Pick(gMC55iSendbuff+12);
	if((len==0)||(len> 1024)){
		return;
	}
	memcpy(gMC55iSendbuff, "������Ч", 8);
	memcpy(gMC55iSendbuff+8, (unsigned char*)&len, 4);
	//	memcpy(gMC55iSendbuff+12, dat, len);
	sprintf(buff, "at^sisw=0,%d\r\n", len);
	gprs_snd(strlen(buff), buff);
	gTcpIpbusy = TcpIPBusy;
}

//�������УǣУңӰ���linkNum = ���͵���·ѡ��
void gprs_send_data_MC323(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
	unsigned int index;
#ifdef _debug_gprs
	debugstring("Gprs_send_data:");
	debugdata((unsigned char*)&len, 1, 1);
#endif
	if (len == 0){
		return;
	}
#ifdef _debug_gprs
	debugstring("Gprs_send_data\r\n");
#endif

	index = 0;
//	sprintf(buff, "AT%%IPSENDX=%d,\"", linkNum);
	memcpy(comSendbuff, "AT^IPSENDEX=1,1,\"",17);
	index = 17;

	BCD2Ascii(dat, comSendbuff+index, len);
	index += (len*2);
	memcpy(comSendbuff+index, "\"\r\n\x0", 4);
	index += 4;
	clr_wdt();
	gprs_snd(index, (char*)comSendbuff);
}

void ModeS_IPSend(unsigned int len, unsigned char Gmode)
{
	unsigned int index = 0; 
	memset(comSendbuff,0,50);
	index = 0;
	if(Gmode == Gmode_ALL4G)
		index = sprintf((char *)comSendbuff,"AT+QISEND=0,%d\r\n",len);
	else	//CDMA2000
		index = sprintf((char *)comSendbuff,"AT^IPSEND=1,%d\r\n",len);
	
#ifdef _debug_gprs
	debugstring("SEND_SEND:");
	debugstring((char *)comSendbuff);
	
	debugstring("\r\nwait >\r\n");
	printf("gprsRevBuf[gprsIndex]=%d\r\n",gprsIndex);
	debugdata(gprsRevBuf,gprsIndex,1);	
#endif

	gprs_snd(index, (char*)comSendbuff);


	gTcpIpbusy = TcpIPBusy;
	gTcpIpbusydly = 3;		//
		
}

//�յ�>�ź�������
void ModeS_DataSend(unsigned int len, unsigned char *dat)
{
	gprsAnswer = 0;
//	memcpy(dat+len, "\x0d\x0a", 2);
#ifdef _debug_gprs
	debugstring("ModeS DataSend:");
	debugdata(dat,len,1);
#endif
	gprs_snd(len, (char*)dat);

}


//#define GPRS_WRITE_WAIT	0xE4	//����3G ģ�鷢��ipsende ��ȴ����ڵġ�>����־�����ʱ���Ҫ��������


//�������УǣУңӰ���linkNum = ���͵���·ѡ��  ֻ��������
void gprs_send_data_CDMA3G(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
//	unsigned int index;

	if (len == 0){
#ifdef _debug_
		debugstring("0 return---\r\n");
#endif
		return;
	}



	if(len != 0)
		TcpIPSd_SAVE(dat, len);//����


// 	  	if((gTcpIpbusy != TcpIPFree)){//���ܷ��ͣ���ȴ�������
// 			if(gTcpIpbusydly == 0){
// 				gTcpIpbusy = TcpIPFree;
// 				gGPRS_data_style.ucSendstyle = 0;
// 				gGPRS_data_style.ucReadStyle = 0;
// #ifdef _debug_gprs
// 				debugstring("((())");
// #endif
// 			}
// 		}

// 		memcpy(gMC55iSendbuff, "������Ч", 8);
// 		memcpy(gMC55iSendbuff+8, (unsigned char*)&len, 4);
// 		memcpy(gMC55iSendbuff+12,dat,len);
// 		gGPRS_data_style.uiNum = len;	
		


}
//�������УǣУңӰ���linkNum = ���͵���·ѡ��
void gprs_send_data_GT900(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
	char buff[50];
	unsigned int index;
	unsigned short t,t1;
	
#ifdef _debug_gprs
	debugstring("Gprs_send_data:");
	debugdata((unsigned char*)&len, 1, 1);
#endif
	if (len == 0){
		return;
	}

	if(checkKEK() < 0) 
		return;
#ifdef _debug_gprs
	debugstring("Gprs_send_data\r\n");
	debugdata((unsigned char*)&gprsAnswer, 1, 1);
#endif
	if(gprsAnswer == 'O'){
		if(len > 512){
			if(gprsSendDlyflag != 'T'){
				gprsSendDlyflag = 'T';
				strcpy((char*)buff, "AT%IPSP=1000\r\n");//
				gprs_snd(strlen(buff), buff);
				set_timer0(50, 2);
				while(gprsAnswer != 'O'){
					clr_wdt();
					if(get_timer0(2)==0)
 						break;
				}
			}
		}
		else{
			if(gprsSendDlyflag != 'N'){
				gprsSendDlyflag = 'N';
				strcpy((char*)buff, "AT%IPSP=0\r\n");//
				gprs_snd(strlen((char*)buff), buff);
				set_timer0(50, 2);
				while(gprsAnswer != 'O'){
					clr_wdt();
 					if(get_timer0(2)==0)
 						break;
	//				debugdata((unsigned char*)&gprsAnswer, 1, 0);
				}
			}
		}
		if(len > 512){//һ�������Ͳ���,��Ҫ������ʱ����			
			index = 0;
			memcpy(comSendbuff, "AT%IPSEND=\"",11);
			index = 11;

			t = len/2;//�ȷ���һ��
			BCD2Ascii(dat, comSendbuff+index, t);
			index += (t*2);
			memcpy(comSendbuff+index, "\"\r\n\x0", 4);
			index += 4;
			clr_wdt();
			gprsAnswer = 0;
			gprs_snd(index, (char*)comSendbuff);
			set_timer0(50, 2);
			while(gprsAnswer != 'O'){
				clr_wdt();
//				debugdata((unsigned char*)&gprsAnswer, 1, 0);
				if(get_timer0(2)==0)
					break;
			}
			
			index = 0;
			memcpy(comSendbuff, "AT%IPSEND=\"",11);
			index = 11;
			
			t1 = len-t;//�ٷ���һ��
			BCD2Ascii(dat, comSendbuff+index, t1);
			index += (t1*2);
			memcpy(comSendbuff+index, "\"\r\n\x0", 4);
			index += 4;
			clr_wdt();
			gprs_snd(index, (char*)comSendbuff);
		}
		else{
			index = 0;
//			sprintf(buff, "AT%%IPSENDX=%d,\"", linkNum);
			memcpy(comSendbuff, "AT%IPSEND=\"",11);
			index = 11;

			BCD2Ascii(dat, comSendbuff+index, len);
			index += (len*2);
			memcpy(comSendbuff+index, "\"\r\n\x0", 4);
			index += 4;
			clr_wdt();
			gprs_snd(index, (char*)comSendbuff);
		}
	}
}
extern void gprs_send_data_OTHER(unsigned char linkNum, unsigned int len, unsigned char *dat);
extern void gprs_send_data_WIFI_TLH(unsigned char linkNum, unsigned int len, unsigned char *dat);
void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat)
{
	unsigned char gprsMode;
	gprsMode = in_getGprsModeStyle();
	gprsMode &= 0x0F;
	if(gprsMode == Gmode_900C){
		gprs_send_data_GT900(linkNum, len, dat);
	}
	else if(gprsMode == Gmode_MC323){
		gprs_send_data_MC323(linkNum, len, dat);
	}
	else if((gprsMode == Gmode_CDMA2000)||(gprsMode == Gmode_ALL4G)){
		gprs_send_data_CDMA3G(linkNum, len, dat);
	}
	else if(gprsMode == Gmode_OTHER){
		gprs_send_data_OTHER(linkNum, len, dat);
	}
	else if(gprsMode == Gmode_WIFI_TLH){
		gprs_send_data_WIFI_TLH(linkNum, len, dat);
	}
	else{
		gprs_send_data_MG323(linkNum, len, dat);
	}
}


//�ر�GPRS��Դ
void GPRS_close_Gt900(unsigned int dlyt)
{
	char buff[50];
	strcpy(buff, "AT%MSO\r\n");
	gprs_snd(8, buff);
	//MC55i���ܹرյ�Դ�����ǿ��Թر�ģ��
	set_timer0(dlyt, 1);//�յ�OK�� �Źرճɹ�	
}
//GPRS����ʽת�� 
unsigned short PeckData(FRAME_BUF *dst, char *scr)
{
	char *sp1, *sp2;
	unsigned int t;
//	unsigned int pos;
	unsigned short l[2];
	
//	pos = 0;
//	i = 0;
	
	l[0] = 0;
	l[1] = 0;
	sp1 = strstr(scr, ":");
	if(sp1 == 0)
		return 0;
	sp2 = strstr(scr, ",");
	if(sp2 == 0)
		return 0;
	sp1++;
	while(sp1 < sp2){
		l[0] = l[0]*10 + (*sp1 - '0');
		sp1++;
	}
	sp1 = sp2+1;
	sp2 = strstr(sp1, ",");
	if(sp2 != 0){
		while(sp1 < sp2){
			l[1] = l[1]*10 + (*sp1 - '0');
			sp1++;
		}
	}
	sp2 = strstr(sp1, "\"");
	if(sp2 != 0){
		sp2++;
		if(l[1] > 0)
			t = 1;
		else
			t = 0;
		if(l[t] > 1024)//����̫��
			return 0;
		Ascii2BCD((unsigned char*)sp2, dst->FrameBuf, l[t]*2);
		memcpy(dst->uiFrameLen, (unsigned char*)&l[t], 2);
		dst->ucFrameType = t;
		return l[t];
	}

	return 0;
}
//mode == 1 �ر�����1 mode==2:�ر�����2 mode==0:�رյ�����
void tcpipClose(unsigned char mode)
{
	char buffer[30];
	unsigned char gGprsMode;
	
	memset(gBusVerInfo.newbusBLKVer, 0, 2);//�Ͽ����Ӻ�Ҫ���µ�½
	gmissflag = 0;

	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;
	

#ifdef _debug_gprs
	debugstring("tcpip Close--\r\n");
#endif
	if(((GPRSLinkProcess >= 20)&&(GPRSLinkProcess < 0x30))||(GPRSLinkProcess == 0xA0))
		;
	else{
		GPRSLinkProcess = 0;
		return;
	}

	if(gGprsMode == Gmode_WIFI_TLH){
		GPRSLinkProcess = GPRS_NEED_CLOSEIP;
		return;
	}
	set_timer0(5000, 1);
	
	if((gGprsMode == Gmode_MC323)||(gGprsMode == Gmode_CDMA2000)){
		strcpy(buffer, "AT^IPCLOSE=1\r\n");//�ر�����
	}
	else if(gGprsMode == Gmode_MG323){
		strcpy(buffer, "at^sisc=0\r\n");//�ر�����
	}
	else if(gGprsMode == Gmode_ALL4G){
		strcpy(buffer, "AT+QICLOSE=0\r\n");//�ر�����
	}
	else{
		if(mode == 1){
			strcpy(buffer, "AT%IPCLOSE=1\r\n");//�ر�����
		}
		else if(mode == 2){
			strcpy(buffer, "AT%IPCLOSE=2\r\n");//�ر�����
		}
		else if(mode == 5){
			strcpy(buffer, "AT%IPCLOSE=5\r\n");//�ر�����
		}
		else{
			strcpy(buffer, "AT%IPCLOSE\r\n");//�ر�����
		}
	}
	gGPRS_data_style.ucReadStyle = 0;
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gAPN_Style = 0;
	gprs_snd(strlen(buffer), (char*)buffer);
	GPRSLinkProcess = GPRS_TCPCLOSE_CMD;
}

void out_GPRS(void)
{
	char buffer[20];
	unsigned char gGprsMode;
	
	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;
	if(gGprsMode == Gmode_MC323){
		strcpy(buffer, "AT%CLOSE=7\r\n");//NOTE��PDP ȥ����
	}
	else{
		strcpy(buffer, "AT+CGATT=0\r\n");//NOTE��PDP ȥ����
	}
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gprs_snd(strlen(buffer), (char*)buffer);
	GPRSLinkProcess = GPRS_Out_CMD;
	set_timer0(30000, 1);
}

void IPERROR_DEAL(unsigned char mode)
{
#ifdef _debug_gprs
	debugstring("IP ERROR DEAL:");
	debugdata((unsigned char*)&mode, 2, 1);
#endif
	switch (mode)
	{
	case 0:	//�û���������ע�ᱻ�����ܾ�������APN ���󣬿�Ƿ�ѣ�����֧��GPRS ҵ��ȡ�
//		set_timer0(10000, 1);
		GPRSLinkProcess = 0;
				debugstring("--2 close\r\n");
//		display(0,4,"�ƶ�Ƿ��",1);
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		break;
	case 1://û�е�½����
	case 21://��ǰû�����PDP ����
//		set_timer0(5000, 1);
		GPRSLinkProcess = 0;
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		break;
	case 2://��ǰ�Ѿ������Ӵ���
		if((GPRSLinkProcess >= 20)&&(GPRSLinkProcess<0xB0))
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;// (0);
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		break;
	case 4://TCP ���ӱ��Է��ܾ�
	case 5://TCP ���ӳ�ʱ������IP �Ͷ˿ڲ���ȷ
		set_timer0(5000, 1);//
		if(GPRSLinkProcess > 20)
			GPRSLinkProcess = 20;
		break;
	case 9://��ǰû��TCP ��UDP ���Ӵ���(���IPSEND)
		set_timer0(5000, 1);//
		if(GPRSLinkProcess > 20){
			GPRSLinkProcess = 20;
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
		}
		break;
	case 20://TCP ���ͻ�������
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		//(0);
		break;
	case 7://��ǰ�Ѿ���������������
		if(GPRSLinkProcess > 19)
			GPRSLinkProcess = 19;
		break;
	case 8://��ǰ���������Ӵ��ڣ������޸�link_flag�����IOMODE��
//		GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
		break;
	case 3://����������Ϸ�
	case 6://��ǰ�Ѿ���������������
	case 10://
	case 11://���ջ�����
	case 12://���ջ���������
	case 13://������Ч���ݳ���512 �ֽ�
	case 14://���û�ģʽ�£��ֽ���Ӧ��Ϊż������
	case 15://�û�ģʽ��������ַǷ��ַ�
	case 16://����Ų�����
	case 17://�Ƿ���ɾ������
	case 18://��������
	case 19://
	case 22://��ǰ%ETCPIP ���ڼ���״̬������ʹ�ø�����
	case 23://����������
	case 24://����������ʱ
	case 25://��������δ֪����
	case 26://�������ѿ���
	default:
		break;
	}
}

void gprsCmdDeal_CDMA(void)
{
	char *ret;
	unsigned int i, index;
	
	if(strstr((char*)gprsRevBuf, "^MODE:2") != 0)
	{
		gprsErrorTimes = 0;
		
		GPRSLinkProcess = GPRS_LINK_UP;//��Ҫ��POWN�����ߣ�GPRS_AT_WAIT;	//GPRSģ��������
		set_timer0(400, 1);//���ӹر�,��ʱ����������
		
		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--MC323 ready\r\n");
#endif
		goto Gprsdealcmdend;
	}
	if(strstr((char*)gprsRevBuf, "^MODE:0") != 0){
		
		gprsErrorTimes = 0;
		
		GPRSLinkProcess = GPRS_LINK_CMD;//��Ҫ��POWN�����ߣ�GPRS_AT_WAIT;	//GPRSģ��������
		set_timer0(400, 1);//���ӹر�,��ʱ����������
		
		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--MC323 OFF \r\n");
#endif
		goto Gprsdealcmdend;
	}

// 	if(strstr((char*)gprsRevBuf, "^HCMGSS:") != 0){//�������
// 		SendmsgEnd();
// 		set_timer0(40, 1);
// 		goto Gprsdealcmdend;
// 	}
// 	if(strstr((char*)gprsRevBuf, "^HCMGSF:") != 0){//����ʧ��
// 		gmissflag = 0;
// 		goto Gprsdealcmdend;
// 	}

// 	if(strstr((char*)gprsRevBuf, "remote close") != 0){//�������ر�
// 		set_timer0(1000, 1);
// #ifdef _debug_gprs
// 		debugstring("remote close!!!!\r\n");
// #endif
// 		if((GPRSLinkProcess == 0xA0)||(GPRSLinkProcess == 0x16)){
// 			GPRSLinkProcess = GPRS_NEED_CLOSEIP;
// 		}
// 		goto Gprsdealcmdend;
// 	}
	if(gmissflag == MISS_G_MSG2){
		if(strstr((char*)gprsRevBuf, "OK") != 0)
			goto Gprsdealcmdend;
	}

	if(GPRSLinkProcess == GPRS_AT_WAIT)
		goto Gprsdealcmdend;

	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I�ر���
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//�رճɹ� 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//�رղ��ɹ� 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
	}
	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 19:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			if(GPRSLinkProcess == 15){///�п����Ѿ�������
				GPRSLinkProcess++;
				set_timer0(500, 1);
				gprsErrorTimes = 0;
				gprsAnswer = 'O';
				goto Gprsdealcmdend;
			}
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case GPRS_TCPCLOSE_CMD://TCPIP�ر��У��ȴ�OK����
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if((GPRSLinkProcess > 20)&&(GPRSLinkProcess<0x30))
				GPRSLinkProcess = 20;// ��������
			set_timer0(500, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		goto Gprsdealcmderror;
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			for(i=5; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
		goto Gprsdealcmdend;
//		break;
	case 17:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(10, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
//		break;
	case 18:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, "%DTUIP");
			if(ret == 0)
				goto Gprsdealcmdend;
			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			if(strstr((char*)gprsRevBuf, "0.0.0") != 0){//1��PPP ����δ�ɹ�
				set_timer0(15000, 1);
				GPRSLinkProcess = 16;
			}
			else{
				GPRSLinkProcess++;
				set_timer0(10, 1);
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else
			return;
//		break;
	case 20:
		debugstring("CMD:");
		debugstring((char*)gprsRevBuf);
//		ret = strstr((char*)gprsRevBuf, "%TCPSTATUS");
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if((gmissflag & 0xf0)==0x10){//��½�ƶ�IF1
				isNetOK[0] = 1;
			}
			else if((gmissflag & 0xf0)==0x20){//��½�ƶ�IF2
				isNetOK[0] = 2;
			}
			else{//��½������������
				isNetOK[0] = 3;
			}
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			CDMA_REV_LEN = 0;
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
//			if(strstr((char*)gprsRevBuf,"Connection already established") != 0){
			GPRSLinkProcess = 15;//GPRS_NEED_CLOSEIP;//(0);
			set_timer0(500, 1);
			goto Gprsdealcmdend;
//			}
// 			else
// 				goto Gprsdealcmderror;
		}
		else{
			return;
		}
//		break;
	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
//		break;
	case 5:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			ret = strstr((char*)gprsRevBuf, "-");
			if(ret != 0){
				if(*(ret+1) != '0'){
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else 
			return;
//		break;
	default:
		break;
	}

	ret = strstr((char*)gprsRevBuf, "^IPDATA:");//^IPDATA:1,10,1234567890

	if(ret != 0){
		ret = strstr((char*)gprsRevBuf, ",");//
		if(ret == 0)
			goto Gprsdealcmdend;
		else{//�������ݽ���
			ret++;
			CDMA_REV_LEN = 0;
			for(i = 0; i<gprsIndex;i++){
				if((*ret >= '0')&&(*ret <= '9')){
					CDMA_REV_LEN *= 10;
					CDMA_REV_LEN += (*ret-'0');
				}
				else if((*ret == 0x0d)||(*ret == 0x0a)||(*ret == ','))
					break;
				ret++;
			}
#ifdef _debug_
			debugstring("R LEN:");
			debugdata((unsigned char*)&CDMA_REV_LEN, 4, 1);
#endif
			if(gprsIndex >= (CDMA_REV_LEN+15)){//^IPDATA:1,10,1234567890 ���ݼ���ǰ����ֽڣ��ټ���0d 0a OK���ٶ���14�ֽ�
#ifdef _debug_gprs
				debugstring("R OK:");
#endif
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(index=0;index<MAX_RCV_PACKET_NUM;index++){
					if(gPacketRcvBuf[index].ucItemStatus != 0xea)
						break;
				}
				if(index>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//�����������˰�����
					return;
				}

				for(i=0; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				for(; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				if(i >= 16){
					CDMA_REV_LEN = 0;
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);
					return;
				}
				gPacketRcvBuf[index].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&CDMA_REV_LEN, 2);
				memcpy(&gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf+i, CDMA_REV_LEN);
				gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_gprs
				debugstring("����:");
				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, CDMA_REV_LEN, 1);
#endif
				CDMA_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				goto Gprsdealcmdend;
			}

			return;
		}
	}	
	if(strstr((char*)gprsRevBuf, "^DSDORMANT:") != 0){
		for(i=11; i<gprsIndex; i++){
			if(gprsRevBuf[i] == 0x0d)
				break;
			else if(gprsRevBuf[i] == '1'){//������
				gprs_send_data_MC323(1, 1, gprsRevBuf);//
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmderror:
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
		if(GPRSLinkProcess == 0xA0){
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;
			goto Gprsdealcmdend;
		}
		gprsErrorTimes++;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				IPERROR_DEAL(i);
			}
		}
		goto Gprsdealcmdend;
	}

	ret = strstr((char*)gprsRevBuf, "^IPCLOSE");
	if((ret != 0)||(strstr((char*)gprsRevBuf, "close")!=0)||(strstr((char*)gprsRevBuf, "CLOSE")!=0)){
		set_timer0(500, 1);//���ӹر�,��ʱ����������
//		gprsErrorTimes = 0;
		for(i=0;i<(strlen((char*)gprsRevBuf));i++)
		{
			if(gprsRevBuf[i] == '5'){
				GPRSLinkProcess = 0;
				break;
			}
			else if((gprsRevBuf[i] == 0x0a)||(gprsRevBuf[i] == 0x0d))
				break;
		}
		if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
			GPRSLinkProcess = 20;
		isNetOK[0] = 0;

		memset(gBusVerInfo.newbusBLKVer, 0, 2);
		gmissflag = 0;
#ifdef _debug_
		debugstring("--IPCLOSE--");
		debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
#endif
		goto Gprsdealcmdend;
	}
Gprsdealcmdend:
	if(gprsErrorTimes > 20){//�������̫�࣬����ģ��
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//�ر�1S
//		debugstring("--1 close\r\n");
		if(GPRSLinkProcess == 20){
			if(gmissflag & MISS_GJ){//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return; //�������ͬʱ������,���˳�����������.
			}
		}
// 		if(GPRSLinkProcess == 20){//��½���˺�̨
// 			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
// 		}
// 		else
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
	gprsIndex = 0;
	memset(gprsRevBuf, 0, sizeof(gprsRevBuf));
	return;
}

//sin ��ʼ�� num �� s2ʱ�򷵻� s2 �ĵ�ַ  ��AT^IPOPEN=1,1200,"113.89.45.231",8811,0
//my_strstr(s1,",",3) ������������������

char *my_strstr(const char *sin,const char *s2, int num)
{
	const char *s1;
    int n;
	int i=0;

	s1 = sin;
    if(*s2)
    {
        while(*s1)
        {
			
			for(n=0;*(s1+n)==*(s2+n);n++)
			{
				if(!*(s2+n+1)){
					i++;
					if (i==num)
						return(char*)s1;
				}
			}
			s1++;
			
        }
        return NULL;
    }
    else
        return (char*)s1;
}


void gprsCmdDeal_CDMA_3G(void)
{
//	unsigned char t;
//	unsigned int count=0;
//	unsigned short len=0;
//	char code[5];
	char *ret=NULL;
//	char *start_p=NULL;
//	char *temp_p=NULL;
	unsigned int i;
//	unsigned char index;
	

	if(gprsIndex < 3){	//  �� ��\r\n>�� �����������Ƚ����ˣ�Ҫ�˳� 
#ifdef _debug_gprs
	//	debugstring("low\r\n");
#endif
		return;
	}

#ifdef _debug_gprs
// 	debugstring("deal3G:");
// 	debugstring((char *)gprsRevBuf);
#endif



/*
	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I�ر���
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//�رճɹ� 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto CDMA_3Gdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//�رղ��ɹ� 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto CDMA_3Gdealcmdend;
		}
	}
*/

	

	ret = strstr((char*)gprsRevBuf, "^MODE:");
	if(ret != 0){
#ifdef _debug_gprs
	//	debugstring("^MODE,��\r\n");
#endif

		gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
		goto CDMA_3GdealEnd;
		
	}


	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 4:
	case 6:
	case 7:
	
//	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 17:
	case 19:
	case 39:
	case 40:


		ret = strstr((char*)gprsRevBuf, "OK");	//�Ƚ���û��OK
		if(ret != 0){

			if(GPRSLinkProcess == 19){
				GPRSLinkProcess = 39;
			}
			else if(GPRSLinkProcess == 39)
				GPRSLinkProcess = 40;
			else if(GPRSLinkProcess == 40)
				GPRSLinkProcess = 20;
// 			else if ((GPRSLinkProcess == 9)&&(MG_CEM_FLAG == Gmode_CEM630))
// 			{
// 				GPRSLinkProcess = 
// 			}
			else
				GPRSLinkProcess++;
			set_timer0(0, 1);		//ʱ�䲻�ٵȴ���ֱ�ӽ�����һ�����Ӵ���
			gprsErrorTimes = 0;		//��������־
			gprsAnswer = 'O';

			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			if(GPRSLinkProcess == 17)
				set_timer0(500, 1);
			gprsErrorTimes++;
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

		
		}

		goto CDMA_3Gdealcmdend;
		//set_timer0(100,1);
		//break;


	case 2:

		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			//
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			//goto CDMA_3Gdealcmdend;
		}
		else{
			gprsReady = GPRS_GOON;
		
		}
		goto CDMA_3Gdealcmdend;
		//break;

	case 3:

		if(strstr((char*)gprsRevBuf, "OK") != 0){
		
	#ifdef _debug_gprs
	//			debugstring("is CEM630????\r\n");
	#endif
// 			if (strstr((char*)gprsRevBuf, "CEM630") != 0)
// 			{
// 			}
			set_timer0(10, 1);	
			gprsErrorTimes = 0;		//��������־
			gprsAnswer = 'O';
			GPRSLinkProcess++;
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

		}
		else
			gprsReady = GPRS_GOON;
		break;

	case 8:	//^IPCALL: 0,113.113.116.56          // 0 ����,  1 �Ͽ�

		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			ret = NULL;
			ret = strstr((char*)gprsRevBuf, "^IPCALL: ");
			if(ret != 0){
	
					ret = strstr((char*)gprsRevBuf, ",");
					if (ret !=0)
					{
#ifdef _debug_gprs
// 						debugstring("IPCALL FLAG:");
// 						debugdata(ret-1,1,1);
#endif
						if(*(ret-1) == '0')		//CEM631
						{
							gprsErrorTimes = 0;
							GPRSLinkProcess++;
							set_timer0(0, 1);
						}
						gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
						goto CDMA_3GdealEnd;
					}
			

			}
		
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			gprsErrorTimes++;
		}
// 		else
// 		{
// 			gprsReady = GPRS_GOON;
// 		}


		break;


	case 5:

		if(strstr((char*)gprsRevBuf, "OK") != 0){


			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			gprsAnswer = 'O';


			ret = NULL;
			ret = strrchr((char*)gprsRevBuf, ',');		//�����һ����

			if(ret != 0){
				if((*(ret+1) == '1')||(*(ret+1) == '4'))
				{
					gprsErrorTimes = 0;
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			
			goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			gprsErrorTimes++;
			goto CDMA_3Gdealcmdend;
		}
		else {
			gprsReady = GPRS_GOON;
			
		}
		break;
	


	case GPRS_Out_CMD:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

			GPRSLinkProcess = 5;// ��������
			set_timer0(1000, 1);
			gprsErrorTimes = 0;
			
			gprsAnswer = 'O';
			//goto CDMA_3GdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes+=5;
//			debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

			//goto CDMA_3GdealEnd;
		}
		goto CDMA_3GdealEnd;
	case GPRS_TCPCLOSE_CMD://TCPIP�ر��У��ȴ�OK����
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess > 20)
				GPRSLinkProcess = 9;// �������ӣ���������ʱ��Ҫ��������ģʽ,����=20ֱ���ڽ������ӣ���Ȼ��ʹ��0ģʽ��
			set_timer0(200, 1);		//�����޸ģ����滹��IPSTAT��1��1�����õȻᡣ
//			gprsErrorTimes = 0;
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
#ifdef _debug_gprs
			debugstring(" GPRS_TCPCLOSE_CMD(0xF1) receive\r\n");
#endif		
			
			gprsAnswer = 'O';
			goto CDMA_3GdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes+=5;
//			debugdata((unsigned char*)&gprsErrorTimes, 1, 1);

			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			goto CDMA_3GdealEnd;
		}
		
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

			for(i=0; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

			goto CDMA_3Gdealcmdend;
		}
		else{
			gprsReady = GPRS_GOON;
			return;
		}
//		break;
	case 18:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			//goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������

			gprsErrorTimes++;
			//goto CDMA_3Gdealcmdend;
		}
		else{
			gprsReady = GPRS_GOON;
			
		}
		goto CDMA_3Gdealcmdend;

//		break;
	case 20:
		
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			
			gprsAnswer = 'O';
#ifdef _debug_gprs
			debugstring("CEM630_link OK\r\n");
// 					debugstring("34343\r\n");
// 					debugstring(gprsRevBuf);
#endif
// 					ret = strstr((char*)gprsRevBuf, "^IPOPEN");
// 					if(ret != 0){
			isNetOK[0]=3;		//socket���ӳɹ�
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			//}
			goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){	//�����޸�
#ifdef _debug_gprs
			
			debugstring("QUANERRORERROEERROE!!!!\r\n");
#endif
			gprsErrorTimes++;			//???????????????
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);//�رգ���Ҫ��ʱ����������
			gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
			
			goto CDMA_3Gdealcmdend;
		}
		else{
			return; //��û����
		}
//		break;
	case 21:
		GPRSLinkProcess++;
		break;

	default:


	/**********************һ����GM631******************************************/

		

/************************************************************************

		//���ݽ���		//^IPRECV:1,113.89.126.181,8082,9,564879!@#
		//				  ^IPRECV:1,113.118.100.92,8811,27,
		ret = strstr((char*)gprsRevBuf, "RECV");
		if(ret != 0){

			//��ʱ����˶�2F �ˣ��������յ����ݣ���֪��ʲôԭ�����������Ϊ����OK
			if ((GPRSLinkProcess != 0x16)&&(GPRSLinkProcess != GPRS_SENDING_CMD))
			{
				GPRSLinkProcess = 0x16;
				isNetOK[0] = 3;		
			}

#ifdef _debug_gprs
// 		debugstring("dat:");
// 		debugdata(gprsRevBuf,gprsIndex,1);
#endif

			ret = my_strstr((char*)gprsRevBuf, ",",3);
			if(ret != 0){	
				i=0;
				ret++;
				while((*ret >= '0')&&(*ret <= '9'))
				{
					if (i>1024){		//�쳣��
						i=0;
						break;		
					}
					
					i = i*10 + (*ret-'0');
					ret++;
				}



				len = i;



				if (*ret==',')		//���Ⱥ����,
				{

					ret ++;			//^IPRECV:1,119.136.47.178,8811,27,BDBDBDD0D0AA83151001014255010100092016013016201490001B6C

					start_p = ret;
					//�����������п����� 0x00�����Կ����Ҳ��������β��\r\n, ����Ҫ���������ң�����һ��Ҫ����������������
					//�����λ�˿��ܻ�����������쳣��һֱ���ϲ�ȥ�ˣ���Ϊ��λ��
// 					ret = strstr((char*)start_p, "\x0D\x0A",1);		
// 					if (ret !=0)
// 					{
// 						count = ret - start_p ;		//����ָ���ַ��ͷ���������ж����ֽڣ����ж���������û��
// 					}else{
// 						debugstring("eeeee\r\n");
// 					}


					count = (unsigned char *)start_p- gprsRevBuf;
					
#ifdef _debug_gprs
// 					debugstring("end:");
// 					debugdata(ret,3,1);
// 					debugstring("end:");
// 					debugdata((unsigned char *)gprsRevBuf,2,1);
// 					debugstring("\r\ngprsIndex:");
// 					debugdata((unsigned char *)&gprsIndex,2,1);
// 					debugstring("code len :");
// 					debugdata((unsigned char *)&len,4,1);
// 					debugstring("count len:");
// 					debugdata((unsigned char *)&count,4,1);
#endif



					if (gprsIndex < (len + count + 2)  )		//û���� ,��Ϊ�����������п�����0x0D,0x0a, 
					{
#ifdef _debug_
						debugstring("BG\r\n");
#endif
						return ;
					}


#ifdef _debug_gprs
					//debugstring("\r\n--over---\r\n");
#endif		
#ifdef _debug_gprs
				
					MSG_LOG("gprsIndex:%d\r\n",gprsIndex);
 					debugstring("gprsRevBuf:");
 					debugdata(gprsRevBuf,gprsIndex,1);
#endif
					gTcpIpbusy = TcpIPFree;
					gTcpIpbusydly = 0;			//����
					gErrorFlag &= (~ERROR_BUS_CONNECT);// ��������ʶ �ܽ�����˵�����綼����
					gGPRS_data_style.ucReadStyle = 0;
					gprsAnswer = 'O';
					gprsErrorTimes = 0;
					gprsSNDerro = 0;
					for(i=0;i<MAX_RCV_PACKET_NUM;i++){
						if(gPacketRcvBuf[i].ucItemStatus != 0xea)
							break;
					}
					if(i>=MAX_RCV_PACKET_NUM){
						gprsReady = GPRS_CLEAR_INDEX;
#ifdef _debug_gprs9
						debugstring("full !!!\r\n");
#endif
						goto CDMA_3Gdealcmdend;
					}
					gPacketRcvBuf[i].LinkFrame.ucFrameType = 1;
					memcpy(gPacketRcvBuf[i].LinkFrame.uiFrameLen, (unsigned char*)&len, 2);
					memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf, start_p, len);
					//Ascii2BCD((unsigned char *)ret,gPacketRcvBuf[i].LinkFrame.FrameBuf,guiMC323_REV_LEN*2);
					memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf,start_p,len);
					gPacketRcvBuf[i].ucItemStatus = 0xea;
#ifdef _debug_gprs
//  					debugstring("server dat:");
//  					debugdata((unsigned char *)start_p,len,1);
// 					debugstring("\r\n����2G:");
// 					debugdata((unsigned char*)&gPacketRcvBuf[i].LinkFrame, guiMC323_REV_LEN+3, 1);
// 					debugstring("���ӡ30Byte~~~���滹����??-------\r\n");
// 					debugdata((unsigned char*)&gPacketRcvBuf[i].LinkFrame.FrameBuf+guiMC323_REV_LEN, 30, 1);
#endif
					// 				debugstring("���ӡ30Byte~~~���滹����??-------\r\n");
					// 				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame.FrameBuf+guiMC323_REV_LEN, 30, 1);
					

					guiMC323_REV_LEN = 0;
					
					gprsReady = GPRS_CLEAR_INDEX;


					goto CDMA_3GdealEnd;
				}	
				
			}
			
			//set_timer0(0, 1);
			goto CDMA_3Gdealcmdend;
		}

		************************************************************************/


		//^IPSEND:1, 78
		//^IPSTAT:1,1   socket ʧ��  GM631
		ret = strstr((char*)gprsRevBuf, "IPSTAT");
		if(ret != 0){//���ӶϿ�
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				if(*(ret+1) == '1')		//CEM631  
				{
					
#ifdef _debug_gprs
					debugstring("--IPCLOSE-***CEM631-");
#endif					
					gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
					
					
					// 					if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					// 						GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					memset(gBusVerInfo.newbusBLKVer, 0, 2);
					
					isNetOK[0]=0;		//socket���ӶϿ�
					//gprsErrorTimes ++;
					//stopCCgprs(ERROR_BUS_CONNECT);
// 					if(GPRSLinkProcess > 20)//�����޸ģ�AT^IPCLOSE=1���滹��IPSTAT��1��1���룬���������ͣ�ĶϿ������ӣ��Ͽ� ��l��������ж�
// 						GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;  
					if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
 						GPRSLinkProcess = 9;//GPRSLinkProcess = GPRS_NEED_CLOSEIP;		//�޸ģ��Ѿ��Ͽ��ˣ�����Ҫ�ٷ�CLOSEָ��		
					//GPRSLinkProcess= 6;	// ������ʮ�����Ƹ�ʽ ��������ָ��ģ�鲻�� �� �������Ӻ�̨
					set_timer0(3000, 1);
				}
			}
			goto CDMA_3Gdealcmdend;
		}
		


	/**********************************������GM631**************************************/


		break;
	}

// end switch();
	



	ret = strstr((char*)gprsRevBuf, "OK");
	if(ret != 0){
#ifdef _debug_gprs
		debugstring("WW\r\n");
#endif
		//if((gTcpIpbusy == TcpWaitOk)){
			gTcpIpbusy = TcpIPFree;
			gGPRS_data_style.ucSendstyle = 0;
			gGPRS_data_style.ucReadStyle = 0;
		//}

		gprsAnswer = 'O';
		guiMC323_REV_LEN = 0;
		gprsReady = GPRS_CLEAR_INDEX;	//���� gprsbuff ������
	}

//CDMA_3Gdealcmderror:
	
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
#ifdef _debug_gprs
	//	debugstring("tt=\r\n");
#endif
		gprsErrorTimes++;
		gprsReady = GPRS_CLEAR_INDEX;

	}
CDMA_3Gdealcmdend:
	
	if(gprsErrorTimes > 20){//�������̫�࣬����ģ��
#ifdef _debug_gprs
	//	debugstring("reset module!!!!!!!!!!\r\n");
#endif
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//�ر�1S
		//debugstring("--1 close\r\n");
		if((GPRSLinkProcess > 16)&&(GPRSLinkProcess < 0x90)){

			if(gmissflag & MISS_GJ){//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
#ifdef _debug_gprs
				debugstring("reset stop CCgprs:ERROR_BUS_CONNECT!\r\n");
#endif
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
//				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //�������ͬʱ������,���˳�����������.
			}
		}
		if(GPRSLinkProcess == 20){//��½���˺�̨
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		}
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
CDMA_3GdealEnd:
	
	if(gprsReady == GPRS_CLEAR_INDEX){//�������µ�����
#ifdef _debug_gprs
//		debugstring("XX\r\n");
// 		MSG_LOG("%d\r\n",gprsIndex);
// 		BCD_LOG(gprsRevBuf,gprsIndex,1);
#endif
		memset(gprsRevBuf, 0, GPRSCOMMAX);
		gprsIndex = 0;
		gprsReady = GPRS_GOON;
	}
	return;
}

//MC323ģ��
void MC55i_SISREAD(void)
{
	char buff[50];
	strcpy(buff, "at^sisr=0,1500\r\n");
	gprs_snd(strlen(buff), buff);
}

void gprs_DATA_snd(unsigned int len, unsigned char *dat)
{
	gprsAnswer = 0;
	memcpy(dat+len, "\x0d\x0a", 2);
	gprs_snd(len+2, (char*)dat);
}

void gprsCmdDeal_MG323(void)
{
	unsigned char t;
	unsigned int count;
	char code[5];
	char *ret;
	unsigned int i;
//	unsigned char index;
	
	if(gprsReady != 0xaa)
		return;

	if(gprsIndex < 3)
		goto GPRSdealEnd;

#ifdef _debug_gprs
	//debugstring("R:");

#endif

	if(strstr((char*)gprsRevBuf, "^SYSSTART") != 0)
	{
		gprsErrorTimes = 0;

		GPRSLinkProcess = GPRS_LINK_UP;//��Ҫ��POWN�����ߣ�GPRS_AT_WAIT;	//GPRSģ��������
		set_timer0(400, 1);//���ӹر�,��ʱ����������

		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--MG323 ready\r\n");
#endif
		goto Gprsdealcmdend;
	}
	if(strstr((char*)gprsRevBuf, "^SHUTDOWN") != 0)//ģ��ػ�
	{
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		set_timer0(2000, 1);//�յ�OK�� �Źرճɹ�
#ifdef _debug_gprs
		debugstring("MG323 SHUTDOWN\r\n");
#endif
		goto Gprsdealcmdend;
	}

	if(in_dealSoundIn() == 1)
		goto Gprsdealcmdend;

// 	if(strstr((char*)gprsRevBuf, "+CGREG") != 0){
// 		ret = strstr((char*)&gprsRevBuf, ",");
// 		if(ret != 0){
// 			ret++;
// 			if(*ret == '0'){
// //				debugstring("--0 close\r\n");
// 				gprsSingle = 0;
// 				//set_timer0(1000, 1);//�ر�1S
// 				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
// 				goto Gprsdealcmdend;
// 			}
// 			else if(*ret == '2'){
// 				gprsSingle = 1;
// 				gprsReady = 0;
// 				return;//���������磬����.
// 			}
// 		}
// 	}

	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I�ر���
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//�رճɹ� 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//�رղ��ɹ� 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
	}

	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 17:
	case 19:
	case 39:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess == 19){
#ifdef _debug_gprs
				debugstring("19--39\r\n");
#endif
				GPRSLinkProcess = 39;
			}
			else if(GPRSLinkProcess == 39)
				GPRSLinkProcess = 40;
			else if(GPRSLinkProcess == 40)
				GPRSLinkProcess = 20;
			else
				GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case 8:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(strstr((char*)gprsRevBuf, "+CGREG") != 0){
				ret = strstr((char*)&gprsRevBuf, ",");
				if(ret != 0){
					ret++;
// 					if(*ret == '0'){
// 						debugstring("  1111111    ");
// 						//				debugstring("--0 close\r\n");
// 						gprsSingle = 0;
// 						//set_timer0(1000, 1);//�ر�1S
// 						GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
// 						//goto Gprsdealcmdend;
// 					}
// 					else 
					if((*ret == '1')||(*ret == '5')){//1��ע�᱾������   5��ע�ᣬ����
						GPRSLinkProcess++;
						set_timer0(0, 1);
						gprsErrorTimes = 0;
						gprsAnswer = 'O';
						//goto Gprsdealcmdend;
					}
					else {//if(*ret == '2'){//0��û��ע�ᣬME Ҳû�г���ȥע�� 2��û��ע�ᣬME ������ͼȥע��  3��ע�ᱻ�ܾ�  4����֪������ע��״̬  8: ��������״̬
						gprsSingle = 1;
						gprsReady = 0;
						if(GPRSLinkProcess < 20)
							GPRSLinkProcess = 8;
						
						//goto Gprsdealcmdend;//���������磬����.
					}
				}
			}
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		else{		//û�յ�OK��Ҳû�յ�ERROR���ȡ�����
			return;
		}
		goto Gprsdealcmdend;//���������磬����.
		
	case 40:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess = 20;
			ret = strstr((char*)gprsRevBuf, "^SICI:");
			if(ret != 0){
				count = 0;
				t = 0;
				for(i=0; i<35; i++){
					if(ret[i]=='\"'){
						if(t == 0)
							t = i;
						else if(count == 0){
							count = i+1;
							break;
						}
					}
				}
#ifdef _debug_gprs
				debugstring("ʲô��:");
				debugstring(ret);
				debugdata((unsigned char*)&t, 4, 1);
				debugdata((unsigned char*)&count, 4, 1);
#endif

				memset(irda_rxBuf, ' ', 20);
				irda_rxBuf[20] = 0;
				memcpy(irda_rxBuf, ret+t, count-t);
			}
			goto Gprsdealcmdend;
		}
		gprsReady = 0;
		return;
	case GPRS_TCPCLOSE_CMD://TCPIP�ر��У��ȴ�OK����
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess > 20)
				GPRSLinkProcess = 20;// ��������
			set_timer0(0, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes+=5;
//			debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
			goto Gprsdealcmdend;
		}
		goto Gprsdealcmderror;
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			for(i=0; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 18:
#ifdef _debug_gprs
		debugstring((char*)gprsRevBuf);
		debugstring("*******");
#endif
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 20:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			guiMC323_REV_LEN = 0;
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;//(9);
			set_timer0(50, 1);//20121227
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case 21:
		ret = strstr((char*)gprsRevBuf, "^SISW:");
		if(ret != 0){
			if((gmissflag & 0xf0)==0x10){//��½�ƶ�IF1
				isNetOK[0] = 1;
			}
			else if((gmissflag & 0xf0)==0x20){//��½�ƶ�IF2
				isNetOK[0] = 2;
			}
			else{//��½������������
				isNetOK[0] = 3;
			}
			gTcpIpbusy = TcpIPFree;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			guiMC323_REV_LEN = 0;
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "^SIS:");
		if(ret != 0){
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			if(i<20){
				i = 0xffffffff;
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					else if((*ret >= '0')&&(*ret <= '9')){
						if(i == 0xffffffff)
							i = 0;
						i = i*10 + *ret-'0';
					}
					else
						break;
					ret++;
				}
			}
#ifdef _debug_gprs
			debugstring("SIS 21:");
			debugdata((unsigned char*)&i, 4, 1);
#endif
			if(i == 48){//^SIS: 0, 0, 48, Remote Peer has closed the connection
				set_timer0(2000, 1);//���ӹر�,��ʱ����������
				if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				isNetOK[0] = 0;
				
				memset(gBusVerInfo.newbusBLKVer, 0, 2);
				gmissflag = 0;
				//#ifdef _debug_
				// 				debugstring("--IPCLOSE--");
				// 				debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
				//#endif
			}
			else if(i == 21){//^SIS: 0, 0, 21, "Remote host has rejected the connection" �ܾ�����
				set_timer0(2000, 1);//���ӹر�,��ʱ����������
				if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				isNetOK[0] = 0;
				memset(gBusVerInfo.newbusBLKVer, 0, 2);
				gmissflag = 0;
			}
			else
				IPERROR_DEAL(i);
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
//			gprsErrorTimes++;
			set_timer0(20, 1);
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);//�رգ���Ҫ��ʱ����������
			goto Gprsdealcmdend;
		}
		break;
	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 5:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				if((*(ret+1) == '1')||(*(ret+1) == '5'))
				{
					gprsErrorTimes = 0;
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else {
			gprsReady = 0;
			return;
		}
//		break;
	default:
		ret = strstr((char*)gprsRevBuf, "^SISR:");
		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				ret++;
				i = 0;
				memset(code, 0, 5);
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					if((*ret == 0x0d)||(*ret <= 0x0a)||(i>=5)){
						break;
					}
					else
						code[i++] = *ret;
					ret++;
				}
				// #ifdef _debug_
				// 				debugstring("code:");
				// 				debugstring(code);
				// #endif
				if(i<5){
					if(strlen(code) == 1){//������
						if(code[0] == '1'){
							gGPRS_data_style.ucReadStyle = GPRS_SISR;
//							gGPRS_data_style.uiNum = 1500;
//							MC55i_SISREAD();//�����ж��в��ܷ�������
							goto Gprsdealcmdend;
						}
						else if(code[0] == '2'){//End of data.
							goto Gprsdealcmdend;
						}
					}
					else if(strlen(code) == 2){
						if(code[0] == '-'){
							GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
							goto Gprsdealcmdend;
						}
					}
				}
				i = str2int((unsigned char*)code, strlen(code));
// #ifdef _debug_
// 				debugstring("code i:");
// 				debugdata((unsigned char*)&i, 4, 1);
// #endif
				guiMC323_REV_LEN = i;
				goto Gprsdealcmdend;
				/*
				ret = strstr((char*)(gprsRevBuf+i+9+strlen(code)), "OK");
				
				if(ret != 0){
#ifdef _debug_
					debugstring("code i:");
					debugdata((unsigned char*)&i, 4, 1);
#endif
#ifdef _debug_
					debugstring("���������:");
					debugdata((unsigned char*)ret, 20, 1);
					debugdata(gprsRevBuf, gprsIndex, 1);
#endif
					gprsAnswer = 'O';
					gprsErrorTimes = 0;
					for(index=0;index<MAX_RCV_PACKET_NUM;index++){
						if(gPacketRcvBuf[index].ucItemStatus != 0xea)
							break;
					}
					if(index>=MAX_RCV_PACKET_NUM){
						memset(gprsRevBuf, 0, gprsIndex+1);	//�����������˰�����
						gprsIndex = 0;
						return;
					}
					
					for(t=0; t<(i+9+strlen(code)); t++){
						if((gprsRevBuf[t]==0x0d)||(gprsRevBuf[t]==0x0a))//�ҵ�0D 0A
							break;
					}
					t++;
					for(; t<(i+9+strlen(code)); t++){
						if((gprsRevBuf[t]!=0x0d)&&(gprsRevBuf[t]!=0x0a))//�ҵ�����0D  0A��
							break;
					}
					gPacketRcvBuf[index].LinkFrame.ucFrameType = 0;
					memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&i, 2);
					memcpy(gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf+t, i);
					gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_
					debugstring("����:");
					debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, i+3, 1);
#endif
					goto Gprsdealcmdend;
				}*/
			}
			return;
		}
		ret = strstr((char*)gprsRevBuf, "^SISW:");
		if(ret != 0){//��Ҫ��������
			t = 0;
			count = 0;
			while(1){
				ret++;
				if(*ret == ','){
					if(t != 0){
						t++;
						break;
					}
					t++;
					count = 0;//ȡ��󶺺ź����ֵ,��������
				}
				else if((*ret == 0x0d)||(*ret == 0x0a))
					break;
				else{
					if(t>0){
						count*=10;
						count+=(*ret-'0');
					}
				}
			}
#ifdef _debug_
			debugstring(",t=");
			debugdata((unsigned char*)&t, 1, 1);
			debugstring(",count=");
			debugdata((unsigned char*)&count, 1, 1);
#endif
			if(t == 0)
				goto Gprsdealcmdend;
			else if(t == 1){//^SISW: 0, 1  �������ݹ���׼����
				if(count == 2){//��̨�ر�
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
					goto Gprsdealcmdend;
				}
				else if(count == 1){//ģ��׼���ý�������
					gTcpIpbusy = TcpIPFree;
					goto Gprsdealcmdend;
				}
			}
			//  t == 2  ^SISW: 0,78,78 ��������
#ifdef _debug_
			debugstring("��������:");
			debugdata(gMC55iSendbuff, 14, 1);
#endif
			gprsAnswer = 'O';
			gGPRS_data_style.ucSendstyle = GPRS_SISW;
			gGPRS_data_style.uiNum = count;
// 			if(memcmp(gMC55iSendbuff, "������Ч", 8)==0){
// 				memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
// 				gprs_DATA_snd(i, gMC55iSendbuff+12);//�����ж��в��ܷ�������
// 				memset(gMC55iSendbuff, 0, 50);
// 			}
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "^SIS:");
		if(ret != 0){
//			ret += 8;
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			if(i<20){
				i = 0xffffffff;
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					else if((*ret >= '0')&&(*ret <= '9')){
						if(i == 0xffffffff)
							i = 0;
						i = i*10 + *ret-'0';
					}
					else
						break;
					ret++;
				}
			}
#ifdef _debug_gprs
			debugstring("SIS:");
			debugdata((unsigned char*)&i, 4, 1);
#endif
			if(i == 48){//^SIS: 0, 0, 48, Remote Peer has closed the connection
				set_timer0(500, 1);//���ӹر�,��ʱ����������
				if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				isNetOK[0] = 0;
				
				memset(gBusVerInfo.newbusBLKVer, 0, 2);
				gmissflag = 0;
#ifdef _debug_gprs
				debugstring("--IPCLOSE--");
				debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
				debugstring("gmissflag:");
				debugdata((unsigned char*)&gmissflag, 1, 1);
				debugstring("gBusVerInfo.newbusBLKVer:");
				debugdata(gBusVerInfo.newbusBLKVer, 2, 1);
#endif
			}
			else if(i == 21){//^SIS: 0, 0, 21, "Remote host has rejected the connection" �ܾ�����
				;
			}
			else
				IPERROR_DEAL(i);
			goto Gprsdealcmdend;
		}
		break;
	}
	ret = strstr((char*)gprsRevBuf, "+CSQ");
	if(ret != 0){
		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				gprsSingle = i;
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmderror:
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
		gprsErrorTimes++;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				IPERROR_DEAL(i);
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmdend:
	if(gprsErrorTimes > 20){//�������̫�࣬����ģ��
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//�ر�1S
		//debugstring("--1 close\r\n");
		if((GPRSLinkProcess > 16)&&(GPRSLinkProcess < 0x90)){
			if(gmissflag & MISS_GJ){//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //�������ͬʱ������,���˳�����������.
			}
		}
		if(GPRSLinkProcess == 20){//��½���˺�̨
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		}
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
GPRSdealEnd:
	if(gprsReady != 0x55){//�������µ�����
		gprsIndex = 0;
		memset(gprsRevBuf, 0, GPRSCOMMAX);
		gprsReady = 0;
	}
	return;
}

/*
	//PDP �����쳣������֪ͨ
	//PDP ����������Ӱ��ᱻ�Զ��رգ���ʱģ����ϱ�URC ֪ͨhost��Host ִ��AT+QIDEACT ��ر�ָ���������ó����µ��������ӡ�
	//+QIURC: ��pdpdeact��,<contextID>		//<contextID> ���ͣ�����ID����Χ��1-16
	//+QIURC: "closed",<connectID>   // <connectID>���ӱ��Ͽ�
*/
void EC20_pdp_down(void)
{
	char *ret1;
	
	if ((GPRSLinkProcess>9)&&(GPRSLinkProcess<0xF0))		////����PDP ��������Ҫ�жϷ����� 
	{
		ret1 = strstr((char*)gprsRevBuf, "+QIURC");
		if(ret1 != 0)
		{
			if (strstr((char*)gprsRevBuf, "pdpdeact")!=0)
			{
				isNetOK[0]=0;		//socket���ӶϿ�
				gprsErrorTimes = 0;
				GPRSLinkProcess= 2;	//��ѯSIM��״̬
				set_timer0(100, 1);
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				
				
			}
		}
	}

}

void gprsCmdDeal_ALL4g(void)
{
	char *ret,*rp;
	unsigned int i;
// 
// 	debugstring("4GReV:");
// 	debugstring((char*)gprsRevBuf);
// 	debugstring("\r\n");
// 	debugdata(gprsRevBuf, gprsIndex, 1);

	if(gprsIndex < 4)
		goto GPRSdealEnd;
 
//	debugstring((char*)gprsRevBuf);

	if ((GPRSLinkProcess<=11)||((GPRSLinkProcess>=0xF0)))		//ģ��ػ��ɹ����Ա�"POWERED DOWN"
	{
		if(strstr((char*)gprsRevBuf, "POWERED DOWN") != 0)//ģ��ػ�,ʹ��PWRKEY�ܽŹػ�Ҳ�����
		{
			GPRSLinkProcess = GPRS_LINK_CMD;
			gprsErrorTimes = 0;
			isNetOK[0] = 0;
			set_timer0(10000, 1);//��ʱ10s
 			debugstring("EC20 SHUTDOWN\r\n");

			gprsErrorTimes = 0;
			goto GPRSdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "RDY") != 0){
			isNetOK[0] = 0;
#ifdef _debug_gprs
			debugstring("EC20 ready\r\n");
#endif
			gprsErrorTimes = 0;
			GPRS_MC55i_open();      //����POWN
			GPRSLinkProcess = GPRS_AT_WAIT;
			set_timer0(500, 1);	//��������Ա�һЩ��Ϣ
			goto GPRSdealEnd;
		}
	}


	if(GPRSLinkProcess == GPRS_AT_RESET){//EC20����ر���
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//�رճɹ� 
	//		GPRSLinkProcess = GPRS_LINK_CMD;
			set_timer0(15000, 1);//��ʱ15��ȴ�POWERED DOWN
			isNetOK[0] = 0;
			gprsErrorTimes = 0;
 			gprsAnswer = 'O';
			goto GPRSdealEnd;
		}

		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//�رղ��ɹ� 
			GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			set_timer0(10, 1);//��Դ���������¿�ʼ����
			isNetOK[0] = 0;
			gprsErrorTimes = 0;
 			gprsAnswer = 'O';
			goto GPRSdealEnd;
		}

		if(strstr((char*)gprsRevBuf, "POWERED DOWN") != 0)//ģ��ػ�
		{
			GPRSLinkProcess = GPRS_LINK_CMD;		
			gprsErrorTimes = 0;
			isNetOK[0] = 0;
			set_timer0(10000, 1);//��ʱ10s
			debugstring("EC20 SHUTDOWN\r\n");
			
			gprsErrorTimes = 0;
			goto GPRSdealEnd;
			
		}

	}


	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 4:
	case 6:		//����PSҵ���״̬�ı���Ա�
	case 7:
	case 10:		//����PDP ������ز���
	case 11:		//����PDP ����1, ͨ���ƶ�����ȡIP
	case 19:	//�ر�PDP ����1,û�мӽ���
	case GPRS_TCPCLOSE_CMD://TCPIP�ر��У��ȴ�OK����
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			
		   if (GPRSLinkProcess==GPRS_TCPCLOSE_CMD)
			{	
				isNetOK[0]=0;		//socket���ӶϿ�
				gprsErrorTimes = 0;
				GPRSLinkProcess = 20;
			}
		    else if (GPRSLinkProcess == 19)
		    {
				isNetOK[0]=0;		
				gprsErrorTimes = 0;
				GPRSLinkProcess = 2;	//���ز�ѯSIM����Ϣ
		    }
			else
				GPRSLinkProcess++;

			set_timer0(100, 1);
			gprsErrorTimes = 0;
 			gprsAnswer = 'O';

			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
 			gprsErrorTimes++;
			if(GPRSLinkProcess == 11)
				GPRSLinkProcess = 19;	//�ر�PDP
#ifdef _debug_gprs
			debugstring("$#:");
			debugstring((char*)gprsRevBuf);
#endif
			set_timer0(1000, 1);
			goto Gprsdealcmdend;//Gprsdealcmdend;
		}

		break;

	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(200, 1);				//������ʱ��Ŀ���Ǳ����һ�β�ѯSIM��IMSI���� (AT+CIMI )����ERROR
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
 			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
	//	break;

	case 3:	//��ѯSIM��IMSI����  //AT+CIMI    460036261069293   OK		204046620134413
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			
			ugISMI_type = 0;
			ret = strstr((char*)gprsRevBuf, "460");
			if (ret != 0)	////460 �й�����
			{
				ret += 3;
				i = (*ret-'0')*10 + (*(ret+1)-'0');
				//debugdata((unsigned char*)&i,4,1);
				switch (i)
				{
				case 0:		//�ƶ���46000/46002/46004/46007
				case 2:
				case 4:
				case 7:
					ugISMI_type = ChinaMobile;	
					break;
				case 1:		//��ͨ��46001/46006/46009
				case 6:
				case 9:
					ugISMI_type = ChinaUnicom;	
					break;
				case 3:		//���ſ�46003/46005/46011
				case 5:
				case 11:
					ugISMI_type = ChinaTelecom;
					GPRSLinkProcess = 5;  //pfquan 2017.09.08 ���ŵ�5ģҪ����4��5����
					break;
				default:
					ugISMI_type = 0;
					break;
				}
					
			}

					
			if(ugISMI_type != 0){
				gprsErrorTimes = 0;
				GPRSLinkProcess++;
				set_timer0(0, 1);
			}

			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
		
	//	break;
		
	case 5:	//��ѯCSҵ������ע�ᣩ��״̬   //+CREG: 1,1   OK
	case 8:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
 			gprsAnswer = 'O';
			//ret = strstr((char*)gprsRevBuf, ",");
			ret = strchr((char*)gprsRevBuf, ',');
	
			if(ret != 0){
				if(*(ret+1) == '1')		
				{
 					gprsErrorTimes = 0;
					GPRSLinkProcess++;
					set_timer0(10, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;	//OK����+CREG:1,1 \x0d\x0a �����0D0A2B435245473A20312C310D0A0D0A4F4B0D0AOK

	
//		break;

	case 12:	//+QIACT: 1,1,1,"100.101.73.205"       OK   
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			//ret = strstr((char*)gprsRevBuf, "QIACT: ");
			ret = strchr((char*)gprsRevBuf, ',');
			//debugstring(ret);
			if(ret != 0){
				if(*(ret+1) == '1')		
				{
			//		debugstring(ret+1);	//��ȡIP��ַ�ɹ�
					gprsErrorTimes = 0;
					GPRSLinkProcess=20;		//���÷�����IP��ַ
					set_timer0(0, 1);
				}
				else if (*(ret+1) == '0')
				{
					gprsErrorTimes = 0;
					GPRSLinkProcess=19;		//������ָ����PDP
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
			
		}
		else{
			EC20_pdp_down();		//PDP �����쳣������֪ͨ
			return;
		}
		
		
	//	break;

	//+QISTATE: <connectID>,<service_type>,<IP_address>,<remote_port>,<local_port>,<socket_state>,<contextID>,<serverID>,<access_mode>,<AT_port>
	// 		<socket_state> ���ͣ�socket ����״̬
	// 			0 ����ʼ���� Socket ��δ������
	// 			1 �������С� �ͻ����������ӣ������������ڽ�������
	// 			2 �����ӳɹ��� �ͻ���/Incomming �����Ѿ������ɹ�
	// 			3 ������״̬�� ���������ڼ���
	// 			4 ���ȴ��رա� Զ�������ر�TCP ���ӣ���ʱģ��ᴦ�ڵȴ��ر�״̬����ҪMCU ִ��QICLOSE ���ͷ�socket ��Դ

	case GPRS_SOCKET_CHECK_CMD:	//��ѯSOCKET1��״̬,��ʱû��ʹ�á�
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			ret = my_strstr((char*)gprsRevBuf, ",", 5);
			if(ret != 0){
				switch (*(ret+1))
				{
				case '0':			    //0 ����ʼ���� Socket ��δ������
					isNetOK[0]=0;		//socket���ӶϿ�
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					break;
				case '1':				//1 �������С� �ͻ�����������
					isNetOK[0]=0;		
					gprsErrorTimes = 0;
					goto Gprsdealcmdend;
//					break;
				case '2':
					isNetOK[0]=3;		//socket���Ӵ���
					GPRSLinkProcess=22;	//����ͨ�Ų���
					break;
				case '4':
					isNetOK[0]=0;		//socket���ӶϿ�
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					break;
				default:
					isNetOK[0]=0;		//socket���ӶϿ�
					gprsErrorTimes = 0;
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					goto Gprsdealcmdend;
//					break;
				}
				
				gprsErrorTimes = 0;
				set_timer0(100, 1);
			}
		
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			EC20_pdp_down();		//PDP �����쳣������֪ͨ
			return;
		}

	//	break;

	case 20:	//+QIOPEN: <connectID>,<err>	(�ӽ����ɹ�ʱ��<err> Ϊ0)		//+QIOPEN: 0,0		//+QIOPEN: 0,566
				/*
				  AT+QIOPEN=1,0,"TCP","113.88.229.174",8082,0,0
				  OK
				  +QIOPEN: 0,0
		        */

		if (strstr((char*)gprsRevBuf, "+QIOPEN")!=0)		////+QIOPEN: 0,0  TCP socket0���ӳɹ�
		{
			gprsAnswer = 'O';

			ret = strchr((char*)gprsRevBuf, ',');
			if(ret != 0){
				if(*(ret+1) == '0')		
				{
					isNetOK[0]=3;			//socket���ӳɹ�
					gprsErrorTimes = 0;
					GPRSLinkProcess++;		//���봫��״̬
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			isNetOK[0]=0;		//socket����ʧ��
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			EC20_pdp_down();		//PDP �����쳣������֪ͨ
			return;
		}
	//	break;

	//�����͵�Buffer ������host�Գ��Է������ݣ��򷵻ء�SEND FAIL��������host �ӳ�һ��ʱ���ٷ�������, �����������ʱ�����򷵻ء�ERROR������ʱ����ر�socket��
// 	case 22:	//���ݷ���ȷ�Ϸ��أ����ݷ��ͳɹ�����"SEND OK�������򷵻ء�SEND FAIL�� ��"ERROR"
// 	case 23:	//��������
// 	case 42:	//�жϡ�>��	
// 	case 24:    //��������
// 		
		
	default:
		//���ݽ���	���ָ�������л������ݣ����أ�+QIRD: <read_actual_length><CR><LF><data>	 OK
// 		if((strstr((char*)gprsRevBuf, "+QIRD")!=0)&&(strstr((char*)gprsRevBuf, "OK")==0))
// 			return;

 		ret = strstr((char*)gprsRevBuf, "OK");
 		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, "+QIRD");
			if(ret != 0)
			{
				ret += 7;
				if (*ret != '0')
				{
					i=0;
					
					while((*ret >= '0')&&(*ret <= '9'))
					{
						if (i>1500){		//�쳣��
							i=0;
							break;		
						}
						
						i = i*10 + (*ret-'0');
						ret++;
					}

					gGPRS_data_style.ucReadStyle = GPRS_SISR;		//���ʱ���������
					guiMC323_REV_LEN = i;		//���յ������ݳ��ȣ������ݸ�ʽ�йأ������
					
					goto Gprsdealcmdend;
					
				}
				gprsErrorTimes = 0;
				goto GPRSdealEnd;
			}
			else if(strstr((char*)gprsRevBuf, "SEND") != 0){		//�������ݱ�ģ���������������������ͳɹ�
				set_timer0(20000, 1);	//20���ٴη���,��������
				gprsErrorTimes = 0;
				gprsAnswer = 'O';
				goto GPRSdealEnd;
			}
		}
		else if (strstr((char*)gprsRevBuf, "+QIURC")!=0)
		{
			ret = strstr((char*)gprsRevBuf, "recv");
			if (ret!=0)//+QIURC: "recv",0,20
			{
//				ret = strstr((char*)gprsRevBuf, "recv");
				i = 0;
				rp = (char *)gprsRevBuf;
				while((ret-rp) < gprsIndex){
					if(*ret == ','){
						i++;
						ret++;
						if(i >= 2) break;//�ҵ��ڶ���,��
					}
					else
						ret++;
				}
				if(i < 2){
					goto Gprsdealcmdend;	//û�еڶ���,�ţ��˳�.
				}

				i=0;
				
				while((*ret >= '0')&&(*ret <= '9'))
				{
					if (i>1500){		//�쳣��
						i=0;
						break;		
					}
					
					i = i*10 + (*ret-'0');
					ret++;
				}
				
				gGPRS_data_style.ucReadStyle = GPRS_SISR;		//���ʱ���������
				guiMC323_REV_LEN = i;							//���յ������ݳ��ȣ������ݸ�ʽ�йأ������
//				printf("RL:%d\r\n", i);
				goto Gprsdealcmdend;
					
			}
			else if (strstr((char*)gprsRevBuf, "closed")!=0)
			{
				isNetOK[0]=0;		//socket���ӶϿ�
				GPRSLinkProcess= GPRS_NEED_CLOSEIP;	//�ر�SOCKET������������
				set_timer0(100, 1);
			}
			else if (strstr((char*)gprsRevBuf, "pdpdeact")!=0)
			{
				isNetOK[0]=0;		//socket���ӶϿ�
				GPRSLinkProcess= 2;	//��ѯSIM��״̬
				set_timer0(100, 1);			
			}
			
			gprsErrorTimes = 0;
			goto GPRSdealEnd;
			
		}
// 		else if(strstr((char*)gprsRevBuf, "SEND OK") != 0){		//�������ݱ�ģ���������������������ͳɹ�
// 				set_timer0(20000, 1);	//20���ٴη���,��������
// 				gprsErrorTimes = 0;
// 				gprsAnswer = 'O';
// 				goto GPRSdealEnd;
// 			
// 		}
		else if(strstr((char*)gprsRevBuf, "SEND FAIL") != 0){		//ģ�鷢��BUFF����������ʱһ��ʱ����ٷ�
			gprsErrorTimes = 0;
			set_timer0(1000, 1);	//��ʱ1000ms���ȴ�������� �� ����AT+QISEND=<connectID>,0��ѯ�������
			goto GPRSdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes = 0;
			GPRSLinkProcess=GPRS_NEED_CLOSEIP;	//�ر�SOCKET0
			set_timer0(100, 1);
			goto GPRSdealEnd;
		}
		else{
			//EC20_pdp_down();		//PDP �����쳣������֪ͨ ,���������ж�
			gprsReady = 0;
//			return;
		}

// 		break;

		break;
	}  //switch����


	ret = strstr((char*)gprsRevBuf, "+CSQ");
	if(ret != 0){
 		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				gprsSingle = i;
			}
		}
		goto Gprsdealcmdend;
	}
	
//Gprsdealcmderror:
//	ret = strstr((char*)gprsRevBuf, "ERROR");
//	if(ret != 0){
//		gprsErrorTimes++;
//		ret = strstr((char*)gprsRevBuf, ":");
//		if(ret != 0){
//			ret++;
//			i = 0xff;
//			while(1){
//				if(*ret == ' '){
//					ret++;
//					continue;
//				}
//				if((*ret >= '0')&&(*ret <= '9')){
//					if(i == 0xff)
//						i = 0;
//					i = i*10 + *ret-'0';
//				}
//				else
//					break;
//				ret++;
//			}
//			if(i != 0xff){
//				IPERROR_DEAL(i);
//			}
//		}
//		goto Gprsdealcmdend;
//	}
Gprsdealcmdend:
	if(gprsErrorTimes > 20){//�������̫�࣬����ģ��
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//�ر�1S
		//debugstring("--1 close\r\n");
		if((GPRSLinkProcess > 16)&&(GPRSLinkProcess < 0x90)){
#ifdef _debug_gprs
			debugstring("�������>16----\r\n");
#endif
			if(gmissflag & MISS_GJ){//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //�������ͬʱ������,���˳�����������.
			}
		}
		if(GPRSLinkProcess >= 20)
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//
		else{
			GPRSLinkProcess = 0;
		}
	}
GPRSdealEnd:
	if(gprsReady != 0x55){//�������µ�����

// #ifdef _debug_gprs
// 		debugstring("WW:\r\n");
// #endif
		gprsIndex = 0;
		memset(gprsRevBuf, 0, GPRSCOMMAX);
		gprsReady = 0;
	}
	return;
}



//GTM900C
void gprsCmdDeal(void)
{
	char *ret;
	unsigned int i;
	unsigned char index;
	
	if(gprsReady != 0xaa)
		return;

	if(gprsIndex < 4)
		goto GPRSdealEnd;

	if(strstr((char*)gprsRevBuf, "AT-Command Interpreter") != 0)
	{
		gprsErrorTimes = 0;
		GPRSLinkProcess = GPRS_AT_WAIT;	//GPRSģ��������
		set_timer0(40000, 1);//���ӹر�,��ʱ����������
		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--AT-Command Interpreter ready\r\n");
#endif
		goto Gprsdealcmdend;
	}

	if(GPRSLinkProcess == GPRS_AT_WAIT)
		goto Gprsdealcmdend;

	if(strstr((char*)gprsRevBuf, "+CGREG") != 0){
		ret = strstr((char*)&gprsRevBuf, ",");
		if(ret != 0){
			ret++;
			if(*ret == '0'){
//				debugstring("--0 close\r\n");
				gprsSingle = 0;
				//set_timer0(1000, 1);//�ر�1S
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
				goto Gprsdealcmdend;
			}
			else if(*ret == '2'){
				gprsSingle = 1;
				gprsReady = 0;
				return;//���������磬����.
			}
		}
	}

	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I�ر���
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//�رճɹ� 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//�رղ��ɹ� 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//��ʱ10����������ģ��
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
	}

	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 17:
	case 19:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case GPRS_TCPCLOSE_CMD://TCPIP�ر��У��ȴ�OK����
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess > 20)
				GPRSLinkProcess = 20;// ��������
			set_timer0(0, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		goto Gprsdealcmdend;
		//goto Gprsdealcmderror;
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			for(i=0; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 18:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, "%ETCPIP");
			if(ret == 0)
				break;
			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 20:
		ret = strstr((char*)gprsRevBuf, "CONNECT");
		if(ret != 0){
			if((gmissflag & 0xf0)==0x10){//��½�ƶ�IF1
				isNetOK[0] = 1;
			}
			else if((gmissflag & 0xf0)==0x20){//��½�ƶ�IF2
				isNetOK[0] = 2;
			}
			else{//��½������������
				isNetOK[0] = 3;
			}
			GPRSLinkProcess++;
			set_timer0(0, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			goto Gprsdealcmderror;
		}
		break;
	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 5:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			ret = strstr((char*)gprsRevBuf, "1,");
			if(ret != 0){
				if(*(ret+2) != '2'){
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else {
			gprsReady = 0;
			return;
		}
//		break;
	default:
	ret = strstr((char*)gprsRevBuf, "%IPDATA:");//%IPDATA:1,3,"676637"
	if(ret != 0){
		gprsAnswer = 'O';
		gprsErrorTimes = 0;
		gprsSNDerro = 0;
		for(index=0;index<MAX_RCV_PACKET_NUM;index++){
			if(gPacketRcvBuf[index].ucItemStatus != 0xea)
				break;
		}
		if(index>=MAX_RCV_PACKET_NUM){
			if(gprsReady != 0x55){//�������µ�����
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);	//�����������˰�����
				gprsReady = 0;
			}
			return;
		}
// #ifdef _debug_
// 		debugstring("����:");
// 		debugdata((unsigned char*)&index, 1, 1);
// #endif
		i = PeckData(&gPacketRcvBuf[index].LinkFrame, ret);
		gPacketRcvBuf[index].ucItemStatus = 0xea;
// #ifdef _debug_
// 		debugstring("����:");
// //		debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, i+3, 1);
// #endif
		goto Gprsdealcmdend;
	}
	
	ret = strstr((char*)gprsRevBuf, "OK");//����
	if(ret != 0){
		if(strstr((char*)gprsRevBuf, "IPSEND") != 0){//IPSENDX
			//gprsAnswer = 'I';
			gprsAnswer = 'O';
			if(strstr((char*)gprsRevBuf, "%IPSD") != 0){
				gprsSendDlyflag = 'T';	//˵������ʱ���͵�����
			}
			else
				gprsSendDlyflag = 'N';
		}
		else
			gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		goto Gprsdealcmdend;
	}
	break;
	}

	if(strstr((char*)gprsRevBuf, "IPSEND") != 0){//����Ƿ��������У�ϣ���ڣϣ˺��ж�
		if(strstr((char*)gprsRevBuf, "%IPSEND:1") != 0){//
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				ret++;
				i = 0xffffffff;
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					if((*ret >= '0')&&(*ret <= '9')){
						if(i == 0xffffffff)
							i = 0;
						i = i*10 + *ret-'0';
					}
					else
						break;
					ret++;
				}
// #ifdef _debug_gprs
// 				debugstring("����IPSS:i:");
// 				debugdata((unsigned char*)&i,1,1);
// #endif
				if(i <=11){//��ǰ�����ݷ��Ͳ��ɹ�
					memset(gBusVerInfo.newbusBLKVer, 0, 2);//�Ͽ����Ӻ�Ҫ���µ�½
					gmissflag = 0;
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
					goto Gprsdealcmdend;
				}
			}
		}
		gprsReady = 0;
		return;
	}
	if(strstr((char*)gprsRevBuf, "%IPSD") != 0){//����Ƿ��������У�ϣ���ڣϣ˺��ж�
		gprsReady = 0;
		return;
	}

	ret = strstr((char*)gprsRevBuf, "%IPSP");//��ѯ��ʱ����״̬����
	if(ret != 0){
		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xffffffff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xffffffff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if((i >= 500)&&(i <= 60000)){//������ʱ����Ϊ500-60000 ���� 0Ϊ�ر���ʱ
				gprsSendDlyflag = 'T';
			}
			else if(i == 0){
				gprsSendDlyflag = 'N';
			}
		}
		goto Gprsdealcmdend;
	}

	ret = strstr((char*)gprsRevBuf, "+CSQ");
	if(ret != 0){
		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				gprsSingle = i;
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmderror:
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
		gprsErrorTimes++;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				IPERROR_DEAL(i);
			}
		}
		goto Gprsdealcmdend;
	}

	ret = strstr((char*)gprsRevBuf, "%IPCLOSE");
	if(ret != 0){
		set_timer0(500, 1);//���ӹر�,��ʱ����������
//		gprsErrorTimes = 0;
		for(i=0;i<(strlen((char*)gprsRevBuf));i++)
		{
			if(gprsRevBuf[i] == '5'){
				GPRSLinkProcess = 0;
				break;
			}
			else if((gprsRevBuf[i] == 0x0a)||(gprsRevBuf[i] == 0x0d))
				break;
		}
		if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
			GPRSLinkProcess = 20;
		isNetOK[0] = 0;

		memset(gBusVerInfo.newbusBLKVer, 0, 2);
		gmissflag = 0;
// #ifdef _debug_
// 		debugstring("--IPCLOSE--");
// 		debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
// #endif
		goto Gprsdealcmdend;
	}

	if(strstr((char*)gprsRevBuf, "AT-Command Interpreter") != 0){
		gprsErrorTimes = 0;
		GPRSLinkProcess = GPRS_AT_WAIT;	//GPRSģ��������
		set_timer0(40000, 1);//���ӹر�,��ʱ����������
		isNetOK[0] = 0;
// #ifdef _debug_
// 		debugstring("retart--AT-Command In ready\r\n");
// #endif
		goto Gprsdealcmdend;
	}

Gprsdealcmdend:
	if(gprsErrorTimes > 20){//�������̫�࣬����ģ��
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//�ر�1S
		//debugstring("--1 close\r\n");
		if(GPRSLinkProcess > 16){
			if(gmissflag & MISS_GJ){//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //�������ͬʱ������,���˳�����������.
			}
		}
		if(GPRSLinkProcess == 20){//��½���˺�̨
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		}
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
GPRSdealEnd:
	if(gprsReady != 0x55){//�������µ�����
		gprsIndex = 0;
		memset(gprsRevBuf, 0, 20);
		gprsReady = 0;
	}
	return;
}
	
void GPRS_handler(unsigned char value)
{
	unsigned char gGprsMode;
 	char *ret=NULL;
//	char *start_p = NULL;
	unsigned int i;

	static unsigned int count_comma=0;		//���ڼ�¼���^IPRECV����ġ��������ֵĴ���  �ڶ���ʱ����ȥ�ҵ������� Ȼ���������

	unsigned int index;

	
	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;

	gprsRevBuf[gprsIndex] = value;

#ifdef _debug_gprs
// 	if(CDMA_REV_LEN>0)
// 	{
// 		//debugstring("CDMA_REV_LEN:");
// 		debugdata((unsigned char *)&value, 1, 0);
// 	}
// 	else if(guiMC323_REV_LEN>0){
// 		//debugstring("guiMC323_REV_LEN:");
// 		debugdata((unsigned char *)&value, 1, 0);
// 	}
// 	else
 	com_snd(COM1, 1, (unsigned char*)&value);
	//debugdataCOM(COM4,(unsigned char*)&value,1,0);
//	debugdata(&value,1,0);
#endif



	if(gGprsMode == Gmode_MC323){
		if(gmissflag == MISS_G_MSG1){
			if(value == '>'){
				sendmsgBody();
				set_timer0(40000, 1);
				CDMA_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return;
			}
		}
		if((CDMA_REV_LEN>0)&&((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0))){
			gprsIndex++;
			if(gprsIndex >= GPRSCOMMAX){
				gprsIndex = 0;
				CDMA_REV_LEN = 0;//������̫��������
				return;
			}
			if(gprsIndex >= (CDMA_REV_LEN+16)){//^IPDATA:1,10,1234567890 ���ݼ���ǰ����ֽڣ��ټ���0d 0a OK���ٶ���14�ֽ�
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(index=0;index<MAX_RCV_PACKET_NUM;index++){
					if(gPacketRcvBuf[index].ucItemStatus != 0xea)
						break;
				}
				if(index>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//�����������˰�����
					return;
				}
				for(i=0; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				for(; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				if(i >= 16){
#ifdef _debug_
					debugstring("...");
#endif
					CDMA_REV_LEN = 0;
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);
					return;
				}
				gPacketRcvBuf[index].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&CDMA_REV_LEN, 2);
				memcpy(&gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf+i, CDMA_REV_LEN);
				gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_gprs
				debugstring("����:");
				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, CDMA_REV_LEN, 1);
#endif
				CDMA_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return;
			}
			return;
		}
	}
	else if(gGprsMode == Gmode_MG323){
		if((guiMC323_REV_LEN>0)&&((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0))){//��������Ҫ����
			gprsIndex++;
			if(gprsIndex >= GPRSCOMMAX){
				gGPRS_data_style.ucReadStyle = 0;
				gprsIndex = 0;
				guiMC323_REV_LEN = 0;//������̫��������
#ifdef _debug_gprs
				debugstring("too long\r\n");
#endif

				gprsIndex = FRAME_SIZE;
				guiMC323_REV_LEN = FRAME_SIZE;	//pfquan �����ˣ����ù�ȥ����
			}
			if(gprsIndex >= guiMC323_REV_LEN){
				gGPRS_data_style.ucReadStyle = 0;
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(index=0;index<MAX_RCV_PACKET_NUM;index++){
					if(gPacketRcvBuf[index].ucItemStatus != 0xea)
						break;
				}
				if(index>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//�����������˰�����
#ifdef _debug_gprs
					debugstring("pack full return\r\n");
#endif
					return;
				}
				gPacketRcvBuf[index].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&guiMC323_REV_LEN, 2);
				memcpy(gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf, guiMC323_REV_LEN);
				gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_gprs
// 				debugstring("MG323����:");
// 				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, guiMC323_REV_LEN+3, 1);
#endif
				guiMC323_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return;
			}
			return;
		}
	}
	else if(gGprsMode == Gmode_CDMA2000){
		if((guiMC323_REV_LEN>0)&&  ((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0)))
		{					//��������Ҫ����
			gprsIndex++;

//			debugstring("E:\r\n");

			if(gprsIndex >= GPRSCOMMAX){
#ifdef _debug_gprs
				debugstring("too long\r\n");
#endif				
				guiMC323_REV_LEN = GPRSCOMMAX;
		//		count = 0;		//Ϊ���ٽ�ȥ��������
			
			}

			if(gprsIndex >= guiMC323_REV_LEN+2)			//\r\b Ҳ����
			{


#ifdef _debug_gprs999
				debugstring("\r\n--over---\r\n");						
				MSG_LOG("gprsIndex:%d\r\n",gprsIndex);
				debugstring("gprsRevBuf:");
				debugdata(gprsRevBuf,gprsIndex,1);
#endif
				gTcpIpbusy = TcpIPFree;
				gTcpIpbusydly = 0;			//����
				gErrorFlag &= (~ERROR_BUS_CONNECT);// ��������ʶ �ܽ�����˵�����綼����
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(i=0;i<MAX_RCV_PACKET_NUM;i++){
					if(gPacketRcvBuf[i].ucItemStatus != 0xea)
						break;
				}
				if(i>=MAX_RCV_PACKET_NUM){
					memset(gprsRevBuf, 0, gprsIndex);	//�����������˰�����
					gprsIndex = 0;
#ifdef _debug_gprs
					debugstring("full !!!\r\n");
#endif
					return;
				}
		
			//^IPRECV:1,113.89.126.181,8082,9,564879!@#
				gPacketRcvBuf[i].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[i].LinkFrame.uiFrameLen, (unsigned char*)&guiMC323_REV_LEN, 2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf, gprsRevBuf, guiMC323_REV_LEN);
				//Ascii2BCD((unsigned char *)ret,gPacketRcvBuf[i].LinkFrame.FrameBuf,guiMC323_REV_LEN*2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf,gprsRevBuf,guiMC323_REV_LEN);
				gPacketRcvBuf[i].ucItemStatus = 0xea;
#ifdef _debug_gprs
// 				debugstring("dat:");
//	 			debugdata(gPacketRcvBuf[i].LinkFrame.FrameBuf,10,1);
#endif
				gGPRS_data_style.ucReadStyle = 0;
				guiMC323_REV_LEN = 0;
				memset(gprsRevBuf, 0, GPRSCOMMAX);
				gprsIndex = 0;
				return;
			}
	
			return;
		}
	//ģ���յ����ݺ����		//^IPSEND:1, 18
	//ASCii ������ܾ������ַ�0x15 NAK(negative)
		if(((value == '>')/*||(value == 0x15)*/)&&(gprsRevBuf[gprsIndex-1]==0x0a)&&(gprsRevBuf[gprsIndex-2]==0x0d)){		//\r\v>   ������յ����ݻ���

			gprsIndex++;
			gprsRevBuf[gprsIndex+1] = 0;//�ڽ��յ����ݺ���ϸ�������

#ifdef _debug_gprs
			debugstring("#>,\r\n");
#endif
			gGPRS_data_style.ucSendstyle  = GPRS_SISW;		//ȥ�������ݰ�
		
			gprsIndex -=3;

			memset(gprsRevBuf+gprsIndex,0,3);		//ֻ����3������Ч��
// 				memset(gprsRevBuf, 0, gprsIndex);
// 				gprsIndex = 0;

// 				gprsIndex = 0;
// 				memset(gprsRevBuf, 0, GPRSCOMMAX);

// 				gprsReady = GPRS_DATA_OK;//����׼����,��Ҫ����
// 				gprsCmdDeal_CDMA_3G();
// 				gprsReady = GPRS_GOON;//�������,��0
			return ;
		}
		
	//���ݽ���		//^IPRECV:1,113.89.126.181,8082,9,564879!@#
	//				  ^IPRECV:1,113.118.100.92,8811,27,
// 	if (  ((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0)))
// 	{
		
		ret = NULL;
		ret = strstr((const char*)gprsRevBuf, "RECV");
		if(ret != 0)
		{
//			start_p = ret; 
			gGPRS_data_style.ucReadStyle = GPRS_SISR;		//���ʱ���������

// 			debugstring("ret");
// 			debugstring(ret);
// 			debugstring("VV\r\n");

 			ret = NULL;

			if(value==',')count_comma++;

			if (count_comma <4)
			{
				gprsIndex++;
				return;
			}

			ret = NULL;
			ret = my_strstr((const char*)gprsRevBuf,",",3);			//�ҵ�ǰ��ĳ���ǰ��ģ�
			if(ret != 0)
			{	
// 				debugstring("eee:");
// 				debugdata(gprsRevBuf,gprsIndex,1);
// 				debugdata(ret,5,1);
// 				debugstring("rrr\r\n");
				i=0;
				ret++;
				while((*ret >= '0')&&(*ret <= '9'))
				{
					if (i>1024){		//�쳣��
						i=0;
						break;		
					}
					
					i = i*10 + (*ret-'0');
					ret++;
				}
				
				count_comma = 0;
				guiMC323_REV_LEN = i;
#ifdef _debug_gprs
				//MSG_LOG("len:%d\r\n",guiMC323_REV_LEN);
#endif
// 											
				gTcpIpbusydly = 3;			//3S��ʱ

				memset(gprsRevBuf, 0, GPRSCOMMAX);
				gprsIndex = 0;
// 
// 
				return;
// 
 			}	
 			
		}
//		}
	}
	else if(gGprsMode == Gmode_ALL4G){
		if((guiMC323_REV_LEN>0)&&  ((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0)))
		{					//��������Ҫ����
			gprsIndex++;

//			debugstring("E:\r\n");

			if(gprsIndex >= GPRSCOMMAX){
#ifdef _debug_gprs
				debugstring("too long\r\n");
#endif				
				guiMC323_REV_LEN = GPRSCOMMAX;
//				count = 0;		//Ϊ���ٽ�ȥ��������
			}

			if(gprsIndex >= guiMC323_REV_LEN)//+2)			//\r\b Ҳ����
			{
#ifdef _debug_gprs999
				debugstring("\r\n--over---\r\n");						
				MSG_LOG("gprsIndex:%d\r\n",gprsIndex);
				debugstring("gprsRevBuf:");
				debugdata(gprsRevBuf,gprsIndex,1);
#endif
// 				gTcpIpbusy = TcpIPFree;
// 				gTcpIpbusydly = 0;			//���ܿ��У������ڵȷ������ݵ�>�ţ�һ���������
				gErrorFlag &= (~ERROR_BUS_CONNECT);// ��������ʶ �ܽ�����˵�����綼����
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(i=0;i<MAX_RCV_PACKET_NUM;i++){
					if(gPacketRcvBuf[i].ucItemStatus != 0xea)
						break;
				}
				if(i>=MAX_RCV_PACKET_NUM){
					memset(gprsRevBuf, 0, gprsIndex);	//�����������˰�����
					gprsIndex = 0;
#ifdef _debug_gprs
					debugstring("full !!!\r\n");
#endif
					return;
				}
		
			//^IPRECV:1,113.89.126.181,8082,9,564879!@#
				gPacketRcvBuf[i].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[i].LinkFrame.uiFrameLen, (unsigned char*)&guiMC323_REV_LEN, 2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf, gprsRevBuf, guiMC323_REV_LEN);
				//Ascii2BCD((unsigned char *)ret,gPacketRcvBuf[i].LinkFrame.FrameBuf,guiMC323_REV_LEN*2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf,gprsRevBuf,guiMC323_REV_LEN);
				gPacketRcvBuf[i].ucItemStatus = 0xea;
// #ifdef _debug_gprs
// 				debugstring("dat:");
// 	 			debugdata(gPacketRcvBuf[i].LinkFrame.FrameBuf,10,1);
// #endif
				gGPRS_data_style.ucReadStyle = 0;
				guiMC323_REV_LEN = 0;
				memset(gprsRevBuf, 0, GPRSCOMMAX);
				gprsIndex = 0;
				return;
			}
	
			return;
		}
	//ģ���յ����ݺ����		//^IPSEND:1, 18
	//ASCii ������ܾ������ַ�0x15 NAK(negative)
//		if(((value == '>')/*||(value == 0x15)*/)&&(gprsRevBuf[gprsIndex-1]==0x0a)&&(gprsRevBuf[gprsIndex-2]==0x0d))
		if(value == '>')
		{		//\r\v>   ������յ����ݻ���
			gprsIndex++;
			gprsRevBuf[gprsIndex+1] = 0;//�ڽ��յ����ݺ���ϸ�������

// #ifdef _debug_gprs
// 			debugstring("4G>,\r\n");
// #endif
			gGPRS_data_style.ucSendstyle  = GPRS_SISW;		//ȥ�������ݰ�
		
			gprsIndex = 0;

			memset(gprsRevBuf+gprsIndex,0,3);		//ֻ����3������Ч��
// 				memset(gprsRevBuf, 0, gprsIndex);
// 				gprsIndex = 0;

// 				gprsIndex = 0;
// 				memset(gprsRevBuf, 0, GPRSCOMMAX);

// 				gprsReady = GPRS_DATA_OK;//����׼����,��Ҫ����
// 				gprsCmdDeal_CDMA_3G();
// 				gprsReady = GPRS_GOON;//�������,��0
			return ;
		}
	}
// 	else if(gGprsMode == Gmode_ALL4G){		
// 		if (GPRSLinkProcess==GPRS_LINK_UP)		//��������RDY���жϿ����ɹ�"0D 0A 52 44 59 0D 0A 0D 0A"		\r\nRDY\r\n\r\n
// 		{
// 			if ((value=='Y')&&(gprsIndex>2))
// 			{
// 				if ((gprsRevBuf[gprsIndex-2]=='R')&&(gprsRevBuf[gprsIndex-1]=='D'))
// 				{
// 					gprsIndex=0;
// 					isNetOK[0] = 0;
// 					//	debugstring("EC20 ready\r\n");
// 					gprsErrorTimes = 0;
// 					GPRS_MC55i_open();      //����POWN
// 					GPRSLinkProcess = GPRS_AT_WAIT;
// 					set_timer0(500, 1);	//��������Ա�һЩ��Ϣ
// //					gprsPowerState = 1;	//ģ�鼤��
// 				}
// 			}
// 			
// 			gprsIndex++;
// 			if(gprsIndex >= GPRSCOMMAX)
// 				gprsIndex = 0;
// 			return;
// 			
// 		}
// 		if ((GPRSLinkProcess==42)&&(value=='>'))
// 		{
// 			GPRSLinkProcess=23;
// 			set_timer0(0, 1);
// 			gprsIndex = 0;
// 			//	gprsErrorTimes = 0;			//�������gprsErrorTimes���쳣ʱ��'>'�����
// 			return;
// 		}
// 	}


	/******************************************************************************************************************/
	/******************************************************************************************************************/


	if((value == 0x0a)&&(gprsRevBuf[gprsIndex-1]==0x0d)){		//һ��������
		gprsIndex++;
		gprsRevBuf[gprsIndex+1] = 0;//�ڽ��յ����ݺ���ϸ�������
#ifdef _debug_gprs
// 		debugstring("dex=:");
// 		debugdata((unsigned char *)&gprsIndex,2,1);
		//	debugdata((unsigned char *)&gprsReady,1,1);
// 		debugstring("CMD:");
// 		debugstring((char*)gprsRevBuf);
#endif
		
		if(gGprsMode == Gmode_MC323){
			gprsReady = GPRS_DATA_OK;//����׼����,��Ҫ����
			gprsCmdDeal_CDMA();
		}
		else if(gGprsMode == Gmode_MG323){
			gprsReady = GPRS_DATA_OK;//����׼����,��Ҫ����
			gprsCmdDeal_MG323();
			gprsReady = GPRS_GOON;//�������,��0
		}
		else if(gGprsMode == Gmode_CDMA2000){
			//��̨���͵��������������0D0A ������Ҳ��ȥ�����ˣ�����Ҫ���ճ���������
			gprsReady = GPRS_DATA_OK;//����׼����,��Ҫ����
			gprsCmdDeal_CDMA_3G();
			gprsReady = GPRS_GOON;//�������,��0
		}
		else if(gGprsMode == Gmode_ALL4G){
			gprsReady = GPRS_DATA_OK;
			gprsCmdDeal_ALL4g();
			gprsReady = GPRS_GOON;
		}
		else{
			gprsReady = GPRS_DATA_OK;//����׼����,��Ҫ����
			gprsCmdDeal();
			gprsReady = GPRS_GOON;//�������,��0
		}

		return;
	}


	gprsIndex++;
	if(gprsIndex >= GPRSCOMMAX)
		gprsIndex = 0;
	return;
}
//��ʼ��GPRS�˿ڼ�ģ��
void GPRS_INIT(unsigned int band)
{
	unsigned char gprcom;
//	unsigned char gGprsMode;


	
//	gGprsMode = in_getGprsModeStyle();
	gprcom = getGprsCom();
#ifdef _debug_gprs
	debugstring("INI gprcom:");
	debugdata((unsigned char*)&gprcom, 1, 0);
	debugstring(":");
	debugdata((unsigned char*)&band, 4, 0);
	debugstring("|\r\n");
#endif
// 	if(gGprsMode == Gmode_900C)
// 		com_init(gprcom, 9600);
// 	else if(gGprsMode == Gmode_WIFI_TLH)
// 		com_init(gprcom, 115200);
// 	else
	com_init(gprcom, band);

//	if(gGprsMode != Gmode_OTHER){//��ӵ��Ȼ������ô˿�
		COM_it_enable(gprcom);
//	}
}

//V231 ��ʼ���ŵ�GPRS�����档
void GPRSConfig(void)
{
	if(in_getGprsModeStyle() != Gmode_Close){
		if((in_getGprsModeStyle() == Gmode_MODEM)||(in_getGprsModeStyle() == Gmode_CDMA2000)//){
			||(in_getGprsModeStyle() == Gmode_ALL4G)){
			GPRS_INIT(115200);
		}
		else{
			GPRS_INIT(9600);//�˶˿�����GPRS���ӿ�
		}
	}
}

void gprs_par_init(void)
{
	unsigned char i;

	GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//0;
	
	gprsErrorTimes=0;
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gGPRS_data_style.ucSendstyle = 0;
	gGPRS_data_style.ucReadStyle = 0;
	gGPRS_data_style.uiNum = 0;
	guiMC323_REV_LEN = 0;
	
	initTCPIPnode();
	for(i=0;i<MAX_RCV_PACKET_NUM;i++)
		gPacketRcvBuf[i].ucItemStatus = 0;
	memset(GprsCardList, 0, 16);
	gpacSNo = 0;//�����
}





//���һ�����е�֡����,���ؿ���֡����jgwei updata
unsigned char GetFrameBuf(void)
{
	unsigned char i;

	for(i=0; i<MAX_RCV_PACKET_NUM; i++) 
	{
		if(!(gPacketRcvBuf[i].ucItemStatus & MASK_RCV_BUF_USED))
		{	
			break;
		}
	}
	if(i == MAX_RCV_PACKET_NUM) 
	{								//û�п��Ի�õĻ���
		i = 0xff;
	} 
	else 
	{
		memset(gPacketRcvBuf[i].LinkFrame.FrameBuf, 0x00, sizeof(gPacketRcvBuf[i].LinkFrame.FrameBuf));	//���㽫Ҫʹ�õ�֡����
	}
	return i;
}

//�ӽ��ջ����л��һ����Ч������֡ OK
unsigned char PeekFrame(void)
{
	unsigned char i;
	
	for(i=0; i<MAX_RCV_PACKET_NUM; i++) 
	{
		if(gPacketRcvBuf[i].ucItemStatus == 0xea) 
		{				//����֡����
			break;
		}
	}
	if(i == MAX_RCV_PACKET_NUM)
	{
		return 0xff;
	}
	else 
	{
		return i;	 //������Ч�Ľ���֡����
	}
}

void mainGprs(void)
{
	unsigned char buff[20];
	unsigned char i, ret;
	unsigned char gGprsMode;
	unsigned short plen=0;//,uslen=0;
#ifdef _debug_gprs
#endif
	
	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;

	if(gGprsMode == Gmode_MG323){ //���ﴦ����������������ݲ���
		if(gGPRS_data_style.ucSendstyle == GPRS_SISW){
#ifdef _debug_gprs
// 			debugstring("����:");
// 			debugdata((unsigned char*)&gGPRS_data_style.uiNum, 4, 1);
#endif
		//		if(memcmp(gMC55iSendbuff, "������Ч", 8)==0){//�����Ƿ���Ч����Ӧ�÷�
		//			memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
			gprs_DATA_snd(gGPRS_data_style.uiNum, gMC55iSendbuff+12);//�����ж��в��ܷ�������
			memset(gMC55iSendbuff, 0, 50);
		//		}
			gGPRS_data_style.ucSendstyle = 0;
			gGPRS_data_style.uiNum = 0;
		}
		else if (gGPRS_data_style.ucReadStyle == GPRS_SISR)
		{
#ifdef _debug_gprs
			debugstring("&");
#endif
			if(gTcpIpbusy != TcpIPBusy){//���ڷ������ݣ�����ȥȡ����
#ifdef _debug_gprs
				debugstring("��ȡ����\r\n");
#endif
				MC55i_SISREAD();
				gGPRS_data_style.ucReadStyle = GPRS_SISRing;
			}
		}
		else{
			if(GPRSLinkProcess == TCPSTARTSTAT){//GPRS�Ѿ���½�����Է�����
				gprs_send_data(1,0,buff);//���������Ƿ������ݷ��͡�û�¾Ͳ�һ��
			}
		}
	}//#ifdef Gmode_MG323
	else if((gGprsMode == Gmode_CDMA2000)||(gGprsMode == Gmode_ALL4G))
	{
		if(gGPRS_data_style.ucSendstyle == GPRS_SISW){
#ifdef _debug_gprs
			debugstring("*^");
//  			debugstring("3G����:");
//  			debugdata((unsigned char*)&gGPRS_data_style.uiNum, 4, 1);
#endif
			//		if(memcmp(gMC55iSendbuff, "������Ч", 8)==0){//�����Ƿ���Ч����Ӧ�÷�
			//			memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
			
			ModeS_DataSend(gGPRS_data_style.uiNum, gMC55iSendbuff+12);//�����ж��в��ܷ�������
			memset(gMC55iSendbuff, 0, 50);
			gGPRS_data_style.uiNum = 0;	
 			gGPRS_data_style.ucSendstyle = 0;
 			gTcpIpbusy = TcpIPFree;
			gTcpIpbusydly = 0;


		}
		else if ((gGPRS_data_style.ucReadStyle != GPRS_SISR)&&(gTcpIpbusy != TcpIPBusy)){//�ڷ�����ʱ�����ٷ�
			plen = TcpIPSd_Pick(gMC55iSendbuff+12);
			if (plen != 0xffff)
			{
				gGPRS_data_style.uiNum = plen;
				
				ModeS_IPSend(plen, gGprsMode);
			}
		}
#ifdef _debug_gprs
		else{
			debugstring(".");
		}
#endif
	}

	in_deal_ring();
//	gprsCmdDeal();//����GPRSģ���������
	gprsReady = 0;//�������,��0
//	if((isNetOK[0] != 0)||(isNetOK[1] != 0)){
		i = PeekFrame();
		if(i != 0xff) 
		{
#ifdef _debug_gprs
// 			debugstring("RcvFrame: ");
// 			memcpy(&plen,gPacketRcvBuf[i].LinkFrame.uiFrameLen,2);
// 			debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf,plen,1);
#endif
//Ӧ�ò�����֡
#ifdef	_debug_gprs 
			debugstring("TaskGprsRev-------:\r\n");
			debugdata((unsigned char *)&i,1,1);
			memcpy(&plen,gPacketRcvBuf[i].LinkFrame.uiFrameLen,2);
			debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf,plen,1);
#endif
			if((gmissflag & 0xf0)==0x10){//��½�ƶ�IF1   isNetOK[0] = 1;
				gGPRS_Cmd_outtime = 0;
				in_IF18583Deal(i);
				gPacketRcvBuf[i].ucItemStatus = 0;
//				GPRSLinkProcess = 21;
			}
			else if((gmissflag & 0xf0)==0x20){//��½�ƶ�IF2 isNetOK[0] = 2;
				gGPRS_Cmd_outtime = 0;
				in_IF2TLVDeal(i);
				gPacketRcvBuf[i].ucItemStatus = 0;
			}
			else{//��½������������  ��·����
//				DataPacketProcess(i); //Ӧ�ò����ݰ����� ������̨����Ӧ�ͺ�̨������
//				GDServerProcess();
				gGPRS_Cmd_outtime = 0;

				//debugstring("xxxx\r\n");

				ret = GJDataDeal((unsigned char*)&gPacketRcvBuf[i].LinkFrame);//�������ݴ���			
				if(ret == 0){
					gPacketRcvBuf[i].ucItemStatus = 0;
				}
				else{
					gPacketRcvBuf[i].ucItemStatus = 0xea;
#ifdef _debug_gprs
					debugstring("MAIN:");
					debugdata((unsigned char*)&gPacketRcvBuf[i], 20, 1);
#endif
				}
			}
		}
//	}
}

//��ͣ��ǰ��GPRS
void stopCCgprs(unsigned short mode)
{
	in_gErrortimes(20, mode);
	gErrorFlag |= mode;
	gmissflag = 0;
}

extern void readmobilsendStyle(void);
void GprsSingleCheck(void)//���ǣУң��ź�
{
//	static char gscDly;
	char buffer[20];

	if(GPRSLinkProcess <= 6){
		return;
	}
	
// 	if(gscDly++ < 8)
// 		return;
//	gscDly = 0;
	if((gprsAnswer == 'O')||(gprsAnswer == 'I')){
		strcpy(buffer, "AT+CSQ\r\n");//���GSM����ע�����
		gprs_snd(strlen(buffer), (char*)buffer);
	}
}

void GprsIPSPcheck(void)//��������ģʽ
{
	char buff[20];

	if(GPRSLinkProcess <= 20){
		return;
	}
	strcpy((char*)buff, "AT%IPSP?\r\n");//������ģʽ
	gprs_snd(strlen((char*)buff), buff);
}


void GPRSModeBaudChage(unsigned char mOdduc)
{
	char gpscbuf[50];
	
#ifdef _debug_gprs
	debugstring("change bps---\r\n");
#endif
	if((mOdduc&0xF0) == 0x80){
		strcpy((char*)gpscbuf, "GT$POWERON:115200\r\n");  //֪ͨģ��CPU�Ķ˿ڸĳ�115200
		gprs_snd(strlen((char*)gpscbuf), gpscbuf);
	}
	else{
		if((mOdduc == Gmode_CDMA2000) || (mOdduc == Gmode_ALL4G))					//�����޸�-����
		//if(mOdduc == Gmode_CDMA2000)
			GPRS_INIT(9600);
		else
			GPRS_INIT(115200);	
	}
	delayxms(50);
	memset(gpscbuf,0,20);
	if(mOdduc == Gmode_CDMA2000)
		strcpy(gpscbuf, "AT+IPR=115200\r\n");//�����ʸĳ�115200
	else if(mOdduc == Gmode_ALL4G)
		strcpy(gpscbuf, "AT+IPR=115200;&W\r\n");		//AT+IPR=115200   + //AT&W
	else
		strcpy(gpscbuf, "AT+IPR=9600\r\n");//�����ʸĳ�9600

	gprs_snd(strlen(gpscbuf), gpscbuf);		//��ģ�鷢��ָ��
	delayxms(200);
	if(mOdduc == 0x80){
		strcpy((char*)gpscbuf, "GT$POWERON:9600\r\n");		//֪ͨģ��CPU�Ķ˿ڸĳ�9600
		gprs_snd(strlen((char*)gpscbuf), gpscbuf);
	}
	else{
		if((mOdduc == Gmode_CDMA2000) || (mOdduc == Gmode_ALL4G))
		//if(mOdduc == Gmode_CDMA2000)
			GPRS_INIT(115200);
		else
			GPRS_INIT(9600);
	}
}

extern void getDefaultAPN(char *rAPN, unsigned char mode);//mode=0 ������1 �ƶ�����;
extern void US1_It( void );
extern void TaskRecWrite(void);
//������Ӧ��
extern unsigned int Buid_GJ_DBLK(unsigned char *dat);
#ifdef _debug_
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);

#endif
void main_GpsRs(void)
{
	char gpsrsP[100];
	char APNnet[32];
	int ret;
	unsigned int t;
	unsigned char gprsMode;
	unsigned char Moddstyle;
//	static unsigned int cnt=0;

	gprsMode = in_getGprsModeStyle();
	Moddstyle = (gprsMode&0xF0);//�Ƿ����ⲿMODEM
	gprsMode &= 0x0F;

	clr_wdt();

	t = get_timer0(1);

	if(t != 0){
		goto maingprs1;	
	}

	if(gmissflag == MISS_G_MSG1){//Ҫ���Ͷ���Ϣ
		if((GPRSLinkProcess > 14) && (GPRSLinkProcess <= TCPSTARTSTAT)){
			sendmsgHead();		
			set_timer0(10000, 1);
			return;
		}
	}

	if(in_checkRingstyle() == -1)
		goto maingprs2;

#ifdef _debug_gprs
// 	debugstring("GPRSLinkProcess:");
// 	debugdata((unsigned char*)&GPRSLinkProcess, 1, 1);
#endif
	if(gprsErrorTimes > 20){//�������̫�࣬����ģ��
		gprsErrorTimes = 0;
#ifdef _debug_gprs
		debugstring("err too much\r\n");
#endif
		if((GPRSLinkProcess == 20)||(GPRSLinkProcess == 21)){
			if((gmissflag & MISS_GJ)||(gmissflag == 0)){//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
// 			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
// 				return; //�������ͬʱ������,���˳�����������.
// 			}
		}
// 		if(gmissflag & MISS_GJ){
// 			gmissflag = 0;
// 			gmissOldflag = 0;
//			return;
// 		}
//		set_timer0(1000, 1);//�ر�1S
#ifdef _debug_gprs
			debugstring("--4 close\r\n");
#endif
		if(GPRSLinkProcess == 20){
			if(gprsMode == Gmode_ALL4G)
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;
			else
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		}
		else {
			if(gprsMode == Gmode_ALL4G)
				GPRSLinkProcess = 0;
			else
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//	
		}
		return;
	}

#ifdef _debug_gprs
 	if (GPRSLinkProcess < 22)
	{
		debugstring("stepxxx:");
		debugdata((unsigned char*)&GPRSLinkProcess,1,1);
	}
#endif


	switch(GPRSLinkProcess)
	{
	case GPRS_MODE_POWER_DOWN:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_DOWN!\r\n");
#endif
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		
		gmissOldflag = 0;
		gmissflag = 0;

		if(gprsMode == Gmode_900C){
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			break;
		}
		powerOff4V0();
		set_timer0(5000, 1);
		GPRSLinkProcess = GPRS_MODE_POWER_ON;
		if(Moddstyle == 0x80){
			strcpy((char*)gpsrsP, "GT$POWEROFF\r\n");
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		}
		break;
	case GPRS_MODE_POWER_ON:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_ON!\r\n");
#endif
		if(gprsMode == Gmode_900C){
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			break;
		}
		
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		
		gmissOldflag = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_LINK_CMD;

		GPRS_open();//�ϵ�ʱ�������ͣ������ߡ�,һֱ�Ǹߵ�ģ�鿪���˻�
		if(Moddstyle == 0x80){
			strcpy((char*)gpsrsP, "GT$POWERON\r\n");
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			powerON4V0(115200);
		}
		else{
			if((gprsMode == Gmode_MODEM)||(gprsMode == Gmode_CDMA2000)//){
				||(gprsMode == Gmode_ALL4G)){
				powerON4V0(115200);
			}
			else{
				powerON4V0(9600);//�˶˿�����GPRS���ӿ�
			}
		}
		set_timer0(4000, 1);
		break;
	case GPRS_MODE_NEED_CLOSE://ģ����Ҫ���������ر�ģ������.�ɵİ��ӣ�����رյ�Դ
		if((gprsMode == Gmode_MG323)||(gprsMode == Gmode_MC323)){
			GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			break;
		}
#ifdef _debug_gprs
	debugstring("case==GPRS_MODE_NEED_CLOSE\r\n");
#endif
		GPRSLinkProcess = GPRS_MC55_CLOSE;
		if(gprsMode == Gmode_MG323){
			GPRS_MC55i_open();//GPRS_open();//�ٱ�ɸߵ�ƽ������POWN >1 ��ػ� ����2�룬�ȹؿ��ٿ���
			set_timer0(4000, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_MC323){
			GPRS_MC55i_open();//����2�룬�ػ���
			set_timer0(2500, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRS_open();////����POWN  //EC20 �����͵�ƽ>=100ms���ػ��͵�ƽ>=600ms;  �ػ�����AT+QPOWD
			set_timer0(30000, 1); //ʵ���������
		}
		else if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode == Gmode_CDMA2000){
			;
		}
		else{
			GPRS_close_Gt900(5000);
		}
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		
		gmissOldflag = 0;
		gmissflag = 0;
		break;
	case GPRS_MC55_CLOSE:
#ifdef _debug_gprs
		debugstring("case==GPRS_MC55_CLOSE");
#endif

		if((gprsMode == Gmode_MG323)||(gprsMode == Gmode_MC323)){
			GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			break;
		}
		
		GPRSLinkProcess = GPRS_LINK_CMD;
		if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode != Gmode_900C){
			GPRS_open();//GPRS_MC55i_open();//����POWN
			set_timer0(5000, 1);
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRS_MC55i_open();     //����POWN
			GPRSLinkProcess = GPRS_LINK_CMD;
			gprsErrorTimes = 0;
			set_timer0(30000, 1);	//�ȴ�POWERED DOWN��ʾ����ػ���Ӧ������������ʵ�ʹػ�
			isNetOK[0] = 0;
		}
		break;
	case GPRS_LINK_CMD://�ر��У���ģ���Դ,�͵�ƽ 800ms
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_CMD\r\n");
#endif
		GPRSLinkProcess = GPRS_LINK_UP;
		if(gprsMode == Gmode_MG323){
			GPRS_MC55i_open();//GPRS_open();//����POWN ����POWN >1 �� ��ģ�飬��ر�ģ�顾�����ǿ��ġ�
			set_timer0(4000, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_MC323){
			GPRS_MC55i_open();//GPRS_open();//����>100ms����
			set_timer0(200, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_ALL4G){
			gprsIndex = 0;
			gprsErrorTimes = 0;
			memset(gprsRevBuf, 0, 50);
			GPRS_open();			//����POWN
			set_timer0(30000, 1);	//��ʱ�����ͣ�ֱ���յ�ģ�顰RDY����ʾ���˳�,Ŀ���Ǹ�λʱ����Ҫ�������������ֻ��>100ms����Ч
		}
		else if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode == Gmode_CDMA2000){
			set_timer0(1000, 1);
		}
		else{
			GPRS_open();
			set_timer0(800, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONHI\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		break;
	case GPRS_LINK_UP:
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_UP\r\n");
#endif

		GPRSLinkProcess = GPRS_AT_WAIT;
		if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode != Gmode_900C){
			GPRS_open();//GPRS_MC55i_open();//�ٱ�ɸߵ�ƽ������POWN >2��ػ� ����2�룬�ȹؿ��ٿ���
			set_timer0(10000, 1);//10000
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else{
			GPRS_MC55i_open();//�ٱ�ɸߵ�ƽ������ģ��.�ɵİ��ӣ�����򿪵�Դ
			set_timer0(40000, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONHI\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		break;
	case GPRS_NEED_CLOSEIP:
#ifdef _debug_gprs
		debugstring("case==GPRS_NEED_CLOSEIP\r\n");
#endif
		tcpipClose(0);
		gmissflag = 0;
		gprsErrorTimes += 2;
		set_timer0(5000, 1);
		break;
	case GPRS_AT_WAIT:
#ifdef _debug_gprs
		debugstring("case==GPRS_AT_WAIT\r\n");
#endif

		if(gprsMode == Gmode_OTHER){
			GPRSLinkProcess = TCPSTARTSTAT;
			isNetOK[0] = 3;
		}
		else{
			gprsErrorTimes = 0;
			GPRSLinkProcess = 0;
		}
		break;
	case GPRS_TCPCLOSE_CMD:
#ifdef _debug_gprs
		debugstring("case==GPRS_TCPCLOSE_CMD\r\n");
#endif

		if(gprsMode == Gmode_CDMA2000)
		{
			GPRSLinkProcess = 9;			//�����޸ģ�����Ϊʲô��0?,����֮����Ҫ�������ô���MODE������6ʼ��
		}else
			GPRSLinkProcess = 20;
		break;
	case GPRS_Out_CMD:
		GPRSLinkProcess = 9;
		set_timer0(1000, 1);
		break;
	case 0:
 
		if((gprsErrorTimes >= 3)&&(gprsErrorTimes < 10)){ //������ԣ�������Ĳ�����115200û�Ĺ���
			GPRSModeBaudChage(gprsMode);
			gprsErrorTimes ++;
			set_timer0(5000, 1);
			break;
		}
		else if(gprsErrorTimes == 8){
			if(gprsMode == Gmode_MG323){
				GPRSLinkProcess = GPRS_LINK_CMD;//������PWON������ػ�һ��
			}
			else if(gprsMode != Gmode_ALL4G){
				GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			}
			gprsErrorTimes+=2;
			break;
		}

		gprsIndex = 0;
		memset(gprsRevBuf, 0, 20);
		
		isNetOK[0] = 0;
		isNetOK[1] = 0;
		gAPN_Style = 0;

#ifdef _debug_gprs
		//debugdata((unsigned char*)&gprsErrorTimes, 1, 0);
#endif
		gprsErrorTimes++;
		set_timer0(3000, 1);
		memcpy((char*)gpsrsP, "AT\r\n", 4);
		gprs_snd(4, gpsrsP);
 		break;
	case 1:
		set_timer0(5000, 1);
		memcpy((char*)gpsrsP, "ATE0\r\n", 6);//�رջ���
		//memcpy((char*)gpsrsP, "AT\r\n", 4);//
		gprs_snd(6, gpsrsP);
		gprsErrorTimes+=5;
		break;
	case 2:
		set_timer0(6000, 1);
		memcpy((char*)gpsrsP, "AT+CPIN?\r\n", 10);//���SIM�����Ƿ�����
//		strcpy(gpsrsP, "AT+CPIN=?\r\n");
		gprs_snd(10, gpsrsP);
		gprsErrorTimes+=5;
		break;
	case 3:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		set_timer0(2000, 1);
		if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "AT^SWSPATH=1\r\n", 14);//��������ͨ��
			gprs_snd(14, gpsrsP);
		}
		else if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT+CTA=120\r\n", 12);//��������ģʽ�ȴ�ʱ��
			gprs_snd(12, gpsrsP);
		}
		else if(gprsMode == Gmode_CDMA2000){
			memcpy((char*)gpsrsP, "AT+CGMM\r\n", 9);//���������Ҫ�����ǲ�ѯ��Ʒ������Ϣ��ATI
			//memcpy((char*)gpsrsP, "ATI\r\n", 5);		//��ѯ����汾  ���Ժ�ʱ�����ʹ��
			gprs_snd(9, gpsrsP);
		}
		else if(gprsMode == Gmode_ALL4G){////��ѯSIM��IMSI����  //AT+CIMI   ʶ��ͬ��Ӫ��
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			memcpy((char*)gpsrsP, "AT+CIMI\r\n", 9);//ʶ��ͬ��Ӫ��
			gprs_snd(9, gpsrsP);
			set_timer0(5000, 1);
		}
		else{
			memcpy((char*)gpsrsP, "AT%SLEEP=0\r\n", 12);//�ر�˯��ģʽ
			gprs_snd(12, gpsrsP);
		}
		gprsErrorTimes+=5;
		break;
	case 4:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		set_timer0(5000, 1);
		if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "AT+CLIP=1\r\n", 11);//����������ʾ
			gprs_snd(11, gpsrsP);
		}
		else if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT^MEID\r\n", 9);//���RUIM ���Ͳ���RUIM ���鵽�ĺ����ǲ�ͬ��
			gprs_snd(9, gpsrsP);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CREG=1\r\n", 11);//����ģ������ע����ʾ����ģ���GSM�����е��ߺ󣬻��Զ��ϱ�
			gprs_snd(11, gpsrsP);
		}
		gprsErrorTimes++;
		break;
	case 5:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT+CREG=?\r\n", 11);//���GSM����ע�����
			gprs_snd(11, gpsrsP);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CREG?\r\n", 10);//���GSM����ע�����
			gprs_snd(10, gpsrsP);
		}
		gprsErrorTimes++;
		set_timer0(5000, 1);
		break;
	case 6:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "AT+CLVL=4\r\n", 11);//������������
			gprs_snd(11, gpsrsP);
		}else if (gprsMode == Gmode_CDMA2000){//ģʽ�����Ƶ�����9
			GPRSLinkProcess++;
			break;
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRSLinkProcess++;
			break;
		}
		else{
			memcpy((char*)gpsrsP, "AT+CSQ\r\n", 8);//���GSM����ע�����
			gprs_snd(8, gpsrsP);
		}
		set_timer0(5000, 1);
		gprsErrorTimes++;
		break;
	case 7:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT+CREG=1\r\n", 11);//����ģ��GPRS����ע����ʾ����ģ���GPRS�����е��ߺ󣬻��Զ��ϱ�+CGREG: 1,0
			gprs_snd(11, gpsrsP);
		}else if (gprsMode == Gmode_CDMA2000){
			memcpy((char*)gpsrsP, "AT^SYSINFO\r\n", 12);//��ѯϵͳ��Ϣ
			gprs_snd(12, gpsrsP);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CGREG=1\r\n", 12);//����ģ��GPRS����ע����ʾ����ģ���GPRS�����е��ߺ󣬻��Զ��ϱ�+CGREG: 1,0
			gprs_snd(12, gpsrsP);
		}
		gprsErrorTimes++;
		set_timer0(5000, 1);
		break;
	case 8:
		memset(gpsrsP,0,sizeof(gpsrsP));
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
// 			strcpy(gpsrsP, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");//���ö��Ŵ洢��ַ
// 			gprs_snd(strlen(gpsrsP), gpsrsP);
		}else if (gprsMode == Gmode_CDMA2000){
#ifdef _debug_gprs
			debugstring("PPP��������\r\n");
#endif
			strcpy((char*)gpsrsP, "AT^IPCALL=0,\"CARD\",\"CARD\"\r\n");
			
			set_timer0(10000, 1);

		}
		else{
			memcpy((char*)gpsrsP, "AT+CGREG?\r\n", 11);//���GPRS����ע�������������������ʱ��ѭ�����ͣ��˽�ģ��ע������״̬
//			gprsErrorTimes++;
			set_timer0(1000, 1);
		}
		gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		break;
	case 9:
		if(gprsMode == Gmode_MC323){
			set_timer0(1000, 1);
			strcpy(gpsrsP, "AT+CNMI=1,1,0,2,0\r\n");//���ö����ϱ�
			gprs_snd(strlen(gpsrsP), gpsrsP);
		}
		else if (gprsMode == Gmode_CDMA2000){//ģʽ�����Ƶ�����9
			memcpy((char*)gpsrsP, "AT^IOMODE=1,3\r\n",15);	//mode 2�ַ���ʽ���ɷ������ַ���IPSEND �Գ��Ƚ��������ַ�����,���Ǻ�̨������͵�����������0d0A,������Ϊ���ˣ���������ã�����Ҫ��mode3
			gprs_snd(15, gpsrsP);			//�����޸� ԭ����17���п���ʹ������ʧЧ��
			set_timer0(1000, 1);
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRSLinkProcess++;
			break;
		}
		else{
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "at+cgatt=1\r\n", 12);//GTM900-Aģ��Ĭ���ǲ�����GPRS����ģ���������Ҫʹ��GPRS���ܣ������ֶ����ø���
			gprs_snd(12, gpsrsP);
		}
		gprsErrorTimes++;
		break;
	case 10:
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
		}
		else if (gprsMode == Gmode_CDMA2000){
			GPRSLinkProcess = 20;
		}
		else if(gprsMode == Gmode_ALL4G){	//case 8: //����PDP ������ز���			
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes+=5;
			
			switch (ugISMI_type)
			{
			case ChinaMobile:		//AT+QICSGP=1,1,"CMNET","","",0
				strcpy((char*)gpsrsP, "AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n");		//���������ñ�1�� CMNET
				//display(4, 0, "�ƶ���������    ", 0);
				break;
				
			case ChinaUnicom:	//AT+QICSGP=1,1,"UNINET","","",1
				strcpy((char*)gpsrsP, "AT+QICSGP=1,1,\"3gnet\",\"\",\"\",1\r\n");		//���������ñ�1��UNINET(2g), 3gnet(3G/4G,ȷ��), TD-LTE/wonet(4G?)
				//display(4, 0, "��ͨ��������    ", 0);
				break;
				
			case ChinaTelecom:	
				//AT+QICSGP=1,1,"0","CARD","CARD",0			//AT+QICSGP=1,1,"CTNET","","",0     //AT+QICSGP=1,1,"CTLTE","","",0
				strcpy((char*)gpsrsP, "AT+QICSGP=1,1,\"CTLTE\",\"\",\"\",0\r\n");	//���������ñ�1, ctnet(3g), CTLTE(4g)
				//display(4, 0, "������������    ", 0);
				break;
				
			default:
				break;
			}
			
			set_timer0(8000, 1);
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		}
		else{
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "AT+CGATT?\r\n", 11);
			gprs_snd(11, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 11:
		if(gprsMode == Gmode_MC323){
			set_timer0(1000, 1);
			strcpy(gpsrsP, "AT^HSMSSS=0,0,6,0\r\n");//���ö��Ų�����UNICODE����
			gprs_snd(strlen(gpsrsP), gpsrsP);
			gprsErrorTimes++;
		}
		else if(gprsMode == Gmode_ALL4G){	// case 9:
			//����PDP ����1, ͨ���ƶ�����ȡIP		AT+QIACT=<contextID>      ��context ID, range is 1-16��
// 			GprsSingleCheck();
// 			delayxs(1);
// 			sprintf((char*)buff, "%02d ", gprsSingle);
// 			display(0, 0, (char*)buff, 0);
			
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes++;
			set_timer0(150000, 1);		//150 seconds,determined by network
			
//			strcpy((char*)gpsrsP, "AT+QIACT=1\r\n");		//���������ñ�1�� CMNET
			memcpy(gpsrsP, "AT+QIACT=1\r\n", 12);
			gprs_snd(12/*strlen((char*)gpsrsP)*/, gpsrsP);
//			display(4, 0, "��¼�ƶ�����    ", 0);
		}
		else{
			gprsErrorTimes+=3;
			set_timer0(3000, 1);
			memcpy((char*)gpsrsP, "AT+CMGF=0\r\n", 11);//���ö���Ϣ��ʽ���� PDU ģʽ
			gprs_snd(11, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 12:
		if(gprsMode == Gmode_ALL4G){//case 10;����PDP ����1, ͨ���ƶ�����ȡIP��AT+QIACT?
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes+=5;
			set_timer0(3000, 1);
			
			strcpy((char*)gpsrsP, "AT+QIACT?\r\n");		//���������ñ�1�� CMNET
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
//			 display(4, 0, "��ѯ��¼״̬    ", 0);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CIMI\r\n", 9);//SIM�����к� ��ѯIMSI ��
			set_timer0(5000, 1);
			//		strcpy((char*)&gpsrsP, "AT+CGPADDR=?\r\n");
			//		strcpy((char*)&gpsrsP, "AT+CGDCONT=?\r\n");
			gprs_snd(9, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 13:	
		if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT^HWVER\r\n", 10);//^HWVER:MC323aME1MC323M VER B
			gprs_snd(10, gpsrsP);
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRSLinkProcess = 20;
		}
		else{
			memcpy((char*)gpsrsP, "AT+CGMM\r\n", 9);//GTM900B
			gprs_snd(9, gpsrsP);
		}
		gprsErrorTimes++;
		set_timer0(5000, 1);
		break;
	case 14:
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
		}
		else{
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "AT+CGSN\r\n", 9);//030CNE7A69005499 ��ѯIMEI ��
			gprs_snd(9, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 15:// 
		if(gprsMode == Gmode_MC323){
			strcpy((char*)gpsrsP, "AT^IPINIT=,\"card\",\"card\"\r\n");//�й�����CDMA ����ҵ��ȱʡ�û�����card,���룺card
			gprs_snd(strlen(gpsrsP), gpsrsP);//
			set_timer0(40000, 1);
		}
		else{
			set_timer0(1, 1);
			GPRSLinkProcess++;
		}
		break;
	case 16:
		if(gprsMode == Gmode_MC323){
			gAPN_Style = 'N';//����
			memcpy((char*)gpsrsP, "AT^IPINIT?\r\n", 12);//�й�����CDMA ����ҵ��ȱʡ�û�����card,���룺card
			gprs_snd(12, gpsrsP);//
		}
		else{
			if(((gmissflag & 0xf0)==0x20)||((gmissflag & 0xf0)==0x10)){//��½�ƶ�ר����				
				memset(APNnet, 0, 32);
				getDefaultAPN(APNnet, 1);

				strcpy(gpsrsP, "AT+CGDCONT=1,\"IP\",\"");
				strcat(gpsrsP, APNnet);
				strcat(gpsrsP, "\"\r\n");//����apn
				gprs_snd(strlen(gpsrsP), gpsrsP);//
				gAPN_Style = 'Y';//ר��
			}
			else{
// #ifdef BUS_PINGXIANG_
// 				memcpy((char*)gpsrsP, "AT+CGDCONT=1,\"IP\",\"PXGJ.JX\"\r\n", 29);//�õ���ר��
// 				gprs_snd(29, gpsrsP);//
// 				gAPN_Style = 'N';//����
//#else
				memset(gpsrsP, 0, sizeof(gpsrsP));
//				memcpy((char*)gpsrsP, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", 27);//����apn
				strcpy((char*)gpsrsP, "AT+CGDCONT=1,\"IP\",\"");
				memset(APNnet, 0, 32);
				getDefaultAPN(APNnet, 0);
				strcat((char*)gpsrsP, APNnet);
				strcat((char*)gpsrsP, "\"\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
				gAPN_Style = 'N';//����
//#endif
			}
		}
		set_timer0(20000, 1);
		gprsErrorTimes++;
		break;
	case 17:
		if(gprsMode == Gmode_MC323){
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "AT^IPDATMODE=1\r\n", 16);//�����Զ��ϱ�
			gprs_snd(16, gpsrsP);
			gprsErrorTimes++;
		}
		else if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "at^sics=0,conType,GPRS0\r\n", 25);//����tcpip ���� //����ppp����
			gprs_snd(25, gpsrsP);
			set_timer0(30000, 1);
			gprsErrorTimes+=5;
		}
		else{
			memcpy((char*)gpsrsP, "AT%ETCPIP\r\n", 11);//����tcpip ���� //����ppp����
			gprs_snd(11, gpsrsP);
			set_timer0(30000, 1);
			gprsErrorTimes+=5;
		}
		break;
	case 18:
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
		}
		else if(gprsMode == Gmode_MG323){
			memset(gpsrsP, 0, sizeof(gpsrsP));
// 			memcpy((char*)gpsrsP, "at^sics=0,apn,CMNET\r\n", 21);//
// 			memcpy((char*)gpsrsP, "at^sics=0,apn,CMNET\r\n", 21);//
			strcpy((char*)gpsrsP, "at^sics=0,apn,");
			memset(APNnet, 0, 32);
			getDefaultAPN(APNnet, 0);
			strcat((char*)gpsrsP, APNnet);
			strcat((char*)gpsrsP, "\r\n");
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			set_timer0(15000, 1);
		}
		else{
			memcpy((char*)gpsrsP, "at%etcpip?\r\n", 12);//����ñ���IP��ַ���Լ�DNS�������ĵ�ַ
			gprs_snd(12, gpsrsP);
			set_timer0(15000, 1);
		}
		gprsErrorTimes+=2;
		break;
 	case 19:
		if(gprsMode == Gmode_MG323){
			set_timer0(5000, 1);
 			strcpy(gpsrsP, "at^siss=0,srvType,socket\r\n");//����Internet ����
			gprs_snd(strlen(gpsrsP), gpsrsP);
		}
		else if(gprsMode == Gmode_ALL4G){//�ر�PDP ����1,  AT+QIDEACT=<contextID>      ��context ID, range is 1-16��
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes+=5;
			set_timer0(3000, 1);
			
			strcpy((char*)gpsrsP, "AT+QIDEACT=1\r\n");		//���������ñ�1�� CMNET
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		}
		else{
			GPRSLinkProcess++;
		}
// 		set_timer0(5000, 1);
// 		strcpy((char*)gpsrsP, "AT%IOMODE=,2\r\n");//���ö�����ģʽ
// 		gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		break;
	case 39:
		if(gprsMode == Gmode_MG323){
			set_timer0(15000, 1);
			strcpy(gpsrsP, "at^siss=0,conID,0\r\n");//����Internet ����
			gprs_snd(strlen(gpsrsP), gpsrsP);
		}
		else{
			GPRSLinkProcess = 40;
		}
		break;
	case 40:
// 		if(gprsMode == Gmode_MG323){
// 			set_timer0(1500, 1);
// //			strcpy(gpsrsP, "AT+CGPADDR=1\r\n");// ��ʾconProfileId Ϊ0 ������Profile ����Ϣ  ȡ��������^SICI: 0,0,0,"0.0.0.0"
// 			strcpy(gpsrsP, "AT^SICI?\r\n");
// 			gprs_snd(strlen(gpsrsP), gpsrsP);
// 		}
// 		else{
			GPRSLinkProcess = 20;
//		}
		break;
	case 20:
// 		if((memcmp(irda_rxBuf, "\"0.0.0.0\"", 8) != 0)&&(irda_rxBuf[0] == '\"')){
// 			irda_rxBuf[16] = 0;
// 			miniDispstr(6, 0, (char*)irda_rxBuf, DIS_CENTER);
// 		}
		gTcpIpbusy = TcpIPFree;
		if(gprsMode == Gmode_OTHER){
			GPRSLinkProcess = TCPSTARTSTAT;
			isNetOK[0] = 3;
		}
		else{
			ClientIP = getCpuIPaddr();
			ClientPort = getCpuPort();
			memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
			if(gprsMode == Gmode_CDMA2000){
				sprintf((char*)gpsrsP, "AT^IPOPEN=1,1200,\"%u.%u.%u.%u\",%u,0\r\n\x0", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else if(gprsMode == Gmode_MC323){
				sprintf((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"%u.%u.%u.%u\",%u,1000\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else if(gprsMode == Gmode_MG323){
				sprintf((char*)gpsrsP, "at^siss=0,address,\"socktcp://%u.%u.%u.%u:%u\"\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else if(gprsMode == Gmode_ALL4G){	//Pushģʽ��AT+QIOPEN=1,0,"TCP","61.190.19.174",8705,0,1   �յ����ݺ�����+QIURC: ��recv��,0,4\r\n �󷵻����ݡ�
												//BufferģʽAT+QIOPEN=1,0,"TCP","61.190.19.174",8705,0,0   �յ����������Ҫ����AT+QIRD=0,1500������
				sprintf((char*)gpsrsP, "AT+QIOPEN=1,0,\"TCP\",\"%u.%u.%u.%u\",%u,0,1\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else{
				sprintf((char*)gpsrsP, "AT%%IPOPEN=\"TCP\",\"%u.%u.%u.%u\",%u\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			set_timer0(30000, 1);
			gprsErrorTimes+=5;

#ifdef _debug_gprs
			debugstring("20-14:");
		//	debugdata((unsigned char *)&gpsrsP, ret, 1);
			debugstring(gpsrsP);
#endif
			gprs_snd(strlen(gpsrsP), gpsrsP);
//		set_timer0(1000, 0);//1S�ӽ�һ��
		}
		break;
	case 21:
#ifdef _phone_test_//ͨ�����ԣ�������
		break;
#endif
		gTcpIpbusy = TcpIPFree;
		if(gprsMode == Gmode_MG323){
			strcpy(gpsrsP, "at^siso=0\r\n");//��ָ����Internet ����
			gprs_snd(strlen(gpsrsP), gpsrsP);
			set_timer0(30000, 1);
			gprsErrorTimes+=5;
		}
		else if (gprsMode == Gmode_CDMA2000){
			GPRSLinkProcess++;
		}
		else
			GPRSLinkProcess++;
		initTCPIPnode(); //���±߽Ӻ�������ͻ����е�����
		break;
	case 22:
		break;
	default:
		break;
	}
maingprs1:
 	t = get_timer0(0);
	if(t == 0){
		set_timer0(1000, 0);//10ms�ӽ�һ��

 		TaskRecWrite();//����Ҫ���͵�����
// 		if(omissDly++ > 10){
// 			omissDly = 0;	
		in_decgM_Senddll();
		in_dispGprsProcess();



//		Sendpincount++;
		gSendGLogin++;


			if(gSendOverTime > 0){
				gSendOverTime--;

			}

			if(tcpipPacket.TTL > 0)
				tcpipPacket.TTL--;
			if(tcpipPacket.TTL > 60)
				tcpipPacket.TTL = 0;
			if(gErrortimes[1]>0){
				gErrortimes[1]--;
				if((gErrortimes[1] == 0) && (gErrorFlag&ERROR_BUS_CONNECT)){//���������������,����
#ifdef _debug_gprs
					debugstring("~!!!~~\r\n");
#endif
					gErrorFlag &= (~ERROR_BUS_CONNECT);			//pfquan  Ҫ��Ȼһֱ�������������00 00 
					if(gprsMode == Gmode_ALL4G)
						GPRSLinkProcess = 0;
					else
						GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
				}
			}
			if(gErrortimes[0]>0){
				gErrortimes[0]--;
			}
			if(gBusDownPro.Miss_Time>0)
				gBusDownPro.Miss_Time--;
			//ȥ���ɣ���������̨ɾ�������ˣ������Ϸ������
// 			if (gBusDownPro.Miss_Time == 0)
// 			{
// 				gBusDownPro.Miss_Time = GPRS_Login_dly;
// 				if(   ( memcmp(gBusVerInfo.newbusBLKVer, "\x00\x00", 2)!=0)		)
// 				{//GPRS�Ѿ���½�����Է�����
// 					if (memcmp(gBusDownPro.Miss_ver,"\x00\x00",2) != 0)
// 					{
// 						ret  = Build_GJ_DPro((unsigned char *)gpsrsP);
// #ifdef _debug_gprs
// 						debugstring("send Pro agin\r\n");
// #endif
// 						gprs_send_data(1, ret, (unsigned char *)gpsrsP);//����Ӧ��
// 					}
// 				}
// 			}

			
			if(gBusblkDownPro.Miss_time>0)
				gBusblkDownPro.Miss_time--;

			if ((gBusblkDownPro.Miss_time == 0))
			{
				gBusblkDownPro.Miss_time = GPRS_Login_dly;
				if(   ( memcmp(gBusVerInfo.newbusBLKVer, "\x00\x00", 2)!=0)		)
				{//GPRS�Ѿ���½�����Է�����
					if (memcmp(gBusblkDownPro.Miss_ver,"\x00\x00",2) != 0)
					{
					ret  = Buid_GJ_DBLK((unsigned char *)gpsrsP);
#ifdef _debug_gprs
					debugstring("send DBLK agin\r\n");
#endif
					gprs_send_data(1, ret, (unsigned char *)gpsrsP);//����Ӧ��
					}
				}
			}
			if(gTcpIpbusydly > 0){
		
				gTcpIpbusydly--;
				if (gTcpIpbusydly == 0)
				{
#ifdef _debug_gprs
					debugstring("��ʱ--free--\r\n");
#endif
					if(gTcpIpbusy == TcpIPBusy){//Ӧ�������ݷ���
						gGPRS_data_style.ucSendstyle  = GPRS_SISW;		//ȥ�������ݰ�
					}
					else{
 						gTcpIpbusy = TcpIPFree;
 						gGPRS_data_style.ucSendstyle = 0;
 						gGPRS_data_style.ucReadStyle = 0;

						if(gprsMode == Gmode_ALL4G){
							GPRSLinkProcess = 19;
						}
					}

				}
			}

			in_sendGPs();//��Ϊ0���Ѿ�����һ��GPS��Ϣ

			if(gprsMode == Gmode_MG323){
				if((GPRSLinkProcess == TCPSTARTSTAT)||(GPRSLinkProcess == 0xA0)){//GPRS�Ѿ���½�����Է�����
					gprs_send_data(1,0,(unsigned char*)gpsrsP);//���������Ƿ������ݷ��͡�ÿ���Ӳ�һ�Σ�
				}
			}
//		}
 	}
maingprs2:
	mainGprs();//�������е�GPRS����
}

//���û��GPRS��������������ݣ���Ҫ���õ�1���Ӽ�������
void main_Onsec(void)
{
	unsigned int t;

 	t = get_timer0(0);
	if(t == 0){
		set_timer0(1000, 0);//1S�ӽ�һ		
		in_dispGprsProcess();
		in_decgM_Senddll();
		in_sendGPs();//��Ϊ0���Ѿ�����һ��GPS��Ϣ

 	}
}



int getgprslibVer(void)		//��ȡ��汾��
{
	return SOFT_VAL;
}

