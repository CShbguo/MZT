#ifndef _CPUCARDJTB_H_
#define _CPUCARDJTB_H_

#define  ON_BUS			3	//Ǯ���ϳ�
#define	 OFF_BUS		4	//Ǯ���³�
#define  COVER_BUS		8 //Ǯ����Ʊ
#define  ON_Nomal		6//��ͨ����

#define FeRC_Write	1
#define FeRC_Dlelt	2
#define FeRC_Check	3

#define NO_START		0x81//δ����
#define NO_Ech			0x83//���ǻ�����
#define NO_APP			0x84//δ����
#define PursAgian		0x85//��Ʊ

#define LocakCard		0x86//���ؿ�		//���ڿ�
#define YiDiCard		0x87//��ؿ�		//ʡ�⿨
#define autoregionCard  0x88//��������

#define ID_REC_HJY_REC		100			//�ҽ��׼�¼����
#define RECORD_JTB_LEN		128

#define WHITELEN		 1000
#define WHITE_SNO_LEN		4 
#define WTACKSUM 16000		//16������

//�ڳ�վ�ϳ�������2 6  �����������ϳ�
#define BUS_Star			(1<<0) //1 3 5 7ˢ������ 1 0XFE����ˢ�˷���	��վҪ���
#define BUS_InStation		(1<<1) //�ڳ�վ�� 2 5 6�ڳ�վ  0XFD
#define BUS_ON				(1<<2) //�б�ʶΪ 4 �ϳ���¼ 0XFB
#define BUS_TES				(1<<7) //���Լ�¼

#define BUS_CL_Star			0xFE //���������
#define BUS_CL_InStation	0xFD//���ڳ�վ
#define BUS_CL_ON			0xFB//�³�
#define BUS_CL_TES			0x7F//���Լ�¼���


typedef struct
{
	unsigned char cRand[4];				/* ����� */
	unsigned char cSnq[2];				/* ����Ǯ���������к� */
	unsigned char cMoney[4];			/* ���׽�� */
	unsigned char cType;				/* �������� */
	unsigned char cDateTime[7];			/* ��������ʱ�� */
	unsigned char cKeyVer;				/* ��Կ�汾 */
	unsigned char cKeyEn;				/* ��Կ��ʶ */
	unsigned char cPublisherSign[8];	/* 8��Ӧ�����кſ��ź�8�ֽ�*/
	unsigned char cKeyFactor[8];		/* 8������������ */
	unsigned char cReaderMAC[12];		//��ͷMAC,����PSAM��У���ͷ�Ƿ�Ϸ�
}pboc_sam_init_purchaseJTB;
typedef struct//����ֵ��С���ֵ� һ��ΪС��ģʽ
{
	//29�����ǵ���ˢ��������
	unsigned char rPublishNumb[10];		//BCD���п���ˮ�ſ���
	unsigned char rCardType;			//����
	unsigned char rCardCand;			//����:������  M11��  2 CPU�� 11 �ֽ�
	unsigned char rSAMxulie[4];			//PSAM�ն˽������к� (�������ƽ̨ �� �����豸�Զ����ɵ���ˮ)
	unsigned char rPSAMNO[6];			//PSAM����72  BCD
	unsigned char rDeviceNo[4];			//�豸������ �������豸�ţ����ƺţ�(HEX)
	unsigned char rDealType;			//��������	HEX
	unsigned char rPiaoJia[2];			//Ʊ�� ���
	//20
	unsigned char rMoneyDealSum[2];		//Ǯ���ۼƽ��״���    Ӧ�ý��׼����� 9F36HEX
	unsigned char rDealMoney[3];		//���׽�� HEX 9F02
	unsigned char rAfterMoney[4];		//���23���
	unsigned char rDealTime[7];			//����ʱ��YYYY/MM/DD/HH/MM/SS 22���ֽ� 
	unsigned char rCPUTAC[4];			//CPU������֤�� *����CPU�����ص���֤��
	//20//
	unsigned char rDriverNo[4];			//˾���� ���
	unsigned char rLineNo[3];			//��·�� ���
	unsigned char rGPSzhandian;			//վ����
	unsigned char rGPSfangxian;			//GPS���� ����/����
	unsigned char rFactorFlg[8];		//�³�������ʶ
	unsigned char rCityCode[2];			//���д���
	unsigned char rHUIJIAOYI;			//�ҽ��� 0���� 1������ 63
	//43//��¼�ļ����ϱ����� (�ϳ���¼)
	unsigned char rDealTimeOLD[7];		//�ϳ�����ʱ��YYYY/MM/DD/HH/MM/SS* BCD
	unsigned char rPSAMNOOLD[6];		//�ϱʽ����ն� BCD ����PSAM����
	unsigned char rDriverNoOLD[4];		//�ϳ�˾���� 
	unsigned char rLineNoOLD[3];		//�ϳ���·�� 
	unsigned char rGPSzhandianOld;		//�ϳ�վ����
	unsigned char rCityCodeOLD[2];		//�ϳ����д���
	unsigned char rFactorFlgOLD[8];		//�ϳ�������ʶ
	unsigned char rProviceOLD[3];		//�ϳ��ĳ��� һ������+һ����ĸ(��ʱ����)
	unsigned char rDeviceNoOLD[4];		//�ϳ����ƺ�(HEX)
	//......................................................
	unsigned char rProvice[3];			//�³��ĳ��� һ������+һ����ĸ(��ʱ����)
	unsigned char rZwyNum[4];			//�³�վ��Ա���
	unsigned char rBusTC[2];			//�˴�
	//����һ��104�ֽ�
	//
	//	QPBOC_RC	  rQpboc_Rc;			//QPBOCר������ 70���ֽ�
	unsigned char rBusFlag;				//��ͬ״̬��ˢ����¼ 
	unsigned char reasea[5];			//����
	unsigned char rProVer[2];			//����汾��
	unsigned char rTTTT[4];				//CRCУ���ĸ��ֽ�AA
	
	
}DEALRECODE_JTB;

typedef struct{
	unsigned char falg[4];//��Ч��ʶ
	unsigned char RCE[128];//��¼
	unsigned char CRC[4];	//CRC
}FeUnKnowREC;
/* ��¼ID��ʶ */
// 01����
// 02 �����ͨ
// 03 ����
// 04�������г�
// 05ͣ���շ�
// 06���⳵
// 07��;����	
// 08�ֶ�
// 09�Ǽ���·
// 10���ٹ�·
typedef struct
{
	unsigned char RcID;					/* BCD��¼ID��ʶ */
	unsigned char RcLen;				/* 	��¼���� �̶�1E*/
	unsigned char LockFlag;				/* ������־ �̶�0x00*/
	unsigned char Qtype;				/* BCD��չ����1-99,1��������Ʊ2ѧ���Ż�......*/
	unsigned char AppAid[8];			/* ��չӦ��AID */
	unsigned char YHtype;				/*BCD ��չ�Ż����� 0 ����Ż�  1�����Ż� */
	unsigned char StarData[4];			/* BCD��չӦ���������� */
	unsigned char EndData[4];			/* BCD��չӦ�������� */
	unsigned char YearMoney[3];			//hex��չӦ�����
	unsigned char ResearX[8];			//Ԥ��
	
}stJTBA6;		//��ͨ��Ǯ����06�ļ��ṹ      32byte
typedef struct
{
	unsigned char Qtype;				/*BCD ��չ����1-99,1��������Ʊ2ѧ���Ż�......*/
	unsigned char Ctype;				/* BCD�Ǵ����� 0 Ǯ���ƴ� 1�ļ��ƴ�*/
	unsigned char Ttype;				/* BCDʱ������ 0 ��  1 ��*/
	unsigned char TimeCycle;			/* HEXʱ������ */
	unsigned char RateTimes[2];			/* hex�������  */
	unsigned char ResearX[10];			//Ԥ��
	
}stJTBC6;		//��ͨ�������ļ���06�ļ��ṹ      16e

typedef struct
{
	unsigned char RcID;					/* ��¼ID��ʶ ���¼��һ�� */
	unsigned char RcLen;				/* 	��¼���� �̶�0x0c*/
	unsigned char LockFlag;				/* ������־ �̶�0x00*/
	unsigned char StarData[4];			/* �������� */
	unsigned char EndData[4];			/* �������� */
	unsigned char Times[2];				//ʣ�����
	unsigned char ResearX[2];			//Ԥ��	
}stJTBC7;		//��ͨ�������ļ���07�ļ��ṹ      15e

typedef struct
{
	unsigned char cPublisherSign[8];	/* ����������ʶ */
	unsigned char cAppType;				/* Ӧ�����ͱ�ʶ 02 ���������� */
	unsigned char cStarFlag;			/* ��������Ӧ�ð汾 *���ñ�ʶ*/
	unsigned char cApp_sn[10];			/* Ӧ�����к�*/
	unsigned char cIssueDate[4];		/* Ӧ���������� */
	unsigned char cValidDate[4];		/* Ӧ����Ч���� */
	unsigned char cFCI[2];				/* ���������Զ���FCI���� */
}stJTB15;		//��ͨ��95�ļ��ṹ      30byte


typedef struct
{
	unsigned char cCardType;			/* �����ͱ�ʶ */
	unsigned char cStuffcode	;		/*����ְ����ʶ*/
	unsigned char cName[20]	;			//�ֿ�������
	unsigned char cCredent[32];			/* ֤���� */
	unsigned char cCretype;				//֤������
}stJTB16;		//��ͨ��96�ļ��ṹ     55byte
typedef struct
{
	unsigned char intercode[4];			/* ���ʴ��� */
	unsigned char provcode[2];			/*ʡ������*/
	unsigned char Citycode[2]	;		/*���д���*/
	unsigned char car_type[2];			//��ͨ����  00�ǻ�ͨ�� 01 ��ͨ��
	unsigned char card_catalog;			//��������
	unsigned char reser[49];			//Ԥ��
}stJTB17;		//��ͨ��97�ļ�:������Ϣ�ļ�  60byte

typedef struct
{
	unsigned char rEPsearl[2];			/* EP���������ѻ��������к� */
	unsigned char rOverdraft[3];		/*͸֧�޶�*/
	unsigned char rPursPrice[4];		//���׽��
	unsigned char rPursType	;			/*�������ͱ�ʶ*/
	unsigned char rPSAMnum[6];			//�ն˻����	
	unsigned char rPursTime[7];			//����ʱ��
}stJTB18;		//��ͨ��98�ļ�:  23

typedef struct//w�ļ��� 2702   128�ֽ�
{	
	unsigned char rID[2];				//1-2���ױ�ʶ  2702  ��������Ӧ����Ϣ
	unsigned char rlen;					//3��¼����
	unsigned char rOk;					//4��Ч��ʶ  �̶�01
	unsigned char rpursFlag;			//5���ױ�ʶ1 ���÷��Ƿֶο۷�/��������    2 Ԥ��Ȩ����
	unsigned char rLock;				//6��Ӧ�ñ�ʶ 0û���� 1����
	unsigned char rPursSearl[8];		//7-14������ˮBCD
	unsigned char rPursState;			//15����״̬��ʶ1	00����ʼֵ 01���ϳ� 02���³�
	unsigned char rCityCodeON[2];		//16-17�ϳ����д���
	unsigned char rFactoryOn[8];		//18-25�ϳ���������
	unsigned char rYunYinCode[2];		//26-27-�ϳ����д���
	unsigned char rLineNum[2];			//28��29�ϳ���·��	2	HEX�����ֽ���ǰ��
	unsigned char rStationOn;			//30�ϳ�վ��
	unsigned char rBusNum[8];			//31-38�ϳ�������Ascii
	unsigned char rPsamOn[8];			//39-46�ϳ��ն˺�BCD
	unsigned char rPursTimeOn[7];		//47-53�ϳ�ʱ��
	unsigned char rPriceMax[4];			//54��57	��ע���4������Ʊ׷�� HEX�����ֽ���ǰ��
	unsigned char rDirection;			//58����
	unsigned char rCityCodeOff[2];		//59-60�³����д���
	unsigned char rFactoryOff[8];		//61-68�³���������
	unsigned char rYunYinCodeOff[2];	//69-70�³���Ӫ����HEX
	unsigned char rLineNumOff[2];		//71��72�³���·��	2	HEX�����ֽ���ǰ��
	unsigned char rStationOff;			//73�³�վ��
	unsigned char rBusNumOff[8];		//74-81�³�������Ascii
	unsigned char rPsamOff[8];			//82-89�³��ն�
	unsigned char rPursTimeOff[7];		//90-96�³�ʱ��
	unsigned char rPrice[4];			//97-100���׽��
	unsigned char rMileage[2];			//101-102�˳���� HEX
	//30								//103-128Ԥ��26

	unsigned char rCiytCodeY[2];		//103-104Ԥ����Ϣ���д���
//	unsigned char rlenY;				//105Ԥ����Ϣ���� 
	//16
	unsigned char rDriverNum[4];		//106-19�ϳ�˾����
	unsigned char rCwuyuanNum[4];		//110-113 �ϳ�����Ա���
	unsigned char rTangCi[2];			//114-115�˴�
	unsigned char rTimeMax[2];			//116-117�ʱ��,��λ ����
	unsigned char rName[10];			//119-128վ������Ascii  �������
	unsigned char rCH[2];				//118 Ԥ��
	
//	unsigned char rResear[8];			//Ԥ����Ϣ����
}stJTB1A;		//��ͨ��1A�䳤��¼�ļ��ṹ     55byte

typedef struct//1E   48�ֽ�
{
	unsigned char eDealType;	//00��ʼֵ  01��EC��վ/�ϳ�/�����ȣ�02��EC��վ/�³�/�����ȣ�03 EP�� 04 EP��վ/�³�/������05��EC�������ѣ�   
								// 	06��EP�������ѣ�07��EC��Ʊ08��EP��Ʊ��09��EC��ֵ��0A��EP��ֵ������Ԥ����						
	unsigned char eDeviceNum[8];//�ն˱��BCD
	unsigned char eTracode;		//10��ҵ���� 01���� 02���� 03����ˮ�Ͽ��� 04�������� 05�������� 06�������г� 07ͣ���շ� 
								//08��;���� 09�ֶ� 0A�Ǽʸ�����· 0B�� 0C���ٹ�·�շ� 0E�н����������٣� 0F��12 ���淶Ԥ��
	unsigned char eLineNUm[4];	//11-14��·�� BCD
	unsigned char eRF[3];		//15-17RF
	unsigned char eDmoney[4];	//18-21���׽�� ���HEX
	unsigned char eYuE[4];		//22-25���
	unsigned char eDealTime[7]; //26-32��������ʱ��
	unsigned char eCityco[2];	//33-34�������д���
	unsigned char eFactoryco[8];//35-42��������
	unsigned char eErrPriceMax[4];//43-46���������������
	unsigned char eTimeMax[2];	//47-48�ʱ��,��λ ���Ӵ�



	unsigned char N[6];			//Ԥ��
}stJTB1E;		//1E 48�ֽ�

typedef struct
{
	unsigned char kid;
	unsigned char PublisherSign[8];
	unsigned char ReceiverSign[8];
	unsigned char startDate[4];
	unsigned char endDate[4];
}sam_pubJTB;
extern stJTB1A	JTB_1A;//1A�ļ��ṹ
extern stJTB15	jtb15file;//15
extern stJTB16	jtb16file;//16
extern stJTB17	jtb17file;//17
extern stJTB18	jtb18file;//18
extern stJTB1E	JTB_1E;//1A�ļ��ṹ
extern pboc_sam_init_purchaseJTB stuSamInitPurchaseJTB;
extern unsigned char PsamNumJTB[6];
extern unsigned char getDafang_CPUKey(unsigned char *Sdata, unsigned char *Rkey);


extern unsigned char  operat_Black(void);


#endif
