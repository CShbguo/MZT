//����ͨ Ȩ����
////����ͨ  ���е�M1��,����CPU���ͽ�ͨ���Ŀ�

#ifndef _MZT_HEAD_H__
#define _MZT_HEAD_H__


#define SETION3   3
#define SETION5   5
#define SETION6   6
#define SETION7   7

typedef struct
{
   unsigned char bytes[16];
}ICCard_block12;

typedef struct     //Ǯ��
{
	unsigned char Purse[4];//Ǯ��
	unsigned char InPurse[4];//Ǯ��ȡ��
	unsigned char PurseBak[4];//Ǯ��
	unsigned char Verify[4];//У��
}ICCard_block13;


typedef struct
{
   ICCard_block12 block12;
  ICCard_block13 block13,block14;  //Ǯ����Ǯ������
}ICCard_section3;

typedef struct     //��Ϣ��
{
	unsigned char bimonthlyDate[4];//˫������
	unsigned char singleMonthDate[4];//��������
	unsigned char cardType;//����
	unsigned char publishBicker[5];//����
	unsigned char remain[2];  //����
}ICCard_block20;

typedef struct      //��Ʊ����
{
	unsigned char monthPurse[4];//Ǯ��
	unsigned char monthInPurse[4];//Ǯ��ȡ��
	unsigned char monthPurseBak[4];//Ǯ��
	unsigned char monthVerify[4];//У��
}ICCard_block21;


typedef struct
{
  ICCard_block20 block20;
  ICCard_block21 block21,block22;
}ICCard_section5;

typedef struct
{
   unsigned char bytes[16];
}ICCard_block24;

typedef struct
{
   unsigned char bytes[16];
}ICCard_block25;

typedef struct
{
   unsigned char remain[8];
   unsigned char evaluationTime[4];     //���˿����м��˿�����ʱ���ж�
   unsigned char remain2[4];
}ICCard_block26;

typedef struct
{
   ICCard_block24 block24;
   ICCard_block25 block25;
   ICCard_block26 block26;
}ICCard_section6;

typedef struct
{
   unsigned char remain[4];
   unsigned char saleCardTime[6];
   unsigned char deviceNumber[6];
}ICCard_block28;

typedef struct
{
   unsigned char remain[12];
   unsigned char rechargeAmount[4];
}ICCard_block29;

typedef struct
{
   unsigned char bytes[16];
}ICCard_block30;

typedef struct
{
   ICCard_block28 block28;
   ICCard_block29 block29;
   ICCard_block30 block30;
}ICCard_section7;

typedef struct
{
   unsigned char keya[4][8];			//����A������Կ		53
   unsigned char keyb[4][8];			//����A������Կ		53
   unsigned char keyc[4][8];			//����A��������		85
   unsigned char keyr[16];			//��¼������Կ		101
   unsigned char keySetion[7][8];
}ICCard_Key;

typedef struct
{
	//���ṹ
	ICCard_section3 section3;    //Ǯ��
	ICCard_section5 section5;   //��Ϣ������Ʊ��
	ICCard_section6 section6;
	ICCard_section7 section7;
	ICCard_Key key;
}stICCard;


typedef struct//�����ļ�������ȡ
{

	unsigned char M1StopFlag;       	//����1   0x31:�ر�M1   0x30:M1����ʹ��
	unsigned char Monthmun[9];			 //����Ԥ��

}stPARA_ARRAY;
//stPARA_ARRAY MZT_Para;

