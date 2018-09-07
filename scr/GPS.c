
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
unsigned int distance[2];//离起始站的距离
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
extern volatile unsigned char gKeyRestorDis;//需要重新刷新KEYPAD显示
#endif
#ifdef FengDuan_BU_

extern unsigned char gFDderict; 
extern unsigned char gFDucID;
#endif
#ifdef _GPS_DIAODU_
//发送GPS报警信息
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
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	Get_SerialNum(dat+len);
	//	Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
	dat[len++] = 0x02;
	
	dat[len++] = 01;//GnCode 
	
	memset(dat+len, 0, 4);//P1到P4
	len+=4;
	
	dat[len++] = 0x00;//长度=48字节
	dat[len++] = 0x40;
	
	memset(dat+len, 0, 4);
	len += 2;
	memcpy(dat+len, pFistVary.LineNo, 2);//主线路4B
	len += 2;

	memset(dat+len, 0, 4);
	len += 2;//子线路4B
	memcpy(dat+len, pFistVary.LineNo, 2);//子线路4B
//	sysferead(BIT_GPSChildLine, 2, dat+len);
	len += 2;
	
	ASCII2ZBcod(pFistVary.DeviceNo, dat+len, 4);//车号4B
	len +=4;
	
	memcpy(dat+len, DriveCardNo, 4);//人员4B
	len +=4;
	
	if(((RunStation.ucDirect < 2)&&(RunStation.ucStation > 0))||\
		((gGpsFeD.direct < 2)&&(gGpsFeD.flag > 0)))
		dat[len++] = 1;
	else
		dat[len++] = 2;//是否运营代码 1B 01-营运中，02-非营运

	if(RunStation.ucDirect >= 2){
		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
		if(gGpsFeD.direct<2){
			dat[len++] = ((LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId>>4)&0x0F)*10 + (LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId&0x0F); //站点编号
			
			dat[len++] = 1;//站点类型
			dat[len++] = gGpsFeD.direct+1;//站点方向
		}
		else{
			dat[len++] = 0; //站点编号
			
			dat[len++] = 1;//01-进入站点,02-离开站点
			dat[len++] = 0;//站点方向
		}
	}
	else{
		dat[len++] = ((LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId>>4)&0x0F)*10+(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId&0x0F);//站点编号
		
		dat[len++] = 1;//01-进入站点,02-离开站点
		dat[len++] = RunStation.ucDirect+1;//站点方向 01-正向,02-反向
	}
	memcpy(dat+len, (unsigned char*)&gGpsInfo.Longitude, 4);//+经度4B
	over_turn(4, dat+len);
	len += 4;
	memcpy(dat+len, (unsigned char*)&gGpsInfo.Latitude, 4);//纬度4B
	over_turn(4, dat+len);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&gGpsInfo.velocity, 2);	//速度2B
	over_turn(2, dat+len);
	len += 2;
	
	memcpy(dat+len, gDiaoDuinfo.limitRDNo, 2);//路段编号
	len +=2; 

	dat[len] = galamessage.alamFlag;//报警代码1B
	len++;

	memcpy(dat+len, (unsigned char*)&SysTime, 7);
	len += 7;

	memset(dat+len, 0, 24);
	len += 24;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
	
}
#endif
#ifdef _GPS_NEED_REC_
//发GPS定位信息
unsigned int Build_GJ_GPS(unsigned char *dat)
{
	unsigned int ltemp;
	unsigned int len=0;
	unsigned short crc16;
	
#ifdef _debug_gprs
	debugstring("gGpsInfo.ucValid:");
	debugdata((unsigned char*)&gGpsInfo.ucValid, 1, 1);
#endif
	if((gGpsInfo.ucValid != GPSVALID)&&(gGpsInfo.ucValid != GPSUES)){//GPS信息无效
		return 0;
	}
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	Get_SerialNum(dat+len);
//	Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
	dat[len++] = 0x01;
	
	dat[len++] = 01;//GnCode 
	
	memset(dat+len, 0, 4);//P1到P4
	len+=4;
	
	dat[len++] = 0x00;//长度=48字节
	dat[len++] = 0x40;
	
	memset(dat+len, 0, 4);
	len += 2;
	memcpy(dat+len, pFistVary.LineNo, 2);//主线路4B
	len += 2;
	
	memset(dat+len, 0, 4);
	len+=2;
//	sysferead(BIT_GPSChildLine, 2, dat+len);//子线路4B   有时会取错
	memcpy(dat+len, pFistVary.LineNo, 2);
	len+=2;
	
	ASCII2ZBcod((char*)pFistVary.DeviceNo, dat+len, 4);//车号4B
	len +=4;
	
	memcpy((unsigned char*)&ltemp, DriveCardNo, 4);
	ltemp %= 100000000;
	memcpy(dat+len, (unsigned char*)&ltemp, 4);//人员4B
	over_turn(4, dat+len);
	len +=4;
#ifdef _GPS_DIAODU_
	if(gDiaoDuCMD.ucStartYorN != 2)
		dat[len++] = 1;
	else
		dat[len++] = 2;//停运 是否运营代码 1B
#else
	dat[len++] = 1;
#endif
	
	if(RunStation.ucDirect >= 2){
		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
		if(gGpsFeD.direct<2){
			dat[len++] = ((LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId>>4)&0x0F)*10 + (LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId&0x0F); //站点编号
			dat[len++] = 1;//站点类型
			dat[len++] = gGpsFeD.direct+1;//站点方向
		}
		else{
			dat[len++] = 1; //站点编号
			
			dat[len++] = 1;//01-进入站点,02-离开站点
			dat[len++] = 0;//站点方向
		}
	}
	else{
		dat[len++] = ((LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId>>4)&0x0F)*10+(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId&0x0F);//站点编号
		dat[len++] = 1;//01-进入站点,02-离开站点
		dat[len++] = RunStation.ucDirect+1;//站点方向 00-正向,01-反向
	}
#ifdef _debug_
	debugstring("GPS记录，方向值：");
	debugdata((unsigned char*)&RunStation.ucDirect, 1, 1);
	debugdata((unsigned char*)&dat+len-1, 1, 1);
#endif

	memcpy(dat+len, (unsigned char*)&gGpsInfo.Longitude, 4);//+经度4B
	over_turn(4, dat+len);
	len += 4;
	memcpy(dat+len, (unsigned char*)&gGpsInfo.Latitude, 4);//纬度4B
	over_turn(4, dat+len);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&gGpsInfo.velocity, 2);	//速度2B
	over_turn(2, dat+len);
	len += 2;
#ifdef _GPS_DIAODU_
	memcpy(dat+len, gDiaoDuinfo.limitRDNo, 2);//路段编号
#else
	memset(dat+len, 0, 2);
#endif
	len +=2; 
	
	dat[len++] = galamessage.alamFlag;//最后一次报警代码
	
#ifdef _GPS_DIAODU_
	memcpy(dat+len, gDiaoDuCMD.ZLseiral, 2);//最后一次调度编号
#else
	memset(dat+len, 0, 2);
#endif
	len += 2;
	dat[len++] = 0;//人次

	memcpy(dat+len, (unsigned char*)&SysTime.year_h, 7);
	len += 7;
	
	memset(dat+len, 0, 21);
	len += 21;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
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
		display(6, 0,"无站名", 0);
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
	//显示下记录轨迹的保存地址
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

//GPS中断处理程序
void GPS_handler(unsigned char value)
{
//中断要尽快退出，数据的处理在中断外执行(否则会影响I2C总线)
//	com_snd(COM4, 1, (unsigned char*)&value);//test
//只采点，用于转发采点信息
#ifdef _GPS_CAIDIAN_
	com_snd(COM1, 1, (unsigned char*)&value);
	return;
#elif defined _GPS_TEST_
	return;
#else
//	com_snd(COM1, 1, (unsigned char*)&value);
	if((gucGPSindex==0)&&(value!='$')){					//数据错误
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
			gGpscand |= GGA_FFLAG;	//这是第一条信息
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
//经纬度格式：度度分分.分分分分分
// $GPGLL,2232.95729,N,11404.74220,E,061316.00,A,D*6D
unsigned char GLLInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned int lt1=0,lt2=0;
	unsigned char i, t;

	//校验
	//找出纬度
	
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
	if((*p2)==',')	return 0;			//没有经纬度信息
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
		for(t=i;t<4;t++)					//保证小数位有5位有效数字
			lt2 *= 10;
	}
	if(lt2>=100000) return 0;
	gGpsInfo.Latitude = lt1*100000+lt2;
	p2++;
//	if((*p2)=='S')
//		pGpsInfo->Latitude = pGpsInfo->Latitude;						//南纬	最高位置1

	p2+=2;
	p1=p2;
	if((*p2)==',')	return 0;			//没有经纬度信息
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
//		pGpsInfo->Longitude = pGpsInfo->Longitude;		//西经	最高位置1

	p1=p2+2;											//指向UTC
	p2=strchr(p1, ',');
	if(p2==0)			return 0;
	p2++;										
//	if((*p2)!='V'){										//当前数据有效
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
	debugstring("纬度:");
	debugdata((unsigned char*)&gGpsInfo.Latitude,4,1);
	debugstring("经度:");
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

	//校验

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
	p1=p2+1;	//时间
	
	p2=strchr(p1, ',');
	p1=p2+1;	//纬度

	p2++;
	if((*p2)==',')	return 0;			//没有纬度信息
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
		if(i==4) break;	//加上循环前共处理5位
	}
	if(i<4){
		for(t=i;t<4;t++)					//保证小数位有5位有效数字
			lt2 *= 10;
	}
	if(lt2>=100000) return 0;
	gGpsInfo.Latitude = lt1*100000+lt2;
	p2++;//指向N

	p2+=2;
	p1=p2;
	if((*p2)==',')	return 0;			//没有经度信息
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
	p2++;//指向E


	p2++;//,
	p2++;//质量因子
