
#ifndef _GPS_SLF_HEAD_
#define _GPS_SLF_HEAD_

#define GPS_MODIFY_LONG_

#include "stdio.h"
#include "string.h"
//#include "board.h"
#include "szct.h"
//#include "public.h"
#include "..\include\include.h"

unsigned char LineRevFlag;	//������·��Ϣ1 �����У� 0�������
unsigned int revLineInfoOut;
unsigned int gGPSStatinoSUM;

//volatile unsigned char gllstyle;//GLL����������������ζ�û�У�����Ϊ��G595������������Ϣ.

unsigned char gpsTimeOK;//gpsʱ����ȷ

#define IDSoundPowerOn 0xa1
#define IDSoundStartST 0xa2
#define IDSoundEndST 0xa3
#define IDSoundOUT 0xa4
#define IDSoundIN1 0xa5
#define IDSoundComm 0xa6
#define IDSoundDefin 0xa7
#define IDSoundRound 0xa8
#define IDSoundFree 0xa9

#define IDSoundOverSpeed 0xB1 //����
#define IDSoundOverStation 0xB2//Խվ
#define IDSoundOverSpeedAT 0xB3 //����
#define IDSoundOverStationAT 0xB4 //����



stLineInfo LineInfo[2];//��·��Ϣ�������վ����Ϣ ��������0�� ����    1�鷴��
stNSpeedInfo SpeedInfo[2];//������Ϣ���� ��������0�� ����    1�鷴��

#define RS_IN_SOUND 0x01	//��վ��վ
#define RS_OUT_SOUND 0x02	//��վ��վ
#define RS_IN_MAIN 0x04		//������վλ��
#define RS_IN_SEND 0x10		//��վ�����ϴ�
#define RS_OUT_SEND 0x20	//��վ�����ϴ�
typedef struct  {
	unsigned char ucStation;			//��ǰվ̨��
	unsigned char ucPeStation;			//ǰһվ̨��
	unsigned char inOrOut;				//��վ��վ��־(1��2��)
	unsigned char ucOverSpeed;			//�Ƿ��� ����00����0C
	unsigned char ucDirect;				//���� ��0 ��1
	unsigned char ucFlag;				//��־���Ƿ�վ��4�ֽڣ�������4�ֽ�
}stRunStation;
stRunStation RunStation;				//�����е�վ����Ϣ

typedef struct  {
	unsigned char ucStation[2];
	unsigned int ulDisxx[2];
	unsigned int ulDisnn[2];
	unsigned char ucFlag[3];
}stFindStation;
stFindStation FindS;					//��վ�����

stAlammessage galamessage;

#ifdef _GPS_DIAODU_
//��������
stDiaoDuCMD gDiaoDuCMD;
#endif
//����Ӫ����Ϣ
stDiaoDuinfo gDiaoDuinfo;

typedef struct  {
	unsigned char Head[3];				//8A8A8A
	unsigned char deviceNo[4];			//�豸��
	unsigned char cmdgps[6];			//8000000000F0 
	unsigned short length;				//����
	unsigned char ucAlarmFlag;			//������־
	unsigned char dataBuf[50];			//��������
	unsigned short CrcCode;				//2�ֽ�У��
}stAlARM;

typedef struct {
	unsigned char flag;					//վ����
	unsigned char direct;				//����
	unsigned char buff[7];				//����ʱ��
}stGpsFED;
stGpsFED gGpsFeD;						//GPS�������������

#define START_MAX 0x03					//���뷢�����������
unsigned char gStartFlag;				//���վ��־��0x00:û�н�����ʼ������,����:�����˷����㣩

stSoundInfo gSoundInfo;				//����

//stLcdCmd gLcdCmd;

volatile unsigned char ucAlarmFlag;				//������־

#define GPS_SEND_DLY 5				//GPS��վ���ϱ�ʱ��
#define GPS_RE_SEND_DLY	65000			//�ط�GPS��Ϣ
#define GPSVALID				0x41	//��Ч
#define GPSINVALID				0x56	//��Ч
#define GPSUES					0xaa	//ʹ����

GPS_INFO gGpsInfo;		//GPS��Ϣ,Ӧ�ó����жϵĻ���
stGpsPar gGPsPar;				//������Ϣ

#define GPS_RCV_BUFFER_SIZE 100

volatile unsigned char gGStyle=0;//GPS��Ϣ����
volatile unsigned char gucGPSindex=0;
unsigned char gps_RcvBuf[GPS_RCV_BUFFER_SIZE+1];	//GPS��Ϣ���ջ���
volatile unsigned char gGpscand;//�յ��˶�����GPS����
typedef struct
{
	unsigned char GGA[100];
	unsigned char GLL[100];
	unsigned char VTG[100];
	unsigned char ZDA[100];
	unsigned char RMC[100];
}stGpscmdbuf;
stGpscmdbuf gGpsCmdBuf;

//---------------------------------------------
extern unsigned char GetDateTime(void);
extern unsigned int GetCrcWord(unsigned char* ptr,int length);
extern unsigned int GpsDistance(unsigned int dsLa, unsigned int dsLo);
extern unsigned int GpsDistanceTow(unsigned int srLa, unsigned int srLo, unsigned int dsLa, unsigned int dsLo);
extern fFlagVary_1 F_FLAG_VARY;
extern pFistVary_1 pFistVary;
extern unsigned char DriveCardNo[4];
extern Parameter2 POINT_MESSAGE;
extern stcomtime SysTime;

//-------------gprs-----------------------

#define TCP_ID_VER 0x01		//TCP���汾
#define DVICE_STYLE 0x01	//�豸����

#define GPSDATA_LEN	34					//��¼����
extern unsigned char GPRSLinkProcess;
extern unsigned char GprsCardList[16];
extern unsigned int ClientIP;
extern unsigned short ClientPort;

//mobile
extern unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.
//0x01 ���ز���
//0x02 ������������
//0x03 �������ؽ��֪ͨ
//0x04 ������
//0x05 �����ͽ���
//0xF0 ��½������̨
#endif
