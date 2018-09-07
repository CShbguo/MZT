
#ifndef __CPU_CCARD_FILE_HEAD_
#define __CPU_CCARD_FILE_HEAD_

#include "string.h"
#include "stdio.h"	  
//#include "Board.h"

#include "..\include\include.h"  
//#include "public.h"
#include "Gszct.h"
//#include "timer_isr.h"
#include "Mydes.h"
#include "E_Gmobile.h"

extern unsigned int get_s_sum1(unsigned char mode);

extern unsigned int s_sum1;
extern unsigned int a_sum1,dis_sum2;
extern stcomtime SysTime;
extern unsigned char restore_flag;
extern fFlagVary_1 F_FLAG_VARY;

/* ���������ļ� --- 0015 file */
typedef struct
{
	unsigned char cPublisherSign[8];	/* ��������ʶ */
	unsigned char cAppType;				/* Ӧ�����ͱ�ʶ */
	unsigned char cCardType;			/* ������ʶ */
	unsigned char cApp_sn[10];			/* Ӧ�����к�(���ұ�ʶ(1),���貿��ʶ(1),���д���(2),Ӧ�ô���(2),��������ˮ��(4) */
	unsigned char cIssueDate[4];		/* �������� */
	unsigned char cValidDate[4];		/* ��Ч���� */
	unsigned char cCardVer;				/* ���汾�� */
	unsigned char cAppVer;				/* Ӧ�ð汾�� */
	unsigned char cRFU[2];				/* ���� */
}typea_public_inf;
typedef struct
{
	unsigned char cPublisherSign[8];	/* ��������ʶ */
	unsigned char cAppType;				/* Ӧ�ð汾�� */
	unsigned char cCitycode[2];			/* ���д���	*/
	unsigned char cCardType;			/* ��Ӧ������ */
	unsigned char cApp_sn[10];			/* Ӧ�����к� 5��00H�ֽ� + 1����Ƭ�����ֽ�+ 4����ˮ�ֽ� ����Ϊ����5���ֽ� */
	unsigned char cIssueDate[4];		/* Ӧ���������� */
	unsigned char cValidDate[4];		/* ��Ч���� */
	unsigned char cSing;				/* �Ƿ���� */
	unsigned char cYaJin[2];			/* Ѻ�� */
	unsigned char cMinremain[2];		/* ���������� */
	unsigned char cUseFlah;				/* ʹ�ñ�� */
	unsigned char cMaxPur[4];			/* ������ѽ�� */
	unsigned char cLinezf;				/* �����˻���� */
	unsigned char cRFU[9];				/* ���� */
}typea_public_inf_zaozuang;				//��ׯCPU����Ϣ�ṹ

typedef struct
{
	unsigned char cPublisherSign[2];	/* ��������ʶ */
	unsigned char cCitycode[2];			/* ���д���	*/
	unsigned char changyecode[2];		//��ҵ����
	unsigned char cref[2];				//RFU��Ԥ����
	unsigned char cQiYongflag;			//���ñ�־
	unsigned char cAppType;				/* Ӧ�ð汾�� */
	unsigned char ctemp[2];				//����

	unsigned char cApp_sn[8];			/* Ӧ�����к� 5��00H�ֽ� + 1����Ƭ�����ֽ�+ 4����ˮ�ֽ� ����Ϊ����5���ֽ� */
	unsigned char cIssueDate[4];		/* Ӧ���������� */
	unsigned char cValidDate[4];		/* ��Ч���� */
	unsigned char cMinremain[2];		/* ���������� */
}typea_public_inf_wuhang;				//�人CPU����Ϣ�ṹ

typedef struct
{
	unsigned char cBalance[4];			/* ����Ǯ����� */
	unsigned char cSnq[2];				/* ����Ǯ���������к� */
	unsigned char cOverdraftLimit[3];	/* ͸֧���� */
	unsigned char cKeyVer;				/* ��Կ�汾 */
	unsigned char cKeyEn;				/* ��Կ��ʶ */
	unsigned char cRand[4];				/* ����� */
}pboc_init_capp_purchase_res;
typedef struct
{
	unsigned char cRand[4];				/* ����� */
	unsigned char cSnq[2];				/* ����Ǯ���������к� */
	unsigned char cMoney[4];			/* ���׽�� */
	unsigned char cType;				/* �������� */
	unsigned char cDateTime[7];			/* ��������ʱ�� */
	unsigned char cKeyVer;				/* ��Կ�汾 */
	unsigned char cKeyEn;				/* ��Կ��ʶ */
	unsigned char cKeyFactor[8];		/* 8��Կ��ɢ���� */
	unsigned char cPublisherSign[8];		/* 8��Կ��ɢ���� */
	unsigned char cReaderMAC[12];//��ͷMAC,����PSAM��У���ͷ�Ƿ�Ϸ�
}pboc_sam_init_purchase;

typedef struct
{
	unsigned char cTermSnq[4];			/* �ն˻�������� */
	unsigned char cDateTime[7];			/* ��������ʱ�� */
	unsigned char cMac[4];				/* ����MAC�� */
}pboc_debit_capp_purchase;

extern typea_public_inf stuPubInf;

typedef struct
{
	unsigned char FCcode[2];	//����������
	unsigned char citycode[2];	//���д���
	unsigned char tradcoed[2];		//��ҵ����
	unsigned char money_type;		//Ӧ�����ͱ�ʶ 03������Ǯ����
	unsigned char GS_QY;			//��˾�Լ������ñ�־
	unsigned char QYflag;			//Ӧ�����ͱ�ʶ�����ñ�־ 01���ã�1
	unsigned char YYVer;			//Ӧ�ð汾1
	unsigned char FYFlag[2];		//��ͨ��ʶ2
	unsigned char YYNO[8];			//Ӧ�����к�8
	unsigned char StartDate[4];		//Ӧ���������ڣ�YYYYMMDD�� 4
	unsigned char EndDate[4];		//Ӧ����Ч���ڣ�YYYYMMDD�� 4
	unsigned char Zicard_catalog;	//�ӿ���
	unsigned char reserve[1];			//Ԥ��2
	unsigned char card_catalog;			//����ͨ����
//	unsigned char MAC[4];		//������
//	unsigned char reserve2[6];		//Ԥ��
}stJSBF15;


typedef  struct 
{
	unsigned char kid;
	unsigned char PublisherSign[8];
	unsigned char ReceiverSign[8];
	unsigned char startDate[4];
	unsigned char endDate[4];
}sam_pub;
//�����Ǻ�24G�ֻ������õı���
// #ifdef NO_MOBILE_24G_
// unsigned char G24PsamNo[6];
// unsigned char g24PSAMDsno[4];//�ն����к�
// typea_public_inf_zaozuang stuPubInfZaoZuang;
// pboc_debit_capp_purchase stuDebitPurchase;
// pboc_sam_init_purchase stuSamInitPurchase;
// pboc_init_capp_purchase_res stuInitPurchaseRet;
// #else
extern unsigned char G24PsamNo[6];
extern unsigned int g24GDisFlash;//24G������ˢ��
extern unsigned char g24PSAMDsno[4];//�ն����к�
extern typea_public_inf_zaozuang stuPubInfZaoZuang;
extern pboc_debit_capp_purchase stuDebitPurchase;
extern pboc_sam_init_purchase stuSamInitPurchase;
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
// #endif
extern unsigned char gPsamErrorFlag;//PSAM�Ƿ����
extern unsigned int g24GDisFlash;//24G������ˢ��
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
extern void end_close_card(unsigned char moded);
#endif