typedef struct 
{
	unsigned char cardProFlag[2];	//��������ʾ
	unsigned char cityCode[2];	//���д���
	unsigned char hangYeCode[2];	//��ҵ����
	unsigned char reserve[2];	//Ԥ��
	unsigned char canUseFlag;//Ӧ�����ͱ�ʶ
	unsigned char reserve2[5];		//Ӧ�ð汾
	unsigned char deposit[4];
	unsigned char appSerail[10];	//Ǯ����ʶ0.5�汾0.5
	unsigned char blackFlag;	//��ʼ������
	unsigned char cardType[2];	//��Ӧ�����ͱ�־
	unsigned char issuingData[4];	//���ñ�ʶ
	unsigned char issuingMach[6];
	unsigned char appVersion;	//�����ͱ�ʶ
	unsigned char reserveExt[9];	//Ԥ��
}MF_PublicBacinformation1;

//94 �ļ�
typedef struct 
{
	unsigned char saleTime[4];	// �ۿ�����
	unsigned char cardExpirationDate[4];	//��ֹ����
	unsigned long depositAmount;	// Ѻ��
	unsigned char sourceCode[6];//���۵����
	unsigned char operatorCode[8];		//����Ա����
	unsigned char enable;	//���ñ�־
	unsigned char blackFlag;	//��������ʶ
	unsigned char cardType[2];	//������
	unsigned char reserve[2];	//Ԥ��
}SaleInformation1;


#define MZT_BC
//#define MZT_ST
//#define MZT_JL
//#define MZT_YT

//----��------
#ifdef MZT_BC
#define code_96_57  0x80     //��������   0x80:����  0x40:��ͤ 0x20:��̨  0x10:����  0x08:����  0x04:ƽ����
#define SYS_HEAD_STR	"��������"
#define SYS_HEAD_STREn	"  BCGJ  "
#define CLOUD_BUSSINESSNO "\x00\x01"//"\x99\x98"//
#elif defined MZT_ST
#define code_96_57  0x20     //��������   0x80:����  0x40:��ͤ 0x20:��̨  0x10:����  0x08:����  0x04:ƽ����
#define SYS_HEAD_STR	"��̨����"
#define SYS_HEAD_STREn	"  STGJ  "
#define CLOUD_BUSSINESSNO "\x00\x03"//"\x99\x98"//
#elif defined MZT_JL
#define code_96_57  0x03     //��������   0x80:����  0x40:��ͤ 0x20:��̨  0x10:����  0x08:����  0x04:ƽ����
#define SYS_HEAD_STR	"��������"
#define SYS_HEAD_STREn	"  JLGJ  "
#define CLOUD_BUSSINESSNO "\x00\x02"//"\x99\x98"//
#elif defined MZT_YT
#define code_96_57  0x40     //��������   0x80:����  0x40:��ͤ 0x20:��̨  0x10:����  0x08:����  0x04:ƽ����
#define SYS_HEAD_STR	"��ͤ����"
#define SYS_HEAD_STREn	"  YTGJ  "
#define CLOUD_BUSSINESSNO "\x00\x04"//"\x99\x98"//
#endif


#define PCityCode "\x00\x27"

//#define CLOUD_BUSSINESSNO "\x99\x98"
#define SYS_cityName "SL"


//#define GJ_IP 0x0A0F89B6		
///#define GJ_PORT 8095


#define GJ_IP 0x023E03772	 //114.55.224.35 8819
#define GJ_PORT 8819

#define NETAPN "CMNET"
#define SWITCH_EC_CASH		1	// �����ֽ�
#define card_type_qpboc
//��˾ƽ̨����IP:202.96.137.46   ��ͨIP:58.251.128.49
// #define GJ_IP 0x2E8960CA
// #define GJ_PORT 8819
//�˿� �ƿ�:8818  ����:8819
//Զ�̣�202.96.137.46:2012


//#define JIULONG 

//-----------����Ϊ�豸�������á�-------
	#define _New_Bu_mode_
// 	#define BUS_PRO_JTB
// 	#define  BUS_CPU_
	//SLZR���Ѷ��� 
//	#define CDMA_MC323_
//  M50Ҳ���� MG323���壬ʹ��STGPRSM50Lib.lib
//	#define GPRS_MG323_
	#define ALL4G