//	debugstring(p2);
	if((*p2)=='0'){						//没有GPS信号 0没有定位，1=实时，2=差分
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
	gGpsInfo.SataletNum=t;				//卫星数量
	p2++;//水平因子

	p1=p2;
	p2=strchr(p1, ',');
	p2++;
	p1=p2;	//天线高度，海平面
	
	if(gGpsDisEN == 'G'){
		if(gGpsInfo.ucValid == GPSVALID){
			miniDispstr(4, 20, "V", 0);
		}
		else{
			miniDispstr(4, 20, "N", 0);
		}
	}

#ifdef _debug_gps_
	debugstring("卫星数量：");
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
	//校验
	
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
	t1 *= 1852; // 1节 = 1.852千米
	t1 /= 1000;//速率(m/小时)
	t1 /= 10;// 把米/小时 换成 10m/小时  平台发下来的是公里*100，单位就是10m/h
	gGpsInfo.velocity = t1;				//速度
#ifdef _debug_gps_
	debugstring("速度：");
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

	//校验
	
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
	TimeAdd((unsigned char*)&gGpsInfo.UTCtime, (unsigned char*)&gGpsInfo.UTCtime,10800);//东三区的时间， 埃塞俄比亚
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
	debugstring("GPS时间：");
	debugdata((unsigned char*)&gGpsInfo.UTCtime.centry,sizeof(TimeGPS),1);
#endif
	return 1;
}
//1节＝1海里／小时，1海里＝1.852公里。1节也就是1.852公里/小时。
//PRMC,082118.000,A,2233.7106,N,11409.4819,E,0.00,39.50,040711,,,D*5C
//$GPRMC,061316.00,A,2232.95729,N,11404.74220,E,0.079,,231008,,,D*72
unsigned char RMCInfoBuild(char *vv)
{
	char *p1,*p2;
	unsigned char i;
	unsigned int t,t1;
	//校验
	
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
	
	for(i=0;i<6;i++){//需要指向地面速率(000.0~999.9节，前面的0也将被传输)
		p1=p2;
		p2=strchr(p1, ',');
		p2++;	// A
	}
	
	if(*p2==','){
		p2++;//没有速率
	}
	else{
		//速率
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
		t1 *= 1852; // 1节 = 1.852千米
		t1 /= 1000;//速率(m/小时)
		t1 /= 10;// 把米/小时 换成 10m/小时  平台发下来的是公里*100，单位就是10m/h
		gGpsInfo.velocity = t1;//速率(m/小时)
	}
	p2++;

	if(*p2==','){
#ifdef _GPS_TEST_
		return 1;
#else
		p2++;//没有航向
#endif
	}
	else{
		//航向
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
		gGpsInfo.uiCog = t1;//航向
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
	debugstring("GPS时间：");
	debugdata((unsigned char*)&gGpsInfo.UTCtime.centry,sizeof(TimeGPS),1);
	debugstring("速度：");
	debugdata((unsigned char*)&gGpsInfo.velocity,4,1);
	debugstring("-----------------\r\n");
#endif
	return 1;
}

//GPS模块不输出
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
// //	strcpy(strbuf, "$PJRC314,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,0*");//每秒
// 	strcpy(strbuf, "$PJRC314,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,3,0*");//每三秒
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

//清除GPS参数，第一次开机调用
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
	flasherase(ERASE64KBYTESECTOR, uladd);	//线路
	Delay(2);
	uladd += 0x10000;
	flasherase(ERASE64KBYTESECTOR, uladd);	//线路

	memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
	fewrite(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);
}
//存GPS数据到铁电中
//运行参数 + 找到的站点状态 + 时间
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
//检查启动前的铁电中的GPS数据是否有效
//如果时间相差小于10秒，则判断有效
void check_fe_gpsdata(void)
{
	unsigned char febuf[50];
//	unsigned int t1,t2;

	sysferead(BIT_GPS_RUN, 37, febuf);

#ifdef _debug_gps_
	debugstring("chek fe gps data:");
	debugdata(febuf, 37, 1);
#endif
	if(time_invalid(febuf+30) == 0)//时间不合法
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
//取调度命令信息
void Load_DIAODUCMD_info(void)
{
//	unsigned short t1, t2;

	sysferead(BIT_GPSDiaoDuCMD_INFO, sizeof(stDiaoDuCMD), (unsigned char*)&gDiaoDuCMD);
	if(memcmp(gDiaoDuCMD.Date, (unsigned char*)&SysTime.year_h, 4)){//日期和当前日期不一致
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
		(gDiaoDuCMD.ucDDstyle == 0x10)||(gDiaoDuCMD.ucDDstyle == 0x11))//有调度信息,可以清除
	{
		gDiaoDuCMD.ucCMD = 0;//指令清为0，保留指令流水，在GPS包中会用到。
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
//取调度营运信息
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
			debugstring("营运信息CRC ERROR!\r\n");
	#endif
			if(i>=3)
				return;
		}else{

			break;
		}
	}
	gDiaoDuinfo.licLON = 0;//每次读出，清除上一点的GPS经纬度，重新从此点开始计数
	gDiaoDuinfo.licLA = 0;
	gDiaoDuinfo.licJD = 0;

	memcpy((unsigned char*)&crc, gDiaoDuinfo.limitRDNo, 2);
	if((crc > 500)||(RunStation.ucDirect >= 2)){
		gDiaoDuinfo.nearS = 0xff;
		gDiaoDuinfo.limitID = 0xff;
	}
#ifdef _GPS_Speed
	debugstring("营运信息:");
	debugdata((unsigned char*)&gDiaoDuinfo, sizeof(stDiaoDuinfo), 1);
#endif
}
//初始化GPS参数
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
//	memset((unsigned char*)&gLcdCmd, 0x00, sizeof(stLcdCmd)); //清除
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

