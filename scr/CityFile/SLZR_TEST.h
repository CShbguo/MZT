#ifndef _SLZR_TEST_HEAD_
#define _SLZR_TEST_HEAD_

//----��------
#define HandListlen 0
extern const unsigned int gHandList[HandListlen+1];

#define SYS_HEAD_STR	"����ͨ��"
#define SYS_HEAD_STREn	"  SLZR  "
#define PCityCode "\x00\x27"

#define CLOUD_BUSSINESSNO "\x99\x98"
#define SYS_cityName "SL"


#define GJ_IP 0x64646464
#define GJ_PORT 8819
#define NETAPN "CMNET"

//��˾ƽ̨����IP:202.96.137.46   ��ͨIP:58.251.128.49
// #define GJ_IP 0x2E8960CA
// #define GJ_PORT 8819
//�˿� �ƿ�:8818  ����:8819
//Զ�̣�202.96.137.46:2012


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
//	#define BUS_PRO_JSB
	//Cloud
	#define BUS_Cloud_ 
	#define _M1KEY_CLOUD_
	//ʹ��9996�̻�,���̻��Ǻ���������ʾ�á�
	//#define CLOUD_9996_
	//��������·�������õ�����
	//#define AutoAudio_line
	//����ΪCPU��,Ӧ�ò���2.4G��

//SLZR���Ѷ��� 
	#define BUS_CPU_

	#define CPU_JSB		//ס����������ͨ
	// #define BUS_PRO_JTB	//��ͨ��������ͨ

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
//	#define _CPU_24G_ 
	//����ʢ������2.4G��ͷ��ʹ��M1���Ĺ��ܣ�ֻ�з�������Ǯ�����͹�����Ϣ��,��Կʹ������ƽ̨�ķ�����
				//CPU��ʹ�õĹ�˾��PSAM�����������Կ
//	#define _XMSHDZ_24G_
//	#define G24Com COM5
//��ESF3000E  COM6
//	#define G24Com COM6
	//����Ϊʹ��3DES������Կ
	//#define _M1KEY_3DES_
	//����Ϊ�п�Ѷ��2.4G��ͷ
//	#define ZKXL_24G_
//�ڹ�˾CPU����ȡ������
//	#define qPBOC_CPU_CARDTYPE_
	//����Ϊ���п�
//	#define qPBOC_BUS
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
	//#define _TTSSound_enable_
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
	//�������ܲ��� ͬʱָʾCOM��
	#define _BlueTooth_COM COM6
	//�Ƿ�ʹ��FLASH�ļ�ϵͳ
	#define _FASH_FATs_SYS_
	#define Factory_NUM "\x04\x02\x61\x10"

//-----------����Ϊ�豸�������á�-------

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


