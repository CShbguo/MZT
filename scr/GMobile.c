
#include <stdlib.h>
#include "szct.h"
#include "GMobile.h"
#include "E_Deal24GCard.h"
#include "E_CPUCard.h"
#include "E_GJGPRS.h"
#include "Mydes.h"
#include "DEF8583.h"

//const 
//#define AT91C_IFLASH ((char *)0x00100000) // Internal FLASH base address

extern unsigned char gTTime[14];
extern unsigned char GPRSLinkProcess;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned int a_sum1,s_sum1;
extern unsigned char restore_flag;
extern unsigned char gsendError;//���ʹ���������������ڳ����Ͷ������Ӧʱֹͣ����
extern unsigned char gchn_eng;//��Ӣ�ı�־
extern unsigned short gErrorFlag;//�����ʶ,���ֽڱ�ʾ�����ţ����ֽڱ�ʾ������� ���"*��������ʶ����"
extern unsigned int gErrortimes[2];//���������,��Ҫ������GPRS����
extern stPricetable NewPriceTab;
extern pFistVary_1 pFistVary;

unsigned char gMACKEY_M[16];//���ܵ�MAC KEY����


//msgCMD �յ�������  BM λͼ  databuf ��λͼ������
extern void deal8583Dat(unsigned short msgCMD, unsigned char BM, unsigned char *datbuf, unsigned short len);
//extern void DEBUG_It( void );
extern unsigned char getmobileADD(void);
void uploadMobilePara(void);
void first_clr_mobile_para(void);
void find_new_mission(void);
extern void stopCCgprs(unsigned short mode);

extern unsigned char gprsSNDerro;//�ͺ�̨ͨѶ���ֵĴ���
extern PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//����֡����

void IF2TLVDeal(unsigned char *pakege);
void IF18583Deal(unsigned char *pakege);

void in_gErrortimes(unsigned char val, unsigned char mode)
{
#ifdef NO_MOBILE_SERVISE
	gErrortimes[0] = 20;
	gErrortimes[1] = 20;
#else  
	unsigned int i;
	i = ((pFistVary.DeviceNo[7]-'0')%10);
	if(i > 9)
		i = 9;
	if(mode == ERROR_CONNECT_MOBILE)
		gErrortimes[0] = 120+(i*60);//��ʱ300��,
	else
		gErrortimes[1] = 120+(i*30);//��ʱ300��,
#endif
}
void in_IF2TLVDeal(unsigned char sindex)
{
#ifndef NO_MOBILE_SERVISE
	IF2TLVDeal((unsigned char*)&gPacketRcvBuf[sindex].LinkFrame);
#else
	;
#endif
}

void in_IF18583Deal(unsigned char sindex)
{
#ifndef NO_MOBILE_SERVISE
	IF18583Deal((unsigned char*)&gPacketRcvBuf[sindex].LinkFrame);
#else
	;
#endif
}

void set_KEK_SN(unsigned char val)
{
#ifndef NO_MOBILE_SERVISE
	gMobileParameter.KEK_SN = val;
#else
	;
#endif
}

int checkKEK(void)
{
#ifdef NO_MOBILE_SERVISE
	return 0;
#else
	gprsSNDerro += 2;
	if(gprsSNDerro > 20){
		if(gmissflag == MISS_M_SINGIN){//��½ʱ��̨��Ӧ��,��Ҫ��������KEK��
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
			gMobileParameter.KEK_SN = 0xff;
			gprsSNDerro = 0;
			return -1;
		}
	}
	return 0;
#endif
	
}

#ifndef NO_MOBILE_SERVISE
void readmobilsendStyle(void)
{
	sysferead(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);//�����Ѿ����͵���Ϣ
}

void setMobilesv(void)
{
/*	unsigned char i;
	unsigned int tm;
	unsigned char x,col;
	unsigned char buffer[2][20];
	unsigned char Ipbuf[5];
	unsigned int uitemp;
	unsigned long ultempIP;

	memset(Ipbuf, 0, 4);
ip_start:
	i=0;
	x=0;
	memset(buffer[0],0,20);
	if(checkIP((char*)gMobileParameter.purIP) == 0){
		memcpy(gMobileParameter.manageIP, "100.100.100.100",15);
		memcpy(gMobileParameter.managePort,"00000\x0",6);
	}
	memcpy(buffer[0], gMobileParameter.manageIP, 15);

	cls();
	if(gchn_eng == 'E')
		display(0,0,(char *)"mobile IP:",1);
	else
		display(0,0,(char *)"�����ƶ�IP:",1);
	display(2,1,(char*)buffer[0],0);
	buffer[0][18]=buffer[0][0];
	display(2,1,(char*)buffer[0]+18,1);

	memset(buffer[1], 0, 20);
	memcpy(buffer[1], gMobileParameter.managePort, 6);
	display(4, 1, (char*)buffer[1], 0);
	col = 0;
	while(1)
	{
		delayxms(50);
		i=getkey(1);
		switch(i)
		{
		case KEY_F1:
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x>0) x--;
				else{
					col = 1;
					x = 4;
				}
			}
			else{
				if(x>0) x--;
				else{
					col = 0;
					x = 14;
				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x--;
			}

			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_UP:
			if((buffer[col][x]<0x30)||(buffer[col][x]>0x39)) buffer[col][x]=0x39;
			if(buffer[col][x]==0x39) buffer[col][x]=0x30;
			else if(buffer[col][x]<0x39) buffer[col][x]++;
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_F2:
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x<14) x++;
				else{
					col = 1;
					x = 0;
				}
			}
			else{
				if(x<4) x++;
				else{
					col = 0;
					x = 0;
				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x++;
			}
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_ESC:		//-10
//			ClientIP=tm;
			return;
		case KEY_DOWN:
			if((buffer[col][x]<0x30)||(buffer[col][x]>0x39)) buffer[col][x]=0x30;
			if(buffer[col][x]==0x30) buffer[col][x]=0x39;
			else if(buffer[col][x]>0x30) buffer[col][x]--;
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_ENT:		//ok
//			if(x>=14)
//			{	
				uitemp=(buffer[0][0]-0x30)*100+(buffer[0][1]-0x30)*10+(buffer[0][2]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][4]-0x30)*100+(buffer[0][5]-0x30)*10+(buffer[0][6]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][8]-0x30)*100+(buffer[0][9]-0x30)*10+(buffer[0][10]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][12]-0x30)*100+(buffer[0][13]-0x30)*10+(buffer[0][14]-0x30);
				if(uitemp>255) goto ip_error;
				
				Ipbuf[0]=(buffer[0][0]-0x30)*100+(buffer[0][1]-0x30)*10+(buffer[0][2]-0x30);
				Ipbuf[1]=(buffer[0][4]-0x30)*100+(buffer[0][5]-0x30)*10+(buffer[0][6]-0x30);
				Ipbuf[2]=(buffer[0][8]-0x30)*100+(buffer[0][9]-0x30)*10+(buffer[0][10]-0x30);
				Ipbuf[3]=(buffer[0][12]-0x30)*100+(buffer[0][13]-0x30)*10+(buffer[0][14]-0x30);
				memcpy((unsigned char*)&tm, Ipbuf, 4);		
				ultempIP=tm; 
				//debugdata((unsigned char *)&ClientIP, 4, 1);
				tm = (buffer[1][0]-0x30)*1000+(buffer[1][1]-0x30)*100+(buffer[1][2]-0x30)*10+(buffer[1][3]-0x30);
				//debugdata((unsigned char*)&ClientPort, 2, 1);
				i=0;
				if(ultempIP<0x00FFFFFFl) i |= 0x10;
				if((tm<100)||(tm>9999)) i |= 0x20;
				if(i&0x30) {
					cls();
					if(gchn_eng == 'E'){
						display(0,0,(char *)"input error:",0);
						display(2,1,"data illegal",0);
					}
					else{
						display(0,0,"�������:",0);
						display(2,1,"IP��ַ���Ϸ�",0);
					}
					delayxs(1);
					goto ip_start;
				}

				memcpy(gMobileParameter.manageIP, buffer[0], 15);
				memcpy(gMobileParameter.managePort, buffer[1], 6);
				memcpy(gMobileParameter.purIP, buffer[0], 15);
				memcpy(gMobileParameter.purPort, buffer[1], 6);
				gMobileParameter.purPort[5] = 0;
				gMobileParameter.managePort[5] = 0;
//				debugstring("�������:");
//				debugdata(gMobileParameter.purIP, 15, 1);
//				debugdata(gMobileParameter.purPort, 6, 1);
				cls();
				if(gchn_eng == 'E')
					display(2,4,"complete",0);
				else
					display(2,4,"�������",0);
				uploadMobilePara();
#ifndef NO_MOBILE_SERVISE
				first_clr_mobile_para();//IP��ַ���ˣ���Ϊ����˱仯��������KEK
#endif
				return; 
ip_error:
				cls();
				if(gchn_eng == 'E'){
					display(0,0,(char *)"input error:",0);
					display(2,1,"data illegal",0);
				}
				else{
					display(0,0,"�������:",0);
					display(2,1,"����ֵҪС��256",0);
				}
				goto ip_start;
//			}
//			else
//			{
//				buffer[18]=buffer[x];
//				Display(2,1+x,buffer+18,0);
//				x++;
//				if((x==3)||(x==7)||(x==11)) x++;
//
//				buffer[18]=buffer[x];
//				Display(2,1+x,buffer+18,1);
//
//			}
//			break;
		default:
			break;
		}
	}*/
}
//-------------------------------------------------------------------------------------------------
void load_fe_SendSeril(void)
{
	unsigned char buff[10];
	unsigned char t;
	unsigned char bcc;
	
	feread(BIT_SendSeril, 5, buff);
	bcc = 0x55;
	for(t=0;t<4;t++)
		bcc ^= buff[t];
	if(bcc == buff[4])
		memcpy((unsigned char*)&gSendSeril, buff, 4);
	else
		gSendSeril = 0xffffffff;
}
void save_FE_SendSeril(void)
{
	unsigned char buff[10];
	unsigned char t;

	memcpy(buff, (unsigned char*)&gSendSeril,4);
	buff[4] = 0x55;
	for(t=0; t<4; t++)
		buff[4] ^= buff[t];
	sysfewrite(BIT_SendSeril, 5, buff);
}
unsigned char checkPiCiNo(void)
{
	unsigned char i,h;
	unsigned int addr;
	unsigned char dt[sizeof(stMobileParameter)+5];
	unsigned char crc;

	for(i=0; i<6; i++){
		if((gMobileParameter.posPiCiNo[i] < '0')||(gMobileParameter.posPiCiNo[i] > '9'))
			break;
	}

	if(i<6){
#ifdef _debug_
		debugstring("chck 0 BIT_PICINO_ Err:");
		debugdata(gMobileParameter.posPiCiNo, 7, 1);
#endif
		addr = FLASH_MOBILE_START;//���¶�ȡ����
		flashread(addr, dt, sizeof(stMobileParameter));
		memcpy((unsigned char*)&gMobileParameter, dt, sizeof(stMobileParameter));
	}
	else
		return ST_OK;
	
	for(i=0; i<6; i++){
		if((gMobileParameter.posPiCiNo[i] < '0')||(gMobileParameter.posPiCiNo[i] > '9'))
			break;
	}

	if(i<6){
		feread(BIT_PICINO_BAK, 7, dt);
#ifdef _debug_
		debugstring("chck 1 BIT_PICINO_BAK:");
		debugdata(dt, 7, 1);
#endif
		crc = 0x55;
		for(h=0; h<6; h++)
			crc ^= dt[h];
		for(h=0; h<6; h++){
			if((dt[h]<'0')||(dt[h]>'9')){//�����������ݲ���ȷ
				dt[6] = 0xff;
				crc = 0;
				break;
			}
		}
		if(dt[6] == crc){
			memcpy(gMobileParameter.posPiCiNo, dt, 6);
			uploadMobilePara();
			return ST_OK;
		}
		else{
			first_clr_mobile_para();//�������ز���
			return ST_ERROR;
		}
	}
	return ST_OK;
}

unsigned int dealPiciNo(void)
{
	unsigned char i,h;
	unsigned int t;
	unsigned char dt[20];
	unsigned char crc;

start:
	t=0;
	for(i=0;i<6;i++){
		if((gMobileParameter.posPiCiNo[i] < '0')||(gMobileParameter.posPiCiNo[i] > '9')){//���κ��д�ʹ��ǰһ�����κ�
//			beep(8, 50, 50);
			feread(BIT_PICINO_BAK, 7, dt);
#ifdef _debug_
			debugstring("BIT_PICINO_BAK:");
			debugdata(dt, 7, 1);
//			while(1);
#endif
			crc = 0x55;
			for(h=0; h<6; h++)
				crc ^= dt[h];
			for(h=0; h<6; h++){
				if((dt[h]<'0')||(dt[h]>'9')){//�����������ݲ���ȷ
					dt[6] = 0xff;
					crc = 0;
					break;
				}
			}
			if(dt[6] == crc){
				memcpy(gMobileParameter.posPiCiNo, dt, 6);
				uploadMobilePara();
				goto start;
			}
			else{
				first_clr_mobile_para();//�������ز���
				return 0;
			}
		}
		t *= 10;
		t += (gMobileParameter.posPiCiNo[i]-'0');
	}
	return t;
}
//�������
void uploadSettleData(void)
{
	gM_Senddll = 0;
	if(memcmp(mobilsendStyle.Single, "���ܴ���", 8)==0)//������Ѿ��������ٴ���
		return;
	else if(memcmp(mobilsendStyle.Single, "���ڷ���", 8) == 0){
		memcpy(mobilsendStyle.Single, "���ܴ���", 8);
		mobilsendStyle.EndAddr = gMobilREC_End;
	}
	else{
		memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
		mobilsendStyle.StartAddr = gMobilREC_Start;
		mobilsendStyle.SendAddr = gMobilREC_Start;
		mobilsendStyle.EndAddr = gMobilREC_End;
		mobilsendStyle.SendPiCI = dealPiciNo();
		memcpy(mobilsendStyle.Single, "���ܴ���", 8);
		mobilsendStyle.EndAddr = gMobilREC_End;
	}
	sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
	find_new_mission();
}

void uploadMobileData(void)
{
// 	debugstring("�����ͽ���:\r\n");
// 	debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
// 	debugdata((unsigned char*)&gMobilREC_End, 4, 1);
	if(gmissflag == MISS_G_DPRO)//�����³���
		return;
	if(gMobilStyle.ISOK != 1){
		if(F_FLAG_VARY.stop_flag)
			gmissOldflag = MISS_M_TradeBatch;
		gmissflag = MISS_M_SINGIN;//����ǩ��
		return;
	}

	if(memcmp(mobilsendStyle.Single, "���ܴ���", 8) == 0){
		if(mobilsendStyle.EndAddr == mobilsendStyle.SendAddr){
			if(mobilsendStyle.all_number == 0){//���͵�������Ϊ��������Ҫ������
				memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
			}
			else
				gmissflag = MISS_M_Settlement;//������ݴ������������
		}
		else{
//			if(gMobilREC_Start == gMobilREC_End){
				gmissflag = MISS_M_TradeBatch;//����������
//			}
		}
	}
	else if(memcmp(mobilsendStyle.Single, "���ڷ���", 8) == 0)
	{
		gmissflag = MISS_M_TradeBatch;//����������
	}
	else{
		memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
		if(F_FLAG_VARY.stop_flag)
			memcpy(mobilsendStyle.Single, "���ܴ���", 8);
		else
			memcpy(mobilsendStyle.Single, "���ڷ���", 8);
		mobilsendStyle.StartAddr = gMobilREC_Start;
		mobilsendStyle.SendAddr = gMobilREC_Start;
		mobilsendStyle.EndAddr = gMobilREC_End;
		mobilsendStyle.SendPiCI = dealPiciNo();
		sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
	}
	if(GPRSLinkProcess == 20)
		set_timer0(100, 1);
//	restore_flag = 3;
}

//�������
void uploadMobilePara(void)
{
	unsigned int addr;

	addr = FLASH_MOBILE_START;
	flasherase(ERASE4KBYTESECTOR, addr);
	flashwrite(addr, (unsigned char*)&gMobileParameter, sizeof(stMobileParameter));

}