// 	#define OTHER_GPRS
//	#define WIFI_TLH_
//���ô���ת���ڣ�ʹ�õ���͸������������DVR��ӡ���COM2RJ45_  �� OTHER_GPRS
// 	#define COM2RJ45_
	//���MODEM������Ҫ�������������ģ�������
	//#define Modem_BUS_
	//�ù�˾ƽ̨
	//#define IP_OF_SLZR_
	//���ô��ڲ���
	//#define COM_TEST_
	//********�ƶ�POSPƽ̨����ֻ�ܿ�һ��***************�ƶ�POSƽ̨����
	//ʹ�ú����ƶ�����

	//#define _MOBILE_IP_HUBEI_
	//ʹ�ó�ɳ����ƽ̨
	#define _MOBILE_IP_CHANGSHA_
	#define KEYBOARD
	//ʹ�ø���ƽ̨
	//#define _MOBILE_IP_FUJIAN_
	//ʹ������ƽ̨
	//#define _MOBILE_IP_XiAN_
	//ʹ���庣ƽ̨�������̣�
	//#define _MOBILE_IP_QINHAI
	//��д��IP��ַ
	//#define _MOBILE_IP_NOLOCK_
	//*********************************
	//*******************************************���綨��*******
	//ʹ�ù��� APN����ʹ��ר���ƶ�����,���˺����ƶ��������Ķ�Ҫ�򿪴˶���
	#define _MOBILE_CMNET_
	//û�й�����̨�����
//	#define NO_GJ_SERVISE
	//û���ƶ���̨//û��GPRS���ܣ�Ҳû���ƶ�GPRS
	#define NO_MOBILE_SERVISE
	//���岻ʹ��2.4G��
	#define NO_MOBILE_24G_ 
	//*********************************
	//***************************************ʹ�ù���PSAM��������***********
	#define NORM_DLY_TIME 0
	//���÷ֶ��շѹ��ܣ�һ̨������̨����
//	#define FengDuan_BU_
		//ǰ�Ŵ�GPS���ܵĻ�����ˢ��ʱд��ˢ���ĵص㣬ˢ������������Ǯ
		//ֻ��һ̨������ˢ���ֿ�Ǯ
//	#define FengDuan_only_1
	//������Ʊ���ͣ���Ҫ������ǰ�Ŀ����λ�õ�ֵ��ȷ�������Բ������⹦�ܣ��Ժ�ĳ���Ӧ���˶���
	//�����ͨ���������뵱ǰվ�㡢���򡣼���������Ч��GPS����ʱҲ����Ч�ġ�[���ڷֶ��շ�-���˵����]
//	#define _GPS_Station_ExKey_enable_
//	#define New_Times_Card__
	//����Ϊ���貿�Ŀ�M1
	#define BUS_PRO_JSB
	//Cloud
// 	#define BUS_Cloud_ 
// 	#define _M1KEY_CLOUD_
	//ʹ��9996�̻�,���̻��Ǻ���������ʾ�á�
	//#define CLOUD_9996_
	//��������·�������õ�����
	//#define AutoAudio_line
	//����ΪCPU��,Ӧ�ò���2.4G��

//SLZR���Ѷ��� 
	#define BUS_CPU_

//	#define CPU_JSB		//ס����������ͨ
	#define BUS_PRO_JTB	//��ͨ��������ͨ

	//����һ��ͨ
	//		#define FUJIAN_EKT_CPU_
	//�п�CPU��--û��PSAM��,Ҫ�� NO_CPU_PSAM_ - D:\CUR WORK\DOC\ͨѶЭ������\�п�_����
	//#define ZongKong_CPU_
		//����ר�ŵ�CPU ���� PSAM������Կͨ��M1����PSAM�����MAC1��������㡣(��2.4G��ͷ��Ҳ��Ч)
//	#define NO_CPU_PSAM_
		//�Ž�CPU����Ҫѡ��PSE �г���PSE��"\x02\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x33"
