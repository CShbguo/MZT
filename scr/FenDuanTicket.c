/*
�ֶ��շ���س���
ʱ�䣺2012-1-16
��д��LiKS.

  2015.10.14�ո���Э�飬������β��"_QM"����ֻ��ǰ�Ż��õĺ���������β��"_HM"��ֻ�Ǻ��Ż�ʹ�õĺ�����"_ALL"���Ƕ����õ��ĺ�����
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "..\include\include.h"
#include "szct.h"

#ifdef FengDuan_BU_

#define FDCMD_VERCX			0x01	//�汾��ѯ
#define FDCMD_VERCX_ECHO	0x81	//�汾��ѯ��Ӧ��

#define FDCMD_GPSINFO		0x02	//��ѯǰ����Ϣ
#define FDCMD_GPSINFO_ECHO	0x82	//ǰ��Ӧ����Ϣ

#define FDCMD_DOWNPar		0x03	//�������ݣ���������·��Ϣ��Ʊ����Ϣ������ļ�����Ҫ�������еı�ʶ
#define FDCMD_DOWNPar_ECHO	0x83	//ǰ��Ӧ���������

#define FDCMD_UPDateREC		0x04	//�������ݣ���������·��Ϣ��Ʊ����Ϣ������ļ�����Ҫ�������еı�ʶ
#define FDCMD_UPDateREC_ECHO	0x84	//ǰ��Ӧ���������


typedef struct  {
	unsigned char ucStation;			//��ǰվ̨��
	unsigned char ucPeStation;			//ǰһվ̨��
	unsigned char inOrOut;				//��վ��վ��־(1��2��)
	unsigned char ucOverSpeed;			//�Ƿ��� ����00����0C
	unsigned char ucDirect;				//���� ��0 ��1
	unsigned char ucFlag;				//��־���Ƿ�վ��4�ֽڣ�������4�ֽ�
}stRunStation;

extern void savesysbupara(void);
extern void addStatMoney(unsigned char mode, unsigned int moneyv);
extern void CountPoint(unsigned char mode);
extern unsigned int GpsDistance(unsigned int dsLa, unsigned int dsLo);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern unsigned int get_s_sum1(unsigned char mode);
extern void disp_no_swipe(void);
extern unsigned char PurseProcess(unsigned char mode);
extern void error(unsigned char code1,unsigned char Index);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void sysSaveDevice(unsigned char *dbuf);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
void FengDuan_Normal_card(void);

extern TCP_IP_PACKET1 tcpipPacket;
extern stPricetable NewPriceTab;
extern GPS_INFO gGpsInfo;		//GPS��Ϣ,Ӧ�ó����жϵĻ���
extern stGpsPar gGPsPar;				//������Ϣ
extern stBusVerIfo gBusVerInfo;
extern Parameter2 POINT_MESSAGE;
extern unsigned char gMCardCand;
extern unsigned char StufferNO[4];
extern Parameter3 ComSumeFile;
extern unsigned char cardSound;			//ˢ��û�гɹ���־
extern unsigned int card_ser;
extern unsigned int a_sum1,s_sum1;
extern unsigned char DriveCardNo[4];
extern unsigned char restore_flag;
extern stcomtime SysTime;
extern Parameter5 cardMessage;
extern unsigned int c_serial;
extern pFistVary_1 pFistVary;
extern stRunStation RunStation;				//�����е�վ����Ϣ
extern stLineInfo LineInfo[2];//��·��Ϣ�������վ����Ϣ ��������0�� ���� 1�鷴��

unsigned char gFDderict;
stFDLineHead gFdLinehead;
unsigned char gFDStaIndex[2];	//��ǰ���ػ�վ�����кŴ�0��ʼ,��վ���Ų�һ��.
unsigned char gFDucID;			//��1��ʼ��վ�����,����ȷ���õ�վ������ڴˣ�վ����ͨ������������Ŵ���·��Ϣ�в��ҡ�
unsigned char gZNFDucID[2];		//��1��ʼ��������վ����ţ�ǰ�Ż���Ч������δȷ��վ��ʱ��Ҫ��ֵ����Ȼ������ʾ�����
								//����Ʊ��ʱ�����ֵ���Ǵ�1��ʼ�ı�š�д�뿨��Ļ򴫸����Ż�����Ŷ��Ǵ�1��ʼ�ġ�
unsigned char gOlderStation = 0xFF;
unsigned char offstion;//�ֶ��ϳ���λ��
//---------------------------------------------
//ͨѶ���̶���
#define FDProcess_GETPriceRate 0x7E		//ͬ��Ʊ���ۿ���Ϣ��
#define FDProcess_GETFind 0x7F			//��������״̬�����˲����µ����񣬲������κζ�������ȡ��ǰ�Ż��汾��Ϣ��Ҫ�����״̬��
#define FDProcess_GETVER 0x80			//ȡ����汾�� 
#define FDProcess_GETGPSLineInfo 0x81	//ͬ����·��Ϣ��
#define FDProcess_GETFDticket 0x82		//ͬ������Ʊ��	---------����ȼ����Ͻ���û����ɣ�����ˢ�����ѡ�
#define FDProcess_GETProgram 0x83		//ͬ��������
#define FDProcess_GETFree 0x8F			//����״̬ ������ȡվ����Ϣ �� �ϴ���¼��

//Э��ͷ���壬���ֶε�ƫ����
#define FDCMDXY_head 0
#define FDCMDXY_LEN 4
#define FDCMDXY_CMD 6
#define FDCMDXY_Serial 7
#define FDCMDXY_DATA 8

//�����豸���ͣ�ǰ������ͬһ������ͨ���Ƿ����GPSģ����ȷ����ǰ�Ż����Ǻ��Ż���
#define FDMODE_GPS 0xA0
#define FDMODE_NORMAL 0x5E
unsigned char gFDMode;	//��ʶ�ֶ��շ��е��豸���ͣ�0xA1Ϊǰ�Ż�������GPS��GPRS����0xA2Ϊ���Ż���ͨ�����GPSģ��ȷ�ϡ�

//CANͨѶ����300�ֽڡ�ʵ�����ֻ��268�ֽڡ�
#define CAN_DATA_MAX_LEN 300
unsigned char CanRevbuf[CAN_DATA_MAX_LEN];
volatile unsigned int gCanInex=0;
volatile unsigned char gCanRevFlag=0;	//0xAAʱ�����ݡ� 

#define FDRUNSTATUES_PJ 0x01		//Ʊ�۴���
#define FDRUNSTATUES_TX (0x01<<1)	//CANͨѶ���󣬺��Ż�ȡ�������µĵ����ˡ�
#define FDRUNSTATUES_GPS (0x01<<2)	//GPS�źŴ���ǰ�Ż�GPS��������
#define FDRUNSTATUES_Line (0x01<<3)	//վ����Ϣ��
unsigned char gFDRunStatues;	//�豸״̬�Ƿ���Ч��0Ϊ��Ч������Ϊ��Ч(�����Ǵ���״̬����λ��)����Чʱ��Ҫ��ֹˢ����
unsigned char gFDCmdSerial=1;	//ͨѶ���.��1��ʼ��
unsigned int gFDCanError=0;		//CANͨѶ���������

#define FDSCountMax	10			//����ͨѶ���ʱ�䣬��λ-�롣
unsigned int gFDScount=0;		//���ͳ�ʱ�����ݣ�=0����
#define FDGetGPSinfoMax 4
unsigned int gFDGetGPSinfo=0;	//���Ż�ȡǰ��վ����Ϣ��

#define FDMISS_NULL 0
#define FDMISS_GETLine (0x01)		//��Ҫ������·��Ϣ
#define FDMISS_GETTicket (0x01<<1)	//��Ҫ���½���Ʊ��
#define FDMISS_GETProgram (0x01<<2)	//��Ҫ���³��� 
#define FDMISS_GETPriceRate (0x01<<3)	//��ҪƱ���ۿ�
unsigned char gFDmissflag;//�����־,��λָʾ����Щ������Ҫִ�С�ֻ�к��Ż��Ż��õ�
unsigned char gFDdealProcess;//������̣�ֻ�к��Ż��Ż��õ�
struct  
{
	unsigned char DownFlag;			//�����ļ���ʶ.
	unsigned char DownPerSerial[2];	//���ص�ǰһ��š�
	unsigned char DownSerial[2];		//��ǰ�İ���š�
}FDDownPar;

#define FDParChange_Line 0x01			//��·
#define FDParChange_Device (0x01<<1)	//�豸��
#define FDParChange_Piaojia (0x01<<2)	//Ʊ��
#define FDParChange_Program (0x01<<4)	//ǰ�ų���
#define FDParChange_Restart 0x80		//ǰ�Ż�������,ǰ�Ż���������Ҫ���Ż�ҲҪ��������(��ͬ���汾)���Ա�ͬ�����ݡ�
unsigned char gFDParChange=0;	//�����Ƿ�仯��0�ޱ仯�������б仯

extern volatile unsigned char gGpscand;//ͨ���˱����ж��Ƿ���GPSģ�顣û���Ǻ��Ż����о���ǰ�Ż���


void clrCanRevbuf(void)
{
	gCanRevFlag = 0;
	gCanInex = 0;
	memset(CanRevbuf, 0, 10);
}

void CAN_RxDataHander(void)
{
	unsigned char buf[8];
	unsigned char ret;
	unsigned short ust, i;

	ret = can_rev(buf);	//һ�������8���ֽ�

	if(gCanRevFlag ==0xAA){//������û����
		if((CanRevbuf[FDCMDXY_CMD] == FDCMD_VERCX)||(CanRevbuf[FDCMDXY_CMD] == FDCMD_GPSINFO)){//��������ǲ�ѯ��Ϣ�ģ����Զ�����������������
			clrCanRevbuf();
		}
		else{
			return;
		}
	}
	if(ret == 0)
		return;

	if(gCanInex+ret > CAN_DATA_MAX_LEN){//����
		clrCanRevbuf();
		return;
	}
	else{
		memcpy(CanRevbuf+gCanInex, buf, ret);
		gCanInex += ret;

		if(CanRevbuf[0] != '$'){//��һ���ֽڱ�����$
			debugstring("canCMD headERROR!\r\n");
			debugdata(CanRevbuf, gCanInex, 1);
			clrCanRevbuf();
			return;
		}
		if(gCanInex < 8){//�յ�����,������������
			return;
		}
				
		if(memcmp(CanRevbuf, "$$$$", 4) != 0){//���ݲ���
			debugstring("canCMD headERROR!\r\n");
			debugdata(CanRevbuf, gCanInex, 1);
			clrCanRevbuf();
			return;
		}
		memcpy((unsigned char*)&ust, CanRevbuf+4, 2);//���ݳ���
		if(ust > CAN_DATA_MAX_LEN){
			debugstring("canCMD LEN-ERROR!\r\n");
			clrCanRevbuf();
			return;
		}
		
		if(gCanInex >= ust){//�Ѿ�����һ����
			//MSG_LOG("CAN ����::ust=%d, gCanInex=%d\r\n", ust, gCanInex);
			//BCD_LOG(CanRevbuf, gCanInex, 1);
			ret = 0;
			for(i=0; i<ust-1; i++){
				ret ^= CanRevbuf[i];
			}
			if(ret != CanRevbuf[ust-1]){//У��� 24242424090002080B
				printf("canCMD XOR-ERROR!ret=%02X\r\n",ret);
				clrCanRevbuf();
				return;
			}
			else{
				gCanRevFlag = 0xAA;
			}
		}
	}
}

//��֯�����ݷ��ͣ����Ȳ�����255�ֽڡ����޸ĵ�����ֱ�ӷ���Sdate�����С�
int FDCMDData_Send(unsigned char Scmd, unsigned char *Sdata, unsigned int Sdatalen)
{
	unsigned char ret=0;
	unsigned short i;
	unsigned short index;
	
	if(Sdatalen > 0){
		memmove(Sdata+FDCMDXY_DATA, Sdata, Sdatalen);	//�������Ƶ�����������8�ֽڿ�ʼ.
	}

	index = 0;
	memset(Sdata+index, '$', 4);
	index += 4;
	i = Sdatalen+9;
	memcpy(Sdata+index, (unsigned char*)&i, 2);	//���ݳ���.���ݳ���+4+2+1+1+1
	index += 2;
	Sdata[index++] = Scmd;
	Sdata[index++] = gFDCmdSerial++;
	if(gFDCmdSerial == 0) gFDCmdSerial++;

	index += Sdatalen;//�����Ѿ��Ƶ���λ�ã���ָ���

	Sdata[index] = 0;	//����У��
	for(i=0; i<index; i++){
		Sdata[index] ^= Sdata[i];
	}
	index++;
	
	MSG_LOG("CandFDCmd_snd:");
	BCD_LOG(Sdata, index, 1);

#ifndef FengDuan_only_1
	ret = can_snd(Sdata, index);
	if(ret != 0)
		gFDCanError++;
	else
		gFDCanError=0;
	if(gFDCanError >= 30){
		gFDRunStatues |= FDRUNSTATUES_TX;
	}
	else{
		gFDRunStatues &= ~FDRUNSTATUES_TX;
	}
	gFDScount = FDSCountMax;	//�ȴ�Ӧ��

	MSG_LOG("CandFDCmd_snd end ret=%04X, gFDCanError = %d\r\n", ret, gFDCanError);
#endif

	return ret;
}


//ȡCPU�еķֶλ�����.
unsigned char getCpuFdMode(void)
{
	unsigned char buf[8];
	stCpuInfo cpuinfo;
	unsigned int offset;
	
	offset = ((unsigned char*)&cpuinfo.FDModeFlag)-((unsigned char*)&cpuinfo);
	cpuInfoRead(buf, offset, 1);
	
	if((buf[0] == FDMODE_GPS) ||(buf[0] == FDMODE_NORMAL)){
		gFDMode = buf[0];	//�����ã���ֵ��û�в���ֵ
	}

	MSG_LOG("get CPUFDMODE:%02X\r\n", buf[0]);
	return buf[0];
}
//��CPUINFO�������豸�����͡������������ж�
void SETCpuFDMode(void)
{
	unsigned int odly=0;
	unsigned char disflag, newFDMODE,rkey;

	newFDMODE = gFDMode;

	cls();
	display(0, 0, "�����豸����", DIS_CENTER);
	disflag = 1;

	while(1){
		if(disflag == 1){
			disflag = 0;
			if(newFDMODE == FDMODE_NORMAL){
				display(2, 1, " 1.ǰ�Ż� ", 0);
				display(4, 1, ">2.���Ż� ", DIS_CONVERT);
			}
			else if(newFDMODE == FDMODE_GPS){
				display(2, 1, ">1.ǰ�Ż� ", DIS_CONVERT);
				display(4, 1, " 2.���Ż� ", 0);
			}
			else{
				display(2, 1, " 1.ǰ�Ż� ", 0);
				display(4, 1, " 2.���Ż� ", 0);
			}
		}
		delayxms(20);
		if(odly++ > 500){
			return;
		}

		rkey = getkey(1);
		if(rkey == KEY_ESC){
			return;	
		}
		else if(rkey == KEY_UP){
			odly = 0;
			if(newFDMODE == FDMODE_GPS)
				newFDMODE = FDMODE_NORMAL;
			else
				newFDMODE = FDMODE_GPS;
			disflag = 1;
		}
		else if(rkey == KEY_ENT){
			saveCpuInfo(12, &newFDMODE);
			if(newFDMODE != gFDMode){
				gFDMode = newFDMODE;
				beep(1, 50, 50);
				display(6, 0, "���óɹ�,����!", DIS_CENTER);
				delayxms(800);
				ReStartCPU();
				restore_flag = 3;
				return;
			}
			return;
		}
	}
}

//��������ϵͳ��ʼ����SystemInit��֮ǰ��Ҫ���е��ã���ȷ���豸���͡�
void FD_Check_devMode(void)
{
	unsigned char i = 0;
	
	gGpscand = 0;
	gFDMode = 0;
	cls();
	display(0, 0, "Ӳ�����", DIS_CENTER);
	
	i = getCpuFdMode();
	if((i != FDMODE_GPS) &&(i != FDMODE_NORMAL)){
		i = 0;
		while (1)
		{
			if((gGpscand & 0x0f) != 0){//˵����GPSģ��
				gFDMode = FDMODE_GPS;
				break;
			}
			delayxms(10);
			if(i++ > 200){	//GPS��Ϣһ�����һ�Σ�����Ҫ��2�롣
				gFDMode = FDMODE_NORMAL;
				break;
			}
		}

		MSG_LOG("GFDMODE = %02X\r\n", gFDMode);
		if((gFDMode != FDMODE_GPS) && (gFDMode != FDMODE_NORMAL)){
			display(6, 0, "�޷�ʶ���豸����", 0);	//�������������ǲ�������ġ�
			while(1)
				clr_wdt();
		}
	}
	CAN_Initiation();	//��ʼ��CAN����
	clrCanRevbuf();
}

//�˺�����ȡ����SPI FLASH�е����ݡ�64�ֽ�һ�顣
unsigned int FDFlashDataRead_QM(unsigned int heaAddr, unsigned char *Dbuf)
{
	unsigned int addr, oi=0;
	unsigned short offset;

	Dbuf[0] = FDDownPar.DownFlag;
	oi++;
	memcpy(Dbuf+oi, FDDownPar.DownSerial, 2);
	oi+=2;

	memcpy((unsigned char *)&offset, FDDownPar.DownSerial, 2);	//ƫ����
	
	addr = heaAddr+(offset*64);
	flashread(addr, Dbuf+oi, 64);
	
	oi += 64;

	return oi;
}

//�˺�����ȡCPU FLASH�еĳ������ݡ�256�ֽ�һ�顣
unsigned int FDProDataRead_QM(unsigned char *Dbuf)
{
#define FBootCodeSize    0x4800  //BootLoader��С16K(0x4000)  ����2K(0x800)���ڴ泵�ػ����кŵȣ�
#define FMCURsetAddress  0x08000000   //MCU��λ��������е���ʼ��ַ
#define FApplicationAddress    (FMCURsetAddress+ FBootCodeSize)

	unsigned int addr, oi=0;
	unsigned short offset;
	
	Dbuf[0] = FDDownPar.DownFlag;
	oi++;
	memcpy(Dbuf+oi, FDDownPar.DownSerial, 2);
	oi+=2;
	
	memcpy((unsigned char *)&offset, FDDownPar.DownSerial, 2);	//ƫ����
	addr = offset * 256;

	addr += FApplicationAddress;
	memcpy(Dbuf+oi, ( unsigned char *)addr, 256);
	oi += 256;
	
	return oi;
#undef FBootCodeSize
#undef FMCURsetAddress
#undef FApplicationAddress
}
void FDinitFDticket_ALL(void);
extern void Load_Line_info(void);
//�����Ӧ������
unsigned int FDDealDownData_HM(unsigned char *rbu)
{//242424244C0083 04 8100000000000006000100000001000000000000003406000000000000000000000000000000000000000000000000000000000000000001000100000001000000010E70
	unsigned int Waddr,wlen,wof;
	unsigned short ofset;
	unsigned char i;

	if(FDDownPar.DownFlag != rbu[FDCMDXY_DATA]){//���Ͳ�һ��,
		MSG_LOG("DealDownData DownFlag Error:\r\n");
		return 1;
	}

	if(memcmp(rbu+FDCMDXY_DATA+1, "\xFF\xFF", 2) == 0){//������
		if(FDDownPar.DownFlag == FDProcess_GETGPSLineInfo){
			Load_Line_info();	//������������
			gFDmissflag &= ~FDMISS_GETLine;
		}
		else if(FDDownPar.DownFlag == FDProcess_GETFDticket){
			FDinitFDticket_ALL();
			gFDmissflag &= ~FDMISS_GETTicket;
		}
		else if(FDDownPar.DownFlag == FDProcess_GETPriceRate){//һ������ɣ����Ժ��Ż�ûҪ���������Ѿ������ˣ�Ӧ�ò��ᵽ���
			gFDmissflag &= ~FDMISS_GETPriceRate;
		}
		else if(FDDownPar.DownFlag == FDProcess_GETProgram){//�������
			gFDmissflag &= ~FDMISS_GETProgram;
			memset(rbu, 0, 64);
			memcpy(rbu,"������Ҫ����", 12);
			memcpy(rbu+12,"\xEE\xFF", 2);
			memcpy(rbu+14, (unsigned char*)&SysTime, 7);

			MSG_LOG("�����������:");
			BCD_LOG(rbu, 21, 1);

			display(2,0,"���,������...",DIS_CENTER);
			Waddr = FLASH_PRO_FLAG;
			
			MSG_LOG("�����������2:");
			BCD_LOG(rbu, 34, 1);
			flashwrite(Waddr, rbu, 64);
			
			beep(1,10,10);

			delayxms(400);
			ReStartCPU();//������ɣ������豸�Ա���³���
			while(1);
		}
		gFDdealProcess = FDProcess_GETFind;
		memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		return 0;
	}
	
	if(memcmp(FDDownPar.DownSerial, rbu+FDCMDXY_DATA+1, 2) != 0){//����Ų�һ��
		MSG_LOG("DealDownData Error:\r\n");
		return 1;
	}
	
	memcpy((unsigned char*)&ofset, rbu+FDCMDXY_DATA+1, 2);	
	if(FDDownPar.DownFlag == FDProcess_GETGPSLineInfo){
		Waddr = FLASH_GPS_LINE_START;
		if(ofset == 0){ //
			MSG_LOG("GPS LINE Erase\r\n");
			flasherase(ERASE64KBYTESECTOR, Waddr);//�Ȳ��� 64K
			Waddr += 0x10000;//��һ��64K�Ŀ�
			//Delay(2);
			flasherase(ERASE64KBYTESECTOR, Waddr);//�Ȳ��� 64K
		}
		Waddr = FLASH_GPS_LINE_START;
		wof = ofset*64;
		wlen = 64;
	}
	else if(FDDownPar.DownFlag == FDProcess_GETFDticket){
		Waddr = FLASH_FD_Ticket_START;
		if(ofset == 0){ //
			MSG_LOG("Ticket Erase\r\n");
			flasherase(ERASE32KBYTESECTOR, Waddr);//����
		}
		wof = ofset*64;
		wlen = 64;
	}
	else if (FDDownPar.DownFlag == FDProcess_GETPriceRate){//����Ʊ�ۡ�һ����256�ֽ���ɡ��������˺�ͽ���������Ҫ�������ˡ�
		memcpy((unsigned char*)&NewPriceTab, rbu+FDCMDXY_DATA+3, 256);
		savesysbupara();
		saveCpuInfo(1, rbu);
		sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);
		
		gFDmissflag &= ~FDMISS_GETPriceRate;
		gFDdealProcess = FDProcess_GETFind;
		memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		return 0;
	}
	else if(FDDownPar.DownFlag == FDProcess_GETProgram){
		if(ofset == 0){ //
			Waddr = FLASH_PRO_START;
			MSG_LOG("Pro Erase\r\n");
			for(i=0; i<4; i++){
				flasherase(ERASE64KBYTESECTOR, Waddr);//�Ȳ��� 256K
				Waddr += 0x10000;//��һ��64K�Ŀ�
			}
		}

		Waddr = FLASH_PRO_START;
		wof=ofset*256;
		wlen = 256;
	}
	else{
		printf("FDDeal DownData Flag is Err:%02X\r\n", FDDownPar.DownFlag);
		return 2;
	}

	Waddr += wof;
	flashwrite(Waddr, rbu+FDCMDXY_DATA+3, wlen);
	flashread(Waddr, irda_rxBuf, wlen);
	if(memcmp(rbu+FDCMDXY_DATA+3, irda_rxBuf, wlen) != 0){
		MSG_LOG("Flash write ERROR!%d, %d\r\n", Waddr, wof);
		memset(FDDownPar.DownPerSerial, 0, 2);
		memset(FDDownPar.DownSerial, 0, 2);
//		while(1)
//			delayxms(100);
		return 3;
	}

	memcpy(FDDownPar.DownPerSerial, (unsigned char*)&ofset, 2);	//�������ز���
	ofset++;
	memcpy(FDDownPar.DownSerial, (unsigned char*)&ofset, 2); //����ż�1
	MSG_LOG("Down Data OK: FDDownPar:");
	BCD_LOG((unsigned char*)&FDDownPar, sizeof(FDDownPar), 1);

	if(FDDownPar.DownFlag == FDProcess_GETProgram)
		sprintf((char*)rbu, " D:%d%% ", ofset*100/0x03B8);
	else
		sprintf((char*)rbu, " D:%d ", ofset);
	miniDispstr(6, 0, (char*)rbu, DIS_CENTER);

	return 0;
}
unsigned char DataIsFF_QM(unsigned char *cbuf, unsigned int cLine)
{
	unsigned int i;

	for(i=0; i<cLine; i++){
		if(cbuf[i] != 0xFF)
			return 0;
	}
	MSG_LOG(" IS FF OK FF\r\n");
	return 1;
}
//rbu�����յ������ݣ���֯�õ�����Ҳ��rbu�д��ء�ǰ�Ż��Żᴫ��
unsigned int FDBuildDownData_QM(unsigned char *rbu)
{											//2424242410000302 81 0000 0000 0000 90
	unsigned int index=0;
	unsigned short of;

	FDDownPar.DownFlag = rbu[FDCMDXY_DATA];					//ǰ�Ż�������ʹ�ú��Ż�����������
	memcpy(FDDownPar.DownPerSerial, rbu+FDCMDXY_DATA+1, 2);
	memcpy(FDDownPar.DownSerial, rbu+FDCMDXY_DATA+3, 2);
	memcpy((unsigned char*)&of, FDDownPar.DownSerial, 2);
	
	MSG_LOG("Build Down Dat:flag=%02X\r\n", FDDownPar.DownFlag);
	BCD_LOG(FDDownPar.DownPerSerial, 2, 1);
	BCD_LOG(FDDownPar.DownSerial, 2, 1);

	if(FDDownPar.DownFlag == FDProcess_GETGPSLineInfo){
		if(of >= (0x20000/64)){//128K,�������
			of = 0xFFFF;
		}
		else{
			index = FDFlashDataRead_QM(FLASH_GPS_LINE_START, rbu);
			if(DataIsFF_QM(rbu+3, 64) == 1){//����ȫ��FF������
				of = 0xFFFF;
			}
		}
	}
	else if(FDDownPar.DownFlag == FDProcess_GETFDticket){
		if(of >= (0x8000/64)){//32K,�������
			of = 0xFFFF;
		}
		else{
			index = FDFlashDataRead_QM(FLASH_FD_Ticket_START, rbu);
			if(DataIsFF_QM(rbu+3, 64) == 1){//����ȫ��FF������
				of = 0xFFFF;
			}
		}
	}
	else if(FDDownPar.DownFlag == FDProcess_GETPriceRate){
		if(of >= 1){//ֻ��256�ֽڣ�һ�����·�������һ���������������Ż����������һ���������������ǲ���ִ�еġ�
			of = 0xFFFF;
		}
		else{
			index = 0;
			rbu[index++] = FDDownPar.DownFlag;
			memcpy(rbu+index, FDDownPar.DownSerial, 2);
			index+=2;
			
			memcpy(rbu+index, (unsigned char *)&NewPriceTab, 256);
			index += 256;
		}
	}
	else if(FDDownPar.DownFlag == FDProcess_GETProgram){
		if(of >= (0x3B800/256)){//238K,������� 24242424 1000 03 BD83B703B803000022
			of = 0xFFFF;
		}
		else{
			index = FDProDataRead_QM(rbu);
			if(DataIsFF_QM(rbu+3, 256) == 1){//����ȫ��FF������
				of = 0xFFFF;
			}
		}
	}
	else{
		return 0;
	}
	
	if(of == 0xFFFF){//�����Ѿ�������
		rbu[0] = FDDownPar.DownFlag;
		rbu[1] = 0xFF;
		rbu[2] = 0xFF;
		index = 3;
	}
	
	return index;

}

//�����,ǰ�š����ŵĴ��������档
int CanFDdealCMD_ALL(unsigned char *Rdata)
{
	DEALRECODE *rRe;
	int ret;
	unsigned int ui, ii, slen=0;
	unsigned char rbuf[300];
	
	if(gCanRevFlag != 0xAA){//û���յ�������
		return -1;
	}
	
	if(gCanInex > 300){
		gCanInex = 300;
	}
	ii = gCanInex;

	memcpy(rbuf, CanRevbuf, gCanInex);
	clrCanRevbuf();
	
	MSG_LOG("Can RevData:");
	BCD_LOG(rbuf, ii, 1);
	
	if(rbuf[FDCMDXY_CMD] & 0x80){	// ���λΪ1��ǰ�Ż���Ӧ��
		if(gFDMode == FDMODE_GPS){	//����ǰ�Ż�������Ӧ������ǰ�Ż�����������ǰ�Ż�ֻ������Ż�������
			MSG_LOG("ǰ�Ż��������ʹ�:%02X\r\n", rbuf[6]);
			return -2;
		}
	}
	else{	//���λΪ0�Ǻ��Ż�������ָ�
		if(gFDMode == FDMODE_NORMAL){	//���Ǻ��Ż�������������Ժ��Ż��������������Ż�ֻ����ǰ�Ż���Ӧ��
			MSG_LOG("���Ż��������ʹ�:%02X\r\n", rbuf[6]);
			return -2;
		}
	}
	
	switch(rbuf[FDCMDXY_CMD]){
	case FDCMD_VERCX://ǰ�Ż����͵�ȡ�����汾������
		gFDCmdSerial = rbuf[FDCMDXY_Serial];
		ii = SOFT_VER_TIME_LOG;	//����汾�š�
		memcpy(rbuf, (unsigned char*)&ii, 4);
		slen += 4;
		if(gGPsPar.EnableGps == 0){//��·��Ϣ��������
			memset(rbuf+slen, 0, 4);	//��·��Ϣ�汾��
		}
		else{
			memcpy(rbuf+slen, (unsigned char*)&LineInfo[0].uiVer, 4);	//��·��Ϣ�汾��
		}
		slen += 4;
		ii = gFdLinehead.CXulieNo;	//����Ʊ�۰汾������ʱ������ɡ�
		memcpy(rbuf+slen, (unsigned char*)&ii, 4);
		slen += 4;
		memcpy(rbuf+slen, pFistVary.LineNo, 2);	//��·��
		slen += 2;
		ASCII2ZBcod((char*)pFistVary.DeviceNo, rbuf+slen, 4);//�豸��
		slen += 4;

		MSG_LOG("CandFDCmd_snd DATA:");
		BCD_LOG(rbuf, slen, 1);

		ret = FDCMDData_Send(FDCMD_VERCX|0x80, rbuf, slen);
		gFDParChange = 0;	//���Ż���ȡ�˰汾��Ϣ�󣬲���֪ͨ���Ż����²�����������
		MSG_LOG("FDCMD_VERCX end ret=%d\r\n", ret);
		break;
	case FDCMD_VERCX_ECHO://ǰ�Ż���Ӧ�İ汾�� 24242424150081B9 73000000 01000000 D0000000 8F
		MSG_LOG("�汾��Ӧ!\r\n");	//����汾����·��Ϣ�汾������Ʊ�۱�
		gFDmissflag = FDMISS_NULL;
		memcpy((unsigned char*)&ui, rbuf+FDCMDXY_DATA, 4);	//����汾,
		if(ui != SOFT_VER_TIME_LOG){//����汾��һ��
			gFDmissflag |= FDMISS_GETProgram;
		}
		memcpy((unsigned char*)&ui, rbuf+FDCMDXY_DATA+4, 4);	//��·��Ϣ�汾
		if((ui!=0)&&(ui != LineInfo[0].uiVer)){//ǰ�Ż����İ汾��Ϊ0��˵��ǰ�Ż�Ҳû�д���Ϣ��
			MSG_LOG("��·��Ϣ��һ��:ui=%d, ver=%d\r\n", ui, LineInfo[0].uiVer);
			gFDmissflag |= FDMISS_GETLine;
		}
		memcpy((unsigned char*)&ui, rbuf+FDCMDXY_DATA+8, 4);	////����Ʊ�۰汾
		if((ui!=0)&&(ui != gFdLinehead.CXulieNo)){	//ǰ�Ż����İ汾��Ϊ0��˵��ǰ�Ż�Ҳû�д���Ϣ��
			MSG_LOG("��·��Ϣ��һ��:ui=%d, ver=%d\r\n", ui, gFdLinehead.CXulieNo);
			gFDmissflag |= FDMISS_GETTicket|FDMISS_GETPriceRate;	//Ҫͬʱ���½���Ʊ�ۼ�Ʊ���ۿ�.
		}
		if(memcmp(pFistVary.LineNo, rbuf+FDCMDXY_DATA+12, 2) != 0){//��·�Ų�һ��������
			memcpy(pFistVary.LineNo, rbuf+FDCMDXY_DATA+12, 2);	//��·��
			
			sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
			sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);
			saveCpuInfo(1, rbuf);
		}
		ZBcod2ASCII(rbuf+FDCMDXY_DATA+14, 4, (char*)pFistVary.DeviceNo_1);	//�豸��
		if(memcmp(pFistVary.DeviceNo, pFistVary.DeviceNo_1, 8) != 0){//��һ�£���Ҫ����
			memcpy(pFistVary.DeviceNo, pFistVary.DeviceNo_1, 8);
			pFistVary.DeviceNo[8] = 0;
			sysSaveDevice(rbuf);
		}

		gFDdealProcess = FDProcess_GETFind;
		restore_flag = 3;
		gFDScount = 0;
		break;
	case FDCMD_GPSINFO://���Ż����͵�ȡλ����Ϣ���
		gFDCmdSerial = rbuf[FDCMDXY_Serial];		
		memcpy(rbuf, (unsigned char*)&SysTime, 7);
		slen = 7;
		if(gFDderict<2)
			rbuf[slen++] = gZNFDucID[gFDderict];//����վ����š���Ż��ô���·��Ϣ���в����
		else
			rbuf[slen++] = 0;
		rbuf[slen++] = gFDderict;
		memcpy(rbuf+slen, (unsigned char*)&gGpsInfo.velocity, 4);
		slen += 4;
		rbuf[slen++] = gFDParChange;	//��ֵ����ȥ�Ժ���0
		gFDParChange = 0;
		ret = FDCMDData_Send(FDCMD_GPSINFO|0x80, rbuf, slen);
		MSG_LOG("FDCMD_GPSINFO end ret=%d\r\n", ret);
		break;
	case FDCMD_GPSINFO_ECHO://���Ż���Ӧ��ʱ�䡢վ�㡢���򡢳��١������Ƿ�仯
		MSG_LOG("��Ϣ��Ӧ!\r\n");//242424241700820D 20151012182409 05 00 00000000 00 9F
		if(memcmp((unsigned char*)&SysTime, rbuf+FDCMDXY_DATA, 5) != 0){//ʱ�䲻һ�£���Ч��ʱ
			MSG_LOG("Уʱ:");
			BCD_LOG(rbuf+FDCMDXY_DATA, 7, 1);
			if(time_invalid(rbuf+FDCMDXY_DATA) == 1){
				timewrite(rbuf+FDCMDXY_DATA);
				MSG_LOG("Уʱ:3\r\n");
			}
		}
		gFDucID = rbuf[FDCMDXY_DATA+7];	//վ�����
		gFDderict = rbuf[FDCMDXY_DATA+8];	//����
		memcpy((unsigned char*)&gGpsInfo.velocity, rbuf+FDCMDXY_DATA+9, 4); //�ٶ�
		if(rbuf[FDCMDXY_DATA+13]&0x7F){//�����仯,������������¸��²�����
			gFDdealProcess = FDProcess_GETVER;//����ȡ�������汾��
		}
		else if(rbuf[FDCMDXY_DATA+13]&FDParChange_Restart){//�豸��Ҫ����
			gFDdealProcess = FDProcess_GETVER;//����ȡ�������汾�ţ� ��������
// 			cls();
// 			display(0, 1, "ָ������", DIS_CENTER);
// 			beep(8, 50, 50);
// 			ReStartCPU();
		}
		gFDScount = 0;
		break;
	case FDCMD_DOWNPar:	//�����յ����ز���ָ���Ҫ��֯���ݽ������ء�
		slen = FDBuildDownData_QM(rbuf);
		BCD_LOG(rbuf, slen, 1);
		ret = FDCMDData_Send(FDCMD_DOWNPar|0x80, rbuf, slen);
		MSG_LOG("FDCMD_DOWNPar end ret=%d\r\n", ret);
		break;
	case FDCMD_DOWNPar_ECHO://ǰ�Ż����ӻ�����Ӧ�����ݡ����Ż�����
		MSG_LOG("���ݻ�Ӧ!\r\n");
		BCD_LOG(rbuf, ii, 1);
		FDDealDownData_HM(rbuf);	//�����Ӧ������
		gFDScount = 0;
		break;
	case FDCMD_UPDateREC://���Ż��������ݹ�����
		MSG_LOG("��¼����!\r\n");
		rRe = (DEALRECODE *)rbuf+FDCMDXY_DATA+3;
		Get_SerialNum(rRe->rResever1);	//Ψһ��
		memcpy(rRe->rDriverNo, DriveCardNo, 4);	//˾����.
		WriteRecord(rbuf+FDCMDXY_DATA+3, 0);
		rbuf[0] = rbuf[FDCMDXY_DATA];
		rbuf[1] = rbuf[FDCMDXY_DATA+1];
		rbuf[2] = rbuf[FDCMDXY_DATA+2];
		rbuf[3] = 0;//״̬
		slen = 4;
		BCD_LOG(rbuf, slen, 1);
		ret = FDCMDData_Send(FDCMD_UPDateREC|0x80, rbuf, slen);
		MSG_LOG("FDCMD_DOWNPar end ret=%d\r\n", ret);
		break;
	case FDCMD_UPDateREC_ECHO:
		MSG_LOG("��¼��Ӧ:%02X, %02X, %02X\r\n", tcpipPacket.tcpBuffer[0], tcpipPacket.tcpBuffer[1], tcpipPacket.tcpBuffer[2]);
		if((rbuf[FDCMDXY_DATA+3] == 0)&&(rbuf[FDCMDXY_DATA] == tcpipPacket.tcpBuffer[0])&&(rbuf[FDCMDXY_DATA+1] == tcpipPacket.tcpBuffer[1])&&(rbuf[FDCMDXY_DATA+2] == tcpipPacket.tcpBuffer[2])){//
			memset((unsigned char*)&tcpipPacket, 0, 5);
			sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
		}
		gFDScount = 0;
		break;
	default:
		break;
	}

	return ret;
}

//ǰ��GPS������ʾ����
void FengDuan_GPS_disp_QM(void)
{
	unsigned int it=0;
	char disbuff[30];
	
	cls();
	if((gFDderict>=2)||(gFDucID == 0)||(gFDucID > 100))
	{
		strcpy(disbuff, "δȷ��λ��");
		LED_Dis3("------");
	}
	else if(gGPsPar.EnableGps == 0){//��·��Ϣ����
		strcpy(disbuff, "��·��Ϣ����");
		LED_Dis3("------");
	}
	else if(gFDRunStatues & FDRUNSTATUES_PJ){
		strcpy(disbuff, "Ʊ����Ϣ����");
		LED_Dis3("------");
	}
	else{
		sprintf(disbuff, "%03X", LineInfo[gFDderict].StationInfo[gFDStaIndex[gFDderict]].ucId);
		if(gFDderict == 0)
			disbuff[0] = '+';
		else
			disbuff[0] = '-';
		LED_Dis3(disbuff);
		
		memset(disbuff, 0, 20);
		if(strlen(LineInfo[gFDderict].StationInfo[gFDStaIndex[gFDderict]].name) <= 10)
		{
			if(gFDderict==0)
				strcpy(disbuff, "����:");
			else if(gFDderict==1)
				strcpy(disbuff, "����:");
			else
				strcpy(disbuff, "λ��:");
		}
		strcat(disbuff, LineInfo[gFDderict].StationInfo[gFDucID-1].name);
	}
	display(0,1,disbuff,0);
	
	memset(disbuff, 0, 30);
	strcpy(disbuff, "��·:");
	BCD2Ascii(pFistVary.LineNo, (unsigned char*)disbuff+5, 2);
	display(2,1,disbuff,0);
	it = 0;
	memcpy((unsigned char*)&it, gFdLinehead.TottalPrice, 2);
	sprintf(disbuff, "ȫ��:%d.%02dԪ", it/100, it%100);
	display(4,1,disbuff,0);
}

void disMission_HM(void)
{
	char disbuf[32];
	switch(gFDdealProcess){
	case FDProcess_GETFind:
		strcpy(disbuf, "ȷ��������");
		break;
	case FDProcess_GETVER:
		strcpy(disbuf, "��ѯ�汾��");
		break;
	case FDProcess_GETGPSLineInfo:
		strcpy(disbuf, "������·��Ϣ");
		break;
	case FDProcess_GETFDticket:
		strcpy(disbuf, "����Ʊ�۱�");
		break;
	case FDProcess_GETProgram:
		strcpy(disbuf, "���³�����");
		break;
	case FDProcess_GETPriceRate:
		strcpy(disbuf, "�����ۿ۱�");
		break;
	default:
		break;
	}
	display(2, 1, disbuf, DIS_CENTER);
}
//������ͨ����ʾ����
void FengDuan_Normal_disp_HM(void)
{
	char disbuf[32];
	
	cls();

	pFistVary.DeviceNo[8] = 0;
	sprintf(disbuf, "L:%02X%02X D:%s %03d", pFistVary.LineNo[0], pFistVary.LineNo[1], (char*)pFistVary.DeviceNo, SOFT_VER_TIME_LOG);
	miniDispstr(0, 0, disbuf, 0);
//	display(0, 0, " �ֶο۷� ", DIS_CENTER|DIS_CONVERT);
	if(gFDdealProcess <= FDProcess_GETProgram){//�ڸ��¸��ֲ���ʱ�����Ż�����ˢ����
		disMission_HM();
		display(4, 0, "��ͣˢ��", DIS_CENTER);
	}
	else if((gFDderict>=2)||(gFDucID == 0)||(gFDucID > 100))
	{
		display(2, 1, "δȷ��λ��", DIS_CENTER);
		display(4, 1, "����ˢ��!", DIS_CENTER);
	}
	else if(gGPsPar.EnableGps == 0){//��·��Ϣ����
		display(2, 1, "��·��Ϣ����", DIS_CENTER);
		display(4, 1, "����ˢ��!", DIS_CENTER);
	}
	else if(gFDRunStatues & FDRUNSTATUES_PJ){
		display(2, 1, "Ʊ����Ϣ����", DIS_CENTER);
		display(4, 1, "����ˢ��!", DIS_CENTER);
	}
	else{
		MSG_LOG("disp SLA:gFDderict=%d,gFDucID=%d,maxID:%d\r\n", gFDderict, gFDucID, LineInfo[0].ucStationMax);
		strcpy(disbuf, LineInfo[gFDderict].StationInfo[gFDucID-1].name);
		display(2, 1, disbuf, DIS_CENTER);
		display(4, 1, "��ˢ��!", DIS_CENTER);
	}
	LED_Dis3("------");
}

//���������仯ʱ�ı��־����������仯��Ʊ�۱仯����·�仯���豸�ű仯��
void FDParChange_QM(unsigned char mode)
{
	gFDParChange |= mode;	
}

void FDinitFDticket_ALL(void)
{
	unsigned char ver;
	unsigned char buff[32];
	
	flashread(FLASH_FD_Ticket_START, buff, 16);
	
	MSG_LOG("Init FD:");
	BCD_LOG(buff, 16, 1);
	
	if(memcmp(buff, "����·��Ϣ", 10) != 0){
		
		MSG_LOG("Init Err1\r\n");
		
		memset((unsigned char*)&gFdLinehead, 0, sizeof(stFDLineHead));
		gFDRunStatues |= FDRUNSTATUES_PJ;
	}
	else{
		ver = buff[12];	// �汾�ţ���������·��ʱ����ó���
		flashread(FLASH_FD_Ticket_START+16, buff, 16);
		if(MiBlockInvalid(1, 15, buff) == ST_ERROR){
			gFDRunStatues |= FDRUNSTATUES_PJ;
			
			MSG_LOG("Init Err2\r\n");
			
			memset((unsigned char*)&gFdLinehead, 0, sizeof(stFDLineHead));
		}
		else{
			gFDRunStatues &= ~FDRUNSTATUES_PJ;
			memcpy((unsigned char*)&gFdLinehead, buff, sizeof(stFDLineHead));
			gFdLinehead.CXulieNo = ver;
			
			MSG_LOG("Init OK..\r\n");
			BCD_LOG((unsigned char*)&gFdLinehead, sizeof(stFDLineHead), 1);
		}
	}
}
//��ʼ���ֶ��շѵı���,��������һ��
void initFDpara_ALL(void)
{	
	memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
	gFDRunStatues = 0;
	if(gFDMode == FDMODE_NORMAL){
		gFDucID = 0xFF;
		gOlderStation = 0xFF;
		gFDdealProcess = FDProcess_GETVER;
		gFDmissflag = FDMISS_NULL;
	}
	else{
		gFDStaIndex[0] = RunStation.ucStation;
		gFDStaIndex[1] = RunStation.ucStation;
		gFDderict = RunStation.ucDirect;

		gZNFDucID[0] = gFDStaIndex[0]+1;	//����š���1����Ϊ��Ŵ�0��ʼ����������Ǵ�1��ʼ�ġ�������վ���š�����վ���ź���Ų�һ��
		gZNFDucID[1] = gFDStaIndex[1]+1;
		gFDucID = gZNFDucID[gFDderict];

		gFDParChange |= FDParChange_Restart;	//ǰ�Ż�����ʱ��֪ͨ���Ż�������
	}
	FDinitFDticket_ALL();
}

//����д��ֶ�Ʊ�ۡ�
/*
void WriteFDLineIfno_demo_QM(void)
{
	unsigned int addr;
	unsigned char buff[1024];
	unsigned short usi;
	unsigned int i, t, r;

	buff[0] = 0;
	buff[1] = 1;
	buff[2] = 1;

	buff[3] = 0;
	buff[4] = 2;
	buff[5] = 0;
	buff[6] = 10;

	buff[7] = 0;
	buff[8] = 0;

	buff[9] = 2;

	buff[10] = 60;
	buff[11] = 0;

	buff[12] = 0xE8;
	buff[13] = 0x03;

	buff[14] = 0;
	buff[15] = MiBlockInvalid(0, 15, buff);
	
	addr = FLASH_FD_Ticket_START;
	flasherase(ERASE32KBYTESECTOR, addr);
	memcpy(buff+16, "����·��ϢOKOKOK", 16);
	i = CRC8_Tab((unsigned char*)&SysTime, 7);	//���㵱ǰ��CRC8��Ϊ�汾��
	buff[12] = i;
	flashwrite(addr, buff+16, 16);	//��־
	addr+=16;
	flashwrite(addr, buff, 16);//16 ͷ
	addr+=16;
		//�����Ƿֶ�����,2���ֽ�һ����.�����10*10��վ��,����100��.��һ����0-0 0-1.....
	for(i=0; i<12; i++){//10�������
		for(t=0; t<12; t++){//10�������
			if(i <= t)
				break;
			usi = abs(i-t);
			if(usi != 0)
				usi++;
			
			r = i*(i-1);
			r /= 2;
			r += t;
			memcpy(buff+(r*2), (unsigned char*)&usi, 2);
		}
	}
	flashwrite(addr, buff, 200);

	for(i=0; i<12; i++){//10�������
		for(t=0; t<12; t++){//10�������
			addr = FLASH_FD_Ticket_START+32;
			
			if(i <= t)
				break;

			r = i*(i-1);
			r /= 2;
			r += t;
			addr += (r*2);

			flashread(addr, (unsigned char*)&usi, 2);
			sprintf((char*)buff, "%d-%d:%d\r\n", i, t, usi);
			debugstring((char*)buff);
		}
	}

}
*/


