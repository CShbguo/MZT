
#include "..\include\include.h"
#include "szct.h"

#ifdef _GPS_ENABLE_
#include "Gps.h"
#include "string.h"
#include "stdio.h"
 
// $GPRMC,061316.00,A,2232.95729,N,11404.74220,E,0.079,,231008,,,D*72
// $GPVTG,,T,,M,0.079,N,0.146,K,D*2B
// $GPGGA,061316.00,2232.95729,N,11404.74220,E,2,08,3.38,21.8,M,-2.2,M,,*78
// $GPGSA,A,3,20,25,27,32,42,07,08,50,,,,,5.04,3.38,3.74*06
// $GPGSV,4,1,13,07,53,198,51,08,59,279,30,11,70,035,,13,02,190,*79
// $GPGSV,4,2,13,17,23,276,,19,,,34,20,25,139,48,25,36,184,45*4A
// $GPGSV,4,3,13,27,57,224,46,32,21,111,46,40,29,252,30,42,51,128,44*7C
// $GPGSV,4,4,13,50,46,122,40*49
// $GPGLL,2232.95729,N,11404.74220,E,061316.00,A,D*6D
void WriteGPSRec(unsigned char mode);
void ClrRunGPSData(void);
void ClrFindData(void);
void Load_Line_info(void);

extern void SoundManage(unsigned char mode, unsigned char index, unsigned char direct);
unsigned int distance[2];//����ʼվ�ľ���
#define GGA_FFLAG (1)
#define GLL_FFLAG (1<<1)
#define VTG_FFLAG (1<<2)
#define ZDA_FFLAG (1<<3)
#define RMC_FFLAG (1<<4)

#define US1_INTERRUPT_LEVEL 5

extern stBusVerIfo gBusVerInfo;
extern unsigned char gGpsDisEN;
extern Parameter3 ComSumeFile;
extern unsigned char restore_flag;

#ifdef KEY_PAD
extern volatile unsigned char gKeyRestorDis;//��Ҫ����ˢ��KEYPAD��ʾ
#endif
#ifdef FengDuan_BU_

extern unsigned char gFDderict; 
extern unsigned char gFDucID;
#endif
#ifdef _GPS_DIAODU_
//����GPS������Ϣ
unsigned int Build_GJ_ALAM(unsigned char *dat)
{
#ifndef NO_MOBILE_24G_
	unsigned int ltemp1;
	unsigned int sendenda;
#endif
	//	unsigned int ltemp;
	unsigned int len=0;
	unsigned short crc16;
	
	if((galamessage.alamFlag==0)||(galamessage.alamFlag > ALARM_LY))
		return 0;

	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	Get_SerialNum(dat+len);
	//	Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
	dat[len++] = 0x02;
	
	dat[len++] = 01;//GnCode 
	
	memset(dat+len, 0, 4);//P1��P4
	len+=4;
	
	dat[len++] = 0x00;//����=48�ֽ�
	dat[len++] = 0x40;
	
	memset(dat+len, 0, 4);
	len += 2;
	memcpy(dat+len, pFistVary.LineNo, 2);//����·4B
	len += 2;

	memset(dat+len, 0, 4);
	len += 2;//����·4B
	memcpy(dat+len, pFistVary.LineNo, 2);//����·4B
//	sysferead(BIT_GPSChildLine, 2, dat+len);
	len += 2;
	
	ASCII2ZBcod(pFistVary.DeviceNo, dat+len, 4);//����4B
	len +=4;
	
	memcpy(dat+len, DriveCardNo, 4);//��Ա4B
	len +=4;
	
	if(((RunStation.ucDirect < 2)&&(RunStation.ucStation > 0))||\
		((gGpsFeD.direct < 2)&&(gGpsFeD.flag > 0)))
		dat[len++] = 1;
	else
		dat[len++] = 2;//�Ƿ���Ӫ���� 1B 01-Ӫ���У�02-��Ӫ��

	if(RunStation.ucDirect >= 2){
		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
		if(gGpsFeD.direct<2){
			dat[len++] = ((LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId>>4)&0x0F)*10 + (LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId&0x0F); //վ����
			
			dat[len++] = 1;//վ������
			dat[len++] = gGpsFeD.direct+1;//վ�㷽��
		}
		else{
			dat[len++] = 0; //վ����
			
			dat[len++] = 1;//01-����վ��,02-�뿪վ��
			dat[len++] = 0;//վ�㷽��
		}
	}
	else{
		dat[len++] = ((LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId>>4)&0x0F)*10+(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId&0x0F);//վ����
		
		dat[len++] = 1;//01-����վ��,02-�뿪վ��
		dat[len++] = RunStation.ucDirect+1;//վ�㷽�� 01-����,02-����
	}
	memcpy(dat+len, (unsigned char*)&gGpsInfo.Longitude, 4);//+����4B
	over_turn(4, dat+len);
	len += 4;
	memcpy(dat+len, (unsigned char*)&gGpsInfo.Latitude, 4);//γ��4B
	over_turn(4, dat+len);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&gGpsInfo.velocity, 2);	//�ٶ�2B
	over_turn(2, dat+len);
	len += 2;
	
	memcpy(dat+len, gDiaoDuinfo.limitRDNo, 2);//·�α��
	len +=2; 

	dat[len] = galamessage.alamFlag;//��������1B
	len++;

	memcpy(dat+len, (unsigned char*)&SysTime, 7);
	len += 7;

	memset(dat+len, 0, 24);
	len += 24;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
	
}
#endif
#ifdef _GPS_NEED_REC_
//��GPS��λ��Ϣ
unsigned int Build_GJ_GPS(unsigned char *dat)
{
	unsigned int ltemp;
	unsigned int len=0;
	unsigned short crc16;
	
#ifdef _debug_gprs
	debugstring("gGpsInfo.ucValid:");
	debugdata((unsigned char*)&gGpsInfo.ucValid, 1, 1);
#endif
	if((gGpsInfo.ucValid != GPSVALID)&&(gGpsInfo.ucValid != GPSUES)){//GPS��Ϣ��Ч
		return 0;
	}
	
	len = 0;
	dat[len++] = 0xBD;//����ͷ(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	Get_SerialNum(dat+len);
//	Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
	dat[len++] = 0x01;
	
	dat[len++] = 01;//GnCode 
	
	memset(dat+len, 0, 4);//P1��P4
	len+=4;
	
	dat[len++] = 0x00;//����=48�ֽ�
	dat[len++] = 0x40;
	
	memset(dat+len, 0, 4);
	len += 2;
	memcpy(dat+len, pFistVary.LineNo, 2);//����·4B
	len += 2;
	
	memset(dat+len, 0, 4);
	len+=2;
//	sysferead(BIT_GPSChildLine, 2, dat+len);//����·4B   ��ʱ��ȡ��
	memcpy(dat+len, pFistVary.LineNo, 2);
	len+=2;
	
	ASCII2ZBcod((char*)pFistVary.DeviceNo, dat+len, 4);//����4B
	len +=4;
	
	memcpy((unsigned char*)&ltemp, DriveCardNo, 4);
	ltemp %= 100000000;
	memcpy(dat+len, (unsigned char*)&ltemp, 4);//��Ա4B
	over_turn(4, dat+len);
	len +=4;
#ifdef _GPS_DIAODU_
	if(gDiaoDuCMD.ucStartYorN != 2)
		dat[len++] = 1;
	else
		dat[len++] = 2;//ͣ�� �Ƿ���Ӫ���� 1B
#else
	dat[len++] = 1;
#endif
	
	if(RunStation.ucDirect >= 2){
		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
		if(gGpsFeD.direct<2){
			dat[len++] = ((LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId>>4)&0x0F)*10 + (LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId&0x0F); //վ����
			dat[len++] = 1;//վ������
			dat[len++] = gGpsFeD.direct+1;//վ�㷽��
		}
		else{
			dat[len++] = 1; //վ����
			
			dat[len++] = 1;//01-����վ��,02-�뿪վ��
			dat[len++] = 0;//վ�㷽��
		}
	}
	else{
		dat[len++] = ((LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId>>4)&0x0F)*10+(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId&0x0F);//վ����
		dat[len++] = 1;//01-����վ��,02-�뿪վ��
		dat[len++] = RunStation.ucDirect+1;//վ�㷽�� 00-����,01-����
	}
#ifdef _debug_
	debugstring("GPS��¼������ֵ��");
	debugdata((unsigned char*)&RunStation.ucDirect, 1, 1);
	debugdata((unsigned char*)&dat+len-1, 1, 1);
#endif

	memcpy(dat+len, (unsigned char*)&gGpsInfo.Longitude, 4);//+����4B
	over_turn(4, dat+len);
	len += 4;
	memcpy(dat+len, (unsigned char*)&gGpsInfo.Latitude, 4);//γ��4B
	over_turn(4, dat+len);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&gGpsInfo.velocity, 2);	//�ٶ�2B
	over_turn(2, dat+len);
	len += 2;
#ifdef _GPS_DIAODU_
	memcpy(dat+len, gDiaoDuinfo.limitRDNo, 2);//·�α��
#else
	memset(dat+len, 0, 2);
#endif
	len +=2; 
	
	dat[len++] = galamessage.alamFlag;//���һ�α�������
	
#ifdef _GPS_DIAODU_
	memcpy(dat+len, gDiaoDuCMD.ZLseiral, 2);//���һ�ε��ȱ��
#else
	memset(dat+len, 0, 2);
#endif
	len += 2;
	dat[len++] = 0;//�˴�

	memcpy(dat+len, (unsigned char*)&SysTime.year_h, 7);
	len += 7;
	
	memset(dat+len, 0, 21);
	len += 21;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 У����
	len+=2;
	return len;
}
#endif
void restore_dis_GPS(void)
{
	char buf[50];
#ifndef _GPS_TRA_REC_
	unsigned int i;
	
	memset(buf, 0, 20);
	if(RunStation.ucDirect <=1){
		memset(buf, ' ', 14);
		i = strlen(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
		if(i > 14)
			i = 14;
		memcpy(buf, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name,i);
		display(6, 0,buf, 0);
		
		i = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
		sprintf(buf+1, "%02X,%d,%d ",LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId,i,LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangIN);
	}
	else{
#ifdef _AREA_ENG_
		display(6, 0,"No station", 0);
#else
		display(6, 0,"��վ��", 0);
#endif
	}
	if(RunStation.ucDirect == 0)
		buf[0] = '+';
	else if(RunStation.ucDirect == 1)
		buf[0] = '-';
	else {
		buf[0] = ucAlarmFlag;
	}
	sprintf(buf+1, " LC:%d ", gDiaoDuinfo.uiALLLiCeng);
	miniDispstr(3, 8, buf, 0);
#else
	//��ʾ�¼�¼�켣�ı����ַ
	buf[0] = ucAlarmFlag;
	sprintf(buf+1, " R:%u ", ComSumeFile.BR_curE2pointer);
	miniDispstr(7, 0, buf, 0);
#endif
}

void showGps(unsigned char mode)
{				   
	char buff[32];

	unsigned int i;

	unsigned char num=0;

	if(mode & 0x01){
		switch (gGpsInfo.SataletNum)
		{
		case 0:
			num = 0;
			break;
		case 1:
		case 2:
			num = gGpsInfo.SataletNum+1;
			break;
		case 3:
		case 4:
			num = 5;
			break;
		case 5:
			num = 6;
			break;
		case 6:
			num = 7; 
			break;
		default:
			num = 8;
			break;
		}
		antDispstr(0,0,num, 2);
	}
	if(gGpsDisEN != 'G'){
		memset(buff, 0, 20);
#ifdef FengDuan_BU_

		RunStation.ucDirect=gFDderict;
		RunStation.ucStation=(gFDucID-1);
#endif		
		if(RunStation.ucDirect <=1){
			i = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
			if(mode!=1)
				i=888;
			sprintf(buff+1, "%02X,%d",LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId,i);
		}
		if(RunStation.ucDirect == 0)
			buff[0] = '+';
		else if(RunStation.ucDirect == 1)
			buff[0] = '-';
		else {
			buff[0] = ucAlarmFlag;
		}
		if(gGPsPar.EnableGps == 0){
			strcat(buff, ",N     ");
		}
		else{
			strcat(buff, ",Y     ");
		}
//		sprintf(buff+4, "LC:%d  ", gDiaoDuinfo.uiALLLiCeng);
		miniDispstr(6, 0, buff, 0);
	}
	else{
		restore_dis_GPS();
	}
}

//GPS�жϴ������
void GPS_handler(unsigned char value)
{
//�ж�Ҫ�����˳������ݵĴ������ж���ִ��(�����Ӱ��I2C����)
//	com_snd(COM4, 1, (unsigned char*)&value);//test
//ֻ�ɵ㣬����ת���ɵ���Ϣ
#ifdef _GPS_CAIDIAN_
	com_snd(COM1, 1, (unsigned char*)&value);
	return;
#elif defined _GPS_TEST_
	return;
#else
//	com_snd(COM1, 1, (unsigned char*)&value);
	if((gucGPSindex==0)&&(value!='$')){					//���ݴ���
		return;
	}
	gps_RcvBuf[gucGPSindex]=value;
	if(gucGPSindex == 4){
		switch(gps_RcvBuf[gucGPSindex]){
		case 'G':
			gGStyle = 'G';
			break;
		case 'L':
			gGStyle = 'L';
			break;
		case 'T':
			gGStyle = 'T';
			break;
		case 'D':
			gGStyle = 'D';
			break;
		case 'M':
			gGStyle = 'M';
			break;
		default:
			gGStyle = 0;
			break;
		}
	}
// 	if(gucGPSindex == 6){
// 		if(memcmp(gps_RcvBuf, "$GPGGA",6)==0){
// 			gGStyle = 'G';
// 		}
// 		else if(memcmp(gps_RcvBuf, "$GPGLL",6)==0){
// 			gGStyle = 'L';
// 		}
// 		else if(memcmp(gps_RcvBuf, "$GPVTG",6)==0){
// 			gGStyle = 'T';		
// 		}
// 		else if(memcmp(gps_RcvBuf, "$GPZDA",6)==0){
// 			gGStyle = 'D';
// 		}
// 		else if(memcmp(gps_RcvBuf, "$GPRMC",6)==0){
// 			gGStyle = 'M';
// 		}
// 	}
	if(gGStyle == 'G'){
		gGpsCmdBuf.GGA[gucGPSindex] = value;
	}
	else if(gGStyle == 'L'){
		gGpsCmdBuf.GLL[gucGPSindex] = value;
	}
	else if(gGStyle == 'T'){
		gGpsCmdBuf.VTG[gucGPSindex] = value;		
	}
	else if(gGStyle == 'D'){
		gGpsCmdBuf.ZDA[gucGPSindex] = value;
	}
	else if(gGStyle == 'M'){
		gGpsCmdBuf.RMC[gucGPSindex] = value;
	}
	if((value==0x0a)&&(gps_RcvBuf[gucGPSindex-1]==0x0d)){
		if(gGStyle == 'G'){
			ucAlarmFlag = 'A';
//			gllstyle++;
			gGpscand |= GGA_FFLAG;	//���ǵ�һ����Ϣ
		}
		else if(gGStyle == 'L'){
			ucAlarmFlag = 'G';
//			gllstyle = 0;
			gGpscand |= GLL_FFLAG;
		}
		else if(gGStyle == 'T'){
			gGpscand |= VTG_FFLAG;
		}
		else if(gGStyle == 'D'){
			gGpscand |= ZDA_FFLAG;
		}
		else if(gGStyle == 'M'){
			gGpscand |= RMC_FFLAG;
		}
		else{;
		}

		gucGPSindex = 0;
		gGStyle = 0;
	}
	else{
		gucGPSindex++;
		if(gucGPSindex > GPS_RCV_BUFFER_SIZE)
			gucGPSindex = 0;
	}
#endif
}
//��γ�ȸ�ʽ���ȶȷַ�.�ַַַַ�
// $GPGLL,2232.95729,N,11404.74220,E,061316.00,A,D*6D
unsigned char GLLInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned int lt1=0,lt2=0;
	unsigned char i, t;

	//У��
	//�ҳ�γ��
	
	if(memcmp(vv, "$GPGLL", 6)!=0)
		return 0;
#ifdef _debug_gps_
	debugstring("deal GLL");
	debugstring((char*)gGpsCmdBuf.GLL);
#endif
	p1=(char*)vv;
//	debugstring(p1);
	p2=strchr(p1, ',');
	if(p2==0)
		return 0;
	p2++;
	if((*p2)==',')	return 0;			//û�о�γ����Ϣ
	lt1=(*p2)-'0'; p2++;
	while (*p2!='.') {
		lt1=lt1*10+(*p2)-'0';
		p2++;
	}
	p2++;
	lt2=(*p2)-'0'; p2++;
	i = 0;
	while(*p2!=','){
		lt2=lt2*10+(*p2)-'0';
		p2++;
		i++;
		if(i==4) break;	
	}
	if(i<4){
		for(t=i;t<4;t++)					//��֤С��λ��5λ��Ч����
			lt2 *= 10;
	}
	if(lt2>=100000) return 0;
	gGpsInfo.Latitude = lt1*100000+lt2;
	p2++;
//	if((*p2)=='S')
//		pGpsInfo->Latitude = pGpsInfo->Latitude;						//��γ	���λ��1

	p2+=2;
	p1=p2;
	if((*p2)==',')	return 0;			//û�о�γ����Ϣ
	lt1=(*p2)-'0'; p2++;
	while (*p2!='.') {
		lt1=lt1*10+(*p2)-'0';
		p2++;
	}
	p2++;
	lt2=(*p2)-'0'; p2++;
	i = 0;
	while(*p2!=','){
		lt2=lt2*10+(*p2)-'0';
		p2++;
		i++;
		if(i==4) break;
	}
	if(i<4){
		for(t=i;t<4;t++)
			lt2 *= 10;
	}
	if(lt2>=100000) return 0;
	gGpsInfo.Longitude = lt1*100000+lt2;
	p2++;
//	if((*p2)=='E')
//		pGpsInfo->Longitude = pGpsInfo->Longitude;		//����	���λ��1

	p1=p2+2;											//ָ��UTC
	p2=strchr(p1, ',');
	if(p2==0)			return 0;
	p2++;										
//	if((*p2)!='V'){										//��ǰ������Ч
		gGpsInfo.ucValid = *p2;
//	}
//	else{
//		gGpsInfo.ucValid = GPSINVALID;
//		gGpsInfo.ucState = GPSINVALID;
//	}
	p2++;
	p2++;
	if(gGpsInfo.ucValid == GPSVALID)
		gGpsInfo.ucState = 1;//*p2;
	else
		gGpsInfo.ucState = 0;  

#ifdef _debug_gps_
	debugstring("γ��:");
	debugdata((unsigned char*)&gGpsInfo.Latitude,4,1);
	debugstring("����:");
	debugdata((unsigned char*)&gGpsInfo.Longitude,4,1);
	debugdata((unsigned char*)&gGpsInfo.ucValid,1,1);
	debugdata((unsigned char*)&p2,1,1);
#endif
	return 1;
}

