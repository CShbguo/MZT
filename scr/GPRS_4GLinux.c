
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"
#include "DEF8583.h"

#include "szct.h"
//#include "QRZFB.h"
//#define _debug_gprs
//#define _debug_time	//
#include "ProcCharacter.h"
#include "qpboc_8583.h"
//V3 : ����4Gģ��
#define SOFT_VAL 3	//��汾

//�̶�λ��Ϊ�������ļ���0x1000����CPU��λ������Ӧ�ó������ʼλ�ü���0x1000.
char  const  MachineCode[64]  __attribute__ ((at(0x8010000+0x1000))) = "��ά�복�ػ�ESF3000L\x00";


#define GPRSCOM_BAUD 230400

#define CP_head  0
#define CP_len 4
#define CP_cmd 6
#define CP_sta 7
#define CP_dat 8
#define CP_minlen 10

typedef struct
{
	unsigned char styleEA;		//������ʱ��ֵ��Ϊ0xEA�����������0
	unsigned char dflag;	//���ݱ�ʶ��1=����״̬ 	2=�ļ�����״̬ 3=�ļ�����
	unsigned short datalen;
	unsigned char data[300];
}stEC20Style;
stEC20Style gEC20Style;

unsigned char GPRS_ICCID[20];
extern unsigned int str2int(unsigned char *pstr, unsigned char len);
extern unsigned char checkIP(char *IPb);

extern unsigned char Xor8(const unsigned char *pData, unsigned long DataLen);
extern void XFsoud(unsigned char num);
extern unsigned char StufferNO[4];
extern unsigned char DriveCardNo[4];

extern void setSendTimeOut(int link, int time);
extern int calFileCRC(unsigned int fileName, unsigned int fileLen);
extern void gprs_send_data_OTHER_CMD(unsigned char cmd, unsigned int len, unsigned char *dat);
extern void savesysbupara(void);
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern unsigned char SPI_Snd_dat(unsigned short len, unsigned char *dat, unsigned char cmd);
extern unsigned char GetDateTime(void);
//extern unsigned char GprsSend(unsigned short serialno,unsigned char cmd,unsigned short len,unsigned char *buff);
extern unsigned char DealStationBTM(unsigned int len, unsigned char *info);
extern void UPData_Line_Info_gprs(unsigned char *info);
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

void gprs_send_CMD(unsigned char cmd, unsigned int len, unsigned char *dat);

extern unsigned char gmissOldflag;
extern volatile unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.

extern stbusdownPara gBusblkDownPro;//����������
extern stbusdownpro gBusDownPro;
extern stGPSdownLine gGPSDownLine;
extern pFistVary_1 pFistVary;
extern stcomtime SysTime;
extern unsigned int a_sum1, s_sum1;
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
unsigned char tcpipsdnode;
stTcpIpSp gTcpIPsd[4];//�����ݴ棬���ܳ���256�ֽ�һ����
volatile stGPrsDataStyle gGPRS_data_style;


#define GPRSCOMMAX 1600
unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS���ջ���
volatile unsigned short gprsIndex;

#define GPRS_CLEAR_INDEX	0x55	//����������������
#define GPRS_DATA_OK		0xaa	//����׼����
#define GPRS_GOON			0		//����������
volatile unsigned char gprsReady = GPRS_GOON;//�յ�GPRS�ش�������,��Ҫ����.

unsigned int ClientIP;
unsigned short ClientPort;

unsigned char gprsSingle;//GPRS�ź�ǿ��
unsigned char gprsAnswer;//�ǣУң�ģ���Ӧ�ϣ˱�־
unsigned char gprsSendDlyflag;//ģ�鷢���Ƿ���ʱ��־
unsigned char isNetOK[5];//��·�����Ƿ�ɹ�

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

int gprs_send_CMD_WaitACK(unsigned char cmd, unsigned int len, unsigned char *dat, unsigned char *odat);
void GPRS_close_Gt900(unsigned int dlyt)
{
;	
}
unsigned short BCC(unsigned char *Bcc_pointer, unsigned short x)					//����BCCУ����
{
	unsigned char data;
	unsigned char k, h;
	unsigned char BCC_L = 0;
	unsigned char BCC_H = 0;
	unsigned short  BCC = 0;
	unsigned short i, j;

	for (i = 0; i < x; i++)
	{
		data = *(Bcc_pointer + i);
		for (j = 0; j < 8; j++)
		{
			if (BCC_H & 0x01) k = 1;
			else k = 0;
			BCC_H = BCC_H >> 1;                 //������ֽڡ�
			if (BCC_L & 0x01) h = 1;
			else h = 0;
			BCC_L = BCC_L >> 1;
			if (k) BCC_L = BCC_L | 0x80;
			else BCC_L = BCC_L & 0X7f;
			if (h) BCC_H = BCC_H | 0x80;
			else BCC_H = BCC_H & 0x7f;
			if (data & 0x01) h = 1;
			else h = 0;
			data = data >> 1;
			if (h)
			{
				BCC_H = BCC_H ^ 0x80;
				goto bcc_1;
			}
			else
			{
			bcc_1:
				if (BCC_H & 0x80)
				{
					BCC_H = BCC_H ^ 0x04;
					BCC_L = BCC_L ^ 0x08;
				}
			}
		}
	}
	BCC = 0;
	BCC = BCC | BCC_H;
	BCC = BCC << 8;
	BCC = BCC | BCC_L;
	return(BCC);
}

void gprs_snd(unsigned int len, char *dat)
{
	debugstring("GPRS SEND...�˺���������û����..\r\n");
}

extern unsigned short getCpuPort(void);
extern unsigned int getCpuIPaddr(void);