void BuildFengDuanRecorde(unsigned char delType, unsigned char *recBuf)
{
	DEALRECODE *rRecordDeal;
	unsigned int temp;
	unsigned char k;
	
	sysferead(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN,(unsigned char*)&POINT_MESSAGE);
	
	rRecordDeal = (DEALRECODE *)recBuf;
	
	//rRecordDeal->rRemain = gFDucID;
	if(gFDderict>2)
		gFDderict=0;
	k = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId; //��0 �ĳ� gFDderict
	k = (((k>>4)*10)+(k&0x0F));	
	rRecordDeal->rRemain=k;


	memcpy(rRecordDeal->ruserTimes, gFdLinehead.NeedMaxTime, 2);
	rRecordDeal->oldLineNO[0] = cardMessage.getdata_w;
	memcpy(rRecordDeal->rMoneyDealSum, gFdLinehead.TottalPrice, 2);

	memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&a_sum1, 4);
	memcpy(rRecordDeal->rDealMoney, (unsigned char *)&s_sum1, 3);
	CountPoint(0);
	temp =POINT_MESSAGE.MoneyDealPointer;
	POINT_MESSAGE.MoneyDealPointer++;
	if(POINT_MESSAGE.MoneyDealPointer > 0x3B9AC9FF)
		POINT_MESSAGE.MoneyDealPointer = 0;
	temp = POINT_MESSAGE.MoneyDealPointer;
	memcpy(rRecordDeal->rCardDealNumb, (unsigned char*)&temp, 4);
	if(delType==ID_REC_TOLL)
		rRecordDeal->rPublishNumb_a=offstion;
	else
		rRecordDeal->rPublishNumb_a = 0;
	rRecordDeal->rCardType = cardMessage.card_catalog;
	Get_SerialNum(rRecordDeal->rResever1);
	//	memcpy(rRecordDeal->rResever1, "\xea\xef\xea\xef", 4);
	memcpy(rRecordDeal->rDealTime, (unsigned char*)&SysTime, 7);
	rRecordDeal->rDealType = delType;
	Ascii2BCD(pFistVary.DeviceNo, rRecordDeal->rDeviceNo, 8);
