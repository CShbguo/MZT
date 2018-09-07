/*
	File Name	��MAIN.h
	Object		��head file for main.c
	Creation	��22/3/2008 by KSLi
*/

#define CARD_SOUND
/////////////////////////���ֿ�����������///////////////////////

//***************************************************************************
//                       ���м�¼�ĸ�ʽ���� 
//***************************************************************************
#define LEN_CARD_RECORD 16

typedef struct
{
	unsigned char DealTime[4];			//����ʱ��4
	unsigned char Balance[4];		    //����ǰ���
	unsigned char DealMoney[3];			//���׽��
	unsigned char DealType;				//��������
	unsigned char DeviceNo[4];			//�豸��
}cardRecord1;
cardRecord1 gHisMassage;//�������ʷ��¼

Parameter6 ErrorDetail[3];
unsigned char error_pointer;		  //��������ָ��

typedef struct  {
	unsigned char Sec;
	unsigned char CardSno[4];
} card_Delay;
card_Delay PCard_delay;
//*************************************
#define POSTCODE_LEN			4 
#define DEVICENO_LEN			9 
#define DEVICENO1_LEN			4
#define LINENO_LEN				3

//***************************************************************
//				��ǰ������Ϣ�ļ��ṹ
//***************************************************************
struct BAK_WG1
{
	unsigned char WGcrc;
	unsigned char wBuffer_po;
	unsigned char GPRS_po;
} ;
struct pMonthTime_1
{
	unsigned char endptr;				
	unsigned char startptr;
	unsigned char LastList[LAST_LIST_LEN][7];              //��Ʊ����ʱ���
};

typedef struct 
{
	unsigned int strAddr;				//��ʼ��ַ
	unsigned int endAddr;				//������ַ
	unsigned int fileLength;			//�ļ�����
	unsigned char hisName[7];			//�ļ�ͷ
	unsigned char fileSno[2];			//�ļ����к�
	unsigned char resev[3];				//���
}Parameter10;
Parameter10 hisFCB[10];

stcomtime SysTime;

unsigned int s_sum1,a_sum1,dis_sum2;
unsigned int a_sumR;//�ֻ�Ǯ���е���ʵ���

unsigned char gucBLight;

unsigned char gucRestorFlag;
unsigned int face_DisTime;

stBusTicketV gBusTicketV;
stBusVerIfo gBusVerInfo;
stbusdownpro gBusDownPro;//��������
stbusdownPara gBusblkDownPro;//����������
stGPSdownLine gGPSDownLine;//��·��Ϣ

unsigned char gchn_eng;//��Ӣ�ı�־
unsigned short gErrorFlag;//�����ʶ,���ֽڱ�ʾ�����ţ����ֽڱ�ʾ������� ���"*��������ʶ����"
unsigned int gErrortimes[2];//���������,��Ҫ������GPRS����

unsigned char gPSAMin;//psam������λ��

#ifdef MOBILE_XIANYANG_
CardUserTimes gUserTimes;//��ʹ�õĴ���
#define XIANYANG_STU_CARD_TIMES_ 800
#endif

#ifdef _GPS_ENABLE_
unsigned char gGpsDisEN;	//GPS��ʾ״̬ 
extern volatile unsigned char ucAlarmFlag;
extern unsigned int GpsDistance(unsigned int dsLa, unsigned int dsLo);
#endif
#ifdef KEY_PAD
extern volatile unsigned char gKeyRestorDis;//��Ҫ����ˢ��KEYPAD��ʾ
extern void KeyPadDis(unsigned char line,unsigned char col, char*dat, unsigned char len);
extern void KeyPadClr(void);
extern void KeyPad_main(void);
#endif
extern unsigned char Keybuffer[10];
//--------GPS------------------
extern void GPS_PAR_CLR(void);
extern void gps_par_init(void);
extern void main_GpsRs(void);
extern void showGps(unsigned char mode);
//Rstr:���ص��ַ�
extern unsigned char GetInSound(char *Rstr);
extern stSoundInfo gSoundInfo;					//����


extern void getCpuInfo(stCpuInfo *inP);
extern void savesysbupara(void);