//ȡǩ����Ϣ��������Ϣ ֻ�ڿ���ʱ����һ��
void getMobilePara(void)
{
	unsigned char i;
	unsigned int addr;
//	unsigned short sst;
	unsigned char buff[sizeof(stMobileParameter)+1];
	unsigned char busSeril[16];

	getGpsProVER(buff);//����汾��
	memcpy(gmobileProVer, buff, 14);
	for(i=0;i<14;i++){
		if((gmobileProVer[i]>'9')||(gmobileProVer[i]<'0')){
			memset(gmobileProVer, '0', 14);
			break;
		}
	}

	getposSN(busSeril, 0);
#ifdef _debug_
	debugstring("�������к�:");
	debugdata(busSeril, 16, 1);
#endif

	for(i=0; i<1; i++){
#ifdef BUS_CPU_
		if(CPUPsamInitialize() == ST_OK)
			break;
#else
#ifndef NO_MOBILE_24G_
		if(G24PsamInitialize() == ST_OK)
#endif
#endif
			break;
	}

	addr = FLASH_MOBILE_START;//��ȡ�ƶ�������������KEK
	flashread(addr, buff, sizeof(stMobileParameter));
	memcpy((unsigned char*)&gMobileParameter, buff, sizeof(stMobileParameter));
	
#ifdef _debug_
	debugstring("gMobileParameter:");
	debugdata((unsigned char*)&gMobileParameter, sizeof(stMobileParameter), 1);
	debugstring("KEK SN:");
	debugdata((unsigned char*)&gMobileParameter.KEK_SN, 1, 1);
	debugstring("KEK:");
	debugdata(gMobileParameter.KEK, 48, 1);
#endif
	
#ifdef _debug_
	debugstring("gMobileParameter.addPICINO_limit:");
	debugdata((unsigned char*)&gMobileParameter.addPICINO_limit, 1, 1);
	debugstring("gMobil_Settlement_error:");
	debugdata((unsigned char*)&gMobileParameter.gMobil_Settlement_error, 1, 1);
#endif
	if(gMobileParameter.addPICINO_limit > 11){
		gMobileParameter.addPICINO_limit = 0;
	}
#ifdef _debug_
	debugstring("gMobileParameter.addPICINO_limit:");
	debugdata((unsigned char*)&gMobileParameter.addPICINO_limit, 1, 1);
#endif

	if(gMobileParameter.KEK_SN > 3){//û��KEK��������KEK
#ifdef _debug_
		debugstring("û��KEK\r\n");
#endif
		memset((unsigned char*)&gMobilStyle, 0, sizeof(stMobilStyle));		//�����µ�½
		sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
		gmissOldflag = MISS_M_DPARAM;//�����ϴ����������־,ִ���걾������󷵻���һ����
		gmissflag = MISS_M_KEK;
	}
	feread(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
#ifdef _debug_
	debugstring("gMobilStyle.SingInDate:");
	debugdata((unsigned char *)&gMobilStyle.SingInDate, 7, 1);
#endif
	if(time_invalid(gMobilStyle.SingInDate) != 0){//ǩ��ʱ����Ч
#ifdef _debug_
		debugstring("gMobilStyle:");
		debugdata((unsigned char *)&gMobilStyle, 60, 1);
		debugdata((unsigned char*)&SysTime, 7, 1);
#endif
		if(memcmp((unsigned char*)&SysTime, gMobilStyle.SingInDate, 4)==0){//���켺��ǩ��
			i = (gMobileParameter.KEK_SN*16);
#ifdef _debug_
			debugstring("ǩ����Ч:i=");
			debugdata((unsigned char*)&i, 1, 1);
#endif
			memset(gMACKEY_M, 0, 16);
			
 			DES3_decrypt(gMobilStyle.MAC_KEY, gMobileParameter.KEK+i, gMACKEY_M);//����MAC_KEY
 			DES3_decrypt(gMobilStyle.MAC_KEY+8, gMobileParameter.KEK+i, gMACKEY_M+8);
#ifdef _debug_
			debugstring("gMobilStyle.LoadStyle:");
			debugdata((unsigned char*)&gMobilStyle.LoadStyle, 1, 1);
#endif
			gMobilStyle.ISOK = 1;//��ǩ����Ч
			if((gMobilStyle.LoadStyle & LoadStyle_KEK)!=0){
				gmissflag = MISS_M_KEK;//��Ҫ����KEK
			}
			else if((gMobilStyle.LoadStyle & LoadStyle_para)!=0){
				gmissflag = MISS_M_DPARAM;//��Ҫ���ز���
			}
			else if((gMobilStyle.LoadStyle & LoadStyle_program)!=0){
				gMobilStyle.LoadStyle &= (~LoadStyle_program);//�����ֻ�POSP���س���
//				gmissflag = MISS_M_DPGRAM;//��Ҫ���س���  �ϰ汾�����س���
//				gmissflag = MISS_M_SINGIN;//��ǩ��
			}
			else
				gmissflag = MISS_G_FREE;//����ƶ��������꣬��½������̨
		}
		else{
			gMobilStyle.ISOK = 0;//��ǩ��������
			if(gmissflag != MISS_M_KEK)
				gmissflag = MISS_M_SINGIN;//
		}
	}
	else {
#ifdef _debug_
		debugstring("ǩ����Ч:");
		debugdata((unsigned char *)&gMobilStyle, 60, 1);
#endif
		gMobilStyle.ISOK = 0;//��ǩ����Ч
		if(gmissflag != MISS_M_KEK)
			gmissflag = MISS_M_SINGIN;//
	}

//	gmissOldflag = gmissflag;//�����ϴ����������־,ִ���걾������󷵻���һ����
//	gmissflag = MISS_M_ActiveTest;
	//�����ʼ������
	gSendOverTime = 0;
	gMobilPOSNS = 0;
	gmissNotTrad = 0;
	gmobileBakSeril = 0;
	gM_Senddll = 0;
	gmobileErrTimes = 0;

	load_fe_SendSeril();

#ifdef _debug_
	debugstring("B_Sendse:");
	debugdata((unsigned char*)&gSendSeril, 4, 1);
#endif

	feread(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);
	if(gMobilPOSNS > 999999){
		gMobilPOSNS = 1;
		sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);
	}

// 	feread(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
// 	feread(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
// 	if((gMobilREC_Start % MOBILE_REC_LEN) != 0){
// 		delayxms(1);
// 		feread(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
// 		if((gMobilREC_Start % MOBILE_REC_LEN) != 0){//����ٴ��Ǵ�ģ�ȡ����
// 			GetMobilStarBAK((unsigned char*)&gMobilREC_Start);
// 		}
// 	}
// 	if((gMobilREC_Start % MOBILE_REC_LEN) != 0){//ͷָ�����
// 		gMobilREC_Start -= (gMobilREC_Start%MOBILE_REC_LEN);
// 		sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
// 		writeMobilStarBAK((unsigned char*)&gMobilREC_Start);
// 	}
// 	if((gMobilREC_End % MOBILE_REC_LEN) != 0){//ָ�����
// 		gMobilREC_End += (MOBILE_REC_LEN-(gMobilREC_End%MOBILE_REC_LEN));
// 		sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
// 		sysfewrite(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
// 	}

// 	gMobilREC_Start = 0x1E100;
//	sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
//	writeMobilStarBAK((unsigned char*)&gMobilREC_Start);
 	getmobileADD();
// 
//  	addr = dealPiciNo();
//  	addr ++;
//  	sprintf((char*)gMobileParameter.posPiCiNo, "%06d", addr);
// 	memcpy(gMobileParameter.posPiCiNo, "000626", 6);
// 	gMobileParameter.gMobil_Settlement_error = 0;
//   	memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
//   	sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
	
// 	gMobilREC_Start = 0x2A400;
// 	sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
// 	writeMobilStarBAK((unsigned char*)&gMobilREC_Start);
// 	memcpy(gMobileParameter.posPiCiNo, "000800", 6);
// 	memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
// 	sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);


	for(i=0;i<6;i++){
		if((gMobileParameter.posPiCiNo[i]<'0')||(gMobileParameter.posPiCiNo[i]>'9')){
			memcpy(gMobileParameter.posPiCiNo, "000001", 6);
			break;
		}
	}
	
// 	memcpy(gMobileParameter.posPiCiNo, "-10080", 6);
// 	sysfewrite(BIT_PICINO_BAK, 7, buff);

#ifdef _debug_
	debugstring("gMobilPOSNS:");
	debugdata((unsigned char*)&gMobilPOSNS, 4, 1);
	debugstring("gMobilREC_Start:");
	debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
	debugstring("gMobilREC_End:");
	debugdata((unsigned char*)&gMobilREC_End, 4, 1);
	debugstring("���κ�:");
	debugdata(gMobileParameter.posPiCiNo, 6, 1);
	feread(BIT_PICINO_BAK, 7, buff);
	debugstring("�������κ�:");
	debugdata(buff, 7, 1);

#endif
// 	memcpy(gMobileParameter.posPiCiNo, "000096", 6);//�ֶ��޸����κ�
// 	uploadMobilePara();

	if(gmissflag == MISS_G_LOGINGJ){
		gmissOldflag = MISS_G_LOGINGJ;
		gmissflag = MISS_M_ActiveTest;//���û�ƶ������Ƚ���һ�λ����ٽ���������
	}
#ifdef _debug_
	debugstring("gMissFlag:");
	debugdata((unsigned char*)&gmissflag, 1, 1);
	debugstring("gmissOldflag:");
	debugdata((unsigned char*)&gmissOldflag, 1, 1);
	debugstring("WK:");
	debugdata((unsigned char*)&gMobilStyle.PIN_KEY, 40, 1);
#endif

	feread(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);//���س����������
	if((downProgPara.Miss_style != 0) || 
		(downProgPara.Miss_dongzuo > 1) ||
		(downProgPara.Miss_pesent[0] > 0x64))
	{
		memset((unsigned char*)&downProgPara, 0, 27);//����������0
		sysfewrite(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);
	}
	if((gMobilStyle.LoadStyle & LoadStyle_program)==0){//����û����Ҫ�³��������
		memset((unsigned char*)&downProgPara, 0, 27);//����������0
		sysfewrite(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);
	}

	if(memcmp(downProgPara.Miss_ver, "\x00\x00\x00\x00\x00", 5) == 0)
	{
		memset(downProgPara.Miss_ver, 0x30, 14);
	}
#ifdef _debug_
	debugstring("gmobileProVer:");
	debugdata(gmobileProVer, 14, 1);
	debugstring("�������ز���:");
	debugdata((unsigned char*)&downProgPara, 27, 1);
#endif

	if((memcmp(gMobileParameter.Ver, "\x00\x00\x00\x00\x00", 5)==0)||
		(memcmp(gMobileParameter.Ver, "\x30\x30\x30\x30\x30", 5)==0)||
		(memcmp(gMobileParameter.posNo, "\x30\x30\x30\x30\x30\x30\x30\x30", 8)==0)||
		(memcmp(gMobileParameter.posNo, "\x00\x00\x00\x00\x00\x00\x00\x00", 8)==0)){
		if((gmissflag != MISS_M_KEK) && (gmissflag != MISS_M_DPARAM)){//��Ҫ���ز���
			gmissOldflag = MISS_M_SINGIN;
			gmissflag = MISS_M_DPARAM;
		}
	}
	
	feread(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);//���ط�������
#ifdef _debug_
	debugstring("mobilsendStyle:");
	debugdata((unsigned char*)&mobilsendStyle, sizeof(stMobilSend), 1);
#endif
	if((memcmp(mobilsendStyle.Single, "���ڷ���", 8) != 0)&&
		(memcmp(mobilsendStyle.Single, "���ܴ���", 8) != 0)){
		memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
	}
#ifdef _MOBILE_IP_FUJIAN_
	memcpy(gMobileParameter.purIP, "218.207.183.082", 15);
	memcpy(gMobileParameter.manageIP, "218.207.183.082", 15);
	memcpy(gMobileParameter.managePort, "9091\x0", 6);
	memcpy(gMobileParameter.purPort, "9092\x0", 6);
#endif
#ifdef _MOBILE_IP_HUBEI_
	memcpy(gMobileParameter.purIP, "172.031.011.193", 15);
	memcpy(gMobileParameter.manageIP, "172.031.011.193", 15);
	memcpy(gMobileParameter.managePort, "36002\x0", 6);
	memcpy(gMobileParameter.purPort, "36007\x0", 6);
	memcpy(gMobileParameter.shopNo, MOBILE_SFNO, 15);// 888071941310000 �̻����
#endif
#ifdef _MOBILE_IP_XiAN_
	memcpy(gMobileParameter.purIP, "211.137.133.038", 15);
	memcpy(gMobileParameter.manageIP, "211.137.133.038", 15);
	memcpy(gMobileParameter.managePort, "8001\x0", 6);
	memcpy(gMobileParameter.purPort, "8000\x0", 6);
//	memcpy(gMobileParameter.shopNo, "888091948000000", 15);// 888073145114511 �̻����
	memcpy(gMobileParameter.shopNo, MOBILE_SFNO, 15);// 888071941310000 �̻����
	memcpy(gMobileParameter.posNo, busSeril, 8);//ǰ8λ�� POS�ն˺�
	memcpy(gMobileParameter.posSerilNo, "A204010010123456", 16);
	memcpy(gMobileParameter.posSerilNo+10, busSeril+10, 6);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(gMobileParameter.purIP, "218.206.165.003", 15);
	memcpy(gMobileParameter.manageIP, "218.206.165.003", 15);
	memcpy(gMobileParameter.managePort, "8002\x0", 6);
	memcpy(gMobileParameter.purPort, "8000\x0", 6);
	//	memcpy(gMobileParameter.shopNo, "888091948000000", 15);// 888073145114511 �̻����
	memcpy(gMobileParameter.shopNo, MOBILE_SFNO, 15);// 888071941310000 �̻����
	memcpy(gMobileParameter.posNo, busSeril, 8);//ǰ8λ�� POS�ն˺�
	memcpy(gMobileParameter.posSerilNo, "A204010010123456", 16);
	memcpy(gMobileParameter.posSerilNo+10, busSeril+10, 6);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#else
	memcpy(gMobileParameter.posSerilNo, busSeril, 16);//������ƽ̨KEK��Ҫ�������кţ������ն˺�Ҳ�����������Ǹ����ľͲ���Ҫ���ն˺�ȫ�����к�
#endif


#ifdef _MOBILE_IP_CHANGSHA_
	//---------------------��ɳ����-----------------
	memcpy(gMobileParameter.purIP, "211.138.236.210", 15);
	memcpy(gMobileParameter.manageIP, "211.138.236.210", 15);
	memcpy(gMobileParameter.managePort, "26202\x0", 6);
	memcpy(gMobileParameter.purPort, "26207\x0", 6);
	memcpy(gmobileProVer, "20091022000000",14);
	memset((unsigned char*)&downProgPara, 0, 27);//�����س��� 
	memcpy(gMobileParameter.posSerilNo, "ZZZZZZZZZZ210003", 16);//zzzzzzzzzz210003 �ն����к�
//	memcpy(gMobileParameter.posNo, "73100102", 8);//73100102 POS�ն˺�
//	memcpy(gMobileParameter.shopNo, "888073145114511", 15);// 888073145114511 �̻����
	//---------------------��ɳ����-----------------
#endif
	memset((unsigned char*)&downProgPara, 0, 27);//�����س��� 

#ifdef NO_MOBILE_SERVISE
	gmissflag = MISS_G_LOGINGJ;
#endif

#ifdef _debug_
	debugstring("shopNo:");
	debugdata((unsigned char*)&gMobileParameter.shopNo, 15, 1);
	debugstring("gMobileParameter.posNo:");
	debugdata((unsigned char*)&gMobileParameter.posNo, 8, 1);
	debugstring("gMobileParameter.posSerilNo:");
	debugdata((unsigned char*)&gMobileParameter.posSerilNo, 16, 1);
#endif

// 	memcpy(gMobileParameter.posNo, "27000006", 8);//73100102 POS�ն˺�
// 	memcpy(gMobileParameter.shopNo, "888001753990000", 15);// 888073145114511 �̻����
}
void showmobilePra(void)
{
	char disbuf[20];
	unsigned char i;

	cls();
	memset(disbuf, 0, 20);
	
	getposSN((unsigned char*)disbuf, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(disbuf, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(disbuf, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif
//	memcpy(disbuf, gMobileParameter.posSerilNo, 16);
	if(gchn_eng == 'E')
		display(0,0,"posSerilNo.:",0);
	else
		display(0,0,"���к�:",0);
	display(2,0,disbuf, 0);
	
	memset(disbuf, 0, 20);
	memcpy(disbuf, gMobileParameter.posNo, 8);
	display(4,0,disbuf, 0);
	
	memset(disbuf, 0, 20);
	memcpy(disbuf, gMobileParameter.shopNo, 15);
	display(6,0,disbuf, 0);
	while(1){
		if(profile(0) !=ST_ERROR)
			return;
		delayxms(50);
		i = getkey(1);
		if(i == ESC)
			return;
	}
}
void clr_mobile_record(void)
{
	unsigned int itemp;

	gMobilREC_Start = 0;
	sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
	writeMobilStarBAK((unsigned char*)&gMobilREC_Start);
	gMobilREC_End = 0;
	sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
	sysfewrite(BIT_MobilREC_End_BAK, 4, (unsigned char*)&gMobilREC_End);
	gMobilPOSNS = 1;
	sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);
	memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
	sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
	gMobileParameter.gMobil_Settlement_error = 0;
	gMobileParameter.addPICINO_limit = 0;

	itemp = dealPiciNo();
	if(gMobileParameter.addPICINO_limit == 5)
		itemp += 30;
	else
		itemp ++;
	itemp %= 1000000;
	sprintf((char*)gMobileParameter.posPiCiNo, "%06d", itemp);
	uploadMobilePara();
}
void first_clr_mobile_para(void)
{
	unsigned int addr;
	stMobileParameter pbuff;

// 	gMobilREC_Start = 0;
// 	sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
// 	gMobilREC_End = 0;
// 	sysfewrite(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
// 	gMobilPOSNS = 1;
// 	sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);

	memset((unsigned char*)&pbuff, 0, sizeof(stMobileParameter));
	memcpy(pbuff.manageIP, gMobileParameter.manageIP, 15);//IP��ַ����
	memcpy(pbuff.managePort, gMobileParameter.managePort, 6);
	memcpy(pbuff.purIP, gMobileParameter.purIP, 15);//IP��ַ����
	memcpy(pbuff.purPort, gMobileParameter.purPort, 6);
	memset(gMobileParameter.Ver, '0', 14);
	memcpy(pbuff.Ver, gMobileParameter.Ver, 14);
	memcpy(pbuff.posPiCiNo, "000001", 6);
	memcpy(gMobileParameter.posPiCiNo, "000001", 6);
// 	strcpy((char*)pbuff.manageIP,"211.138.236.210");
// 	strcpy((char*)pbuff.purIP, "211.138.236.210");
// 	memcpy(pbuff.managePort, "26202\x00", 6);
// 	memcpy(pbuff.purPort, "26207\x00", 6);
//	memset(pbuff.Ver, '0', 14);//�����汾
	pbuff.KEK_SN = 0xff;
	memcpy(pbuff.posAcktime, "00001234", 8);//��������
	addr = FLASH_MOBILE_START;
	flasherase(ERASE4KBYTESECTOR, addr);
	flashwrite(addr, (unsigned char*)&pbuff, sizeof(stMobileParameter));

	memset((unsigned char*)&gMobilStyle, 0, 20);
	sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);

	memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));//�����ϴ�
	feread(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);

	gmissOldflag = MISS_M_DPARAM;
	gmissflag = MISS_M_KEK;//��������KEK
	set_timer0(100, 0);//1S�ӽ�һ��
}
unsigned char posnoIsOK(unsigned char *dat)
{
	unsigned int i;
	for(i=0; i<7; i++){
		if((dat[i] > '9')||(dat[i] < '0'))
			break;
	}
	if(i < 7)
		return 1;
	else
		return 0;
}