//	Ascii2BCD(pFistVary.DeviceNo_1, rRecordDeal->oldDevceNo, 8);
	memcpy(rRecordDeal->rDriverNo, DriveCardNo, 4);
	memcpy(rRecordDeal->rstuffernum, StufferNO, 4);
	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 2);
	memcpy(rRecordDeal->rPublishNumb, cardMessage.PublishBicker, 4);
	rRecordDeal->rCardCand = gMCardCand;

	memcpy(rRecordDeal->rBussinessNo, cardMessage.TradeCode, 2);//�̻�����Կ��ڱ��Ϊ׼������ȡˢ�����ڱ��
	rRecordDeal->rReseverX = 0xFF;
	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);//����汾

	CountRecMac((unsigned char*)&rRecordDeal, 60, rRecordDeal->rTAC);
	memcpy(rRecordDeal->rTAC+2, "\xea\xef", 2);

	MSG_LOG("BLD REC:");
	BCD_LOG(recBuf, 64, 1);

	
	if(delType == ID_REC_TOLL)
		addStatMoney(delType, s_sum1);
}

//ȡ��վ���Ʊ��,
unsigned int get2stationPrice(unsigned char sta1, unsigned char sta2)
{
	unsigned char gspbuf[16];
	unsigned int i, t, addr;

	if(sta1 == sta2){//վ��û��, ���Ʊ���е��𲽼۲���0����Ҳ��Ҫ�۷ѡ�
		addr = FLASH_FD_Ticket_START;
		addr+=16;	//��־
		flashread(addr, gspbuf, 16);//16 ͷ
		MSG_LOG("get stationPrice:");
		BCD_LOG(gspbuf, 16, 1);
		i = gspbuf[14];	//�𲽼ۣ���Ϊ��λ��0.1
		i *= 10;
		return i;
	}
	if(sta1 > sta2){
		i = sta1;
		t = sta2;
	}
	else{
		i = sta2;	//iΪ��ı��.tΪС�ı��
		t = sta1;
	}
	i -= 1;
	t -= 1;	//��Ŵ�0��ʼ,λ�ô�0��ʼ
	
	addr = (i-1);
	i *= addr;
	i /= 2;
	i += t;
#ifdef _debug_

	MSG_LOG("CountSum i:");
	BCD_LOG((unsigned char*)&i, 4, 1);
	debugstring("CountSum gFDucID:");
	debugdata((unsigned char*)&sta1, 1, 1);
	debugstring("CountSum Cp1->stationNo:");
	debugdata((unsigned char*)&sta2, 1, 1);
	debugstring("CountSum addr:");
	debugdata((unsigned char*)&addr, 1, 1);
	debugstring("CountSum t:");
	debugdata((unsigned char*)&t, 1, 1);
#endif
	addr = FLASH_FD_Ticket_START+32;//��ʼ��ַ
	addr += (i*2); //ÿ��Ʊ��2���ֽ�.
	
	MSG_LOG("CountSum addr:");
	BCD_LOG((unsigned char*)&addr, 4, 1);

	flashread(addr, gspbuf, 2);

	MSG_LOG("CountSum gspbuf:");
	BCD_LOG(gspbuf, 2, 1);

	t = 0;
	memcpy((unsigned char*)&t, gspbuf, 2);

	MSG_LOG("CountSum t:");
	BCD_LOG((unsigned char*)&t, 4, 1);

	return t;
}