//����IP���˿�,ģ���յ�������½�������
int buildSeverInfo_06(char cmd, char link)
{
	unsigned char sendbuf[128];
	unsigned char bu[8];
	unsigned int pos = 0;
	unsigned short itmp;
	unsigned int ip = 0;
	unsigned int port = 0;
	unsigned char posLinkCount = 0;

	MSG_LOG("buildSeverInfo_06 start:%d,%d\n", cmd, link);

	// ͨѶЭ���ʽ�汾
	memcpy(sendbuf + pos, "\x01\x01", 2);
	pos += 2;
	//�����豸��Ϣ
	memcpy(sendbuf + pos, "BUS", 3);
	pos += 3;
	memcpy(sendbuf + pos, pFistVary.DeviceNo, 8);
	pos += 8;
	Get_SerialNumF4(sendbuf + pos);
	pos += 4;
	memcpy(sendbuf + pos, DriveCardNo, 4);
	pos += 4;
	memcpy(sendbuf + pos, pFistVary.LineNo, 3);
	pos += 3;
	sendbuf[pos++] = 0;	//���ƣ���0λ=1ʱ����GPS

	posLinkCount = pos;
	sendbuf[pos++] = 0;	//���ӵ�������ȡֵΪ1��4
	memset(sendbuf + pos, 0, 18);
	strcpy((char*)sendbuf + pos, "CMNET"); //APN1	18	ASCII�룬��0
	pos += 18;
	memset(sendbuf + pos, 0, 18);
	strcpy((char*)sendbuf + pos, "NO APN2"); //APN2	18	ASCII�룬��0
	pos += 18;
	memcpy(sendbuf + pos, gBusVerInfo.busProVer, 2);	//����汾��
	pos += 0x02;
	itmp = 60;
	memcpy(sendbuf + pos, (unsigned char*)&itmp, 2);//����ʱ��	2	HEX��λ��ǰ����λ�롣ģ��û�յ�ƽ̨���ݺ�������0xFFFF���Զ�������
	pos += 2;

	if (link == LINK_GJ || link == 5) {
		++sendbuf[posLinkCount];

		sendbuf[pos++] = LINK_GJ + 1;	//����1���	1	��1��4ȡֵ
		memset(sendbuf + pos, 0, 15);	//����1IP��ַ	15	ASCII�룬��0���磺192.168.0.11Ϊ3139322E3136382E302E3131000000
		memcpy(bu, (unsigned char*)&ClientIP, 4);
		sprintf((char*)sendbuf + pos, "%d.%d.%d.%d", bu[0], bu[1], bu[2], bu[3]);
		pos += 15;
		memcpy(sendbuf + pos, (unsigned char*)&ClientPort, 2);
		pos += 2;
		// ����1���Կ���
		// HEX����8λ��ÿλΪ1������Ч��0λ��HTTPS��1λΪ0��APN1��1λΪ1��APN2.
		sendbuf[pos] = 0x00;
		++pos;
	}
	//MSG_LOG("buildSeverInfo_06 start3:%d\n", pos);

	if (link == LINK_PBOC || link == 5) {
		++sendbuf[posLinkCount];
		MSG_LOG("buildSeverInfo_06 start11:%d\n", ip);

		// ����2
		getMobileParameter(3, (unsigned char *)&ip);
		//MSG_LOG("buildSeverInfo_06 start12:%d\n", link);

		//MSG_LOG("buildSeverInfo_06 start1:%d\n", ip);
		getMobileParameter(4, (unsigned char *)&port);

		//MSG_LOG("buildSeverInfo_06 start2:%d\n", port);
		sendbuf[pos++] = LINK_PBOC + 1;	//����2���	1	��1��4ȡֵ
		memset(sendbuf + pos, 0, 15);	//����1IP��ַ	15	ASCII�룬��0���磺192.168.0.11Ϊ3139322E3136382E302E3131000000
		memcpy(bu, (unsigned char*)&ip, 4);
		sprintf((char*)sendbuf + pos, "%d.%d.%d.%d", bu[0], bu[1], bu[2], bu[3]);
		pos += 15;
		//MSG_LOG("buildSeverInfo_06 start6:%d\n", port);
		memcpy(sendbuf + pos, (unsigned char*)&port, 2);
		pos += 2;
		//MSG_LOG("buildSeverInfo_06 start7:%d\n", link);
		// ����1���Կ���
		// HEX����8λ��ÿλΪ1������Ч��0λ��HTTPS; 1λΪ0��APN1��1λΪ1��APN2; 2λΪ0������HTTPͷ,2λΪ1������HTTPͷ,3λΪ1��������SOCKET
		sendbuf[pos] = 0x0D;  //0000 1101    0000 0101
		++pos;
	}

	//MSG_LOG("buildSeverInfo_06:");
	//debugdata(sendbuf, pos, 1);

	gprs_send_CMD(cmd, pos, sendbuf);

	return pos;
}

int buildFileDown_HTTP_07(unsigned char *fflag, unsigned char *fflag2)
{
	unsigned char sndbuf[128];
	unsigned int pos = 0;

	//���Ӻ�1�ֽ�+2�ֽ��̻���+3�ֽ��ļ���ʶ+2�ֽ���Ҫ���صİ汾��
	sndbuf[pos++] = 1;
	memcpy(sndbuf + pos, CLOUD_BUSSINESSNO, 2);
	pos += 2;
	memcpy(sndbuf + pos, fflag, 3);	//�ļ���ʶ����ʶ+�ӱ�ʶ
	pos += 3;
	memcpy(sndbuf + pos, fflag2, 3);	//�ļ���ʶ�ӱ�ʶ
	pos += 3;
	memcpy(sndbuf + pos, gsl8583FileDownPara.Miss_ver, 2);
	pos += 2;

	gprs_send_CMD(7, pos, sndbuf);

	return pos;
}

//�������ݵ�ģ�� ��������1�ֽڡ�1=ˢ����¼��2=��ǰ��¼�ļ���3=��ʷ��¼��+�ļ�����(4�ֽڣ�+ƫ������4�ֽڣ�+���ݳ��ȣ�4�ֽڣ�+���ݣ�L)+�ļ������м䲿�֡�
int buildDataSend_0B(unsigned char mode, unsigned int offset, unsigned int Alen, unsigned int datLen, unsigned char *data, unsigned char *obuf, unsigned char *tmb)
{
	unsigned char sndbuf[2048];
	unsigned int pos = 0;

	sndbuf[pos++] = mode;
	memcpy(sndbuf + pos, (unsigned char *)&Alen, 4);
	pos += 4;
	memcpy(sndbuf + pos, (unsigned char *)&offset, 4);
	pos += 4;
	memcpy(sndbuf + pos, (unsigned char *)&datLen, 4);
	pos += 4;
	memcpy(sndbuf + pos, data, datLen);
	pos += datLen;
	memcpy(sndbuf + pos, pFistVary.DeviceNo, 8);	//���淢���豸�ż�����ʱ�䣬��Ϊ�������ļ�����һ����,LINUXģ���е�ǰֻ�е�ǰ��¼�ͱ���(��ʷ)��¼��Ч��
	pos += 8;
	memcpy(sndbuf + pos, tmb, 14);
	pos += 14;
	sndbuf[pos++] = 0;	//��󷢽�����

	return gprs_send_CMD_WaitACK(0x0B, pos, sndbuf, obuf);
}