extern unsigned char getmobileADD(void);
//��Ѱ�µ��������� ��������TaskRecWrite()�������ж�����ʱ����
void find_new_mission(void)
{
#ifdef NO_MOBILE_24G_
	gErrorFlag |= ERROR_CONNECT_MOBILE;//û���ƶ�ƽ̨,�����ƶ������Ӳ�����.
	return;
#else
	unsigned int ltemp1,ltemp2;
	unsigned char i;
	unsigned char dt[20];
	
//	debugstring("ggmiss:");
//	debugdata((unsigned char*)&gmissflag, 1, 0);
#ifdef NO_MOBILE_SERVISE
	gErrorFlag |= ERROR_CONNECT_MOBILE;//û���ƶ�ƽ̨,�����ƶ������Ӳ�����.
	return;
#endif
	
	if(gmissflag == MISS_G_DPRO)
		goto finnemend;
	
	if(gErrorFlag&ERROR_CONNECT_MOBILE){
#ifdef _debug_gprs
		sprintf((char*)dt, "��:%u", gErrortimes[0]);
		debugstring((char*)dt);
#endif
		if(gErrortimes[0] > 0){
			debugstring("-12-");//ttttttttttt
			if((gmissflag & (MISS_IF2|MISS_IF1)) != 0)
				gmissflag = 0;
			goto finnemend;//�ϴ����Ӵ���,ʱ��û�������ƶ�����.
		}
	}
	
	gErrorFlag &= (~ERROR_CONNECT_MOBILE);// ��������ʶ
	//	gsendError = SEND_FILE_ERROR_;//�������ļ�

// 	if((gmissflag & MISS_GJ)==0){//�������ƶ�����
// 		gmissflag = MISS_G_FREE;
// 		goto finnemend;
// 	}
// 	goto finnemend;

// 	if(gMobilConnectErr >= 5){//����д������>=5���������ƶ�ƽ̨
// 		gMobilConnectErr--;
// 		gmissflag = MISS_G_FREE;//IF1 IF2û����
// 		goto finnemend;
// 	}
	
	if(posnoIsOK(gMobileParameter.posNo) == 1){
		if((gMobilStyle.LoadStyle & LoadStyle_KEK)!=0){
			gmissflag = MISS_M_KEK;//��Ҫ����KEK
			goto finnemend;
		}
		gmissflag = MISS_M_DPARAM;
		goto finnemend;
	}
	if(gMobileParameter.KEK_SN > 3){//û��KEK��������KEK
#ifdef _debug_gprs
		debugstring("û��KEK\r\n");
#endif
		if(gmissflag == MISS_M_KEK)
			goto finnemend;
		gMobilStyle.ISOK = 0;
		gmissOldflag = MISS_M_DPARAM;//�����ϴ����������־,ִ���걾������󷵻���һ����
		gmissflag = MISS_M_KEK;
	}
	
	if (gMobilStyle.ISOK == 1)//ǩ��������Ч
	{
		if((gMobilStyle.LoadStyle & LoadStyle_KEK)!=0){
			gmissflag = MISS_M_KEK;//��Ҫ����KEK
			goto finnemend;
		}
		else if((gMobilStyle.LoadStyle & LoadStyle_para)!=0){
			gmissflag = MISS_M_DPARAM;//��Ҫ���ز���
			goto finnemend;
		}
		else if((gMobilStyle.LoadStyle & LoadStyle_program)!=0){
			gMobilStyle.LoadStyle &= (~LoadStyle_program);//�����ֻ�POSP���س���
			//gmissflag = MISS_M_DPGRAM;//��Ҫ���س���
			//goto finnemend;
		}
	}
	if(gmissflag & MISS_IF1){//��ֹgmissflag�����־����
		if((gmissflag == MISS_M_SINGIN)||(gmissflag == MISS_M_Settlement)||(gmissflag != MISS_M_TradeBatch)){
			goto finnemend;//��������
		}
	}
	if(gmissflag & MISS_IF2){//��ֹgmissflag�����־����
		if((gmissflag == MISS_M_DPARAM)||(gmissflag == MISS_M_DPGRAM)||(gmissflag == MISS_M_DPGRAMOK)||
			(gmissflag == MISS_M_KEK)){
			goto finnemend;//��������
		}
	}

	if(F_FLAG_VARY.stop_flag == 0){
		if(gMobilStyle.ISOK != 1){//ǩ����Ч
			if(gmissflag != MISS_M_KEK){
				gmissflag = MISS_M_SINGIN;//
				goto finnemend;
			}
		}
	}
	
//	debugstring("mobilsendStyle:");
//	debugdata((unsigned char*)&mobilsendStyle, sizeof(stMobilSend), 1);
// debugstring("gM_Senddll:");
// debugdata((unsigned char*)&gM_Senddll, 1, 0);
	if(memcmp(mobilsendStyle.Single, "���ܴ���", 8)==0){//����ǻ��ܴ������������м�¼�����
		uploadMobileData();
	}
	else{
		ltemp1 = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
//		if(ltemp1 > 0){
			getmobileADD();
//		}
		if(memcmp(mobilsendStyle.Single, "���ڷ���", 8)==0){
			ltemp1 = (gMobilREC_End+LEN_M_REC-mobilsendStyle.SendAddr)%LEN_M_REC;
			ltemp2 = (gMobilREC_End+LEN_M_REC-mobilsendStyle.StartAddr)%LEN_M_REC;//����������
		}
		else{
			if(checkPiCiNo() == ST_ERROR)
				goto finnemend;
			memcpy(dt, gMobileParameter.posPiCiNo, 6);
			dt[6] = 0x55;
			for(i=0;i<6;i++)
				dt[6] ^= dt[i];
			sysfewrite(BIT_PICINO_BAK, 7, dt);//����Ǹտ�ʼ�����������κ�

			ltemp1 = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
			ltemp2 = ltemp1;
		}
		ltemp2 /= MOBILE_REC_LEN;
//		memcpy(dt, (unsigned char*)&SysTime.minutes, 2);// ������0����,����30��ʱ�����ݾ�����
		if((ltemp2 >= 10)||(SysTime.minutes > 3)){//����15���������㶨ʱ����
//			gM_Senddll = 0;

			if(gM_Senddll>0){
				gmissflag = MISS_G_FREE;//IF1 IF2û����
				goto finnemend;
			}

			if(ltemp2 == 0){//û�м�¼���ϴ�
				gmissflag = MISS_G_FREE;//IF1 IF2û����
				goto finnemend;
			}

			if(memcmp(mobilsendStyle.Single, "���ܴ���", 8)==0)//������Ѿ��������ٴ���
				;
			else if(memcmp(mobilsendStyle.Single, "���ڷ���", 8) == 0){
				memcpy(mobilsendStyle.Single, "���ܴ���", 8);
				mobilsendStyle.EndAddr = gMobilREC_End;
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
			}
			else{
				memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
				mobilsendStyle.StartAddr = gMobilREC_Start;
				mobilsendStyle.SendAddr = gMobilREC_Start;
				feread(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);
				mobilsendStyle.EndAddr = gMobilREC_End;
				mobilsendStyle.SendPiCI = dealPiciNo();
				memcpy(mobilsendStyle.Single, "���ܴ���", 8);
				mobilsendStyle.EndAddr = gMobilREC_End;
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
			}
		}
		else{
			gmissflag = MISS_G_FREE;//IF1 IF2û����
		}

		ltemp1 /= MOBILE_REC_LEN;
		if(ltemp1 > 5){//���ڣ����Ϳ�ʼ�����ƶ�����,������
			uploadMobileData();
		}
	}
finnemend:
	;
#ifdef _debug_
	debugstring("gmissflag:");
	debugdata((unsigned char*)&gmissflag, 1, 1);
#endif
	
#endif
}
//N6��6���ֽڵĶ��������ַ��� ѹ��ʱ��BCD���ʾ��3���ֽڵĶ�����
unsigned int Build8583_11(unsigned char *dat)
{
	unsigned char buff[20];
	sprintf((char*)buff, "%06d",gMobilPOSNS);
	Ascii2BCD(buff, dat, 6);
	return 3;
}
//9.1.3.6.	BM #12�յ����ն����ڵ�ʱ��
unsigned int Build8583_12(unsigned char *dat)
{
	memcpy(dat, (unsigned char*)&SysTime.hours, 3);
	return 3;
}
//9.1.3.7.	BM #13�յ����ն����ڵ�����
unsigned int Build8583_13(unsigned char *dat)
{
	memcpy(dat, (unsigned char*)&SysTime.year_h, 4);
	return 4;
}