// $GPGGA,061316.00,2232.95729,N,11404.74220,E,2,08,3.38,21.8,M,-2.2,M,,*78
unsigned char GGAInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned char i,t;
	unsigned int lt1, lt2;

	//У��

	if(memcmp(vv, "$GPGGA", 6)!=0)
		return 0;	
#ifdef _debug_gps_
	debugstring("deal GGA");
	debugstring((char*)gGpsCmdBuf.GGA);
#endif
	gGpsInfo.ucValid = GPSINVALID;

	p1=(char*)vv;
//	debugstring(p1);
	p2=strchr(p1, ',');
	if(p2 == 0)
		return 0;
	p1=p2+1;	//ʱ��
	
	p2=strchr(p1, ',');
	p1=p2+1;	//γ��

	p2++;
	if((*p2)==',')	return 0;			//û��γ����Ϣ
	lt1=(*p2)-'0'; p2++;
	while (*p2!='.') {
		lt1=lt1*10+(*p2)-'0';
		p2++;
	}
	p2++;
	lt2=(*p2)-'0'; p2++;
	i = 0;
	while(*p2!=','){
		lt2=lt2*10+(*p2)-'0';
		p2++;
		i++;
		if(i==4) break;	//����ѭ��ǰ������5λ
	}
	if(i<4){
		for(t=i;t<4;t++)					//��֤С��λ��5λ��Ч����
			lt2 *= 10;
	}
	if(lt2>=100000) return 0;
	gGpsInfo.Latitude = lt1*100000+lt2;
	p2++;//ָ��N

	p2+=2;
	p1=p2;
	if((*p2)==',')	return 0;			//û�о�����Ϣ
	lt1=(*p2)-'0'; p2++;
	while (*p2!='.') {
		lt1=lt1*10+(*p2)-'0';
		p2++;
	}
	p2++;
	lt2=(*p2)-'0'; p2++;
	i = 0;
	while(*p2!=','){
		lt2=lt2*10+(*p2)-'0';
		p2++;
		i++;
		if(i==4) break;
	}
	if(i<4){
		for(t=i;t<4;t++)
			lt2 *= 10;
	}
	if(lt2>=100000) return 0;
	gGpsInfo.Longitude = lt1*100000+lt2;
	p2++;//ָ��E


	p2++;//,
	p2++;//��������
//	debugstring(p2);
	if((*p2)=='0'){						//û��GPS�ź� 0û�ж�λ��1=ʵʱ��2=���
#ifdef _GPS_TEST_
		return 1;
#else
		gGpsInfo.ucValid = GPSINVALID;
		gGpsInfo.ucState = 0;
		gpsTimeOK = 0;
#endif
//		return 0;
	}
	else{
		gGpsInfo.ucValid = GPSVALID;
		gGpsInfo.ucState = 1;//*p2;
	}
 
	p2+=2;
	t=0;
	while ((*p2)!=',') {
		if(((*p2)<'0') || ((*p2)>'9'))
			break;
		t=t*10+((*p2)-'0');
		p2++;
	}
	gGpsInfo.SataletNum=t;				//��������
	p2++;//ˮƽ����

	p1=p2;
	p2=strchr(p1, ',');
	p2++;
	p1=p2;	//���߸߶ȣ���ƽ��
	
	if(gGpsDisEN == 'G'){
		if(gGpsInfo.ucValid == GPSVALID){
			miniDispstr(4, 20, "V", 0);
		}
		else{
			miniDispstr(4, 20, "N", 0);
		}
	}

#ifdef _debug_gps_
	debugstring("����������");
	debugdata((unsigned char*)&gGpsInfo.SataletNum,1,1);
	debugdata((unsigned char*)&gGpsInfo.Latitude, 4, 1);
	debugdata((unsigned char*)&gGpsInfo.Longitude, 4, 1);
#endif
//	Debughex(gGpsInfo.SataletNum);
	return 1;
}
// $GPVTG,,T,,M,0.079,N,0.146,K,D*2B
unsigned char VTGInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned char i;
	unsigned int t,t1;
	//У��
	
	if(memcmp(vv, "$GPVTG", 6)!=0)
		return 0;
#ifdef _debug_gps_
	debugstring("deal VTG");
	debugstring((char*)gGpsCmdBuf.VTG);
#endif
	p1=(char*)vv;
//	debugstring(p1);
	for(i=0;i<7;i++){
		p2=strchr(p1, ',');
		if(p2 == 0)
			return 0;
		p1=p2+1;
	}
	p2++;
//	debugstring(p2);
	if((*p2)==',') return 0;
	t=0;
	while ((*p2)!='.') {
		t=t*10+((*p2)-'0');
		p2++;
	}
	p2++;
	t1=0;
	while ((*p2)!=',') {
		t1=t1*10+((*p2)-'0');
		p2++;
	}
	if(t1>=1000) t1 /= 10;
	t1 += t*1000;
	t1 *= 1852; // 1�� = 1.852ǧ��
	t1 /= 1000;//����(m/Сʱ)
	t1 /= 10;// ����/Сʱ ���� 10m/Сʱ  ƽ̨���������ǹ���*100����λ����10m/h
	gGpsInfo.velocity = t1;				//�ٶ�
#ifdef _debug_gps_
	debugstring("�ٶȣ�");
	debugdata((unsigned char*)&gGpsInfo.velocity,4,1);
	debugstring("-----------------\r\n");
#endif
	return 1;
}

unsigned char ZDAInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned char i;
	char buf[30];

	//У��
	
	if(memcmp(vv, "$GPZDA", 6)!=0)
		return 0;
#ifdef _debug_gps_
	debugstring("deal ZDA");
	debugstring((char*)gGpsCmdBuf.ZDA);
#endif
	p1=(char*)vv;
//	debugstring(p1);
	p2=strchr(p1, ',');
	if(p2 == 0)
		return 0;
	p2++;
	if(*p2==',') return 0;
	i = (*p2)<<4; p2++;
	i = i + ((*p2) &0x0f); p2++;
	gGpsInfo.UTCtime.hour = i;	//UTC
	i = (*p2)<<4; p2++;
	gGpsInfo.UTCtime.min = i+ ((*p2)&0x0f); p2++; 
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.sec = i + (*(p2)&0x0f); p2++;
	
	p1=p2;
	p2=strchr(p1, ',');
	p2++;
	if(*p2==',') return 0;
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.day = i+((*p2)&0x0f); p2++;		//Date and year
	p2++;
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.month = i+((*p2)&0x0f); p2++;
	p2++;
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.centry = i+((*p2)&0x0f); p2++;
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.year = i+((*p2)&0x0f); p2++;
//#ifdef BUS_HNHQ_EN
	TimeAdd((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&gGpsInfo.UTCtime,10800);//��������ʱ�䣬 ���������
//#else
//	TimeAdd((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&gGpsInfo.UTCtime, 28800);					
//#endif
	//TimeAdd((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&gGpsInfo.UTCtime, 28800);
	
	if(gGpsDisEN == 'G'){
		sprintf(buf, "   %02x-%02x-%02x %02x:%02x:%02x", gGpsInfo.UTCtime.year, gGpsInfo.UTCtime.month, 
			gGpsInfo.UTCtime.day, gGpsInfo.UTCtime.hour, gGpsInfo.UTCtime.min, gGpsInfo.UTCtime.sec);
		miniDispstr(2,0,buf, 0);
	}
	
#ifdef _debug_gps_
	debugstring("GPSʱ�䣺");
	debugdata((unsigned char*)&gGpsInfo.UTCtime.centry,sizeof(TimeGPS),1);
#endif
	return 1;
}
//1�ڣ�1���Сʱ��1���1.852���1��Ҳ����1.852����/Сʱ��
//PRMC,082118.000,A,2233.7106,N,11409.4819,E,0.00,39.50,040711,,,D*5C
//$GPRMC,061316.00,A,2232.95729,N,11404.74220,E,0.079,,231008,,,D*72
unsigned char RMCInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned char i;
	unsigned int t,t1;
	//У��
	
	if(memcmp(vv, "$GPRMC", 6)!=0)
		return 0;
#ifdef _debug_gps_
	debugstring("deal RMC");
	debugstring((char*)gGpsCmdBuf.RMC);
#endif
	p1=(char*)vv;
	//	debugstring(p1);
	p2=strchr(p1, ',');
	if(p2 == 0)
		return 0;
	p2++;
	if(*p2==',') return 0;
	i = (*p2)<<4; p2++;
	i = i + ((*p2) &0x0f); p2++;
	gGpsInfo.UTCtime.hour = i;	//UTC
	i = (*p2)<<4; p2++;
	gGpsInfo.UTCtime.min = i+ ((*p2)&0x0f); p2++; 
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.sec = i + (*(p2)&0x0f); p2++;
	
	for(i=0;i<6;i++){//��Ҫָ���������(000.0~999.9�ڣ�ǰ���0Ҳ��������)
		p1=p2;
		p2=strchr(p1, ',');
		p2++;	// A
	}
	
	if(*p2==','){
		p2++;//û������
	}
	else{
		//����
		t=0;
		while ((*p2)!='.') {
			t=t*10+((*p2)-'0');
			p2++;
		}
		p2++;
		t1=0;
		while ((*p2)!=',') {
			t1=t1*10+((*p2)-'0');
			p2++;
		}
		if(t1>=1000) t1 /= 10;
		t1 += t*1000;
		t1 *= 1852; // 1�� = 1.852ǧ��
		t1 /= 1000;//����(m/Сʱ)
		t1 /= 10;// ����/Сʱ ���� 10m/Сʱ  ƽ̨���������ǹ���*100����λ����10m/h
		gGpsInfo.velocity = t1;//����(m/Сʱ)
	}
	p2++;

	if(*p2==','){
#ifdef _GPS_TEST_
		return 1;
#else
		p2++;//û�к���
#endif
	}
	else{
		//����
		t=0;
		while ((*p2)!='.') {
			t=t*10+((*p2)-'0');
			p2++;
		}
		p2++;
		t1=0;
		while ((*p2)!=',') {
			t1=t1*10+((*p2)-'0');
			p2++;
		}
		if(t1>=100) t1 %= 100;
		t1 += t*100;
		gGpsInfo.uiCog = t1;//����
	}
	p2++;

	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.day = i+((*p2)&0x0f); p2++;		//Date and year
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.month = i+((*p2)&0x0f); p2++;
	i = (*p2) << 4; p2++;
	gGpsInfo.UTCtime.year = i+((*p2)&0x0f); p2++;
	gGpsInfo.UTCtime.centry = 0x20;