//	#define CPU_PSE "\x02\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x33"
				//ʤ�����ӵ��ƶ��ֻ�����13.56M ����ʢ��13.56MHZ�ֻ���������Կ
//			#define CPU_PSE "\x02\x00\xA4\x04\x00\x10\xD1\x56\x00\x00\x40\x10\x00\x30\x00\x00\x00\x01\x00\x00\x00\x00"
	//�ƶ�2.4G�ֻ�CPU���������ƶ���������ר����
	#define _CPU_24G_		//��������ͷ
//��ESF3000E  COM6
 	#define G24Com COM6
	//����ʢ������2.4G��ͷ��ʹ��M1���Ĺ��ܣ�ֻ�з�������Ǯ�����͹�����Ϣ��,��Կʹ������ƽ̨�ķ�����
				//CPU��ʹ�õĹ�˾��PSAM�����������Կ
//	#define _XMSHDZ_24G_
//	#define G24Com COM5
	//����Ϊʹ��3DES������Կ
	//#define _M1KEY_3DES_
	//����Ϊ�п�Ѷ��2.4G��ͷ
//	#define ZKXL_24G_
//�ڹ�˾CPU����ȡ������
//	#define qPBOC_CPU_CARDTYPE_
	//����Ϊ���п�
	#define qPBOC_BUS
//	==1 ��˳��� 
//	#define qPBOC_HAND 1
//	== 3û���κξ������
//	#define qPBOC_HAND 3
//	==2 �ӹ�Ӫ����Ҫ��������������Ϣ��
//	#define qPBOC_HAND 2
	//�������ѯTYPE B�Ŀ�
//	#define Card_typeB_enable_
	//*********************************
	//ʹ����Ӽ��̣�������COM1
//	#define KEYBOARD
	//�ϵĿۿʽ�����ŵĿ��ã�
	#define _PURSE_OLD_STYLE
	//�������շ�״̬
	#define _BU_NO_STOP_
	//����ˢ����������
//	#define _Counter_SWcard_times
//�����û��������ʹ�0��ʼ����ͨ��Ϊ0
//	#define Card_typeM_Min_is0_
//�ʼ��ȡPSAM��Կ�ķ�����ֻ�й�˾��PSAM����Ч
//	#define _old_PSAM_GET_KEY_
	//********************************
	//ֻ��һ��Ǯ
//	#define _Money_purchase_test
	//#define _Money_zero_
	//************************************************GPS ��ض���**********
	//����GPS����,�Զ���վ  
//	#define _GPS_ENABLE_
	//����ʹ���������� ʹ��M50��65�������ǰ��ģ��һ�����õĿ��C�ļ���һ����
//	#define _TTSSound_enable_
	//�е��ȹ���
	//#define _GPS_DIAODU_
//���͵�վ��¼��GPS��Ϣ10����һ��
//	#define _GPS_NEED_REC_
	//���ڲɵ������PC������
	//#define _GPS_CAIDIAN_
	//��¼���ػ���GPS�켣��ÿ��дһ��GPS��¼
	//#define _GPS_TRA_REC_
	//���ڴ�������GPS��Ϣ��
//	#define _GPS_TEST_
	//ʹ����ӿ�������COM4
	//#define KEY_PAD
	//����GPRSģ����������
	//#define GPRS_SOUD_ENABLE
	//����ʹ����ǰ16������
	//#define AUDIO_SOUND_OLD
	//û�и��ӷѹ���
	#define NO_PlusSta_

	//���Ӷ�ά�빦��,ͬʱָʾCOM��λ��
	#define QR_CODE_COM COM_QR
// 	//�������ܲ��� ͬʱָʾCOM��
// 	#define _BlueTooth_COM COM6
	//�Ƿ�ʹ��FLASH�ļ�ϵͳ
	#define _FASH_FATs_SYS_
	#define Factory_NUM "\x04\x22\x65\x90"