//9.1.3.20.	BM #40 POS�ն����к�
unsigned int Build8583_40(unsigned char *dat)
{
	getposSN(dat, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(dat, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(dat, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif

//	memcpy(dat, gMobileParameter.posSerilNo, 16);
	return 16;
}
//9.1.3.21.	BM #41�յ����ն˱�ʶ��
unsigned int Build8583_41(unsigned char *dat)
{
	memcpy(dat, gMobileParameter.posNo, 8);
	return 8;
}
//9.1.3.22.	BM #42�̻���
unsigned int Build8583_42(unsigned char *dat)
{
	memcpy(gMobileParameter.shopNo, MOBILE_SFNO, 15);
	memcpy(dat, gMobileParameter.shopNo, 15);
	return 15;
}
//9.1.3.24.	BM #49���׻��Ҵ���
unsigned int Build8583_49(unsigned char *dat)
{
	memcpy(dat, "156", 3);
	return 3;
}
//9.1.3.29.	BM #60�Զ���Ӧ������
unsigned int Build8583_60(unsigned char *dat, unsigned char mode)
{
	stP8583_60 p8583_60;
	unsigned int itemp;

	p8583_60.MSG60_LLL[0] = 0;
	p8583_60.MSG60_LLL[1] = 0x72;
	memcpy(p8583_60.MSG60_1, MobileAID, 16);//Ӧ�ñ�ʶAID
//	memcpy(p8583_60.MSG60_1, "\xD1\x56\x00\x01\x01\x80\x03\x80\x00\x00\x00\x01\x00\x00\x00\x00", 16);
	memcpy(p8583_60.MSG60_2, gmobileProVer, 14);//Ӧ�ó���汾��
	BCD2Ascii((unsigned char*)&gMobileParameter.KEK_SN, p8583_60.MSG60_3, 1);//KEK��Կ������
//	memset(p8583_60.MSG60_4, '0', 14);
	memcpy(p8583_60.MSG60_4, gMobileParameter.Ver, 14);//�����汾��
	memcpy(p8583_60.MSG60_5, "FF", 2);//POS��Ϣ������
	if(mode == 0)
		memcpy(p8583_60.MSG60_6, "00", 2);//POS������Ϣ������ 00 ����
	else if(mode == 2)
		memcpy(p8583_60.MSG60_6, "02", 2);//POS������Ϣ������ 02 ������
	else if(mode == 3)
		memcpy(p8583_60.MSG60_6, "03", 2);//POS������Ϣ������ 03 δȷ�ϼ�¼�ϴ�
	else
		memcpy(p8583_60.MSG60_6, "01", 2);//POS������Ϣ������ 01 ǩ��
	
	if(checkPiCiNo() == ST_ERROR)
		return 0;
	memcpy(p8583_60.MSG60_7, gMobileParameter.posPiCiNo, 6);//���κ�
//	memcpy(p8583_60.MSG60_7, "000002", 6);//���κ�
	memcpy((unsigned char*)&itemp, DriveCardNo, 4);
	if(itemp == 0)
		itemp = 1;
	itemp %= 1000000;
	sprintf((char*)p8583_60.MSG60_8, "%-8d", itemp);//˾������
	memcpy(p8583_60.MSG60_9, pFistVary.DeviceNo, 8);//�����豸���
	memcpy(dat, (unsigned char *)&p8583_60, 74);
	return 74;
}
unsigned char checkMobileRec(stMobileRecord *rec)
{
	unsigned char buff[30];
	unsigned int temp;

	if(rec->dealType != 0x02)
		return ST_ERROR;
	if(memcmp(rec->AID, MobileAID, 16) != 0)
		return ST_ERROR;
	memcpy(buff, rec->date, 7);
	if(time_invalid(buff) == 0)
		return ST_ERROR;
	memcpy((unsigned char*)&temp, rec->purseMoney, 4);
	over_turn(4, (unsigned char*)&temp);
	if((temp == 0)||(temp > 1000))
		return ST_ERROR;
	return ST_OK;
}

//9.1.3.30.	BM #61�Զ�������������
unsigned int Build8583_61(unsigned char *dat)
{
	stP8583_61 p8583_61;
	unsigned int itemp,i,iit,cmax,mmoney;
	unsigned int addr,ii;
	unsigned char buff[20];
	stMobileRecord rectemp;
	unsigned int ss, sendenda;
	
	getmobileADD();

	p8583_61.LLL[0] = 0x04;
	p8583_61.LLL[1] = 0x17;
	
	ss = mobilsendStyle.StartAddr;
	ii = mobilsendStyle.SendAddr;
	for(i=0;i<3;i++){
		feread(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);//���ط���ǰ��״̬
		if((mobilsendStyle.SendAddr == ii)&&(mobilsendStyle.StartAddr == ss))
			break;
	}
	sysferead(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
	if((gMobilREC_Start % MOBILE_REC_LEN) != 0){//����ٴ��Ǵ�ģ�ȡ����
		GetMobilStarBAK((unsigned char*)&gMobilREC_Start);//
	}
	sysferead(BIT_MobilREC_End, 4, (unsigned char*)&gMobilREC_End);

	if(memcmp(mobilsendStyle.Single, "���ܴ���", 8) == 0){
		sendenda = mobilsendStyle.EndAddr;
	}
	else{
		sendenda = gMobilREC_End;//��Ҫ���㵽����Ϊֹ
	}
	addr = (sendenda+LEN_M_REC-mobilsendStyle.StartAddr)%LEN_M_REC;
	itemp = (sendenda+LEN_M_REC-mobilsendStyle.SendAddr)%LEN_M_REC;
	ss = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;//���м�¼������
#ifdef _debug_
	//	sprintf((char*)buff, "%04d|%04d ����", gMTrad63MSG1,gMTrad63MSG2);
	debugstring("gMobilREC_Start:");
	debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
	debugstring("gMobilREC_End:");
	debugdata((unsigned char*)&gMobilREC_End, 4, 1);
	debugstring("sendenda:");
	debugdata((unsigned char*)&sendenda, 4, 1);
	debugstring("Build_61:mobilsendStyle.StartAddr:");
	debugdata((unsigned char*)&mobilsendStyle.StartAddr, 4, 1);
	debugdata((unsigned char*)&mobilsendStyle.SendAddr, 4, 1);
	debugstring("addr:");
	debugdata((unsigned char*)&addr, 4, 1);
	debugdata((unsigned char*)&itemp, 4, 1);
	debugdata((unsigned char*)&ss, 4, 1);
	debugstring("mobilsendStyle.all_number:");
	debugdata((unsigned char*)&mobilsendStyle.all_number, 4, 1);
	debugstring("mobilsendStyle.all_money:");
	debugdata((unsigned char*)&mobilsendStyle.all_money, 4, 1);
#endif

	memcpy((unsigned char*)&mmoney, NewPriceTab.busPrice, 4);
	if((itemp>addr)||((mobilsendStyle.StartAddr%MOBILE_REC_LEN)!=0)||\
		(mobilsendStyle.all_money > (mobilsendStyle.all_number*mmoney))||\
		((itemp % MOBILE_REC_LEN)!=0)||((mobilsendStyle.SendAddr%MOBILE_REC_LEN)!=0)||
		(mobilsendStyle.all_number > ss)||(addr > ss)){
#ifdef _debug_
		debugstring("�ϴ���Ϣ�������´���\r\n");
#endif
		
		getmobileADD();
		memcpy(buff, mobilsendStyle.Single, 8);
		memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
		mobilsendStyle.StartAddr = gMobilREC_Start;
		mobilsendStyle.SendAddr = gMobilREC_Start;
		mobilsendStyle.EndAddr = gMobilREC_End;
		mobilsendStyle.SendPiCI = dealPiciNo();
		memcpy(mobilsendStyle.Single, buff, 8);
		mobilsendStyle.EndAddr = gMobilREC_End;

		gSendSeril = 0xffffffff;
		memset(buff, 5, 0);
		sysfewrite(BIT_SendSeril, 5, buff);
		gseflag = 0;
		sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
		
		if(memcmp(mobilsendStyle.Single, "���ܴ���", 8) == 0){
			sendenda = mobilsendStyle.EndAddr;
		}
		else{
			sendenda = gMobilREC_End;//��Ҫ���㵽����Ϊֹ
		}
		addr = (sendenda+LEN_M_REC-mobilsendStyle.StartAddr)%LEN_M_REC;
		itemp = (sendenda+LEN_M_REC-mobilsendStyle.SendAddr)%LEN_M_REC;
		
#ifdef _debug_
		//	sprintf((char*)buff, "%04d|%04d ����", gMTrad63MSG1,gMTrad63MSG2);
		debugstring("-gMobilREC_Start:");
		debugdata((unsigned char*)&gMobilREC_Start, 4, 1);
		debugstring("-gMobilREC_End:");
		debugdata((unsigned char*)&gMobilREC_End, 4, 1);
		debugstring("-sendenda:");
		debugdata((unsigned char*)&sendenda, 4, 1);
		debugstring("-Build_61:mobilsendStyle.StartAddr:");
		debugdata((unsigned char*)&mobilsendStyle.StartAddr, 4, 1);
		debugdata((unsigned char*)&mobilsendStyle.SendAddr, 4, 1);
		debugstring("-addr:");
		debugdata((unsigned char*)&addr, 4, 1);
		debugdata((unsigned char*)&itemp, 4, 1);
		debugstring("-mobilsendStyle.all_number:");
		debugdata((unsigned char*)&mobilsendStyle.all_number, 4, 1);
		debugstring("-mobilsendStyle.all_money:");
		debugdata((unsigned char*)&mobilsendStyle.all_money, 4, 1);
#endif
	}
	sprintf((char*)buff, "%d/%d ", (itemp/128), (addr/128));
	miniDispstr(6,0,(char*)buff,0);

	addr = FLASH_M_REC_START+mobilsendStyle.SendAddr;
	if(itemp == 0){//˵���Ѿ��������
		return 0;
	}

	if(mobilsendStyle.all_number == 0){
		gSendSeril = 0xffffffff;//�տ�ʼ���ͣ���Ҫ��ʼ��
		save_FE_SendSeril();
	}
	
#ifdef _watchdog_open
	clr_wdt();
#endif
	load_fe_SendSeril();

	if(itemp > (MOBILE_REC_LEN*5)){
		itemp = MOBILE_REC_LEN*5;//һ��ֻ��5��
		p8583_61.MSG1 = 0x01;
		p8583_61.MSG2 = 0x05;
	}
	else{
		p8583_61.MSG1 = 0x00;
		p8583_61.MSG2 = itemp/MOBILE_REC_LEN;
	}
	
	iit = 0;
	cmax = p8583_61.MSG2;
	for(i=0;i<cmax;i++){
#ifdef _watchdog_open
		clr_wdt();
#endif
		flashread(addr, (unsigned char *)&rectemp, 83);
		ss = BCD2HEX(rectemp.posSerial, 3);
#ifdef _debug_
		debugstring("����:");
		debugdata((unsigned char*)&rectemp, 83, 1);
		debugdata((unsigned char*)&ss, 4, 0);
		debugdata((unsigned char*)&gSendSeril, 4, 1);
#endif
		if(checkMobileRec(&rectemp) == ST_ERROR){
#ifdef _debug_
			debugstring("��¼����\r\n");
#endif
			addr+=MOBILE_REC_LEN;
			if(addr >= FLASH_M_REC_START + LEN_M_REC)
				addr = FLASH_M_REC_START;
			mobilsendStyle.SendAddr += MOBILE_REC_LEN;//��Ҫ���͵�ָ��Ҳ�ӣ�������ɹ��������ûأ�gMobilREC_Start
			if(mobilsendStyle.SendAddr >= LEN_M_REC)
				mobilsendStyle.SendAddr = 0;
			p8583_61.MSG2--;
			continue;
		}
#ifdef _debug_
		debugstring("��¼OK \r\n");
#endif
		if(gSendSeril <= 999999){
			if(ss <= gSendSeril){
				gSendSeril++;
				if(gSendSeril > 999999)
					gSendSeril = 1;
				sprintf((char*)buff, "%06d", gSendSeril);
				Ascii2BCD(buff, rectemp.posSerial, 6);
				ss = gSendSeril;
#ifdef _debug_
				debugstring("����2:");
				debugdata((unsigned char*)&rectemp, 83, 1);
#endif
			}
		}

		gSendSeril = ss;
		
		addr+=MOBILE_REC_LEN;
		if(addr >= FLASH_M_REC_START + LEN_M_REC)
			addr = FLASH_M_REC_START;
		mobilsendStyle.SendAddr += MOBILE_REC_LEN;//��Ҫ���͵�ָ��Ҳ�ӣ�������ɹ��������ûأ�gMobilREC_Start
		if(mobilsendStyle.SendAddr >= LEN_M_REC)
			mobilsendStyle.SendAddr = 0;
		mobilsendStyle.all_number++;//�ӻ��ܴ���
		memcpy((unsigned char*)&ii, rectemp.purseMoney, 4);
		over_turn(4, (unsigned char*)&ii);
		mobilsendStyle.all_money += ii;//�ӻ��ܽ��

#ifdef _watchdog_open
		clr_wdt();
#endif

		sprintf((char*)buff, "%012d", ii);
		Ascii2BCD(buff, rectemp.dealMoney, 12);
		memcpy(p8583_61.MSG3+iit*83, (unsigned char *)&rectemp, 83);//ÿ����¼��ǰ83�ֽ�
		iit++;
#ifdef _debug_
//	sprintf((char*)buff, "%04d|%04d ����", gMTrad63MSG1,gMTrad63MSG2);
	debugstring("����:");
	debugdata(gMobileParameter.posPiCiNo, 6, 0);
	debugstring("iit:");
	debugdata((unsigned char*)&iit, 4, 1);
#endif
	}
	i = (iit*83)+4;//83��ÿ����¼+2��+LLL��2��
	sprintf((char*)buff, "%04d", (i-2));
	Ascii2BCD(buff, p8583_61.LLL, 4);//����ȷ�����ֶδ�С

	memcpy(dat, (unsigned char *)&p8583_61, i);
#ifdef _debug_
	//	sprintf((char*)buff, "%04d|%04d ����", gMTrad63MSG1,gMTrad63MSG2);
	debugstring("��� i:");
	debugdata((unsigned char*)&i, 4, 1);
#endif

	return i;
}
//9.1.3.32.	BM #63�Զ����ն���Ϣ����
unsigned int Build8583_63(unsigned char *dat)
{
 	stP8583_63 p8583_63;
// 	unsigned int itemp,ii;
// 	stMobileRecord rectemp;
// 	unsigned int addr,recSum, sumMoney;
// 
 	p8583_63.LLL[0] = 0;
 	p8583_63.LLL[1] = 0x45;
// 	addr = gMobilREC_Start+FLASH_M_REC_START;
// 	recSum = (gMobilREC_End + LEN_M_REC - gMobilREC_Start)/LEN_M_REC;
// 	sumMoney = 0;
// 	for(itemp=0;itemp<recSum;itemp++){
// 		flashread(addr, (unsigned char *)&rectemp, 83);
// 		memcpy((unsigned char*)&ii, rectemp.purseMoney, 4);
// 		sumMoney+=ii;//�������ѽ��
// 		addr += MOBILE_REC_LEN;
// 	}
// 
// 	recSum = 1;
// 	sumMoney = 100;

#ifdef _watchdog_open
	clr_wdt();
#endif
//	mobilsendStyle.all_number = 126;
	sprintf((char*)p8583_63.MSG1, "%03d", mobilsendStyle.all_number);
//	debugstring((char*)p8583_63.MSG1);
//	mobilsendStyle.all_money = 11088;
	sprintf((char*)p8583_63.MSG2, "%012d", mobilsendStyle.all_money);
//	debugstring((char*)p8583_63.MSG2);

	memset(p8583_63.MSG3, '0', 3);
	memset(p8583_63.MSG4, '0', 12);
	memset(p8583_63.MSG5, '0', 3);
	memset(p8583_63.MSG6, '0', 12);
	memcpy(dat, (unsigned char*)&p8583_63, 47);
	return 47;
}
//9.1.3.33.	BM #64��Ϣ������ MAC 
unsigned int Build8583_64(unsigned char *dat, unsigned int len, unsigned char *omac){
	unsigned char radom[8];
	memset(radom, 0, 8);
//	CountMac1(gMobileParameter.KEK+(gMobileParameter.KEK_SN*16), dat, len, omac);
	CountMac1(gMACKEY_M, radom, dat, len, omac);
//	CountMac1(gMobilStyle.MAC_KEY, dat, len, omac);
	return 8;
}
//mode =0��ʼ��  ==1����
unsigned int Build_DPARAM(unsigned char *dat, unsigned char mode)
{
	unsigned int len;
	unsigned int crc32;

	len = 0;
	dat[len++] = 0x02;//STX ��ʼλ
	dat[len++] = 0x00;//���ȸ�λ LEN_H
	dat[len++] = 0x00;//���ȵ�λ LEN_L

	dat[len++] = 0xff;//0xff �������� 0x02: ��������
	dat[len++] = 0x01;//
	dat[len++] = 0x02;//0x02: ��������
	
	dat[len++] = 0x28;//0x28 POS�ն����к�
	dat[len++] = 16;//
	
	getposSN(dat+len, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif
//	memcpy(dat+len, gMobileParameter.posSerilNo, 16);
	len += 16;

	if(mode == 1){//���²���
		dat[len++] = 0x29;//0x29 POS�ն˺�
		dat[len++] = 8;//
		memcpy(dat+len, gMobileParameter.posNo, 8);
		len += 8;

		dat[len++] = 0x2a;//0x2a �̻����
		dat[len++] = 15;//
		memcpy(dat+len, gMobileParameter.shopNo, 15);
		len += 15;
	}

	dat[len++] = 0x90;//0x90 Ӧ�ñ�ʶ��AID
	dat[len++] = 16;//
	memcpy(dat+len, MobileAID, 16);
	len += 16;

	dat[len++] = 0xaa;//0xaa �ն˲����汾��
	dat[len++] = 14;//
	memcpy(dat+len, gMobileParameter.Ver, 14);
	len += 14;

	dat[len++] = 0x03;//ETX ֹͣλ
	dat[1] = ((len-4)/256);//����ȷ������ ����EXT STX LEN_H LEN_L-1
	dat[2] = ((len-4)%256);
	crc32 = GenerateCRC32(dat+3, len-4);
	memcpy(dat+len, (unsigned char*)&crc32, 4);//CRC32 У����
	len+=4;
	return len;
}
//020047 FF0103 FE0101 28105A5A5A5A5A5A5A5A5A5A323130303033 9010D1560001018003800000000100000000
//FC1B000000000000000000000000000000000000000000000000000000
//03 D9ADBA86
unsigned int Build_DPGRAM(unsigned char *dat)
{
	unsigned int len;
	unsigned int crc32;
	
	len = 0;
	dat[len++] = 0x02;//STX ��ʼλ
	dat[len++] = 0x00;//���ȸ�λ LEN_H
	dat[len++] = 0x00;//���ȵ�λ LEN_L
	
	dat[len++] = 0xff;//0xff �������� 0x03: 
	dat[len++] = 0x01;//
	dat[len++] = 0x03;//0x03: ��������
	
	dat[len++] = 0xfe;//0xff ����
	dat[len++] = 0x01;//
	dat[len++] = 0x01;//0x01: Ӧ�ó�������
	
	dat[len++] = 0x28;//0x28 POS�ն����к�
	dat[len++] = 16;//
	
	getposSN(dat+len, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif
//	memcpy(dat+len, gMobileParameter.posSerilNo, 16);
	len += 16;
	
	dat[len++] = 0x90;//0x90 Ӧ�ñ�ʶ��AID
	dat[len++] = 16;//
	memcpy(dat+len, MobileAID, 16);
	len += 16;
	
	dat[len++] = 0xfc;//0xfc �����
	dat[len++] = 27;//
	memcpy(dat+len, (unsigned char*)&downProgPara, 27);
	len += 27;
	
	dat[len++] = 0x03;//ETX ֹͣλ
	dat[1] = ((len-4)/256);//����ȷ������ ����EXT STX LEN_H LEN_L
	dat[2] = ((len-4)%256);
	crc32 = GenerateCRC32(dat+3, len-4);
	memcpy(dat+len, (unsigned char*)&crc32, 4);//CRC32 У����
	len+=4;
	return len;
}
unsigned int Build_DPGRAMOK(unsigned char *dat)
{
	unsigned int len;
	unsigned int crc32;
	
	len = 0;
	dat[len++] = 0x02;//STX ��ʼλ
	dat[len++] = 0x00;//���ȸ�λ LEN_H
	dat[len++] = 0x00;//���ȵ�λ LEN_L
	
	dat[len++] = 0xff;//0xff �������� 0x02: ��������
	dat[len++] = 0x01;//
	dat[len++] = 0x03;//0x02: ��������
	
	dat[len++] = 0xfe;//0xff �������� 0x02: ��������
	dat[len++] = 0x01;//
	dat[len++] = 0x02;//0x01: Ӧ�ó�������
	
	dat[len++] = 0x28;//0x28 POS�ն����к�
	dat[len++] = 16;//
	
	getposSN(dat+len, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif
//	memcpy(dat+len, gMobileParameter.posSerilNo, 16);
	len += 16;
	
	dat[len++] = 0x90;//0x90 Ӧ�ñ�ʶ��AID
	dat[len++] = 16;//
	memcpy(dat+len, MobileAID, 16);
	len += 16;
	
	dat[len++] = 0xfc;//0xfc �����
	dat[len++] = 17;//
	memcpy(dat+len, downProgPara.MISS_Sreil, 2);//������
	len+=2;
	dat[len++] = 0;//��������
	memcpy(dat+len, downProgPara.Miss_ver, 14);//����汾��
	len += 14;
	
	dat[len++] = 0x03;//ETX ֹͣλ
	dat[1] = ((len-4)/256);//����ȷ������ ����EXT STX LEN_H LEN_L
	dat[2] = ((len-4)%256);
	crc32 = GenerateCRC32(dat+3, len-4);
	memcpy(dat+len, (unsigned char*)&crc32, 4);//CRC32 У����
	len+=4;
	return len;
}
unsigned int Build_DKEK(unsigned char *dat)
{
	unsigned int len;
	unsigned int crc32;
	
	len = 0;
	dat[len++] = 0x02;//STX ��ʼλ
	dat[len++] = 0x00;//���ȸ�λ LEN_H
	dat[len++] = 0x00;//���ȵ�λ LEN_L
	
	dat[len++] = 0xff;//0xff �������� 0x02: ��������
	dat[len++] = 0x01;//
	dat[len++] = 0x04;//0x04: KEK����
	
	dat[len++] = 0x28;//0x28 POS�ն����к�
	dat[len++] = 16;//
	
	getposSN(dat+len, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif
//	memcpy(dat+len, gMobileParameter.posSerilNo, 16);
	len += 16;

	dat[len++] = 0x29;//0x29 POS�ն˺�
	dat[len++] = 8;//
	memcpy(dat+len, gMobileParameter.posNo, 8);
//	memset(dat+len, '0', 8);
	len += 8;
	
	dat[len++] = 0x2a;//0x2a �̻����
	dat[len++] = 15;//
	memcpy(dat+len, gMobileParameter.shopNo, 15);
//	memset(dat+len, '0', 15);
	len += 15;
	
	dat[len++] = 0x90;//0x90 Ӧ�ñ�ʶ��AID
	dat[len++] = 16;//
	memcpy(dat+len, MobileAID, 16);
	len += 16;
	
	dat[len++] = 0x03;//ETX ֹͣλ
	dat[1] = ((len-4)/256);//����ȷ������ ����EXT STX LEN_H LEN_L
	dat[2] = ((len-4)%256);
	crc32 = GenerateCRC32(dat+3, len-4);
	memcpy(dat+len, (unsigned char*)&crc32, 4);//CRC32 У����
	len+=4;
	return len;
}
unsigned int Build_ActiveTest(unsigned char *dat)
{
	unsigned int len;
	unsigned int crc32;
	
	len = 0;
	dat[len++] = 0x02;//STX ��ʼλ
	dat[len++] = 0x00;//���ȸ�λ LEN_H
	dat[len++] = 0x00;//���ȵ�λ LEN_L
	
	dat[len++] = 0xff;//0xff �������� 0x01: ����
	dat[len++] = 0x01;//
	dat[len++] = 0x01;//0x01: 
	
	dat[len++] = 0x28;//0x28 POS�ն����к�
	dat[len++] = 16;//
	
	getposSN(dat+len, 0);
#ifdef _MOBILE_IP_XiAN_
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#elif defined _MOBILE_IP_QINHAI
	memcpy(dat+len, "A204010010123456", 10);//ȡ��6λ�����кţ�ǰ��̶�������ƽ̨
#endif
//	memcpy(dat+len, gMobileParameter.posSerilNo, 16);
	len += 16;

	dat[len++] = 0x29;//0x29 POS�ն˺�
	dat[len++] = 8;//
	memcpy(dat+len, gMobileParameter.posNo, 8);
	len += 8;
	
	dat[len++] = 0x2a;//0x2a �̻����
	dat[len++] = 15;//
	memcpy(dat+len, gMobileParameter.shopNo, 15);
	len += 15;
	
	dat[len++] = 0x90;//0x90 Ӧ�ñ�ʶ��AID
	dat[len++] = 16;//
	memcpy(dat+len, MobileAID, 16);
	len += 16;

	dat[len++] = 0xa0;
	dat[len++] = 1;
	dat[len++] = 1;
	
	dat[len++] = 0xa1;
	dat[len++] = 1;
	dat[len++] = 0;
	
	dat[len++] = 0xa2;
	dat[len++] = 1;
	dat[len++] = 1;
	
	dat[len++] = 0xa3;
	dat[len++] = 1;
	dat[len++] = 0;
	
	dat[len++] = 0xa4;
	dat[len++] = 1;
	dat[len++] = 0;
	
	dat[len++] = 0xa5;//ͨѶ�ɹ���
	dat[len++] = 1;
	dat[len++] = 100;

	dat[len++] = 0xa6;//�ն�����״̬
	dat[len++] = 1;
	dat[len++] = 0;
	
	dat[len++] = 0xa8;//POS����ʱ��
	dat[len++] = 8;
	memcpy(dat+len, "\x00\x00\x00\x00\x00\x00\x00\x02", 8); len+=8;
	
	dat[len++] = 0xa9;//�ն�Ӧ�ó���汾��
	dat[len++] = 14;
	memcpy(dat+len, gmobileProVer, 14); len+=14;
	
	dat[len++] = 0xaa;//�ն˲����汾��
	dat[len++] = 14;
	memcpy(dat+len, gMobileParameter.Ver, 14); len+=14;

	dat[len++] = 0x03;//ETX ֹͣλ
	dat[1] = ((len-4)/256);//����ȷ������ ����EXT STX LEN_H LEN_L
	dat[2] = ((len-4)%256);
	crc32 = GenerateCRC32(dat+3, len-4);
	memcpy(dat+len, (unsigned char*)&crc32, 4);//CRC32 У����
	len+=4;
	return len;
}
unsigned int Build8583Packge(unsigned char *revBuf, unsigned char mode)
{
	unsigned int len = 0;
	unsigned int blen;

	switch (mode)
	{
	case MISS_M_SINGIN://ǩ��
		revBuf[len++] = 0x00;
		revBuf[len++] = 0x8A;//��Ҫ����ǰ������
		memset(revBuf+len, 0, 5);
		revBuf[len] = 0x60;
		len += 5;
		revBuf[len++] = 0x08;
		revBuf[len++] = 0x00;
//bitMap:0038000001C00010
		revBuf[len++] = 0x00;//1-8
		revBuf[len++] = 0x38;//9-16
		revBuf[len++] = 0x00;//17-24
		revBuf[len++] = 0x00;//25-32
		revBuf[len++] = 0x01;//33-40
		revBuf[len++] = 0xC0;//41-48
		revBuf[len++] = 0x00;//49-56
		revBuf[len++] = 0x10;//57-64
		blen = Build8583_11(revBuf+len);
		len+=blen;
		blen = Build8583_12(revBuf+len);
		len+=blen;
		blen = Build8583_13(revBuf+len);
		len+=blen;
		blen = Build8583_40(revBuf+len);
		len+=blen;
		blen = Build8583_41(revBuf+len);
		len+=blen;
		blen = Build8583_42(revBuf+len);
		len+=blen;
		blen = Build8583_60(revBuf+len, 1);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	case MISS_M_Settlement://9.2.6.2.	������
		revBuf[len++] = 0x00;
		revBuf[len++] = 0x8A;//��Ҫ����ǰ������
		memset(revBuf+len, 0, 5);
		revBuf[len] = 0x60;
		len += 5;
		revBuf[len++] = 0x05;
		revBuf[len++] = 0x00;
//0038000001C08013
		revBuf[len++] = 0x00;//1-8
		revBuf[len++] = 0x38;//9-16
		revBuf[len++] = 0x00;//17-24
		revBuf[len++] = 0x00;//25-32
		revBuf[len++] = 0x01;//33-40
		revBuf[len++] = 0xC0;//41-48
		revBuf[len++] = 0x80;//49-56
		revBuf[len++] = 0x13;//57-64
		blen = Build8583_11(revBuf+len);
		len+=blen;
		blen = Build8583_12(revBuf+len);
		len+=blen;
		blen = Build8583_13(revBuf+len);
		len+=blen;
		blen = Build8583_40(revBuf+len);
		len+=blen;
		blen = Build8583_41(revBuf+len);
		len+=blen;
		blen = Build8583_42(revBuf+len);
		len+=blen;
		blen = Build8583_49(revBuf+len);
		len+=blen;
		blen = Build8583_60(revBuf+len, 0);
		if(blen == 0)
			return 0;
		len+=blen;
		blen = Build8583_63(revBuf+len);
		len+=blen;
		blen = Build8583_64(revBuf+7, len-7, revBuf+len);
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	case MISS_M_TradeBatch://.������
		revBuf[len++] = 0x00;
		revBuf[len++] = 0x8A;//��Ҫ����ǰ������
		memset(revBuf+len, 0, 5);
		revBuf[len] = 0x60;
		len += 5;
		revBuf[len++] = 0x03;
		revBuf[len++] = 0x00;
//0038000001C00019
		revBuf[len++] = 0x00;//1-8
		revBuf[len++] = 0x38;//9-16
		revBuf[len++] = 0x00;//17-24
		revBuf[len++] = 0x00;//25-32
		revBuf[len++] = 0x01;//33-40
		revBuf[len++] = 0xC0;//41-48
		revBuf[len++] = 0x80;//49-56
		revBuf[len++] = 0x19;//57-64
		blen = Build8583_11(revBuf+len);
		len+=blen;
		blen = Build8583_12(revBuf+len);
		len+=blen;
		blen = Build8583_13(revBuf+len);
		len+=blen;
		blen = Build8583_40(revBuf+len);
		len+=blen;
		blen = Build8583_41(revBuf+len);
		len+=blen;
		blen = Build8583_42(revBuf+len);
		len+=blen;
		blen = Build8583_49(revBuf+len);
		len+=blen;
		blen = Build8583_60(revBuf+len, 2);
		if(blen == 0)
			return 0;
		len+=blen;
		blen = Build8583_61(revBuf+len);
		if(blen == 0){//˵�������Ѿ��������,����Ҫ�ٷ���
			return 0;
		}
		len+=blen;
		blen = Build8583_64(revBuf+7, len-7, revBuf+len);
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	case MISS_M_DPARAM://������������
		len = Build_DPARAM(revBuf, 0);
		break;
	case MISS_M_DPGRAM://������������
		len = Build_DPGRAM(revBuf);
		break;
	case MISS_M_DPGRAMOK://�������ؽ��
		len = Build_DPGRAMOK(revBuf);
		break;
	case MISS_M_KEK://����KEK
		len = Build_DKEK(revBuf);
		break;
	case MISS_M_ActiveTest://����
		len = Build_ActiveTest(revBuf);
		break;
// 	case MISS_M_LOGINGJ://��½��������
// 		break;
	default:
		len = 0;
		break;
	}
	return len;
}

//msgCMD �յ�������  BM λͼ  databuf ��λͼ������
void deal8583Dat(unsigned short msgCMD, unsigned char BM, unsigned char *datbuf, unsigned short len)
{
//#define debug_deal_8583_data

	char dtemp[20];
	unsigned char i;
	unsigned int itemp, itemp1;
	unsigned int index = 0;

	switch (BM+1)//������������,λ��Ҫ�Ȳ�����1,��Ϊ�Ǵ�1��ʼ��
	{
	case 11://�յ���ϵͳ���ٺ�
		itemp = BCD2HEX(datbuf, 3);
#ifdef debug_deal_8583_data
		debugstring("11���ٺ�:");
		debugdata((unsigned char*)&itemp, 4, 1);
#endif
// 		gmobileBakSeril = itemp;
// 		if(itemp > gMobilPOSNS){
// //		if(gMobilPOSNS != itemp){//�����Ĳ�һ�����������ͬ��
// 			gMobilPOSNS = itemp;
// 			sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);
// 		}
		break;
	case 12://�յ������ڵ�ʱ��
		memcpy(gTTime+4, datbuf, 3);
		break;
	case 13:
		memcpy(gTTime, datbuf, 4);
#ifdef debug_deal_8583_data
		debugstring("13ʱ��:");
		debugdata(gTTime, 14, 1);
#endif
		break;//�յ������ڵ�����
	case 15://��������
#ifdef debug_deal_8583_data
		debugstring("BM #15:��������\r\n");
#endif
		break;
	case 39:
#ifdef debug_deal_8583_data
		debugstring("Ӧ���룺");
		debugdata(datbuf, 2, 1);
#endif
		memset(dtemp, 0, 4);
		memcpy(dtemp, datbuf, 2);
		miniDispstr(7,17,dtemp,0);
		memcpy((unsigned char *)&g8583ACK, datbuf, 2);
		if(g8583ACK == 0x3030){// memcmp(datbuf, "\x30\x30", 2)== 0){//Ӧ����ȷ
			break;
		}
		else if(g8583ACK == 0x3545){// (memcmp(datbuf, "\x45\x35", 2) == 0){//��Ҫ����KEK
			gmissOldflag = gmissflag;
			gmissflag = MISS_M_KEK;
			break;
		}
		else if(g8583ACK == 0x3534){// (memcmp(datbuf, "45", 2) == 0){//��Ҫ������ǩ��
//			gmissflag = MISS_M_SINGIN;
			break;
		}
		else if(g8583ACK == 0x3038){//MAC������Ҫ������KEK������
			gmissOldflag = MISS_M_DPARAM;
			gmissflag = MISS_M_KEK;
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);//�رյ�����
			return;
		}
		else if(g8583ACK == 0x3939){//POSPϵͳ�쳣,����汾�Ŵ���
			for(i=0;i<14;i++){
				if((gmobileProVer[i]>'9')||(gmobileProVer[i]<'0')){
					memset(gmobileProVer, '0', 14);
					break;
				}
			}
			gmobileErrTimes++;
		}
		break;
	case 40://�ն����к�
	case 41://POS�ն˺�
	case 42://�ܿ�����ʶ�� = �̻���
		break;
	case 49://���׻��Ҵ���
#ifdef debug_deal_8583_data
		debugstring("BM #49:���׻��Ҵ���\r\n");
#endif
		break;
	case 60://�Զ���Ӧ��������
		if(msgCMD == 0x1008){//ǩ��
			memcpy(gMobileParameter.Ver, datbuf+32, 14);
			itemp = dealPiciNo();
			itemp1 = Ascii2INT(datbuf+50, 6);
			for(i=0;i<6;i++){
				if((datbuf[50+i]<'0')||(datbuf[50+i]>'9')){//���ص��д���
					break;
				}
			}
			if((i<6)||(itemp1>999999)){
				break;
			}
			else{
				if(itemp1 > itemp){
					;//memcpy(gMobileParameter.posPiCiNo, datbuf+50, 6);tttttttttttttttttttttttttttttttttt
				}
			}
		}
#ifdef debug_deal_8583_data
		debugstring("BM #60 ���²���Ver:");
		debugdata(gMobileParameter.Ver, 14, 1);
		debugstring("BM #60 ���κ�:");
		debugdata(datbuf+50, 6, 1);
#endif
		break;
	case 61://�ѻ����ѡ����ѳ������˻�������������
#ifdef debug_deal_8583_data
		debugstring("BM #61 ����������Ӧ��\r\n");
		debugdata(gMobileParameter.Ver, 14, 1);
#endif
		break;
	case 63:
#ifdef debug_deal_8583_data
		debugstring("BM #63:");
#endif
		index = 0;
		if(msgCMD == 0x1008){//ǩ��

			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);//�رյ�����
			if(g8583ACK != 0x3030){
#ifdef debug_deal_8583_data
				debugstring("ǩ��ERROR!\r\n");
#endif
				first_clr_mobile_para();
				gmissflag = MISS_M_KEK;
				gmissOldflag = MISS_M_DPARAM;
				gmobileErrTimes = 0;
				return;
			}
			if(datbuf[index] == 0)//Ӧ�ó�����±�־
				gMobilStyle.LoadStyle &= (~LoadStyle_program);//����Ҫ���س���
			else
				gMobilStyle.LoadStyle |= LoadStyle_program;//��Ҫ���س���
			index++;
			if(datbuf[index] == 0)//�������±�־
				gMobilStyle.LoadStyle &= (~LoadStyle_para);//����Ҫ���ز���
			else
				gMobilStyle.LoadStyle |= LoadStyle_para;//��Ҫ���ز���
			index++;
			memcpy(gTTime, datbuf+index, 7);
			if(time_invalid(gTTime) == 1){//����ʱ��
				timewrite(gTTime);
			}
			index+=7;
			memcpy(gMobilStyle.PIN_KEY, datbuf+index, 16);//PIN_KEY
			index += 16;
			memcpy(gMobilStyle.PIN_KEY_CHECKVALUE, datbuf+index, 4);
			index += 4;
			memcpy(gMobilStyle.MAC_KEY, datbuf+index, 16);
			index += 16;
			memcpy(gMobilStyle.MAC_KEY_CHECKVALUE, datbuf+index, 4);//������Կ
			memcpy(gMobilStyle.SingInDate, gTTime, 7);
			gMobilStyle.ISOK = 1;//ǩ������
			
			i = (gMobileParameter.KEK_SN*16);
			memset(gMACKEY_M, 0, 16);
 			DES3_decrypt(gMobilStyle.MAC_KEY, gMobileParameter.KEK+i, gMACKEY_M);//����MAC_KEY
 			DES3_decrypt(gMobilStyle.MAC_KEY+8, gMobileParameter.KEK+i, gMACKEY_M+8);
#ifdef _debug_
			debugstring("ǩ��gMACKEY_M:");
			debugdata(gMACKEY_M, 16, 1);
#endif			
			sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
#ifdef debug_deal_8583_data
			debugstring("ǩ���ɹ�:");
			debugdata((unsigned char*)&gMobilStyle, 60, 1);
#endif
			uploadMobilePara();
#ifdef debug_deal_8583_data
			debugstring("gmissflag:");
			debugdata((unsigned char*)&gmissflag, 1, 1);
#endif
		}
		else if(msgCMD == 0x1005){
#ifdef debug_deal_8583_data
			debugstring("���������:");
			debugdata(datbuf, len, 1);
#endif
			;
		}
		break;
	case 64://MAC 8Byte
#ifdef debug_deal_8583_data
		debugstring("BM #64:");
		debugdata(datbuf, 8, 1);
#endif
//		if(gmissflag == MISS_M_Settlement){//�յ��������Ӧ
		if(msgCMD == 0x1005){
#ifdef debug_deal_8583_data
			debugstring("�յ��������Ӧ:");
			debugdata((unsigned char*)&g8583ACK, 2, 0);
#endif
			if(g8583ACK == 0x3030){//�ɹ�
#ifdef debug_deal_8583_data
				debugstring("�ɹ�\r\n");
#endif
				gMobileParameter.gMobil_Settlement_error = 0;//����ɹ�
				gMobileParameter.addPICINO_limit = 0;
				
				sysferead(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
				if((mobilsendStyle.SendAddr%MOBILE_REC_LEN)!=0)//����ָ��Ĵ���
				{
					mobilsendStyle.SendAddr -= (mobilsendStyle.SendAddr%MOBILE_REC_LEN);
				}
				
				itemp = (mobilsendStyle.SendAddr+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;
				itemp1 = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;//���м�¼������

				if(itemp > itemp1){
					gMobilREC_Start = gMobilREC_End;//������͵����ݱ�ȫ�����ݻ��࣬���͵���Ϣ�д�
				}
				else
					gMobilREC_Start = mobilsendStyle.SendAddr;

  				sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
				writeMobilStarBAK((unsigned char*)&gMobilREC_Start);

				memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);

				itemp = dealPiciNo();
				itemp++;
				itemp %= 1000000;
				sprintf(dtemp, "%06d", itemp);
				memcpy(gMobileParameter.posPiCiNo, dtemp, 6);
				
				dtemp[6] = 0x55;
				for(i=0; i<6; i++){
					dtemp[6] ^= dtemp[i];
				}
				sysfewrite(BIT_PICINO_BAK, 7, (unsigned char*)dtemp);//�������κ�

				gmissNotTrad = 0;//�ϴ��ƶ���¼���

				uploadMobilePara();
				memset((unsigned char*)&gMobilStyle, 0, sizeof(stMobilStyle));//��Ҫ����ǩ��
				sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
				gmissOldflag = 0;
				gmissflag = 0;
				find_new_mission();
				display(0, 4, "�ϴ����",DIS_CENTER|DIS_CONVERT);
				g24GDisFlash = 3;
				restore_flag = 0;
				gSendSeril = 0xffffffff;
				gmobileErrTimes = 0;
				save_FE_SendSeril();
			}
			else if(g8583ACK == 0x3734){
#ifdef _debug_
				debugstring("��ƽ\r\n");
				debugstring("gMobil_Settlement_error:");
				debugdata((unsigned char*)&gMobileParameter.gMobil_Settlement_error, 1, 1);
#endif
//				display(0, 0, "���˲�ƽ",DIS_CENTER);

				gMobileParameter.gMobil_Settlement_error++;//�����������һ.
				
				if((gMobileParameter.gMobil_Settlement_error >= 2)&&(gMobileParameter.addPICINO_limit < 10)){//���κż�һ
					itemp = dealPiciNo();
					if(gMobileParameter.addPICINO_limit == 5)
						itemp += 30;
					else
						itemp ++;
					itemp %= 1000000;
					sprintf((char*)gMobileParameter.posPiCiNo, "%06d", itemp);
					gMobileParameter.addPICINO_limit++;
					gMobileParameter.gMobil_Settlement_error = 0;

					memcpy(dtemp, gMobileParameter.posPiCiNo, 6);
					dtemp[6] = 0x55;
					for(i=0; i<6; i++){
						dtemp[6] ^= dtemp[i];
					}
					sysfewrite(BIT_PICINO_BAK, 7, (unsigned char*)dtemp);//�������κ�
					stopCCgprs(ERROR_CONNECT_MOBILE);
				}
				uploadMobilePara();

				gSendSeril = 0xffffffff;
				save_FE_SendSeril();
				gseflag = 0;
// 				gMobilREC_Start = mobilsendStyle.StartAddr;//��Ҫ���͵�ָ��ָ���¼ͷ
// 				sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
	
				memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));//�������״̬�����·���
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);

				uploadMobileData();

				uploadSettleData();
				
				gmissOldflag = 0;
				gmissflag = 0;
				find_new_mission();
// 				gMobilStyle.ISOK = 0;
// 				gmissflag = MISS_M_SINGIN;
			}
			else if(g8583ACK == 0x3038){//MAC������Ҫ������KEK������
				gmissOldflag = MISS_M_DPARAM;
				gmissflag = MISS_M_KEK;
			}
			else{//ʧ��
#ifdef debug_deal_8583_data
				debugstring("ʧ��\r\n");
#endif
//				display(0, 0, "�ϴ�����ʧ��",DIS_CENTER);
//				display(3, 0, "�������ϴ�",DIS_CENTER);
//				gmissflag = MISS_M_Settlement;//������
				
				gSendSeril = 0xffffffff;
				save_FE_SendSeril();
				gseflag = 0;

// 				gMobilREC_Start = mobilsendStyle.StartAddr;//��Ҫ���͵�ָ��ָ���¼ͷ
// 				sysfewrite(BIT_MobilREC_Start, 4, (unsigned char*)&gMobilREC_Start);
				
				memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));//�������״̬�����·���
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
				
				uploadMobileData();

// 				itemp = dealPiciNo(); 
// 				itemp ++;
// 				itemp %= 1000000;
// 				sprintf((char*)gMobileParameter.posPiCiNo, "%06u", itemp);
				uploadMobilePara();

				find_new_mission();
				gMobilStyle.ISOK = 0;
				gmissOldflag = 0;
				gmissflag = MISS_M_SINGIN;
				gmissOldflag = MISS_M_TradeBatch;
				gmobileErrTimes++;
			}
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);//�رյ�����
		}
//		else if(gmissflag == MISS_M_TradeBatch){//�յ������ͻ�Ӧ
		else if(msgCMD == 0x1003){
#ifdef debug_deal_8583_data
			debugstring("�յ�������Ӧ��:");
			debugdata((unsigned char*)&g8583ACK, 2, 1);
#endif
			if(g8583ACK == 0x3030){//Ӧ����ȷ
				sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);//�����Ѿ����͵���Ϣ
				save_FE_SendSeril();
				gmissflag = 0;
				find_new_mission();
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);//�رյ�����
				gmobileErrTimes = 0;
			}
			else  if(g8583ACK == 0x3038){//MAC������Ҫ������KEK������
				gmissOldflag = MISS_M_DPARAM;
				gmissflag = MISS_M_KEK;
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);//�رյ�����
			}
			else{//���Ͳ��ɹ�
#ifdef debug_deal_8583_data
				debugstring("�յ�������Ӧ�𲻳ɹ�\r\n");
#endif
				sysferead(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);//���ط���ǰ��״̬
				load_fe_SendSeril();

				gmissflag = 0;
				find_new_mission();
				gmobileErrTimes++;
// 				first_clr_mobile_para();
// 				gmissflag = MISS_M_KEK;
// 				gmissOldflag = MISS_M_DPARAM;//�������ز�����KEK��
			}
		}
//		else if(gmissflag == MISS_M_SINGIN){
		else if(msgCMD == 0x1008){//ǩ��
#ifdef debug_deal_8583_data
			debugstring("ǩ�������64:");
#endif
			gmissflag = gmissOldflag;
//			if((gmissflag != MISS_M_KEK)&&(gmissflag != MISS_M_DPGRAM)){//��һ���������������KEK������������
			if(gmissflag != MISS_M_KEK){
				gmissflag = 0;//��������µ�����
				find_new_mission();
			}
			if(gmissflag == MISS_M_DPGRAM)
				restore_flag = 1;
#ifdef debug_deal_8583_data
			debugstring("gmissflag:");
			debugdata((unsigned char*)&gmissflag, 1, 1);
#endif
		}
		break;
	default:
#ifdef debug_deal_8583_data
		debugstring("BM unKnown:");
		debugdata((unsigned char *)&BM, 1, 1);
#endif
		break;
	}
	
// 	if(msgCMD == 0x0810){//ǩ��
// 	;}
// 	else if(msgCMD == 0x0510){//������
// #ifdef debug_deal_8583_data
// 		debugstring("��������Ϣ����:");
// 		debugdata((unsigned char*)&msgCMD, 2, 1);
// #endif
// 	}
// 	else if(msgCMD == 0x0310){//�����ͽ���
// #ifdef debug_deal_8583_data
// 		debugstring("�����ͽ�����Ϣ����:");
// 		debugdata((unsigned char*)&msgCMD, 2, 1);
// #endif
// 	}
// 	else {//δ֪����
// 		;
// #ifdef debug_deal_8583_data
// 		debugstring("δ֪8583����Ϣ����:");
// 		debugdata((unsigned char*)&msgCMD, 2, 1);
// #endif
//	}

}