#ifdef BUS_HNHQ_EN
	TimeAdd((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&gGpsInfo.UTCtime, 10800);
#else
TimeAdd((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&gGpsInfo.UTCtime, 28800);
#endif
	
#ifdef _debug_gps_
	debugstring("GPSʱ�䣺");
	debugdata((unsigned char*)&gGpsInfo.UTCtime.centry,sizeof(TimeGPS),1);
	debugstring("�ٶȣ�");
	debugdata((unsigned char*)&gGpsInfo.velocity,4,1);
	debugstring("-----------------\r\n");
#endif
	return 1;
}

//GPSģ�鲻���
void closeGPS(void)
{
	char strbuf[80];
	unsigned char i,j;
	
	strcpy(strbuf, "$PJRC314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*");
	i=0;
	for(j=1;j<(strlen(strbuf)-1);j++)
		i ^= strbuf[j];
	
	BCD2Ascii((unsigned char*)&i, (unsigned char*)strbuf+65, 1);
	strbuf[67] = 0;
	strcat(strbuf, strbuf+65);
	strcat(strbuf, "\r\n");
	
//	debugstring(strbuf);
	com_snd(GPS_COM, strlen(strbuf), (unsigned char*)strbuf);
//	debugstring("END\r\n");
}
// void gps_config_hw(void)
// {
// #ifndef _GPS_CAIDIAN_
// 	char strbuf[64];
// 	unsigned char i,j;
// 	
// 	
// 	
// //	strcpy(strbuf, "$PJRC314,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,0*");//ÿ��
// 	strcpy(strbuf, "$PJRC314,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,3,0*");//ÿ����
// 	i=0;
// 	for(j=1;j<(strlen(strbuf)-1);j++)
// 		i ^= strbuf[j];
// 	
// // 	strcpy(strbuf, "$PJRC314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*");
// // 	i=0;
// // 	for(j=1;j<(strlen(strbuf)-1);j++)
// // 		i ^= strbuf[j];
// 
// 	BCD2Ascii((unsigned char*)&i, (unsigned char*)strbuf+65, 1);
// 	strbuf[67] = 0;
// 	strcat(strbuf, strbuf+65);
// 	strcat(strbuf, "\r\n");
// 
// 	com_snd(GPS_COM, strlen(strbuf), (unsigned char*)strbuf);
// #endif
// 	return;
// }

//���GPS��������һ�ο�������
void GPS_PAR_CLR(void)
{
	unsigned int uladd;
	
	memcpy(LineInfo[0].lineNo, "\x00\x00\x04", 3);
	memcpy(LineInfo[1].lineNo, "\x00\x00\x04", 3);
	memset((unsigned char*)&LineInfo[0], 0, 6);
	LineInfo[0].ucDirect = 0;
	memset((unsigned char*)&LineInfo[1], 0, 6);
	LineInfo[1].ucDirect = 1;
	uladd = FLASH_GPS_LINE_START;
	flasherase(ERASE64KBYTESECTOR, uladd);	//��·
	Delay(2);
	uladd += 0x10000;
	flasherase(ERASE64KBYTESECTOR, uladd);	//��·

	memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
	fewrite(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
}
//��GPS���ݵ�������
//���в��� + �ҵ���վ��״̬ + ʱ��
void save_fe_gpsdata(void)
{
	unsigned char febuf[50];

	memset(febuf, 0, 50);
	if(GetDateTime() == ST_OK){
		memcpy(febuf, (unsigned char*)&RunStation, 6);
		memcpy(febuf+6, (unsigned char*)&FindS, 24);
		memcpy(febuf+30, (unsigned char*)&SysTime, 7);
// #ifdef _debug_gps_
// 		debugstring("save fe:");
// 		debugdata(febuf, 37, 1);
// #endif
		sysfewrite(BIT_GPS_RUN, 37, febuf);
	}
}
//�������ǰ�������е�GPS�����Ƿ���Ч
//���ʱ�����С��10�룬���ж���Ч
void check_fe_gpsdata(void)
{
	unsigned char febuf[50];
//	unsigned int t1,t2;

	sysferead(BIT_GPS_RUN, 37, febuf);

#ifdef _debug_gps_
	debugstring("chek fe gps data:");
	debugdata(febuf, 37, 1);
#endif
	if(time_invalid(febuf+30) == 0)//ʱ�䲻�Ϸ�
		return;
	if(GetDateTime()==ST_ERROR)
		return;
#ifdef _debug_gps_
	debugstring("sysdatetime:");
	debugdata((unsigned char*)&SysTime, 7, 1);
#endif

	if((SysTime.year_h != febuf[30])||(SysTime.year_l != febuf[31])||
		(SysTime.month != febuf[32])||(SysTime.day != febuf[33]))
		return;

	memcpy((unsigned char*)&RunStation, febuf, 6);
	memcpy((unsigned char*)&FindS, febuf+6, 24);
#ifdef _debug_gps_
	debugdata((unsigned char*)&RunStation, 6, 1);
	debugdata((unsigned char*)&FindS, 24, 1);
#endif

}
#ifdef _GPS_DIAODU_
//ȡ����������Ϣ
void Load_DIAODUCMD_info(void)
{
//	unsigned short t1, t2;

	sysferead(BIT_GPSDiaoDuCMD_INFO, sizeof(stDiaoDuCMD), (unsigned char*)&gDiaoDuCMD);
	if(memcmp(gDiaoDuCMD.Date, (unsigned char*)&SysTime.year_h, 4)){//���ں͵�ǰ���ڲ�һ��
		memset((unsigned char*)&gDiaoDuCMD, 0, sizeof(stDiaoDuCMD));
		return;
	}

	if(gDiaoDuCMD.ucStartYorN != 2)
		gDiaoDuCMD.ucStartYorN = 1;
#ifdef _debug_gps_
	debugstring("gps DIAODUCMD_info:");
	debugdata((unsigned char*)&gDiaoDuCMD, sizeof(stDiaoDuCMD), 1);
#endif
}
//
void Save_DIAODUCMD_info(void)
{
	memcpy(gDiaoDuCMD.Date, (unsigned char*)&SysTime.year_h, 4);
	sysfewrite(BIT_GPSDiaoDuCMD_INFO, 24, (unsigned char*)&gDiaoDuCMD);
#ifdef _debug_gps_
	debugstring("save DIAODUCMD_info:");
	debugdata((unsigned char*)&gDiaoDuCMD, sizeof(stDiaoDuCMD), 1);
#endif
}

void Clr_DiaoDuCmd_info(void)
{
	if((gDiaoDuCMD.ucCMD == 0x10)||(gDiaoDuCMD.ucCMD == 0x11)||\
		(gDiaoDuCMD.ucDDstyle == 0x10)||(gDiaoDuCMD.ucDDstyle == 0x11))//�е�����Ϣ,�������
	{
		gDiaoDuCMD.ucCMD = 0;//ָ����Ϊ0������ָ����ˮ����GPS���л��õ���
		gDiaoDuCMD.ucDDstyle = 0;
#ifdef KEY_PAD
		gKeyRestorDis = 3;
#endif
	}
	memset(gDiaoDuCMD.FCTime, 0, 2);
	gDiaoDuCMD.ucStartYorN = 0;
	Save_DIAODUCMD_info();
}
#endif
//
void save_DD_YinYun_info(void)
{
	unsigned short crc;
	crc = Getcrc16((unsigned char*)&gDiaoDuinfo, sizeof(stDiaoDuinfo)-2, 0xFF, 0xFF);
	memcpy(gDiaoDuinfo.CRC16, (unsigned char*)&crc, 2);
	sysfewrite(BIT_GPSDDYY_INFO, sizeof(stDiaoDuinfo), (unsigned char*)&gDiaoDuinfo);

	MSG_LOG("SaveDiaodu:");
	BCD_LOG((unsigned char*)&gDiaoDuinfo,sizeof(stDiaoDuinfo),1);
	//beep(1,100,100);

}
//ȡ����Ӫ����Ϣ
void Load_DD_YinYun_info(void)
{
	unsigned short crc;
	unsigned char i=0;
	for(i=0;i<3;i++)
	{
		sysferead(BIT_GPSDDYY_INFO, sizeof(stDiaoDuinfo), (unsigned char*)&gDiaoDuinfo);
		crc = Getcrc16((unsigned char*)&gDiaoDuinfo, sizeof(stDiaoDuinfo)-2, 0xFF, 0xFF);
		if(memcmp((unsigned char*)&crc, gDiaoDuinfo.CRC16, 2) != 0){
			memset((unsigned char*)&gDiaoDuinfo, 0, sizeof(stDiaoDuinfo));
			gDiaoDuinfo.nearS = 0xff;
			gDiaoDuinfo.limitID = 0xff;
			save_DD_YinYun_info();
	#ifdef _GPS_Speed
			debugstring("Ӫ����ϢCRC ERROR!\r\n");
	#endif
			if(i>=3)
				return;
		}else{

			break;
		}
	}
	gDiaoDuinfo.licLON = 0;//ÿ�ζ����������һ���GPS��γ�ȣ����´Ӵ˵㿪ʼ����
	gDiaoDuinfo.licLA = 0;
	gDiaoDuinfo.licJD = 0;

	memcpy((unsigned char*)&crc, gDiaoDuinfo.limitRDNo, 2);
	if((crc > 500)||(RunStation.ucDirect >= 2)){
		gDiaoDuinfo.nearS = 0xff;
		gDiaoDuinfo.limitID = 0xff;
	}
#ifdef _GPS_Speed
	debugstring("Ӫ����Ϣ:");
	debugdata((unsigned char*)&gDiaoDuinfo, sizeof(stDiaoDuinfo), 1);
#endif
}
//��ʼ��GPS����
void gps_par_init(void)
{
	unsigned char buff[20];
#ifdef _debug_
	debugstring("gps par init---\r\n");
#endif
	LineRevFlag = 0;
	sysferead(FE_PACKSERIAL, 2, (unsigned char *)&POINT_MESSAGE.PackSerial);
	sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
	if(gGpsFeD.direct < 2){
		timeread(buff);
		if(memcmp(buff, gGpsFeD.buff, 4) != 0){
			memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
		}
	}
	else
		memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
	
	memset((unsigned char*)&gGpsInfo, 0, sizeof(GPS_INFO));
//	memset((unsigned char*)&gLcdCmd, 0x00, sizeof(stLcdCmd)); //���
	ucAlarmFlag = 'X';
	gStartFlag = 0;
//	gllstyle = 0;
	gGpscand = 0;
	gucGPSindex = 0;
#ifdef _debug_
	debugstring("gps par init1---\r\n");
#endif
	Load_Line_info();
#ifdef _debug_
		debugstring("gps par init2---\r\n");
#endif
	ClrRunGPSData();
#ifdef _debug_
	debugstring("gps par init3---\r\n");
#endif
	ClrFindData();
#ifdef _GPS_DIAODU_
#ifdef _debug_
	debugstring("gps par init4---\r\n");
#endif
	Load_DIAODUCMD_info();
#ifdef _debug_
	debugstring("gps par init5---\r\n");
#endif
#endif
	Load_DD_YinYun_info();
#ifdef _debug_gps_
	debugstring("gps par init6---\r\n");
#endif

	check_fe_gpsdata();

	gGpsDisEN = 0;
//#error "init GPS"

}
int com_rcvb(unsigned char com, unsigned char *val)
{
	int i;
	int ret;
	unsigned int vv;

	for(i=900000; i>0; i--){//1000
		ret = com_rev(com, &vv);
		if(ret == 0){
			*val = (unsigned char)(vv&0xff);
			return 0;
		}
	}
	return -1;
}

//Rstr:���ص��ַ�
unsigned char GetInSound(char *Rstr)
{
	char buff[64];
	unsigned char i;

	Rstr[0] = 0;
	if(gSoundInfo.usNum == 0){
		return 0;
	}
	if ((gSoundInfo.uiSaddr[gSoundInfo.index] >= FLASH_GPS_LENE_END)||(gSoundInfo.uiSaddr[gSoundInfo.index] < gSoundInfo.uiGaddr))
	{
#ifdef _debug_
		debugstring("gSoundInfo.uiGaddr:");
		debugdata((unsigned char*)&gSoundInfo.uiGaddr, 4, 1);
		debugdata((unsigned char*)&gSoundInfo.uiSaddr[gSoundInfo.index], 4, 1);
		debugstring(Rstr);
#endif
		gSoundInfo.index = 0;
		return 0;
	}
	flashread(gSoundInfo.uiSaddr[gSoundInfo.index], (unsigned char*)&buff, 64);
	for(i=0; i<64; i++){
		if((buff[i] == '|') || (buff[i] == 0) || (buff[i] == ' ')){
			break;
		}
		Rstr[i] = buff[i];
	}
	Rstr[i] = 0;
	gSoundInfo.index++;
	if(gSoundInfo.index >= gSoundInfo.usNum)
		gSoundInfo.index = 0;
#ifdef _debug_
	debugstring("index:");
	debugdata((unsigned char*)&gSoundInfo.index, 1, 1);
	debugdata((unsigned char*)&gSoundInfo.uiSaddr[gSoundInfo.index], 4, 1);
	debugstring(Rstr);
#endif
	if(i >= 64)
		return 0;
	else
		return i;
}
//װ����·��Ϣ
unsigned char SpeedLimitMode;//
void Load_Line_info(void)
{
	unsigned int uladdr;
	unsigned char i,t;
	unsigned char buffer[100];
	unsigned char ffbuf[100];
	unsigned char zlbuf[10];
	unsigned short ust;
	unsigned int uitemp;

	uladdr = FLASH_GPS_LINE_START;//�����ǳ��ȺͰ汾��,����վ����Ϣ��������Ϣ�������Ϣ

	memset(gBusVerInfo.busLineVer, 0, 2);
	memset((unsigned char*)&LineInfo[0], 0, 20);
	memset((unsigned char*)&LineInfo[1], 0, 20);
	memset((unsigned char*)&SpeedInfo[0], 0, 20);
	memset((unsigned char*)&SpeedInfo[1], 0, 20);
	memset((unsigned char*)&gSoundInfo, 0, (sizeof(stSoundInfo)));

	sysferead(BIT_GPSChildLine, 2, zlbuf);

	memset(ffbuf, 0xff, 100);
	while(1){
		clr_wdt();
		if(uladdr >= FLASH_GPS_LENE_END)
			break;
		flashread(uladdr, buffer, 52);
		uladdr += 52;
#ifdef _debug_
		debugstring("Line Head:");
		debugdata(buffer,52,1);
		debugdata((unsigned char*)&uladdr, 4, 1);
#endif
		if(uladdr < FLASH_GPS_LINE_START){
			MessageBox(1, "LOADLine Wrong!");
			while(1)
				clr_wdt();
		}

		if(memcmp(buffer, ffbuf, 52) == 0)//ȫ��FF�����ݲ�����
			break;
		if(buffer[0] == 0){//�ܰ汾��Ϣ ��Ϣ���� 1B��00��+������4B +���汾��2B+���߱��4B+Ԥ��41B
			if(memcmp(pFistVary.LineNo, buffer+9, 2)!=0){//��·��Ų�һ�£�ȡ��
#ifdef _debug_
				debugstring(" 0xA1 Line No. Err:");
				debugdata(pFistVary.LineNo, 2, 1);
				debugdata(buffer+9, 2, 1);
#endif
				continue;
			}
			memcpy(gBusVerInfo.busLineVer, buffer+5, 2);//�汾��
#ifdef _debug_
			debugstring(" 0xA1 Line busLineVer:");
			debugdata(gBusVerInfo.busLineVer, 2, 1);
#endif
		}
		else if(buffer[0] == 0x01){//��վ����Ϣͷ
			if(memcmp(pFistVary.LineNo, buffer+5, 2)!=0){//��·��Ų�һ�£�ȡ��
#ifdef _debug_
				debugstring("Line No. Err:");
				debugdata(buffer+5, 2, 0);
				debugstring("::");
				debugdata(pFistVary.LineNo, 2, 1);
#endif
				uladdr += buffer[11]*52;//��������վ������
				uladdr += buffer[12]*52;//��������վ������
				continue;
			}
//			if((pFistVary.LineNo[2] != 0)&&(pFistVary.LineNo[2] != buffer[10])){//����·��Ų�һ�£�ȡ��,����·Ϊ0����Ч
/*			if((memcmp(zlbuf, pFistVary.LineNo,2)!=0)&&(memcmp(zlbuf, buffer+9, 2)!=0)){
#ifdef _debug_gps_
				debugstring("Line F No. Err:");
				debugdata(zlbuf, 2, 0);
				debugdata(pFistVary.LineNo, 2, 0);
				debugdata(buffer+9, 2, 0);
#endif
				uladdr = buffer[11]*52;//��������վ������
				uladdr = buffer[12]*52;//��������վ������
				memcpy(LineInfo[0].lineNo, buffer+5, 2);//
				LineInfo[0].lineNo[2] = buffer[10];
				memcpy(LineInfo[1].lineNo, buffer+5, 2);
				LineInfo[1].lineNo[2] = buffer[10];
				
				LineInfo[0].ucStationMax = buffer[11];
				LineInfo[1].ucStationMax = buffer[12];
				
				memcpy((unsigned char*)&ust, buffer+1, 2);
				over_turn(2, (unsigned char*)&ust);
				LineInfo[0].uiVer = ust;
				LineInfo[1].uiVer = ust;

				continue;
			}*/
			memcpy(LineInfo[0].lineNo, buffer+5, 2);//
			LineInfo[0].lineNo[2] = buffer[10];
			memcpy(LineInfo[1].lineNo, buffer+5, 2);
			LineInfo[1].lineNo[2] = buffer[10];
			
			LineInfo[0].ucStationMax = buffer[11];
			LineInfo[1].ucStationMax = buffer[12];

			memcpy((unsigned char*)&ust, buffer+1, 2);
			over_turn(2, (unsigned char*)&ust);
			LineInfo[0].uiVer = ust;
			LineInfo[1].uiVer = ust;
			
			for(i=0; i<LineInfo[0].ucStationMax; i++){//����վ
				if(uladdr >= FLASH_GPS_LENE_END)
					break;
				flashread(uladdr, buffer, 52);
				uladdr += 52;
				clr_wdt();
#ifdef _debug_
				debugstring("Line+:");
				debugdata(buffer,52,1);
				debugdata(ffbuf,52,1);
#endif
				LineInfo[0].ucDirect = 0;
				//if(memcmp(buffer, ffbuf, 52) == 0)//ȫ��FF�����ݲ�����
				if(buffer[0] > 200){//UID����
					memset(gBusVerInfo.busLineVer, 0, 2);
					memset((unsigned char*)&LineInfo[0], 0, 20);
					memset((unsigned char*)&LineInfo[1], 0, 20);
					memset((unsigned char*)&SpeedInfo[0], 0, 20);
					memset((unsigned char*)&SpeedInfo[1], 0, 20);
					memset((unsigned char*)&gSoundInfo, 0, (sizeof(stSoundInfo)));
					return;
				}
				if(i>=STATION_MAX_COUNT)
					continue; //����50����Ϣ������
				LineInfo[0].StationInfo[i].ucId = buffer[0];
				memcpy((unsigned char*)&uitemp, buffer+1, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulLongitudeIN = uitemp;
				memcpy((unsigned char*)&uitemp, buffer+5, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulLatitudeIN = uitemp;
				uitemp = 0;
				memcpy((unsigned char*)&uitemp, buffer+9, 3);//over_turn(3, (unsigned char*)&uitemp);
				over_turn(3, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulHxiangIN = uitemp;
				
				memcpy((unsigned char*)&uitemp, buffer+12, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulLongitude = uitemp;
				memcpy((unsigned char*)&uitemp, buffer+16, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulLatitude = uitemp;
				uitemp = 0;
				memcpy((unsigned char*)&uitemp, buffer+20, 3);//over_turn(3, (unsigned char*)&uitemp);
				over_turn(3, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulHxiang = uitemp;
				
				memcpy((unsigned char*)&uitemp, buffer+23, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulLongitudeOUT = uitemp;
				memcpy((unsigned char*)&uitemp, buffer+27, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulLatitudeOUT = uitemp;
				uitemp = 0;
				memcpy((unsigned char*)&uitemp, buffer+31, 3);//over_turn(3, (unsigned char*)&uitemp);
				over_turn(3, (unsigned char*)&uitemp);
				LineInfo[0].StationInfo[i].ulHxiangOUT = uitemp;
				
				memset(LineInfo[0].StationInfo[i].name, 0, 23);
				for(uitemp=0;uitemp<17;uitemp++){//34-51,��17���ֽ�
					if((buffer[34+uitemp] == ' ')||(buffer[34+uitemp] == 0))
						break;
					LineInfo[0].StationInfo[i].name[uitemp] = buffer[34+uitemp];
				}
			}
			
#ifdef _debug_
			for(i=0; i<LineInfo[0].ucStationMax;i++){
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ucId, 1, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLatitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLongitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiangIN, 4, 0);
				debugstring("||");
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLatitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLongitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiang, 4, 0);
				debugstring("||");
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLatitudeOUT, 4, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulLongitudeOUT, 4, 0);
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiangOUT, 4, 0);
				debugstring(LineInfo[0].StationInfo[i].name);
				debugstring("->");
				debugdata((unsigned char*)&LineInfo[0].StationInfo[i].ulHxiang, 4, 1);
			}
#endif
			for(i=0; i<LineInfo[1].ucStationMax; i++){//����վ
				if(uladdr >= FLASH_GPS_LENE_END)
					break;
				flashread(uladdr, buffer, 52);
				uladdr += 52;
				clr_wdt();
#ifdef _debug_
				debugstring("Line-:");
				debugdata(buffer,52,1);
#endif
				LineInfo[1].ucDirect = 1;
				//if(memcmp(buffer, ffbuf, 52) == 0)//ȫ��FF�����ݲ�����
				if(buffer[0] > 200){//UID����
					memset(gBusVerInfo.busLineVer, 0, 2);
					memset((unsigned char*)&LineInfo[0], 0, 20);
					memset((unsigned char*)&LineInfo[1], 0, 20);
					memset((unsigned char*)&SpeedInfo[0], 0, 20);
					memset((unsigned char*)&SpeedInfo[1], 0, 20);
					memset((unsigned char*)&gSoundInfo, 0, (sizeof(stSoundInfo)));
					return;
				//	break;
				}
				if(i>=STATION_MAX_COUNT)
					continue; //����50����Ϣ������
				t = (LineInfo[1].ucStationMax-i-1);//�����Ƿ���ģ�����Ҫ���򱣴�
				LineInfo[1].StationInfo[t].ucId = buffer[0];
				memcpy((unsigned char*)&uitemp, buffer+1, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulLongitudeIN = uitemp;
				memcpy((unsigned char*)&uitemp, buffer+5, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulLatitudeIN = uitemp;
				uitemp = 0;
				memcpy((unsigned char*)&uitemp, buffer+9, 3);//over_turn(3, (unsigned char*)&uitemp);
				over_turn(3, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulHxiangIN = uitemp;
				
				memcpy((unsigned char*)&uitemp, buffer+12, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulLongitude = uitemp;
				memcpy((unsigned char*)&uitemp, buffer+16, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulLatitude = uitemp;
				uitemp = 0;
				memcpy((unsigned char*)&uitemp, buffer+20, 3);//over_turn(3, (unsigned char*)&uitemp);
				over_turn(3, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulHxiang = uitemp;
				
				memcpy((unsigned char*)&uitemp, buffer+23, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulLongitudeOUT = uitemp;
				memcpy((unsigned char*)&uitemp, buffer+27, 4);
				over_turn(4, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulLatitudeOUT = uitemp;
				uitemp = 0;
				memcpy((unsigned char*)&uitemp, buffer+31, 3);//over_turn(3, (unsigned char*)&uitemp);
				over_turn(3, (unsigned char*)&uitemp);
				LineInfo[1].StationInfo[t].ulHxiangOUT = uitemp;
				
				memset(LineInfo[1].StationInfo[t].name, 0, 23);
				for(uitemp=0;uitemp<17;uitemp++){//34-51,��17���ֽ�
					if((buffer[34+uitemp] == ' ')||(buffer[34+uitemp] == 0))
						break;
					LineInfo[1].StationInfo[t].name[uitemp] = buffer[34+uitemp];
				}
			}
#ifdef _debug_
			for(i=0; i<LineInfo[1].ucStationMax;i++){
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ucId, 1, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLatitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLongitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiangIN, 4, 0);
				debugstring("||");
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLatitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLongitude, 4, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiang, 4, 0);
				debugstring("||");
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLatitudeOUT, 4, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulLongitudeOUT, 4, 0);
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiangOUT, 4, 0);
				debugstring(LineInfo[1].StationInfo[i].name);
				debugstring("->");
				debugdata((unsigned char*)&LineInfo[1].StationInfo[i].ulHxiang, 4, 1);
			}
#endif
		}
		else if(buffer[0] == 0x02){//��������Ϣ
			if(memcmp(pFistVary.LineNo, buffer+5, 2)!=0){//��·��Ų�һ�£�ȡ��
#ifdef _debug_
				debugstring("SPEED L No. Err:");
				debugdata(buffer+5, 2, 0);
				debugstring("::");
				debugdata(pFistVary.LineNo, 2, 1);
#endif
				
				uladdr += buffer[11]*52;//��������վ������
				uladdr += buffer[12]*52;//��������վ������
			}
//			if(pFistVary.LineNo[2] != buffer[10]){//����·��Ų�һ�£�ȡ��
		//	if((memcmp(zlbuf, pFistVary.LineNo,2)!=0)&&(memcmp(zlbuf, buffer+9, 2)!=0)){
			if(memcmp(pFistVary.LineNo, buffer+5, 2)!=0){//��·��Ų�һ�£�ȡ��
#ifdef _debug_
				debugstring("SPEED F No. Err:");
#endif
				uladdr += buffer[11]*52;//��������վ������
				uladdr += buffer[12]*52;//��������վ������
			}

//�ж�ģʽ 
			if (memcmp(buffer+13,"\x00\x00\x00\x00",4)==0)
				SpeedLimitMode=SpeedLimit_Old;
			else
				SpeedLimitMode=SpeedLimit_New;//�µ�ģʽ
#ifdef BUS_SHAODONG_
	SpeedLimitMode=SpeedLimit_New;
#endif
			memcpy(SpeedInfo[0].lineNo, buffer+5, 2);//
			SpeedInfo[0].lineNo[2] = buffer[10];
			memcpy(SpeedInfo[1].lineNo, buffer+5, 2);
			SpeedInfo[1].lineNo[2] = buffer[10];
			
			SpeedInfo[0].ucStationMax = buffer[11];
			SpeedInfo[1].ucStationMax = buffer[12];
			
			memcpy((unsigned char*)&ust, buffer+1, 2);
			over_turn(2, (unsigned char*)&ust);
			SpeedInfo[0].uiVer = ust;
			SpeedInfo[1].uiVer = ust;
			
			for(i=0; i<SpeedInfo[0].ucStationMax; i++){//����վ����
				if(uladdr >= FLASH_GPS_LENE_END)
					break;
				flashread(uladdr, buffer, 26);
				uladdr += 26;
#ifdef _debug_
				debugstring("SPEED+:");
				debugdata(buffer,26,1);
#endif
				if(memcmp(buffer, ffbuf, 26) == 0)//ȫ��FF�����ݲ�����
					break;
				if(i>=SPEED_MAX_COUNT)
					continue; //����50����Ϣ������
				SpeedInfo[0].SpeedPInfo[i].ucId = buffer[0];
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usRDNo, buffer+1, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usRDNo);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usTIme1, buffer+3, 2);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed1, buffer+5, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed1);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usTIme2, buffer+7, 2);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed2, buffer+9, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed2);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usTIme3, buffer+11, 2);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed3, buffer+13, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed3);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulLongitude, buffer+15, 4);
				over_turn(4, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulLongitude);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulLatitude, buffer+19, 4);
				over_turn(4, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulLatitude);
				memcpy((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulHxiang, buffer+23, 3);
				over_turn(3, (unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulHxiang);
#ifdef _debug_
				debugdata((unsigned char*)&SpeedInfo[0].SpeedPInfo[i], 12, 0);
				debugstring("||");
				debugdata((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usRDNo, 2, 0);
				debugdata((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usTIme1, 2, 0);
				debugdata((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].usNSpeed1, 2, 0);
				debugstring("||");
				debugdata((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulLatitude, 4, 0);
				debugdata((unsigned char*)&SpeedInfo[0].SpeedPInfo[i].ulLongitude, 4, 1);
#endif
			}
			
			for(i=0; i<SpeedInfo[1].ucStationMax; i++){//����վ����
				if(uladdr >= FLASH_GPS_LENE_END)
					break;
				flashread(uladdr, buffer, 26);
				uladdr += 26;
#ifdef _debug_
				debugstring("SPEED-");
				debugdata(buffer,26,1);
#endif
				if(memcmp(buffer, ffbuf, 26) == 0)//ȫ��FF�����ݲ�����
					break;
				if(i>=SPEED_MAX_COUNT)
					continue; //����50����Ϣ������
				SpeedInfo[1].SpeedPInfo[i].ucId = buffer[0];
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usRDNo, buffer+1, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usRDNo);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usTIme1, buffer+3, 2);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed1, buffer+5, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed1);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usTIme2, buffer+7, 2);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed2, buffer+9, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed2);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usTIme3, buffer+11, 2);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed3, buffer+13, 2);
				over_turn(2, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed3);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulLongitude, buffer+15, 4);
				over_turn(4, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulLongitude);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulLatitude, buffer+19, 4);
				over_turn(4, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulLatitude);
				memcpy((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulHxiang, buffer+23, 3);
				over_turn(3, (unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulHxiang);
#ifdef _debug_
				debugdata((unsigned char*)&SpeedInfo[1].SpeedPInfo[i], 12, 0);
				debugstring("||");
				debugdata((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usRDNo, 2, 0);
				debugdata((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usTIme1, 2, 0);
				debugdata((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].usNSpeed1, 2, 0);
				debugstring("||");
				debugdata((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulLatitude, 4, 0);
				debugdata((unsigned char*)&SpeedInfo[1].SpeedPInfo[i].ulLongitude, 4, 1);
#endif
			}
		}
		else if(buffer[0] == 0x03){//�����Ϣ,��������Ϣ
			memset((unsigned char*)&gSoundInfo, 0, (sizeof(stSoundInfo)));
			memcpy((unsigned char*)&ust, buffer+1, 2);
			over_turn(2, (unsigned char*)&ust);
			gSoundInfo.uiVer = ust;
			gSoundInfo.usNum = buffer[7];
			gSoundInfo.uiGaddr = uladdr; //������ʼ��ַ
#ifdef _debug_
			debugdata((unsigned char*)&gSoundInfo, 12, 0);
#endif
			if(gSoundInfo.usNum == 0){ //��������
				break;//��������Ϣ
			}
			for(i=0; i<gSoundInfo.usNum; i++){
				if(uladdr >= FLASH_GPS_LENE_END)
					break;//��������Ϣ
				flashread(uladdr, buffer, 64);
#ifdef _debug_
				debugstring("Sound-");
				debugdata(buffer,64,1);
#endif
				if(memcmp(buffer, ffbuf, 64) == 0)//ȫ��FF�����ݲ�����
					break;
				gSoundInfo.uiSaddr[i] = uladdr;//��ε���ʼ��ַ
				for(uitemp=0;uitemp<64;uitemp++){//���¶ε���ʼ��ַ
					if(buffer[uitemp] == '|'){
						uladdr += uitemp;
						uladdr++;
						break;
					}
					else if(buffer[uitemp] == 0){
						uitemp = 64;//����������������
						break;
					}
				}
				if(uitemp>=64){//û�ҵ��¶���ʼ��ַ,�����ң��˳�
					gSoundInfo.usNum = i+1;
					break;
				}
			}
#ifdef _debug_
			debugdata((unsigned char*)&gSoundInfo, (sizeof(stSoundInfo)), 1);
			for(uitemp=0;uitemp<gSoundInfo.usNum;uitemp++){
				debugdata((unsigned char*)&gSoundInfo.uiSaddr[uitemp], 4, 1);
			}
#endif
			break;//��������Ϣ
		}
	}
#ifdef _debug_
	debugstring("LineInfo[0]:");
	debugdata((unsigned char*)&LineInfo[0], 14, 1);
	debugstring("LineInfo[1]:");
	debugdata((unsigned char*)&LineInfo[1], 14, 1);
#endif
	if((LineInfo[0].ucStationMax == 0) || (LineInfo[0].ucStationMax > 50) ||
		(LineInfo[1].ucStationMax == 0) || (LineInfo[1].ucStationMax > 50)){
		gGPsPar.EnableGps = 0;	//��·��Ϣ����ȷ  ��ֹGPS��Ϣ
		LineInfo[0].uiVer=0;
		LineInfo[1].uiVer=0;
		for (i=0; i<STATION_MAX_COUNT; i++)
		{
			memset((unsigned char*)&LineInfo[0], 0, sizeof(stLineInfo));
			memset((unsigned char*)&LineInfo[1], 0, sizeof(stLineInfo));
		}
	}
	else{
		gGPsPar.EnableGps = 1;	//����GPS��Ϣ����   �������ص��ļ����ܰ汾������ǰ��ġ�
		if((gBusVerInfo.busLineVer[0] == 0)&&(gBusVerInfo.busLineVer[1] == 0))//���ǰ��ûȡ���汾�ţ���������ȡ
			flashread(FLASH_GPS_LENE_VER, gBusVerInfo.busLineVer, 2);//ȡ��·�汾��
		//flashread(FLASH_GPS_LENE_VER, gBusVerInfo.busLineVer, 2);//ȡ��·�汾��
	}

	//flashread(FLASH_GPS_LENE_VER, gBusVerInfo.busLineVer, 2);//ȡ��·�汾��
#ifdef _debug_
	if(gGPsPar.EnableGps == 0)
		debugstring("GPS��ֹ ��·���ԣ�");
	else
		debugstring("GPS����!");
	debugdata(gBusVerInfo.busLineVer, 2, 1);
	debugstring("\r\n");
#endif
}

//��ʼ����ǰ����GPS��Ϣ
void ClrRunGPSData(void)
{
	RunStation.inOrOut = 3;			
	RunStation.ucDirect = 0xff;
	RunStation.ucOverSpeed = 0;
	RunStation.ucPeStation = 0xff;
	RunStation.ucStation = 0xff;
}
void ClrFindData(void)
{
	memset(FindS.ucFlag, 0, 3);
	FindS.ucStation[0] = 0xff;
	FindS.ucStation[1] = 0xff;
	FindS.ulDisxx[0] = 0;
	FindS.ulDisxx[1] = 0;
}

void UPdataFeDirect(void)
{
	gGpsFeD.direct = RunStation.ucDirect;
	gGpsFeD.flag = RunStation.ucStation;
	GetDateTime();
	memcpy(gGpsFeD.buff, (unsigned char*)&SysTime.year_h, 7);
	fewrite(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
}

void ClrGpsFED(void)
{
	memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
	fewrite(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
}

//4�ֽ�int���͡���λΪ"0.000001��"
unsigned int GpsNDeal(unsigned int valueg)
{
	unsigned int t, t1;
	
	t = valueg/10000000;//ǰ���Ƕ�
	t1 = valueg%10000000;//�����Ƿ֣���7λ
	t1 *= 100;
	t1 /= 60;//ת���ɷ֣�С��λ����
// 	for(;;){
// 		if(t1 >= 1000000)
// 			break;
// 		else
// 			t1 *= 10;
// 	}
	t = t*10000000 + t1;

	t /= 10; //��ֻ̨��6λС��λ������С10��
	return t;
}

extern void CountPoint(unsigned char mode);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
//��¼���� ������E7��
//����̼�¼����¼����E0��//�����
//�˴μ�¼(��¼����E1)
//���ټ�¼(��¼����E2)
//��վ��վ��¼(��¼����E3)
//��վ��վ��¼(��¼����E4)
//��վ��վ��¼(��¼����E5)
//��վ��վ��¼(��¼����E6)
// 30���ӵ���̼�¼����¼����E7��
//˾������վ���¼����¼����E8��

//�м��վ		����¼����EA��
//�м��վ		����¼����EB��
//Խվ��¼		����¼����EC��
void WriteGPSRec(unsigned char mode)
{
#ifdef _GPS_NEED_REC_
//	unsigned int temp;
//	unsigned char buff[100];
	//unsigned char disbuf[20];
	RECORD_GPS gpsRec;
	MSG_LOG("WriteGPSRec:");
	BCD_LOG((unsigned char*)&mode,1,1);
	memset((unsigned char*)&gpsRec, 0, sizeof(RECORD_GPS));
	GetDateTime();
	Load_DD_YinYun_info();
	Ascii2BCD(pFistVary.DeviceNo, gpsRec.rDeviceNO, 8);
	Get_SerialNum(gpsRec.rDeviceSl);
	sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
	if(((RunStation.ucDirect < 2)&&(RunStation.ucStation > 0))||\
		((gGpsFeD.direct < 2)&&(gGpsFeD.flag > 0)))
		gpsRec.rinRun = 1;
	else
		gpsRec.rinRun = 0;
	gpsRec.rDealType = mode;

	memset(gpsRec.ZULine, 0, 4);
	memcpy(gpsRec.ZULine+2, pFistVary.LineNo, 2);
	memset(gpsRec.ZiLine, 0, 4);
	sysferead(BIT_GPSChildLine, 2, gpsRec.ZiLine+2);
	memcpy(gpsRec.ulCardNo, DriveCardNo, 4);
	
	if(RunStation.ucDirect >= 2){
		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
		if(gGpsFeD.direct<2){
			gpsRec.stationNo = ((LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId>>4)&0x0F)*10 + (LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId&0x0F); //վ����
			gpsRec.RunDirect = gGpsFeD.direct+1;//վ�㷽��
		}
		else{
			gpsRec.stationNo = 0; //վ����
			gpsRec.RunDirect = 0;//վ�㷽��
		}
	}
	else{
		gpsRec.stationNo = ((LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId>>4)&0x0F)*10+(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId&0x0F);//վ����
		gpsRec.RunDirect = RunStation.ucDirect+1;//վ�㷽��
		if(mode == 0xE8&&distance[0]<500)//��վ��¼ С��500�� ����һ��վ
			gpsRec.stationNo=1;
		if(mode == 0xE8&&distance[1]<500)//��վ��¼ ȡ���վ��
			gpsRec.stationNo=LineInfo[0].ucStationMax;
	}
	//	gpsRec.stationNo++;  //վ���Ŵ�1��ʼ

	memcpy(gpsRec.LuduanNo, gDiaoDuinfo.overSpeedRDNO, 2);
	over_turn(2, (unsigned char*)&gpsRec.LuduanNo);
	gpsRec.MTimesUP = gDiaoDuinfo.RenCIUp;
	gpsRec.MTimesDown = gDiaoDuinfo.RenCiDown;
	if(mode==0xE7)//30�������
		memcpy(gpsRec.LiCeng, (unsigned char*)&gDiaoDuinfo.uiHallfLiCeng, 4);
	else
		memcpy(gpsRec.LiCeng, (unsigned char*)&gDiaoDuinfo.uiALLLiCeng, 4);

	memcpy(gpsRec.STARTDealTime, gDiaoDuinfo.overSpeedStart, 7);

	memcpy(gpsRec.DealTime, (unsigned char*)&SysTime.year_h, 7);
	POINT_MESSAGE.ControlDealPointer++;
	if(POINT_MESSAGE.ControlDealPointer > 0x3B9AC9FF)
		POINT_MESSAGE.ControlDealPointer = 0;
	memcpy(gpsRec.rCardDealNumb1, (unsigned char*)&POINT_MESSAGE.ControlDealPointer, 4);
	CountPoint(2);

	//��γ��
	memcpy(gpsRec.longitude, (unsigned char*)&gGpsInfo.Longitude, 4);
//	memcpy(gpsRec.latitude, (unsigned char*)&gGpsInfo.Latitude, 4);
	memcpy(gpsRec.BussNO,CLOUD_BUSSINESSNO,2);//
	//���ٿ�ʼվ��
	gpsRec.OverStartStation=gDiaoDuinfo.StarStation;
	if(mode==ID_REC_GPS_YUE)
		gpsRec.MaxSpeed=gDiaoDuinfo.MaxYueSpeed;//Խվ����ٶ�
	else
		gpsRec.MaxSpeed=gDiaoDuinfo.MaxSpeed;		//����ٶ�
//	gpsRec.MaxSpeed=gDiaoDuinfo.MaxSpeed;			//����ٶ�
	gpsRec.LimitSpeed=gDiaoDuinfo.LimitSpeed;		//�����ٶ�
	gpsRec.OverSpeedTime=gDiaoDuinfo.limitcounter;	//����ʱ��
#ifdef _debug_gps_
	debugstring("GPS REC write:");
	debugdata((unsigned char*)&mode, 1, 1);
	debugdata((unsigned char*)&gpsRec, sizeof(RECORD_GPS), 1);
#endif
	WriteRecord((unsigned char*)&gpsRec, 0);				  
//���³�ʼ��һ��
	if(mode == ID_REC_GPS){//������̼�¼���Ѵ���������̼�¼���
		gDiaoDuinfo.licCounter = 0;
		memcpy(gDiaoDuinfo.licengTime, (unsigned char*)&SysTime.year_h, 7);//
		gDiaoDuinfo.uiALLLiCeng = 0;
		gDiaoDuinfo.licLON = gGpsInfo.Longitude;
		gDiaoDuinfo.licLA = gGpsInfo.Latitude;
		gDiaoDuinfo.licJD = gGpsInfo.uiCog;
		save_DD_YinYun_info();
	}else if (mode == ID_REC_GPS_30)
	{
		memcpy(gDiaoDuinfo.licengTime, (unsigned char*)&SysTime.year_h, 7);
		gDiaoDuinfo.uiHallfLiCeng = 0;
		gDiaoDuinfo.licLON = gGpsInfo.Longitude;
		gDiaoDuinfo.licLA = gGpsInfo.Latitude;
		gDiaoDuinfo.licJD = gGpsInfo.uiCog;
		save_DD_YinYun_info();
	}

#endif
}
void deal_out_station(void)
{
#ifdef _debug_gps_
	debugstring("��վ 2: ");
	debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
#ifdef KEY_PAD
	gKeyRestorDis = 3;
#endif
#ifdef _GPS_DIAODU_
	Clr_DiaoDuCmd_info();
#endif
	if(RunStation.ucStation == 0){
		if(RunStation.ucDirect == 0){//��վ��վ
			WriteGPSRec(0xE4);
		}
		else{
			WriteGPSRec(0xE6);//��վ��վ
		}
		SoundManage(IDSoundStartST, 0, RunStation.ucDirect);
	}
	else if((1+RunStation.ucStation) >= LineInfo[RunStation.ucDirect].ucStationMax){
#ifdef _debug_gps_
		debugstring("�յ�վ��û�г�վ\r\n");
#endif
		return;
	}
	else{
		SoundManage(IDSoundOUT, RunStation.ucStation, RunStation.ucDirect);
	}
	
	memset(FindS.ucFlag, 0, 3);
	RunStation.inOrOut = 2;
	RunStation.ucFlag = 0;
	RunStation.ucPeStation = RunStation.ucStation;
	RunStation.ucStation++;
	FindS.ulDisxx[RunStation.ucDirect] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);

}

void deal_in_station(void)
{
#ifdef _debug_gps_
	debugstring("��վ 1: ");
	debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
	debugdata((unsigned char*)&LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].Sstyle, 1, 1);
#endif
	
#ifdef KEY_PAD
	gKeyRestorDis = 3;
#endif
	if((1+RunStation.ucStation) >= LineInfo[RunStation.ucDirect].ucStationMax){//���յ�վ
		WriteGPSRec(0xE0);
		SoundManage(IDSoundEndST, RunStation.ucStation, RunStation.ucDirect);
#ifdef _GPS_Station_ExKey_enable_
		RunStation.inOrOut = 1;
		RunStation.ucFlag |= RS_IN_SOUND;
		RunStation.ucOverSpeed = 0;
		memset(FindS.ucFlag, 0, 3);
		gStartFlag = 0;
#else

		if(RunStation.ucDirect == 0){
			WriteGPSRec(0xE5);//���򣬸�վ��վ
			RunStation.ucDirect = 1;
		}
		else{
			WriteGPSRec(0xE3);//������վ��վ
			RunStation.ucDirect = 0;
		}
		RunStation.ucFlag |= RS_IN_SEND;
		RunStation.inOrOut = 3;
		RunStation.ucStation = 0;
		RunStation.ucFlag = RS_IN_SEND;
		RunStation.ucFlag |= RS_IN_SOUND;
		memset(FindS.ucFlag, 0, 3);
		FindS.ulDisxx[RunStation.ucDirect] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);	
#endif //#ifdef _GPS_Station_ExKey_enable_

#ifdef _debug_gps_
		debugstring("�����յ�վ 2������Ϊʼ��վ��");
		debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect], 4, 1);
#endif
#ifdef _GPS_NEED_REC_ //ͨ��@zzc
		gDiaoDuinfo.limitID = 0xff; //���������ٵ�
		gDiaoDuinfo.nearS=0xFF;
		memcpy(gDiaoDuinfo.limitRDNo, (unsigned char*)&SpeedInfo[RunStation.ucDirect].SpeedPInfo[0].usRDNo, 2);
		save_DD_YinYun_info();
#endif
	}
	else if(RunStation.ucStation == 0){//��ʼվû�н�վ
#ifdef _debug_gps_
		debugstring("��ʼվû�н�վ\r\n");
#endif
		RunStation.ucFlag |= RS_IN_SOUND;
		return;
	}
	else{
		RunStation.inOrOut = 1;
		RunStation.ucFlag |= RS_IN_SOUND;
		RunStation.ucOverSpeed = 0;
		memset(FindS.ucFlag, 0, 3);
		gStartFlag = 0;
		SoundManage(IDSoundIN1, RunStation.ucStation, RunStation.ucDirect);
	}
}
#ifdef _GPS_NEED_REC_
extern unsigned char SoundCMD;//������վ����� һ������8������ ��
//Խվ����  ȡ50�׷�Χ����С�ٶȱȽ�
void DealYueZan(void)
{
	//
#define  OverStation	1000
#define  MaxSpeedErr	13000//�����ܴﵽ��˸��ٵĹ�����

	unsigned int ds1,ds2;
	unsigned int i;
	static unsigned int cout=0;
	static unsigned int SP1=0xFFFF;
	static unsigned int Dly=0;
	#ifdef _debug_
	char disbuf[20];
	#endif
	if(RunStation.ucDirect>2)
		return;
   
	ds2 = 0xffffffff;
	for(i=0; i<LineInfo[RunStation.ucDirect].ucStationMax; i++){// ������ľ��� 
		ds1 = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[i].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[i].ulLongitude);
		if(ds1 <= ds2){
			ds2 = ds1;
		}
	}