unsigned int CountSum(unsigned char *UWdat)
{
	stFDCard1 *Cp1;
	stFDCard2 *Cp2;
	unsigned char cbuff[16];

	int i, t;
	unsigned char zdbh, czdbh;

	Cp1 = (stFDCard1*)UWdat;
	Cp2 = (stFDCard2*)(UWdat+16);

	if(gFDderict == 1)
		irda_rxBuf[0] = Cp2->NstationNO;
	else
		irda_rxBuf[0] =  Cp1->stationNo;	//��������ʱ������,������װ���� ��CandFDCmd_snd�����е�FDCMD_MPurChase�����С� 
	memcpy(irda_rxBuf+1, LineInfo[gFDderict].StationInfo[irda_rxBuf[0]-1].name, 23);

	if(Cp1->getupFlag != 0xAA)//��־����
		return 0;
	
	Ascii2BCD(pFistVary.DeviceNo, cbuff, 8);
	i = 0;
	memcpy((unsigned char*)&i, Cp2->timeDly, 2);//��������ʱ�䳤 ��λ�֡�
	i *= 60;
	t = DataTimeDec((unsigned char*)&SysTime, Cp1->getupTime);
	
	Ascii2BCD(pFistVary.DeviceNo, cbuff, 8);


	BCD_LOG(Cp2->deviceNo, 4, 1);
	BCD_LOG(cbuff, 4, 1);


	if(Cp1->times == 0){//û�����Ѵ���
		return 0;
	}

	if((memcmp(Cp1->LineNo, pFistVary.LineNo, 2) != 0)||(memcmp(Cp2->deviceNo, cbuff, 4) != 0)||(t > i)){//��ͬ��,��ͬ��·,��ʱ,��ȫ�� 
		i = 0;
		memcpy((unsigned char*)&i, Cp1->Allprice, 2);
		return i;
	}
	
//	zdbh = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;	//��1����Ϊ��Ŷ��Ǵ�1��ʼ�ġ�
	zdbh=LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//�³���λ��
	czdbh=Cp1->stationNo; //�ϳ���λ��
	offstion=czdbh;//���� ���¼Ҫ��
// 	if(gFDderict == 0){//����
// 		//czdbh = LineInfo[0].StationInfo[Cp1->stationNo-1].ucId;
// 		czdbh=Cp1->stationNo;
// 	}
// 	else{//����
// 		czdbh = LineInfo[1].StationInfo[Cp2->NstationNO-1].ucId;
// 	}
	//��������·��Ϣ����������BCD�롣
	zdbh = (zdbh>>4)*10+(zdbh&0x0F); //�³���λ��
	//czdbh = (czdbh>>4)*10+(czdbh&0x0F);

	s_sum1 = get2stationPrice(zdbh, czdbh);
// 	debugstring("Ʊ��\r\n");
// 	debugdata((unsigned char*)&zdbh,1,1);
// 	debugdata((unsigned char*)&czdbh,1,1);
// 	debugdata((unsigned char*)&i,4,1);
	MSG_LOG("CountSum sum:%d\r\n", i);
	i=get_s_sum1(0);//ȡ�ۿ�
	return i;
}