void IF2TLVDeal(unsigned char *pakege)
{
	unsigned char buff[64];
	unsigned char Tver[14];//Ӧ��Ĳ����汾
	unsigned short Tack=0x55;
	unsigned char *p;
	unsigned int index,itemp,itemp1, i, addr;//, it2;
	unsigned short Alen;//�˰��ܳ���
	unsigned char Tlen;//
	unsigned short proLen=0;//���񸽼�����
	unsigned char *proP;//������ʼָ��
	unsigned char proCmd=0;//����
	unsigned char dealtype=0;//����
	stdownProgPara bakdownProgPara;//���������������
	unsigned char paraCF=0;//�����Ƿ����
//	unsigned int iss;
	unsigned char TAGerror=0;

#ifdef _debug_
	debugstring("deal IF2 CMD:");
#endif
	memset(gTTime, 0, 14);

	index = 0;
	p = pakege;
	proP = p;
	p += 3;//ǰ�����ֽ��� ֡���ͺ�֡���ȣ��������޹�
#ifdef _debug_
	debugdata(p, 10, 1);
#endif
	if(p[index] != 0x02)
	{
#ifdef debug_IF2_TLVDeal_
		debugstring("STX Error!!\r\n");
#endif
		return;
	}
	index++;//ָ����ĳ��� ����	ʮ������2λ����λ��ǰ����λ�ں�
	memcpy((unsigned char *)&Alen, p+index, 2);
	index+=2;
	over_turn(2, (unsigned char*)&Alen);
#ifdef _debug_
	debugstring("ALEN:");
	debugdata((unsigned char*)&Alen, 2, 1);
#endif
	while(index < (Alen+3)){
#ifdef debug_IF2_TLVDeal_
		debugstring("TAG:");
		debugdata(p+index, p[index+1]+2, 1);
#endif
		switch (p[index])
		{
		case 0xff://TAG ��������
			index++;//LEN
			Tlen = p[index++];
			dealtype = p[index];//ָ������
			index += Tlen;
			if(dealtype > 0x04)//����04�Ǵ��
				return;
			break;
		case 0x0b://POS��ˮ��
			index++;
			Tlen = p[index++];
// 			if(Tlen < 8){
// 				iss = str2int(p+index, Tlen);
// 				gmobileBakSeril = iss;
// 				if(iss > gMobilPOSNS){
// 					gMobilPOSNS = iss;
// 					if(gMobilPOSNS > 999999)
// 						gMobilPOSNS = 1;
// 					sysfewrite(BIT_MobilPOSNS, 4, (unsigned char*)&gMobilPOSNS);
// 				}
// 			}
			index += Tlen;
			break;
		case 0x0c://�ն�ʱ��
			index++;//LEN
			Tlen = p[index++];
			Ascii2BCD(p+index, gTTime+4, 6);//�������� ʱ��
			index += Tlen;
			break;
		case 0x0d://�ն�����
			index++;//LEN
			Tlen = p[index++];
			Ascii2BCD(p+index, gTTime, 8);//�������� ����
			index += Tlen;
			break;
		case 0x27://Ӧ����
			index++;//LEN
			Tlen = p[index++];
			memcpy((unsigned char*)&Tack, p+index, 2);//��������
			index += Tlen;
			
			memset((char*)buff, 0, 4);
			memcpy((char*)buff, (unsigned char*)&Tack, 2);
			miniDispstr(7,17,(char*)buff,0);
			if(memcmp((unsigned char*)&Tack, "D1", 2) == 0){
				getMobilePara();
				gmobileErrTimes++;
				return;
			}
			else if(Tack != 0x3030){
				gMobilConnectErr++;
				if(gMobilConnectErr > 5)
					gMobilConnectErr = 35;//�ӳ�30��
				gmobileErrTimes++;
				return;
			}
			else{
				gMobilConnectErr = 0;
			}
			break;
		case 0x28://�ն����к�
			index++;//LEN
			Tlen = p[index++];
//			memcpy(gMobileParameter.posSerilNo, p+index, 16);//��������
			index += Tlen;
			break;
		case 0x29://POS�ն˺�
			index++;//LEN
			Tlen = p[index++];
#ifdef MOBILE_SHIYAN_
			if(Tack == 0x3030)
				memcpy(gMobileParameter.posNo, p+index, 8);//��������
#endif
			index += Tlen;
			break;
		case 0x2a://�̻����
			index++;//LEN
			Tlen = p[index++];
			if(Tack == 0x3030)
				memcpy(gMobileParameter.shopNo, p+index, 15);//��������
			index += Tlen;
			break;
		case 0x31://���ױ���
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0x90://Ӧ�ñ�ʶ��AID
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0x93://�̻����Ƽ�д20
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.organName, p+index, 20);//��������
			index += Tlen;
			break;
		case 0x94://POSӦ������1
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0x95://POS���κ�
			index++;//LEN
			Tlen = p[index++];
			
			if(Tack == 0x3030){
				itemp = dealPiciNo();
				itemp1 = Ascii2INT(p+index, 6);
				for(i=0;i<6;i++){
					if((p[index+i]<'0')||(p[index+i]>'9')){//���ص��д���
						break;
					}
				}
				if((i<6)||(itemp1>999999)){
					;
				}
				else{
#ifdef _MOBILE_IP_XiAN_//����ƽ̨�����κ�һ��Ҫ�ͺ�̨һ�£�������
					if(itemp1 != itemp){
						memcpy(gMobileParameter.posPiCiNo, p+index, 6);
						uploadMobilePara();//�������ķ������Ĳ���
						
						memcpy(buff, mobilsendStyle.Single, 8);//���κű仯�������ϴ�����
						memset((unsigned char*)&mobilsendStyle, 0, sizeof(stMobilSend));
						mobilsendStyle.StartAddr = gMobilREC_Start;
						mobilsendStyle.SendAddr = gMobilREC_Start;
						mobilsendStyle.EndAddr = gMobilREC_End;
						mobilsendStyle.SendPiCI = dealPiciNo();
						memcpy(mobilsendStyle.Single, buff, 8);
						mobilsendStyle.EndAddr = gMobilREC_End;
						
						gSendSeril = 0xffffffff;
						memset(buff, 5, 0);
						sysfewrite(BIT_SendSeril, 5, buff);
						gseflag = 0;
						sysfewrite(BIT_SendInfo, sizeof(stMobilSend), (unsigned char*)&mobilsendStyle);
					}
#else

					if(itemp1 > itemp){
						memcpy(gMobileParameter.posPiCiNo, p+index, 6);
					}
#endif
				}
			}

			index += Tlen;
			break;
		case 0x96://��ʼƾ֤��
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0x97://���׳�ʱʱ��
			index++;//LEN
			Tlen = p[index++];
			gMobileParameter.puchaceTimeOut = p[index];//��������
			index += Tlen;
			break;
		case 0x98://��ӡƱ�ݵ���
			index++;//LEN
			Tlen = p[index++];
			gMobileParameter.tikectNumber = p[index];//��������
			index += Tlen;
			break;
		case 0x99://����Ľ�������
			index++;//LEN
			Tlen = p[index++];
			if(Tlen > 8)
				memcpy(gMobileParameter.prchaceType, p+index, 8);
			else
				memcpy(gMobileParameter.prchaceType, p+index, Tlen);
			index += Tlen;
			break;
		case 0x9a://�����Զ�ǩ��
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0x9b://POS��������
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.posAcktime, p+index, 8);//��������
			index += Tlen;
			break;
		case 0x9c://�ն˵��ʽ��׽������
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.oneMaxMoney, p+index, 12);//��������
			index += Tlen;
			break;
		case 0x9d://�ն��ۼƽ��׽������
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.posMaxmoney, p+index, 12);//��������
			index += Tlen;
			break;
		case 0x9e://�����������
			index++;//LEN
			Tlen = p[index++];
			gMobileParameter.congZenMactimes = p[index];//��������
			index += Tlen;
			break;
		case 0x9f://�ն�ÿ������ױ���
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.posMaxPurTimes, p+index, 3);
			index += Tlen;
			break;
		case 0xa0://��ӡ���Ƿ�����
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xa1://ͨѶģ���Ƿ�����
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xa2://�������״̬����
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xa3://��������λ״̬
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xa4://PSAM�Ƿ����
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xa5://ͨѶ�ɹ���
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xa6://�ն�����״̬
			index++;//LEN
			Tlen = p[index++];
			if(Tack == 0x3030){
				gMobileParameter.posRunStyle = p[index];//��������
				if(gMobileParameter.posRunStyle == 0x01){//����ǩ��״̬����һ��ǩ��
					gmissOldflag = 0;
					gmissflag = MISS_M_SINGIN;
				}
			}
			index += Tlen;
			break;
		case 0xa7://ƽ���޹�������ʱ��
			index++;//LEN
			Tlen = p[index++];
			if(Tlen > 8)
				memcpy(gMobileParameter.posOKGoTimes, p+index, 8);
			else
				memcpy(gMobileParameter.posOKGoTimes, p+index, Tlen);
			index += Tlen;
			break;
		case 0xa8://POS����ʱ��
			index++;//LEN
			Tlen = p[index++];
			if(Tlen > 8)
				memcpy(gMobileParameter.posOnlineTimes, p+index, 8);
			else
				memcpy(gMobileParameter.posOnlineTimes, p+index, Tlen);//��������
			index += Tlen;
			break;
		case 0xa9://�ն�Ӧ�ó���汾��
			index++;//LEN
			Tlen = p[index++];
			if(Tlen == 14)
				memcpy(gMobileParameter.posProNewVer, p+index, 14);//��������
			index += Tlen;
			break;
		case 0xaa://�ն˲����汾��
			index++;//LEN
			Tlen = p[index++];
			if(Tlen == 14)
				memcpy(Tver, p+index, 14);//��������,���صĲ������ȷ�����ʱ������������Ƿ����
			index += Tlen;
			break;
		case 0xab://�ն˲������ر�־
			index++;//LEN
			Tlen = p[index++];
			if(p[index] == 0x00)//������
				gMobilStyle.LoadStyle &= (~LoadStyle_para);//��������
			else if((p[index] == 0x01) || (p[index] == 0x02)){//��Ҫ����
				gMobilStyle.LoadStyle |= LoadStyle_para;
				gmissOldflag = MISS_M_DPARAM;
				gmissflag = MISS_M_DPARAM;
				paraCF = 1;
			}
			index += Tlen;
			break;
		case 0xac://�ն�Ӧ�ó������ر�־
			index++;//LEN
			Tlen = p[index++];
			if(p[index] == 0x00)//������
				gMobilStyle.LoadStyle &= (~LoadStyle_program);//��������
			else if((p[index] == 0x01) || (p[index] == 0x02)){//��Ҫ����
//				gMobilStyle.LoadStyle |= LoadStyle_program;
//				gmissOldflag = MISS_M_DPGRAM;
//				gmissflag = MISS_M_DPGRAM;
				paraCF = 1;
				restore_flag = 1;
			}
			index += Tlen;
			break;
		case 0xad://KEK������
			index++;//LEN
			Tlen = p[index++];
			if(Tlen == 1)
				gMobileParameter.KEK_SN = p[index];//��������
			index += Tlen;
			break;
		case 0xae://KEK
			index++;//LEN
			Tlen = p[index++];
			if(Tlen == 48)
				memcpy(gMobileParameter.KEK, p+index, 48);//��������
			index += Tlen;
			break;
		case 0xb0://�������ص�ַ
			index++;//LEN
			Tlen = p[index++];
			//memcpy(gMobileParameter.purGate, p+index, 15);//��������
			index += Tlen;
			break;
		case 0xb1://����APN
			index++;//LEN
			Tlen = p[index++];
			//memcpy(gMobileParameter.pruAPN, p+index, 20);//��������
			index += Tlen;
			break;
		case 0xb2://�û���
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xb3://����
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xb4://����IP��ַ
			index++;//LEN
			Tlen = p[index++];
//			memcpy(gMobileParameter.purIP, p+index, 15);//��������
			index += Tlen;
			break;
		case 0xb5://���׶˿ں�
			index++;//LEN
			Tlen = p[index++];
//			memset(gMobileParameter.purPort, 0, 6);
			if(Tlen > 6)
				memcpy(gMobileParameter.purPort, p+index, 6);//��������
			else
				memcpy(gMobileParameter.purPort, p+index, Tlen);//��������
			index += Tlen;
			break;
		case 0xb6://���ײ��ŵ绰1
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xb7://���ײ��ŵ绰2
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xb8://���ײ��ŵ绰3
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xc0://�������ص�ַ
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.manageGate, p+index, 15);//��������
			index += Tlen;
			break;
		case 0xc1://����APN
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.manageAPN, p+index, 20);//��������
			index += Tlen;
			break;
		case 0xc2://�û���
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xc3://����
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xc4://����IP��ַ
			index++;//LEN
			Tlen = p[index++];
//			memcpy(gMobileParameter.manageIP, p+index, 15);//��������
			index += Tlen;
			break;
		case 0xc5://����˿ں�
			index++;//LEN
			Tlen = p[index++];
// 			memset(gMobileParameter.managePort, 0, 6);
// 			if(Tlen > 6)
// 				memcpy(gMobileParameter.managePort, p+index, 6);//��������
// 			else
// 				memcpy(gMobileParameter.managePort, p+index, Tlen);//��������
			index += Tlen;
			break;
		case 0xc6://�����ŵ绰1
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xc7://�����ŵ绰2
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xc8://�����ŵ绰3
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		case 0xc9://�������
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.organNo, p+index, 10);//��������
			index += Tlen;
			break;
		case 0xca://�������Ƽ�д
			index++;//LEN
			Tlen = p[index++];
			memcpy(gMobileParameter.organName, p+index, 20);//��������
			index += Tlen;
			break;
		case 0xfe://������������
			index++;//LEN
			Tlen = p[index++];
			proCmd = p[index];//��������
			index += Tlen;
			break;
		case 0xfc://�����
			index++;//LEN
			Tlen = p[index++];
			memcpy((unsigned char*)&bakdownProgPara, (unsigned char*)&downProgPara, 27);
			memcpy((unsigned char*)&downProgPara, p+index, 27);//��������
			index += Tlen;
			break;
		case 0xfd://��������
			index++;//LEN
			Tlen = p[index++];
			proLen = 0;
			if(Tlen == 1)
				proLen = p[index];
			else{
				memcpy((unsigned char*)&proLen, p+index, 2);//��������
				over_turn(2, (unsigned char*)&proLen);
			}
			index += Tlen;
			proP = p+index;
			goto IF2DealOK; //ȡ���������ȣ��������
//			break;
		default:
#ifdef debug_IF2_TLVDeal_
			debugstring("TLV TAG ERROR:");
			debugdata(p+index, 3, 1);
#endif
			TAGerror++;
			if(TAGerror > 8){//����8�Σ���Ϊ�Ǵ���İ�
				return;
			}
			index++;//LEN
			Tlen = p[index++];
			//��������
			index += Tlen;
			break;
		}
	}