int BuildHTTPPackge(unsigned char *revBuf, unsigned char mode)
{
	int relen = 0;

	switch (mode)
	{
	case MISS_HTTP_BLK:
		MSG_LOG("����ģ�������ļ�ָ��.Ver:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		relen = buildFileDown_HTTP_07(SL8583FileFLAG_BLK, "BUS");


		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;
	case MISS_HTTP_EC20:
		MSG_LOG("����ģ�������ļ�ָ��.Ver:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		relen = buildFileDown_HTTP_07(SL8583FileFLAG_EC20, POS_EC20_FLAG);

		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;
	case MISS_HTTP_PRO:
		MSG_LOG("����ģ�������ļ�ָ��.Ver:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		relen = buildFileDown_HTTP_07(SL8583FileFLAG_PRO, POS_Cand_FLAG);

		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;
	default:
		break;
	}
	return relen;
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

	if ((len > 1024) || (len == 0)) {
#ifdef _debug_gprs
		debugstring("TcpIpsd Save Data too long!!\r\n");
#endif
		return;
	}//BDBDBD226868752001020000000000072012070508265618E6
	for (i = 0; i < 4; i++) {//����ͬ�����ݰ�
		if (gTcpIPsd[i].T == 0xE501) {//
			if (memcmp(gTcpIPsd[i].V + GCMD_CmdCode, dat + GCMD_CmdCode, 2) == 0) {
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
	if (i >= 4) {//û���ҵ���ͬ��
		for (i = 0; i < 4; i++) {//�ҿ���ط�
			if (gTcpIPsd[i].T != 0xE501) {//�ձ�
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if (i >= 4) {//û���ҵ����еģ������
		if (tcpipsdnode > 0)//
			i = tcpipsdnode - 1;
		else
			i = 3;

		gTcpIPsd[i].T = 0xE501;
		gTcpIPsd[i].L = len;
		memcpy(gTcpIPsd[i].V, dat, len);
	}
#ifdef _debug_gprs
	// 	for(i=0;i<4;i++){
	debugstring("TcpIPSd_SAVE:");
	debugdata((unsigned char *)&gTcpIPsd[i], gTcpIPsd[i].L + 4, 1);
	// 	}
#endif
}

//ȡ��������Ҫ��������
unsigned short TcpIPSd_Pick(unsigned char *dat)
{
	unsigned char i;
	unsigned short rLen;

	rLen = 0xFFFF;
	if (tcpipsdnode > 3)
		tcpipsdnode = 0;
	for (i = 0; i < 4; i++) {
		if (gTcpIPsd[tcpipsdnode].T == 0xE501) {
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
		else {
			tcpipsdnode++;
			if (tcpipsdnode > 3)
				tcpipsdnode = 0;
		}
	}
#ifdef _debug_gprs
	if (rLen != 0xFFFF)
	{
		debugstring("TcpIPSd_Pick:");
		debugdata(dat, rLen, 1);
	}
#endif
	return rLen;
}

//ֻ����01����
void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos = 0;

	//	unsigned int CRC;
	unsigned short i;

	MSG_LOG("gprs_send_data:%d,%d\n", linkNum, len);
	memset(buff, '$', 4);//��ͷ
	pos = 4;

	i = len + 10;		//ֻ��һ��XOR,�������Ӻ�
	memcpy(buff + pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff + pos);//���ݳ��ȣ���������+Ӧ�ò����� �ĳ���,���ֽ���ǰ
	pos += 2;

	buff[pos++] = 0x01;//�������ݣ�ֱ�ӷ��͵���̨�����ֽڲ���Ҫ����
	buff[pos++] = gpacSNo;//�����
	buff[pos++] = linkNum + 1;
	if (len != 0)
	{
		memcpy(buff + pos, dat, len);
		pos += len;
	}

	// 	CRC = crc16_isr((unsigned char*)buff, pos);	
	// 	memcpy(buff+pos, (unsigned char*)&CRC, 2);
	// 	//over_turn(2, buff+pos);
	//	pos += 2;
	i = pos + 1;
	memcpy(buff + 4, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff + 4);//���¼��㳤��

	buff[pos++] = Xor8(buff, pos);

#if defined _debug_//DVR
	debugstring("SEND DVR:::");
	debugdata(buff, pos, 1);
#endif
	com_snd(COM_GPRS, pos, buff);
}

void gprs_send_CMD(unsigned char cmd, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos = 0;
	unsigned short i;

	memset(buff, '$', 4);//��ͷ
	pos = 4;

	i = len + 9;
	memcpy(buff + pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff + pos);//���ݳ��ȣ���������+Ӧ�ò����� �ĳ���,���ֽ���ǰ
	pos += 2;

	buff[pos++] = cmd;//����
	buff[pos++] = gpacSNo;//�����
	if (len != 0)
	{
		memcpy(buff + pos, dat, len);
		pos += len;
	}

	buff[pos++] = Xor8(buff, pos);

#ifdef _debug_
	debugstring("Send DVR-CMD:");
	debugdata(buff, pos, 1);
#endif

	com_snd(COM_GPRS, pos, buff);

}

//������ ���ȴ�Ӧ�𣬲���Ӧ�����ݷ��ء�
int gprs_send_CMD_WaitACK(unsigned char cmd, unsigned int len, unsigned char *dat, unsigned char *odat)
{
	unsigned char i;

	memset(irda_rxBuf, 0, sizeof(irda_rxBuf));//ͨ���жϽ���ģ�鷵�ص�ֵ

	gprs_send_CMD(cmd, len, dat);

	set_timer0(500, 2);
	while (1) {
		if (irda_rxBuf[0] == 0xEA) {
			i = irda_rxBuf[1];
			MSG_LOG("�յ�Ӧ����len=%d:", i);
			memcpy(odat, irda_rxBuf + 2, i);
			BCD_LOG(odat, i, 1);
			return irda_rxBuf[1];
		}
		if (get_timer0(2) == 0)
			return -1;	//��ʱ
	}
}

//��Linuxģ���в��Һ�����
unsigned int Linux4GFindBLK(unsigned char *sno)
{
	unsigned char sndbuf[16];
	int ret;

	memset(sndbuf, 0, sizeof(sndbuf));
	memcpy(sndbuf, SL8583FileFLAG_BLK, 3);
	sndbuf[3] = 10;	//LINUX�ڵĺ������ĵ���Ϊ10�ֽ�
	memcpy(sndbuf + 4, sno, BLK_SNO_LEN);//����10�ֽڣ�����������油0

	ret = gprs_send_CMD_WaitACK(0x0A, 14, sndbuf, sndbuf);
	MSG_LOG("LinuxBLKF ret=%d\r\n", ret);
	if (ret >= 0) {
		BCD_LOG(sndbuf, ret, 1);
		if (memcmp(sndbuf, "\x90\x00", 2) == 0) {//�Ǻ�����
			return 0;
		}
	}
	return 0xFFFFFFFF;
}

//mode == 1 �ر�����1 mode==2:�ر�����2 mode==0:�رյ�����
void tcpipClose(unsigned char mode)
{
	GPRSLinkProcess = 20;
	set_timer0(1, 1);
}

int DealComData0x2B(unsigned char *inbuf)
{//��������1�ֽڡ�1=ˢ����¼��2=��ǰ��¼�ļ���3=��ʷ��¼��4=���ػ�����ģ�鷢�ͣ���+�ļ�����(4�ֽڣ�+ƫ������4�ֽڣ�+���ݳ��ȣ�4�ֽڣ�+���ݣ�L)
	unsigned int addr, itmp, pos, flen, offset, dlen, i;
	unsigned char buftemp[64];

	pos = 0;

	if (inbuf[pos] != 4) {//���ǳ����ļ�,��ǰֻ�г����ļ�����ʶΪ4
		return -1;
	}
	pos++;
	memcpy((unsigned char *)&flen, inbuf + pos, 4);	//�ļ�����
	pos += 4;
	memcpy((unsigned char *)&offset, inbuf + pos, 4);	//ƫ����
	pos += 4;
	memcpy((unsigned char *)&dlen, inbuf + pos, 4);	//���ݳ���
	pos += 4;

	if (offset == 0) {//��һ��������Ҫ����
		clr_wdt();
		addr = FLASH_PRO_START;//д��ĵ�ַ ������ʼ��ַ
		for (i = 0; i < 7; i++) {
#ifdef _debug_
			debugstring("��մ�����X:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 
			addr += 0x10000;//��һ��64K�Ŀ�
		}
		flasherase(ERASE4KBYTESECTOR, FLASH_PRO_FLAG);//�Ȳ�����־��4K
	}

	sprintf((char*)buftemp, "P:%d%% ", (((offset + dlen) * 100) / flen));
	miniDispstr(14, 0, (char*)buftemp, 0);
	addr = FLASH_PRO_START + offset;

	flashwrite(addr, inbuf + pos, dlen);	//д������

	MSG_LOG("%s, flen:%d, offset:%d, dlen:%d\r\n", __FUNCTION__, flen, offset, dlen);

	itmp = offset + dlen;
	if (itmp >= flen)//�������
	{
		memset(buftemp, '$', 4);
		pos = 4;
		buftemp[pos++] = 0;
		buftemp[pos++] = 9;
		buftemp[pos++] = 0x2B;
		buftemp[pos++] = 0;

		buftemp[pos++] = 0x90;
		buftemp[pos++] = 0x00;
		i = pos + 1;
		memcpy(buftemp + 4, (unsigned char*)&i, 2);
		over_turn(2, buftemp + 4);	//�������볤��
		buftemp[pos++] = Xor8(buftemp, pos);

#ifdef _debug_
		printf("%s:", __FUNCTION__);
		debugdata(buftemp, pos, 1);
#endif
		com_snd(COM_GPRS, pos, buftemp);


		delayxs(10);//����Ҫ��ʱ10���Ӳ��������豸�� ��Ȼ��Ӱ�쵽LINUXģ����ļ�����

// 		feread(BIT_PRO_LINUX_CRC32, 4, (unsigned char*)&addr);//��ȡCRC32ֵ
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

		flashread(FLASH_PRO_START + 0x1000, buftemp, 64);	//������ʶ
		buftemp[63] = 0;
		if (memcmp(MachineCode, buftemp, strlen(MachineCode)) != 0) {//����������ػ��ĳ���
			MessageBox(1, "���Ǳ��豸����");
			display(6, 0, (char*)buftemp, DIS_CENTER);
			beep(5, 50, 50);
			while (1) {
				if (getkey(1) == KEY_ENT) {
					return ST_ERROR;
				}
				delayxms(50);
			}
		}



		memset(buftemp, 0, 64);
		memcpy(buftemp, "������Ҫ����", 12);
		memcpy(buftemp + 12, gsl8583FileDownPara.Miss_ver, 2);
		memcpy(buftemp + 14, (unsigned char*)&SysTime, 7);
#ifdef _debug_
		debugstring("�����������:");
		debugdata(buftemp, 21, 1);
#endif
		display(3, 0, "���,������...", DIS_CENTER);
		addr = FLASH_PRO_FLAG;

#ifdef _debug_
		debugstring("�����������2:");
		debugdata(buftemp, 34, 1);
#endif
		flashwrite(addr, buftemp, 64);

		beep(1, 10, 10);
		delayxms(400);
		ReStartCPU();//������ɣ������豸�Ա���³���
		while (1);
	}
	return 0;
}

extern stBusVerIfo gBusVerInfo;
//�����豸��������
void GPSDD_Deal_4GLinux(unsigned char mode, unsigned char pasn)
{
	//	unsigned char bu[16];
	unsigned char sendbuf[128];
	unsigned char pos = 0;
	unsigned int itmp;
	unsigned short packLen = 0;

	//MSG_LOG("GPSDD_Deal_4GLinux:\n");
	memcpy((unsigned char*)&packLen, gprsRevBuf + CP_len, 2);
	over_turn(2, (unsigned char*)&packLen);

	memset(sendbuf, '$', 4);
	pos = 4;
	sendbuf[pos++] = 0;
	sendbuf[pos++] = 9;
	sendbuf[pos++] = mode;
	sendbuf[pos++] = pasn;//�����
	switch (mode) {
	case 0x21://ת���������ļ�
		break;
	case 0x06://��������
	case 0x26://����ģ�����������IP��Ϣ��ģ��
		if (GPRSLinkProcess == 0) {
			GPRSLinkProcess++;
		}
		if (packLen > 0x09)
			memcpy((unsigned char*)&gBusVerInfo.Linux4GVer, gprsRevBuf + CP_dat + 1, 2);	//ģ�鷵�صİ汾��

		gsl8583Style.ISOK = 0;	//�յ������ϢҪ����ǩ��
		//if (gEC20Style.styleEA == 0) {
		//MSG_LOG("4Gģ�����������IP�˿�����:%04X\n", GET_INT16(gBusVerInfo.Linux4GVer));
		gEC20Style.styleEA = 0x26;
		//}
		break;
	case 0x07:
	case 0x27:	//�����ļ���Ӧ 3�ֽ��ļ���ʶ+2�ֽڰ汾��+9000���ļ��Ѿ�����)��8001����,87007  LINUX������������Ҫ�������������������ء�
		if (memcmp(gprsRevBuf + CP_dat, SL8583FileFLAG_BLK, 3) == 0) {//������
			if (memcmp(gprsRevBuf + CP_dat + 5, "\x80\x07", 2) != 0)
				clrgsl8583filelist(SL8583FileFLAG_BLK); //�Ѿ��յ��ˣ��Ͳ��ٷ�������
		}
		else if (memcmp(gprsRevBuf + CP_dat, SL8583FileFLAG_EC20, 3) == 0) {
			if (memcmp(gprsRevBuf + CP_dat + 5, "\x80\x07", 2) != 0)
				clrgsl8583filelist(SL8583FileFLAG_EC20);
		}
		else if (memcmp(gprsRevBuf + CP_dat, SL8583FileFLAG_PRO, 3) == 0) {
			if (memcmp(gprsRevBuf + CP_dat + 5, "\x80\x07", 2) != 0)
				clrgsl8583filelist(SL8583FileFLAG_PRO);
		}
		if (gmissflag >= 0x80)
			gmissflag = MISS_G_FREE;
		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x09:
	case 0x29:	//ģ�鷢״̬��Ϣ���������Ҫ���� ��������void dealgEC20Style(void)
		if (gEC20Style.styleEA == 0) {
			gEC20Style.styleEA = 0x29;
			gEC20Style.dflag = gprsRevBuf[CP_dat + 1];//��Ϣ��ʶ��ǰ���и����Ӻţ�û�ã����Լ�1
			gEC20Style.datalen = packLen - CP_dat - 2;
			memcpy(gEC20Style.data, gprsRevBuf + CP_dat + 1 + 1, gEC20Style.datalen);
			sendbuf[pos++] = gEC20Style.dflag;
			sendbuf[pos++] = 0x90;
			sendbuf[pos++] = 0x00;
		}
		break;
	case 0x0A:
	case 0x2A:	//��ѯ���������к����ڵȣ�ͨ��irda_buff����
	case 0x0B:	//�������ݵ�Ӧ��
		MSG_LOG("DEAL 0x0A CMD\r\n");
		irda_rxBuf[0] = 0xEA; //�з��ر�ʶ
		irda_rxBuf[1] = packLen - CP_dat - 1;;//�������ݳ���
		memcpy(irda_rxBuf + 2, gprsRevBuf + CP_dat, irda_rxBuf[1]);	//����״̬��
		break;
	case 0x2B:	//ƽ̨�����ļ�������
		MSG_LOG("DEAL 0x2B CMD\r\n");
		gEC20Style.styleEA = 0x29;
		gEC20Style.dflag = 0x2B;//gprsRevBuf[CP_dat+1];//��Ϣ��ʶ��ǰ���и����Ӻţ�û�ã����Լ�1
		gEC20Style.datalen = packLen - CP_dat - 1;
		memcpy(gEC20Style.data, gprsRevBuf + CP_dat + 1, gEC20Style.datalen);
		return;	//���ݻ�Ӧ�ڴ������ݺ���
	default:
		break;
	}

	if (mode > 0x21 && mode != 0x26) {//ģ����������������ҪӦ��
		itmp = pos + 1;//�ȼ��볤��
		memcpy(sendbuf + 4, (unsigned char*)&itmp, 2);
		over_turn(2, sendbuf + 4);	//�������볤��
		sendbuf[pos++] = Xor8(sendbuf, pos);

		// #ifdef _debug_
		// 		debugstring("GPSDD_Deal Send:");
		// 		debugdata(sendbuf,pos,1);
		// #endif
		com_snd(COM_GPRS, pos, sendbuf);
	}
}

void OTHER_Linux4G_hand(unsigned char value)
{
	unsigned short plen, st;
	unsigned char ii;

	ii = value;
	//	debugdata((unsigned char*)&ii, 1, 0);
#ifdef _debug_gprs
	com_snd(COM1, 1, (unsigned char*)&ii);
#endif

	switch (gprsIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if (ii != '$') {
			//			debugstring("^");
			gprsIndex = 0;
		}
		else {
			gprsRevBuf[gprsIndex++] = ii;
		}
		break;
	case 4://���ȸ�λ
		gprsRevBuf[gprsIndex++] = ii;
		break;
	case 5://���׵�λ

		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf + 4, 2);
		over_turn(2, (unsigned char*)&plen);
		if (plen > 2048) {//���ȴ���
			MSG_LOG("���ȴ���--:%04X\r\n", plen);
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 10);
			break;
		}
		break;
	default:
		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf + 4, 2);
		over_turn(2, (unsigned char*)&plen);
		if (plen > 2048) {//���ȴ���
#ifdef _debug_
			debugstring("���ȴ���*************\r\n");
#endif
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 10);
			break;
		}
		if (gprsIndex >= plen) {//���ݽ������
			clr_wdt();
#ifdef _debug_
			debugstring("��DVR_CMD:0x");
			debugdata(gprsRevBuf + 6, 1, 1);
			debugdata(gprsRevBuf, gprsIndex, 1);
#endif

			//gSendOverTime = 0;
			//#ifndef _debug_
			if (Xor8(gprsRevBuf, gprsIndex - 1) != gprsRevBuf[gprsIndex - 1])
			{
				debugstring("XOR error*************\r\n");
				delayxms(1000);
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 10);
				break;
			}
			//#endif
#if 0
#ifdef _debug_DVR
			miniDispstr(6, 2, "R", 0);
			beep(1, 50, 50);
			delayxms(100);
			miniDispstr(6, 2, " ", 0);
#endif
#endif

			//MSG_LOG("DVR CMD:%02X\r\n", gprsRevBuf[6]);

			clr_wdt();
			if (gprsRevBuf[CP_cmd] == 0x21) {//�Ǻ�̨����
				gprsErrorTimes = 0;
				for (ii = 0; ii < MAX_RCV_PACKET_NUM; ii++) {
					if (gPacketRcvBuf[ii].ucItemStatus != 0xea)
						break;
				}
				if (ii >= MAX_RCV_PACKET_NUM) {
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//�����������˰�����
//					debugstring("����\r\n");
					return;
				}
				st = plen - 10;	//���ݶȳ���
				memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf + CP_dat + 1, st);//ǰ����һ���ֽڵ����Ӻ�
				gPacketRcvBuf[ii].LinkFrame.ucFrameType = gprsRevBuf[CP_dat];	//�������ڴ����Ӻ�
				memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&st, 2);
				gPacketRcvBuf[ii].ucItemStatus = 0xea;
			}

			GPSDD_Deal_4GLinux(gprsRevBuf[CP_cmd], gprsRevBuf[CP_sta]);
			if (gprsRevBuf[CP_cmd] < 0x20) {	//ģ������������Ҫ��Ӧ
				gpacSNo++;//��������л�Ӧ���ͼӣ����潫���°�
			}
#ifdef _debug_DVR
			// 			delayxms(200);
			//			miniDispstr(6,2," ",0);
#endif

			gprsIndex = 0;
			memset(gprsRevBuf, 0, 10);
		}
		break;
	}
}

void GPRS_handler(unsigned char value)
{
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
	// 	com_snd(COM1, 1, (unsigned char*)&value);
		//debugdataCOM(COM4,(unsigned char*)&value,1,0);
	//	debugdata(&value,1,0);
#endif

	OTHER_Linux4G_hand(value);
}
extern unsigned char getGprsCom(void);
//��ʼ��GPRS�˿ڼ�ģ��
void GPRS_INIT(unsigned int band)
{
	unsigned char gprcom;
	gprcom = getGprsCom();

	band = GPRSCOM_BAUD;

#ifdef _debug_gprs
	debugstring("INI gprcom:");
	debugdata((unsigned char*)&gprcom, 1, 0);
	debugstring(":");
	debugdata((unsigned char*)&band, 4, 0);
	debugstring("|\r\n");
#endif

	com_init(gprcom, band);
	COM_it_enable(gprcom);
}

//V231 ��ʼ���ŵ�GPRS�����档
void GPRSConfig(void)
{
	if (in_getGprsModeStyle() != Gmode_Close) {
		if ((in_getGprsModeStyle() == Gmode_MODEM) || (in_getGprsModeStyle() == Gmode_CDMA2000)//){
			|| (in_getGprsModeStyle() == Gmode_ALL4G) || (in_getGprsModeStyle() == Gmode_YYLinux)) {
			GPRS_INIT(GPRSCOM_BAUD);
		}
		else {
			GPRS_INIT(9600);//�˶˿�����GPRS���ӿ�
		}
	}
}

void gprs_par_init(void)
{
	unsigned char i;

	GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//0;

	gprsErrorTimes = 0;
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gGPRS_data_style.ucSendstyle = 0;
	gGPRS_data_style.ucReadStyle = 0;
	gGPRS_data_style.uiNum = 0;
	guiMC323_REV_LEN = 0;

	initTCPIPnode();
	for (i = 0; i < MAX_RCV_PACKET_NUM; i++)
		gPacketRcvBuf[i].ucItemStatus = 0;
	memset(GprsCardList, 0, 16);
	gpacSNo = 0;//�����
}





//���һ�����е�֡����,���ؿ���֡����jgwei updata
unsigned char GetFrameBuf(void)
{
	unsigned char i;

	for (i = 0; i < MAX_RCV_PACKET_NUM; i++)
	{
		if (!(gPacketRcvBuf[i].ucItemStatus & MASK_RCV_BUF_USED))
		{
			break;
		}
	}
	if (i == MAX_RCV_PACKET_NUM)
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

	for (i = 0; i < MAX_RCV_PACKET_NUM; i++)
	{
		if (gPacketRcvBuf[i].ucItemStatus == 0xea)
		{				//����֡����
			break;
		}
	}
	if (i == MAX_RCV_PACKET_NUM)
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
	unsigned char i, ret;
	unsigned short plen = 0;//,uslen=0;
#ifdef	_debug_gprs 
	
#endif
	int packLen = 0;

	gprsReady = 0;//�������,��0
//	if((isNetOK[0] != 0)||(isNetOK[1] != 0)){
	i = PeekFrame();
	if (i != 0xff)
	{
#ifdef _debug_gprs
		// 			debugstring("RcvFrame: ");
		// 			memcpy(&plen,gPacketRcvBuf[i].LinkFrame.uiFrameLen,2);
		// 			debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf,plen,1);
#endif
			memcpy(&plen, gPacketRcvBuf[i].LinkFrame.uiFrameLen, 2);
//Ӧ�ò�����֡
// #ifdef	_debug_gprs 
// 		debugstring("TaskGprsRev-------:\r\n");
// 		debugdata((unsigned char *)&i, 1, 1);
// 		memcpy(&plen, gPacketRcvBuf[i].LinkFrame.uiFrameLen, 2);
// 		debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf, plen, 1);
// #endif
		ret = gPacketRcvBuf[i].LinkFrame.ucFrameType;	//
		MSG_LOG("link:%d\r\n", ret);

		switch (ret - 1)
		{
		case LINK_PBOC:
			gGPRS_Cmd_outtime = 0;
			//printf("HTTPͨѶ���� %d\r\n",link);
			ret = QPBOC_DataDeal((unsigned char*)&gPacketRcvBuf[i].LinkFrame, plen);//�������ݴ���

			break;
//		case ZFB_LINK - 1:
		case LINK_GJ:
			gGPRS_Cmd_outtime = 0;
#ifdef _debug_
			printf("mainGprs�������� %d\r\n", ret);
#endif
			GPRSLinkProcess = TCPSTARTSTAT;
			gmissflag = MISS_G_FREE;
			//gsl8583Style.ISOK= 'O';
			ret = GJDataDeal((unsigned char*)&gPacketRcvBuf[i].LinkFrame);//�������ݴ���

			//	XFsoud(100);
			break;
		default:
			printf("mainGprs���Ӵ��� %d\r\n", ret);
			break;
		}


		gPacketRcvBuf[i].ucItemStatus = 0;
		gGPRS_data_style.ucSendstyle = 0;
		//			gGPRS_data_style.ucReadStyle = GPRS_SISR;
		gTcpIpbusy = 0;

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


extern void getDefaultAPN(char *rAPN, unsigned char mode);//mode=0 ������1 �ƶ�����;
extern void US1_It(void);
extern void TaskRecWrite(void);
//������Ӧ��
extern unsigned int Buid_GJ_DBLK(unsigned char *dat);
#ifdef _debug_
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);

#endif

extern unsigned char s_isAuthOk;
//1=����״̬ 2=�ļ�����״̬ 3=�ļ�����
void dealgEC20StyleIp(void)
{
	unsigned char disbuf[64];
	unsigned int i, t, pos;

	if (gEC20Style.styleEA != 0) {//��δ�������Ϣ
		MSG_LOG("deal EC20�豸״̬:");
		BCD_LOG(gEC20Style.data, gEC20Style.datalen, 1);

		if (gEC20Style.styleEA == 0x29) {
			clr_wdt();
			if (gEC20Style.dflag == 2) {//2�ļ�����״̬���ļ�����20�ֽڣ���0������+�ܳ��ȣ�4�ֽڣ���λ��ǰ��+��ǰ���س��ȣ�4�ֽڣ���λ��ǰ����
				memset(disbuf, 0, sizeof(disbuf));
				memcpy(disbuf, gEC20Style.data, 20);//�ļ���
				memcpy((unsigned char*)&i, gEC20Style.data + 20, 4);
				memcpy((unsigned char*)&t, gEC20Style.data + 24, 4);
				if (t == 0xFFFFFFFF) {//����ʧ��
					sprintf((char*)disbuf, "%s����ʧ��", gEC20Style.data);
					MessageBox(1, (char*)disbuf);
					beep(2, 100, 100);
					delayxs(1);
					restore_flag = 3;
				}
				else {
					sprintf((char*)disbuf + strlen((char*)disbuf), "%d|%d %d%%", t, i, (t * 100) / i);
					MSG_LOG((char*)disbuf);
					miniDispstr(13, 0, (char*)disbuf, 0);
				}
			}
			else if (gEC20Style.dflag == 1) {//1����״̬��//�����Ƿ�ɹ���ʶ+20λSIM����+IP1����״̬+IP2����״̬+IP3����״̬+IP4����״̬
				if (gEC20Style.data[0] == 1) {//�������ӳɹ�
					if ((GPRSLinkProcess == 0x2F) || (GPRSLinkProcess >= 0xF0) || (GPRSLinkProcess < 9))
						GPRSLinkProcess = 9;
				}
				else {
					GPRSLinkProcess = 8;
				}
				memcpy(GprsCardList, gEC20Style.data + 1, 16);

				BCD_LOG(gEC20Style.data + 21, 4, 1);
				i = 0;
				for (i = 0; i < 2; ++i)
				{

					if (gEC20Style.data[21 + i] == 1) {//���ӷ������ɹ���
						if ((GPRSLinkProcess == 0x2F) || (GPRSLinkProcess >= 0xF0) || (GPRSLinkProcess < TCPSTARTSTAT))
							GPRSLinkProcess = TCPSTARTSTAT;
						isNetOK[i] = 3;
					}
					else {
						//MSG_LOG("deal EC20�豸״̬:");
						//BCD_LOG(gEC20Style.data + 21, 4, 1);
						isNetOK[i] = 0;
						if (i == LINK_PBOC) {
							s_isAuthOk = 1;
						}
						//gsl8583Style.ISOK = 0;
					}
				}
			MSG_LOG("gEC20Style.data:%d,%d,%d,%d\r\n", gEC20Style.data[21],gEC20Style.data[22], gEC20Style.data[23], gEC20Style.data[24]);
		MSG_LOG("isNetOK__before:%d,%d,%d,%d\r\n", isNetOK[0], isNetOK[1], isNetOK[2], isNetOK[3]);
			}
			else if (gEC20Style.dflag == 3) {//3�ļ�������1�ֽ��ļ�����+3�ֽ��ļ���ʶ_4�ֽڰ汾��+4�ֽڳ���....
				pos = 1;
				for (i = 0; i < gEC20Style.data[0]; i++)
				{
					if (memcmp(gEC20Style.data + pos, SL8583FileFLAG_BLK, 3) == 0) {//�Ǻ������ļ�.
	// 					MSG_LOG("�յ��������汾��Ϣ!!");
	// 					Ascii2BCD(gEC20Style.data+pos+4, gBusVerInfo.busBLKVer, 4);//��4�ֽ��ǰ汾�ţ�����汾��
	// 					over_turn(2, gBusVerInfo.busBLKVer);
	// 					BCD_LOG(gBusVerInfo.busBLKVer, 2, 1);
	// 					memcpy((unsigned char*)&pFistVary.BlackListNum, gEC20Style.data+pos+8, 4);//�ļ���С
	// 					MSG_LOG("����:%d\r\n", pFistVary.BlackListNum);
	// 
	// 					if(pFistVary.BlackListNum < 10){//�ļ�����ȷ
	// 						memset(gBusVerInfo.busBLKVer, 0, 2);	//��������
	// 					}
	// 					else{
	// 						pFistVary.BlackListNum /= 10;	//ƽ̨���ļ���10�ֽ�һ��
	// 						pFistVary.BlackListNum -= 1;	//ǰ10�ֽڲ��Ǻ�����
	// 						sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //����������		
	// 					}
	// 					t = FLASH_BLK_START;
	// 					flasherase(ERASE4KBYTESECTOR, t);
	// 					flashwrite(t, gBusVerInfo.busBLKVer, 2);//���º������汾
	// 					
	// 					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
	// 					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
	// 					gsl8583Style.ISOK = 0;
	// 					restore_flag = 3;
					}

					pos += 12;
				}
				if (gmissflag > MISS_HTTP) {
					gmissflag = MISS_G_FREE;
					if (GPRSLinkProcess == GPRS_SENDING_CMD)
						GPRSLinkProcess = TCPSTARTSTAT;
				}
			}
			else if (gEC20Style.dflag == 0x2B) {//2B-�����˳����ļ�
				memset(disbuf, '$', 4);
				pos = 4;
				disbuf[pos++] = 0;
				disbuf[pos++] = 9;
				disbuf[pos++] = 0x2B;
				disbuf[pos++] = 0;

				if (DealComData0x2B(gEC20Style.data) >= 0) {
					disbuf[pos++] = 0x90;
					disbuf[pos++] = 0x00;
				}
				else {
					disbuf[pos++] = 0x80;
					disbuf[pos++] = 0x01;
				}
				t = pos + 1;
				memcpy(disbuf + 4, (unsigned char*)&t, 2);
				over_turn(2, disbuf + 4);	//�������볤��
				disbuf[pos++] = Xor8(disbuf, pos);

#ifdef _debug_
				debugstring("GPSDD_Deal Send:");
				debugdata(disbuf, pos, 1);
#endif
				com_snd(COM_GPRS, pos, disbuf);
			}
		}
		else if (gEC20Style.styleEA == 0x26) {
			buildSeverInfo_06(0x26, 5);
			Sign_Infor.ISOK = 0;
		}

		memset((unsigned char*)&gEC20Style, 0, sizeof(gEC20Style));
	}
}


int open_sock(unsigned int addr, unsigned short port, unsigned int outtime, unsigned char link)
{
	unsigned char gprsMode = 0;
	char gpsrsP[200];
	unsigned int delay = 0;

	// 	unsigned int outdly=0;
	//	unsigned int i=0;
	//unsigned long long read_dat=0;
	//unsigned int count=0;
#ifdef _debug_gprs00
	unsigned char disbuff[20];
#endif

	MSG_LOG("do %s\r\n", __FUNCTION__);

	gprsMode = in_getGprsModeStyle();

	//	display(6, 0, "������...", DIS_ClsLine|DIS_CENTER);

	memset(gpsrsP, 0, sizeof(gpsrsP));
	memcpy((unsigned char*)gpsrsP + 50, (unsigned char*)&addr, 4);
	//	printf("������...\r\n");

	gGPRS_data_style.openLink = link;

	if (gprsMode == Gmode_CDMA2000) {
		sprintf((char*)gpsrsP, "AT^IPOPEN=%u,1200,\"%u.%u.%u.%u\",%u,0\r\n", link + 1, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);//1,2,3,4 ��Ч�� socket ID
	}
	else if (gprsMode == Gmode_MC323) {
		sprintf((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"%u.%u.%u.%u\",%u,1000\r\n", gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
	}
	else if (gprsMode == Gmode_MG323) {
		sprintf((char*)gpsrsP, "at^siss=0,address,\"socktcp://%u.%u.%u.%u:%u\"\r\n", gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
	}
	else if (gprsMode == Gmode_ALL4G) {	//Pushģʽ��AT+QIOPEN=1,0,"TCP","61.190.19.174",8705,0,1   �յ����ݺ�����+QIURC: ��recv��,0,4\r\n �󷵻����ݡ�
		if (link == LINK_PBOC)
		{
			//buildSeverInfo_06(0x26, LINK_PBOC);
			sprintf((char*)gpsrsP, "AT+QSSLOPEN=1,1,%d,\"%u.%u.%u.%u\",%u\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
		else
		{		//AT+QSSLOPEN=1,1,1,"202.101.25.188",20141        �����0��0 �����յ����ݱ���������ܳ���
			sprintf((char*)gpsrsP, "AT+QIOPEN=1,%u,\"TCP\",\"%u.%u.%u.%u\",%u,0,0\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
	}
	else if (gprsMode == Gmode_YYLinux)
	{
		gpsrsP[0] = '\0';
#if 0
		if (link == LINK_POBC)
		{
			gpsrsP[0] = '\0';
			//return 1;
			//sprintf((char*)gpsrsP, "AT+QSSLOPEN=1,1,%d,\"%u.%u.%u.%u\",%u\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
		else
		{		//AT+QSSLOPEN=1,1,1,"202.101.25.188",20141        �����0��0 �����յ����ݱ���������ܳ���
			sprintf((char*)gpsrsP, "AT+QIOPEN=1,%u,\"TCP\",\"%u.%u.%u.%u\",%u,0,0\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
#endif
	}
	else {
		sprintf((char*)gpsrsP, "AT%%IPOPEN=\"TCP\",\"%u.%u.%u.%u\",%u\r\n", gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
	}

	MSG_LOG("gpsrsP:%s", gpsrsP);



	// 	gprs_snd(strlen(gpsrsP), gpsrsP);

	//LINUX 4G ģ���Ҫ�������ͣ��򿪴����Ӻ�Ҫ�ж�һ��״̬�� �����ӣ���������ʱ���ٴ򿪣�
	if (gpsrsP[0] != '\0')
	{
		gprs_send_data(link, strlen(gpsrsP), gpsrsP);

	}
	else if (link == LINK_PBOC) {	// ��������Ҫ������
		//buildSeverInfo_06(0x06, LINK_PBOC);
	}
	while (1)
	{
		delayxms(1);
		if (getkey(1) != 0)
		{
			return -1;
		}
#ifdef _debug_gprs00
		memset(disbuff, 0, sizeof(disbuff));
		sprintf((char *)disbuff, "T2:%04d", outtime - delay);
		miniDispstr(7, 30, (const char *)disbuff, 0);
#endif

		if (isNetOK[link] == 3)
		{
#ifdef _debug_gprs
			MSG_LOG("open time:%u\r\n", delay);
#endif

			break;
		}
		if (delay++ > outtime)
		{

			MSG_LOG("=^=");
			//gprsErrorTimes+=25;		//ֱ����������
			MSG_LOG("19==\r\n");
			GPRSLinkProcess = 19;	//
			return -1;
		}

	}

	return 1;

}


void main_GpsRs(void)
{
	char gpsrsP[100];
	int ret;
	unsigned int t;

	t = get_timer0(1);

	if (t != 0) {
		goto maingprs1;
	}

	//MSG_LOG("G");

	if (gprsErrorTimes > 20) {//�������̫�࣬����ģ��
		gprsErrorTimes = 0;
#ifdef _debug_gprs
		debugstring("err too much\r\n");
#endif
		if ((GPRSLinkProcess == 20) || (GPRSLinkProcess == 21)) {
			if ((gmissflag & MISS_GJ) || (gmissflag == 0)) {//������ڵ�½����,�ǲ���ȥ,����ͣ,������־
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if ((gmissflag & MISS_IF1) || (gmissflag & MISS_IF2)) {
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			// 			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
			// 				return; //�������ͬʱ������,���˳�����������.
			// 			}
		}

#ifdef _debug_gprs
		debugstring("--4 close\r\n");
#endif
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		return;
	}


	switch (GPRSLinkProcess)
	{
	case GPRS_MODE_POWER_DOWN:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_DOWN!\r\n");
#endif
		gprsErrorTimes = 0;
		isNetOK[0] = 0;

		gmissOldflag = 0;
		gmissflag = 0;

		powerOff4V0();
		set_timer0(5000, 1);
		GPRSLinkProcess = GPRS_MODE_POWER_ON;
		break;
	case GPRS_MODE_POWER_ON:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_ON!\r\n");
#endif

		gprsErrorTimes = 0;
		isNetOK[0] = 0;

		gmissOldflag = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_LINK_CMD;

		GPRS_open();//�ϵ�ʱ�������ͣ������ߡ�,һֱ�Ǹߵ�ģ�鿪���˻�
		set_timer0(4000, 1);
		break;
	case GPRS_MODE_NEED_CLOSE://ģ����Ҫ���������ر�ģ������.�ɵİ��ӣ�����رյ�Դ
#ifdef _debug_gprs
		debugstring("case==GPRS_MODE_NEED_CLOSE\r\n");
#endif
		GPRSLinkProcess = GPRS_MC55_CLOSE;
		GPRS_open();////����POWN  //EC20 �����͵�ƽ>=100ms���ػ��͵�ƽ>=600ms;  �ػ�����AT+QPOWD
		gprsErrorTimes = 0;
		isNetOK[0] = 0;

		gmissOldflag = 0;
		gmissflag = 0;
		break;
	case GPRS_MC55_CLOSE:
#ifdef _debug_gprs
		debugstring("case==GPRS_MC55_CLOSE");
#endif

		GPRS_MC55i_open();     //����POWN
		GPRSLinkProcess = GPRS_LINK_CMD;
		gprsErrorTimes = 0;
		set_timer0(300, 1);	//�ȴ�POWERED DOWN��ʾ����ػ���Ӧ������������ʵ�ʹػ�
		isNetOK[0] = 0;
		break;
	case GPRS_LINK_CMD://�ر��У���ģ���Դ,�͵�ƽ 800ms
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_CMD\r\n");
#endif
		GPRSLinkProcess = GPRS_LINK_UP;
		gprsIndex = 0;
		gprsErrorTimes = 0;
		memset(gprsRevBuf, 0, 50);
		GPRS_open();			//����POWN
		set_timer0(300, 1);	//��ʱ�����ͣ�ֱ���յ�ģ�顰RDY����ʾ���˳�,Ŀ���Ǹ�λʱ����Ҫ�������������ֻ��>100ms����Ч
		break;
	case GPRS_LINK_UP:
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_UP\r\n");
#endif

		isNetOK[0] = 3;
		GPRSLinkProcess = GPRS_AT_WAIT;
		GPRS_MC55i_open();//�ٱ�ɸߵ�ƽ������ģ��.�ɵİ��ӣ�����򿪵�Դ
		set_timer0(400, 1);
		break;
	case GPRS_NEED_CLOSEIP:
#ifdef _debug_gprs
		debugstring("case==GPRS_NEED_CLOSEIP\r\n");
#endif
		//buildSeverInfo_06(0x06, 4);
		//isNetOK[0] = 0;
		GPRSLinkProcess = 0;
		gprsErrorTimes = 0;
		break;
	case GPRS_AT_WAIT:
#ifdef _debug_gprs
		debugstring("case==GPRS_AT_WAIT\r\n");
#endif
		GPRSLinkProcess = 0;

		isNetOK[0] = 3;
		break;
	case GPRS_TCPCLOSE_CMD:
#ifdef _debug_gprs
		debugstring("case==GPRS_TCPCLOSE_CMD\r\n");
#endif

		GPRSLinkProcess = 20;
		break;
	case GPRS_Out_CMD:
		GPRSLinkProcess = 9;
		set_timer0(1000, 1);
		break;
	case 0:
		//dealgEC20StyleIp();	//����EC20��״̬��Ϣ
		break;	//��4Gģ���IP��ѯ�����ʾģ������
	case 2:
		break;//��4Gģ�鱨��SIM��״̬��
	case 8:
		break;//��4Gģ�鱨������״̬��
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:// 
	case 16:
	case 17:
	case 18:
		GPRSLinkProcess++;
		break;
	case 19:
		GPRSLinkProcess++;
		break;
	case 20:
		gTcpIpbusy = TcpIPFree;
		initTCPIPnode(); //���±߽Ӻ�������ͻ����е�����
		GPRSLinkProcess++;
		break;
	case 21:	//�ȷ��������ӳɹ�
		if (gGPRS_data_style.openLink == LINK_PBOC) {
			GPRSLinkProcess++;
		}
		break;
	case 22:
		break;
	default:
		break;
	}
maingprs1:
	t = get_timer0(0);
	if (t == 0) {
		set_timer0(1000, 0);//10ms�ӽ�һ��

		TaskRecWrite();//����Ҫ���͵�����
		in_decgM_Senddll();
		in_dispGprsProcess();

		//MSG_LOG("P");
		
		reduce_delay_send();
		if (gSendOverTime > 0) {
			gSendOverTime--;
		}

		if (tcpipPacket.TTL > 0)
			tcpipPacket.TTL--;
		if (tcpipPacket.TTL > 60)
			tcpipPacket.TTL = 0;
		if (gErrortimes[1] > 0) {
			gErrortimes[1]--;
			if ((gErrortimes[1] == 0) && (gErrorFlag&ERROR_BUS_CONNECT)) {//���������������,����
#ifdef _debug_gprs
				debugstring("~!!!~~\r\n");
#endif
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
			}
		}
		if (gErrortimes[0] > 0) {
			gErrortimes[0]--;
		}
		if (gBusDownPro.Miss_Time > 0)
			gBusDownPro.Miss_Time--;


		gSendGLogin++;

		if (gBusblkDownPro.Miss_time > 0)
			gBusblkDownPro.Miss_time--;

		if ((gBusblkDownPro.Miss_time == 0))
		{
			gBusblkDownPro.Miss_time = GPRS_Login_dly;
			if ((memcmp(gBusVerInfo.newbusBLKVer, "\x00\x00", 2) != 0))
			{//GPRS�Ѿ���½�����Է�����
				if (memcmp(gBusblkDownPro.Miss_ver, "\x00\x00", 2) != 0)
				{
					ret = Buid_GJ_DBLK((unsigned char *)gpsrsP);
#ifdef _debug_gprs
					debugstring("send DBLK agin\r\n");
#endif
					gprs_send_data(1, ret, (unsigned char *)gpsrsP);//����Ӧ��
				}
			}
		}
		if (gTcpIpbusydly > 0) {

			gTcpIpbusydly--;
			if (gTcpIpbusydly == 0)
			{
#ifdef _debug_gprs
				debugstring("��ʱ--free--\r\n");
#endif
				if (gTcpIpbusy == TcpIPBusy) {//Ӧ�������ݷ���
					gGPRS_data_style.ucSendstyle = GPRS_SISW;		//ȥ�������ݰ�
				}
				else {
					gTcpIpbusy = TcpIPFree;
					gGPRS_data_style.ucSendstyle = 0;
					gGPRS_data_style.ucReadStyle = 0;

					GPRSLinkProcess = 19;
				}

			}
		}

		in_sendGPs();//��Ϊ0���Ѿ�����һ��GPS��Ϣ


		//MSG_LOG("Q");

	}

// #ifndef PBOC_TEST
// 	if ((gmissflag == 0) || (gmissflag == MISS_G_FREE) || (gmissflag == MISS_G_LOGINGJ)) {//�ӿ�������ң�����ÿһ���Ӳ���һ�Ρ�
// 
// 		find_G_new_mission();
// 	}
// #endif

	dealgEC20StyleIp();	//����EC20��״̬��Ϣ
	//MSG_LOG("R");
	mainGprs();//�������е�GPRS����
	//MSG_LOG("S");
}

//���û��GPRS��������������ݣ���Ҫ���õ�1���Ӽ�������
void main_Onsec(void)
{
	unsigned int t;

	t = get_timer0(0);
	if (t == 0) {
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


void setSendTimeOut(int link, int time) {
	clr_wdt();
	gGPRS_data_style.sendLink = link;
	gSendOverTime = time;
}