//�ֶ�����վ����
void EX_dealFengDuanStation(void)
{
//	unsigned char i, station;
//	unsigned int ds1,ds2;
	if(gFDMode == FDMODE_NORMAL){
		return;	//���Ų���Ҫ����
	}
	gFDStaIndex[0] = RunStation.ucStation;
	gFDStaIndex[1] = RunStation.ucStation;;
	

	MSG_LOG("dealFDstaGPS:%02X, %02X\r\n", gFDStaIndex[0], gFDStaIndex[1]);
	
	gFDderict = RunStation.ucDirect;

	MSG_LOG("gFDderict:%02X, %02X\r\n", gFDderict, gFDStaIndex[gFDderict]);

	//		i = LineInfo[gFDderict].StationInfo[gFDStaIndex[gFDderict]].ucId;
	//		gFDucID = (((i>>4)*10)+(i&0x0F));
	gZNFDucID[0] = gFDStaIndex[0]+1;	//����š���1����Ϊ��Ŵ�0��ʼ����������Ǵ�1��ʼ�ġ�������վ���š�����վ���ź���Ų�һ��
	gZNFDucID[1] = gFDStaIndex[1]+1;
	gFDucID = gZNFDucID[gFDderict];
	
	MSG_LOG("dealFDstaGPS2 gFDucID:%02X, gZNFDucID=%02X\r\n", gFDucID, gZNFDucID);

}
//����ֶε�վ���� ,
 unsigned int distar1,diend2;// �������յ�ľ���
void dealFengDuanStation(void)
{
	unsigned char i, station;
	unsigned int ds1,ds2;
	
	if(gFDMode == FDMODE_NORMAL){
		return;	//���Ż�����Ҫ��վ��
	}

		station = 0;
		ds2 = 0xffffffff;
		for(i=0; i<LineInfo[0].ucStationMax; i++){//����
			ds1 = GpsDistance(LineInfo[0].StationInfo[i].ulLatitude, LineInfo[0].StationInfo[i].ulLongitude);
			if(ds1 <= ds2){
				ds2 = ds1;
				station = i;
			}
		}

		gFDStaIndex[0] = station;
		
		station = 0;
		ds2 = 0xffffffff;
		for(i=0; i<LineInfo[1].ucStationMax; i++){//����
			ds1 = GpsDistance(LineInfo[1].StationInfo[i].ulLatitude, LineInfo[1].StationInfo[i].ulLongitude);
			if(ds1 <= ds2){
				ds2 = ds1;
				station = i;
			}
		}
		gFDStaIndex[1] = station;
		
		MSG_LOG("\r\ndealFDstaGPS gFDStaIndex:%02X,%02X", gFDStaIndex[0], gFDStaIndex[1]);

		if(RunStation.ucDirect < 2)	//�Ѿ�ȷ������
			gFDderict = RunStation.ucDirect;
		else{
			if(gFDderict > 1){
				gFDderict = 0;
				if((gFDStaIndex[0] == 0)||(gFDStaIndex[1] >= (LineInfo[1].ucStationMax-1)))
					gFDderict = 0;
				else if((gFDStaIndex[1] == 0)||(gFDStaIndex[0] >= (LineInfo[0].ucStationMax-1)))
					gFDderict = 1;
			}
		}
		
		MSG_LOG("gFDderict:");
		BCD_LOG((unsigned char*)&gFDderict, 1, 1);
		BCD_LOG((unsigned char*)&gFDStaIndex[gFDderict], 1, 1);

//		i = LineInfo[gFDderict].StationInfo[gFDStaIndex[gFDderict]].ucId;
//		gFDucID = (((i>>4)*10)+(i&0x0F));
		gZNFDucID[0] = gFDStaIndex[0]+1;	//����š���1����Ϊ��Ŵ�0��ʼ����������Ǵ�1��ʼ�ġ�������վ���š�����վ���ź���Ų�һ��
		gZNFDucID[1] = gFDStaIndex[1]+1;
		gFDucID = gZNFDucID[gFDderict];

		distar1 = GpsDistance(LineInfo[0].StationInfo[0].ulLatitude, LineInfo[0].StationInfo[0].ulLongitude);//��һ��վ��ľ���
		diend2 = GpsDistance(LineInfo[1].StationInfo[0].ulLatitude, LineInfo[1].StationInfo[0].ulLongitude);//���һ��վ��ľ���
		if (distar1<200&&gFDucID==1){//����� ��λ����
			gFDderict=0;
			RunStation.ucDirect=gFDderict;//û��ȷ�������������ڵ�һ��վ�㶨λ����
		}
		if(diend2<200&&gFDucID==1){//���յ�   ��Ϊ����
			gFDderict=1;
			RunStation.ucDirect=gFDderict;
		}
		MSG_LOG("dealFDstaGPS2 gFDucID:");
		BCD_LOG((unsigned char*)&gFDucID, 1, 1);
		MSG_LOG("dealFDstaGPS2 gZNFDucID:");
		BCD_LOG((unsigned char*)&gZNFDucID, 2, 1);
//	}

}
typedef struct  {
	unsigned char Sec;
	unsigned char CardSno[4];
} card_Delay;
extern card_Delay PCard_delay;
unsigned char Delay_FenDuan_Normal(void)
{
	char i;
	if(memcmp(PCard_delay.CardSno,(unsigned char *)&c_serial,4))
	{
		memset(PCard_delay.CardSno,0,4);
		return 0;
	}
	i=SysTime.seconds-PCard_delay.Sec;
	if(i > 3)
	{
		memset(PCard_delay.CardSno,0,4);
		return 0;
	}
	else 
		return 1;
}
extern void SoundMessage(unsigned char cmd);
//����Ƭ����д����Ϣ��
void FengDuan_GPS_card(void)
{
	stFDCard1 *pcard1;
	stFDCard2 *pcard2;
	unsigned char zdbh;
	unsigned int i, t;
	unsigned int delMoney;	//���۵Ľ��
	unsigned char fdcard[2][16];
	unsigned char buff[70];
	unsigned char ucRnt, k;
	
	if(memcmp(gFdLinehead.LineNo, pFistVary.LineNo, 3) != 0){//��·��Ϣ�д���Ҫ��������
		restore_flag = 3;
		MessageBox(1, "��·��Ϣ����������������!");
		beep(8, 50, 50);
		end_card();
		return;
	}

	if(gGPsPar.EnableGps == 0){
		restore_flag = 3;
		MessageBox(1, "û��վ����Ϣ!");
		beep(8, 50, 50);
		end_card();
		return;
	}
	if((gFDderict > 1)||(gFDucID == 0) || (gFDucID > 99)){
		restore_flag = 3;
		MessageBox(1, "վ��û�ж�λ!");
		beep(8, 50, 50);
		end_card();
		return;
	}

	delMoney = 0;
	if(Delay_FenDuan_Normal() !=0 )
		return;
// 	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
// 	{
// 		disp_no_swipe();
// 		end_card();
// 		return ;	
// 	}

	for(i=0; i<3; i++){//�������ڷֶ��շѵ���Ϣ,д���¼�����һ������
		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,cardMessage.dealRecordSector[2]*4)!=0)
		{
			if(MifareRead(cardMessage.dealRecordSector[2]*4,fdcard[0])!=0)
			{
				break;
			}
		}
		PcdRfReset(0);
		profile(0);
	}
	if(i >= 3){//����
		memset(fdcard[0], 0x00, 16);
		memset(fdcard[1], 0x00, 16);
		return;
	}
	else{
		if(MifareRead((cardMessage.dealRecordSector[2]*4)+1,fdcard[1])==0)
		{
			memset(fdcard[1], 0x00, 16);
			return;
		}
	}
	
	restore_flag = 3;

	MSG_LOG("fdcard 1::");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	
	if(MiBlockInvalid(1,15,fdcard[0])==ST_ERROR){
		memset(fdcard[0], 0x00, 16);
	}
	if(MiBlockInvalid(1,15,fdcard[1])==ST_ERROR){
		memset(fdcard[1], 0x00, 16);
	}
	//���洦��Ƭ����

	MSG_LOG("fdcard 2::");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	pcard1 = (stFDCard1 *)fdcard[0];
	pcard2 = (stFDCard2 *)fdcard[1];
	if(pcard1->getupFlag == 0xAA){//ʱ�������Ч
		Ascii2BCD(pFistVary.DeviceNo, buff, 8);
		i = 0;
		memcpy((unsigned char*)&i, pcard2->timeDly, 2);//��������ʱ�䳤 ��λ�֡�
		i *= 60;

		if((memcmp(pcard2->deviceNo, buff, 4)==0)&&	//����ͬ
			(DataTimeDec((unsigned char*)&SysTime, pcard1->getupTime) <= i))//�����ʱ���ڣ�
		{	//����ˢ��
			if(gFDderict == 0)//����
				ucRnt = pcard1->stationNo;
			else
				ucRnt = pcard2->NstationNO;

			if(ucRnt == gFDucID){//վ����ͬ,��������ˢ��
				;
 			//	FengDuan_Normal_card();//�ϳ���Ҳ��Ǯttttttttttttttttttttttttttt
				MessageBox(0,"���ܴ��˳˳�");
				end_card();
 				return;
			}
			else
			{//����ͬ��վ�㶼��ǰ��ˢ�������ֹ�ٴ�ˢ�������߿�Ǯ�³���
// 				MessageBox(0, "�뵽�³���ˢ��!");
// 				led_on(LED_RED);
// 				end_card();
				FengDuan_Normal_card();//�ϳ���Ҳ��Ǯ
				return;
			}
		}
		else{//���۽���������־
			delMoney = 0;
			memcpy((unsigned char*)&delMoney, pcard1->Allprice, 2);
			s_sum1 = (delMoney*pcard1->times);	//һ�ο۹����д�����
			ucRnt = PurseProcess(0);
																	//�����׽���
			MSG_LOG("FD Purse Process = ");
			BCD_LOG(&ucRnt,1, 1);

			switch(ucRnt)   //96ms
			{
			case ST_ERROR:						//��ͨ����
				MessageBox(1, "����Ǯ������!");
				break;
			case 21:
			case RET_MONEY_SN_EROR:																//����Ǯ��������
				error(ERR_MONEY_SN, 0);
				return;
			case 22:
				display(0,0,"��ʾ:", DIS_Cls);
				display(2,0,"Ʊ����Ϣ����ȷ",DIS_CENTER);
				break;
			case 2:
			case 3://����
				led_on(LED_RED);
				break;
			case 4:
			case 6:
				audio(Audio_TRY_AGAIN);		//����ˢ
				card_ser = c_serial;
				cardSound = 0xaa;
				return;
			case 5:
			case 7:
			case 8:
				cardSound=0;
				ComSumeFile.SumMoney+=s_sum1; //��ǰ�����ܶ�
				cardMessage.getdata_w = pcard1->times;	//ˢ������
				BuildFengDuanRecorde(ID_REC_TOLL, buff);
				card_ser = c_serial;
				i = ((unsigned char*)&ComSumeFile.SumMoney)-((unsigned char*)&ComSumeFile);
				sysfewrite(BIT_COMSUME_FILE+i, 4 ,(unsigned char*)&ComSumeFile.SumMoney);
				
				WriteRecord(buff, 0);

				MSG_LOG("FengDuan puchase ok!!\r\n");

				break;
			default:
				break;
			}
			if((ucRnt < 5)||(ucRnt > 8)){
				end_card();
				return;
			}
//����� ���ϳ�
			memset(fdcard[0], 0x00, 16);
			memset(fdcard[1], 0x00, 16);
		}
	}//��������ˢ����
	else{
		memset(fdcard[0], 0x00, 16);
		memset(fdcard[1], 0x00, 16);
	}
	pcard1->getupFlag = 0xAA;
	pcard1->times++;
//	pcard1->stationNo =gZNFDucID[0]; 	//д�뿨�ڵ�����Ǵ�1��ʼ�ġ�

	//@zzc
	zdbh= LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;	
	zdbh = (zdbh>>4)*10+(zdbh&0x0F);
	pcard1->stationNo=zdbh;