//-----------����Ϊ�豸�������á�-------

// #ifndef KEYB_COM
// #define KEYB_COM 4
// #endif





/*
M1			
M1	��ͨ��	��	41
M1	�Żݿ�	��	42
M1	AԱ����	��	43
M1	B���˿�	��	44
M1	C���м��˿�	��	45
M1	D��	��	46
M1	E��	��	47
M1	ѧ����	��	48
M1	��Ů��	��	49


����ͨ	��ͨ��		88	81
����ͨ	Ա����		88	83
����ͨ	���꿨		88	84
����ͨ	�м���		88	85
����ͨ	ѧ����		88	88
����ͨ	Ա����Ů��	88	89
�ֻ�2.4G��NFC��		89	����ͨ�ӿ����ۿ�	�����ݶ�Ӧ������

*/


///////////////////////////////���ֿ��Ķ���/////////////////////////////////
#define CARD_DATA_GATHER	1				//���ݲɼ���
#define CARD_DEV_SET		2				//�豸���ÿ�
#define CARD_LINE_SET		3				//��·���ÿ�
#define CARD_NUMBER_SET		4				//�������ÿ�
#define CARD_DELAY_SET		5				//��ʱ��
#define CARD_TOLL_START		6				//�շ�������
#define CARD_TIME_SET		7				//ʱ�����ÿ�
#define CARD_LEV_BUS		8				//������
#define CARD_ARV_BUS		9				//��վ��
#define CARD_DRIVER_BUS		10				//˾����
#define CARD_CLERK_BUS		11				//վ��Ա��
#define CARD_AIR_BUS		12				//�յ���
#define CARD_MONTH_STA		13				//��Ʊ������
#define CARD_Business		49				//�̻����ÿ�
#define CARD_WHITE_BUS		50				//�׿�	
#define CARD_KEY_BUS		60				//��Կ��	
///////////////////////////////�˿Ϳ�32��,��Χ40H~5FH//////////////////
#define CARD_NORMAL_BUS		0x41			//1��ͨ��65
#define CARD_YOUHUI_BUS		0x42			//�Żݿ�73  ..
#define CARD_STUFF_BUS		0x43			//9�Ŵ���67  .. ����ͨԱ����
#define CARD_OLDMAN_BUS		0x44			//B���˿�
#define CARD_Love_BUS		0x45			//��Ȳм��˿�
#define CARD_Love_BUS_2		0x46			//�ضȲм��˿�
#define CARD_STUDENT_BUS	0x48			//ѧ����
#define CARD_ZILU_BUS		0x49			//��Ů�� 
#define CARD_UNION_BUS		0x4A			//��ͨ���Ͽ�
#define CARD_QR_BUS			0x4B			//��ά�뿨
#define CARD_PBOC_CASH		0x4C			//�����ֽ� 76	
#define CARD_QPBOC_BUS		0x4D			//������   77
#define CARD_QR_PBOC		0x4E            //������ά�� 78

#define CARD_NORMAL				CARD_NORMAL_BUS				//��ͨA��
#define CARD_STUDENT			CARD_STUDENT_BUS				//ѧ��A��

#define CARD_OLDMAN				CARD_OLDMAN_BUS				//���꿨
#define CARD_STUFF				CARD_STUFF_BUS				//Ա����
#define CARD_COUPON				CARD_YOUHUI_BUS				//�Żݿ�
#define CARD_NORMAL_B			CARD_ZILU_BUS				//��ͨB��


#define  CARD_FENDUAN_Line	33		//�ֶ���·Ʊ�ۿ�







































extern unsigned char second_money_flag;

unsigned char Card_typeProcess_MZT(unsigned char mode);


int set_line_mzt(char *infor);

//U��ȡ��·��Ϣ������0������<0:ʧ�ܣ�����>0�ɹ�
int usb_loadlineinfor(void);


#endif 


