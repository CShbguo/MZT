
#ifndef _FUZHOU_HEAD_
#define _FUZHOU_HEAD_



//************����****************/
#define Ver_city "FuZhou"
extern int getCardtypecommon_FuZhou(char *cardD, unsigned char type);
extern void getFixPrice_FuZhou(void);//���ݵ�����·����д���ڳ�������
extern void setAudio_FuZhou(void);
//>>>>>>>>>>>>����>>>>>>>>>>>>>>>>

//----��------
#define HandListlen 1
extern const unsigned int gHandList[HandListlen+1];

//����

#define SYS_HEAD_STR	"����ͨ��"
#define SYS_HEAD_STREn	"  SLZR  "
#define SYS_cityName "FZ"
#define PCityCode "\x00\x27"
#define CLOUD_BUSSINESSNO "\x99\x98"	//�̻���

#define GJ_IP 0x64646464
#define GJ_PORT 8819
#define NETAPN "CMNET"
//-----------����Ϊ�豸�������á�-------
	#define _New_Bu_mode_
	//SLZR���Ѷ���
//  #define CDMA_MC323_
//  M50Ҳ���� MG323���壬ʹ��STGPRSM50Lib.lib
//	#define GPRS_MG323_
//	#define OTHER_GPRS
	//���ô���ת���ڣ�ʹ�õ���͸������������DVR��ӡ���COM2RJ45_  �� OTHER_GPRS
//	#define COM2RJ45_
//	#define WIFI_TLH_

//	//CDMA 3gģ��  CEM630 3gģ��
	#define CDMA_3G
//ȫ��ͨ4G
//	#define ALL_4G
	//����ʹ��
//	#define IP_OF_SLZR_	
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
	#define NORM_DLY_TIME 1

//����Ʊ��Ʊ��һ��Ҫ��
//	#define New_Times_Card__
	#define line_older_
	//����ΪCPU��,Ӧ�ò���2.4G��
//  	#define BUS_CPU_

	//����Ϊ���п�
	#define qPBOC_BUS	
	//�������շ�״̬
	#define _BU_NO_STOP_

//�����û��������ʹ�0��ʼ����ͨ��Ϊ0
	#define Card_typeM_Min_is0_
//�ʼ��ȡPSAM��Կ�ķ�����ֻ�й�˾��PSAM����Ч
	#define _old_PSAM_GET_KEY_
	//********************************
	//#define _Money_zero_

	//û�и��ӷѹ���
	#define NO_PlusSta_
	//ѹ����ĸ�豸��ʹ�������
//	#define ZBCD_DRICENO	
//-----------����Ϊ�豸�������á�-------

#ifndef KEYB_COM
#define KEYB_COM 4
#endif

///////////////////////////////���ֿ��Ķ���/////////////////////////////////
#define CARD_DATA_GATHER	61				//���ݲɼ���
#define CARD_DEV_SET		62				//�豸���ÿ�
#define CARD_LINE_SET		63				//��·���ÿ�
#define CARD_NUMBER_SET		64				//�������ÿ�
#define CARD_DELAY_SET		65				//��ʱ��
#define CARD_TOLL_START		66				//�շ�������
#define CARD_TIME_SET		67				//ʱ�����ÿ�
#define CARD_LEV_BUS		68				//������
#define CARD_ARV_BUS		69				//��վ��
#define CARD_DRIVER_BUS		70				//˾����
#define CARD_CLERK_BUS		71				//վ��Ա��
#define CARD_AIR_BUS		72				//�յ���
#define CARD_MONTH_STA		73				//��Ʊ������
#define CARD_WHITE_BUS		50				//�׿�	
//---------����
#define CARD_NORMAL_BUS		0				//��ͨ��
#define CARD_STUDENT_BUS	1				//ѧ����
#define CARD_SX_BUS			2				//���ǿ�
#define CARD_STUFF_BUS		3				//Ա����
#define CARD_YH_BUS			4				//�Żݿ� //������Ʊ��(��������)
#define CARD_YF_BUS			5				//�ŷ��� //������Ʊ��(���ݽ���)
#define CARD_OLDMAN_BUS		6				//���꿨 ���Ŀ�-�м���(����)
#define CARD_Love_BUS		7				//���Ŀ�
#define CARD_ZILV_BUS		8				//��Ů�� ���ڿ������ݣ�
#define CARD_SCHOOL_BUS		9				//У԰�������ݣ�

#define CARD_NORMAL				CARD_NORMAL_BUS				//��ͨA��



#endif