//	debugstring("�ϳ�λ�ã�");
//	debugdata((unsigned char*)&zdbh,1,1);
//	memcpy(pcard1->Allprice, gFdLinehead.TottalPrice, 2);//ȫ��Ʊ��
	
	MSG_LOG(" �ҵ��ķ���::");
	BCD_LOG((unsigned char*)&RunStation.ucDirect, 1, 1);
	MSG_LOG(" gFDucID::");
	BCD_LOG((unsigned char*)&gFDucID, 1, 1);
	if(RunStation.ucDirect == 1)//�����յ�Ϊ0��վ��
		i = 1;
	else	//�����յ�Ϊ���վ��
		i = LineInfo[0].StationInfo[LineInfo[0].ucStationMax-1].ucId;//�ָ�Ϊ�ɵ�ǰվ���յ�վ��Ʊ��
	
	MSG_LOG(" i::");
	BCD_LOG((unsigned char*)&i, 4, 1);
	MSG_LOG(" ucId::");
	BCD_LOG((unsigned char*)&LineInfo[0].StationInfo[LineInfo[0].ucStationMax-1].ucId, 1, 1);

	i = (((i>>4)*10)+(i&0x0F));	//վ����  ���������BCD��
	
	if(gFDderict<2)
		k = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//��1����Ϊ��Ŷ��Ǵ�1��ʼ�ġ�
	else
		k = LineInfo[0].StationInfo[gFDucID-1].ucId;

	t = (((	t >>4)*10)+(t&0x0F));	//վ����  ���������BCD��
	t = get2stationPrice(i, k);

	i = 0;
	memcpy((unsigned char*)&i, gFdLinehead.TottalPrice, 2);
	
	MSG_LOG("ȫ��Ʊ��: ");
	BCD_LOG((unsigned char*)&t, 4, 1);
	BCD_LOG((unsigned char*)&i, 4, 1);
	if(PurseProcess(100)==ST_ERROR)//ȡ�����
		return;

	if(t > i)
		t = i;
	if(a_sum1<t)//���С�ڵ��յ�� �����ϳ�
	{
		MessageBox(0,"����");
		sprintf((char*)buff,"���:%d.%dԪ",a_sum1/100,a_sum1%100);
		display(4,0,(char*)buff,0);
		beep(5,50,50);
		end_card();
		return;
	}
	memcpy(pcard1->Allprice, (unsigned char*)&t, 2);
	memcpy(pcard1->LineNo, pFistVary.LineNo, 3);
	memcpy(pcard1->getupTime, (unsigned char*)&SysTime, 7);
	pcard1->Cxor = MiBlockInvalid(0,15,fdcard[0]);
	
	Ascii2BCD(pFistVary.DeviceNo, pcard2->deviceNo, 8);
	memcpy(pcard2->DriverNo, DriveCardNo, 3);
	pcard2->NstationNO = gZNFDucID[1];	//д�뿨�ڵ�����Ǵ�1��ʼ�ġ�
	Get_SerialNum(pcard2->BUSerial);
	//pcard2->getdownTimes   �³�������������
	memcpy(pcard2->timeDly, gFdLinehead.NeedMaxTime, 2);//�ϳ������ʱ�䳤��
	pcard2->Cxor = MiBlockInvalid(0,15,fdcard[1]);

	MSG_LOG("FD need writeData:\r\n");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	//д����Ϣ
	for(i=0; i<3; i++){//�������ڷֶ��շѵ���Ϣ,д���¼�����һ������
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,cardMessage.dealRecordSector[2]*4);
		MifareWrite(cardMessage.dealRecordSector[2]*4,fdcard[0]);
		MifareWrite((cardMessage.dealRecordSector[2]*4)+1,fdcard[1]);

		MifareRead(cardMessage.dealRecordSector[2]*4, buff);
		MifareRead((cardMessage.dealRecordSector[2]*4)+1, buff+16);
		if((memcmp(buff, fdcard[0], 16) == 0)&&(memcmp(buff+16, fdcard[1], 16) == 0)){
			break;
		}
		PcdRfReset(0);
		profile(0);
	}
	if(i >= 3){//д���
		MSG_LOG("End Write FD Error!");
		MessageBox(1, "д������!");
		led_on(LED_RED);
		end_card();
		return;
	}
	cls();
	sprintf((char*)buff, "�ϳ�:%d��", pcard1->times);
	display(0, 0, (char*)buff, 0);
	if(delMoney != 0){
		sprintf((char*)&buff, "����:%d.%02d", delMoney/100, delMoney%100);
		display(2, 0, (char*)buff, 0);
		sprintf((char*)&buff, "���:%d.%02d", a_sum1/100, a_sum1%100);
		display(4, 0, (char*)buff, 0);
	}
	else{
		display(3, 0, "ˢ���ɹ�!", DIS_CENTER);
	}
	display(6, 0, "�³�ʱ���ٴ�ˢ��", 0);
	save_normal();
	//save_h_month();//ȫ�������ܴ��˳˳�
	led_on(LED_GREEN);
	//audio(Audio_SuaKaOK);
	SoundMessage(SOUND_DEAL);
	cardMessage.getdata_w = pcard1->times;
	BuildFengDuanRecorde(ID_REC_FDUp, buff);	//�����ϳ�ˢ����¼.
	WriteRecord(buff, 0);
	end_card();
}

//----------------------------------------------------

unsigned int old_c_serial;
void FengDuan_Normal_card(void)
{
	stFDCard1 *pcard1;
	stFDCard2 *pcard2;
	unsigned short i;
	unsigned char ucUP_Station;

	unsigned char fdcard[2][16];
	unsigned char buff[70];
	unsigned char ucRnt;
	
	old_c_serial = c_serial;

	if((gFDucID >= 100)||(gFDderict>=2))
	{
		MessageBox(1, "վ��û��ȷ��!");
		restore_flag = 3;
		end_card();
	}
	
	for(i=0; i<3; i++){//�������ڷֶ��շѵ���Ϣ,д���¼�����һ������
		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,cardMessage.dealRecordSector[2]*4)!=0)
		{
			if(MifareRead(cardMessage.dealRecordSector[2]*4,fdcard[0])!=0)
			{
				break;
			}
		}
		PcdRfReset(0);
		profile(0);
	}
	if(i >= 3){//����
		memset(fdcard[0], 0x00, 16);
		memset(fdcard[1], 0x00, 16);
		return;
	}
	else{
		if(MifareRead((cardMessage.dealRecordSector[2]*4)+1,fdcard[1])==0)
		{
			memset(fdcard[1], 0x00, 16);
			return;
		}
	}
	
	restore_flag = 3;

	MSG_LOG("fdScard 1::");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);
	
	if(MiBlockInvalid(1,15,fdcard[0])==ST_ERROR){
		memset(fdcard[0], 0x00, 16);
	}
	if(MiBlockInvalid(1,15,fdcard[1])==ST_ERROR){
		memset(fdcard[1], 0x00, 16);
	}
	//���洦��Ƭ����

	MSG_LOG("fdScard 2::");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	pcard1 = (stFDCard1 *)fdcard[0];
	pcard2 = (stFDCard2 *)fdcard[1];
	if(pcard1->getupFlag == 0xAA){
	//	if(gFDderict == 0)//����
			ucUP_Station = pcard1->stationNo;//���������� ����ȡ����ı�ţ�
	//	else
	//		ucUP_Station = pcard2->NstationNO;

		s_sum1 = CountSum((unsigned char *)fdcard[0]);

		MSG_LOG("FDS Purse s_sum1 = ");
		BCD_LOG((unsigned char*)&s_sum1,4, 1);

		memcpy((unsigned char*)&i,gFdLinehead.TottalPrice,2);
// 		memcpy((unsigned char*)&i, pcard1->Allprice, 2);
		
		if(s_sum1 > i){
			MessageBox(1, "Ʊ�۴���!");
			end_card();
			return;
		}

		ucRnt = PurseProcess(0);//�����׽���
	

		MSG_LOG("FDS Purse Process = ");
		BCD_LOG(&ucRnt,1, 1);

		switch(ucRnt)    //96ms
		{
		case ST_ERROR:						//��ͨ����
			MessageBox(1, "����Ǯ������!");
			break;
		case 21:
		case RET_MONEY_SN_EROR:				//����Ǯ��������
			error(ERR_MONEY_SN, 0);
			return;
		case 22:
			display(0,0,"��ʾ:", DIS_Cls);
			display(2,0,"Ʊ����Ϣ����ȷ",DIS_CENTER);
			break;
		case 2:
		case 3://����
			led_on(LED_RED);
			break;
		case 4:
		case 6:
			audio(Audio_TRY_AGAIN);		//����ˢ
			card_ser = c_serial;
			cardSound = 0xaa;
			return;
		case 5:
		case 7:
		case 8:
			cardSound=0;
			ComSumeFile.SumMoney+=s_sum1; //��ǰ�����ܶ�
			cardMessage.getdata_w = pcard1->times;	//ˢ������
			card_ser = 0;

			BuildFengDuanRecorde(ID_REC_TOLL, buff);	//����������Ѽ�¼.
			WriteRecord(buff, 0);

			if(tcpipPacket.packetStatus != RECORD_DEAL)
				tcpipPacket.packetStatus = 0;	//������ͻ�����û�����ݣ���������������֯���ݡ�
			MSG_LOG("FengDuanS puchase ok!!\r\n");
			break;
		default:
			break;
		}
		if((ucRnt < 5)||(ucRnt > 8)){
			end_card();
			return;
		}
	}
	else{
		cls();
		display(0,0,"��ʾ:",0);
		display(3,0,"�����ˢ��!",DIS_CENTER);
//		beep(1, 100, 100);
		end_card();
		return;
	}
	pcard1->getupFlag = 0xAA;
	if(pcard1->times > 0)
		pcard1->times--;
	if(pcard1->times == 0){
		memset(fdcard[0], 0, 16);
		memset(fdcard[1], 0, 16);
	}
	else{
		pcard1->Cxor = MiBlockInvalid(0,15,fdcard[0]);
		pcard2->getdownTimes++;   //�³�������������
		pcard2->Cxor = MiBlockInvalid(0,15,fdcard[1]);
	}

	MSG_LOG("FD needS writeData:\r\n");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	//д����Ϣ
	for(i=0; i<5; i++){//�������ڷֶ��շѵ���Ϣ,д���¼�����һ������
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,cardMessage.dealRecordSector[2]*4);
		MifareWrite(cardMessage.dealRecordSector[2]*4,fdcard[0]);
		MifareWrite((cardMessage.dealRecordSector[2]*4)+1,fdcard[1]);

		MifareRead(cardMessage.dealRecordSector[2]*4, buff);
		MifareRead((cardMessage.dealRecordSector[2]*4)+1, buff+16);
		if((memcmp(buff, fdcard[0], 16) == 0)&&(memcmp(buff+16, fdcard[1], 16) == 0)){
			break;
		}
		PcdRfReset(0);
		display(6, 0, "  ���ٴ�ˢ��!   ", DIS_CONVERT);
		while(1){
			if(profile(0) == ST_OK){
				if(old_c_serial != c_serial){
					display(6, 0, " ��ˢͬһ�ſ�!  ", DIS_CONVERT);
				}
				else{
					break;
				}
			}
			delayxms(100);
			beep(1, 50, 0);
		}
	}
	if(i >= 5){//д���
		debugstring("End Write FD Error!");
		MessageBox(1, "д������!");
		led_on(LED_RED);
		end_card();
		return;
	}
	cls();

	if((gFDderict < 2)&&(gFDStaIndex[gFDderict] < 100))
		strcpy((char*)buff, LineInfo[0].StationInfo[ucUP_Station-1].name);

	buff[12] = 0;
	strcat((char*)buff, "�ϳ�");
	display(0, 0, (char*)buff, 0);
	if(s_sum1 != 0){
		sprintf((char*)&buff, "�ۿ�:%d.%02d", s_sum1/100, s_sum1%100);
		display(2, 0, (char*)buff, 0);
		sprintf((char*)&buff, "���:%d.%02d", a_sum1/100, a_sum1%100);
		display(4, 0, (char*)buff, 0);
	}
	else{
		display(2, 0, "�ۿ�:0.00Ԫ", 0);
		display(4, 0, "ˢ�����!", 0);
	}
	sprintf((char*)buff, "����Ҫˢ%d��", pcard1->times);
	display(6, 0, (char*)buff, DIS_CENTER);
	
	sprintf((char*)buff, "%d.%02d", s_sum1/100, s_sum1%100);
	LED_Dis3((char*)buff);
	if(pcard1->times == 0)
		save_normal();
	led_on(LED_GREEN);
	audio(Audio_DONG);
	end_card();
}

//------------��������ͨѶ������------
//�����ļ����������Ż��ú�������ǰ�Ż���������.
void FDDownLoadInfo_HM(unsigned char DownMode)
{
	unsigned char sbuf[32];
	unsigned int slen=0;

	if(DownMode != FDDownPar.DownFlag){//���Ͳ�һ�£���0��ʼ
		memset(FDDownPar.DownPerSerial, 0, 2);
		memset(FDDownPar.DownSerial, 0, 2);
	}
	if((DownMode == FDProcess_GETGPSLineInfo)||(DownMode == FDProcess_GETFDticket)||(DownMode == FDProcess_GETPriceRate)||(DownMode == FDProcess_GETProgram)){
		FDDownPar.DownFlag = DownMode;
		memcpy(sbuf, (unsigned char*)&FDDownPar, 5);
		memset(sbuf+5, 0, 2);
		slen = 7;
	}
	else{	//�����Ĳ�֪����ʲô��
		return;
	}
	FDCMDData_Send(FDCMD_DOWNPar, sbuf, slen);
}