//	if(ds2<60)//60�׷�Χ���ٶȴ���20km/h�ٶȲ�ͣ ��ΪԽվ
	//distance�������վ���յ�վ�ľ���
	if((ds2<60)&&(distance[0]>100)&&(distance[1]>100))//60�׷�Χ���ٶȴ���20km/h�ٶȲ�ͣ ��ΪԽվ
	{

		MSG_LOG("�Ƿ�Խվ����:\r\n");
		BCD_LOG((unsigned char*)&gGpsInfo.velocity,4,1);
		BCD_LOG((unsigned char *)&SP1,4,1);
			//WriteGPSRec(GPS_YueZan);
#ifdef _debug_
		sprintf(disbuf,"%02d",SP1/100);
		if(SP1<MaxSpeedErr)
		miniDispstr(6,16,disbuf,0);
#endif
	
		if(gGpsInfo.velocity<=SP1)//ȡ��С�ٶ�
		{
			
			if(cout++%2==0)//3����ȡһ����Сֵ
				SP1=gGpsInfo.velocity;//ȡ��С

		}else{
	
			cout=0;
		}
	
	}
	else{//����վ�㷶Χ�� �����Ƿ�Խվ��
		if(SP1>OverStation&&SP1<MaxSpeedErr)//��С�ٶȴ���10km/h ��ΪԽվ��
		{
			MSG_LOG("Խվ��\r\n");
			BCD_LOG((unsigned char *)&SP1,4,1);
			gDiaoDuinfo.MaxYueSpeed=SP1;
			save_DD_YinYun_info();
			WriteGPSRec(GPS_YueZan);
			SP1=0xFFFF;
			Dly=0;
			gDiaoDuinfo.MaxYueSpeed=0;
			//SoundManage(IDSoundOverStation, 0, RunStation.ucDirect);
			SoundCMD|=SoundYueZ;//����ʶ�� �� soundCMD������߲�����Ҫ��������
	
		}else
		{
			SP1=0xFFFF;

		}
		
	}
#undef  OverStation
}
void Deal_Speed(void)
{
	unsigned int ulSPdis[2];
	unsigned int ulSPNdis[2];
	unsigned int ulSPCog[2];
	unsigned int ulSPNCog[2];
	unsigned short us1,usLIMIt;
	stNSpeedInfo_inf *p;
	char disbuf[20];
	memset(disbuf,0,20);
	sprintf(disbuf,"%02dV",gGpsInfo.velocity/100);
	miniDispstr(5,18,disbuf,0);

	memset(disbuf,0,20);
	sprintf(disbuf,"%02dL",gDiaoDuinfo.LimitSpeed/100);
	miniDispstr(4,18,disbuf,0);

	if(RunStation.ucDirect >= 2)//û�ҵ����򣬲��ṩ���ٹ���
		return;
	if(SpeedInfo[RunStation.ucDirect].ucStationMax == 0)
		return;
	if(gDiaoDuinfo.REFFF!=RunStation.ucDirect)
	{
			gDiaoDuinfo.limitID=0xFF;//����һ��  ������
			gDiaoDuinfo.nearS=0xFF;
			gDiaoDuinfo.REFFF=RunStation.ucDirect;//���淽��  �������һ��  ��Ҫ����������վ��
			save_DD_YinYun_info();
			MSG_LOG("����һ��");
	}
	if(gDiaoDuinfo.limitID >= SpeedInfo[RunStation.ucDirect].ucStationMax){//��ǰ���ٵ㲻��ȷ
		if(gDiaoDuinfo.nearS >= SpeedInfo[RunStation.ucDirect].ucStationMax){
			ulSPdis[1] = 0xffffffff;
			for (us1=0; us1<SpeedInfo[RunStation.ucDirect].ucStationMax; us1++)
			{
				ulSPCog[1] = ((gGpsInfo.uiCog+36000) - SpeedInfo[RunStation.ucDirect].SpeedPInfo[us1].ulHxiang)%36000;
				if(ulSPCog[1] > 18000)
					ulSPCog[1] = (36000 - ulSPCog[1]);

				ulSPdis[0] = GpsDistance(SpeedInfo[RunStation.ucDirect].SpeedPInfo[us1].ulLatitude, \
					SpeedInfo[RunStation.ucDirect].SpeedPInfo[us1].ulLongitude);
				if(ulSPdis[0] <= ulSPdis[1]){
					ulSPdis[1] = ulSPdis[0];//��������ľ���ͺ���
					ulSPCog[0] = ulSPCog[1];
					gDiaoDuinfo.nearS = us1;
				}
			}
#ifdef _GPS_Speed
			debugstring("�ҵ�����:");
			debugdata((unsigned char*)&gDiaoDuinfo.nearS, 1, 1);
			debugdata((unsigned char*)&SpeedInfo[RunStation.ucDirect].ucStationMax, 1, 1);
#endif
			if((ulSPdis[0] < 20) && (ulSPCog[0] < 9000)){
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS;	//�ҵ���Ҫʹ�õ����ٵ�
				save_DD_YinYun_info();
			}
		}
		else{
			ulSPCog[0] = ((gGpsInfo.uiCog+36000) - SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS].ulHxiang)%36000;
			if(ulSPCog[0] > 18000)
				ulSPCog[0] = (36000 - ulSPCog[0]);
			ulSPdis[0] = GpsDistance(SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS].ulLatitude, \
					SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS].ulLongitude);
		
			if((gDiaoDuinfo.nearS+1) >= SpeedInfo[RunStation.ucDirect].ucStationMax){
				ulSPdis[1] = 0xffffffff;
				ulSPCog[1] = 0xffffffff;
			}
			else{//����һ��
				ulSPCog[1] = ((gGpsInfo.uiCog+36000) - SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS].ulHxiang)%36000;
				if(ulSPCog[1] > 18000)//��һ��
					ulSPCog[1] = (36000 - ulSPCog[1]);
				ulSPdis[1] = GpsDistance(SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS+1].ulLatitude, \
						SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS+1].ulLongitude);
			}
			
#ifdef _GPS_Speed
			debugstring("gDiaoDuinfo.nearS:");
			debugdata((unsigned char*)&gDiaoDuinfo.nearS, 1, 1);
			debugstring("���ٵ�0 ulSPdis[0]:");
			debugdata((unsigned char*)&ulSPdis[0], 4, 1);
			debugstring("ulSPCog[0]:");
			debugdata((unsigned char*)&ulSPCog[0], 4, 1);
			debugstring("ulSPdis[1]:");
			debugdata((unsigned char*)&ulSPdis[1], 4, 1);
			debugstring("ulSPCog[1]:");
			debugdata((unsigned char*)&ulSPCog[1], 4, 1);
#endif
			if(gDiaoDuinfo.nearS < SpeedInfo[RunStation.ucDirect].ucStationMax)//û���ҵ����ٵ� ��������Ǹ�
			{
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS;
				p = &SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID];
				gDiaoDuinfo.LimitSpeed=p->usTIme1;//ȡ�����ٶ�
			}
			if((ulSPdis[0]<20)&&(ulSPCog[0] < 9000)){
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS;
#ifdef _GPS_Speed
				debugstring("�������ٵ�1:");
				debugdata((unsigned char*)&gDiaoDuinfo.limitID, 1, 1);
#endif
				save_DD_YinYun_info();
			}
			if((ulSPdis[1]<20)&&(ulSPCog[1] < 9000)){
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS+1;
#ifdef _GPS_Speed
				debugstring("�������ٵ�2:");
				debugdata((unsigned char*)&gDiaoDuinfo.limitID, 1, 1);
#endif
				save_DD_YinYun_info();
			}
		}
	} //������ȷ����
	else{//�ж��Ƿ�����һ����  
		if((1+gDiaoDuinfo.limitID) < SpeedInfo[RunStation.ucDirect].ucStationMax){
			ulSPNdis[0] = GpsDistance(SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID+1].ulLatitude, \
				SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID+1].ulLongitude);
			ulSPNCog[0] = ((gGpsInfo.uiCog+36000) - SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID+1].ulHxiang)%36000;
			if(ulSPNCog[0] > 18000)
				ulSPNCog[0] = (36000 - ulSPNCog[0]);
 		}
		else{//
			
			ulSPNdis[0] = 0xffffffff;
 		}
#ifdef _GPS_Speed
		debugstring("Deal speed 1:");
		debugdata((unsigned char*)&ulSPNdis[0], 4, 1);
#endif
		if((ulSPNdis[0] < 40)&&(ulSPNCog[0] < 9000)){//������һ�����ٵ�
			gDiaoDuinfo.limitID++;
			gDiaoDuinfo.REFFF=RunStation.ucDirect;//���淽��  �������һ��  ��Ҫ����������վ��
			save_DD_YinYun_info();
		}
		p = &SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID];
//		gGpsInfo.velocity /= 10; // ����/Сʱ ���� 10m/Сʱ  ƽ̨���������ǹ���*100����λ����10m/h �ڽ���ʱ����

		if(SpeedLimitMode==SpeedLimit_Old)
		{
			memcpy((unsigned char*)&us1, (unsigned char*)&SysTime.hours, 2);
			over_turn(2, (unsigned char*)&us1);
			
	#ifdef _GPS_Speed
			debugstring("p->usTIme1 :");
			debugdata((unsigned char*)&p->usTIme1, 2, 1);
			debugdata((unsigned char*)&p->usTIme2, 2, 1);
			debugdata((unsigned char*)&p->usTIme3, 2, 1);
			debugdata((unsigned char*)&us1, 2, 1);
	#endif
	//2016��5��6��18:15:48	@zzc
	 //�ϵ�ģʽ
			if(us1 > p->usTIme3){//ʱ��1
				usLIMIt = p->usNSpeed3;
			}
			else if(us1 > p->usTIme2){
				usLIMIt = p->usNSpeed2;
			}
			else if(us1 > p->usTIme1){
				usLIMIt = p->usNSpeed1;
			}
			else//���������ʱ��ζ�С����ȡ����ʱ��㡣
				usLIMIt = p->usNSpeed3;

		}
		else{//�µ�ģʽ
				
				usLIMIt = p->usTIme1;//ȡ�����ٶ�
				over_turn(2,(unsigned char*)&usLIMIt);
				if(usLIMIt==0||usLIMIt>20000)//����  Ĭ������60
					usLIMIt=60;
				gDiaoDuinfo.LimitSpeed=usLIMIt;//�������Ƶ��ٶ�
			}
#ifdef _GPS_Speed
		debugstring("gDiaouinfo_limitID :");
		debugdata((unsigned char*)&gDiaoDuinfo.limitID, 1, 1);
		debugdata((unsigned char*)&gGpsInfo.velocity, 4, 1);
		debugdata((unsigned char*)&p->usNSpeed1, 2, 1);
		debugdata((unsigned char*)&usLIMIt, 2, 1);
		debugdata((unsigned char*)&gDiaoDuinfo.limitcounter,1,1);
#endif

		if(gGpsInfo.velocity > usLIMIt){//10m/h
			if((gGpsInfo.velocity - usLIMIt) > 50){
				if(gDiaoDuinfo.limitcounter == 10){//����10���ʼ�϶����٣���������ʱ��¼��ʼ����ʼʱ��
					if(memcmp(gDiaoDuinfo.overSpeedStart, (unsigned char*)&SysTime, 4) != 0){
						memcpy(gDiaoDuinfo.overSpeedStart, (unsigned char*)&SysTime, 7);//��¼��ʼʱ��
						memcpy(gDiaoDuinfo.overSpeedRDNO, (unsigned char*)&p->usRDNo, 2);

						//memcpy((unsigned char*)&gDiaoDuinfo.StarStation,(unsigned char*)&p->usRDNo, 2);
							gDiaoDuinfo.StarStation=LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId;//ȡվ���� BCD
							gDiaoDuinfo.StarStation = (gDiaoDuinfo.StarStation>>4)*10+(gDiaoDuinfo.StarStation&0x0F);//ת��HEX
					
						if(gGpsInfo.velocity>gDiaoDuinfo.MaxSpeed)//ע��MaxSpeed��short���� ȡ�����ٶ�
							memcpy((unsigned char*)&gDiaoDuinfo.MaxSpeed,(unsigned char*)&gGpsInfo.velocity,2);
						if(gDiaoDuinfo.MaxSpeed>15000)//�����ܳ���150km/h
							gDiaoDuinfo.MaxSpeed=0;
					//	SoundCMD|=SoundOverSpeed;//�ѳ���
						SoundManage(IDSoundOverSpeed, 0, RunStation.ucDirect);
					}
				}
// 				else if(gDiaoDuinfo.limitcounter==4||(gDiaoDuinfo.limitcounter%8)==0){//���� ������ ����
// 					//SoundManage(IDSoundOverSpeedAT, 0, RunStation.ucDirect);
// 					SoundCMD|=SoundOverSpeedAT;
// 				}
				gDiaoDuinfo.limitcounter++;

				if(gDiaoDuinfo.limitcounter > 120){	//����һֱ���٣����¼  ���ٿ�ʼ
#ifdef _GPS_Speed
					debugstring("��¼����\r\n");
					debugdata(gDiaoDuinfo.overSpeedStart, 7, 1);
					debugdata((unsigned char*)&usLIMIt, 2, 1);
#endif
					if(gDiaoDuinfo.MaxSpeed!=0)//�ٶȲ��ԾͲ�Ҫ���ɼ�¼��
						WriteGPSRec(ID_REC_OverSpeed);//������
					
					memset(gDiaoDuinfo.overSpeedStart, 0, 7);
					gDiaoDuinfo.MaxSpeed=0;
					gDiaoDuinfo.limitcounter = 0;
					memset(gDiaoDuinfo.overSpeedRDNO, 0, 2);
					//SoundManage(IDSoundOverSpeed, 0, RunStation.ucDirect);
					SoundCMD|=SoundOverSpeed;
						save_DD_YinYun_info();
				}
				save_DD_YinYun_info();
#ifdef _GPS_Speed
				debugstring("���� :");
				debugdata(gDiaoDuinfo.overSpeedStart, 7, 1);
				debugdata((unsigned char*)&usLIMIt, 2, 1);
#endif
			}
		}
		else{//û���٣����Ƿ��г��ټ�¼    ���ٽ���
			if(memcmp(gDiaoDuinfo.overSpeedStart, (unsigned char*)&SysTime, 4) == 0){//�г��ټ�¼
#ifdef _GPS_Speed
				debugstring("��¼����\r\n");
				debugdata(gDiaoDuinfo.overSpeedStart, 7, 1);
				debugdata((unsigned char*)&usLIMIt, 2, 1);
#endif
				if(gDiaoDuinfo.MaxSpeed!=0)//�ٶȲ��� �Ͳ�Ҫ���ɼ�¼��
					WriteGPSRec(ID_REC_OverSpeed);//������
				memset(gDiaoDuinfo.overSpeedStart, 0, 7);
				gDiaoDuinfo.limitcounter = 0;
				memset(gDiaoDuinfo.overSpeedRDNO, 0, 2);
				gDiaoDuinfo.MaxSpeed=0;
					save_DD_YinYun_info();
				//SoundManage(IDSoundOverSpeed, 0, RunStation.ucDirect);
				//	SoundCMD|=SoundOverSpeed;
			}
			if(gDiaoDuinfo.limitcounter>0){
				save_DD_YinYun_info();
				gDiaoDuinfo.limitcounter = 0;
			}
		}
	}
}
#endif

#ifdef _GPS_TEST_
unsigned char GPSPCindex=0;
unsigned char GPSPCcount;
//unsigned char GPSPCibuff[100];  ʹ��irda_rxBuf
void GPS_PC_Test_hand(unsigned char value)
{
	if((GPSPCindex == 0)&&(value != 0xAA)){
		GPSPCindex = 0;
		return;
	}
	if((GPSPCindex == 1)&&(value != 0xAA)){
		GPSPCindex = 0;
		return;
	}
	irda_rxBuf[GPSPCindex] = value;
	if(GPSPCindex == 3)//���ȸ��ֽ�
		GPSPCcount = value;
	else if(GPSPCindex == 4){//���ȵ��ֽ�
		GPSPCcount *= 256;
		GPSPCcount += value;
	}
	else if(GPSPCindex >= 38){//�������
		memcpy((unsigned char*)&gGpsInfo, irda_rxBuf+6, sizeof(GPS_INFO));//����
// 		memcpy((unsigned char*)&gGpsInfo.Latitude, irda_rxBuf+10, 4);//γ��
// 		memcpy((unsigned char*)&gGpsInfo.uiCog, irda_rxBuf+14, 4);//����
// 		gGpsInfo.velocity = 2100;
// 		memcpy((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&SysTime, 7);
// 		gGpsInfo.ucValid = GPSVALID;
// 		gGpsInfo.SataletNum = 8;
		GPSPCindex = 0;
		return;
	}
	GPSPCindex++;
}
#endif

void GpsLichengClear(void){
	gDiaoDuinfo.licLON = 0;//���ʱ����Ч
	gDiaoDuinfo.licLA = 0;//��̵ľ�γ����0
	gDiaoDuinfo.licJD = 0;
	gDiaoDuinfo.uiALLLiCeng = 0;
	gDiaoDuinfo.uiHallfLiCeng=0;
	gDiaoDuinfo.licCounter = 0;
}

//�������
extern unsigned char IF_TIME_OK(void);
void GpsLiChengDeal(void)
{
	unsigned int ultemp;
	unsigned int t=0;

	if(IF_TIME_OK()==ST_ERROR)
		return;
	if(time_invalid(gDiaoDuinfo.licengTime) == 1){//ʱ����Ч
		if((memcmp(gDiaoDuinfo.licengTime, (unsigned char*)&SysTime, 4) != 0) && (gDiaoDuinfo.uiALLLiCeng>0)){//���ǵ�����������
			WriteGPSRec(GPS_LICHENG);
			WriteGPSRec(GPS_LiCheng30);
		}

		if(memcmp((unsigned char*)&SysTime,gDiaoDuinfo.licengTime,7)>0)//���ڲż�
			t=DataTimeDec((unsigned char*)&SysTime,gDiaoDuinfo.licengTime);
		MSG_LOG("���ʱ��:\r\n");
		BCD_LOG((unsigned char*)&gDiaoDuinfo,sizeof(gDiaoDuinfo),1);
		BCD_LOG((unsigned char*)&t,4,1);
		if(t>1800)//30����
			WriteGPSRec(GPS_LiCheng30);
	}
	else{
		GpsLichengClear();
	}
	
	if((gDiaoDuinfo.licLON < 73000000)||(gDiaoDuinfo.licLON > 136000000)||\
		(gDiaoDuinfo.licLA < 20000000)||(gDiaoDuinfo.licLA >  54000000)){//�������ʱ����һ��γ��
#ifdef _debug_gps_
		debugstring("��̾�γ��ERR!\r\n");
#endif
		gDiaoDuinfo.licLON = gGpsInfo.Longitude;
		gDiaoDuinfo.licLA = gGpsInfo.Latitude;
		gDiaoDuinfo.licJD = gGpsInfo.uiCog;
		gDiaoDuinfo.licCounter = 0;
		//�����ϴε���̼���  
// 		if(time_invalid((unsigned char*)&SysTime) == 1)
// 			memcpy(gDiaoDuinfo.licengTime, (unsigned char*)&SysTime, 7);
	}
	else{
		ultemp = GpsDistance(gDiaoDuinfo.licLA, gDiaoDuinfo.licLON); //ÿ��������:19���
		if(ultemp > 5){//�ƶ��������8�ײ����¼�ֵ
			gDiaoDuinfo.licLA = gGpsInfo.Latitude;
			gDiaoDuinfo.licLON = gGpsInfo.Longitude;
			gDiaoDuinfo.licJD = gGpsInfo.uiCog;
			gDiaoDuinfo.uiALLLiCeng += ultemp;

			gDiaoDuinfo.licCounter++;
			if(gDiaoDuinfo.licCounter >= 10){//ÿ�붼��һ�ξ��룬����10�ӱ���һ��.
				save_DD_YinYun_info();
				gDiaoDuinfo.licCounter = 0;
			}
		}
	}
}

void Find_StationFZ(void)
{
	
	//���������վ�ľ���
	//	unsigned char ds2,ds1;
	//unsigned char station,i;
	static unsigned short cout;

	distance[0] = GpsDistance(LineInfo[0].StationInfo[0].ulLatitude, LineInfo[0].StationInfo[0].ulLongitude);//��һ��վ��ľ���
	distance[1] = GpsDistance(LineInfo[1].StationInfo[0].ulLatitude, LineInfo[1].StationInfo[0].ulLongitude);//���һ��վ��ľ���
	
	if(RunStation.ucDirect<2)//�Ѿ�ȷ������
		return;
	//������ʼվ����
	if((distance[0]<=100)&&(RunStation.ucDirect>1))//100�׷�Χ����վ ����
	{
		cout++;
		if(cout>=5)
		{
			cout=0;
			RunStation.ucDirect=0;
			RunStation.ucStation=0;
		}
	}
	else if(distance[1]<=100&&(RunStation.ucDirect>1))
	{
		cout++;
		if(cout>=5)
		{
			cout=0;
			RunStation.ucDirect=1;
			RunStation.ucStation=0;
		}
	} 
	
}
extern void SoundAtantionCmd(void);
//����GPS����
unsigned char GpsInfoDeal(void)
{
//��վ���վ��վ���� 20
#define _ZU_IN_ 20
//��վ���վ��վ���� 15
#define _IN_IN_ 15
//��վ���վ��վ���� 50
#define _ZU_OUT_ 20
//��վ���վ��վ���� 15
#define _OUT_OUT_ 15
//�Ƿ��վ�����жϣ�����վ��û������վ�㣩 90
#define _OVERSTATION_ 90
//�ȽϽǶȵ���Ч��ֵ ����.00�ȣ��Ӵ�100��
#define COG_IS_OK 6000
	unsigned char i,flag;
	unsigned int ulDis[2];
	unsigned int ulINDis[2];
	unsigned int ulOUTDis[2];
	unsigned int ultemp, ultemp1;
	unsigned int ulCog;
	unsigned int ulINCog[2];
	unsigned int ulOUTCog[2];
	unsigned int ulXCog[2];
	unsigned int ulNxCog[2];
	unsigned int ulNextOutDis[2];//�������ҷ���վ��ʱ���Ƚ���һվ�Ľ�վ�����
	char disbuf[40];
#ifdef _debug_gps_
	debugstring("GPS INFO DEAL:\r\n");
	debugdata((unsigned char*)&gGpsInfo, sizeof(GPS_INFO), 1);
#endif
//�� ����135�� 2��30�� �������������ｭ���㴦 
//�� ����73��  40��    ���׶���ԭ���ȱ���ɽ��
//�� ��γ3��   52��    ��ɳȺ����ĸ��ɳ
//�� ��γ53��  33��    Į���Ա�������������
//���� ��γ20��

	ulDis[0] = GpsNDeal(gGpsInfo.Latitude);
	gGpsInfo.Latitude = ulDis[0];
	ulDis[0] = GpsNDeal(gGpsInfo.Longitude);
	gGpsInfo.Longitude = ulDis[0];

	if((gGpsInfo.Longitude < 73000000)||(gGpsInfo.Longitude > 136000000)||\
		(gGpsInfo.Latitude < 20000000)||(gGpsInfo.Latitude >  54000000)){
#ifdef _debug_
		debugstring("��γ����Ϣ����\r\n");
#endif
		return 0xF0;
	}
#ifdef _debug_
	debugdata((unsigned char*)&gGpsInfo, sizeof(GPS_INFO), 1);
#endif
	GpsLiChengDeal();//������̡�
	Find_StationFZ();//�������ҷ���
	if(gGPsPar.EnableGps == 0){		//GPS��Ϣ����ֹ
//		gGpsInfo.ucValid = GPSINVALID;
		return 0xF1;
	}

	if(LineInfo[0].ucStationMax > LineInfo[1].ucStationMax)
		flag = LineInfo[0].ucStationMax;
	else
		flag = LineInfo[1].ucStationMax;//ȡ����������վ����
	if((RunStation.ucDirect > 1)||(RunStation.ucStation >= flag)){//��û��ȷ�������ûȷ��վ��
//		Load_DIAODUCMD_info()  ���ڵ�����ǲ���Ҫ֪������ֻҪû�ҵ�վ�㣬������������һ���ң���Ϊ�󲿷���������ҵ�������վ��Ҳ����ȷ���ˡ�
// 		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);//ȡ�������б�������з���
// 		if((gGpsFeD.flag == 1)&&(gGpsFeD.direct < 2)){
// 			GetDateTime();
// 			if(memcmp((unsigned char*)&SysTime, gGpsFeD.buff, 5) != 0){//�Ƚϵ�Сʱһ�£���Ч
// 				memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
// 			}
// 			else{//������Ϣ��Ч
// 				RunStation.ucDirect = gGpsFeD.direct;
// 				RunStation.ucStation = 0xff;
// #ifdef _debug_gps_
// 				debugstring("������Ϣ��Ч ����: ");
// 				debugdata((unsigned char*)&RunStation.ucDirect,1,1);
// #endif
// 				return 1;
// 			}
// 		}
// 		else
// 			memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
		
		if(FindS.ucStation[0] >= LineInfo[0].ucStationMax){//---�ҵ�һ������ĵ�-------
			ulDis[1] = 0xffffffff;
			FindS.ulDisxx[0] = 0;
			FindS.ulDisxx[1] = 0;
			for(i=0; i<LineInfo[0].ucStationMax; i++){
				ulDis[0] = GpsDistance(LineInfo[0].StationInfo[i].ulLatitude, LineInfo[0].StationInfo[i].ulLongitude);
				if(ulDis[0] <= ulDis[1]){
					ulDis[1] = ulDis[0];
					FindS.ucStation[0] = i;
					FindS.ulDisxx[0] = ulDis[0];	//��������վ��ľ���
				}
			}
			if(FindS.ucStation[0]+1 >= LineInfo[0].ucStationMax)//�Ѿ����յ�վ��û����һվ
				FindS.ulDisnn[0] = 0xffffffff;
			else
				FindS.ulDisnn[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLatitude, LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLongitude);
#ifdef _debug_gps_
			debugstring("G Find 4  ");
			debugstring("�������վ��:");
			debugstring(LineInfo[0].StationInfo[FindS.ucStation[0]].name);
			debugdata((unsigned char*)&FindS.ucStation[0],1,1);
			debugdata((unsigned char*)&FindS.ulDisxx[0],4,1);
			debugdata((unsigned char*)&FindS.ulDisnn[0],4,1);
			debugdata((unsigned char*)&LineInfo[1].ucStationMax,1,1);
#endif
			ulDis[1] = 0xffffffff;
			for(i=0; i<LineInfo[1].ucStationMax; i++){//�ҷ������վ��
				ulDis[0] = GpsDistance(LineInfo[1].StationInfo[i].ulLatitude, LineInfo[1].StationInfo[i].ulLongitude);
				if(ulDis[0] <= ulDis[1]){
					ulDis[1] = ulDis[0];
					FindS.ucStation[1] = i;
					FindS.ulDisxx[1] = ulDis[0];
				}
			}
			if(FindS.ucStation[1]+1 >= LineInfo[1].ucStationMax)	//�Ѿ����յ�վ��û����һվ
				FindS.ulDisnn[1] = 0xffffffff;
			else
				FindS.ulDisnn[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulLatitude, LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulLongitude);
#ifdef _debug_gps_
			debugstring("G Find 5  ");
			debugstring("�������վ��:");
			debugstring(LineInfo[1].StationInfo[FindS.ucStation[1]].name);
			debugdata((unsigned char*)&FindS.ucStation[1],1,1);
			debugdata((unsigned char*)&FindS.ulDisxx[1],4,1);
			debugdata((unsigned char*)&FindS.ulDisnn[1],4,1);
#endif
			return 0;
		}//---�����ҵ�һ������ĵ�-------

		ulDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]].ulLatitude, LineInfo[0].StationInfo[FindS.ucStation[0]].ulLongitude);
		ulINDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]].ulLatitudeIN, LineInfo[0].StationInfo[FindS.ucStation[0]].ulLongitudeIN);
		ulOUTDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]].ulLatitudeOUT, LineInfo[0].StationInfo[FindS.ucStation[0]].ulLongitudeOUT);
		if(FindS.ucStation[0]+1 >= LineInfo[0].ucStationMax)	//�Ѿ����յ�վ��û����һվ
			ulNextOutDis[0] = 0xffffffff;
		else{
			ulNextOutDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLatitudeIN, LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLongitudeIN);
			ulNxCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulHxiangIN)%36000;
			if(ulNxCog[0] > 18000)//ȡС�Ĳ�ǣ����
				ulNxCog[0] = (36000 - ulNxCog[0]);
		}
		ulINCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]].ulHxiangIN)%36000;
		if(ulINCog[0] > 18000)//ȡС�Ĳ�ǣ����
			ulINCog[0] = (36000 - ulINCog[0]);
		
		ulOUTCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]].ulHxiangOUT)%36000;
		if(ulOUTCog[0] > 18000)//ȡС�Ĳ�ǣ����
			ulOUTCog[0] = (36000 - ulOUTCog[0]);
		
		ulXCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]].ulHxiang)%36000;
		if(ulXCog[0] > 18000)
			ulXCog[0] = (36000 - ulXCog[0]);
		
		ulDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]].ulLatitude, LineInfo[1].StationInfo[FindS.ucStation[1]].ulLongitude);
		ulINDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]].ulLatitudeIN, LineInfo[1].StationInfo[FindS.ucStation[1]].ulLongitudeIN);
		ulOUTDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]].ulLatitudeOUT, LineInfo[1].StationInfo[FindS.ucStation[1]].ulLongitudeOUT);
		if(FindS.ucStation[1]+1 >= LineInfo[1].ucStationMax)	//�Ѿ����յ�վ��û����һվ
			ulNextOutDis[1] = 0xffffffff;
		else{
			ulNextOutDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulLatitudeIN, LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulLongitudeIN);
			
			ulNxCog[1] = ((gGpsInfo.uiCog+36000) - LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulHxiangIN)%36000;
			if(ulNxCog[1] > 18000)
				ulNxCog[1] = (36000 - ulNxCog[1]);
		}
		
		ulINCog[1] = ((gGpsInfo.uiCog+36000) - LineInfo[1].StationInfo[FindS.ucStation[1]].ulHxiangIN)%36000;
		if(ulINCog[1] > 18000)
			ulINCog[1] = (36000 - ulINCog[1]);
		
		ulOUTCog[1] = ((gGpsInfo.uiCog+36000) - LineInfo[1].StationInfo[FindS.ucStation[1]].ulHxiangOUT)%36000;
		if(ulOUTCog[1] > 18000)
			ulOUTCog[1] = (36000 - ulINCog[1]);
		
		ulXCog[1] = ((gGpsInfo.uiCog+36000) - LineInfo[1].StationInfo[FindS.ucStation[1]].ulHxiang)%36000;
		if(ulXCog[1] > 18000)
			ulXCog[1] = (36000 - ulXCog[1]);

		flag = 0;
#ifdef _debug_gps_
		debugstring("���վ��������:");
		debugstring(LineInfo[0].StationInfo[FindS.ucStation[0]].name);
		debugdata((unsigned char*)&ulDis[0],4,1);
		debugdata((unsigned char*)&ulNextOutDis[0],4,1);
		debugdata((unsigned char*)&ulXCog[0],4,1);
		debugdata((unsigned char*)&ulNxCog[0],4,1);
		debugstring("���վ�����ݷ�:");
		debugstring(LineInfo[1].StationInfo[FindS.ucStation[1]].name);
		debugdata((unsigned char*)&ulDis[1],4,1);
		debugdata((unsigned char*)&ulNextOutDis[1],4,1);
		debugdata((unsigned char*)&ulXCog[1],4,1);
		debugdata((unsigned char*)&ulNxCog[1],4,1);
#endif

		if(gGpsDisEN == 'G'){
			memset(disbuf, 0, 20);
			sprintf(disbuf,"+:%d,-:%d ", ulDis[0], ulDis[1]);
			miniDispstr(1, 0, disbuf, 0);
		}

		if((FindS.ucStation[1]+1) < LineInfo[1].ucStationMax){//�������һվ���Ƚ��Ƿ��������һվ
			if((ulNxCog[1]<COG_IS_OK) && (ulNextOutDis[1]<_IN_IN_)){//������һվ�ˡ�
				RunStation.ucDirect = 1;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//���ǽ�վ
				RunStation.ucFlag = 0;
				RunStation.ucStation = FindS.ucStation[1]+1;
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("������վ XX: ");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
				deal_in_station();
				return 1;
			}
		}
		
		if((FindS.ucStation[0]+1) < LineInfo[0].ucStationMax){//�������һվ���Ƚ��Ƿ��������һվ
			if((ulNxCog[0]<COG_IS_OK) && (ulNextOutDis[0]<_IN_IN_)){//������һվ�ˡ�
				RunStation.ucDirect = 0;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//���ǽ�վ
				RunStation.ucFlag = 0;
				RunStation.ucStation = FindS.ucStation[0]+1;
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("������վ XX: ");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
				deal_in_station();
				return 1;
			}
		}
		if(ulDis[0] <= 15){//������ڱ�վ���ȽϺ�����ȷ������--����
			if(ulXCog[0]<COG_IS_OK){//ȷ���Ѿ����˱�վ������ȷ��
				RunStation.ucDirect = 0;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//���ǳ�վ�����վ��־
				RunStation.ucStation = FindS.ucStation[0];
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("GCOG�� ȷ��վ�㣺");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
				debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				deal_in_station();

				if((1+RunStation.ucStation)>=LineInfo[RunStation.ucDirect].ucStationMax){//������յ�վ������Ϊʼ��վ
					RunStation.ucDirect = 1;
					RunStation.inOrOut = 0;
					RunStation.ucOverSpeed = 0;
					RunStation.ucFlag = RS_IN_SOUND;//���ǳ�վ�����վ��־
					RunStation.ucFlag |= RS_IN_SEND;
					RunStation.ucStation = 0;
					memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
					debugstring("���յ㣬��Ϊʼ��:");
					debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
					debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				}
				UPdataFeDirect();
				return 1;
			}
		}
		if(ulDis[1] <= 15){//������ڱ�վ���ȽϺ�����ȷ������--����
			if(ulXCog[1]<COG_IS_OK){//ȷ���Ѿ����˱�վ������ȷ��
				RunStation.ucDirect = 1;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//���ǳ�վ�����վ��־
				RunStation.ucStation = FindS.ucStation[1];
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("GCOG�� ȷ��վ�㣺");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
				debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				if((1+RunStation.ucStation)>=LineInfo[RunStation.ucDirect].ucStationMax){//������յ�վ������Ϊʼ��վ
					RunStation.ucDirect = 0;
					RunStation.inOrOut = 0;
					RunStation.ucOverSpeed = 0;
					RunStation.ucFlag = RS_IN_SOUND;//���ǳ�վ�����վ��־
					RunStation.ucFlag |= RS_IN_SEND;
					RunStation.ucStation = 0;
					memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
					debugstring("���յ㣬��Ϊʼ��:");
					debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
					debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				}
				else{
					deal_in_station();
				}
				UPdataFeDirect();
				return 1;
			}
		}
#ifdef _debug_gps_
		debugstring(" �����վ����:");
		debugdata((unsigned char*)&ulOUTDis[0],4,1);
		debugdata((unsigned char*)&ulOUTDis[1],4,1);
		debugdata((unsigned char*)&ulOUTCog[0],4,1);
		debugdata((unsigned char*)&ulOUTCog[1],4,1);
		debugstring(" �����վ����:");
		debugdata((unsigned char*)&ulINDis[01],4,1);
		debugdata((unsigned char*)&ulINDis[1],4,1);
		debugdata((unsigned char*)&ulINCog[0],4,1);
		debugdata((unsigned char*)&ulINCog[1],4,1);
#endif
		if((ulOUTDis[0] < _OUT_OUT_)&&(ulOUTCog[0] < COG_IS_OK)){//�����վ
			RunStation.ucDirect = 0;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = RS_IN_SOUND;//���ǳ�վ�����վ��־
			RunStation.ucFlag |= RS_IN_SEND;
			RunStation.ucStation = FindS.ucStation[0];
			memset(FindS.ucFlag, 0, 3);
			flag = 0x55;
		}
		else if((ulOUTDis[1] < _OUT_OUT_)&&(ulOUTCog[1] < COG_IS_OK)){//�����վ
			RunStation.ucDirect = 1;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = RS_IN_SOUND;//���ǳ�վ�����վ��־
			RunStation.ucFlag |= RS_IN_SEND;
			RunStation.ucStation = FindS.ucStation[1];
			memset(FindS.ucFlag, 0, 3);
			flag = 0x55;
		}
		else if((ulINDis[0] < _OUT_OUT_)&&(ulINCog[0] < COG_IS_OK)){//�����վ
			RunStation.ucDirect = 0;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = 0;//���ǽ�վ
			RunStation.ucFlag = 0;
			RunStation.ucStation = FindS.ucStation[0];
			memset(FindS.ucFlag, 0, 3);
			flag = 0xaa;
		}
		else if((ulINDis[1] < _OUT_OUT_)&&(ulINCog[1] < COG_IS_OK)){//�����վ
			RunStation.ucDirect = 1;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = 0;//���ǽ�վ
			RunStation.ucFlag = 0;
			RunStation.ucStation = FindS.ucStation[1];
			memset(FindS.ucFlag, 0, 3);
			flag = 0xaa;
		}
		if(flag==0x55){
#ifdef _debug_gps_
			debugstring("��վ XX: ");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
			deal_out_station();
			return 1;
		}
		else if(flag == 0xaa){//�����վ��
#ifdef _debug_gps_
			debugstring("��վ XX: ");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
			deal_in_station();
			return 1;
		}

		flag = 0;
		//�����ж�,����ͱ�վ����վ��Խ��ԽԶ
		if((ulDis[0] > FindS.ulDisxx[0])&&(ulNextOutDis[0] > FindS.ulDisnn[0])){
			flag |= 0xF0;//�������
		}
		if((ulDis[0] > FindS.ulDisxx[0])&&(ulNextOutDis[0] > FindS.ulDisnn[0])){
			flag |= 0x0F;//����Ҳ����
		}
		if(flag == 0xFF){//ȫ���������¿�ʼ
#ifdef _debug_gps_
			debugstring("����ȫ�� \r\n");
#endif
			ClrRunGPSData();
			ClrFindData();
			return 0;
		}
		
		if(ulDis[0] >= 2000){
#ifdef _debug_gps_
			debugstring("�������վ̫Զ�� :");
			debugdata((unsigned char*)&ulDis[0], 4, 1);
#endif
			ClrRunGPSData();
			ClrFindData();
			return 1;
		}
		///////////////////////////�����ǵ���ĳվ��ʱ���ܹ�������ȷ��/////////////
	}//����ûȷ������
	else{//����ȷ����վ��ȷ��
//		debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);

#ifdef _GPS_NEED_REC_
		Deal_Speed();//����������
		DealYueZan();//Խվ���⴦��
#ifdef _TTSSound_enable_
		//�����Ƿ���Ҫ��
		SoundAtantionCmd();//������Ҫ��������
#endif
#endif

#ifdef _debug_gps_
		debugstring("��վ: ");
		debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
		debugdata((unsigned char*)&RunStation.ucDirect,1,1);
		debugdata((unsigned char*)&RunStation, sizeof(stRunStation), 1);
		debugdata((unsigned char*)&LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude,4,1);
		debugdata((unsigned char*)&LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude,4,1);
#endif
		ulDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
		ulINDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitudeIN, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitudeIN);
		ulOUTDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitudeOUT, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitudeOUT);
		if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangOUT != 0){//�������Ϊ0����Ϊ�Ǵ���ģ�����û�к���
			ulOUTCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangOUT)%36000;
			if(ulOUTCog[0] > 18000)//ȡС�Ĳ�ǣ����
				ulOUTCog[0] = (36000 - ulOUTCog[0]);
		}
		else
			ulOUTCog[0] = 0;
		if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangIN != 0){//�������Ϊ0����Ϊ�Ǵ���ģ�����û�к���
			ulINCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangIN)%36000;
			if(ulINCog[0] > 18000)//ȡС�Ĳ�ǣ����
				ulINCog[0] = (36000 - ulINCog[0]);
		}
		else
			ulINCog[0] = 0;

		if(RunStation.ucStation+1 >= LineInfo[RunStation.ucDirect].ucStationMax){//�����һվ
			ulNextOutDis[0] = 0xffffffff;
		}
		else{
			ulNextOutDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulLatitudeIN, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulLongitudeIN);
			if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN != 0){//�������Ϊ0����Ϊ�Ǵ���ģ�����û�к���
				ulINCog[1] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN)%36000;
				if(ulINCog[1] > 18000)//ȡС�Ĳ�ǣ����
					ulINCog[1] = (36000 - ulINCog[1]);
			}
			else
				ulINCog[1] = 0;
		}
		
		if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN != 0){//�������Ϊ0����Ϊ�Ǵ���ģ�����û�к���
			ulCog= ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN)%36000;
			if(ulCog > 18000)//ȡС�Ĳ�ǣ����
				ulCog = (36000 - ulCog);
		}
		else
			ulCog = 0;

#ifdef _debug_gps_
		debugstring("ulDis:");
		debugdata((unsigned char*)&ulDis[0],4,0);
		debugstring("ulINDis:");
		debugdata((unsigned char*)&ulINDis[0],4,0);
		debugstring("ulOUTDis:");
		debugdata((unsigned char*)&ulOUTDis[0],4,0);
		debugstring("ulDisxx:");
		debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect],4,1);
		debugdata((unsigned char*)&RunStation.ucFlag,1,1);
		debugstring("ulNextOutDis:");
		debugdata((unsigned char*)&ulNextOutDis[0],4,0);
		debugstring("ulOUTCog:");
		debugdata((unsigned char*)&ulOUTCog[0],4,0);
		debugstring("ulINCog:");
		debugdata((unsigned char*)&ulINCog[0],4,0);
		debugstring("ulCog:");
		debugdata((unsigned char*)&ulCog,4,0);
#endif
		if(gGpsDisEN == 'G'){
			memset(disbuf, 0, 20);
			sprintf(disbuf,"I:%d,:%d,O:%d ",ulINDis[0], ulDis[0], ulOUTDis[0]);
			miniDispstr(1, 0, disbuf, 0);
		}
//		if(ulDis[0] >= 3000){//����̫Զ
// 			ClrRunGPSData();
// 			ClrFindData();
// 			ClrGpsFED();
// 			if(gGpsDisEN == 'G'){
// 				display(6, 10, "̫Զ", 0);
// 			}
// 			return 1;
//		}

//		if(((ulDis[0] <= _ZU_IN_)||(ulINDis[0] <= _IN_IN_)) && (RunStation.ucFlag == 0) && ((ulINCog[0] < COG_IS_OK)||(ulCog<COG_IS_OK))){//��վ
		if(((ulDis[0] <= _ZU_IN_)||(ulINDis[0] <= _IN_IN_)) && (RunStation.ucFlag == 0)){
			deal_in_station();
			UPdataFeDirect();
			if(gGpsDisEN == 'G'){
				display(6, 10, "��վ", 0);
			}
			
			if(ulDis[0] < 20){
				RunStation.ucFlag |= RS_IN_MAIN;
			}
			goto DEAL_IN_END;  //��վ�������
		}
		
		if(ulDis[0]>FindS.ulDisxx[RunStation.ucDirect])
			ultemp = ulDis[0] - FindS.ulDisxx[RunStation.ucDirect];
		else
			ultemp = FindS.ulDisxx[RunStation.ucDirect] - ulDis[0];
		if(ultemp < 3){
#ifdef _debug_gps_
			debugstring("Runs С��10��\r\n");
#endif
			return 1;		//С��10�׺��ƶ�������
		}
		if(RunStation.ucFlag == 0){ //û��վҲû��վ �ض��Ƿ��˳�վ����
			if(ulOUTDis[0] < _OUT_OUT_){//��վ
				deal_out_station();
				UPdataFeDirect();
#ifdef _debug_gps_
				debugstring("��վonly!\r\n");
#endif
				if(gGpsDisEN == 'G'){
					display(6, 10, "��վ", 0);
				}
				goto DEAL_IN_END;  //��վ�������
			}
		}
		//��վ�жϴ���(�Ѿ����˽�վ������û����վ),��Ҫ���ж��Ƿ������վλ�ã�Ȼ�����ж��Ƿ��վ
		else if(((RunStation.ucFlag & RS_IN_SOUND) != 0) && ((RunStation.ucFlag & RS_OUT_SOUND) == 0)){
			if((RunStation.ucFlag & RS_IN_MAIN) != 0){//�Ѿ���������վ
				if(((ulDis[0] > _ZU_OUT_) && (ulDis[0] < 120))||(ulOUTDis[0] < _OUT_OUT_)){//��վ
					deal_out_station();
					UPdataFeDirect();
#ifdef _debug_gps_
					debugstring("��վmain\r\n");
#endif
					if(gGpsDisEN == 'G'){
						display(6, 10, "��վ", 0);
					}
					goto DEAL_IN_END;  //��վ�������
				}
			}
			else{//��û�о�����վ������վ��
				if(ulDis[0] < 20){
#ifdef _debug_gps_
					debugstring("�������ĵ�\r\n");
#endif
					if(RunStation.ucStation == 0){//�������ʼ�㣬�����õ����ĵ�λ�ã�һ��Ҫ��������
						RunStation.ucFlag = RS_IN_SOUND;
					}
					else
						RunStation.ucFlag |= RS_IN_MAIN;
				}
				else if((ulOUTDis[0] < _OUT_OUT_)&&(ulOUTCog[0] < COG_IS_OK)){//�����˽�վ��,���ҷ���һ��
					deal_out_station();
					UPdataFeDirect();
#ifdef _debug_gps_
					debugstring("�������ĵ��վ\r\n");
#endif
					if(gGpsDisEN == 'G'){
						display(6, 10, "��վ", 0);
					}
					goto DEAL_IN_END;  //��վ�������
				}
			}
		}

		if((ulNextOutDis[0] < _IN_IN_) && (ulINCog[1] < COG_IS_OK)){//��������һվ,˵����һվ�����⣬Խվ�����վû��
			RunStation.inOrOut = 3;
			RunStation.ucFlag = 0;
			RunStation.ucStation++;							//վ��ָ����һվ
			memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
			debugstring("��������һվ:");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
			debugstring("\r\n");			
#endif
			deal_in_station();
			UPdataFeDirect();
			if(gGpsDisEN == 'G'){
				display(6, 10, "��վ", 0);
			}
			goto DEAL_IN_END;  //��վ�������
		}
		if(((RunStation.ucFlag & RS_IN_SOUND) != 0) && (ulDis[0] > _OVERSTATION_)){   //���˽�վ��������վ
#ifdef _debug_gps_
			debugstring("G P 3: ");
#endif
			RunStation.inOrOut = 3;
			RunStation.ucFlag = 0;
			RunStation.ucStation++;							//վ��ָ����һվ
			memset(FindS.ucFlag, 0, 3);
			FindS.ulDisxx[RunStation.ucDirect] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
#ifdef _debug_gps_
			debugstring("��վ�ˣ�ָ����һվ: ");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
			debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect],4,1);
#endif
			if(gGpsDisEN == 'G'){
				display(6, 10, "��վ1", 0);
			}
		}
		else{
#ifdef _debug_gps_
			debugstring("G P 5: ");
			debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect], 4, 1);
#endif		
			if((ulOUTDis[0] < _OUT_OUT_)&&(ulOUTCog[0] < COG_IS_OK)) {//���ﱾվ�ĳ�վ�㣬ָ����һվ
				deal_out_station();
				UPdataFeDirect();
#ifdef _debug_gps_
				debugstring("��վû����վ\r\n");
#endif
				if(gGpsDisEN == 'G'){
					display(6, 10, "��վ", 0);
				}
				goto DEAL_IN_END;
			}

			if(ulDis[0] > FindS.ulDisxx[RunStation.ucDirect]){//�뱾վԽ��ԽԶ
				if(ulDis[0] > 90){
					if(RunStation.ucStation > 0)//����ʼվ����
						FindS.ucFlag[RunStation.ucDirect]++;//Խ��ԽԶ������1�����ñ�Ч��һվ��������һվ����ʾ
				}
#ifdef _debug_gps_
				debugstring("G P 6: ");
				debugdata((unsigned char*)&FindS.ucFlag[RunStation.ucDirect], 1, 1);
#endif		
			}
			else{
				if(FindS.ucFlag[RunStation.ucDirect]>0)
					FindS.ucFlag[RunStation.ucDirect]--;
			}

			if(FindS.ucFlag[RunStation.ucDirect] > 120){//��վ��Խ��ԽԶ 6 ����������
				ClrRunGPSData();	//����ȷ��վ��
				ClrFindData();
				ClrGpsFED();
#ifdef _debug_gps_
				debugstring("G P 61����Ѱ��վ�㷽��: ");
#endif
				return 1;
			}
#ifdef _debug_gps_
			debugstring("G P 62 RunStation.inOrOut = 3?\r\n");
