/*
分段收费相关程序
时间：2012-1-16
编写：LiKS.

  2015.10.14日更改协议，函数的尾有"_QM"的是只能前门机用的函数，函数尾有"_HM"的只是后门机使用的函数，"_ALL"的是都会用到的函数。
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "..\include\include.h"
#include "szct.h"

#ifdef FengDuan_BU_

#define FDCMD_VERCX			0x01	//版本查询
#define FDCMD_VERCX_ECHO	0x81	//版本查询的应答

#define FDCMD_GPSINFO		0x02	//查询前门信息
#define FDCMD_GPSINFO_ECHO	0x82	//前门应答信息

#define FDCMD_DOWNPar		0x03	//下载数据，具体是线路信息，票价信息或程序文件数据要看数据中的标识
#define FDCMD_DOWNPar_ECHO	0x83	//前门应答数据命令。

#define FDCMD_UPDateREC		0x04	//下载数据，具体是线路信息，票价信息或程序文件数据要看数据中的标识
#define FDCMD_UPDateREC_ECHO	0x84	//前门应答数据命令。


typedef struct  {
	unsigned char ucStation;			//当前站台号
	unsigned char ucPeStation;			//前一站台号
	unsigned char inOrOut;				//进站出站标志(1进2出)
	unsigned char ucOverSpeed;			//是否超速 正常00超速0C
	unsigned char ucDirect;				//方向 正0 负1
	unsigned char ucFlag;				//标志，是否报站低4字节，传数高4字节
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
extern GPS_INFO gGpsInfo;		//GPS信息,应用程序判断的缓冲
extern stGpsPar gGPsPar;				//基本信息
extern stBusVerIfo gBusVerInfo;
extern Parameter2 POINT_MESSAGE;
extern unsigned char gMCardCand;
extern unsigned char StufferNO[4];
extern Parameter3 ComSumeFile;
extern unsigned char cardSound;			//刷卡没有成功标志
extern unsigned int card_ser;
extern unsigned int a_sum1,s_sum1;
extern unsigned char DriveCardNo[4];
extern unsigned char restore_flag;
extern stcomtime SysTime;
extern Parameter5 cardMessage;
extern unsigned int c_serial;
extern pFistVary_1 pFistVary;
extern stRunStation RunStation;				//运行中的站点信息
extern stLineInfo LineInfo[2];//线路信息定义包括站点信息 正反两组0组 正向 1组反向

unsigned char gFDderict;
stFDLineHead gFdLinehead;
unsigned char gFDStaIndex[2];	//当前车载机站点序列号从0开始,和站点编号不一样.
unsigned char gFDucID;			//从1开始的站点序号,最终确定好的站点序号在此，站点编号通过方向和这个序号从线路信息中查找。
unsigned char gZNFDucID[2];		//从1开始的正反向站点序号（前门机有效），在未确定站点时需要此值，不然反向显示会错误
								//在算票价时代入的值都是从1开始的编号。写入卡里的或传给后门机的序号都是从1开始的。
unsigned char gOlderStation = 0xFF;
unsigned char offstion;//分段上车的位置
//---------------------------------------------
//通讯进程定义
#define FDProcess_GETPriceRate 0x7E		//同步票价折扣信息。
#define FDProcess_GETFind 0x7F			//查找任务状态。除了查找新的任务，不能有任何动作。如取得前门机版本信息后，要进入此状态。
#define FDProcess_GETVER 0x80			//取程序版本号 
#define FDProcess_GETGPSLineInfo 0x81	//同步线路信息中
#define FDProcess_GETFDticket 0x82		//同步阶梯票中	---------这进度及以上进度没有完成，不能刷卡消费。
#define FDProcess_GETProgram 0x83		//同步程序中
#define FDProcess_GETFree 0x8F			//正常状态 ，可以取站点信息 和 上传记录。

//协议头定义，各字段的偏移量
#define FDCMDXY_head 0
#define FDCMDXY_LEN 4
#define FDCMDXY_CMD 6
#define FDCMDXY_Serial 7
#define FDCMDXY_DATA 8

//定义设备类型，前后门用同一个程序，通过是否具有GPS模块来确认是前门机还是后门机。
#define FDMODE_GPS 0xA0
#define FDMODE_NORMAL 0x5E
unsigned char gFDMode;	//标识分段收费中的设备类型，0xA1为前门机（具有GPS，GPRS），0xA2为后门机。通过检测GPS模块确认。

//CAN通讯长度300字节。实际最大只有268字节。
#define CAN_DATA_MAX_LEN 300
unsigned char CanRevbuf[CAN_DATA_MAX_LEN];
volatile unsigned int gCanInex=0;
volatile unsigned char gCanRevFlag=0;	//0xAA时有数据。 

#define FDRUNSTATUES_PJ 0x01		//票价错误
#define FDRUNSTATUES_TX (0x01<<1)	//CAN通讯错误，后门机取不到最新的点编号了。
#define FDRUNSTATUES_GPS (0x01<<2)	//GPS信号错误，前门机GPS不正常。
#define FDRUNSTATUES_Line (0x01<<3)	//站点信息错。
unsigned char gFDRunStatues;	//设备状态是否有效，0为有效，其它为无效(上面是错误状态，按位计)，无效时需要禁止刷卡。
unsigned char gFDCmdSerial=1;	//通讯序号.从1开始。
unsigned int gFDCanError=0;		//CAN通讯错误计数。

#define FDSCountMax	10			//最大的通讯间隔时间，单位-秒。
unsigned int gFDScount=0;		//发送超时计数据，=0空闲
#define FDGetGPSinfoMax 4
unsigned int gFDGetGPSinfo=0;	//后门机取前门站点信息。

#define FDMISS_NULL 0
#define FDMISS_GETLine (0x01)		//需要更新线路信息
#define FDMISS_GETTicket (0x01<<1)	//需要更新阶梯票价
#define FDMISS_GETProgram (0x01<<2)	//需要更新程序 
#define FDMISS_GETPriceRate (0x01<<3)	//需要票价折扣
unsigned char gFDmissflag;//任务标志,按位指示有哪些任务需要执行。只有后门机才会用到
unsigned char gFDdealProcess;//任务进程，只有后门机才会用到
struct  
{
	unsigned char DownFlag;			//下载文件标识.
	unsigned char DownPerSerial[2];	//下载的前一序号。
	unsigned char DownSerial[2];		//当前的包序号。
}FDDownPar;

#define FDParChange_Line 0x01			//线路
#define FDParChange_Device (0x01<<1)	//设备号
#define FDParChange_Piaojia (0x01<<2)	//票价
#define FDParChange_Program (0x01<<4)	//前门程序
#define FDParChange_Restart 0x80		//前门机刚启动,前门机开机后，需要后门机也要进行重启(或同步版本)，以便同步数据。
unsigned char gFDParChange=0;	//参数是否变化，0无变化，其它有变化

extern volatile unsigned char gGpscand;//通过此变量判断是否有GPS模块。没有是后门机，有就是前门机。


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

	ret = can_rev(buf);	//一次最多收8个字节

	if(gCanRevFlag ==0xAA){//有数据没处理
		if((CanRevbuf[FDCMDXY_CMD] == FDCMD_VERCX)||(CanRevbuf[FDCMDXY_CMD] == FDCMD_GPSINFO)){//如果上条是查询信息的，可以丢弃，优先其它命令
			clrCanRevbuf();
		}
		else{
			return;
		}
	}
	if(ret == 0)
		return;

	if(gCanInex+ret > CAN_DATA_MAX_LEN){//超长
		clrCanRevbuf();
		return;
	}
	else{
		memcpy(CanRevbuf+gCanInex, buf, ret);
		gCanInex += ret;

		if(CanRevbuf[0] != '$'){//第一个字节必须是$
			debugstring("canCMD headERROR!\r\n");
			debugdata(CanRevbuf, gCanInex, 1);
			clrCanRevbuf();
			return;
		}
		if(gCanInex < 8){//收到数据,解析包及命令
			return;
		}
				
		if(memcmp(CanRevbuf, "$$$$", 4) != 0){//数据不对
			debugstring("canCMD headERROR!\r\n");
			debugdata(CanRevbuf, gCanInex, 1);
			clrCanRevbuf();
			return;
		}
		memcpy((unsigned char*)&ust, CanRevbuf+4, 2);//数据长度
		if(ust > CAN_DATA_MAX_LEN){
			debugstring("canCMD LEN-ERROR!\r\n");
			clrCanRevbuf();
			return;
		}
		
		if(gCanInex >= ust){//已经收完一个包
			//MSG_LOG("CAN 收完::ust=%d, gCanInex=%d\r\n", ust, gCanInex);
			//BCD_LOG(CanRevbuf, gCanInex, 1);
			ret = 0;
			for(i=0; i<ust-1; i++){
				ret ^= CanRevbuf[i];
			}
			if(ret != CanRevbuf[ust-1]){//校验错 24242424090002080B
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

//组织的数据发送，长度不超过255字节。所修改的数据直接放在Sdate缓存中。
int FDCMDData_Send(unsigned char Scmd, unsigned char *Sdata, unsigned int Sdatalen)
{
	unsigned char ret=0;
	unsigned short i;
	unsigned short index;
	
	if(Sdatalen > 0){
		memmove(Sdata+FDCMDXY_DATA, Sdata, Sdatalen);	//把数据移到数据区，第8字节开始.
	}

	index = 0;
	memset(Sdata+index, '$', 4);
	index += 4;
	i = Sdatalen+9;
	memcpy(Sdata+index, (unsigned char*)&i, 2);	//数据长度.数据长度+4+2+1+1+1
	index += 2;
	Sdata[index++] = Scmd;
	Sdata[index++] = gFDCmdSerial++;
	if(gFDCmdSerial == 0) gFDCmdSerial++;

	index += Sdatalen;//数据已经移到此位置，把指针加

	Sdata[index] = 0;	//计算校验
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
	gFDScount = FDSCountMax;	//等待应答。

	MSG_LOG("CandFDCmd_snd end ret=%04X, gFDCanError = %d\r\n", ret, gFDCanError);
#endif

	return ret;
}


//取CPU中的分段机类型.
unsigned char getCpuFdMode(void)
{
	unsigned char buf[8];
	stCpuInfo cpuinfo;
	unsigned int offset;
	
	offset = ((unsigned char*)&cpuinfo.FDModeFlag)-((unsigned char*)&cpuinfo);
	cpuInfoRead(buf, offset, 1);
	
	if((buf[0] == FDMODE_GPS) ||(buf[0] == FDMODE_NORMAL)){
		gFDMode = buf[0];	//有设置，给值，没有不给值
	}

	MSG_LOG("get CPUFDMODE:%02X\r\n", buf[0]);
	return buf[0];
}
//在CPUINFO中设置设备的类型。此设置优先判断
void SETCpuFDMode(void)
{
	unsigned int odly=0;
	unsigned char disflag, newFDMODE,rkey;

	newFDMODE = gFDMode;

	cls();
	display(0, 0, "设置设备类型", DIS_CENTER);
	disflag = 1;

	while(1){
		if(disflag == 1){
			disflag = 0;
			if(newFDMODE == FDMODE_NORMAL){
				display(2, 1, " 1.前门机 ", 0);
				display(4, 1, ">2.后门机 ", DIS_CONVERT);
			}
			else if(newFDMODE == FDMODE_GPS){
				display(2, 1, ">1.前门机 ", DIS_CONVERT);
				display(4, 1, " 2.后门机 ", 0);
			}
			else{
				display(2, 1, " 1.前门机 ", 0);
				display(4, 1, " 2.后门机 ", 0);
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
				display(6, 0, "设置成功,重启!", DIS_CENTER);
				delayxms(800);
				ReStartCPU();
				restore_flag = 3;
				return;
			}
			return;
		}
	}
}

//开机后在系统初始化（SystemInit）之前需要进行调用，来确认设备类型。
void FD_Check_devMode(void)
{
	unsigned char i = 0;
	
	gGpscand = 0;
	gFDMode = 0;
	cls();
	display(0, 0, "硬件检测", DIS_CENTER);
	
	i = getCpuFdMode();
	if((i != FDMODE_GPS) &&(i != FDMODE_NORMAL)){
		i = 0;
		while (1)
		{
			if((gGpscand & 0x0f) != 0){//说明有GPS模块
				gFDMode = FDMODE_GPS;
				break;
			}
			delayxms(10);
			if(i++ > 200){	//GPS信息一秒输出一次，致少要等2秒。
				gFDMode = FDMODE_NORMAL;
				break;
			}
		}

		MSG_LOG("GFDMODE = %02X\r\n", gFDMode);
		if((gFDMode != FDMODE_GPS) && (gFDMode != FDMODE_NORMAL)){
			display(6, 0, "无法识别设备类型", 0);	//正常流程这里是不会进来的。
			while(1)
				clr_wdt();
		}
	}
	CAN_Initiation();	//初始化CAN总线
	clrCanRevbuf();
}

//此函数读取数据SPI FLASH中的数据。64字节一组。
unsigned int FDFlashDataRead_QM(unsigned int heaAddr, unsigned char *Dbuf)
{
	unsigned int addr, oi=0;
	unsigned short offset;

	Dbuf[0] = FDDownPar.DownFlag;
	oi++;
	memcpy(Dbuf+oi, FDDownPar.DownSerial, 2);
	oi+=2;

	memcpy((unsigned char *)&offset, FDDownPar.DownSerial, 2);	//偏移量
	
	addr = heaAddr+(offset*64);
	flashread(addr, Dbuf+oi, 64);
	
	oi += 64;

	return oi;
}

//此函数读取CPU FLASH中的程序数据。256字节一组。
unsigned int FDProDataRead_QM(unsigned char *Dbuf)
{
#define FBootCodeSize    0x4800  //BootLoader大小16K(0x4000)  后面2K(0x800)用于存车载机序列号等．
#define FMCURsetAddress  0x08000000   //MCU复位后程序运行的起始地址
#define FApplicationAddress    (FMCURsetAddress+ FBootCodeSize)

	unsigned int addr, oi=0;
	unsigned short offset;
	
	Dbuf[0] = FDDownPar.DownFlag;
	oi++;
	memcpy(Dbuf+oi, FDDownPar.DownSerial, 2);
	oi+=2;
	
	memcpy((unsigned char *)&offset, FDDownPar.DownSerial, 2);	//偏移量
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
//处理回应的数据
unsigned int FDDealDownData_HM(unsigned char *rbu)
{//242424244C0083 04 8100000000000006000100000001000000000000003406000000000000000000000000000000000000000000000000000000000000000001000100000001000000010E70
	unsigned int Waddr,wlen,wof;
	unsigned short ofset;
	unsigned char i;

	if(FDDownPar.DownFlag != rbu[FDCMDXY_DATA]){//类型不一致,
		MSG_LOG("DealDownData DownFlag Error:\r\n");
		return 1;
	}

	if(memcmp(rbu+FDCMDXY_DATA+1, "\xFF\xFF", 2) == 0){//结束包
		if(FDDownPar.DownFlag == FDProcess_GETGPSLineInfo){
			Load_Line_info();	//重新载入数据
			gFDmissflag &= ~FDMISS_GETLine;
		}
		else if(FDDownPar.DownFlag == FDProcess_GETFDticket){
			FDinitFDticket_ALL();
			gFDmissflag &= ~FDMISS_GETTicket;
		}
		else if(FDDownPar.DownFlag == FDProcess_GETPriceRate){//一个包完成，所以后门机没要结束包，已经保存了，应该不会到这里。
			gFDmissflag &= ~FDMISS_GETPriceRate;
		}
		else if(FDDownPar.DownFlag == FDProcess_GETProgram){//程序完成
			gFDmissflag &= ~FDMISS_GETProgram;
			memset(rbu, 0, 64);
			memcpy(rbu,"程序需要更新", 12);
			memcpy(rbu+12,"\xEE\xFF", 2);
			memcpy(rbu+14, (unsigned char*)&SysTime, 7);

			MSG_LOG("程序下载完成:");
			BCD_LOG(rbu, 21, 1);

			display(2,0,"完成,请重启...",DIS_CENTER);
			Waddr = FLASH_PRO_FLAG;
			
			MSG_LOG("程序下载完成2:");
			BCD_LOG(rbu, 34, 1);
			flashwrite(Waddr, rbu, 64);
			
			beep(1,10,10);

			delayxms(400);
			ReStartCPU();//下载完成，重启设备以便更新程序。
			while(1);
		}
		gFDdealProcess = FDProcess_GETFind;
		memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		return 0;
	}
	
	if(memcmp(FDDownPar.DownSerial, rbu+FDCMDXY_DATA+1, 2) != 0){//包序号不一致
		MSG_LOG("DealDownData Error:\r\n");
		return 1;
	}
	
	memcpy((unsigned char*)&ofset, rbu+FDCMDXY_DATA+1, 2);	
	if(FDDownPar.DownFlag == FDProcess_GETGPSLineInfo){
		Waddr = FLASH_GPS_LINE_START;
		if(ofset == 0){ //
			MSG_LOG("GPS LINE Erase\r\n");
			flasherase(ERASE64KBYTESECTOR, Waddr);//先擦除 64K
			Waddr += 0x10000;//下一个64K的块
			//Delay(2);
			flasherase(ERASE64KBYTESECTOR, Waddr);//先擦除 64K
		}
		Waddr = FLASH_GPS_LINE_START;
		wof = ofset*64;
		wlen = 64;
	}
	else if(FDDownPar.DownFlag == FDProcess_GETFDticket){
		Waddr = FLASH_FD_Ticket_START;
		if(ofset == 0){ //
			MSG_LOG("Ticket Erase\r\n");
			flasherase(ERASE32KBYTESECTOR, Waddr);//擦除
		}
		wof = ofset*64;
		wlen = 64;
	}
	else if (FDDownPar.DownFlag == FDProcess_GETPriceRate){//更新票价。一个包256字节完成。保存完了后就结束，不需要结束包了。
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
				flasherase(ERASE64KBYTESECTOR, Waddr);//先擦除 256K
				Waddr += 0x10000;//下一个64K的块
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

	memcpy(FDDownPar.DownPerSerial, (unsigned char*)&ofset, 2);	//更新下载参数
	ofset++;
	memcpy(FDDownPar.DownSerial, (unsigned char*)&ofset, 2); //包序号加1
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
//rbu中是收到的数据，组织好的数据也放rbu中传回。前门机才会传。
unsigned int FDBuildDownData_QM(unsigned char *rbu)
{											//2424242410000302 81 0000 0000 0000 90
	unsigned int index=0;
	unsigned short of;

	FDDownPar.DownFlag = rbu[FDCMDXY_DATA];					//前门机的数据使用后门机传过来的数
	memcpy(FDDownPar.DownPerSerial, rbu+FDCMDXY_DATA+1, 2);
	memcpy(FDDownPar.DownSerial, rbu+FDCMDXY_DATA+3, 2);
	memcpy((unsigned char*)&of, FDDownPar.DownSerial, 2);
	
	MSG_LOG("Build Down Dat:flag=%02X\r\n", FDDownPar.DownFlag);
	BCD_LOG(FDDownPar.DownPerSerial, 2, 1);
	BCD_LOG(FDDownPar.DownSerial, 2, 1);

	if(FDDownPar.DownFlag == FDProcess_GETGPSLineInfo){
		if(of >= (0x20000/64)){//128K,到最后了
			of = 0xFFFF;
		}
		else{
			index = FDFlashDataRead_QM(FLASH_GPS_LINE_START, rbu);
			if(DataIsFF_QM(rbu+3, 64) == 1){//数据全是FF，结束
				of = 0xFFFF;
			}
		}
	}
	else if(FDDownPar.DownFlag == FDProcess_GETFDticket){
		if(of >= (0x8000/64)){//32K,到最后了
			of = 0xFFFF;
		}
		else{
			index = FDFlashDataRead_QM(FLASH_FD_Ticket_START, rbu);
			if(DataIsFF_QM(rbu+3, 64) == 1){//数据全是FF，结束
				of = 0xFFFF;
			}
		}
	}
	else if(FDDownPar.DownFlag == FDProcess_GETPriceRate){
		if(of >= 1){//只有256字节，一个包下发，超过一个包发结束。后门机不会申请多一个包，所以这里是不会执行的。
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
		if(of >= (0x3B800/256)){//238K,到最后了 24242424 1000 03 BD83B703B803000022
			of = 0xFFFF;
		}
		else{
			index = FDProDataRead_QM(rbu);
			if(DataIsFF_QM(rbu+3, 256) == 1){//数据全是FF，结束
				of = 0xFFFF;
			}
		}
	}
	else{
		return 0;
	}
	
	if(of == 0xFFFF){//数据已经发完了
		rbu[0] = FDDownPar.DownFlag;
		rbu[1] = 0xFF;
		rbu[2] = 0xFF;
		index = 3;
	}
	
	return index;

}

//命令处理,前门、后门的处理都在里面。
int CanFDdealCMD_ALL(unsigned char *Rdata)
{
	DEALRECODE *rRe;
	int ret;
	unsigned int ui, ii, slen=0;
	unsigned char rbuf[300];
	
	if(gCanRevFlag != 0xAA){//没有收到完整包
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
	
	if(rbuf[FDCMDXY_CMD] & 0x80){	// 最高位为1是前门机的应答。
		if(gFDMode == FDMODE_GPS){	//这是前门机发出的应答。所以前门机不处理此命令。前门机只处理后门机的命令
			MSG_LOG("前门机命令类型错:%02X\r\n", rbuf[6]);
			return -2;
		}
	}
	else{	//最高位为0是后门机发出的指令。
		if(gFDMode == FDMODE_NORMAL){	//这是后门机发出的命令。所以后门机不处理此命令。后门机只处理前门机的应答
			MSG_LOG("后门机命令类型错:%02X\r\n", rbuf[6]);
			return -2;
		}
	}
	
	switch(rbuf[FDCMDXY_CMD]){
	case FDCMD_VERCX://前门机发送的取参数版本号命令
		gFDCmdSerial = rbuf[FDCMDXY_Serial];
		ii = SOFT_VER_TIME_LOG;	//程序版本号。
		memcpy(rbuf, (unsigned char*)&ii, 4);
		slen += 4;
		if(gGPsPar.EnableGps == 0){//线路信息参数不对
			memset(rbuf+slen, 0, 4);	//线路信息版本。
		}
		else{
			memcpy(rbuf+slen, (unsigned char*)&LineInfo[0].uiVer, 4);	//线路信息版本。
		}
		slen += 4;
		ii = gFdLinehead.CXulieNo;	//阶梯票价版本，设置时随机生成。
		memcpy(rbuf+slen, (unsigned char*)&ii, 4);
		slen += 4;
		memcpy(rbuf+slen, pFistVary.LineNo, 2);	//线路号
		slen += 2;
		ASCII2ZBcod((char*)pFistVary.DeviceNo, rbuf+slen, 4);//设备号
		slen += 4;

		MSG_LOG("CandFDCmd_snd DATA:");
		BCD_LOG(rbuf, slen, 1);

		ret = FDCMDData_Send(FDCMD_VERCX|0x80, rbuf, slen);
		gFDParChange = 0;	//后门机在取了版本信息后，不再通知后门机更新参数或重启。
		MSG_LOG("FDCMD_VERCX end ret=%d\r\n", ret);
		break;
	case FDCMD_VERCX_ECHO://前门机回应的版本号 24242424150081B9 73000000 01000000 D0000000 8F
		MSG_LOG("版本回应!\r\n");	//程序版本、线路信息版本、阶梯票价表
		gFDmissflag = FDMISS_NULL;
		memcpy((unsigned char*)&ui, rbuf+FDCMDXY_DATA, 4);	//程序版本,
		if(ui != SOFT_VER_TIME_LOG){//程序版本不一致
			gFDmissflag |= FDMISS_GETProgram;
		}
		memcpy((unsigned char*)&ui, rbuf+FDCMDXY_DATA+4, 4);	//线路信息版本
		if((ui!=0)&&(ui != LineInfo[0].uiVer)){//前门机给的版本号为0，说明前门机也没有此信息。
			MSG_LOG("线路信息不一致:ui=%d, ver=%d\r\n", ui, LineInfo[0].uiVer);
			gFDmissflag |= FDMISS_GETLine;
		}
		memcpy((unsigned char*)&ui, rbuf+FDCMDXY_DATA+8, 4);	////阶梯票价版本
		if((ui!=0)&&(ui != gFdLinehead.CXulieNo)){	//前门机给的版本号为0，说明前门机也没有此信息。
			MSG_LOG("线路信息不一致:ui=%d, ver=%d\r\n", ui, gFdLinehead.CXulieNo);
			gFDmissflag |= FDMISS_GETTicket|FDMISS_GETPriceRate;	//要同时更新阶梯票价及票价折扣.
		}
		if(memcmp(pFistVary.LineNo, rbuf+FDCMDXY_DATA+12, 2) != 0){//线路号不一样，保存
			memcpy(pFistVary.LineNo, rbuf+FDCMDXY_DATA+12, 2);	//线路号
			
			sysfewrite(BIT_LINENO,3,pFistVary.LineNo);
			sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);
			saveCpuInfo(1, rbuf);
		}
		ZBcod2ASCII(rbuf+FDCMDXY_DATA+14, 4, (char*)pFistVary.DeviceNo_1);	//设备号
		if(memcmp(pFistVary.DeviceNo, pFistVary.DeviceNo_1, 8) != 0){//不一致，需要保存
			memcpy(pFistVary.DeviceNo, pFistVary.DeviceNo_1, 8);
			pFistVary.DeviceNo[8] = 0;
			sysSaveDevice(rbuf);
		}

		gFDdealProcess = FDProcess_GETFind;
		restore_flag = 3;
		gFDScount = 0;
		break;
	case FDCMD_GPSINFO://后门机发送的取位置信息命令。
		gFDCmdSerial = rbuf[FDCMDXY_Serial];		
		memcpy(rbuf, (unsigned char*)&SysTime, 7);
		slen = 7;
		if(gFDderict<2)
			rbuf[slen++] = gZNFDucID[gFDderict];//传入站点序号。编号还得从线路信息表中查出。
		else
			rbuf[slen++] = 0;
		rbuf[slen++] = gFDderict;
		memcpy(rbuf+slen, (unsigned char*)&gGpsInfo.velocity, 4);
		slen += 4;
		rbuf[slen++] = gFDParChange;	//此值传过去以后，清0
		gFDParChange = 0;
		ret = FDCMDData_Send(FDCMD_GPSINFO|0x80, rbuf, slen);
		MSG_LOG("FDCMD_GPSINFO end ret=%d\r\n", ret);
		break;
	case FDCMD_GPSINFO_ECHO://后门机回应的时间、站点、方向、车速、参数是否变化
		MSG_LOG("信息回应!\r\n");//242424241700820D 20151012182409 05 00 00000000 00 9F
		if(memcmp((unsigned char*)&SysTime, rbuf+FDCMDXY_DATA, 5) != 0){//时间不一致，比效到时
			MSG_LOG("校时:");
			BCD_LOG(rbuf+FDCMDXY_DATA, 7, 1);
			if(time_invalid(rbuf+FDCMDXY_DATA) == 1){
				timewrite(rbuf+FDCMDXY_DATA);
				MSG_LOG("校时:3\r\n");
			}
		}
		gFDucID = rbuf[FDCMDXY_DATA+7];	//站点序号
		gFDderict = rbuf[FDCMDXY_DATA+8];	//方向
		memcpy((unsigned char*)&gGpsInfo.velocity, rbuf+FDCMDXY_DATA+9, 4); //速度
		if(rbuf[FDCMDXY_DATA+13]&0x7F){//参数变化,最好是重启重新更新参数。
			gFDdealProcess = FDProcess_GETVER;//重新取各参数版本号
		}
		else if(rbuf[FDCMDXY_DATA+13]&FDParChange_Restart){//设备需要重启
			gFDdealProcess = FDProcess_GETVER;//重新取各参数版本号， 不重启。
// 			cls();
// 			display(0, 1, "指令重启", DIS_CENTER);
// 			beep(8, 50, 50);
// 			ReStartCPU();
		}
		gFDScount = 0;
		break;
	case FDCMD_DOWNPar:	//主机收到下载参数指令。需要组织数据进行下载。
		slen = FDBuildDownData_QM(rbuf);
		BCD_LOG(rbuf, slen, 1);
		ret = FDCMDData_Send(FDCMD_DOWNPar|0x80, rbuf, slen);
		MSG_LOG("FDCMD_DOWNPar end ret=%d\r\n", ret);
		break;
	case FDCMD_DOWNPar_ECHO://前门机（从机）加应的数据。后门机处理。
		MSG_LOG("数据回应!\r\n");
		BCD_LOG(rbuf, ii, 1);
		FDDealDownData_HM(rbuf);	//处理回应的数据
		gFDScount = 0;
		break;
	case FDCMD_UPDateREC://后门机发送数据过来。
		MSG_LOG("记录数据!\r\n");
		rRe = (DEALRECODE *)rbuf+FDCMDXY_DATA+3;
		Get_SerialNum(rRe->rResever1);	//唯一号
		memcpy(rRe->rDriverNo, DriveCardNo, 4);	//司机号.
		WriteRecord(rbuf+FDCMDXY_DATA+3, 0);
		rbuf[0] = rbuf[FDCMDXY_DATA];
		rbuf[1] = rbuf[FDCMDXY_DATA+1];
		rbuf[2] = rbuf[FDCMDXY_DATA+2];
		rbuf[3] = 0;//状态
		slen = 4;
		BCD_LOG(rbuf, slen, 1);
		ret = FDCMDData_Send(FDCMD_UPDateREC|0x80, rbuf, slen);
		MSG_LOG("FDCMD_DOWNPar end ret=%d\r\n", ret);
		break;
	case FDCMD_UPDateREC_ECHO:
		MSG_LOG("记录回应:%02X, %02X, %02X\r\n", tcpipPacket.tcpBuffer[0], tcpipPacket.tcpBuffer[1], tcpipPacket.tcpBuffer[2]);
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

//前门GPS机器显示界面
void FengDuan_GPS_disp_QM(void)
{
	unsigned int it=0;
	char disbuff[30];
	
	cls();
	if((gFDderict>=2)||(gFDucID == 0)||(gFDucID > 100))
	{
		strcpy(disbuff, "未确定位置");
		LED_Dis3("------");
	}
	else if(gGPsPar.EnableGps == 0){//线路信息错误
		strcpy(disbuff, "线路信息错误");
		LED_Dis3("------");
	}
	else if(gFDRunStatues & FDRUNSTATUES_PJ){
		strcpy(disbuff, "票价信息错误");
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
				strcpy(disbuff, "上行:");
			else if(gFDderict==1)
				strcpy(disbuff, "下行:");
			else
				strcpy(disbuff, "位置:");
		}
		strcat(disbuff, LineInfo[gFDderict].StationInfo[gFDucID-1].name);
	}
	display(0,1,disbuff,0);
	
	memset(disbuff, 0, 30);
	strcpy(disbuff, "线路:");
	BCD2Ascii(pFistVary.LineNo, (unsigned char*)disbuff+5, 2);
	display(2,1,disbuff,0);
	it = 0;
	memcpy((unsigned char*)&it, gFdLinehead.TottalPrice, 2);
	sprintf(disbuff, "全程:%d.%02d元", it/100, it%100);
	display(4,1,disbuff,0);
}

void disMission_HM(void)
{
	char disbuf[32];
	switch(gFDdealProcess){
	case FDProcess_GETFind:
		strcpy(disbuf, "确定任务中");
		break;
	case FDProcess_GETVER:
		strcpy(disbuf, "查询版本中");
		break;
	case FDProcess_GETGPSLineInfo:
		strcpy(disbuf, "更新线路信息");
		break;
	case FDProcess_GETFDticket:
		strcpy(disbuf, "更新票价表");
		break;
	case FDProcess_GETProgram:
		strcpy(disbuf, "更新程序中");
		break;
	case FDProcess_GETPriceRate:
		strcpy(disbuf, "更新折扣表");
		break;
	default:
		break;
	}
	display(2, 1, disbuf, DIS_CENTER);
}
//后门普通机显示界面
void FengDuan_Normal_disp_HM(void)
{
	char disbuf[32];
	
	cls();

	pFistVary.DeviceNo[8] = 0;
	sprintf(disbuf, "L:%02X%02X D:%s %03d", pFistVary.LineNo[0], pFistVary.LineNo[1], (char*)pFistVary.DeviceNo, SOFT_VER_TIME_LOG);
	miniDispstr(0, 0, disbuf, 0);
//	display(0, 0, " 分段扣费 ", DIS_CENTER|DIS_CONVERT);
	if(gFDdealProcess <= FDProcess_GETProgram){//在更新各种参数时，后门机不能刷卡。
		disMission_HM();
		display(4, 0, "暂停刷卡", DIS_CENTER);
	}
	else if((gFDderict>=2)||(gFDucID == 0)||(gFDucID > 100))
	{
		display(2, 1, "未确定位置", DIS_CENTER);
		display(4, 1, "不能刷卡!", DIS_CENTER);
	}
	else if(gGPsPar.EnableGps == 0){//线路信息错误
		display(2, 1, "线路信息错误", DIS_CENTER);
		display(4, 1, "不能刷卡!", DIS_CENTER);
	}
	else if(gFDRunStatues & FDRUNSTATUES_PJ){
		display(2, 1, "票价信息错误", DIS_CENTER);
		display(4, 1, "不能刷卡!", DIS_CENTER);
	}
	else{
		MSG_LOG("disp SLA:gFDderict=%d,gFDucID=%d,maxID:%d\r\n", gFDderict, gFDucID, LineInfo[0].ucStationMax);
		strcpy(disbuf, LineInfo[gFDderict].StationInfo[gFDucID-1].name);
		display(2, 1, disbuf, DIS_CENTER);
		display(4, 1, "请刷卡!", DIS_CENTER);
	}
	LED_Dis3("------");
}

//主机参数变化时改变标志，包括程序变化，票价变化，线路变化，设备号变化。
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
	
	if(memcmp(buff, "此线路信息", 10) != 0){
		
		MSG_LOG("Init Err1\r\n");
		
		memset((unsigned char*)&gFdLinehead, 0, sizeof(stFDLineHead));
		gFDRunStatues |= FDRUNSTATUES_PJ;
	}
	else{
		ver = buff[12];	// 版本号，在设置线路号时计算得出。
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
//初始化分段收费的变量,开机调用一次
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

		gZNFDucID[0] = gFDStaIndex[0]+1;	//给序号【加1是因为序号从0开始，具体操作是从1开始的】，不给站点编号。反向站点编号和序号不一样
		gZNFDucID[1] = gFDStaIndex[1]+1;
		gFDucID = gZNFDucID[gFDderict];

		gFDParChange |= FDParChange_Restart;	//前门机启动时，通知后门机重启。
	}
	FDinitFDticket_ALL();
}

//测试写入分段票价。
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
	memcpy(buff+16, "此线路信息OKOKOK", 16);
	i = CRC8_Tab((unsigned char*)&SysTime, 7);	//计算当前的CRC8作为版本号
	buff[12] = i;
	flashwrite(addr, buff+16, 16);	//标志
	addr+=16;
	flashwrite(addr, buff, 16);//16 头
	addr+=16;
		//下面是分段数据,2个字节一个点.如果是10*10个站点,则是100个.第一个是0-0 0-1.....
	for(i=0; i<12; i++){//10个正向点
		for(t=0; t<12; t++){//10个反向点
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

	for(i=0; i<12; i++){//10个正向点
		for(t=0; t<12; t++){//10个反向点
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
	k = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId; //由0 改成 gFDderict
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

	memcpy(rRecordDeal->rBussinessNo, cardMessage.TradeCode, 2);//商户编号以卡内编号为准，不再取刷卡机内编号
	rRecordDeal->rReseverX = 0xFF;
	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);//程序版本

	CountRecMac((unsigned char*)&rRecordDeal, 60, rRecordDeal->rTAC);
	memcpy(rRecordDeal->rTAC+2, "\xea\xef", 2);

	MSG_LOG("BLD REC:");
	BCD_LOG(recBuf, 64, 1);

	
	if(delType == ID_REC_TOLL)
		addStatMoney(delType, s_sum1);
}

//取两站点的票价,
unsigned int get2stationPrice(unsigned char sta1, unsigned char sta2)
{
	unsigned char gspbuf[16];
	unsigned int i, t, addr;

	if(sta1 == sta2){//站点没变, 如果票价中的起步价不是0，则也需要扣费。
		addr = FLASH_FD_Ticket_START;
		addr+=16;	//标志
		flashread(addr, gspbuf, 16);//16 头
		MSG_LOG("get stationPrice:");
		BCD_LOG(gspbuf, 16, 1);
		i = gspbuf[14];	//起步价，角为单位。0.1
		i *= 10;
		return i;
	}
	if(sta1 > sta2){
		i = sta1;
		t = sta2;
	}
	else{
		i = sta2;	//i为大的编号.t为小的编号
		t = sta1;
	}
	i -= 1;
	t -= 1;	//编号从0开始,位置从0开始
	
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
	addr = FLASH_FD_Ticket_START+32;//起始地址
	addr += (i*2); //每个票价2个字节.
	
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
		irda_rxBuf[0] =  Cp1->stationNo;	//保存在临时变量里,用于组装命令 在CandFDCmd_snd函数中的FDCMD_MPurChase命令中。 
	memcpy(irda_rxBuf+1, LineInfo[gFDderict].StationInfo[irda_rxBuf[0]-1].name, 23);

	if(Cp1->getupFlag != 0xAA)//标志不对
		return 0;
	
	Ascii2BCD(pFistVary.DeviceNo, cbuff, 8);
	i = 0;
	memcpy((unsigned char*)&i, Cp2->timeDly, 2);//允许的最大时间长 单位分。
	i *= 60;
	t = DataTimeDec((unsigned char*)&SysTime, Cp1->getupTime);
	
	Ascii2BCD(pFistVary.DeviceNo, cbuff, 8);


	BCD_LOG(Cp2->deviceNo, 4, 1);
	BCD_LOG(cbuff, 4, 1);


	if(Cp1->times == 0){//没有消费次数
		return 0;
	}

	if((memcmp(Cp1->LineNo, pFistVary.LineNo, 2) != 0)||(memcmp(Cp2->deviceNo, cbuff, 4) != 0)||(t > i)){//不同车,不同线路,超时,扣全程 
		i = 0;
		memcpy((unsigned char*)&i, Cp1->Allprice, 2);
		return i;
	}
	
//	zdbh = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;	//减1是因为序号都是从1开始的。
	zdbh=LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//下车的位置
	czdbh=Cp1->stationNo; //上车的位置
	offstion=czdbh;//保存 组记录要的
// 	if(gFDderict == 0){//正向
// 		//czdbh = LineInfo[0].StationInfo[Cp1->stationNo-1].ucId;
// 		czdbh=Cp1->stationNo;
// 	}
// 	else{//反向
// 		czdbh = LineInfo[1].StationInfo[Cp2->NstationNO-1].ucId;
// 	}
	//从里面线路信息里查出来的是BCD码。
	zdbh = (zdbh>>4)*10+(zdbh&0x0F); //下车的位置
	//czdbh = (czdbh>>4)*10+(czdbh&0x0F);

	s_sum1 = get2stationPrice(zdbh, czdbh);
// 	debugstring("票价\r\n");
// 	debugdata((unsigned char*)&zdbh,1,1);
// 	debugdata((unsigned char*)&czdbh,1,1);
// 	debugdata((unsigned char*)&i,4,1);
	MSG_LOG("CountSum sum:%d\r\n", i);
	i=get_s_sum1(0);//取折扣
	return i;
}

//手动输入站点编号
void EX_dealFengDuanStation(void)
{
//	unsigned char i, station;
//	unsigned int ds1,ds2;
	if(gFDMode == FDMODE_NORMAL){
		return;	//后门不需要设置
	}
	gFDStaIndex[0] = RunStation.ucStation;
	gFDStaIndex[1] = RunStation.ucStation;;
	

	MSG_LOG("dealFDstaGPS:%02X, %02X\r\n", gFDStaIndex[0], gFDStaIndex[1]);
	
	gFDderict = RunStation.ucDirect;

	MSG_LOG("gFDderict:%02X, %02X\r\n", gFDderict, gFDStaIndex[gFDderict]);

	//		i = LineInfo[gFDderict].StationInfo[gFDStaIndex[gFDderict]].ucId;
	//		gFDucID = (((i>>4)*10)+(i&0x0F));
	gZNFDucID[0] = gFDStaIndex[0]+1;	//给序号【加1是因为序号从0开始，具体操作是从1开始的】，不给站点编号。反向站点编号和序号不一样
	gZNFDucID[1] = gFDStaIndex[1]+1;
	gFDucID = gZNFDucID[gFDderict];
	
	MSG_LOG("dealFDstaGPS2 gFDucID:%02X, gZNFDucID=%02X\r\n", gFDucID, gZNFDucID);

}
//处理分段的站点编号 ,
 unsigned int distar1,diend2;// 离起点和终点的距离
void dealFengDuanStation(void)
{
	unsigned char i, station;
	unsigned int ds1,ds2;
	
	if(gFDMode == FDMODE_NORMAL){
		return;	//后门机不需要找站点
	}

		station = 0;
		ds2 = 0xffffffff;
		for(i=0; i<LineInfo[0].ucStationMax; i++){//正向
			ds1 = GpsDistance(LineInfo[0].StationInfo[i].ulLatitude, LineInfo[0].StationInfo[i].ulLongitude);
			if(ds1 <= ds2){
				ds2 = ds1;
				station = i;
			}
		}

		gFDStaIndex[0] = station;
		
		station = 0;
		ds2 = 0xffffffff;
		for(i=0; i<LineInfo[1].ucStationMax; i++){//反向
			ds1 = GpsDistance(LineInfo[1].StationInfo[i].ulLatitude, LineInfo[1].StationInfo[i].ulLongitude);
			if(ds1 <= ds2){
				ds2 = ds1;
				station = i;
			}
		}
		gFDStaIndex[1] = station;
		
		MSG_LOG("\r\ndealFDstaGPS gFDStaIndex:%02X,%02X", gFDStaIndex[0], gFDStaIndex[1]);

		if(RunStation.ucDirect < 2)	//已经确定方向
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
		gZNFDucID[0] = gFDStaIndex[0]+1;	//给序号【加1是因为序号从0开始，具体操作是从1开始的】，不给站点编号。反向站点编号和序号不一样
		gZNFDucID[1] = gFDStaIndex[1]+1;
		gFDucID = gZNFDucID[gFDderict];

		distar1 = GpsDistance(LineInfo[0].StationInfo[0].ulLatitude, LineInfo[0].StationInfo[0].ulLongitude);//第一个站点的距离
		diend2 = GpsDistance(LineInfo[1].StationInfo[0].ulLatitude, LineInfo[1].StationInfo[0].ulLongitude);//最后一个站点的距离
		if (distar1<200&&gFDucID==1){//在起点 定位正向
			gFDderict=0;
			RunStation.ucDirect=gFDderict;//没有确定方向的情况，在第一个站点定位正向
		}
		if(diend2<200&&gFDucID==1){//在终点   定为反向
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
//处理卡片，并写入信息。
void FengDuan_GPS_card(void)
{
	stFDCard1 *pcard1;
	stFDCard2 *pcard2;
	unsigned char zdbh;
	unsigned int i, t;
	unsigned int delMoney;	//补扣的金额
	unsigned char fdcard[2][16];
	unsigned char buff[70];
	unsigned char ucRnt, k;
	
	if(memcmp(gFdLinehead.LineNo, pFistVary.LineNo, 3) != 0){//线路信息有错，需要重新设置
		restore_flag = 3;
		MessageBox(1, "线路信息参数错，请重新设置!");
		beep(8, 50, 50);
		end_card();
		return;
	}

	if(gGPsPar.EnableGps == 0){
		restore_flag = 3;
		MessageBox(1, "没有站点信息!");
		beep(8, 50, 50);
		end_card();
		return;
	}
	if((gFDderict > 1)||(gFDucID == 0) || (gFDucID > 99)){
		restore_flag = 3;
		MessageBox(1, "站点没有定位!");
		beep(8, 50, 50);
		end_card();
		return;
	}

	delMoney = 0;
	if(Delay_FenDuan_Normal() !=0 )
		return;
// 	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
// 	{
// 		disp_no_swipe();
// 		end_card();
// 		return ;	
// 	}

	for(i=0; i<3; i++){//读出卡内分段收费的信息,写入记录区最后一扇区。
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
	if(i >= 3){//读错
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
	//上面处理卡片数据

	MSG_LOG("fdcard 2::");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	pcard1 = (stFDCard1 *)fdcard[0];
	pcard2 = (stFDCard2 *)fdcard[1];
	if(pcard1->getupFlag == 0xAA){//时间必须有效
		Ascii2BCD(pFistVary.DeviceNo, buff, 8);
		i = 0;
		memcpy((unsigned char*)&i, pcard2->timeDly, 2);//允许的最大时间长 单位分。
		i *= 60;

		if((memcmp(pcard2->deviceNo, buff, 4)==0)&&	//车相同
			(DataTimeDec((unsigned char*)&SysTime, pcard1->getupTime) <= i))//在最大时长内，
		{	//连续刷卡
			if(gFDderict == 0)//正向
				ucRnt = pcard1->stationNo;
			else
				ucRnt = pcard2->NstationNO;

			if(ucRnt == gFDucID){//站点相同,当作连续刷卡
				;
 			//	FengDuan_Normal_card();//上车门也扣钱ttttttttttttttttttttttttttt
				MessageBox(0,"不能带人乘车");
				end_card();
 				return;
			}
			else
			{//不相同的站点都在前门刷卡，则禁止再次刷卡，或者扣钱下车。
// 				MessageBox(0, "请到下车门刷卡!");
// 				led_on(LED_RED);
// 				end_card();
				FengDuan_Normal_card();//上车门也扣钱
				return;
			}
		}
		else{//补扣金额后，再做标志
			delMoney = 0;
			memcpy((unsigned char*)&delMoney, pcard1->Allprice, 2);
			s_sum1 = (delMoney*pcard1->times);	//一次扣光所有次数。
			ucRnt = PurseProcess(0);
																	//卡交易金额处理
			MSG_LOG("FD Purse Process = ");
			BCD_LOG(&ucRnt,1, 1);

			switch(ucRnt)   //96ms
			{
			case ST_ERROR:						//普通错误
				MessageBox(1, "操作钱包错误!");
				break;
			case 21:
			case RET_MONEY_SN_EROR:																//两个钱包都错误
				error(ERR_MONEY_SN, 0);
				return;
			case 22:
				display(0,0,"提示:", DIS_Cls);
				display(2,0,"票价信息不正确",DIS_CENTER);
				break;
			case 2:
			case 3://余额不足
				led_on(LED_RED);
				break;
			case 4:
			case 6:
				audio(Audio_TRY_AGAIN);		//请重刷
				card_ser = c_serial;
				cardSound = 0xaa;
				return;
			case 5:
			case 7:
			case 8:
				cardSound=0;
				ComSumeFile.SumMoney+=s_sum1; //当前消费总额
				cardMessage.getdata_w = pcard1->times;	//刷卡次数
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
//清掉后 再上车
			memset(fdcard[0], 0x00, 16);
			memset(fdcard[1], 0x00, 16);
		}
	}//以上是连刷处理
	else{
		memset(fdcard[0], 0x00, 16);
		memset(fdcard[1], 0x00, 16);
	}
	pcard1->getupFlag = 0xAA;
	pcard1->times++;
//	pcard1->stationNo =gZNFDucID[0]; 	//写入卡内的序号是从1开始的。

	//@zzc
	zdbh= LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;	
	zdbh = (zdbh>>4)*10+(zdbh&0x0F);
	pcard1->stationNo=zdbh;
//	debugstring("上车位置：");
//	debugdata((unsigned char*)&zdbh,1,1);
//	memcpy(pcard1->Allprice, gFdLinehead.TottalPrice, 2);//全程票价
	
	MSG_LOG(" 找到的方向::");
	BCD_LOG((unsigned char*)&RunStation.ucDirect, 1, 1);
	MSG_LOG(" gFDucID::");
	BCD_LOG((unsigned char*)&gFDucID, 1, 1);
	if(RunStation.ucDirect == 1)//反向，终点为0号站点
		i = 1;
	else	//正向，终点为最大站点
		i = LineInfo[0].StationInfo[LineInfo[0].ucStationMax-1].ucId;//现改为由当前站到终点站的票价
	
	MSG_LOG(" i::");
	BCD_LOG((unsigned char*)&i, 4, 1);
	MSG_LOG(" ucId::");
	BCD_LOG((unsigned char*)&LineInfo[0].StationInfo[LineInfo[0].ucStationMax-1].ucId, 1, 1);

	i = (((i>>4)*10)+(i&0x0F));	//站点编号  保存里的是BCD码
	
	if(gFDderict<2)
		k = LineInfo[gFDderict].StationInfo[gFDucID-1].ucId;//减1是因为序号都是从1开始的。
	else
		k = LineInfo[0].StationInfo[gFDucID-1].ucId;

	t = (((	t >>4)*10)+(t&0x0F));	//站点编号  保存里的是BCD码
	t = get2stationPrice(i, k);

	i = 0;
	memcpy((unsigned char*)&i, gFdLinehead.TottalPrice, 2);
	
	MSG_LOG("全程票价: ");
	BCD_LOG((unsigned char*)&t, 4, 1);
	BCD_LOG((unsigned char*)&i, 4, 1);
	if(PurseProcess(100)==ST_ERROR)//取卡余额
		return;

	if(t > i)
		t = i;
	if(a_sum1<t)//余额小于到终点的 不能上车
	{
		MessageBox(0,"余额不足");
		sprintf((char*)buff,"余额:%d.%d元",a_sum1/100,a_sum1%100);
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
	pcard2->NstationNO = gZNFDucID[1];	//写入卡内的序号是从1开始的。
	Get_SerialNum(pcard2->BUSerial);
	//pcard2->getdownTimes   下车次数，不处理。
	memcpy(pcard2->timeDly, gFdLinehead.NeedMaxTime, 2);//上车允许最长时间长度
	pcard2->Cxor = MiBlockInvalid(0,15,fdcard[1]);

	MSG_LOG("FD need writeData:\r\n");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	//写入信息
	for(i=0; i<3; i++){//读出卡内分段收费的信息,写入记录区最后一扇区。
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
	if(i >= 3){//写入错
		MSG_LOG("End Write FD Error!");
		MessageBox(1, "写卡出错!");
		led_on(LED_RED);
		end_card();
		return;
	}
	cls();
	sprintf((char*)buff, "上车:%d次", pcard1->times);
	display(0, 0, (char*)buff, 0);
	if(delMoney != 0){
		sprintf((char*)&buff, "补扣:%d.%02d", delMoney/100, delMoney%100);
		display(2, 0, (char*)buff, 0);
		sprintf((char*)&buff, "余额:%d.%02d", a_sum1/100, a_sum1%100);
		display(4, 0, (char*)buff, 0);
	}
	else{
		display(3, 0, "刷卡成功!", DIS_CENTER);
	}
	display(6, 0, "下车时请再次刷卡", 0);
	save_normal();
	//save_h_month();//全部卡不能带人乘车
	led_on(LED_GREEN);
	//audio(Audio_SuaKaOK);
	SoundMessage(SOUND_DEAL);
	cardMessage.getdata_w = pcard1->times;
	BuildFengDuanRecorde(ID_REC_FDUp, buff);	//生成上车刷卡记录.
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
		MessageBox(1, "站点没有确定!");
		restore_flag = 3;
		end_card();
	}
	
	for(i=0; i<3; i++){//读出卡内分段收费的信息,写入记录区最后一扇区。
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
	if(i >= 3){//读错
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
	//上面处理卡片数据

	MSG_LOG("fdScard 2::");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	pcard1 = (stFDCard1 *)fdcard[0];
	pcard2 = (stFDCard2 *)fdcard[1];
	if(pcard1->getupFlag == 0xAA){
	//	if(gFDderict == 0)//正向
			ucUP_Station = pcard1->stationNo;//不管正向反向， 都是取正向的编号，
	//	else
	//		ucUP_Station = pcard2->NstationNO;

		s_sum1 = CountSum((unsigned char *)fdcard[0]);

		MSG_LOG("FDS Purse s_sum1 = ");
		BCD_LOG((unsigned char*)&s_sum1,4, 1);

		memcpy((unsigned char*)&i,gFdLinehead.TottalPrice,2);
// 		memcpy((unsigned char*)&i, pcard1->Allprice, 2);
		
		if(s_sum1 > i){
			MessageBox(1, "票价错误!");
			end_card();
			return;
		}

		ucRnt = PurseProcess(0);//卡交易金额处理
	

		MSG_LOG("FDS Purse Process = ");
		BCD_LOG(&ucRnt,1, 1);

		switch(ucRnt)    //96ms
		{
		case ST_ERROR:						//普通错误
			MessageBox(1, "操作钱包错误!");
			break;
		case 21:
		case RET_MONEY_SN_EROR:				//两个钱包都错误
			error(ERR_MONEY_SN, 0);
			return;
		case 22:
			display(0,0,"提示:", DIS_Cls);
			display(2,0,"票价信息不正确",DIS_CENTER);
			break;
		case 2:
		case 3://余额不足
			led_on(LED_RED);
			break;
		case 4:
		case 6:
			audio(Audio_TRY_AGAIN);		//请重刷
			card_ser = c_serial;
			cardSound = 0xaa;
			return;
		case 5:
		case 7:
		case 8:
			cardSound=0;
			ComSumeFile.SumMoney+=s_sum1; //当前消费总额
			cardMessage.getdata_w = pcard1->times;	//刷卡次数
			card_ser = 0;

			BuildFengDuanRecorde(ID_REC_TOLL, buff);	//生成完成消费记录.
			WriteRecord(buff, 0);

			if(tcpipPacket.packetStatus != RECORD_DEAL)
				tcpipPacket.packetStatus = 0;	//如果发送缓冲中没有数据，这样可以立刻组织数据。
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
		display(0,0,"提示:",0);
		display(3,0,"已完成刷卡!",DIS_CENTER);
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
		pcard2->getdownTimes++;   //下车次数，不处理。
		pcard2->Cxor = MiBlockInvalid(0,15,fdcard[1]);
	}

	MSG_LOG("FD needS writeData:\r\n");
	BCD_LOG(fdcard[0], 16, 1);
	BCD_LOG(fdcard[1], 16, 1);

	//写入信息
	for(i=0; i<5; i++){//读出卡内分段收费的信息,写入记录区最后一扇区。
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.RecodeKEYA,cardMessage.dealRecordSector[2]*4);
		MifareWrite(cardMessage.dealRecordSector[2]*4,fdcard[0]);
		MifareWrite((cardMessage.dealRecordSector[2]*4)+1,fdcard[1]);

		MifareRead(cardMessage.dealRecordSector[2]*4, buff);
		MifareRead((cardMessage.dealRecordSector[2]*4)+1, buff+16);
		if((memcmp(buff, fdcard[0], 16) == 0)&&(memcmp(buff+16, fdcard[1], 16) == 0)){
			break;
		}
		PcdRfReset(0);
		display(6, 0, "  请再次刷卡!   ", DIS_CONVERT);
		while(1){
			if(profile(0) == ST_OK){
				if(old_c_serial != c_serial){
					display(6, 0, " 请刷同一张卡!  ", DIS_CONVERT);
				}
				else{
					break;
				}
			}
			delayxms(100);
			beep(1, 50, 0);
		}
	}
	if(i >= 5){//写入错
		debugstring("End Write FD Error!");
		MessageBox(1, "写卡出错!");
		led_on(LED_RED);
		end_card();
		return;
	}
	cls();

	if((gFDderict < 2)&&(gFDStaIndex[gFDderict] < 100))
		strcpy((char*)buff, LineInfo[0].StationInfo[ucUP_Station-1].name);

	buff[12] = 0;
	strcat((char*)buff, "上车");
	display(0, 0, (char*)buff, 0);
	if(s_sum1 != 0){
		sprintf((char*)&buff, "扣款:%d.%02d", s_sum1/100, s_sum1%100);
		display(2, 0, (char*)buff, 0);
		sprintf((char*)&buff, "余额:%d.%02d", a_sum1/100, a_sum1%100);
		display(4, 0, (char*)buff, 0);
	}
	else{
		display(2, 0, "扣款:0.00元", 0);
		display(4, 0, "刷卡完成!", 0);
	}
	sprintf((char*)buff, "还需要刷%d次", pcard1->times);
	display(6, 0, (char*)buff, DIS_CENTER);
	
	sprintf((char*)buff, "%d.%02d", s_sum1/100, s_sum1%100);
	LED_Dis3((char*)buff);
	if(pcard1->times == 0)
		save_normal();
	led_on(LED_GREEN);
	audio(Audio_DONG);
	end_card();
}

//------------下载数据通讯函数组------
//下载文件参数。后门机用函数，向前门机请求数据.
void FDDownLoadInfo_HM(unsigned char DownMode)
{
	unsigned char sbuf[32];
	unsigned int slen=0;

	if(DownMode != FDDownPar.DownFlag){//类型不一致，从0开始
		memset(FDDownPar.DownPerSerial, 0, 2);
		memset(FDDownPar.DownSerial, 0, 2);
	}
	if((DownMode == FDProcess_GETGPSLineInfo)||(DownMode == FDProcess_GETFDticket)||(DownMode == FDProcess_GETPriceRate)||(DownMode == FDProcess_GETProgram)){
		FDDownPar.DownFlag = DownMode;
		memcpy(sbuf, (unsigned char*)&FDDownPar, 5);
		memset(sbuf+5, 0, 2);
		slen = 7;
	}
	else{	//其它的不知道是什么。
		return;
	}
	FDCMDData_Send(FDCMD_DOWNPar, sbuf, slen);
}

//不能刷卡时提示，显示不能刷卡原因。
int NoCardShow_ALL(void)
{
	int ret;
	char dspb[32];

	if(gFDRunStatues&FDRUNSTATUES_PJ){
		sprintf(dspb, "票价参数错误:%02X", gFDRunStatues);
		ret = 1;
	}
	else if(gFDRunStatues&FDRUNSTATUES_Line){
		sprintf(dspb, "线路信息错误:%02X", gFDRunStatues);
		ret = 2;
	}
	else if(gFDRunStatues&FDRUNSTATUES_TX){
		sprintf(dspb, "CAN通讯错误:%02X", gFDRunStatues);
		ret = 3;
	}
	else if(gFDRunStatues&FDRUNSTATUES_GPS){
		sprintf(dspb, "GPS定位错误:%02X", gFDRunStatues);
		ret = 4;
	}
	else if((gFDderict>=2)||(gFDucID == 0)||(gFDucID > 100))
	{
		sprintf(dspb, "未确定位置");
		ret = 5;
	}
	else if(gFDRunStatues != 0){
		sprintf(dspb, "其它错误:%02X", gFDRunStatues);
		ret = 6;
	}
	else{
		ret = 0;
	}
	if(ret != 0){
		cls();
		display(2,0,dspb, DIS_CENTER);
		sprintf(dspb, "错误:%d", ret);
		display(0,0,dspb,0);
		display(4, 0, "不接受刷卡", DIS_CENTER);
	}
	return ret;
}
//----------------------------------------------------
//前门机-读到卡的处理函数
void FengDuan_BUS_card(void)
{
	if(NoCardShow_ALL() != 0){//有错误，不能刷卡
		delayxms(500);
		end_card();
		restore_flag = 3;
		return;
	}

	if(gFDMode == FDMODE_NORMAL){
		if(gFDdealProcess <= FDProcess_GETProgram){//在更新各种参数时，后门机不能刷卡。
//			display(4, 0, "不能刷卡", DIS_CENTER);
			return;
		}
		FengDuan_Normal_card();
	}
	else{
		FengDuan_GPS_card();
	}
}

//显示界面
void FengDuan_disp(void)
{
	if(gFDMode == FDMODE_GPS)
		FengDuan_GPS_disp_QM();
	else if(gFDMode == FDMODE_NORMAL)
		FengDuan_Normal_disp_HM();
}

//把数据发送到前门机。
unsigned int FDUPDateRecord_HM(void)
{
	unsigned char sbuff[300];

	if(tcpipPacket.packetStatus == RECORD_DEAL){//有需要发送的数据
		if(tcpipPacket.length > 0){
			memcpy(sbuff, tcpipPacket.tcpBuffer, tcpipPacket.length);	//因为FDCMDData_Send函数中发送用同一个buff，所以需要开个buff
			FDCMDData_Send(FDCMD_UPDateREC, sbuff, tcpipPacket.length);
			return tcpipPacket.length;
		}
		else{
			tcpipPacket.packetStatus = 0;//此记录包不正常
			sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
		}
	}
	return 0;
}

//生成记录保存在铁电中。
unsigned int FDRecord_Build_HM(void)
{
	unsigned int curp, headp, len, addr;
	unsigned char rbuff[256];
	unsigned char disbuf[RECORD_LEN+8];
	
	if(tcpipPacket.packetStatus==RECORD_DEAL)
	{//铁电中有记录。
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
	//如果记录缓冲中没有记录可发将发送E2PROM中没有发成功的记录
	if(curp>headp)
	{
		MSG_LOG("Read Record From E2prom to CAN !\r\n");
		len=0;
		rbuff[len++] = 0x01;	//0x01-记录数据
		
		addr = headp/RECORD_LEN;
		memcpy(rbuff+len, (unsigned char*)&addr, 2);
		len += 2;

		
		addr = headp;
		flashread(addr, disbuf, RECORD_LEN);
		if(memcmp(rbuff+len, "银行数据", 8) == 0){//
			memcpy(rbuff+len, disbuf, (RECORD_LEN-8));
			len += (RECORD_LEN-8);//56
			
			if(addr < FLASH2_START1){//当前区才写0,历史区不写0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//把这条写成全0，
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}
			
			flashread(addr, rbuff+len, RECORD_LEN);
			len += RECORD_LEN;//64
			
			if(addr < FLASH2_START1){//当前区才写0,历史区不写0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//把这条写成全0，
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
		
		if(addr < FLASH2_START1){//当前区才写0,历史区不写0
			memset(disbuf, 0, RECORD_LEN);
			flashwrite(addr, disbuf, RECORD_LEN);//把这条写成全0，
		}
		
		tcpipPacket.packetStatus = RECORD_DEAL;
		tcpipPacket.length = len;//要发送的长度
		memcpy(tcpipPacket.tcpBuffer, rbuff, len);//要发送的数据
		sysfewrite(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket);

		MSG_LOG("CAN生成发送包完成:");
		BCD_LOG((unsigned char*)&headp, 4, 1);
		BCD_LOG((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
		BCD_LOG((unsigned char*)&tcpipPacket.length, 2, 1);
		BCD_LOG((unsigned char*)&tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
		return len;
	}
	else{
		if(curp >= (FLASH1_START+FLASH_RCLR)){//0x3E800如果记录地址已经达到了40000条（64字节），则重置存贮区，重新开始
			lib_clr_Record_point();
		}
		tcpipPacket.packetStatus = 40; //没有记录，等40秒。
		return 0;
	}
}

//每秒钟会进入一次。
void FD1secondPro_ALL(void)
{	
	//unsigned char i;
	char disbuf[32];
	//-------显示实时信息
	if(gFDderict < 2){
		if(gOlderStation != gFDucID){		//站点号有变化，需要更新显示
			gOlderStation = gFDucID;
			restore_flag = 3;	//需要重新刷新显示
			/*memset(disbuf, 0, 20);
			MSG_LOG("gOlderStation:%d gFDucID:%d\r\n", gOlderStation, gFDucID);
			if(gFDMode == FDMODE_GPS){
				i = gFDStaIndex[gFDderict];	//前门机用这个，这是算得到。
				if(strlen(LineInfo[gFDderict].StationInfo[i].name) <= 10)
					strcpy(disbuf, "位置:");
			}
			else{
				i = gFDucID-1;				//后门用这个变量。这个变量是从前门机查询信息后得到的。//减1是因为序号都是从1开始的。
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
		FDRecord_Build_HM();//把要发的记录移动到铁电待发区。
	}
}

void FDFindMiss_HM(void)
{
	if(gFDmissflag & FDMISS_GETLine){//需要取线路信息
		gFDdealProcess = FDProcess_GETGPSLineInfo;
		if(FDDownPar.DownFlag != FDProcess_GETGPSLineInfo){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else if(gFDmissflag & FDMISS_GETPriceRate){//需要票价折扣率,先取折扣表。然后再取阶梯票价。因为是否更新是从阶梯票价的版本号更新的。
		gFDdealProcess = FDProcess_GETPriceRate;
		if(FDDownPar.DownFlag != FDProcess_GETPriceRate){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else if(gFDmissflag & FDMISS_GETTicket){//需要阶梯票价
		gFDdealProcess = FDProcess_GETFDticket;
		if(FDDownPar.DownFlag != FDProcess_GETFDticket){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else if(gFDmissflag & FDMISS_GETProgram){//需要更新程序
		gFDdealProcess = FDProcess_GETProgram;
		if(FDDownPar.DownFlag != FDProcess_GETProgram){
			memset((unsigned char*)&FDDownPar, 0, sizeof(FDDownPar));
		}
	}
	else{
		gFDdealProcess = FDProcess_GETFree;
	}
	restore_flag = 3;	//重新找任务后，重新刷新显示。
	gFDScount = 0;	//立刻执行
	set_timer1(1000);
}
//分段主函数,处理数据及命令
void FengDuan_main(void)
{
	unsigned char Tbuf[16];	//定义数据发送缓冲，组织数据发送时，此缓冲的数据会改变为完整的发送指令，所以要预留足够的空间。
	CanFDdealCMD_ALL(irda_rxBuf);

	if(gFDMode == FDMODE_NORMAL){	//只有后门机才会有一系列任务
		switch(gFDdealProcess){
		case FDProcess_GETFind:
			FDFindMiss_HM();	//查询任务，只有后门机会执行。
			break;
		case FDProcess_GETVER:
			if(gFDScount==0){//取前门机版本号
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
				if((gFDGetGPSinfo%4)==0){//4秒钟，取一次前门信息。
					MSG_LOG("取信息 \r\n");
					FDCMDData_Send(FDCMD_GPSINFO, Tbuf, 0);
					gFDGetGPSinfo++;
				}
				else{
					if(FDUPDateRecord_HM() != 0){//发送记录, =1有记录上发
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





