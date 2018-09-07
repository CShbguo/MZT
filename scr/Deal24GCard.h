
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

extern stcomtime SysTime;
extern unsigned char restore_flag;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned int g24GDisFlash;//24G������ˢ��

unsigned char G24PsamNo[6];
unsigned char gG24CardNo[10];//���ţ�Ǯ������

unsigned char gG24Ver;//��ͷ�汾��ʶ 0xA1 0xA2 0xA3 0xA4(�汾��ûȡ��)
unsigned char gG24VerBuf[21];//��ͷ�汾
unsigned char gG24ReaderErr;//��ͷ�������
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

pboc_debit_capp_purchase stuDebitPurchase;
pboc_sam_init_purchase stuSamInitPurchase;
pboc_init_capp_purchase_res stuInitPurchaseRet;

struct sam_pub
{
	unsigned char kid;
	unsigned char PublisherSign[8];
	unsigned char ReceiverSign[8];
	unsigned char startDate[4];
	unsigned char endDate[4];
}stuSamPubInf_LL;
typea_public_inf stuPubInf;

unsigned char g24GProFlag;//24G��ѯ��ָ�� 0:��Ҫ��ѯ������ ����:����ѯ������,�Ȼ�Ӧ����Ϣ

typedef struct  {
	unsigned char Sec;
	unsigned char CardSno[8];
} stg24card_Delay;
stg24card_Delay g24PCard_delay;//G24��

unsigned char g24PurError;//ˢ����־������Ҫ����ˢ��һ�ſ�
unsigned char g24ErrorCardSno[8];//ˢ��ʱ����Ŀ���
unsigned char g24PSAMDsno[4];//�ն����к�

volatile unsigned char g24ComFlag;//���ڽ��ձ�־
unsigned char g24ComRBuf[500];
unsigned int g24ComRindex;
unsigned short com1Rlen;//�������ݵĳ��ȡ�

