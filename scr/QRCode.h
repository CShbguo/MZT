

#ifndef _SLZR_QR_
#define _SLZR_QR_

#define TACLEN			5      //tac����
#define TAC_DATA_LEN    29    //TAC���ݳ���


typedef struct
{
	unsigned char UIID[3];//�˻����з�ID
    unsigned char UserID[8];//�˻�ID
	unsigned char DealType[1];			//00��������01������բ02������բ����
	unsigned char EenTime[4];// ���� ʱ�����16�����ַ��������ֽ���ǰ����д
	
}st_QR_BLK;  //16 �ֽ�



typedef struct  
{
	unsigned char TagCode[2];			//��ʶ��ASC  �̶�Ϊ SL
	unsigned char CompanyCode[2];		//��Ӫ������BCD
	unsigned char AccountNo[10];		//�ʺű���BCD
	unsigned char Accounttype;			//�˻�����Hex
	unsigned char Counter[4];			//������Hex
	unsigned char ExpiredTime[7];		//����ʱ��BCD
	unsigned char MaxDealMoney[4];		//���ʽ��������Hex
	unsigned char Balance[4];			//���Hex
	unsigned char TicketMAC[4];			//Ʊ��Mac Hex
	unsigned char TacVersion[4];		//��Կ�汾Hex 
	unsigned char TAC[4];				//TAC	Hex	
}Card_QRC_infor;


typedef struct
{
	unsigned char rCardDealNumb[4];		//�豸��ˮ��0-3
	unsigned char rSerialNum[4];		//���к�4-7
	unsigned char rCardType;			//����8
	unsigned char rDealType;			//��������9
	unsigned char rPublishNumb[10];		//�ʺű��10-19
	unsigned char rAfterMoney[4];		//ԭ��20-23
	unsigned char rDealMoney[3];		//���׽��24-26
	unsigned char rDealTime[7];			//����ʱ��YYYY/MM/DD/HH/MM/SS 27-33
	unsigned char rRemain;				//������M1��˾�����Ÿ�λ34
	//	unsigned char rTradCode[2];			//��ҵ���� ����������ͬ��Կ�Ĳ�ͬ���л�Ӧ��-------
	unsigned char rDriverNo[4];			//˾������35-38
	unsigned char rCityCode[2];		    //��Ӫ������39-40
	unsigned char rLineNo[2];			//��·��41-42
	
	unsigned char rCounter[4];			//������43-46
	unsigned char rTicketMac[4];		//Ʊ��Mac47-50
	unsigned char rTacVer[4];			//TAC�汾51-54
	unsigned char rDeviceNo[4];			//�豸��55-58
	unsigned char rTAC[4];				//У����59-62
	unsigned char rReseverX;			//���� 00��ͨ����׼�� 01��ͨ�������Ĺ�˾�������㣩63
	unsigned char rProVer[2];			//����汾��64-65
	unsigned char rEwmData[62];			//��ά������66-127
}DEALRECODE_QRC;

//�����Ķ�ά���¼��ʽ
typedef struct{
	unsigned char rQrcrecHead[6];		//��¼��ʶ��0-5
	unsigned char rQrcrecHead2[2];		//��¼��ʶ�������ϴ�ʱ������¼��ʶ6-7
	
	unsigned char rCardDealNumb[4];		//�豸������ˮ8-11
	unsigned char rDevSerial[4];		//�豸Ψһ���к� 4  12-15
	unsigned char rCardType;			//���� 16
	unsigned char rDealType;			//��������  17
	unsigned char ruserTimes[2];		//ʹ�ô��� ˾���Ÿ�λ  [�ֶ� ȫ��ʱ��] ������пصļ�¼�����湲18�ֽ�Ϊ��Ƭ�����֤�� 18-19
	unsigned char rAfterMoney[4];		//ԭ��  20-23
	unsigned char rDealMoney[3];		//���׽�� 24-26
	unsigned char rDealTime[7];			//����ʱ��YYYY/MM/DD/HH/MM/SS 27-33
	unsigned char rDriverNo[4];			//˾������ 34-37
	unsigned char rDeviceNo[4];			//�豸�� 38-41
	unsigned char rLineNo[2];			//��·�� 42-43
	unsigned char rProVer[2];			//����汾�� 44-45
	unsigned char rTicket[2];			//Ʊ�� 46-47
	unsigned char rQRCdat[204];			//��ά����Ϣ�� 48-251
	unsigned char rTAC[4];				//  252-255
}stOtherQRCrecord;


extern void QR_Code_Test_4500R(void);
extern void QR_Code_init_par(unsigned char Ccom);


unsigned char saveTACInfo(unsigned int flash_Addr);

#define QR_BLK_MAX 1000
extern unsigned int QR_BLK_Ver;	//���ͨ��ά��������� �汾
// extern unsigned int gGprs_Check_QR_BLK;	// 15�������Ҳ�ѯһ�κ����� 
unsigned char save_QR_blkVer(unsigned int a);
unsigned int get_QR_blkVer(void);

//1 ������  0 ����  uiid  mode 0��������  1,��������
unsigned char find_BLK_QR(unsigned char *UIID,unsigned char *UserID ,unsigned char mode);
int write_BLK_QR(unsigned char *filehead,unsigned char *wbuf,unsigned int datlen);

//void show_QR_Ver(void);
//ȡ �汾 
unsigned char LoadfileVer(unsigned int flash_Addr,unsigned char *outver);

unsigned char saveSPKInfo(unsigned int flash_Addr);

void show_QR_Ver(void);
//�˵����ã�����ÿ�ζ�����
void set_QR_para(void);

#endif




