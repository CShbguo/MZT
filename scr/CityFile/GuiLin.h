#ifndef _GuangXI_HEAD_
#define _GuangXI_HEAD_

//----��------
#define HandListlen 0
extern const unsigned int gHandList[HandListlen+1];

#define SYS_HEAD_STR	" ���ֹ��� "
#define SYS_HEAD_STREn	"  SLZR  "
#define PCityCode "\x00\x27"

#define CLOUD_BUSSINESSNO "\x99\x98"
#define SYS_cityName "GX"

#define GJ_IP 0x64646464
#define GJ_PORT 8819
#define NETAPN "CMNET"

//-----------����Ϊ�豸�������á�-------
	#define _New_Bu_mode_
	//SLZR���Ѷ��� 
	#define BUS_PRO_JTB
	#define BUS_CPU_
//	#define CDMA_MC323_
//  M50Ҳ���� MG323���壬ʹ��STGPRSM50Lib.lib
//	#define GPRS_MG323_
	#define ALL4G
//	#define OTHER_GPRS
//	#define COM2RJ45_
//	#define WIFI_TLH_
//���ô���ת���ڣ�ʹ�õ���͸������������DVR��ӡ���COM2RJ45_  �� OTHER_GPRS
//	#define COM2RJ45_
	//���MODEM������Ҫ�������������ģ�������
	#define _MOBILE_IP_CHANGSHA_
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
	//Cloud
// 	#define BUS_Cloud_ 
// 	#define _M1KEY_CLOUD_
	#define _YiDa_Card_
	//*********************************
	//ʹ����Ӽ��̣�������COM1
//	#define KEYB_COM 1
//	#define KEYBOARD
	//�ϵĿۿʽ�����ŵĿ��ã�
	#define _PURSE_OLD_STYLE
	//�������շ�״̬
	#define _BU_NO_STOP_
	//û�и��ӷѹ���
	#define NO_PlusSta_
	//����GPS����,�Զ���վ  
//	#define _GPS_ENABLE_
//	#define _TTSSound_enable_
	//���÷ֶ��շѹ��ܣ�һ̨������̨����
//	#define FengDuan_BU_
//���Ӷ�ά�빦��,ͬʱָʾCOM��λ��
	#define QR_CODE_COM COM_QR
//�������ܲ��� ͬʱָʾCOM��
	#define _BlueTooth_COM COM6
//�Ƿ�ʹ��FLASH�ļ�ϵͳ
	#define _FASH_FATs_SYS_
	#define Factory_NUM "\x04\x02\x61\x10"


#ifndef KEYB_COM
#define KEYB_COM 4
#endif

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
#define CARD_STUDENT_BUS	0x42			//2ѧ����66 
#define CARD_YOUDAI_BUS		0x43			//9�Ŵ���67  .. 
#define CARD_ZILU_BUS		0x44			//4��Ů�� 68 ����-ѧ���Żݿ�
#define CARD_STUFFC_BUS		0x45			//5����Ա����69  ��Ʊ��
#define CARD_OLDMAN_BUS		0x46			//6���꿨70  ..������ ���꿨

#define CARD_COUPON_BUS		0x47			//7���ſ�71  �˴�������Ա��˾��������Ʊ��..
#define CARD_STUFF_BUS		0x48			//8Ա����72
#define CARD_YOUHUI_BUS		0x49			//9�Żݿ�73  ..
#define CARD_RELATION_BUS	0x4A			//10������74	����-�ͱ����˿�
#define CARD_JILIAN_BU		0x4B			//11���75	����-������Ա��

#define CARD_HANDICAPPED	0x4C			//12�м��˿�76
#define SCARD_HETONG_BUS	0x4D			//��ͬԱ����77 �����ľ��˿�
#define	CARD_PRACTISE_BUS	0x4F			//13ѧԱ��79	����-�ͱ�ѧ����
#define	CARD_JICHA_BUS		0x50			//���鿨80

#define CARD_CONGCI_BUS		87				//��ο� ��Ʊ��(�۶�����ѧ����)

#define CARD_SD_YIXIN		0x56			//86 �۶�������ͨ��
#define CARD_TELECOM_USER   0x58			//�۶������û����Ŀ�����
#define CARD_TELECOM_BUS    0x59			//�۶������ֻ����Ŀ�����

#define CARD_MOBILE_BUS		0x5A			//90 //�ƶ��ֻ�����·���еĿ�����

#define CARD_WH_QIYE_DRIVER 105				//�人��˾����

#define CARD_NORMAL				CARD_NORMAL_BUS				//��ͨA��
#define CARD_STUDENT			CARD_STUDENT_BUS				//ѧ��A��

#define CARD_OLDMAN				CARD_OLDMAN_BUS				//���꿨
#define CARD_STUFF				CARD_STUFF_BUS				//Ա����
#define CARD_COUPON				CARD_YOUHUI_BUS				//�Żݿ�
#define CARD_NORMAL_B			CARD_ZILU_BUS				//��ͨB��


#define CARD_qPBOC_BUS		0x5B			//91 ���п����� [����Щ�ط����п�����д�ɲ�ͬ��]

#define  CARD_FENDUAN_Line	33		//�ֶ���·Ʊ�ۿ�

#endif 