//Rstr:返回的字符
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
//装载线路信息
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

	uladdr = FLASH_GPS_LINE_START;//这里是长度和版本号,先是站点信息，限速信息，广告信息

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

		if(memcmp(buffer, ffbuf, 52) == 0)//全是FF，数据不存在
			break;
		if(buffer[0] == 0){//总版本信息 信息类型 1B（00）+最大包号4B +主版本号2B+主线编号4B+预留41B
			if(memcmp(pFistVary.LineNo, buffer+9, 2)!=0){//线路编号不一致，取消
#ifdef _debug_
				debugstring(" 0xA1 Line No. Err:");
				debugdata(pFistVary.LineNo, 2, 1);
				debugdata(buffer+9, 2, 1);
#endif
				continue;
			}
			memcpy(gBusVerInfo.busLineVer, buffer+5, 2);//版本号
#ifdef _debug_
			debugstring(" 0xA1 Line busLineVer:");
			debugdata(gBusVerInfo.busLineVer, 2, 1);
#endif
		}
		else if(buffer[0] == 0x01){//是站点信息头
			if(memcmp(pFistVary.LineNo, buffer+5, 2)!=0){//线路编号不一致，取消
#ifdef _debug_
				debugstring("Line No. Err:");
				debugdata(buffer+5, 2, 0);
				debugstring("::");
				debugdata(pFistVary.LineNo, 2, 1);
#endif
				uladdr += buffer[11]*52;//跳过正向站点数量
				uladdr += buffer[12]*52;//跳过反向站点数量
				continue;
			}
//			if((pFistVary.LineNo[2] != 0)&&(pFistVary.LineNo[2] != buffer[10])){//分线路编号不一致，取消,分线路为0，有效
/*			if((memcmp(zlbuf, pFistVary.LineNo,2)!=0)&&(memcmp(zlbuf, buffer+9, 2)!=0)){
#ifdef _debug_gps_
				debugstring("Line F No. Err:");
				debugdata(zlbuf, 2, 0);
				debugdata(pFistVary.LineNo, 2, 0);
				debugdata(buffer+9, 2, 0);
#endif
				uladdr = buffer[11]*52;//跳过正向站点数量
				uladdr = buffer[12]*52;//跳过反向站点数量
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
			
			for(i=0; i<LineInfo[0].ucStationMax; i++){//正向站
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
				//if(memcmp(buffer, ffbuf, 52) == 0)//全是FF，数据不存在
				if(buffer[0] > 200){//UID错误
					memset(gBusVerInfo.busLineVer, 0, 2);
					memset((unsigned char*)&LineInfo[0], 0, 20);
					memset((unsigned char*)&LineInfo[1], 0, 20);
					memset((unsigned char*)&SpeedInfo[0], 0, 20);
					memset((unsigned char*)&SpeedInfo[1], 0, 20);
					memset((unsigned char*)&gSoundInfo, 0, (sizeof(stSoundInfo)));
					return;
				}
				if(i>=STATION_MAX_COUNT)
					continue; //大于50的信息不保存
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
				for(uitemp=0;uitemp<17;uitemp++){//34-51,共17个字节
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
			for(i=0; i<LineInfo[1].ucStationMax; i++){//反向站
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
				//if(memcmp(buffer, ffbuf, 52) == 0)//全是FF，数据不存在
				if(buffer[0] > 200){//UID错误
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
					continue; //大于50的信息不保存
				t = (LineInfo[1].ucStationMax-i-1);//反向是反向的，所以要反向保存
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
				for(uitemp=0;uitemp<17;uitemp++){//34-51,共17个字节
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
		else if(buffer[0] == 0x02){//是限速信息
			if(memcmp(pFistVary.LineNo, buffer+5, 2)!=0){//线路编号不一致，取消
#ifdef _debug_
				debugstring("SPEED L No. Err:");
				debugdata(buffer+5, 2, 0);
				debugstring("::");
				debugdata(pFistVary.LineNo, 2, 1);
#endif
				
				uladdr += buffer[11]*52;//跳过正向站点数量
				uladdr += buffer[12]*52;//跳过反向站点数量
			}
//			if(pFistVary.LineNo[2] != buffer[10]){//分线路编号不一致，取消
		//	if((memcmp(zlbuf, pFistVary.LineNo,2)!=0)&&(memcmp(zlbuf, buffer+9, 2)!=0)){
			if(memcmp(pFistVary.LineNo, buffer+5, 2)!=0){//线路编号不一致，取消
#ifdef _debug_
				debugstring("SPEED F No. Err:");
#endif
				uladdr += buffer[11]*52;//跳过正向站点数量
				uladdr += buffer[12]*52;//跳过反向站点数量
			}

//判断模式 
			if (memcmp(buffer+13,"\x00\x00\x00\x00",4)==0)
				SpeedLimitMode=SpeedLimit_Old;
			else
				SpeedLimitMode=SpeedLimit_New;//新的模式
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
			
			for(i=0; i<SpeedInfo[0].ucStationMax; i++){//正向站限速
				if(uladdr >= FLASH_GPS_LENE_END)
					break;
				flashread(uladdr, buffer, 26);
				uladdr += 26;
#ifdef _debug_
				debugstring("SPEED+:");
				debugdata(buffer,26,1);
#endif
				if(memcmp(buffer, ffbuf, 26) == 0)//全是FF，数据不存在
					break;
				if(i>=SPEED_MAX_COUNT)
					continue; //大于50的信息不保存
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
			
			for(i=0; i<SpeedInfo[1].ucStationMax; i++){//反向站限速
				if(uladdr >= FLASH_GPS_LENE_END)
					break;
				flashread(uladdr, buffer, 26);
				uladdr += 26;
#ifdef _debug_
				debugstring("SPEED-");
				debugdata(buffer,26,1);
#endif
				if(memcmp(buffer, ffbuf, 26) == 0)//全是FF，数据不存在
					break;
				if(i>=SPEED_MAX_COUNT)
					continue; //大于50的信息不保存
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
		else if(buffer[0] == 0x03){//广告信息,是最后的信息
			memset((unsigned char*)&gSoundInfo, 0, (sizeof(stSoundInfo)));
			memcpy((unsigned char*)&ust, buffer+1, 2);
			over_turn(2, (unsigned char*)&ust);
			gSoundInfo.uiVer = ust;
			gSoundInfo.usNum = buffer[7];
			gSoundInfo.uiGaddr = uladdr; //语音起始地址
#ifdef _debug_
			debugdata((unsigned char*)&gSoundInfo, 12, 0);
#endif
			if(gSoundInfo.usNum == 0){ //语音找完
				break;//是最后的信息
			}
			for(i=0; i<gSoundInfo.usNum; i++){
				if(uladdr >= FLASH_GPS_LENE_END)
					break;//是最后的信息
				flashread(uladdr, buffer, 64);
#ifdef _debug_
				debugstring("Sound-");
				debugdata(buffer,64,1);
#endif
				if(memcmp(buffer, ffbuf, 64) == 0)//全是FF，数据不存在
					break;
				gSoundInfo.uiSaddr[i] = uladdr;//这段的起始地址
				for(uitemp=0;uitemp<64;uitemp++){//找下段的起始地址
					if(buffer[uitemp] == '|'){
						uladdr += uitemp;
						uladdr++;
						break;
					}
					else if(buffer[uitemp] == 0){
						uitemp = 64;//结束符，不用找了
						break;
					}
				}
				if(uitemp>=64){//没找到下段起始地址,不再找，退出
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
			break;//是最后的信息
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
		gGPsPar.EnableGps = 0;	//线路信息不正确  禁止GPS信息
		LineInfo[0].uiVer=0;
		LineInfo[1].uiVer=0;
		for (i=0; i<STATION_MAX_COUNT; i++)
		{
			memset((unsigned char*)&LineInfo[0], 0, sizeof(stLineInfo));
			memset((unsigned char*)&LineInfo[1], 0, sizeof(stLineInfo));
		}
	}
	else{
		gGPsPar.EnableGps = 1;	//开启GPS信息处理   红外下载的文件，总版本号是在前面的。
		if((gBusVerInfo.busLineVer[0] == 0)&&(gBusVerInfo.busLineVer[1] == 0))//如果前面没取到版本号，就在这里取
			flashread(FLASH_GPS_LENE_VER, gBusVerInfo.busLineVer, 2);//取线路版本号
		//flashread(FLASH_GPS_LENE_VER, gBusVerInfo.busLineVer, 2);//取线路版本号
	}

	//flashread(FLASH_GPS_LENE_VER, gBusVerInfo.busLineVer, 2);//取线路版本号
#ifdef _debug_
	if(gGPsPar.EnableGps == 0)
		debugstring("GPS禁止 线路不对！");
	else
		debugstring("GPS开启!");
	debugdata(gBusVerInfo.busLineVer, 2, 1);
	debugstring("\r\n");
#endif
}

//初始化当前车载GPS信息
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

//4字节int类型。单位为"0.000001度"
unsigned int GpsNDeal(unsigned int valueg)
{
	unsigned int t, t1;
	
	t = valueg/10000000;//前面是度
	t1 = valueg%10000000;//后面是分，有7位
	t1 *= 100;
	t1 /= 60;//转换成分，小数位丢弃
// 	for(;;){
// 		if(t1 >= 1000000)
// 			break;
// 		else
// 			t1 *= 10;
// 	}
	t = t*10000000 + t1;

	t /= 10; //后台只有6位小数位，需缩小10倍
	return t;
}

extern void CountPoint(unsigned char mode);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
//记录类型 （增加E7）
//总里程记录（记录类型E0）//总里程
//人次记录(记录类型E1)
//超速记录(记录类型E2)
//主站进站记录(记录类型E3)
//主站离站记录(记录类型E4)
//副站进站记录(记录类型E5)
//副站离站记录(记录类型E6)
// 30分钟的里程记录（记录类型E7）
//司机卡总站点记录（记录类型E8）

//中间进站		（记录类型EA）
//中间出站		（记录类型EB）
//越站记录		（记录类型EC）
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
			gpsRec.stationNo = ((LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId>>4)&0x0F)*10 + (LineInfo[gGpsFeD.direct].StationInfo[gGpsFeD.flag].ucId&0x0F); //站点编号
			gpsRec.RunDirect = gGpsFeD.direct+1;//站点方向
		}
		else{
			gpsRec.stationNo = 0; //站点编号
			gpsRec.RunDirect = 0;//站点方向
		}
	}
	else{
		gpsRec.stationNo = ((LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId>>4)&0x0F)*10+(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId&0x0F);//站点编号
		gpsRec.RunDirect = RunStation.ucDirect+1;//站点方向
		if(mode == 0xE8&&distance[0]<500)//总站记录 小于500米 当第一个站
			gpsRec.stationNo=1;
		if(mode == 0xE8&&distance[1]<500)//总站记录 取最大站点
			gpsRec.stationNo=LineInfo[0].ucStationMax;
	}
	//	gpsRec.stationNo++;  //站点编号从1开始

	memcpy(gpsRec.LuduanNo, gDiaoDuinfo.overSpeedRDNO, 2);
	over_turn(2, (unsigned char*)&gpsRec.LuduanNo);
	gpsRec.MTimesUP = gDiaoDuinfo.RenCIUp;
	gpsRec.MTimesDown = gDiaoDuinfo.RenCiDown;
	if(mode==0xE7)//30分钟里程
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

	//经纬度
	memcpy(gpsRec.longitude, (unsigned char*)&gGpsInfo.Longitude, 4);
//	memcpy(gpsRec.latitude, (unsigned char*)&gGpsInfo.Latitude, 4);
	memcpy(gpsRec.BussNO,CLOUD_BUSSINESSNO,2);//
	//超速开始站点
	gpsRec.OverStartStation=gDiaoDuinfo.StarStation;
	if(mode==ID_REC_GPS_YUE)
		gpsRec.MaxSpeed=gDiaoDuinfo.MaxYueSpeed;//越站最低速度
	else
		gpsRec.MaxSpeed=gDiaoDuinfo.MaxSpeed;		//最高速度
//	gpsRec.MaxSpeed=gDiaoDuinfo.MaxSpeed;			//最高速度
	gpsRec.LimitSpeed=gDiaoDuinfo.LimitSpeed;		//限制速度
	gpsRec.OverSpeedTime=gDiaoDuinfo.limitcounter;	//超速时长
#ifdef _debug_gps_
	debugstring("GPS REC write:");
	debugdata((unsigned char*)&mode, 1, 1);
	debugdata((unsigned char*)&gpsRec, sizeof(RECORD_GPS), 1);
#endif
	WriteRecord((unsigned char*)&gpsRec, 0);				  
//以下初始化一下
	if(mode == ID_REC_GPS){//生成里程记录，把存贮器的里程记录清除
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
	debugstring("出站 2: ");
	debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
#ifdef KEY_PAD
	gKeyRestorDis = 3;
#endif
#ifdef _GPS_DIAODU_
	Clr_DiaoDuCmd_info();
#endif
	if(RunStation.ucStation == 0){
		if(RunStation.ucDirect == 0){//主站离站
			WriteGPSRec(0xE4);
		}
		else{
			WriteGPSRec(0xE6);//副站离站
		}
		SoundManage(IDSoundStartST, 0, RunStation.ucDirect);
	}
	else if((1+RunStation.ucStation) >= LineInfo[RunStation.ucDirect].ucStationMax){
#ifdef _debug_gps_
		debugstring("终点站，没有出站\r\n");
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
	debugstring("进站 1: ");
	debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
	debugdata((unsigned char*)&LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].Sstyle, 1, 1);
#endif
	
#ifdef KEY_PAD
	gKeyRestorDis = 3;
#endif
	if((1+RunStation.ucStation) >= LineInfo[RunStation.ucDirect].ucStationMax){//是终点站
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
			WriteGPSRec(0xE5);//正向，副站进站
			RunStation.ucDirect = 1;
		}
		else{
			WriteGPSRec(0xE3);//反向，主站进站
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
		debugstring("进了终点站 2，设置为始发站：");
		debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect], 4, 1);
#endif
#ifdef _GPS_NEED_REC_ //通用@zzc
		gDiaoDuinfo.limitID = 0xff; //重新找限速点
		gDiaoDuinfo.nearS=0xFF;
		memcpy(gDiaoDuinfo.limitRDNo, (unsigned char*)&SpeedInfo[RunStation.ucDirect].SpeedPInfo[0].usRDNo, 2);
		save_DD_YinYun_info();
#endif
	}
	else if(RunStation.ucStation == 0){//起始站没有进站
#ifdef _debug_gps_
		debugstring("起始站没有进站\r\n");
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
extern unsigned char SoundCMD;//语音报站任务表 一共保存8个语音 从
//越站处理  取50米范围内最小速度比较
void DealYueZan(void)
{
	//
#define  OverStation	1000
#define  MaxSpeedErr	13000//不可能达到如此高速的公交车

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
	for(i=0; i<LineInfo[RunStation.ucDirect].ucStationMax; i++){// 找最近的距离 
		ds1 = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[i].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[i].ulLongitude);
		if(ds1 <= ds2){
			ds2 = ds1;
		}
	}

//	if(ds2<60)//60米范围内速度大于20km/h速度不停 认为越站
	//distance是离起点站和终点站的距离
	if((ds2<60)&&(distance[0]>100)&&(distance[1]>100))//60米范围内速度大于20km/h速度不停 认为越站
	{

		MSG_LOG("是否越站计算:\r\n");
		BCD_LOG((unsigned char*)&gGpsInfo.velocity,4,1);
		BCD_LOG((unsigned char *)&SP1,4,1);
			//WriteGPSRec(GPS_YueZan);
#ifdef _debug_
		sprintf(disbuf,"%02d",SP1/100);
		if(SP1<MaxSpeedErr)
		miniDispstr(6,16,disbuf,0);
#endif
	
		if(gGpsInfo.velocity<=SP1)//取最小速度
		{
			
			if(cout++%2==0)//3秒钟取一次最小值
				SP1=gGpsInfo.velocity;//取最小

		}else{
	
			cout=0;
		}
	
	}
	else{//不在站点范围， 看看是否越站了
		if(SP1>OverStation&&SP1<MaxSpeedErr)//最小速度大于10km/h 认为越站了
		{
			MSG_LOG("越站了\r\n");
			BCD_LOG((unsigned char *)&SP1,4,1);
			gDiaoDuinfo.MaxYueSpeed=SP1;
			save_DD_YinYun_info();
			WriteGPSRec(GPS_YueZan);
			SP1=0xFFFF;
			Dly=0;
			gDiaoDuinfo.MaxYueSpeed=0;
			//SoundManage(IDSoundOverStation, 0, RunStation.ucDirect);
			SoundCMD|=SoundYueZ;//做标识， 在 soundCMD函数里边查找需要报的语音
	
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

	if(RunStation.ucDirect >= 2)//没找到方向，不提供限速功能
		return;
	if(SpeedInfo[RunStation.ucDirect].ucStationMax == 0)
		return;
	if(gDiaoDuinfo.REFFF!=RunStation.ucDirect)
	{
			gDiaoDuinfo.limitID=0xFF;//方向不一致  重新找
			gDiaoDuinfo.nearS=0xFF;
			gDiaoDuinfo.REFFF=RunStation.ucDirect;//保存方向  如果方向不一致  需要重新找限速站点
			save_DD_YinYun_info();
			MSG_LOG("方向不一致");
	}
	if(gDiaoDuinfo.limitID >= SpeedInfo[RunStation.ucDirect].ucStationMax){//当前超速点不正确
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
					ulSPdis[1] = ulSPdis[0];//保存最近的距离和航向
					ulSPCog[0] = ulSPCog[1];
					gDiaoDuinfo.nearS = us1;
				}
			}
#ifdef _GPS_Speed
			debugstring("找到近点:");
			debugdata((unsigned char*)&gDiaoDuinfo.nearS, 1, 1);
			debugdata((unsigned char*)&SpeedInfo[RunStation.ucDirect].ucStationMax, 1, 1);
#endif
			if((ulSPdis[0] < 20) && (ulSPCog[0] < 9000)){
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS;	//找到需要使用的限速点
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
			else{//有下一点
				ulSPCog[1] = ((gGpsInfo.uiCog+36000) - SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS].ulHxiang)%36000;
				if(ulSPCog[1] > 18000)//下一点
					ulSPCog[1] = (36000 - ulSPCog[1]);
				ulSPdis[1] = GpsDistance(SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS+1].ulLatitude, \
						SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.nearS+1].ulLongitude);
			}
			
#ifdef _GPS_Speed
			debugstring("gDiaoDuinfo.nearS:");
			debugdata((unsigned char*)&gDiaoDuinfo.nearS, 1, 1);
			debugstring("限速点0 ulSPdis[0]:");
			debugdata((unsigned char*)&ulSPdis[0], 4, 1);
			debugstring("ulSPCog[0]:");
			debugdata((unsigned char*)&ulSPCog[0], 4, 1);
			debugstring("ulSPdis[1]:");
			debugdata((unsigned char*)&ulSPdis[1], 4, 1);
			debugstring("ulSPCog[1]:");
			debugdata((unsigned char*)&ulSPCog[1], 4, 1);
#endif
			if(gDiaoDuinfo.nearS < SpeedInfo[RunStation.ucDirect].ucStationMax)//没有找到限速点 用最近的那个
			{
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS;
				p = &SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID];
				gDiaoDuinfo.LimitSpeed=p->usTIme1;//取限制速度
			}
			if((ulSPdis[0]<20)&&(ulSPCog[0] < 9000)){
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS;
#ifdef _GPS_Speed
				debugstring("到达限速点1:");
				debugdata((unsigned char*)&gDiaoDuinfo.limitID, 1, 1);
#endif
				save_DD_YinYun_info();
			}
			if((ulSPdis[1]<20)&&(ulSPCog[1] < 9000)){
				gDiaoDuinfo.limitID = gDiaoDuinfo.nearS+1;
#ifdef _GPS_Speed
				debugstring("到达限速点2:");
				debugdata((unsigned char*)&gDiaoDuinfo.limitID, 1, 1);
#endif
				save_DD_YinYun_info();
			}
		}
	} //这里正确处理
	else{//判断是否到了下一个点  
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
		if((ulSPNdis[0] < 40)&&(ulSPNCog[0] < 9000)){//到了下一个限速点
			gDiaoDuinfo.limitID++;
			gDiaoDuinfo.REFFF=RunStation.ucDirect;//保存方向  如果方向不一致  需要重新找限速站点
			save_DD_YinYun_info();
		}
		p = &SpeedInfo[RunStation.ucDirect].SpeedPInfo[gDiaoDuinfo.limitID];
//		gGpsInfo.velocity /= 10; // 把米/小时 换成 10m/小时  平台发下来的是公里*100，单位就是10m/h 在接收时处理

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
	//2016年5月6日18:15:48	@zzc
	 //老的模式
			if(us1 > p->usTIme3){//时段1
				usLIMIt = p->usNSpeed3;
			}
			else if(us1 > p->usTIme2){
				usLIMIt = p->usNSpeed2;
			}
			else if(us1 > p->usTIme1){
				usLIMIt = p->usNSpeed1;
			}
			else//比所输入的时间段都小，就取最大的时间点。
				usLIMIt = p->usNSpeed3;

		}
		else{//新的模式
				
				usLIMIt = p->usTIme1;//取限制速度
				over_turn(2,(unsigned char*)&usLIMIt);
				if(usLIMIt==0||usLIMIt>20000)//出错  默认限速60
					usLIMIt=60;
				gDiaoDuinfo.LimitSpeed=usLIMIt;//保存限制的速度
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
				if(gDiaoDuinfo.limitcounter == 10){//超速10秒后开始认定超速，当不超速时记录开始及起始时间
					if(memcmp(gDiaoDuinfo.overSpeedStart, (unsigned char*)&SysTime, 4) != 0){
						memcpy(gDiaoDuinfo.overSpeedStart, (unsigned char*)&SysTime, 7);//记录开始时间
						memcpy(gDiaoDuinfo.overSpeedRDNO, (unsigned char*)&p->usRDNo, 2);

						//memcpy((unsigned char*)&gDiaoDuinfo.StarStation,(unsigned char*)&p->usRDNo, 2);
							gDiaoDuinfo.StarStation=LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ucId;//取站点编号 BCD
							gDiaoDuinfo.StarStation = (gDiaoDuinfo.StarStation>>4)*10+(gDiaoDuinfo.StarStation&0x0F);//转成HEX
					
						if(gGpsInfo.velocity>gDiaoDuinfo.MaxSpeed)//注意MaxSpeed是short类型 取最大的速度
							memcpy((unsigned char*)&gDiaoDuinfo.MaxSpeed,(unsigned char*)&gGpsInfo.velocity,2);
						if(gDiaoDuinfo.MaxSpeed>15000)//不可能超过150km/h
							gDiaoDuinfo.MaxSpeed=0;
					//	SoundCMD|=SoundOverSpeed;//已超速
						SoundManage(IDSoundOverSpeed, 0, RunStation.ucDirect);
					}
				}
// 				else if(gDiaoDuinfo.limitcounter==4||(gDiaoDuinfo.limitcounter%8)==0){//提醒 两下子 超速
// 					//SoundManage(IDSoundOverSpeedAT, 0, RunStation.ucDirect);
// 					SoundCMD|=SoundOverSpeedAT;
// 				}
				gDiaoDuinfo.limitcounter++;

				if(gDiaoDuinfo.limitcounter > 120){	//但是一直超速，则记录  超速开始
#ifdef _GPS_Speed
					debugstring("记录超速\r\n");
					debugdata(gDiaoDuinfo.overSpeedStart, 7, 1);
					debugdata((unsigned char*)&usLIMIt, 2, 1);
#endif
					if(gDiaoDuinfo.MaxSpeed!=0)//速度不对就不要生成记录了
						WriteGPSRec(ID_REC_OverSpeed);//超速了
					
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
				debugstring("超速 :");
				debugdata(gDiaoDuinfo.overSpeedStart, 7, 1);
				debugdata((unsigned char*)&usLIMIt, 2, 1);
#endif
			}
		}
		else{//没超速，看是否有超速记录    超速结束
			if(memcmp(gDiaoDuinfo.overSpeedStart, (unsigned char*)&SysTime, 4) == 0){//有超速记录
#ifdef _GPS_Speed
				debugstring("记录超速\r\n");
				debugdata(gDiaoDuinfo.overSpeedStart, 7, 1);
				debugdata((unsigned char*)&usLIMIt, 2, 1);
#endif
				if(gDiaoDuinfo.MaxSpeed!=0)//速度不对 就不要生成记录了
					WriteGPSRec(ID_REC_OverSpeed);//超速了
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
//unsigned char GPSPCibuff[100];  使用irda_rxBuf
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
	if(GPSPCindex == 3)//长度高字节
		GPSPCcount = value;
	else if(GPSPCindex == 4){//长度低字节
		GPSPCcount *= 256;
		GPSPCcount += value;
	}
	else if(GPSPCindex >= 38){//接收完成
		memcpy((unsigned char*)&gGpsInfo, irda_rxBuf+6, sizeof(GPS_INFO));//经度
// 		memcpy((unsigned char*)&gGpsInfo.Latitude, irda_rxBuf+10, 4);//纬度
// 		memcpy((unsigned char*)&gGpsInfo.uiCog, irda_rxBuf+14, 4);//经度
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
	gDiaoDuinfo.licLON = 0;//里程时间无效
	gDiaoDuinfo.licLA = 0;//里程的经纬度清0
	gDiaoDuinfo.licJD = 0;
	gDiaoDuinfo.uiALLLiCeng = 0;
	gDiaoDuinfo.uiHallfLiCeng=0;
	gDiaoDuinfo.licCounter = 0;
}

//处理里程
extern unsigned char IF_TIME_OK(void);
void GpsLiChengDeal(void)
{
	unsigned int ultemp;
	unsigned int t=0;

	if(IF_TIME_OK()==ST_ERROR)
		return;
	if(time_invalid(gDiaoDuinfo.licengTime) == 1){//时间有效
		if((memcmp(gDiaoDuinfo.licengTime, (unsigned char*)&SysTime, 4) != 0) && (gDiaoDuinfo.uiALLLiCeng>0)){//不是当天的里程数据
			WriteGPSRec(GPS_LICHENG);
			WriteGPSRec(GPS_LiCheng30);
		}

		if(memcmp((unsigned char*)&SysTime,gDiaoDuinfo.licengTime,7)>0)//大于才减
			t=DataTimeDec((unsigned char*)&SysTime,gDiaoDuinfo.licengTime);
		MSG_LOG("里程时间:\r\n");
		BCD_LOG((unsigned char*)&gDiaoDuinfo,sizeof(gDiaoDuinfo),1);
		BCD_LOG((unsigned char*)&t,4,1);
		if(t>1800)//30分钟
			WriteGPSRec(GPS_LiCheng30);
	}
	else{
		GpsLichengClear();
	}
	
	if((gDiaoDuinfo.licLON < 73000000)||(gDiaoDuinfo.licLON > 136000000)||\
		(gDiaoDuinfo.licLA < 20000000)||(gDiaoDuinfo.licLA >  54000000)){//计算里程时的上一经纬度
#ifdef _debug_gps_
		debugstring("里程经纬度ERR!\r\n");
#endif
		gDiaoDuinfo.licLON = gGpsInfo.Longitude;
		gDiaoDuinfo.licLA = gGpsInfo.Latitude;
		gDiaoDuinfo.licJD = gGpsInfo.uiCog;
		gDiaoDuinfo.licCounter = 0;
		//继续上次的里程计算  
// 		if(time_invalid((unsigned char*)&SysTime) == 1)
// 			memcpy(gDiaoDuinfo.licengTime, (unsigned char*)&SysTime, 7);
	}
	else{
		ultemp = GpsDistance(gDiaoDuinfo.licLA, gDiaoDuinfo.licLON); //每秒计算次数:19万次
		if(ultemp > 5){//移动距离大于8米才重新加值
			gDiaoDuinfo.licLA = gGpsInfo.Latitude;
			gDiaoDuinfo.licLON = gGpsInfo.Longitude;
			gDiaoDuinfo.licJD = gGpsInfo.uiCog;
			gDiaoDuinfo.uiALLLiCeng += ultemp;

			gDiaoDuinfo.licCounter++;
			if(gDiaoDuinfo.licCounter >= 10){//每秒都加一次距离，但是10钟保存一次.
				save_DD_YinYun_info();
				gDiaoDuinfo.licCounter = 0;
			}
		}
	}
}

void Find_StationFZ(void)
{
	
	//处理距离总站的距离
	//	unsigned char ds2,ds1;
	//unsigned char station,i;
	static unsigned short cout;

	distance[0] = GpsDistance(LineInfo[0].StationInfo[0].ulLatitude, LineInfo[0].StationInfo[0].ulLongitude);//第一个站点的距离
	distance[1] = GpsDistance(LineInfo[1].StationInfo[0].ulLatitude, LineInfo[1].StationInfo[0].ulLongitude);//最后一个站点的距离
	
	if(RunStation.ucDirect<2)//已经确定方向
		return;
	//辅助起始站查找
	if((distance[0]<=100)&&(RunStation.ucDirect>1))//100米范围在总站 正向
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
//处理GPS数据
unsigned char GpsInfoDeal(void)
{
//主站点进站报站距离 20
#define _ZU_IN_ 20
//进站点进站报站距离 15
#define _IN_IN_ 15
//主站点出站报站距离 50
#define _ZU_OUT_ 20
//出站点出站报站距离 15
#define _OUT_OUT_ 15
//是否过站距离判断（报进站后，没经过出站点） 90
#define _OVERSTATION_ 90
//比较角度的有效差值 精度.00度，加大100倍
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
	unsigned int ulNextOutDis[2];//用于在找方向站点时，比较下一站的进站点距离
	char disbuf[40];
#ifdef _debug_gps_
	debugstring("GPS INFO DEAL:\r\n");
	debugdata((unsigned char*)&gGpsInfo, sizeof(GPS_INFO), 1);
#endif
//东 东经135度 2分30秒 黑龙江和乌苏里江交汇处 
//西 东经73度  40分    帕米尔高原乌兹别里山口
//南 北纬3度   52分    南沙群岛曾母暗沙
//北 北纬53度  33分    漠河以北黑龙江主航道
//海南 北纬20度

	ulDis[0] = GpsNDeal(gGpsInfo.Latitude);
	gGpsInfo.Latitude = ulDis[0];
	ulDis[0] = GpsNDeal(gGpsInfo.Longitude);
	gGpsInfo.Longitude = ulDis[0];

	if((gGpsInfo.Longitude < 73000000)||(gGpsInfo.Longitude > 136000000)||\
		(gGpsInfo.Latitude < 20000000)||(gGpsInfo.Latitude >  54000000)){
#ifdef _debug_
		debugstring("经纬度信息错误\r\n");
#endif
		return 0xF0;
	}
#ifdef _debug_
	debugdata((unsigned char*)&gGpsInfo, sizeof(GPS_INFO), 1);
#endif
	GpsLiChengDeal();//处理里程。
	Find_StationFZ();//辅助查找方向
	if(gGPsPar.EnableGps == 0){		//GPS信息被禁止
//		gGpsInfo.ucValid = GPSINVALID;
		return 0xF1;
	}

	if(LineInfo[0].ucStationMax > LineInfo[1].ucStationMax)
		flag = LineInfo[0].ucStationMax;
	else
		flag = LineInfo[1].ucStationMax;//取正反向的最大站点编号
	if((RunStation.ucDirect > 1)||(RunStation.ucStation >= flag)){//还没有确定方向或都没确定站点
//		Load_DIAODUCMD_info()  现在的设计是不需要知道方向，只要没找到站点，都是正反两边一起找，因为大部分情况都是找到方向了站点也基本确定了。
// 		sysferead(FE_GPS_DIRECT, sizeof(stGpsFED), (unsigned char*)&gGpsFeD);//取出铁电中保存的运行方向
// 		if((gGpsFeD.flag == 1)&&(gGpsFeD.direct < 2)){
// 			GetDateTime();
// 			if(memcmp((unsigned char*)&SysTime, gGpsFeD.buff, 5) != 0){//比较到小时一致，有效
// 				memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
// 			}
// 			else{//铁电信息有效
// 				RunStation.ucDirect = gGpsFeD.direct;
// 				RunStation.ucStation = 0xff;
// #ifdef _debug_gps_
// 				debugstring("铁电信息有效 方向: ");
// 				debugdata((unsigned char*)&RunStation.ucDirect,1,1);
// #endif
// 				return 1;
// 			}
// 		}
// 		else
// 			memset((unsigned char*)&gGpsFeD, 0xff, sizeof(stGpsFED));
		
		if(FindS.ucStation[0] >= LineInfo[0].ucStationMax){//---找第一个最近的点-------
			ulDis[1] = 0xffffffff;
			FindS.ulDisxx[0] = 0;
			FindS.ulDisxx[1] = 0;
			for(i=0; i<LineInfo[0].ucStationMax; i++){
				ulDis[0] = GpsDistance(LineInfo[0].StationInfo[i].ulLatitude, LineInfo[0].StationInfo[i].ulLongitude);
				if(ulDis[0] <= ulDis[1]){
					ulDis[1] = ulDis[0];
					FindS.ucStation[0] = i;
					FindS.ulDisxx[0] = ulDis[0];	//保存和最近站点的距离
				}
			}
			if(FindS.ucStation[0]+1 >= LineInfo[0].ucStationMax)//已经是终点站，没有下一站
				FindS.ulDisnn[0] = 0xffffffff;
			else
				FindS.ulDisnn[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLatitude, LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLongitude);
#ifdef _debug_gps_
			debugstring("G Find 4  ");
			debugstring("正向最近站点:");
			debugstring(LineInfo[0].StationInfo[FindS.ucStation[0]].name);
			debugdata((unsigned char*)&FindS.ucStation[0],1,1);
			debugdata((unsigned char*)&FindS.ulDisxx[0],4,1);
			debugdata((unsigned char*)&FindS.ulDisnn[0],4,1);
			debugdata((unsigned char*)&LineInfo[1].ucStationMax,1,1);
#endif
			ulDis[1] = 0xffffffff;
			for(i=0; i<LineInfo[1].ucStationMax; i++){//找反向最近站点
				ulDis[0] = GpsDistance(LineInfo[1].StationInfo[i].ulLatitude, LineInfo[1].StationInfo[i].ulLongitude);
				if(ulDis[0] <= ulDis[1]){
					ulDis[1] = ulDis[0];
					FindS.ucStation[1] = i;
					FindS.ulDisxx[1] = ulDis[0];
				}
			}
			if(FindS.ucStation[1]+1 >= LineInfo[1].ucStationMax)	//已经是终点站，没有下一站
				FindS.ulDisnn[1] = 0xffffffff;
			else
				FindS.ulDisnn[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulLatitude, LineInfo[1].StationInfo[FindS.ucStation[1]+1].ulLongitude);
#ifdef _debug_gps_
			debugstring("G Find 5  ");
			debugstring("反向最近站点:");
			debugstring(LineInfo[1].StationInfo[FindS.ucStation[1]].name);
			debugdata((unsigned char*)&FindS.ucStation[1],1,1);
			debugdata((unsigned char*)&FindS.ulDisxx[1],4,1);
			debugdata((unsigned char*)&FindS.ulDisnn[1],4,1);
#endif
			return 0;
		}//---上面找第一个最近的点-------

		ulDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]].ulLatitude, LineInfo[0].StationInfo[FindS.ucStation[0]].ulLongitude);
		ulINDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]].ulLatitudeIN, LineInfo[0].StationInfo[FindS.ucStation[0]].ulLongitudeIN);
		ulOUTDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]].ulLatitudeOUT, LineInfo[0].StationInfo[FindS.ucStation[0]].ulLongitudeOUT);
		if(FindS.ucStation[0]+1 >= LineInfo[0].ucStationMax)	//已经是终点站，没有下一站
			ulNextOutDis[0] = 0xffffffff;
		else{
			ulNextOutDis[0] = GpsDistance(LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLatitudeIN, LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulLongitudeIN);
			ulNxCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]+1].ulHxiangIN)%36000;
			if(ulNxCog[0] > 18000)//取小的差角，锐角
				ulNxCog[0] = (36000 - ulNxCog[0]);
		}
		ulINCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]].ulHxiangIN)%36000;
		if(ulINCog[0] > 18000)//取小的差角，锐角
			ulINCog[0] = (36000 - ulINCog[0]);
		
		ulOUTCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]].ulHxiangOUT)%36000;
		if(ulOUTCog[0] > 18000)//取小的差角，锐角
			ulOUTCog[0] = (36000 - ulOUTCog[0]);
		
		ulXCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[0].StationInfo[FindS.ucStation[0]].ulHxiang)%36000;
		if(ulXCog[0] > 18000)
			ulXCog[0] = (36000 - ulXCog[0]);
		
		ulDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]].ulLatitude, LineInfo[1].StationInfo[FindS.ucStation[1]].ulLongitude);
		ulINDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]].ulLatitudeIN, LineInfo[1].StationInfo[FindS.ucStation[1]].ulLongitudeIN);
		ulOUTDis[1] = GpsDistance(LineInfo[1].StationInfo[FindS.ucStation[1]].ulLatitudeOUT, LineInfo[1].StationInfo[FindS.ucStation[1]].ulLongitudeOUT);
		if(FindS.ucStation[1]+1 >= LineInfo[1].ucStationMax)	//已经是终点站，没有下一站
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
		debugstring("最近站点数据正:");
		debugstring(LineInfo[0].StationInfo[FindS.ucStation[0]].name);
		debugdata((unsigned char*)&ulDis[0],4,1);
		debugdata((unsigned char*)&ulNextOutDis[0],4,1);
		debugdata((unsigned char*)&ulXCog[0],4,1);
		debugdata((unsigned char*)&ulNxCog[0],4,1);
		debugstring("最近站点数据反:");
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

		if((FindS.ucStation[1]+1) < LineInfo[1].ucStationMax){//如果有下一站，比较是否进入了下一站
			if((ulNxCog[1]<COG_IS_OK) && (ulNextOutDis[1]<_IN_IN_)){//进了下一站了。
				RunStation.ucDirect = 1;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//算是进站
				RunStation.ucFlag = 0;
				RunStation.ucStation = FindS.ucStation[1]+1;
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("反进下站 XX: ");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
				deal_in_station();
				return 1;
			}
		}
		
		if((FindS.ucStation[0]+1) < LineInfo[0].ucStationMax){//如果有下一站，比较是否进入了下一站
			if((ulNxCog[0]<COG_IS_OK) && (ulNextOutDis[0]<_IN_IN_)){//进了下一站了。
				RunStation.ucDirect = 0;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//算是进站
				RunStation.ucFlag = 0;
				RunStation.ucStation = FindS.ucStation[0]+1;
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("正进下站 XX: ");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
				deal_in_station();
				return 1;
			}
		}
		if(ulDis[0] <= 15){//如果车在本站，比较航向来确定方向--正向
			if(ulXCog[0]<COG_IS_OK){//确定已经到了本站，方向确认
				RunStation.ucDirect = 0;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//算是出站，设进站标志
				RunStation.ucStation = FindS.ucStation[0];
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("GCOG正 确定站点：");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
				debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				deal_in_station();

				if((1+RunStation.ucStation)>=LineInfo[RunStation.ucDirect].ucStationMax){//如果是终点站，需设为始发站
					RunStation.ucDirect = 1;
					RunStation.inOrOut = 0;
					RunStation.ucOverSpeed = 0;
					RunStation.ucFlag = RS_IN_SOUND;//算是出站，设进站标志
					RunStation.ucFlag |= RS_IN_SEND;
					RunStation.ucStation = 0;
					memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
					debugstring("在终点，设为始发:");
					debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
					debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				}
				UPdataFeDirect();
				return 1;
			}
		}
		if(ulDis[1] <= 15){//如果车在本站，比较航向来确定方向--反向
			if(ulXCog[1]<COG_IS_OK){//确定已经到了本站，方向确认
				RunStation.ucDirect = 1;
				RunStation.inOrOut = 0;
				RunStation.ucOverSpeed = 0;
				RunStation.ucFlag = 0;//算是出站，设进站标志
				RunStation.ucStation = FindS.ucStation[1];
				memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
				debugstring("GCOG反 确定站点：");
				debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
				debugdata((unsigned char*)&RunStation.ucStation,1,1);
#endif
				if((1+RunStation.ucStation)>=LineInfo[RunStation.ucDirect].ucStationMax){//如果是终点站，需设为始发站
					RunStation.ucDirect = 0;
					RunStation.inOrOut = 0;
					RunStation.ucOverSpeed = 0;
					RunStation.ucFlag = RS_IN_SOUND;//算是出站，设进站标志
					RunStation.ucFlag |= RS_IN_SEND;
					RunStation.ucStation = 0;
					memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
					debugstring("在终点，设为始发:");
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
		debugstring(" 正向出站距离:");
		debugdata((unsigned char*)&ulOUTDis[0],4,1);
		debugdata((unsigned char*)&ulOUTDis[1],4,1);
		debugdata((unsigned char*)&ulOUTCog[0],4,1);
		debugdata((unsigned char*)&ulOUTCog[1],4,1);
		debugstring(" 反向出站距离:");
		debugdata((unsigned char*)&ulINDis[01],4,1);
		debugdata((unsigned char*)&ulINDis[1],4,1);
		debugdata((unsigned char*)&ulINCog[0],4,1);
		debugdata((unsigned char*)&ulINCog[1],4,1);
#endif
		if((ulOUTDis[0] < _OUT_OUT_)&&(ulOUTCog[0] < COG_IS_OK)){//正向出站
			RunStation.ucDirect = 0;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = RS_IN_SOUND;//算是出站，设进站标志
			RunStation.ucFlag |= RS_IN_SEND;
			RunStation.ucStation = FindS.ucStation[0];
			memset(FindS.ucFlag, 0, 3);
			flag = 0x55;
		}
		else if((ulOUTDis[1] < _OUT_OUT_)&&(ulOUTCog[1] < COG_IS_OK)){//反向出站
			RunStation.ucDirect = 1;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = RS_IN_SOUND;//算是出站，设进站标志
			RunStation.ucFlag |= RS_IN_SEND;
			RunStation.ucStation = FindS.ucStation[1];
			memset(FindS.ucFlag, 0, 3);
			flag = 0x55;
		}
		else if((ulINDis[0] < _OUT_OUT_)&&(ulINCog[0] < COG_IS_OK)){//正向进站
			RunStation.ucDirect = 0;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = 0;//算是进站
			RunStation.ucFlag = 0;
			RunStation.ucStation = FindS.ucStation[0];
			memset(FindS.ucFlag, 0, 3);
			flag = 0xaa;
		}
		else if((ulINDis[1] < _OUT_OUT_)&&(ulINCog[1] < COG_IS_OK)){//反向进站
			RunStation.ucDirect = 1;
			RunStation.inOrOut = 0;
			RunStation.ucOverSpeed = 0;
			RunStation.ucFlag = 0;//算是进站
			RunStation.ucFlag = 0;
			RunStation.ucStation = FindS.ucStation[1];
			memset(FindS.ucFlag, 0, 3);
			flag = 0xaa;
		}
		if(flag==0x55){
#ifdef _debug_gps_
			debugstring("出站 XX: ");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
			deal_out_station();
			return 1;
		}
		else if(flag == 0xaa){//到达进站点
#ifdef _debug_gps_
			debugstring("进站 XX: ");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
#endif
			deal_in_station();
			return 1;
		}

		flag = 0;
		//出错判断,正向和本站及下站都越来越远
		if((ulDis[0] > FindS.ulDisxx[0])&&(ulNextOutDis[0] > FindS.ulDisnn[0])){
			flag |= 0xF0;//正向出错
		}
		if((ulDis[0] > FindS.ulDisxx[0])&&(ulNextOutDis[0] > FindS.ulDisnn[0])){
			flag |= 0x0F;//反向也出错
		}
		if(flag == 0xFF){//全都错误，重新开始
#ifdef _debug_gps_
			debugstring("方向全错 \r\n");
#endif
			ClrRunGPSData();
			ClrFindData();
			return 0;
		}
		
		if(ulDis[0] >= 2000){
#ifdef _debug_gps_
			debugstring("最近的下站太远了 :");
			debugdata((unsigned char*)&ulDis[0], 4, 1);
#endif
			ClrRunGPSData();
			ClrFindData();
			return 1;
		}
		///////////////////////////上面是当在某站点时，能过航向来确定/////////////
	}//上面没确定方向
	else{//方向确定，站点确定
//		debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);

#ifdef _GPS_NEED_REC_
		Deal_Speed();//处理超速问题
		DealYueZan();//越站问题处理
#ifdef _TTSSound_enable_
		//查找是否需要报
		SoundAtantionCmd();//查找需要报的语音
#endif
#endif

#ifdef _debug_gps_
		debugstring("下站: ");
		debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
		debugdata((unsigned char*)&RunStation.ucDirect,1,1);
		debugdata((unsigned char*)&RunStation, sizeof(stRunStation), 1);
		debugdata((unsigned char*)&LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude,4,1);
		debugdata((unsigned char*)&LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude,4,1);
#endif
		ulDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
		ulINDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitudeIN, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitudeIN);
		ulOUTDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitudeOUT, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitudeOUT);
		if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangOUT != 0){//如果航向为0，认为是错误的，就是没有航向。
			ulOUTCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangOUT)%36000;
			if(ulOUTCog[0] > 18000)//取小的差角，锐角
				ulOUTCog[0] = (36000 - ulOUTCog[0]);
		}
		else
			ulOUTCog[0] = 0;
		if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangIN != 0){//如果航向为0，认为是错误的，就是没有航向。
			ulINCog[0] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulHxiangIN)%36000;
			if(ulINCog[0] > 18000)//取小的差角，锐角
				ulINCog[0] = (36000 - ulINCog[0]);
		}
		else
			ulINCog[0] = 0;

		if(RunStation.ucStation+1 >= LineInfo[RunStation.ucDirect].ucStationMax){//是最后一站
			ulNextOutDis[0] = 0xffffffff;
		}
		else{
			ulNextOutDis[0] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulLatitudeIN, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulLongitudeIN);
			if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN != 0){//如果航向为0，认为是错误的，就是没有航向。
				ulINCog[1] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN)%36000;
				if(ulINCog[1] > 18000)//取小的差角，锐角
					ulINCog[1] = (36000 - ulINCog[1]);
			}
			else
				ulINCog[1] = 0;
		}
		
		if(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN != 0){//如果航向为0，认为是错误的，就是没有航向。
			ulCog= ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation+1].ulHxiangIN)%36000;
			if(ulCog > 18000)//取小的差角，锐角
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
//		if(ulDis[0] >= 3000){//距离太远
// 			ClrRunGPSData();
// 			ClrFindData();
// 			ClrGpsFED();
// 			if(gGpsDisEN == 'G'){
// 				display(6, 10, "太远", 0);
// 			}
// 			return 1;
//		}

//		if(((ulDis[0] <= _ZU_IN_)||(ulINDis[0] <= _IN_IN_)) && (RunStation.ucFlag == 0) && ((ulINCog[0] < COG_IS_OK)||(ulCog<COG_IS_OK))){//进站
		if(((ulDis[0] <= _ZU_IN_)||(ulINDis[0] <= _IN_IN_)) && (RunStation.ucFlag == 0)){
			deal_in_station();
			UPdataFeDirect();
			if(gGpsDisEN == 'G'){
				display(6, 10, "进站", 0);
			}
			
			if(ulDis[0] < 20){
				RunStation.ucFlag |= RS_IN_MAIN;
			}
			goto DEAL_IN_END;  //进站处理完成
		}
		
		if(ulDis[0]>FindS.ulDisxx[RunStation.ucDirect])
			ultemp = ulDis[0] - FindS.ulDisxx[RunStation.ucDirect];
		else
			ultemp = FindS.ulDisxx[RunStation.ucDirect] - ulDis[0];
		if(ultemp < 3){
#ifdef _debug_gps_
			debugstring("Runs 小于10米\r\n");
#endif
			return 1;		//小于10米和移动不处理。
		}
		if(RunStation.ucFlag == 0){ //没进站也没出站 关断是否到了出站点了
			if(ulOUTDis[0] < _OUT_OUT_){//出站
				deal_out_station();
				UPdataFeDirect();
#ifdef _debug_gps_
				debugstring("出站only!\r\n");
#endif
				if(gGpsDisEN == 'G'){
					display(6, 10, "出站", 0);
				}
				goto DEAL_IN_END;  //进站处理完成
			}
		}
		//出站判断处理(已经报了进站，但是没报出站),需要先判断是否进了主站位置，然后再判断是否出站
		else if(((RunStation.ucFlag & RS_IN_SOUND) != 0) && ((RunStation.ucFlag & RS_OUT_SOUND) == 0)){
			if((RunStation.ucFlag & RS_IN_MAIN) != 0){//已经进入了主站
				if(((ulDis[0] > _ZU_OUT_) && (ulDis[0] < 120))||(ulOUTDis[0] < _OUT_OUT_)){//出站
					deal_out_station();
					UPdataFeDirect();
#ifdef _debug_gps_
					debugstring("出站main\r\n");
#endif
					if(gGpsDisEN == 'G'){
						display(6, 10, "出站", 0);
					}
					goto DEAL_IN_END;  //进站处理完成
				}
			}
			else{//还没有经过主站（中心站）
				if(ulDis[0] < 20){
#ifdef _debug_gps_
					debugstring("到达中心点\r\n");
#endif
					if(RunStation.ucStation == 0){//如果是起始点，不设置到中心点位置，一定要到发车点
						RunStation.ucFlag = RS_IN_SOUND;
					}
					else
						RunStation.ucFlag |= RS_IN_MAIN;
				}
				else if((ulOUTDis[0] < _OUT_OUT_)&&(ulOUTCog[0] < COG_IS_OK)){//进到了进站点,而且方向一致
					deal_out_station();
					UPdataFeDirect();
#ifdef _debug_gps_
					debugstring("不过中心点出站\r\n");
#endif
					if(gGpsDisEN == 'G'){
						display(6, 10, "出站", 0);
					}
					goto DEAL_IN_END;  //进站处理完成
				}
			}
		}

		if((ulNextOutDis[0] < _IN_IN_) && (ulINCog[1] < COG_IS_OK)){//进入了下一站,说明上一站有问题，越站，或出站没报
			RunStation.inOrOut = 3;
			RunStation.ucFlag = 0;
			RunStation.ucStation++;							//站点指向下一站
			memset(FindS.ucFlag, 0, 3);
#ifdef _debug_gps_
			debugstring("进到了下一站:");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
			debugstring("\r\n");			
#endif
			deal_in_station();
			UPdataFeDirect();
			if(gGpsDisEN == 'G'){
				display(6, 10, "进站", 0);
			}
			goto DEAL_IN_END;  //进站处理完成
		}
		if(((RunStation.ucFlag & RS_IN_SOUND) != 0) && (ulDis[0] > _OVERSTATION_)){   //报了进站，己经过站
#ifdef _debug_gps_
			debugstring("G P 3: ");
#endif
			RunStation.inOrOut = 3;
			RunStation.ucFlag = 0;
			RunStation.ucStation++;							//站点指向下一站
			memset(FindS.ucFlag, 0, 3);
			FindS.ulDisxx[RunStation.ucDirect] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
#ifdef _debug_gps_
			debugstring("过站了，指向下一站: ");
			debugstring(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].name);
			debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect],4,1);
#endif
			if(gGpsDisEN == 'G'){
				display(6, 10, "过站1", 0);
			}
		}
		else{
#ifdef _debug_gps_
			debugstring("G P 5: ");
			debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect], 4, 1);
#endif		
			if((ulOUTDis[0] < _OUT_OUT_)&&(ulOUTCog[0] < COG_IS_OK)) {//到达本站的出站点，指向下一站
				deal_out_station();
				UPdataFeDirect();
#ifdef _debug_gps_
				debugstring("进站没报出站\r\n");
#endif
				if(gGpsDisEN == 'G'){
					display(6, 10, "出站", 0);
				}
				goto DEAL_IN_END;
			}

			if(ulDis[0] > FindS.ulDisxx[RunStation.ucDirect]){//离本站越来越远
				if(ulDis[0] > 90){
					if(RunStation.ucStation > 0)//在起始站不加
						FindS.ucFlag[RunStation.ucDirect]++;//越来越远次数加1，不用比效下一站，到达下一站会提示
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

			if(FindS.ucFlag[RunStation.ucDirect] > 120){//离站点越来越远 6 持续两分钟
				ClrRunGPSData();	//重新确定站点
				ClrFindData();
				ClrGpsFED();
#ifdef _debug_gps_
				debugstring("G P 61重新寻找站点方向: ");
#endif
				return 1;
			}
#ifdef _debug_gps_
			debugstring("G P 62 RunStation.inOrOut = 3?\r\n");
#endif
			RunStation.inOrOut = 3;
			RunStation.ucOverSpeed = 0;
		}
		//下面是找最近站点
		ultemp1 = 0xffffffff;
		FindS.ulDisxx[RunStation.ucDirect] = 0xffffffff;	//找此方向最近的点，如果最近的站点比当前站点大并距离近则重新置站点
		for(i=0; i<LineInfo[RunStation.ucDirect].ucStationMax; i++){
			ultemp = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[i].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[i].ulLongitude);
			if(ultemp <= ultemp1){
				ultemp1 = ultemp;
				FindS.ucStation[RunStation.ucDirect] = i;
				FindS.ulDisxx[RunStation.ucDirect] = ultemp;	//保存和最近站点的距离
			}
		}
		ulXCog[RunStation.ucDirect] = ((gGpsInfo.uiCog+36000) - LineInfo[RunStation.ucDirect].StationInfo[FindS.ucStation[RunStation.ucDirect]].ulHxiang)%36000;
		if(ulXCog[RunStation.ucDirect] > 18000)
			ulXCog[RunStation.ucDirect] = (36000 - ulXCog[RunStation.ucDirect]);
		
		ultemp1 = 0xffffffff;
		flag = (RunStation.ucDirect^0x01);
		FindS.ulDisxx[flag] = 0xffffffff;	//找反方向最近的点，如果最近的站点比当前站点大并距离近则重新置站点
		for(i=0; i<LineInfo[flag].ucStationMax; i++){
			ultemp = GpsDistance(LineInfo[flag].StationInfo[i].ulLatitude, LineInfo[flag].StationInfo[i].ulLongitude);
			if(ultemp <= ultemp1){
				ultemp1 = ultemp;
				FindS.ucStation[flag] = i;
				FindS.ulDisxx[flag] = ultemp;	//保存和最近站点的距离
			}
		}
		ulXCog[flag] = ((gGpsInfo.uiCog+36000) - LineInfo[flag].StationInfo[FindS.ucStation[flag]].ulHxiang)%36000;
		if(ulXCog[flag] > 18000)
			ulXCog[flag] = (36000 - ulXCog[flag]);
#ifdef _debug_gps_
		debugstring("G Find 4  ");
		debugstring(" 此方向最近站点:");
		debugstring(LineInfo[RunStation.ucDirect].StationInfo[FindS.ucStation[RunStation.ucDirect]].name);
		debugdata((unsigned char*)&FindS.ucStation[RunStation.ucDirect],1,1);
		debugdata((unsigned char*)&FindS.ulDisxx[RunStation.ucDirect],4,1);
		debugdata((unsigned char*)&FindS.ucFlag[RunStation.ucDirect],1,1);
		debugdata((unsigned char*)&ulXCog[RunStation.ucDirect],4,1);
		debugstring(" 反方向最近站点:");
		debugstring(LineInfo[flag].StationInfo[FindS.ucStation[flag]].name);
		debugdata((unsigned char*)&FindS.ucStation[flag],1,1);
		debugdata((unsigned char*)&FindS.ulDisxx[flag],4,1);
		debugdata((unsigned char*)&FindS.ucFlag[flag],1,1);
		debugdata((unsigned char*)&ulXCog[flag],4,1);
#endif
		if(((FindS.ulDisxx[flag] < _ZU_IN_)&&(ulXCog[flag] < COG_IS_OK)) &&
			((((1+RunStation.ucStation) < LineInfo[RunStation.ucDirect].ucStationMax)&&(RunStation.ucStation > 0))
			||(((1+FindS.ucStation[flag]) < LineInfo[flag].ucStationMax)&&(FindS.ucStation[flag] > 0)))){//如果反向到了,并且不是终点或起点，中途返回？去重新找站点。
			ClrRunGPSData();	//重新确定站点
			ClrFindData();
			ClrGpsFED();
#ifdef _debug_gps_
			debugstring(" 中途返回 ?重新寻找站点方向\r\n");
#endif
			return 1;
		}
		//如果最近站点的距离比当前站点更近，并且车离本站也越来越远 
		else if((FindS.ucStation[RunStation.ucDirect] > RunStation.ucStation) && 
			(((FindS.ucFlag[RunStation.ucDirect] > 5)&&(ulDis[0] > FindS.ulDisxx[RunStation.ucDirect]))||
			((FindS.ulDisxx[RunStation.ucDirect] < (_ZU_IN_+_ZU_IN_))&&(ulXCog[RunStation.ucDirect] < COG_IS_OK)))){//或者已经到达这个最近站，而且方向一到致
#ifdef _debug_gps_
			debugstring(" 真过了!\r\n");
#endif
			RunStation.inOrOut = 3;
			RunStation.ucFlag = 0;
			RunStation.ucStation = FindS.ucStation[RunStation.ucDirect]; //站点指向最近的站点。
			memset(FindS.ucFlag, 0, 3);
			FindS.ulDisxx[RunStation.ucDirect] = GpsDistance(LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLatitude, LineInfo[RunStation.ucDirect].StationInfo[RunStation.ucStation].ulLongitude);
			if(gGpsDisEN == 'G'){
				display(6, 10, "过站2", 0);
			}
		}

DEAL_IN_END:
		FindS.ulDisxx[RunStation.ucDirect] = ulDis[0];
		return 1;	
	}
	return 1;
	//主站点进站报站距离
#undef _ZU_IN_
	//进站点进站报站距离
#undef _IN_IN_
	//主站点出站报站距离
#undef _ZU_OUT_
	//出站点出站报站距离
#undef _OUT_OUT_
	//是否过站距离判断（报进站后，没经过出站点）
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
		memcpy(gGpsCmdBuf.GGA, "$GPGGA,", 7);//在中断接收里面GPS信息标识没有保存
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
void writeGpsTradRec(void)//记录车移动轨迹，用于调试的数据
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
extern unsigned char Keybuffer[10];			//和按键输入票价共用BUFF
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
	//小数点不能超过两个
	
	for(i=0;i<6;i++)
	{
		if((Keybuffer[5-i]&0x80)==0x80)
			a++;
	}
	//寻找小数点的位置
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
		if(RunStation.ucDirect >= 2){//站点信息不对
			beep(2,50,50);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			GPS_Ex_KeyDis(0,Keybuffer);
			LedDisplay(Keybuffer);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			return ST_OK;
		}
		else if(KeyVa > LineInfo[RunStation.ucDirect].ucStationMax)//输入值大于总站点数，错误
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
	if(kkvv==0x0d)	//确定键
	{
		beep(1,50,25);
		if(KeyVa==0)	//0号站点？无效, 重报当前站点
		{
			if((RunStation.ucDirect >= 2)||(RunStation.ucStation > LineInfo[RunStation.ucDirect].ucStationMax)){
				beep(2,50,50);					//没有方向和站点，需要输入
				GPS_Ex_KeyDis(0,Keybuffer);
				LedDisplay(Keybuffer);
				memset(Keybuffer,0x20,6);
				Keybuffer[5]='0';
				
				return ST_OK;
			}
			else{//已经有了，可以重报一次
				if((RunStation.ucFlag & RS_IN_SOUND) != 0)//已经报进站
					deal_out_station();
				else{
					deal_in_station();
#ifndef _TTSSound_enable_	//如果没有语音，需要指向下一站了[就是马上出站],所以按一次就行了
					deal_out_station();
#endif
				}
			}
		}
		else
		{
			if(RunStation.ucDirect >= 2)
				RunStation.ucDirect = 0;
			if(RunStation.ucStation != (KeyVa-1)){//当前站点和输入站点不一致，清除状态
				memset(FindS.ucFlag, 0, 3);
				RunStation.inOrOut = 0;
				RunStation.ucFlag = 0;
				RunStation.ucPeStation = RunStation.ucStation;
			}

			RunStation.ucStation = (KeyVa-1);
#ifdef _TTSSound_enable_
			if((RunStation.ucFlag & RS_IN_SOUND) != 0)//已经报进站
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
	else if(dat==0x08)//清除键
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
	else if(dat==0x0d)//确定键
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
	}//  41=F1键  42=F2键
	else if(dat == 0x41){//正向，反向
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
			save_fe_gpsdata();//保存运行参数到铁电
		}
	}
	return ST_OK;
}
#endif

//GPS 主处理程序
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
	if(gGpscand){//有定位信息GPS数据接收完成
		if(dealGPS() == ST_ERROR)
			;//return;//GPS数据无效
	}
	else
		return;
#endif
	if(gGpsInfo.ucValid == GPSVALID){
#ifdef _debug_gps_
		debugstring("GPS时间：");
		debugdata((unsigned char*)&gGpsInfo.UTCtime, 7, 1);
		sprintf((char*)buf, "纬度：%u\r\n\x0",gGpsInfo.Latitude);
		debugstring((char*)buf);
		sprintf((char*)buf, "经度：%u\r\n\x0",gGpsInfo.Longitude);
		debugstring((char*)buf);
		debugstring("速度：");
		debugdata((unsigned char*)&gGpsInfo.velocity,4,1);
		debugstring("卫星数量：");
		debugdata((unsigned char*)&gGpsInfo.SataletNum,1,1);
		debugstring("状态：");
		debugdata((unsigned char*)&gGpsInfo.ucState,1 ,1);
#endif
		memcpy((unsigned char*)&SysTime.year_h, (unsigned char*)&gGpsInfo.UTCtime, 7);
		if(time_invalid((unsigned char*)&SysTime.year_h) != 0)
			gpsTimeOK = 0xe5;//GPS时间有效
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
		writeGpsTradRec();//记录车移动轨迹，用于调试的数据
		gGpsInfo.ucValid = GPSINVALID;
		return;
#endif 

		gGpsInfo.ucValid = GPSUES;

		uct = GpsInfoDeal();
		if(uct != 0xF0)	//经维度信息错误，不处理。
#ifdef FengDuan_BU_

			dealFengDuanStation();
#endif
		if(uct < 0xF0)
			save_fe_gpsdata();//保存运行参数到铁电
//		if((GPRSLinkProcess < TCPSTARTSTAT+6)||(GPRSLinkProcess == 0xb1)){	//没登陆上
		if(RunStation.inOrOut == 2){	//出站处理完成，指向了下一站点
			;
		}
		else if(RunStation.inOrOut == 1){	//进了终点站
			;
		}
	}
}

//GPS处理主函数
void main_Gps(void)
{
	mainGps();
}

#endif
