#ifndef _CPUCARDJSB_H_
#define _CPUCARDJSB_H_




typedef struct
{
	unsigned char rPublishNumb_H[4];	//CPUӦ�����к�ǰ4�ֽ�
	unsigned char rPublishNumb_L[4];		//���п���ˮ��
	unsigned char rDeviceNo[4];			//�豸��
	unsigned char rCardType;			//����
	unsigned char rDealType;			//��������
	
	unsigned char city_code[2];			//���д���
	unsigned char rAfterMoney[4];		//��
	unsigned char rDealMoney[3];		//���׽��
	unsigned char rDealTime[7];			//����ʱ��YYYY/MM/DD/HH/MM/SS
	unsigned char rKeyEn;				//�㷨��ʶ
	unsigned char rdealtpye_JSB;			//���貿���ױ�ʶ
	unsigned char rDriverNo[4];			//˾����
	unsigned char rPSAM_DealNumb[4];		//M1����������ˮ�� CPU�ն˽������
	unsigned char rLineNo[2];			//��·��
	unsigned char rST_Serial[4];		//stm �������к�
	unsigned char record_Flag[2];	//�¼�¼��ʶ��55AA
	unsigned char rPSAM_NO[6];		//psam����
	unsigned char rBussinessNo[2];		//�̻����
	unsigned char rCPUCard_DealNumb[2];	//CPU��Ƭ�������
	unsigned char rProVer[2];			//����汾
	unsigned char rTAC[4];				//У����
}Recode_CPU_JSB;



typedef struct 
{
	unsigned char city_code[2];
	unsigned char card_serial[8];
	unsigned char ckeyEn;		//�㷨��ʶ
	unsigned char deal_money[4];
	unsigned char deal_type;		//=6
	unsigned char psam_num[6];		//8050�ļ�����6-11
	unsigned char deal_serial[4];	//8070���ص�1-4
	unsigned char card_deal_serial[2];
	unsigned char deal_time[7];
	unsigned char TAC[4];
}jsb_rec;






unsigned char CPUPsamInitialize_JSB(void);
unsigned char CPUcardType_JSB(void);
unsigned int load_white_list(void);

unsigned char CPUDealCard_JSB(unsigned char mode, unsigned char cool);

//�µļ�¼��ʽ  64 �ֽ�
void BuildRecorde_CPU_JSB(unsigned char delType, unsigned char *recBuf);
//��ʾ64�ֽڼ�¼
unsigned char CPU_JSB_history(unsigned char *dat,unsigned int Index);

#endif