#endif
			RunStation.inOrOut = 3;
			RunStation.ucOverSpeed = 0;
		}
		//�����������վ��
		ultemp1 = 0xffffffff;
		FindS.ulDisxx[RunStation.ucDirect] = 0xffffffff;	//�Ҵ˷�������ĵ㣬��������վ��ȵ�ǰվ��󲢾������������վ��
		for(i=0; i<LineInfo[RunStation.ucDirect].ucStationMax; i++){
			ultemp = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[i].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[i].ulLongitude);
			if(ultemp <= ultemp1){
				ultemp1 = ultemp;
				FindS.ucStation[RunStation.ucDirect] = i;
				FindS.ulDisxx[RunStation.ucDirect] = ultemp;	//��������վ��ľ���
			}
		}
		ulXCog[RunStation.ucDirect] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[FindS.ucStation[RunStation.ucDirect]].ulHxiang)%36000;
		if(ulXCog[RunStation.ucDirect] > 18000)
			ulXCog[RunStation.ucDirect] = (36000 - ulXCog[RunStation.ucDirect]);
		
		ultemp1 = 0xffffffff;
		flag = (RunStation.ucDirect^0x01);
		FindS.ulDisxx[flag] = 0xffffffff;	//�ҷ���������ĵ㣬��������վ��ȵ�ǰվ��󲢾������������վ��
		for(i=0; i<LineInfo[flag].ucStationMax; i++){
			ultemp = GpsDistance(LineInfo[flag].StationInfo[i].ulLatitude, LineInfo[flag].StationInfo[i].ulLongitude);
			if(ultemp <= ultemp1){
				ultemp1 = ultemp;
				FindS.ucStation[flag] = i;
				FindS.ulDisxx[flag] = ultemp;	//��������վ��ľ���
			}
		}
		ulXCog[flag] = ((gGpsInfo.uiCog+36000) - LineInfo[flag].StationInfo[FindS.ucStation[flag]].ulHxiang)%36000;
		if(ulXCog[flag] > 18000)
			ulXCog[flag] = (36000 - ulXCog[flag]);
#ifdef _debug_gps_
		debugstring("G Find 4  ");
		debugstring(" �˷������վ��:");
		debugstring(LineInfo[RunStation.ucDirect].StationInfo[FindS.ucStation[RunStation.ucDirect]].name);
		debugdata((unsigned char*)&FindS.ucStation[RunStation.ucDirect],1,1);
		debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect],4,1);
		debugdata((unsigned char*)&FindS.ucFlag[RunStation.ucDirect],1,1);
		debugdata((unsigned char*)&ulXCog[RunStation.ucDirect],4,1);
		debugstring(" ���������վ��:");
		debugstring(LineInfo[flag].StationInfo[FindS.ucStation[flag]].name);
		debugdata((unsigned char*)&FindS.ucStation[flag],1,1);
		debugdata((unsigned char*)&FindS.ulDisxx[flag],4,1);
		debugdata((unsigned char*)&FindS.ucFlag[flag],1,1);
		debugdata((unsigned char*)&ulXCog[flag],4,1);
#endif
		if(((FindS.ulDisxx[flag] < _ZU_IN_)&&(ulXCog[flag] < COG_IS_OK)) &&
			((((1+RunStation.ucStation) < LineInfo[RunStation.ucDirect].ucStationMax)&&(RunStation.ucStation > 0))
			||(((1+FindS.ucStation[flag]) < LineInfo[flag].ucStationMax)&&(FindS.ucStation[flag] > 0)))){//���������,���Ҳ����յ����㣬��;���أ�ȥ������վ�㡣
			ClrRunGPSData();	//����ȷ��վ��
			ClrFindData();
			ClrGpsFED();
#ifdef _debug_gps_
			debugstring(" ��;���� ?����Ѱ��վ�㷽��\r\n");
#endif
			return 1;
		}
		//������վ��ľ���ȵ�ǰվ����������ҳ��뱾վҲԽ��ԽԶ 
		else if((FindS.ucStation[RunStation.ucDirect] > RunStation.ucStation) && 
			(((FindS.ucFlag[RunStation.ucDirect] > 5)&&(ulDis[0] > FindS.ulDisxx[RunStation.ucDirect]))||
			((FindS.ulDisxx[RunStation.ucDirect] < (_ZU_IN_+_ZU_IN_))&&(ulXCog[RunStation.ucDirect] < COG_IS_OK)))){//�����Ѿ�����������վ�����ҷ���һ����
#ifdef _debug_gps_
			debugstring(" �����!\r\n");
#endif
			RunStation.inOrOut = 3;
			RunStation.ucFlag = 0;
			RunStation.ucStation = FindS.ucStation[RunStation.ucDirect]; //վ��ָ�������վ�㡣
			memset(FindS.ucFlag, 0, 3);
			FindS.ulDisxx[RunStation.ucDirect] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
			if(gGpsDisEN == 'G'){
				display(6, 10, "��վ2", 0);
			}
		}

DEAL_IN_END:
		FindS.ulDisxx[RunStation.ucDirect] = ulDis[0];
		return 1;	
	}
	return 1;
	//��վ���վ��վ����
#undef _ZU_IN_
	//��վ���վ��վ����
#undef _IN_IN_
	//��վ���վ��վ����
#undef _ZU_OUT_
	//��վ���վ��վ����
#undef _OUT_OUT_
	//�Ƿ��վ�����жϣ�����վ��û������վ�㣩
#undef _OVERSTATION_
#undef COG_IS_OK
}

unsigned char dealGPS(void)
{
	unsigned char err = ST_ERROR;
//	char *p;

	if(gGpscand&GLL_FFLAG){
		memcpy(gGpsCmdBuf.GLL, "$GPGLL,", 7);
		gGpscand &= (~GLL_FFLAG);
//		GLLInfoBuild((char*)gGpsCmdBuf.GLL);
		memset(gGpsCmdBuf.GLL, 0, 10);
	}
	if(gGpscand&GGA_FFLAG){
		memcpy(gGpsCmdBuf.GGA, "$GPGGA,", 7);//���жϽ�������GPS��Ϣ��ʶû�б���
		gGpscand &= (~GGA_FFLAG);
		GGAInfoBuild((char*)gGpsCmdBuf.GGA);
		memset(gGpsCmdBuf.GGA, 0, 10);
	}
	if(gGpscand&VTG_FFLAG){
		memcpy(gGpsCmdBuf.VTG, "$GPVTG,", 7);
		gGpscand &= (~VTG_FFLAG);
//		VTGInfoBuild((char*)gGpsCmdBuf.VTG);
		memset(gGpsCmdBuf.VTG, 0, 10);
	}
	if(gGpscand&ZDA_FFLAG){
		memcpy(gGpsCmdBuf.ZDA, "$GPZDA,", 7);
		gGpscand &= (~ZDA_FFLAG);
//		ZDAInfoBuild((char*)gGpsCmdBuf.ZDA);
		memset(gGpsCmdBuf.ZDA, 0, 10);
	}
	if(gGpscand&RMC_FFLAG){
		memcpy(gGpsCmdBuf.RMC, "$GPRMC,", 7);
		gGpscand &= (~RMC_FFLAG);
		RMCInfoBuild((char*)gGpsCmdBuf.RMC);
		memset(gGpsCmdBuf.RMC, 0, 10);
	}
	return err;
}

#ifdef _GPS_TRA_REC_
void writeGpsTradRec(void)//��¼���ƶ��켣�����ڵ��Ե�����
{
	unsigned char buff[RECORD_LEN];
	
	sysferead(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	if(ComSumeFile.BR_curE2pointer == 0)
		clr_Record_point();
	memset(buff, 0, RECORD_LEN);
	memcpy(buff, (unsigned char*)&gGpsInfo, sizeof(GPS_INFO));
	WriteRecord(buff, 0);

	ComSumeFile.BR_curE2pointer++;
	sysfewrite(BIT_COMSUME_FILE,sizeof(Parameter3),(unsigned char*)&ComSumeFile);
	beep(1, 100, 100);
}
#endif 

#ifdef FengDuan_BU_
extern void dealFengDuanStation(void);
extern void EX_dealFengDuanStation(void);
#endif

#ifdef _GPS_Station_ExKey_enable_
extern void keyboard_open(void);
extern unsigned char KeyMange(unsigned char kkvv);
extern void LedDisplay(unsigned char *ptr);
extern unsigned char Keybuffer[10];			//�Ͱ�������Ʊ�۹���BUFF
extern volatile unsigned char itKEYB_value;
extern void LedDisplay(unsigned char *ptr);
void GPS_Ex_KeyDis(unsigned int value,unsigned char *ptr);
void Ex_keyBoardStart(void)
{
	keyboard_open();
	memset(Keybuffer,0x20,5);
	
	GPS_Ex_KeyDis(RunStation.ucStation+1,Keybuffer);
	LedDisplay(Keybuffer);
	memset(Keybuffer,0x20,5);
	Keybuffer[5]='0';
}

void GPS_Ex_KeyDis(unsigned int value,unsigned char *ptr)
{
	int i;
	if(RunStation.ucDirect != 0){
		i = 0-value;
		sprintf((char*)ptr,"%-6d",i);
	}
	else
		sprintf((char*)ptr,"%-6d",value);
}
unsigned char GPS_Ex_KeyMange(unsigned char kkvv)
{
	unsigned char i=0,a=0,j=0;
	unsigned int ii=0,KeyVa;
	a=0;
	//С���㲻�ܳ�������
	
	for(i=0;i<6;i++)
	{
		if((Keybuffer[5-i]&0x80)==0x80)
			a++;
	}
	//Ѱ��С�����λ��
	for(i=0;i<6;i++)
	{
		if((Keybuffer[5-i]&0x80)==0x80)
			break;
	}
	if(((i>=2)&&(i!=6))||(a>=1))
	{
		beep(2,50,50);
		memset(Keybuffer,0x20,5);
		Keybuffer[5]='0';
		KeyVa=0;
	} 
	else
	{
		a=0;
		KeyVa=0;
		for(j=0;j<6;j++)
		{
			if(Keybuffer[5-j]!=0x20)
				a++;
		}
		if(i==1)
		{
			KeyVa=(unsigned int)((Keybuffer[5]-'0')*10);

		}
		else if(i==2)
		{
			KeyVa=(unsigned int)((Keybuffer[5]-'0')+((Keybuffer[4]-'0')*10));

		}
		else if(i==6)
		{
             i=0;
		}
		ii=10;
	
		for(j=i;j<a;j++)
		{
			ii=ii*10;
			KeyVa+=(unsigned int)(((Keybuffer[5-j]-'0')&0x7f)*ii);
		}
		KeyVa /= 100;
// 		debugstring(" Xv ");
// 		debugdata((unsigned char *)&KeyVa, 4, 1);
// 		debugstring(" Xb ");
// 		debugdata((unsigned char *)&RunStation.ucDirect, 1, 1);
		if(RunStation.ucDirect >= 2){//վ����Ϣ����
			beep(2,50,50);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			GPS_Ex_KeyDis(0,Keybuffer);
			LedDisplay(Keybuffer);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			return ST_OK;
		}
		else if(KeyVa > LineInfo[RunStation.ucDirect].ucStationMax)//����ֵ������վ����������
		{
			beep(2,50,50);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			KeyVa=0;
			GPS_Ex_KeyDis(KeyVa,Keybuffer);
			LedDisplay(Keybuffer);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			KeyVa=0;
			return ST_OK;
		}
		else{
			LedDisplay(Keybuffer);
		}

	}
	if(kkvv==0x0d)	//ȷ����
	{
		beep(1,50,25);
		if(KeyVa==0)	//0��վ�㣿��Ч, �ر���ǰվ��
		{
			if((RunStation.ucDirect >= 2)||(RunStation.ucStation > LineInfo[RunStation.ucDirect].ucStationMax)){
				beep(2,50,50);					//û�з����վ�㣬��Ҫ����
				GPS_Ex_KeyDis(0,Keybuffer);
				LedDisplay(Keybuffer);
				memset(Keybuffer,0x20,6);
				Keybuffer[5]='0';
				
				return ST_OK;
			}
			else{//�Ѿ����ˣ������ر�һ��
				if((RunStation.ucFlag & RS_IN_SOUND) != 0)//�Ѿ�����վ
					deal_out_station();
				else{
					deal_in_station();
#ifndef _TTSSound_enable_	//���û����������Ҫָ����һվ��[�������ϳ�վ],���԰�һ�ξ�����
					deal_out_station();
#endif
				}
			}
		}
		else
		{
			if(RunStation.ucDirect >= 2)
				RunStation.ucDirect = 0;
			if(RunStation.ucStation != (KeyVa-1)){//��ǰվ�������վ�㲻һ�£����״̬
				memset(FindS.ucFlag, 0, 3);
				RunStation.inOrOut = 0;
				RunStation.ucFlag = 0;
				RunStation.ucPeStation = RunStation.ucStation;
			}

			RunStation.ucStation = (KeyVa-1);
#ifdef _TTSSound_enable_
			if((RunStation.ucFlag & RS_IN_SOUND) != 0)//�Ѿ�����վ
				deal_out_station();
			else{
#endif
				deal_in_station();
#ifdef _TTSSound_enable_
			}
#endif
		}
		GPS_Ex_KeyDis(RunStation.ucStation+1,Keybuffer);
		LedDisplay(Keybuffer);
		memset(Keybuffer,0x20,5);
		Keybuffer[5]='0';
	}
	else
	{
		KeyVa=0;
	}
	return ST_OK;
}

unsigned char GPS_StationEnable(unsigned char value)
{
	unsigned char j=0, dat;
	
	if(value == 0xFF){
		return ST_ERROR;
	}
	else
		dat = value;
	if((dat>='0')&&(dat<='9'))
	{
		for(j=0;j<5;j++)
		{
			if(memcmp(Keybuffer,"\x20\x20\x20\x20\x20",5)==0)
				if(Keybuffer[5]=='0')
					Keybuffer[5]=0x20;
				Keybuffer[j]=Keybuffer[j+1];
		}
		Keybuffer[5]=dat;
	}
	else if(dat==0x08)//�����
	{
		for(j=0;j<5;j++)
		{
			Keybuffer[5-j]=Keybuffer[4-j];
		}
		if(Keybuffer[0]!=0x20)
			Keybuffer[0]=0x20;
		
		if(Keybuffer[5]==0x20)
		{
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
		}
	}
	else if(dat==0x0d)//ȷ����
	{
		if((Keybuffer[5]&0x80)==0x80)
		{
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
		}
		
	}
	else if(dat==0x2e)
	{
		Keybuffer[5]|=0x80;
	}//  41=F1��  42=F2��
	else if(dat == 0x41){//���򣬷���
		if(RunStation.ucDirect > 0){
			RunStation.ucDirect = 0;
		}
		else{
			RunStation.ucDirect = 1;
		}
		restore_flag = 3;
	}
	else{
		return ST_OK;
	}

	if(GPS_Ex_KeyMange(dat) == ST_OK){
		if(dat == 0x0d){
			save_fe_gpsdata();//�������в���������
		}
	}
	return ST_OK;
}
#endif

//GPS ���������
void mainGps(void)
{
//	unsigned int ulT1;
	unsigned char uct;
#ifdef _debug_gps_
	unsigned char buf[100];
#endif
	char disbuf[50];

//	stUPGPS upgps;
//	unsigned int uiCrc16;
	
#ifdef _GPS_Station_ExKey_enable_
	if(GPS_StationEnable(itKEYB_value) == ST_OK){
		itKEYB_value = 0xff;
		EX_dealFengDuanStation();
	}

#endif	//#ifdef _GPS_Station_ExKey_enable_

#ifndef _GPS_TEST_
	if(gGpscand){//�ж�λ��ϢGPS���ݽ������
		if(dealGPS() == ST_ERROR)
			;//return;//GPS������Ч
	}
	else
		return;
#endif
	if(gGpsInfo.ucValid == GPSVALID){
#ifdef _debug_gps_
		debugstring("GPSʱ�䣺");
		debugdata((unsigned char*)&gGpsInfo.UTCtime, 7, 1);
		sprintf((char*)buf, "γ�ȣ�%u\r\n\x0",gGpsInfo.Latitude);
		debugstring((char*)buf);
		sprintf((char*)buf, "���ȣ�%u\r\n\x0",gGpsInfo.Longitude);
		debugstring((char*)buf);
		debugstring("�ٶȣ�");
		debugdata((unsigned char*)&gGpsInfo.velocity,4,1);
		debugstring("����������");
		debugdata((unsigned char*)&gGpsInfo.SataletNum,1,1);
		debugstring("״̬��");
		debugdata((unsigned char*)&gGpsInfo.ucState,1 ,1);
#endif
		memcpy((unsigned char*)&SysTime.year_h, (unsigned char*)&gGpsInfo.UTCtime, 7);
		if(time_invalid((unsigned char*)&SysTime.year_h) != 0)
			gpsTimeOK = 0xe5;//GPSʱ����Ч
		else
			gpsTimeOK = 0;

		if(gGpsDisEN == 'G'){
			memset(disbuf, 0, 20);
			sprintf(disbuf, "Num:%02d ", gGpsInfo.SataletNum);
			miniDispstr(3,0,disbuf, 0);
			sprintf(disbuf, "E:%d ", gGpsInfo.Longitude);
			miniDispstr(4, 0, disbuf, 0);
			sprintf(disbuf, "N:%d ", gGpsInfo.Latitude);
			miniDispstr(5, 0, disbuf, 0);
			
			sprintf(disbuf, "%02x-%02x-%02x %02x:%02x:%02x \0", gGpsInfo.UTCtime.year, gGpsInfo.UTCtime.month, 
				gGpsInfo.UTCtime.day, gGpsInfo.UTCtime.hour, gGpsInfo.UTCtime.min, gGpsInfo.UTCtime.sec);
			miniDispstr(2,0,disbuf, 0);
			sprintf(disbuf, "C:%d,V:%d \0", gGpsInfo.uiCog, gGpsInfo.velocity);
			miniDispstr(0,2,disbuf, 0);
		}
		
#ifdef _GPS_TRA_REC_
		writeGpsTradRec();//��¼���ƶ��켣�����ڵ��Ե�����
		gGpsInfo.ucValid = GPSINVALID;
		return;
#endif 

		gGpsInfo.ucValid = GPSUES;

		uct = GpsInfoDeal();
		if(uct != 0xF0)	//��ά����Ϣ���󣬲�����
#ifdef FengDuan_BU_

			dealFengDuanStation();
#endif
		if(uct < 0xF0)
			save_fe_gpsdata();//�������в���������
//		if((GPRSLinkProcess < TCPSTARTSTAT+6)||(GPRSLinkProcess == 0xb1)){	//û��½��
		if(RunStation.inOrOut == 2){	//��վ������ɣ�ָ������һվ��
			;
		}
		else if(RunStation.inOrOut == 1){	//�����յ�վ
			;
		}
	}
}

//GPS����������
void main_Gps(void)
{
	mainGps();
}

#endif