IF2DealOK:
	if(dealtype == 0x01){//����
#ifdef debug_IF2_TLVDeal_
		debugstring("���� �������\r\n");
#endif
		if(paraCF == 1){
			sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
		}
		gmissflag = gmissOldflag;//������һ������
		if(gmissflag == MISS_M_ActiveTest){
			gmissOldflag = 0;
			gmissflag = 0;
			find_new_mission();
		}
	}
	else if(dealtype == 0x02){//��������
#ifdef debug_IF2_TLVDeal_
		debugstring("�������� �������\r\n");
#endif
		if((memcmp(gMobileParameter.Ver, "\x00\x00\x00\x00\x00", 5)==0)||
			(memcmp(gMobileParameter.Ver, "\x30\x30\x30\x30\x30", 5)==0)){
			memcpy(gMobileParameter.Ver, "20100513010203", 14);
		}
		GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
		uploadMobilePara();//�������ķ������Ĳ���
		gMobilStyle.LoadStyle &= (~LoadStyle_para);
#ifdef debug_IF2_TLVDeal_
		debugdata((unsigned char*)&gMobilStyle, sizeof(stMobilStyle), 1);
#endif
		sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
		gmissflag = MISS_M_SINGIN;//������һ������
		if(gmissflag == MISS_M_DPARAM){
			gmissOldflag = 0;
			gmissflag = 0;
			find_new_mission();
		}
		
	}
	else if(dealtype == 0x03){//Ӧ�ó�������
		GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
		if(proCmd == 2){//���ؽ��֪ͨ���
#ifdef debug_IF2_TLVDeal_
			debugstring("�������ؽ��֪ͨ���\r\n");
#endif
			;//Tack  Ӧ�����ж�
			gmissOldflag = 0;
			gmissflag = 0;
			memset((unsigned char*)&gMobilStyle, 0, sizeof(stMobilStyle));
			sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);//��Ҫ����ǩ��
			memset((unsigned char*)&downProgPara, 0, 27);
			sysfewrite(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);//�����������
			display(3,0,"���,������...",DIS_CENTER);
// 			beep(1,10,10);
// 			ReStartCPU();//������ɣ������豸�Ա���³���
		}
		else if(proCmd == 1){//��������
			if(Tack != 0x3030)
			{//Ӧ��ʧ��
#ifdef debug_IF2_TLVDeal_
				debugstring("Ӧ�����\r\n");
#endif
				if(Tack == 0x3939){
					memset((unsigned char*)&downProgPara, 0, sizeof(stdownProgPara));
					sysfewrite(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);//�����������
				}
//				gmissflag = MISS_M_DPGRAM;//���س���
				return;
			}
			memcpy((unsigned char*)&index, downProgPara.Miss_len, 4);//�����С
			over_turn(4, (unsigned char*)&index);
			memcpy((unsigned char*)&itemp, downProgPara.Miss_pesent+1, 4);//ƫ����
			over_turn(4, (unsigned char*)&itemp);
#ifdef debug_IF2_TLVDeal_
			sprintf((char*)buff, "������С:%d ƫ����:%d\r\n", proLen, itemp);
			debugstring((char*)buff);
#endif
			if(proLen == itemp){//�������ȣ���ƫ�������յ������һ����
				addr = FLASH_PRO_START;//������ʼ��ַ
				for(i=0; i<4; i++){
					flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 256K
					addr += 0x10000;//��һ��64K�Ŀ�
				}
			}
			addr = (FLASH_PRO_START + itemp-proLen);//д��ĵ�ַ��ƫ����-Ҫд��ĳ���
#ifdef debug_IF2_TLVDeal_
			sprintf((char*)buff, "�����С:%d ƫ����:%d\r\n", index, itemp);
			debugstring((char*)buff);
			sprintf((char*)buff, "д�볤��:%d д���ַ:%d\r\n", proLen, addr);
			debugstring((char*)buff);
#endif
			sprintf((char*)buff, " %d%% ",  ((itemp*100)/index));
			display(5, 0, (char*)buff, DIS_CENTER);

			if(proLen % 2){
				*(p+proLen) = 0xff;//��ΪҪд��2������λ�����Բ���Ҫ��������0xff,��д��
				proLen++;
			}
			if(addr > FLASH_PRO_END){
				debugstring("��ַ����!\r\n");
				memcpy((unsigned char*)&downProgPara, (unsigned char*)&bakdownProgPara, 27);
				sysfewrite(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);//ȡ�ϴ����ز����ݡ���������
//				gmissflag = MISS_M_DPGRAM;//���س���
				restore_flag = 1;
				return;
			}
			else{
#ifdef debug_IF2_TLVDeal_
				debugstring("addr:");
				debugdata((unsigned char*)&addr, 4, 1);
				debugdata(proP, proLen, 1);
#endif
				flashwrite(addr, proP, proLen);//д��
			}
			
			downProgPara.Miss_dongzuo = 0x01;//����
			sysfewrite(BIT_MobilePRO_INFO, 27, (unsigned char*)&downProgPara);//�����������

			if(index == itemp)//�������
			{
				memset(buff, 0, 64);
				strcpy((char*)buff,"������Ҫ����");
				strcat((char*)buff,(char*)downProgPara.Miss_ver);
#ifdef debug_IF2_TLVDeal_
				debugstring("�����������:");
				debugstring((char*)buff);
				debugstring("\r\n");
#endif
				addr = FLASH_PRO_FLAG;
				flashwrite(addr, buff, 64);

//				gmissflag = MISS_M_DPGRAMOK;//���������ؽ��
				return;
			}
//			else
//				gmissflag = MISS_M_DPGRAM;//��������³���
		}
	}
	else if(dealtype == 0x04){//KEK����
#ifdef debug_IF2_TLVDeal_
		debugstring("KEK �������\r\n"); 
#endif
		if(Tack != 0x3030)
		{//Ӧ��ʧ��
#ifdef debug_IF2_TLVDeal_
			debugstring("Ӧ�����\r\n");
#endif
			stopCCgprs(ERROR_CONNECT_MOBILE);
			
			gmobileErrTimes++;
			return;
		}
		uploadMobilePara();//�������ķ������Ĳ��� ������ KEK
		gMobilStyle.LoadStyle &= (~LoadStyle_KEK);
#ifdef debug_IF2_TLVDeal_
		debugdata((unsigned char*)&gMobilStyle, sizeof(stMobilStyle), 1);
#endif
		sysfewrite(BIT_MobilStyle, sizeof(stMobilStyle), (unsigned char*)&gMobilStyle);
		gmissOldflag = MISS_M_SINGIN;
		gmissflag = MISS_M_DPARAM;//MISS_M_DPARAM;//������������ǩ��
		
		GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
		gmobileErrTimes = 0;
	}
	if(GPRSLinkProcess == GPRS_SENDING_CMD){
		GPRSLinkProcess = TCPSTARTSTAT;
	}
}
//00 98 00 60 00 00 00 00 08 10 00 38 00 00 03 C0 00 13 00 00 01 18 00 46 20 09 09 04 45 35 5A 5A 5A 5A 5A 5A 5A 5A 5A 5A 32 31 30 30 30 33 37 33 31 30 30 31 30 32 38 38 38 30 37 33 31 34 35 31 31 34 35 31 31 00 58 D1 56 00 01 01 80 03 80 00 00 00 01 00 00 00 00 30 30 32 30 30 39 30 39 30 33 31 30 33 31 35 37 46 46 30 31 30 30 30 30 30 31 31 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 00 16 00 20 09 09 04 18 00 46 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
void IF18583Deal(unsigned char *pakege)
{
	struct	ISO_8583 iso8583[128] = 
	{
		{  8, L_ASC, FIX_LEN},  //1
		{ 19, L_ASC, LLVAR_LEN}, //2
		{  6, L_BCD, FIX_LEN},	//3
		{  12, L_BCD, FIX_LEN},	//4
		{  8, L_BCD, FIX_LEN},	//5
		{  8, L_BCD, FIX_LEN},	//6
		{ 99, R_BCD, LLVAR_LEN},//7
		{  8, R_BCD, FIX_LEN},	//8
		{  8, R_BCD, FIX_LEN}, //9
		{  8, R_BCD, FIX_LEN}, //10
		{  6, R_BCD, FIX_LEN},	//11
		{  6, R_BCD, FIX_LEN},	//12
		{  8, R_BCD, FIX_LEN},	//13
		{  8, R_BCD, FIX_LEN},	//14
		{  4, R_BCD, FIX_LEN},	//15
		{  4, R_BCD, FIX_LEN},	//16
		{  4, R_BCD, FIX_LEN},	//17
		{  4, R_BCD, FIX_LEN},	//18
		{  4, R_BCD, FIX_LEN},	//19
		{  4, R_BCD, FIX_LEN},	//20
		{  4, R_BCD, FIX_LEN},	//21
		{  4, R_BCD, FIX_LEN}, 	//22
		{  4, R_BCD, FIX_LEN},	//23
		{  4, R_BCD, FIX_LEN},	//24
		{  2, R_BCD, FIX_LEN},	//25
		{  2, R_BCD, FIX_LEN},	//26
		{  99, L_BCD, LLVAR_LEN},	//27
		{  8, L_BCD, FIX_LEN},	//28
		{  99, L_BCD, LLVAR_LEN},	//29
		{  99, L_BCD, LLVAR_LEN},	//30
		{  99, L_BCD, LLVAR_LEN},	//31
		{  11, L_BCD, LLVAR_LEN},	//32
		{  99, L_BCD, LLVAR_LEN},	//33
		{ 999, L_BCD, LLLVAR_LEN},	//34
		{ 37,  L_BCD, LLVAR_LEN},	//35
		{104,  L_BCD, LLLVAR_LEN},	//36
		{ 12,  L_ASC, FIX_LEN},		//37
		{  6,  L_ASC, FIX_LEN},		//38
		{  2,  L_ASC, FIX_LEN},		//39
		{ 16,  L_ASC, FIX_LEN},	//40
		{  8,  R_ASC, FIX_LEN},		//41
		{ 15,  R_ASC, FIX_LEN},		//42
		{ 99,  L_BCD, LLVAR_LEN},	//43
		{ 25,  L_ASC, LLVAR_LEN},	//44
		{ 99,  L_BCD, LLVAR_LEN},	//45
		{999, R_BCD, LLLVAR_LEN},	//46
		{999, R_BCD, LLLVAR_LEN},	//47
		{999, R_BCD, LLLVAR_LEN},	//48
		{  3, R_ASC, FIX_LEN},		//49
		{  3, R_BCD, FIX_LEN},		//50
		{  3, R_BCD, FIX_LEN},		//51
		{  16, R_BCD, FIX_LEN},		//52
		{ 16, R_ASC, FIX_LEN},		//53
		{20, R_ASC, LLLVAR_LEN},	//54
		{999, R_ASC, LLLVAR_LEN},	//55
		{999, R_BCD, LLLVAR_LEN},	//56
		{999, R_BCD, LLLVAR_LEN},	//57
		{999, R_BCD, LLLVAR_LEN},	//58
		{999, R_BCD, LLLVAR_LEN},	//59
		{999, R_ASC, LLLVAR_LEN},	//60
		{999, L_ASC, LLVAR_LEN},	//61
		{999, R_BCD, LLLVAR_LEN},	//62
		{999, R_ASC, LLLVAR_LEN},	//63
		{ 16, R_BCD, FIX_LEN},		//64
		{  8, R_BCD, FIX_LEN},
		{  1, R_ASC, FIX_LEN},
		{  2, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  4, R_ASC, FIX_LEN},
		{  4, R_ASC, FIX_LEN},
		{  6, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 42, R_ASC, FIX_LEN},
		{  1, R_BCD, FIX_LEN},
		{  2, R_BCD, FIX_LEN},
		{  5, R_BCD, FIX_LEN},
		{  7, R_BCD, FIX_LEN},
		{ 42, R_BCD, FIX_LEN},
		{  8, R_BCD, FIX_LEN},
		{ 16, R_BCD, FIX_LEN},
		{ 25, R_BCD, FIX_LEN},
		{ 11, R_ASC, LLVAR_LEN},
		{ 11, R_ASC, LLVAR_LEN},
		{ 17, R_BCD, LLVAR_LEN},
		{ 28, R_BCD, LLVAR_LEN},
		{ 28, R_BCD, LLVAR_LEN},
		{10,  R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{  8, R_BCD, FIX_LEN}
	};

	unsigned short stemp;
	unsigned short sAlen=0;//�յ����ܵİ��Ĵ�С
	unsigned short index;
	unsigned short msgCmd=0;//��Ϣ����
	unsigned short sfi;
	unsigned int lc;
	unsigned char getbuff[1024];
	
	unsigned long long bitmap;//100110000000011000000000000110000000000000000 00111000 00000000 
	//00000000 00111000 00000000 00000000 00000011 11000000 00000000 00010011//0038000003C00013
	unsigned long long bitflag;//λ��־

	memset(gTTime, 0, 14);
	//	unsigned char msgtype;
	memcpy((unsigned char*)&sAlen, pakege+1, 2);//֡�ܳ���
#ifdef debug_deal_8583_data
	debugstring("deal IF1 CMD!\r\n");
	debugstring("sAlen:");
	debugdata((unsigned char *)&sAlen, 2, 1);
#endif
	if(sAlen == 2){//����һ�������ܳ���
		gmobileHeadFlag = 0xA9;
		return;
	}
	index = 3;//ָ����Ϣ���� TPDU 5�ֽ�
	index += 5;//ָ��������:ISO8583 Msg
	if(gmobileHeadFlag != 0xA9){//���û�з��ܳ��ȡ�����ʹ��ȫ�������ܳ���
		index += 2; //������������ڳ���ͬһ�����ڣ������ķ������ȷ����ȣ�������
		sAlen -= 2;//���ȼ�2����Ϊ���˳���
	}
	gmobileHeadFlag = 0;
	memcpy((unsigned char*)&msgCmd, pakege+index, 2);//�˰���Ϣ����
	index+=2;
	memcpy((unsigned char *)&bitmap, pakege+index, 8);//bitmap;
	over_turn(8, (unsigned char *)&bitmap);
#ifdef debug_deal_8583_data
	debugstring("bitmap:");
	debugdata((unsigned char *)&bitmap, 8, 1);
#endif
	index += 8;
	bitflag = 0x8000000000000000ull;
	g8583ACK = 0x3030;

	for(sfi=0; sfi<64; sfi++){
// #ifdef _debug_
// 		debugstring("bitflag:");
// 		debugdata((unsigned char *)&bitflag, 8, 1);
// #endif
		if((bitflag&bitmap)==0ull){
			bitflag >>= 1;
			continue;//��λ�����ڣ�������һλ
		}
#ifdef debug_deal_8583_data
		debugstring("bit site:");
		debugdata((unsigned char *)&sfi, 2, 1);
#endif
		lc = iso8583[sfi].len;
		if( iso8583[sfi].type == 0 )	/*�����BCD��*/
		{
			lc=(lc+1)/2;
		}
		else if( iso8583[sfi].type == 1 )  /*�����ASC*/
		{
			lc = lc;
		}
		else if( iso8583[sfi].type == 2 )	/*�Ҷ���BCD��*/
		{
			lc=(lc+1)/2;
		}
		else if( iso8583[sfi].type == 3 )	/*�Ҷ���ASC��*/
		{
			lc = lc;
		}
#ifdef debug_deal_8583_data
		debugstring("field lc1:");
		debugdata((unsigned char *)&lc, 4, 0);
#endif
		if(iso8583[sfi].flag == FIX_LEN)/*�̶�����*/
		{
			;
		}
		else if(iso8583[sfi].flag == LLVAR_LEN)/*��λ�䳤 1Byte*/
		{
			lc = 0;
			lc = (((pakege[index]>>4) * 10)+(pakege[index]&0x0f));
//			memcpy((unsigned char *)&lc, pakege+index, 2);//
//			over_turn(2, (unsigned char*)&lc);
			index += 1;
		}
		else/*��λ�䳤*/
		{
			lc = 0;
			lc = (((pakege[index]>>4) * 10)+(pakege[index]&0x0f));
			stemp = (((pakege[index+1]>>4) * 10)+(pakege[index+1]&0x0f));
			lc *= 100;
			lc += stemp;
			index += 2;
		}
#ifdef debug_deal_8583_data
		debugstring("field lc2:");
		debugdata((unsigned char *)&lc, 4, 1);
#endif
		if(lc >= 1000)//���ȴ���
			return;
		memcpy(getbuff, pakege+index, lc);
#ifdef debug_deal_8583_data
		debugstring("data:");
		debugdata(getbuff, lc, 1);
#endif
		deal8583Dat(msgCmd, sfi, getbuff, lc);
		index += lc;
		if((index >= sAlen)||(sfi >= 63))//�������ݳ��Ȼ����������һ���� ���
			return;
		bitflag >>= 1;

		if(g8583ACK == 0x3545){// (memcmp(datbuf, "\x45\x35", 2) == 0){//��Ҫ����KEK
			gmissOldflag = gmissflag;
			gmissflag = MISS_M_KEK;
			return;
		}
		else if(g8583ACK == 0x3534){// (memcmp(datbuf, "45", 2) == 0){//��Ҫ������ǩ��
			gmissflag = MISS_M_SINGIN;
			return;
		}
		else if(g8583ACK == 0x3038){//MAC������Ҫ������KEK������
			gmissOldflag = MISS_M_DPARAM;
			gmissflag = MISS_M_KEK;
			return;
		}
	}
}

//-----------���ڲ�������--------------
//�ж��յ����ݺ���ֹ����
// void MobileCOMDeal(unsigned char val)
// {
// 	switch (gLcdCmd.usIndex)
// 	{
// 	case 0:
// 		if(val != 0x02)
// 			return;
// 		gLcdCmd.databuf[gLcdCmd.usIndex++] = val;
// 		break;
// 	case 1:
// 		gLcdCmd.databuf[gLcdCmd.usIndex++] = val;
// 		gLcdCmd.usLen = val*256;//��Ϊ��λ
// 		break;
// 	case 2:
// 		gLcdCmd.databuf[gLcdCmd.usIndex++] = val;
// 		gLcdCmd.usLen += val;
// 		break;
// 	default:
// 		if(gLcdCmd.usIndex >= 1024){
// 			return;
// 		}
// 		gLcdCmd.databuf[gLcdCmd.usIndex++] = val;
// 		if(gLcdCmd.usIndex >= gLcdCmd.usLen+8)
// 		{
// //			debugstring("END:");
// //			debugdata((unsigned char *)&gLcdCmd.usIndex, 2, 1);
// 			gLcdCmd.ucValid = 0x01;			//������Ч
// 			return;
// 		}
// 		break;
// 	}
// }

void DownKEK(void)
{
	first_clr_mobile_para();
	cls();
	if(gchn_eng == 'E'){
		display(0,0,"download KEK",0);
		display(3,0,"exit PLS",DIS_CENTER);
	}
	else{
		display(0,0,"����KEK",0);
		display(3,0,"���˳��˵�",DIS_CENTER);
	}
	delayxs(1);
	return;
}
// extern void IF2TLVDeal(unsigned char *pakege);
// void DownMPara(void)
// {
// 	unsigned char *datBuff;
// 	unsigned int len;
// 	unsigned char key;
// 	
// 	cls();
// 	if(gchn_eng == 'E'){
// 		display(0,0,"POS init", DIS_CENTER);
// 		display(5,0,"press ok to start.",0);
// 	}
// 	else{
// 		display(0,0,"�豸��ʼ��", DIS_CENTER);
// 		display(3,0,"�Ӻ�POS�����ߺ�,",0);
// 		display(5,0,"��OK����ʼ.",0);
// 	}
// 	while(1){
// 		key = getkey(1);
// 		if(key == KEY_ENT)
// 			break;
// 		else if(key == KEY_ESC)
// 			return;
// 	}
// 	com_init(COM1, 115200);
// 	COM_it_enable(COM1);
// 
// 	cls();
// 	if(gchn_eng == 'E'){
// 		display(0,0,"download KEK", DIS_CENTER);
// 		display(3,0,"PLS waite...",0);
// 	}
// 	else{
// 		display(0,0,"KEK����", DIS_CENTER);
// 		display(3,0,"���Ե�...",DIS_CENTER);
// 	}
// 	gmissOldflag = gmissflag;
// 	gmissflag = MISS_COM_PARA;
// 
// 
// 	gLcdCmd.ucValid = 0;
// 	gLcdCmd.usIndex = 0;
// 	len = Build_DKEK(datBuff);
// 	com_snd(COM1, len, datBuff);
// 	len = 0;
// 	while (1)//�ȴ��ջ�Ӧ
// 	{
// 		if(gLcdCmd.ucValid == 0x01)
// 			break;
//  		len++;
// 		delayxms(10);
// 		if(len > 1000){
// 			cls();
// 			
// 			if(gchn_eng == 'E')
// 				display(3,0,"time out!",DIS_CENTER);
// 			else
// 				display(3,0,"ͨѶ��ʱ1!",DIS_CENTER);
// 			delayxs(3);
// 			goto DMPend;
// 		}
// 	}
// 	memcpy(datBuff+3, gLcdCmd.databuf, gLcdCmd.usIndex);//ǰ�����ֽ�����,��IF2TLVDeal����
// 	IF2TLVDeal(datBuff);//KEK
// 
// 	delayxms(100);
// 	
// 	gmissOldflag = gmissflag;
// 	gmissflag = MISS_COM_PARA;
// 	cls();
// 	if(gchn_eng == 'E'){
// 		display(0,0,"download KEK", DIS_CENTER);
// 		display(3,0,"PLS waite...",0);
// 	}
// 	else{
// 		display(0,0,"KEK����", DIS_CENTER);
// 		display(3,0,"���Ե�...",DIS_CENTER);
// 	}
// 	gLcdCmd.ucValid = 0;
// 	gLcdCmd.usIndex = 0;
// 	len = Build_DPARAM(datBuff, 0);
// 	com_snd(COM1, len, datBuff);
// 	len = 0;
// 	while (1)//�ȴ��ջ�Ӧ
// 	{
// 		if(gLcdCmd.ucValid == 0x01)
// 			break;
// 		delayxms(10);
//  		len++;
// 		if(len > 1000){
// 			cls();
// 			if(gchn_eng == 'E')
// 				display(3,0,"time out!",DIS_CENTER);
// 			else
// 				display(3,0,"ͨѶ��ʱ2!",DIS_CENTER);
// 			delayxs(3);
// 			goto DMPend;
// 		}
// 	}
// 	memcpy(datBuff+3, gLcdCmd.databuf, gLcdCmd.usIndex);//ǰ�����ֽ�����,��IF2TLVDeal����
// 	IF2TLVDeal(datBuff);//PARA
// 	cls();
// 	if(gchn_eng == 'E')
// 		display(3,0,"complete!",DIS_CENTER);
// 	else
// 		display(3,0,"��ʼ�����!",DIS_CENTER);
// 	delayxs(3);
// DMPend:
// 	gmissflag = gmissOldflag;
// 	gmissOldflag = 0;
// 
// 	com_init(COM1, 19200);
// 	COM_it_enable(COM1);
// }

#endif