//����ˢ��ʱ��ʾ����ʾ����ˢ��ԭ��
int NoCardShow_ALL(void)
{
	int ret;
	char dspb[32];

	if(gFDRunStatues&FDRUNSTATUES_PJ){
		sprintf(dspb, "Ʊ�۲�������:%02X", gFDRunStatues);
		ret = 1;
	}
	else if(gFDRunStatues&FDRUNSTATUES_Line){
		sprintf(dspb, "��·��Ϣ����:%02X", gFDRunStatues);
		ret = 2;
	}
	else if(gFDRunStatues&FDRUNSTATUES_TX){
		sprintf(dspb, "CANͨѶ����:%02X", gFDRunStatues);
		ret = 3;
	}
	else if(gFDRunStatues&FDRUNSTATUES_GPS){
		sprintf(dspb, "GPS��λ����:%02X", gFDRunStatues);
		ret = 4;
	}
	else if((gFDderict>=2)||(gFDucID == 0)||(gFDucID > 100))
	{
		sprintf(dspb, "δȷ��λ��");
		ret = 5;
	}
	else if(gFDRunStatues != 0){
		sprintf(dspb, "��������:%02X", gFDRunStatues);
		ret = 6;
	}
	else{
		ret = 0;
	}
	if(ret != 0){
		cls();
		display(2,0,dspb, DIS_CENTER);
		sprintf(dspb, "����:%d", ret);
		display(0,0,dspb,0);
		display(4, 0, "������ˢ��", DIS_CENTER);
	}
	return ret;
}
//----------------------------------------------------
//ǰ�Ż�-�������Ĵ�����
void FengDuan_BUS_card(void)
{
	if(NoCardShow_ALL() != 0){//�д��󣬲���ˢ��
		delayxms(500);
		end_card();
		restore_flag = 3;
		return;
	}

	if(gFDMode == FDMODE_NORMAL){
		if(gFDdealProcess <= FDProcess_GETProgram){//�ڸ��¸��ֲ���ʱ�����Ż�����ˢ����
//			display(4, 0, "����ˢ��", DIS_CENTER);
			return;
		}
		FengDuan_Normal_card();
	}
	else{
		FengDuan_GPS_card();
	}
}

//��ʾ����
void FengDuan_disp(void)
{
	if(gFDMode == FDMODE_GPS)
		FengDuan_GPS_disp_QM();
	else if(gFDMode == FDMODE_NORMAL)
		FengDuan_Normal_disp_HM();
}

//�����ݷ��͵�ǰ�Ż���
unsigned int FDUPDateRecord_HM(void)
{
	unsigned char sbuff[300];

	if(tcpipPacket.packetStatus == RECORD_DEAL){//����Ҫ���͵�����
		if(tcpipPacket.length > 0){
			memcpy(sbuff, tcpipPacket.tcpBuffer, tcpipPacket.length);	//��ΪFDCMDData_Send�����з�����ͬһ��buff��������Ҫ����buff
			FDCMDData_Send(FDCMD_UPDateREC, sbuff, tcpipPacket.length);
			return tcpipPacket.length;
		}
		else{
			tcpipPacket.packetStatus = 0;//�˼�¼��������
			sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
		}
	}
	return 0;
}

//���ɼ�¼�����������С�
unsigned int FDRecord_Build_HM(void)
{
	unsigned int curp, headp, len, addr;
	unsigned char rbuff[256];
	unsigned char disbuf[RECORD_LEN+8];
	
	if(tcpipPacket.packetStatus==RECORD_DEAL)
	{//�������м�¼��
		return 0;
	}
	else
	{
		if(tcpipPacket.packetStatus>0)
			tcpipPacket.packetStatus--;

		if((tcpipPacket.packetStatus == 0) || (tcpipPacket.packetStatus > 60))
			tcpipPacket.packetStatus = 0;
		else{
//			MSG_LOG("RBuild,packetStatus=%d\r\n", tcpipPacket.packetStatus);
			return 0;
		}
	}

	curp = Get_Record_point((unsigned char*)&headp, 0);
	//�����¼������û�м�¼�ɷ�������E2PROM��û�з��ɹ��ļ�¼
	if(curp>headp)
	{
		MSG_LOG("Read Record From E2prom to CAN !\r\n");
		len=0;
		rbuff[len++] = 0x01;	//0x01-��¼����
		
		addr = headp/RECORD_LEN;
		memcpy(rbuff+len, (unsigned char*)&addr, 2);
		len += 2;

		
		addr = headp;
		flashread(addr, disbuf, RECORD_LEN);
		if(memcmp(rbuff+len, "��������", 8) == 0){//
			memcpy(rbuff+len, disbuf, (RECORD_LEN-8));
			len += (RECORD_LEN-8);//56
			
			if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//������д��ȫ0��
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}
			
			flashread(addr, rbuff+len, RECORD_LEN);
			len += RECORD_LEN;//64
			
			if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//������д��ȫ0��
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}
			
			flashread(addr, rbuff+len, 30);
			len += 30;//30          56+64+30 = 150
		}
		else{
			memcpy(rbuff+len, disbuf, RECORD_LEN);
			len+=RECORD_LEN;
		}
		
		if(addr < FLASH2_START1){//��ǰ����д0,��ʷ����д0
			memset(disbuf, 0, RECORD_LEN);
			flashwrite(addr, disbuf, RECORD_LEN);//������д��ȫ0��
		}
		
		tcpipPacket.packetStatus = RECORD_DEAL;
		tcpipPacket.length = len;//Ҫ���͵ĳ���
		memcpy(tcpipPacket.tcpBuffer, rbuff, len);//Ҫ���͵�����
		sysfewrite(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket);

		MSG_LOG("CAN���ɷ��Ͱ����:");
		BCD_LOG((unsigned char*)&headp, 4, 1);
		BCD_LOG((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
		BCD_LOG((unsigned char*)&tcpipPacket.length, 2, 1);
		BCD_LOG((unsigned char*)&tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
		return len;
	}
	else{
		if(curp >= (FLASH1_START+FLASH_RCLR)){//0x3E800�����¼��ַ�Ѿ��ﵽ��40000����64�ֽڣ��������ô����������¿�ʼ
			lib_clr_Record_point();
		}
		tcpipPacket.packetStatus = 40; //û�м�¼����40�롣
		return 0;
	}
}

//ÿ���ӻ����һ�Ρ�
void FD1secondPro_ALL(void)
{	
	//unsigned char i;
	char disbuf[32];
	//-------��ʾʵʱ��Ϣ
	if(gFDderict < 2){
		if(gOlderStation != gFDucID){		//վ����б仯����Ҫ������ʾ
			gOlderStation = gFDucID;
			restore_flag = 3;	//��Ҫ����ˢ����ʾ
			/*memset(disbuf, 0, 20);
			MSG_LOG("gOlderStation:%d gFDucID:%d\r\n", gOlderStation, gFDucID);
			if(gFDMode == FDMODE_GPS){
				i = gFDStaIndex[gFDderict];	//ǰ�Ż��������������õ���
				if(strlen(LineInfo[gFDderict].StationInfo[i].name) <= 10)
					strcpy(disbuf, "λ��:");
			}
			else{
				i = gFDucID-1;				//�����������������������Ǵ�ǰ�Ż���ѯ��Ϣ��õ��ġ�//��1����Ϊ��Ŷ��Ǵ�1��ʼ�ġ�
			}
			strcat(disbuf, LineInfo[gFDderict].StationInfo[i].name);
			if(gFDMode == FDMODE_GPS){
				display(0,1,disbuf,DIS_ClsLine);
			}
			else{
				display(2,1,disbuf,DIS_CENTER|DIS_ClsLine);
			}*/
		}
	}
	//-----------------------
	if(gFDMode == FDMODE_NORMAL){
		sprintf(disbuf, "%02X", gFDdealProcess);
		miniDispstr(7, 19, disbuf, 1);
		if(gFDmissflag != 0){
			sprintf(disbuf, "%02X", gFDmissflag);
			miniDispstr(6, 19, disbuf, 1);
		}
		FDRecord_Build_HM();//��Ҫ���ļ�¼�ƶ��������������
	}
}

void FDFindMiss_HM(void)
{
	if(gFDmissflag & FDMISS_GETLine){//��Ҫȡ��·��Ϣ
		gFDdealProcess = FDProcess_GETGPSLineInfo;
		if(FDDownPar.DownFlag != FDProcess_GETGPSLineInfo){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else if(gFDmissflag & FDMISS_GETPriceRate){//��ҪƱ���ۿ���,��ȡ�ۿ۱�Ȼ����ȡ����Ʊ�ۡ���Ϊ�Ƿ�����Ǵӽ���Ʊ�۵İ汾�Ÿ��µġ�
		gFDdealProcess = FDProcess_GETPriceRate;
		if(FDDownPar.DownFlag != FDProcess_GETPriceRate){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else if(gFDmissflag & FDMISS_GETTicket){//��Ҫ����Ʊ��
		gFDdealProcess = FDProcess_GETFDticket;
		if(FDDownPar.DownFlag != FDProcess_GETFDticket){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else if(gFDmissflag & FDMISS_GETProgram){//��Ҫ���³���
		gFDdealProcess = FDProcess_GETProgram;
		if(FDDownPar.DownFlag != FDProcess_GETProgram){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else{
		gFDdealProcess = FDProcess_GETFree;
	}
	restore_flag = 3;	//���������������ˢ����ʾ��
	gFDScount = 0;	//����ִ��
	set_timer1(1000);
}
//�ֶ�������,�������ݼ�����
void FengDuan_main(void)
{
	unsigned char Tbuf[16];	//�������ݷ��ͻ��壬��֯���ݷ���ʱ���˻�������ݻ�ı�Ϊ�����ķ���ָ�����ҪԤ���㹻�Ŀռ䡣
	CanFDdealCMD_ALL(irda_rxBuf);

	if(gFDMode == FDMODE_NORMAL){	//ֻ�к��Ż��Ż���һϵ������
		switch(gFDdealProcess){
		case FDProcess_GETFind:
			FDFindMiss_HM();	//��ѯ����ֻ�к��Ż���ִ�С�
			break;
		case FDProcess_GETVER:
			if(gFDScount==0){//ȡǰ�Ż��汾��
				FDCMDData_Send(FDCMD_VERCX, Tbuf, 0);
			}
			break;
		case FDProcess_GETPriceRate:
		case FDProcess_GETGPSLineInfo:
		case FDProcess_GETFDticket:
		case FDProcess_GETProgram:
			if(gFDScount==0){
				FDDownLoadInfo_HM(gFDdealProcess);
			}
			break;
		case FDProcess_GETFree:
			if(gFDScount == 0){
				if((gFDGetGPSinfo%4)==0){//4���ӣ�ȡһ��ǰ����Ϣ��
					MSG_LOG("ȡ��Ϣ \r\n");
					FDCMDData_Send(FDCMD_GPSINFO, Tbuf, 0);
					gFDGetGPSinfo++;
				}
				else{
					if(FDUPDateRecord_HM() != 0){//���ͼ�¼, =1�м�¼�Ϸ�
						;
					}
				}
				
			}
			break;
		default:
			gFDdealProcess = FDProcess_GETFree;
			break;
		}
	}
	
	if(get_timer1() == 0){
		set_timer1(1000);
		FD1secondPro_ALL();
		if(gFDScount>0)
			gFDScount--;
		gFDGetGPSinfo++;
	}
}

#endif	//#ifdef FengDuan_BU_





