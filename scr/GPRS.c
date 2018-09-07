
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"

#include "szct.h"

//#define _debug_gprs
//#define _debug_time	//


//V3 : 加入4G模块
#define SOFT_VAL 4	//库版本



extern unsigned int str2int(unsigned char *pstr, unsigned char len);
extern unsigned char checkIP(char *IPb);

extern void savesysbupara(void);
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern unsigned char SPI_Snd_dat(unsigned short len, unsigned char *dat, unsigned char cmd);
extern unsigned char GetDateTime(void);
//extern unsigned char GprsSend(unsigned short serialno,unsigned char cmd,unsigned short len,unsigned char *buff);
extern unsigned char DealStationBTM(unsigned int len, unsigned char *info);
extern void UPData_Line_Info_gprs(unsigned char *info);
extern void gprs_snd(unsigned int len, char *dat);
extern void uploadMobilePara(void);
extern void first_clr_mobile_para(void);
extern unsigned char GJDataDeal(unsigned char *pakege);
extern void getMobilePara(void);
extern void stopCCgprs(unsigned short mode);
extern void find_G_new_mission(void);
extern void KeyPad_restordis(void);
extern void in_deal_ring(void);
extern int in_dealSoundIn(void);
extern int in_checkRingstyle(void);
extern int in_sendGPs(void);
extern void GPRS_MC55i_open(void);
extern void GPRS_open(void);
extern unsigned char in_getGprsModeStyle(void);
extern void in_dispGprsProcess(void);

extern void sendmsgHead(void);
extern void sendmsgBody(void);
extern void SendmsgEnd(void);

extern void in_gErrortimes(unsigned char val, unsigned char mode);
extern void in_decgM_Senddll(void);
extern void in_IF2TLVDeal(unsigned char sindex);
extern void in_IF18583Deal(unsigned char sindex);
extern void set_KEK_SN(unsigned char val);
extern int checkKEK(void);

extern void powerOff4V0(void);
extern void powerON4V0(unsigned int baund);

extern unsigned char gmissOldflag;
extern volatile unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.

extern stbusdownPara gBusblkDownPro;//黑名单下载
extern stbusdownpro gBusDownPro;
extern stGPSdownLine gGPSDownLine;
extern pFistVary_1 pFistVary;
extern stcomtime SysTime;
extern unsigned int a_sum1,s_sum1;
extern stBusVerIfo gBusVerInfo;
extern unsigned short gErrorFlag;//错误标识,高字节表示错误编号，低字节表示错次数据 详见"*错误编码标识定义"
extern unsigned int gErrortimes[2];//错误次数据,主要是用于GPRS连接

extern unsigned char gchn_eng;//中英文标志
extern volatile unsigned char gpacSNo;//包序号
//GPRS主板状态
volatile unsigned char gAPN_Style;//APN状态，是专用网还是公网
unsigned char gGPSdly;//GPS延时
unsigned short gSendOverTime;//发送后收应答超时计数器
unsigned char omissDly;
extern unsigned int gSendGLogin;//登陆命令 用这个查询信号

TCP_IP_PACKET1 tcpipPacket;//500
volatile unsigned char GPRSLinkProcess;
										//gprs的中断信息 每次拨号前复位	
PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//接收帧缓冲	//发送队列缓冲

volatile unsigned int CDMA_REV_LEN;//需要接收数据的长度,如果是0xffffffff为没有数据需要接收。

unsigned char gTcpIpbusy;//模块是正正在发数据，模块工作是发命令，然后回应后再发数据。在等回应时不能发其它数据。
volatile unsigned int guiMC323_REV_LEN;//需要接收数据的长度,如果是0xffffffff为没有数据需要接收。
unsigned char gTcpIpbusydly;//收不到空闲延时处理
unsigned char gMC55iSendbuff[256];//MC55i发送数据缓冲，前8字节为：“数据有效”
unsigned char tcpipsdnode;
stTcpIpSp gTcpIPsd[4];//发送暂存，不能超过256字节一个包
volatile stGPrsDataStyle gGPRS_data_style;

#define GPRSCOMMAX 1024+64
unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS接收缓冲
volatile unsigned short gprsIndex;

#define GPRS_CLEAR_INDEX	0x55	//处理后可以清理缓冲区
#define GPRS_DATA_OK		0xaa	//数据准备好
#define GPRS_GOON			0		//继续收数据
volatile unsigned char gprsReady=GPRS_GOON;//收到GPRS回传的数据,需要处理.

unsigned int ClientIP;
unsigned short ClientPort;

unsigned char gprsSingle;//GPRS信号强度
unsigned char gprsAnswer;//ＧＰＲＳ模块回应ＯＫ标志
unsigned char gprsSendDlyflag;//模块发送是否延时标志
unsigned char isNetOK[2];//两路链接是否成功

unsigned char gprsErrorTimes;//出现ＥＲＲＯＲ返回错误次数
unsigned char gprsSNDerro;//和后台通讯出现的错误
unsigned char gTTime[14];//保存中心发下来的日期时间
unsigned char gGPRS_Cmd_outtime;//GPRS包超时计数
unsigned char GprsCardList[16];


#define  ChinaMobile   1
#define  ChinaUnicom   2
#define  ChinaTelecom  3 
unsigned char ugISMI_type;	///全网通时指示当前卡的运营商

extern unsigned char restore_flag;

extern unsigned short getCpuPort(void);
extern unsigned int getCpuIPaddr(void);
extern void GPRSSASK(unsigned char *dat, unsigned char cmd, unsigned int len);



unsigned short BCC( unsigned char *Bcc_pointer,unsigned short x)					//生成BCC校验码
{
	unsigned char data;
	unsigned char k,h;
	unsigned char BCC_L=0;
	unsigned char BCC_H=0;
	unsigned short  BCC=0;
	unsigned short i,j;

	for(i=0;i<x;i++)
	{
		data= *(Bcc_pointer+i);
		for(j=0;j<8;j++)
		{
			if(BCC_H&0x01) k=1;
			else k=0;
			BCC_H=BCC_H>>1;                 //保存高字节。
			if(BCC_L&0x01) h=1;
			else h=0;
			BCC_L=BCC_L>>1;
			if(k) BCC_L=BCC_L|0x80;
			else BCC_L=BCC_L&0X7f;
			if(h) BCC_H=BCC_H|0x80;
			else BCC_H=BCC_H&0x7f;
			if(data&0x01) h=1;
			else h=0;
			data=data>>1;
			if(h)
			{
				BCC_H=BCC_H^0x80;
				goto bcc_1;
			}
			else
			{
bcc_1:
			if(BCC_H&0x80)
			{
				BCC_H=BCC_H^0x04;
				BCC_L=BCC_L^0x08;
			}
			}   
		}
	}
	BCC=0;
	BCC=BCC|BCC_H;
	BCC=BCC<<8;
	BCC=BCC|BCC_L;
	return(BCC);
}

//只用于发送给MODEM模块，ST的车载机应该用不上。
void gprs_ModSend(unsigned char Scom, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos=0;
	unsigned int CRC;
	unsigned short i;
	
	memset(buff, '$', 4);//包头
	pos = 4;
	
	i = len+10;
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;
	
	buff[pos++] = 0x01;//公交数据，直接发送到后台，此字节不需要发送
	buff[pos++] = gpacSNo;//包序号
	memcpy(buff+pos, dat, len);
	pos += len;
	
	CRC = 0xFEFE;//crc16_isr((unsigned char*)buff, pos);
	
	memcpy(buff+pos, (unsigned char*)&CRC, 2);
	//over_turn(2, buff+pos);
	pos += 2;
	com_snd(Scom, pos, (unsigned char*)buff);
}

extern unsigned char getGprsCom(void);
void gprs_snd(unsigned int len, char *dat)
{
	unsigned char sndcom;
	unsigned char sndGMode;

	sndcom = getGprsCom();
	
	sndGMode = in_getGprsModeStyle();
	sndGMode &= 0xF0;
	if(sndGMode == 0x80){//发送给模块，需要封装
		gprs_ModSend(sndcom, len, (unsigned char*)dat);
	}
	else{
		clr_wdt();
#ifdef _debug_gprs
// 		debugstring("Send:");
//  		debugstring(dat);
// 		debugstring("\r\n");
#endif

		com_snd(sndcom, len, (unsigned char*)dat);
	}
}

void initTCPIPnode(void)
{
	memset((unsigned char*)&gTcpIPsd[0], 0, 10);
	memset((unsigned char*)&gTcpIPsd[1], 0, 10);
	memset((unsigned char*)&gTcpIPsd[2], 0, 10);
	memset((unsigned char*)&gTcpIPsd[3], 0, 10);
	
	tcpipsdnode = 0;
	
}
void TcpIPSd_SAVE(unsigned char *dat, unsigned int len)
{
	unsigned char i;

#ifdef _debug_gprs
	debugstring("TcpIpsd save --\r\n");
#endif

	if((len > 1024)||(len == 0)){
#ifdef _debug_gprs
		debugstring("TcpIpsd Save Data too long!!\r\n");
#endif
		return;
	}//BDBDBD226868752001020000000000072012070508265618E6
	for(i=0; i<4; i++){//找相同的数据包
		if(gTcpIPsd[i].T == 0xE501){//
			if(memcmp(gTcpIPsd[i].V+GCMD_CmdCode, dat+GCMD_CmdCode, 2) == 0){
// 				debugstring("有相同的命令，壶盖！\r\n");
// 				debugdata((unsigned char *)gTcpIPsd[i].V, 10, 1);
// 				debugdata((unsigned char *)dat, 10, 1);
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if(i >= 4){//没有找到相同包
		for(i=0;i<4;i++){//找空余地方
			if(gTcpIPsd[i].T != 0xE501){//空表
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if(i>=4){//没有找到空闲的，用最后
		if(tcpipsdnode > 0)//
			i = tcpipsdnode-1;
		else
			i = 3;

		gTcpIPsd[i].T = 0xE501;
		gTcpIPsd[i].L = len;
		memcpy(gTcpIPsd[i].V, dat, len);
	}
#ifdef _debug_gprs
// 	for(i=0;i<4;i++){
		debugstring("TcpIPSd_SAVE:");
		debugdata((unsigned char *)&gTcpIPsd[i], gTcpIPsd[i].L +4, 1);
// 	}
#endif
}

//取出链表中要发送数据
unsigned short TcpIPSd_Pick(unsigned char *dat)
{
	unsigned char i;
	unsigned short rLen;
	
	rLen = 0xFFFF;
	if(tcpipsdnode > 3)
		tcpipsdnode = 0;
	for(i=0; i<4; i++){
		if(gTcpIPsd[tcpipsdnode].T == 0xE501){
			rLen = gTcpIPsd[tcpipsdnode].L;
			memcpy(dat, gTcpIPsd[tcpipsdnode].V, gTcpIPsd[tcpipsdnode].L);
			memset((unsigned char*)&gTcpIPsd[tcpipsdnode], 0, 10);//清除
			tcpipsdnode++;
#ifdef _debug_gprs
			debugstring("TCPip_pick OK:");
			debugdata((unsigned char*)&rLen, 2, 1);
#endif
			break;
		}
		else{
			tcpipsdnode++;
			if(tcpipsdnode>3)
				tcpipsdnode = 0;
		}
	}
#ifdef _debug_gprs
	if(rLen != 0xFFFF)
	{
		debugstring("TcpIPSd_Pick:");
		debugdata(dat, rLen, 1);
	}
#endif
	return rLen;
}

//发送所有ＧＰＲＳ包，linkNum = 发送的链路选择
void gprs_send_data_MG323(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
	char buff[50];

	if(len != 0)
		TcpIPSd_SAVE(dat, len);//保存

	if(gTcpIpbusy == TcpIPBusy){//不能发送，需等待，保存
//		debugstring("@");
		if(gTcpIpbusydly == 0){
			gTcpIpbusy = TcpIPFree;
#ifdef _debug_gprs
			debugstring("()");
#endif
		}
		return;
	}
	if ((gGPRS_data_style.ucReadStyle == GPRS_SISR)||(gGPRS_data_style.ucReadStyle == GPRS_SISRing)){//正在取数据，不能发数据
#ifdef _debug_gprs
//		debugstring("{}");
#endif
		return;
	}

	gTcpIpbusydly = 5;
	len = TcpIPSd_Pick(gMC55iSendbuff+12);
	if((len==0)||(len> 1024)){
		return;
	}
	memcpy(gMC55iSendbuff, "数据有效", 8);
	memcpy(gMC55iSendbuff+8, (unsigned char*)&len, 4);
	//	memcpy(gMC55iSendbuff+12, dat, len);
	sprintf(buff, "at^sisw=0,%d\r\n", len);
	gprs_snd(strlen(buff), buff);
	gTcpIpbusy = TcpIPBusy;
}

//发送所有ＧＰＲＳ包，linkNum = 发送的链路选择
void gprs_send_data_MC323(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
	unsigned int index;
#ifdef _debug_gprs
	debugstring("Gprs_send_data:");
	debugdata((unsigned char*)&len, 1, 1);
#endif
	if (len == 0){
		return;
	}
#ifdef _debug_gprs
	debugstring("Gprs_send_data\r\n");
#endif

	index = 0;
//	sprintf(buff, "AT%%IPSENDX=%d,\"", linkNum);
	memcpy(comSendbuff, "AT^IPSENDEX=1,1,\"",17);
	index = 17;

	BCD2Ascii(dat, comSendbuff+index, len);
	index += (len*2);
	memcpy(comSendbuff+index, "\"\r\n\x0", 4);
	index += 4;
	clr_wdt();
	gprs_snd(index, (char*)comSendbuff);
}

void ModeS_IPSend(unsigned int len, unsigned char Gmode)
{
	unsigned int index = 0; 
	memset(comSendbuff,0,50);
	index = 0;
	if(Gmode == Gmode_ALL4G)
		index = sprintf((char *)comSendbuff,"AT+QISEND=0,%d\r\n",len);
	else	//CDMA2000
		index = sprintf((char *)comSendbuff,"AT^IPSEND=1,%d\r\n",len);
	
#ifdef _debug_gprs
	debugstring("SEND_SEND:");
	debugstring((char *)comSendbuff);
	
	debugstring("\r\nwait >\r\n");
	printf("gprsRevBuf[gprsIndex]=%d\r\n",gprsIndex);
	debugdata(gprsRevBuf,gprsIndex,1);	
#endif

	gprs_snd(index, (char*)comSendbuff);


	gTcpIpbusy = TcpIPBusy;
	gTcpIpbusydly = 3;		//
		
}

//收到>号后发送数据
void ModeS_DataSend(unsigned int len, unsigned char *dat)
{
	gprsAnswer = 0;
//	memcpy(dat+len, "\x0d\x0a", 2);
#ifdef _debug_gprs
	debugstring("ModeS DataSend:");
	debugdata(dat,len,1);
#endif
	gprs_snd(len, (char*)dat);

}


//#define GPRS_WRITE_WAIT	0xE4	//用于3G 模块发送ipsende 后等待串口的“>”标志，这个时候就要送数据了


//发送所有ＧＰＲＳ包，linkNum = 发送的链路选择  只保存数据
void gprs_send_data_CDMA3G(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
//	unsigned int index;

	if (len == 0){
#ifdef _debug_
		debugstring("0 return---\r\n");
#endif
		return;
	}



	if(len != 0)
		TcpIPSd_SAVE(dat, len);//保存


// 	  	if((gTcpIpbusy != TcpIPFree)){//不能发送，需等待，保存
// 			if(gTcpIpbusydly == 0){
// 				gTcpIpbusy = TcpIPFree;
// 				gGPRS_data_style.ucSendstyle = 0;
// 				gGPRS_data_style.ucReadStyle = 0;
// #ifdef _debug_gprs
// 				debugstring("((())");
// #endif
// 			}
// 		}

// 		memcpy(gMC55iSendbuff, "数据有效", 8);
// 		memcpy(gMC55iSendbuff+8, (unsigned char*)&len, 4);
// 		memcpy(gMC55iSendbuff+12,dat,len);
// 		gGPRS_data_style.uiNum = len;	
		


}
//发送所有ＧＰＲＳ包，linkNum = 发送的链路选择
void gprs_send_data_GT900(unsigned char linkNum, unsigned int len, unsigned char *dat)
{					
	char buff[50];
	unsigned int index;
	unsigned short t,t1;
	
#ifdef _debug_gprs
	debugstring("Gprs_send_data:");
	debugdata((unsigned char*)&len, 1, 1);
#endif
	if (len == 0){
		return;
	}

	if(checkKEK() < 0) 
		return;
#ifdef _debug_gprs
	debugstring("Gprs_send_data\r\n");
	debugdata((unsigned char*)&gprsAnswer, 1, 1);
#endif
	if(gprsAnswer == 'O'){
		if(len > 512){
			if(gprsSendDlyflag != 'T'){
				gprsSendDlyflag = 'T';
				strcpy((char*)buff, "AT%IPSP=1000\r\n");//
				gprs_snd(strlen(buff), buff);
				set_timer0(50, 2);
				while(gprsAnswer != 'O'){
					clr_wdt();
					if(get_timer0(2)==0)
 						break;
				}
			}
		}
		else{
			if(gprsSendDlyflag != 'N'){
				gprsSendDlyflag = 'N';
				strcpy((char*)buff, "AT%IPSP=0\r\n");//
				gprs_snd(strlen((char*)buff), buff);
				set_timer0(50, 2);
				while(gprsAnswer != 'O'){
					clr_wdt();
 					if(get_timer0(2)==0)
 						break;
	//				debugdata((unsigned char*)&gprsAnswer, 1, 0);
				}
			}
		}
		if(len > 512){//一个包发送不完,需要采用延时发送			
			index = 0;
			memcpy(comSendbuff, "AT%IPSEND=\"",11);
			index = 11;

			t = len/2;//先发送一半
			BCD2Ascii(dat, comSendbuff+index, t);
			index += (t*2);
			memcpy(comSendbuff+index, "\"\r\n\x0", 4);
			index += 4;
			clr_wdt();
			gprsAnswer = 0;
			gprs_snd(index, (char*)comSendbuff);
			set_timer0(50, 2);
			while(gprsAnswer != 'O'){
				clr_wdt();
//				debugdata((unsigned char*)&gprsAnswer, 1, 0);
				if(get_timer0(2)==0)
					break;
			}
			
			index = 0;
			memcpy(comSendbuff, "AT%IPSEND=\"",11);
			index = 11;
			
			t1 = len-t;//再发另一半
			BCD2Ascii(dat, comSendbuff+index, t1);
			index += (t1*2);
			memcpy(comSendbuff+index, "\"\r\n\x0", 4);
			index += 4;
			clr_wdt();
			gprs_snd(index, (char*)comSendbuff);
		}
		else{
			index = 0;
//			sprintf(buff, "AT%%IPSENDX=%d,\"", linkNum);
			memcpy(comSendbuff, "AT%IPSEND=\"",11);
			index = 11;

			BCD2Ascii(dat, comSendbuff+index, len);
			index += (len*2);
			memcpy(comSendbuff+index, "\"\r\n\x0", 4);
			index += 4;
			clr_wdt();
			gprs_snd(index, (char*)comSendbuff);
		}
	}
}
extern void gprs_send_data_OTHER(unsigned char linkNum, unsigned int len, unsigned char *dat);
extern void gprs_send_data_WIFI_TLH(unsigned char linkNum, unsigned int len, unsigned char *dat);
void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat)
{
	unsigned char gprsMode;
	gprsMode = in_getGprsModeStyle();
	gprsMode &= 0x0F;
	if(gprsMode == Gmode_900C){
		gprs_send_data_GT900(linkNum, len, dat);
	}
	else if(gprsMode == Gmode_MC323){
		gprs_send_data_MC323(linkNum, len, dat);
	}
	else if((gprsMode == Gmode_CDMA2000)||(gprsMode == Gmode_ALL4G)){
		gprs_send_data_CDMA3G(linkNum, len, dat);
	}
	else if(gprsMode == Gmode_OTHER){
		gprs_send_data_OTHER(linkNum, len, dat);
	}
	else if(gprsMode == Gmode_WIFI_TLH){
		gprs_send_data_WIFI_TLH(linkNum, len, dat);
	}
	else{
		gprs_send_data_MG323(linkNum, len, dat);
	}
}


//关闭GPRS电源
void GPRS_close_Gt900(unsigned int dlyt)
{
	char buff[50];
	strcpy(buff, "AT%MSO\r\n");
	gprs_snd(8, buff);
	//MC55i不能关闭电源，但是可以关闭模块
	set_timer0(dlyt, 1);//收到OK， 才关闭成功	
}
//GPRS包格式转换 
unsigned short PeckData(FRAME_BUF *dst, char *scr)
{
	char *sp1, *sp2;
	unsigned int t;
//	unsigned int pos;
	unsigned short l[2];
	
//	pos = 0;
//	i = 0;
	
	l[0] = 0;
	l[1] = 0;
	sp1 = strstr(scr, ":");
	if(sp1 == 0)
		return 0;
	sp2 = strstr(scr, ",");
	if(sp2 == 0)
		return 0;
	sp1++;
	while(sp1 < sp2){
		l[0] = l[0]*10 + (*sp1 - '0');
		sp1++;
	}
	sp1 = sp2+1;
	sp2 = strstr(sp1, ",");
	if(sp2 != 0){
		while(sp1 < sp2){
			l[1] = l[1]*10 + (*sp1 - '0');
			sp1++;
		}
	}
	sp2 = strstr(sp1, "\"");
	if(sp2 != 0){
		sp2++;
		if(l[1] > 0)
			t = 1;
		else
			t = 0;
		if(l[t] > 1024)//长度太大
			return 0;
		Ascii2BCD((unsigned char*)sp2, dst->FrameBuf, l[t]*2);
		memcpy(dst->uiFrameLen, (unsigned char*)&l[t], 2);
		dst->ucFrameType = t;
		return l[t];
	}

	return 0;
}
//mode == 1 关闭链接1 mode==2:关闭链接2 mode==0:关闭单链接
void tcpipClose(unsigned char mode)
{
	char buffer[30];
	unsigned char gGprsMode;
	
	memset(gBusVerInfo.newbusBLKVer, 0, 2);//断开链接后要重新登陆
	gmissflag = 0;

	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;
	

#ifdef _debug_gprs
	debugstring("tcpip Close--\r\n");
#endif
	if(((GPRSLinkProcess >= 20)&&(GPRSLinkProcess < 0x30))||(GPRSLinkProcess == 0xA0))
		;
	else{
		GPRSLinkProcess = 0;
		return;
	}

	if(gGprsMode == Gmode_WIFI_TLH){
		GPRSLinkProcess = GPRS_NEED_CLOSEIP;
		return;
	}
	set_timer0(5000, 1);
	
	if((gGprsMode == Gmode_MC323)||(gGprsMode == Gmode_CDMA2000)){
		strcpy(buffer, "AT^IPCLOSE=1\r\n");//关闭链接
	}
	else if(gGprsMode == Gmode_MG323){
		strcpy(buffer, "at^sisc=0\r\n");//关闭链接
	}
	else if(gGprsMode == Gmode_ALL4G){
		strcpy(buffer, "AT+QICLOSE=0\r\n");//关闭链接
	}
	else{
		if(mode == 1){
			strcpy(buffer, "AT%IPCLOSE=1\r\n");//关闭链接
		}
		else if(mode == 2){
			strcpy(buffer, "AT%IPCLOSE=2\r\n");//关闭链接
		}
		else if(mode == 5){
			strcpy(buffer, "AT%IPCLOSE=5\r\n");//关闭链接
		}
		else{
			strcpy(buffer, "AT%IPCLOSE\r\n");//关闭链接
		}
	}
	gGPRS_data_style.ucReadStyle = 0;
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gAPN_Style = 0;
	gprs_snd(strlen(buffer), (char*)buffer);
	GPRSLinkProcess = GPRS_TCPCLOSE_CMD;
}

void out_GPRS(void)
{
	char buffer[20];
	unsigned char gGprsMode;
	
	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;
	if(gGprsMode == Gmode_MC323){
		strcpy(buffer, "AT%CLOSE=7\r\n");//NOTE：PDP 去激活
	}
	else{
		strcpy(buffer, "AT+CGATT=0\r\n");//NOTE：PDP 去激活
	}
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gprs_snd(strlen(buffer), (char*)buffer);
	GPRSLinkProcess = GPRS_Out_CMD;
	set_timer0(30000, 1);
}

void IPERROR_DEAL(unsigned char mode)
{
#ifdef _debug_gprs
	debugstring("IP ERROR DEAL:");
	debugdata((unsigned char*)&mode, 2, 1);
#endif
	switch (mode)
	{
	case 0:	//用户名和密码注册被网络侧拒绝，或者APN 错误，卡欠费，卡不支持GPRS 业务等。
//		set_timer0(10000, 1);
		GPRSLinkProcess = 0;
				debugstring("--2 close\r\n");
//		display(0,4,"移动欠费",1);
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		break;
	case 1://没有登陆网络
	case 21://当前没有完成PDP 激活
//		set_timer0(5000, 1);
		GPRSLinkProcess = 0;
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		break;
	case 2://当前已经有链接存在
		if((GPRSLinkProcess >= 20)&&(GPRSLinkProcess<0xB0))
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;// (0);
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		break;
	case 4://TCP 连接被对方拒绝
	case 5://TCP 连接超时，可能IP 和端口不正确
		set_timer0(5000, 1);//
		if(GPRSLinkProcess > 20)
			GPRSLinkProcess = 20;
		break;
	case 9://当前没有TCP 或UDP 连接存在(针对IPSEND)
		set_timer0(5000, 1);//
		if(GPRSLinkProcess > 20){
			GPRSLinkProcess = 20;
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
		}
		break;
	case 20://TCP 发送缓冲区满
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		//(0);
		break;
	case 7://当前已经启动单连接命令
		if(GPRSLinkProcess > 19)
			GPRSLinkProcess = 19;
		break;
	case 8://当前处于有连接存在，不能修改link_flag（针对IOMODE）
//		GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
		break;
	case 3://输入参数不合法
	case 6://当前已经启动多连接命令
	case 10://
	case 11://接收缓存满
	case 12://接收缓存无数据
	case 13://输入有效数据超过512 字节
	case 14://在用户模式下，字节数应该为偶数个数
	case 15://用户模式下输入出现非法字符
	case 16://该序号不存在
	case 17://非法的删除类型
	case 18://其他错误
	case 19://
	case 22://当前%ETCPIP 处于激活状态，不能使用该命令
	case 23://域名不存在
	case 24://域名解析超时
	case 25://域名解析未知错误
	case 26://服务器已开启
	default:
		break;
	}
}

void gprsCmdDeal_CDMA(void)
{
	char *ret;
	unsigned int i, index;
	
	if(strstr((char*)gprsRevBuf, "^MODE:2") != 0)
	{
		gprsErrorTimes = 0;
		
		GPRSLinkProcess = GPRS_LINK_UP;//需要把POWN脚拉高，GPRS_AT_WAIT;	//GPRS模块重启了
		set_timer0(400, 1);//链接关闭,延时后重新连接
		
		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--MC323 ready\r\n");
#endif
		goto Gprsdealcmdend;
	}
	if(strstr((char*)gprsRevBuf, "^MODE:0") != 0){
		
		gprsErrorTimes = 0;
		
		GPRSLinkProcess = GPRS_LINK_CMD;//需要把POWN脚拉高，GPRS_AT_WAIT;	//GPRS模块重启了
		set_timer0(400, 1);//链接关闭,延时后重新连接
		
		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--MC323 OFF \r\n");
#endif
		goto Gprsdealcmdend;
	}

// 	if(strstr((char*)gprsRevBuf, "^HCMGSS:") != 0){//发送完成
// 		SendmsgEnd();
// 		set_timer0(40, 1);
// 		goto Gprsdealcmdend;
// 	}
// 	if(strstr((char*)gprsRevBuf, "^HCMGSF:") != 0){//发送失败
// 		gmissflag = 0;
// 		goto Gprsdealcmdend;
// 	}

// 	if(strstr((char*)gprsRevBuf, "remote close") != 0){//服务器关闭
// 		set_timer0(1000, 1);
// #ifdef _debug_gprs
// 		debugstring("remote close!!!!\r\n");
// #endif
// 		if((GPRSLinkProcess == 0xA0)||(GPRSLinkProcess == 0x16)){
// 			GPRSLinkProcess = GPRS_NEED_CLOSEIP;
// 		}
// 		goto Gprsdealcmdend;
// 	}
	if(gmissflag == MISS_G_MSG2){
		if(strstr((char*)gprsRevBuf, "OK") != 0)
			goto Gprsdealcmdend;
	}

	if(GPRSLinkProcess == GPRS_AT_WAIT)
		goto Gprsdealcmdend;

	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I关闭中
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//关闭成功 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//关闭不成功 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
	}
	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 19:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			if(GPRSLinkProcess == 15){///有可能已经连上了
				GPRSLinkProcess++;
				set_timer0(500, 1);
				gprsErrorTimes = 0;
				gprsAnswer = 'O';
				goto Gprsdealcmdend;
			}
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case GPRS_TCPCLOSE_CMD://TCPIP关闭中，等待OK返回
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if((GPRSLinkProcess > 20)&&(GPRSLinkProcess<0x30))
				GPRSLinkProcess = 20;// 重新连接
			set_timer0(500, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		goto Gprsdealcmderror;
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			for(i=5; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
		goto Gprsdealcmdend;
//		break;
	case 17:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(10, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
//		break;
	case 18:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, "%DTUIP");
			if(ret == 0)
				goto Gprsdealcmdend;
			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			if(strstr((char*)gprsRevBuf, "0.0.0") != 0){//1：PPP 拨号未成功
				set_timer0(15000, 1);
				GPRSLinkProcess = 16;
			}
			else{
				GPRSLinkProcess++;
				set_timer0(10, 1);
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else
			return;
//		break;
	case 20:
		debugstring("CMD:");
		debugstring((char*)gprsRevBuf);
//		ret = strstr((char*)gprsRevBuf, "%TCPSTATUS");
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if((gmissflag & 0xf0)==0x10){//登陆移动IF1
				isNetOK[0] = 1;
			}
			else if((gmissflag & 0xf0)==0x20){//登陆移动IF2
				isNetOK[0] = 2;
			}
			else{//登陆公交数据中心
				isNetOK[0] = 3;
			}
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			CDMA_REV_LEN = 0;
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
//			if(strstr((char*)gprsRevBuf,"Connection already established") != 0){
			GPRSLinkProcess = 15;//GPRS_NEED_CLOSEIP;//(0);
			set_timer0(500, 1);
			goto Gprsdealcmdend;
//			}
// 			else
// 				goto Gprsdealcmderror;
		}
		else{
			return;
		}
//		break;
	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
//		break;
	case 5:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			ret = strstr((char*)gprsRevBuf, "-");
			if(ret != 0){
				if(*(ret+1) != '0'){
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else 
			return;
//		break;
	default:
		break;
	}

	ret = strstr((char*)gprsRevBuf, "^IPDATA:");//^IPDATA:1,10,1234567890

	if(ret != 0){
		ret = strstr((char*)gprsRevBuf, ",");//
		if(ret == 0)
			goto Gprsdealcmdend;
		else{//将有数据接收
			ret++;
			CDMA_REV_LEN = 0;
			for(i = 0; i<gprsIndex;i++){
				if((*ret >= '0')&&(*ret <= '9')){
					CDMA_REV_LEN *= 10;
					CDMA_REV_LEN += (*ret-'0');
				}
				else if((*ret == 0x0d)||(*ret == 0x0a)||(*ret == ','))
					break;
				ret++;
			}
#ifdef _debug_
			debugstring("R LEN:");
			debugdata((unsigned char*)&CDMA_REV_LEN, 4, 1);
#endif
			if(gprsIndex >= (CDMA_REV_LEN+15)){//^IPDATA:1,10,1234567890 数据加上前面的字节，再加上0d 0a OK致少多了14字节
#ifdef _debug_gprs
				debugstring("R OK:");
#endif
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(index=0;index<MAX_RCV_PACKET_NUM;index++){
					if(gPacketRcvBuf[index].ucItemStatus != 0xea)
						break;
				}
				if(index>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
					return;
				}

				for(i=0; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				for(; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				if(i >= 16){
					CDMA_REV_LEN = 0;
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);
					return;
				}
				gPacketRcvBuf[index].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&CDMA_REV_LEN, 2);
				memcpy(&gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf+i, CDMA_REV_LEN);
				gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_gprs
				debugstring("接收:");
				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, CDMA_REV_LEN, 1);
#endif
				CDMA_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				goto Gprsdealcmdend;
			}

			return;
		}
	}	
	if(strstr((char*)gprsRevBuf, "^DSDORMANT:") != 0){
		for(i=11; i<gprsIndex; i++){
			if(gprsRevBuf[i] == 0x0d)
				break;
			else if(gprsRevBuf[i] == '1'){//休眠了
				gprs_send_data_MC323(1, 1, gprsRevBuf);//
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmderror:
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
		if(GPRSLinkProcess == 0xA0){
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;
			goto Gprsdealcmdend;
		}
		gprsErrorTimes++;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				IPERROR_DEAL(i);
			}
		}
		goto Gprsdealcmdend;
	}

	ret = strstr((char*)gprsRevBuf, "^IPCLOSE");
	if((ret != 0)||(strstr((char*)gprsRevBuf, "close")!=0)||(strstr((char*)gprsRevBuf, "CLOSE")!=0)){
		set_timer0(500, 1);//链接关闭,延时后重新连接
//		gprsErrorTimes = 0;
		for(i=0;i<(strlen((char*)gprsRevBuf));i++)
		{
			if(gprsRevBuf[i] == '5'){
				GPRSLinkProcess = 0;
				break;
			}
			else if((gprsRevBuf[i] == 0x0a)||(gprsRevBuf[i] == 0x0d))
				break;
		}
		if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
			GPRSLinkProcess = 20;
		isNetOK[0] = 0;

		memset(gBusVerInfo.newbusBLKVer, 0, 2);
		gmissflag = 0;
#ifdef _debug_
		debugstring("--IPCLOSE--");
		debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
#endif
		goto Gprsdealcmdend;
	}
Gprsdealcmdend:
	if(gprsErrorTimes > 20){//错误次数太多，重启模块
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//关闭1S
//		debugstring("--1 close\r\n");
		if(GPRSLinkProcess == 20){
			if(gmissflag & MISS_GJ){//如果正在登陆公交,登不上去,先暂停,做个标志
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return; //如果不是同时有问题,则退出重新找任务.
			}
		}
// 		if(GPRSLinkProcess == 20){//登陆不了后台
// 			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
// 		}
// 		else
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
	gprsIndex = 0;
	memset(gprsRevBuf, 0, sizeof(gprsRevBuf));
	return;
}

//sin 开始找 num 个 s2时候返回 s2 的地址  如AT^IPOPEN=1,1200,"113.89.45.231",8811,0
//my_strstr(s1,",",3) 找三个“，”后跳出

char *my_strstr(const char *sin,const char *s2, int num)
{
	const char *s1;
    int n;
	int i=0;

	s1 = sin;
    if(*s2)
    {
        while(*s1)
        {
			
			for(n=0;*(s1+n)==*(s2+n);n++)
			{
				if(!*(s2+n+1)){
					i++;
					if (i==num)
						return(char*)s1;
				}
			}
			s1++;
			
        }
        return NULL;
    }
    else
        return (char*)s1;
}


void gprsCmdDeal_CDMA_3G(void)
{
//	unsigned char t;
//	unsigned int count=0;
//	unsigned short len=0;
//	char code[5];
	char *ret=NULL;
//	char *start_p=NULL;
//	char *temp_p=NULL;
	unsigned int i;
//	unsigned char index;
	

	if(gprsIndex < 3){	//  有 “\r\n>” 过来，但是先进来了，要退出 
#ifdef _debug_gprs
	//	debugstring("low\r\n");
#endif
		return;
	}

#ifdef _debug_gprs
// 	debugstring("deal3G:");
// 	debugstring((char *)gprsRevBuf);
#endif



/*
	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I关闭中
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//关闭成功 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto CDMA_3Gdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//关闭不成功 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto CDMA_3Gdealcmdend;
		}
	}
*/

	

	ret = strstr((char*)gprsRevBuf, "^MODE:");
	if(ret != 0){
#ifdef _debug_gprs
	//	debugstring("^MODE,清\r\n");
#endif

		gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
		goto CDMA_3GdealEnd;
		
	}


	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 4:
	case 6:
	case 7:
	
//	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 17:
	case 19:
	case 39:
	case 40:


		ret = strstr((char*)gprsRevBuf, "OK");	//比较有没有OK
		if(ret != 0){

			if(GPRSLinkProcess == 19){
				GPRSLinkProcess = 39;
			}
			else if(GPRSLinkProcess == 39)
				GPRSLinkProcess = 40;
			else if(GPRSLinkProcess == 40)
				GPRSLinkProcess = 20;
// 			else if ((GPRSLinkProcess == 9)&&(MG_CEM_FLAG == Gmode_CEM630))
// 			{
// 				GPRSLinkProcess = 
// 			}
			else
				GPRSLinkProcess++;
			set_timer0(0, 1);		//时间不再等待，直接进入下一个链接处理
			gprsErrorTimes = 0;		//清零错误标志
			gprsAnswer = 'O';

			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			if(GPRSLinkProcess == 17)
				set_timer0(500, 1);
			gprsErrorTimes++;
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

		
		}

		goto CDMA_3Gdealcmdend;
		//set_timer0(100,1);
		//break;


	case 2:

		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			//
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			//goto CDMA_3Gdealcmdend;
		}
		else{
			gprsReady = GPRS_GOON;
		
		}
		goto CDMA_3Gdealcmdend;
		//break;

	case 3:

		if(strstr((char*)gprsRevBuf, "OK") != 0){
		
	#ifdef _debug_gprs
	//			debugstring("is CEM630????\r\n");
	#endif
// 			if (strstr((char*)gprsRevBuf, "CEM630") != 0)
// 			{
// 			}
			set_timer0(10, 1);	
			gprsErrorTimes = 0;		//清零错误标志
			gprsAnswer = 'O';
			GPRSLinkProcess++;
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

		}
		else
			gprsReady = GPRS_GOON;
		break;

	case 8:	//^IPCALL: 0,113.113.116.56          // 0 连接,  1 断开

		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			ret = NULL;
			ret = strstr((char*)gprsRevBuf, "^IPCALL: ");
			if(ret != 0){
	
					ret = strstr((char*)gprsRevBuf, ",");
					if (ret !=0)
					{
#ifdef _debug_gprs
// 						debugstring("IPCALL FLAG:");
// 						debugdata(ret-1,1,1);
#endif
						if(*(ret-1) == '0')		//CEM631
						{
							gprsErrorTimes = 0;
							GPRSLinkProcess++;
							set_timer0(0, 1);
						}
						gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
						goto CDMA_3GdealEnd;
					}
			

			}
		
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			gprsErrorTimes++;
		}
// 		else
// 		{
// 			gprsReady = GPRS_GOON;
// 		}


		break;


	case 5:

		if(strstr((char*)gprsRevBuf, "OK") != 0){


			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			gprsAnswer = 'O';


			ret = NULL;
			ret = strrchr((char*)gprsRevBuf, ',');		//找最后一个，

			if(ret != 0){
				if((*(ret+1) == '1')||(*(ret+1) == '4'))
				{
					gprsErrorTimes = 0;
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			
			goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			gprsErrorTimes++;
			goto CDMA_3Gdealcmdend;
		}
		else {
			gprsReady = GPRS_GOON;
			
		}
		break;
	


	case GPRS_Out_CMD:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

			GPRSLinkProcess = 5;// 重新连接
			set_timer0(1000, 1);
			gprsErrorTimes = 0;
			
			gprsAnswer = 'O';
			//goto CDMA_3GdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes+=5;
//			debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

			//goto CDMA_3GdealEnd;
		}
		goto CDMA_3GdealEnd;
	case GPRS_TCPCLOSE_CMD://TCPIP关闭中，等待OK返回
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess > 20)
				GPRSLinkProcess = 9;// 重新连接，重新连接时需要重新设置模式,不能=20直接在进行连接，不然会使用0模式。
			set_timer0(200, 1);		//错误修改，后面还有IPSTAT，1，1。还得等会。
//			gprsErrorTimes = 0;
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
#ifdef _debug_gprs
			debugstring(" GPRS_TCPCLOSE_CMD(0xF1) receive\r\n");
#endif		
			
			gprsAnswer = 'O';
			goto CDMA_3GdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes+=5;
//			debugdata((unsigned char*)&gprsErrorTimes, 1, 1);

			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			goto CDMA_3GdealEnd;
		}
		
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

			for(i=0; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

			goto CDMA_3Gdealcmdend;
		}
		else{
			gprsReady = GPRS_GOON;
			return;
		}
//		break;
	case 18:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			//goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引

			gprsErrorTimes++;
			//goto CDMA_3Gdealcmdend;
		}
		else{
			gprsReady = GPRS_GOON;
			
		}
		goto CDMA_3Gdealcmdend;

//		break;
	case 20:
		
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			
			gprsAnswer = 'O';
#ifdef _debug_gprs
			debugstring("CEM630_link OK\r\n");
// 					debugstring("34343\r\n");
// 					debugstring(gprsRevBuf);
#endif
// 					ret = strstr((char*)gprsRevBuf, "^IPOPEN");
// 					if(ret != 0){
			isNetOK[0]=3;		//socket链接成功
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			//}
			goto CDMA_3Gdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){	//错误修改
#ifdef _debug_gprs
			
			debugstring("QUANERRORERROEERROE!!!!\r\n");
#endif
			gprsErrorTimes++;			//???????????????
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);//关闭，需要延时长点再连接
			gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
			
			goto CDMA_3Gdealcmdend;
		}
		else{
			return; //还没收完
		}
//		break;
	case 21:
		GPRSLinkProcess++;
		break;

	default:


	/**********************一下是GM631******************************************/

		

/************************************************************************

		//数据接收		//^IPRECV:1,113.89.126.181,8082,9,564879!@#
		//				  ^IPRECV:1,113.118.100.92,8811,27,
		ret = strstr((char*)gprsRevBuf, "RECV");
		if(ret != 0){

			//有时候断了都2F 了，还是能收到数据，不知道什么原因，这个还是认为网络OK
			if ((GPRSLinkProcess != 0x16)&&(GPRSLinkProcess != GPRS_SENDING_CMD))
			{
				GPRSLinkProcess = 0x16;
				isNetOK[0] = 3;		
			}

#ifdef _debug_gprs
// 		debugstring("dat:");
// 		debugdata(gprsRevBuf,gprsIndex,1);
#endif

			ret = my_strstr((char*)gprsRevBuf, ",",3);
			if(ret != 0){	
				i=0;
				ret++;
				while((*ret >= '0')&&(*ret <= '9'))
				{
					if (i>1024){		//异常？
						i=0;
						break;		
					}
					
					i = i*10 + (*ret-'0');
					ret++;
				}



				len = i;



				if (*ret==',')		//长度后面的,
				{

					ret ++;			//^IPRECV:1,119.136.47.178,8811,27,BDBDBDD0D0AA83151001014255010100092016013016201490001B6C

					start_p = ret;
					//数据域里面有可能有 0x00，所以可能找不到这个结尾的\r\n, 所以要改用索引找，但是一定要保障索引用完后清空
					//否则错位了可能会连上网络后，异常后一直再上不去了，因为错位了
// 					ret = strstr((char*)start_p, "\x0D\x0A",1);		
// 					if (ret !=0)
// 					{
// 						count = ret - start_p ;		//按照指针地址找头到数据域有多少字节，来判断数据收完没有
// 					}else{
// 						debugstring("eeeee\r\n");
// 					}


					count = (unsigned char *)start_p- gprsRevBuf;
					
#ifdef _debug_gprs
// 					debugstring("end:");
// 					debugdata(ret,3,1);
// 					debugstring("end:");
// 					debugdata((unsigned char *)gprsRevBuf,2,1);
// 					debugstring("\r\ngprsIndex:");
// 					debugdata((unsigned char *)&gprsIndex,2,1);
// 					debugstring("code len :");
// 					debugdata((unsigned char *)&len,4,1);
// 					debugstring("count len:");
// 					debugdata((unsigned char *)&count,4,1);
#endif



					if (gprsIndex < (len + count + 2)  )		//没收完 ,因为数据域里面有可能有0x0D,0x0a, 
					{
#ifdef _debug_
						debugstring("BG\r\n");
#endif
						return ;
					}


#ifdef _debug_gprs
					//debugstring("\r\n--over---\r\n");
#endif		
#ifdef _debug_gprs
				
					MSG_LOG("gprsIndex:%d\r\n",gprsIndex);
 					debugstring("gprsRevBuf:");
 					debugdata(gprsRevBuf,gprsIndex,1);
#endif
					gTcpIpbusy = TcpIPFree;
					gTcpIpbusydly = 0;			//空闲
					gErrorFlag &= (~ERROR_BUS_CONNECT);// 清除错误标识 能进这里说明网络都正常
					gGPRS_data_style.ucReadStyle = 0;
					gprsAnswer = 'O';
					gprsErrorTimes = 0;
					gprsSNDerro = 0;
					for(i=0;i<MAX_RCV_PACKET_NUM;i++){
						if(gPacketRcvBuf[i].ucItemStatus != 0xea)
							break;
					}
					if(i>=MAX_RCV_PACKET_NUM){
						gprsReady = GPRS_CLEAR_INDEX;
#ifdef _debug_gprs9
						debugstring("full !!!\r\n");
#endif
						goto CDMA_3Gdealcmdend;
					}
					gPacketRcvBuf[i].LinkFrame.ucFrameType = 1;
					memcpy(gPacketRcvBuf[i].LinkFrame.uiFrameLen, (unsigned char*)&len, 2);
					memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf, start_p, len);
					//Ascii2BCD((unsigned char *)ret,gPacketRcvBuf[i].LinkFrame.FrameBuf,guiMC323_REV_LEN*2);
					memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf,start_p,len);
					gPacketRcvBuf[i].ucItemStatus = 0xea;
#ifdef _debug_gprs
//  					debugstring("server dat:");
//  					debugdata((unsigned char *)start_p,len,1);
// 					debugstring("\r\n接收2G:");
// 					debugdata((unsigned char*)&gPacketRcvBuf[i].LinkFrame, guiMC323_REV_LEN+3, 1);
// 					debugstring("多打印30Byte~~~后面还有吗??-------\r\n");
// 					debugdata((unsigned char*)&gPacketRcvBuf[i].LinkFrame.FrameBuf+guiMC323_REV_LEN, 30, 1);
#endif
					// 				debugstring("多打印30Byte~~~后面还有吗??-------\r\n");
					// 				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame.FrameBuf+guiMC323_REV_LEN, 30, 1);
					

					guiMC323_REV_LEN = 0;
					
					gprsReady = GPRS_CLEAR_INDEX;


					goto CDMA_3GdealEnd;
				}	
				
			}
			
			//set_timer0(0, 1);
			goto CDMA_3Gdealcmdend;
		}

		************************************************************************/


		//^IPSEND:1, 78
		//^IPSTAT:1,1   socket 失联  GM631
		ret = strstr((char*)gprsRevBuf, "IPSTAT");
		if(ret != 0){//链接断开
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				if(*(ret+1) == '1')		//CEM631  
				{
					
#ifdef _debug_gprs
					debugstring("--IPCLOSE-***CEM631-");
#endif					
					gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
					
					
					// 					if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					// 						GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					memset(gBusVerInfo.newbusBLKVer, 0, 2);
					
					isNetOK[0]=0;		//socket链接断开
					//gprsErrorTimes ++;
					//stopCCgprs(ERROR_BUS_CONNECT);
// 					if(GPRSLinkProcess > 20)//错误修改，AT^IPCLOSE=1后面还有IPSTAT，1，1输入，这里会引起不停的断开，连接，断开 。l加入进程判断
// 						GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;  
					if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
 						GPRSLinkProcess = 9;//GPRSLinkProcess = GPRS_NEED_CLOSEIP;		//修改，已经断开了，不需要再发CLOSE指令		
					//GPRSLinkProcess= 6;	// 先设置十六进制格式 否则后面的指令模块不认 再 重新链接后台
					set_timer0(3000, 1);
				}
			}
			goto CDMA_3Gdealcmdend;
		}
		


	/**********************************以上是GM631**************************************/


		break;
	}

// end switch();
	



	ret = strstr((char*)gprsRevBuf, "OK");
	if(ret != 0){
#ifdef _debug_gprs
		debugstring("WW\r\n");
#endif
		//if((gTcpIpbusy == TcpWaitOk)){
			gTcpIpbusy = TcpIPFree;
			gGPRS_data_style.ucSendstyle = 0;
			gGPRS_data_style.ucReadStyle = 0;
		//}

		gprsAnswer = 'O';
		guiMC323_REV_LEN = 0;
		gprsReady = GPRS_CLEAR_INDEX;	//清理 gprsbuff 和索引
	}

//CDMA_3Gdealcmderror:
	
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
#ifdef _debug_gprs
	//	debugstring("tt=\r\n");
#endif
		gprsErrorTimes++;
		gprsReady = GPRS_CLEAR_INDEX;

	}
CDMA_3Gdealcmdend:
	
	if(gprsErrorTimes > 20){//错误次数太多，重启模块
#ifdef _debug_gprs
	//	debugstring("reset module!!!!!!!!!!\r\n");
#endif
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//关闭1S
		//debugstring("--1 close\r\n");
		if((GPRSLinkProcess > 16)&&(GPRSLinkProcess < 0x90)){

			if(gmissflag & MISS_GJ){//如果正在登陆公交,登不上去,先暂停,做个标志
#ifdef _debug_gprs
				debugstring("reset stop CCgprs:ERROR_BUS_CONNECT!\r\n");
#endif
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
//				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //如果不是同时有问题,则退出重新找任务.
			}
		}
		if(GPRSLinkProcess == 20){//登陆不了后台
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		}
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
CDMA_3GdealEnd:
	
	if(gprsReady == GPRS_CLEAR_INDEX){//还在收新的数据
#ifdef _debug_gprs
//		debugstring("XX\r\n");
// 		MSG_LOG("%d\r\n",gprsIndex);
// 		BCD_LOG(gprsRevBuf,gprsIndex,1);
#endif
		memset(gprsRevBuf, 0, GPRSCOMMAX);
		gprsIndex = 0;
		gprsReady = GPRS_GOON;
	}
	return;
}

//MC323模块
void MC55i_SISREAD(void)
{
	char buff[50];
	strcpy(buff, "at^sisr=0,1500\r\n");
	gprs_snd(strlen(buff), buff);
}

void gprs_DATA_snd(unsigned int len, unsigned char *dat)
{
	gprsAnswer = 0;
	memcpy(dat+len, "\x0d\x0a", 2);
	gprs_snd(len+2, (char*)dat);
}

void gprsCmdDeal_MG323(void)
{
	unsigned char t;
	unsigned int count;
	char code[5];
	char *ret;
	unsigned int i;
//	unsigned char index;
	
	if(gprsReady != 0xaa)
		return;

	if(gprsIndex < 3)
		goto GPRSdealEnd;

#ifdef _debug_gprs
	//debugstring("R:");

#endif

	if(strstr((char*)gprsRevBuf, "^SYSSTART") != 0)
	{
		gprsErrorTimes = 0;

		GPRSLinkProcess = GPRS_LINK_UP;//需要把POWN脚拉高，GPRS_AT_WAIT;	//GPRS模块重启了
		set_timer0(400, 1);//链接关闭,延时后重新连接

		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--MG323 ready\r\n");
#endif
		goto Gprsdealcmdend;
	}
	if(strstr((char*)gprsRevBuf, "^SHUTDOWN") != 0)//模块关机
	{
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		set_timer0(2000, 1);//收到OK， 才关闭成功
#ifdef _debug_gprs
		debugstring("MG323 SHUTDOWN\r\n");
#endif
		goto Gprsdealcmdend;
	}

	if(in_dealSoundIn() == 1)
		goto Gprsdealcmdend;

// 	if(strstr((char*)gprsRevBuf, "+CGREG") != 0){
// 		ret = strstr((char*)&gprsRevBuf, ",");
// 		if(ret != 0){
// 			ret++;
// 			if(*ret == '0'){
// //				debugstring("--0 close\r\n");
// 				gprsSingle = 0;
// 				//set_timer0(1000, 1);//关闭1S
// 				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
// 				goto Gprsdealcmdend;
// 			}
// 			else if(*ret == '2'){
// 				gprsSingle = 1;
// 				gprsReady = 0;
// 				return;//正在找网络，不管.
// 			}
// 		}
// 	}

	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I关闭中
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//关闭成功 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//关闭不成功 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
	}

	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 17:
	case 19:
	case 39:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess == 19){
#ifdef _debug_gprs
				debugstring("19--39\r\n");
#endif
				GPRSLinkProcess = 39;
			}
			else if(GPRSLinkProcess == 39)
				GPRSLinkProcess = 40;
			else if(GPRSLinkProcess == 40)
				GPRSLinkProcess = 20;
			else
				GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case 8:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(strstr((char*)gprsRevBuf, "+CGREG") != 0){
				ret = strstr((char*)&gprsRevBuf, ",");
				if(ret != 0){
					ret++;
// 					if(*ret == '0'){
// 						debugstring("  1111111    ");
// 						//				debugstring("--0 close\r\n");
// 						gprsSingle = 0;
// 						//set_timer0(1000, 1);//关闭1S
// 						GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
// 						//goto Gprsdealcmdend;
// 					}
// 					else 
					if((*ret == '1')||(*ret == '5')){//1：注册本地网络   5：注册，漫游
						GPRSLinkProcess++;
						set_timer0(0, 1);
						gprsErrorTimes = 0;
						gprsAnswer = 'O';
						//goto Gprsdealcmdend;
					}
					else {//if(*ret == '2'){//0：没有注册，ME 也没有尝试去注册 2：没有注册，ME 正在试图去注册  3：注册被拒绝  4：不知道网络注册状态  8: 紧急呼叫状态
						gprsSingle = 1;
						gprsReady = 0;
						if(GPRSLinkProcess < 20)
							GPRSLinkProcess = 8;
						
						//goto Gprsdealcmdend;//正在找网络，不管.
					}
				}
			}
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		else{		//没收到OK，也没收到ERROR，等。。。
			return;
		}
		goto Gprsdealcmdend;//正在找网络，不管.
		
	case 40:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess = 20;
			ret = strstr((char*)gprsRevBuf, "^SICI:");
			if(ret != 0){
				count = 0;
				t = 0;
				for(i=0; i<35; i++){
					if(ret[i]=='\"'){
						if(t == 0)
							t = i;
						else if(count == 0){
							count = i+1;
							break;
						}
					}
				}
#ifdef _debug_gprs
				debugstring("什么鬼:");
				debugstring(ret);
				debugdata((unsigned char*)&t, 4, 1);
				debugdata((unsigned char*)&count, 4, 1);
#endif

				memset(irda_rxBuf, ' ', 20);
				irda_rxBuf[20] = 0;
				memcpy(irda_rxBuf, ret+t, count-t);
			}
			goto Gprsdealcmdend;
		}
		gprsReady = 0;
		return;
	case GPRS_TCPCLOSE_CMD://TCPIP关闭中，等待OK返回
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess > 20)
				GPRSLinkProcess = 20;// 重新连接
			set_timer0(0, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes+=5;
//			debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
			goto Gprsdealcmdend;
		}
		goto Gprsdealcmderror;
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			for(i=0; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 18:
#ifdef _debug_gprs
		debugstring((char*)gprsRevBuf);
		debugstring("*******");
#endif
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 20:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			guiMC323_REV_LEN = 0;
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;//(9);
			set_timer0(50, 1);//20121227
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case 21:
		ret = strstr((char*)gprsRevBuf, "^SISW:");
		if(ret != 0){
			if((gmissflag & 0xf0)==0x10){//登陆移动IF1
				isNetOK[0] = 1;
			}
			else if((gmissflag & 0xf0)==0x20){//登陆移动IF2
				isNetOK[0] = 2;
			}
			else{//登陆公交数据中心
				isNetOK[0] = 3;
			}
			gTcpIpbusy = TcpIPFree;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			guiMC323_REV_LEN = 0;
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "^SIS:");
		if(ret != 0){
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			if(i<20){
				i = 0xffffffff;
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					else if((*ret >= '0')&&(*ret <= '9')){
						if(i == 0xffffffff)
							i = 0;
						i = i*10 + *ret-'0';
					}
					else
						break;
					ret++;
				}
			}
#ifdef _debug_gprs
			debugstring("SIS 21:");
			debugdata((unsigned char*)&i, 4, 1);
#endif
			if(i == 48){//^SIS: 0, 0, 48, Remote Peer has closed the connection
				set_timer0(2000, 1);//链接关闭,延时后重新连接
				if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				isNetOK[0] = 0;
				
				memset(gBusVerInfo.newbusBLKVer, 0, 2);
				gmissflag = 0;
				//#ifdef _debug_
				// 				debugstring("--IPCLOSE--");
				// 				debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
				//#endif
			}
			else if(i == 21){//^SIS: 0, 0, 21, "Remote host has rejected the connection" 拒绝连接
				set_timer0(2000, 1);//链接关闭,延时后重新连接
				if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				isNetOK[0] = 0;
				memset(gBusVerInfo.newbusBLKVer, 0, 2);
				gmissflag = 0;
			}
			else
				IPERROR_DEAL(i);
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
//			gprsErrorTimes++;
			set_timer0(20, 1);
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);//关闭，需要延时长点再连接
			goto Gprsdealcmdend;
		}
		break;
	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 5:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				if((*(ret+1) == '1')||(*(ret+1) == '5'))
				{
					gprsErrorTimes = 0;
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else {
			gprsReady = 0;
			return;
		}
//		break;
	default:
		ret = strstr((char*)gprsRevBuf, "^SISR:");
		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				ret++;
				i = 0;
				memset(code, 0, 5);
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					if((*ret == 0x0d)||(*ret <= 0x0a)||(i>=5)){
						break;
					}
					else
						code[i++] = *ret;
					ret++;
				}
				// #ifdef _debug_
				// 				debugstring("code:");
				// 				debugstring(code);
				// #endif
				if(i<5){
					if(strlen(code) == 1){//有数据
						if(code[0] == '1'){
							gGPRS_data_style.ucReadStyle = GPRS_SISR;
//							gGPRS_data_style.uiNum = 1500;
//							MC55i_SISREAD();//串口中断中不能发送数据
							goto Gprsdealcmdend;
						}
						else if(code[0] == '2'){//End of data.
							goto Gprsdealcmdend;
						}
					}
					else if(strlen(code) == 2){
						if(code[0] == '-'){
							GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
							goto Gprsdealcmdend;
						}
					}
				}
				i = str2int((unsigned char*)code, strlen(code));
// #ifdef _debug_
// 				debugstring("code i:");
// 				debugdata((unsigned char*)&i, 4, 1);
// #endif
				guiMC323_REV_LEN = i;
				goto Gprsdealcmdend;
				/*
				ret = strstr((char*)(gprsRevBuf+i+9+strlen(code)), "OK");
				
				if(ret != 0){
#ifdef _debug_
					debugstring("code i:");
					debugdata((unsigned char*)&i, 4, 1);
#endif
#ifdef _debug_
					debugstring("数据收完成:");
					debugdata((unsigned char*)ret, 20, 1);
					debugdata(gprsRevBuf, gprsIndex, 1);
#endif
					gprsAnswer = 'O';
					gprsErrorTimes = 0;
					for(index=0;index<MAX_RCV_PACKET_NUM;index++){
						if(gPacketRcvBuf[index].ucItemStatus != 0xea)
							break;
					}
					if(index>=MAX_RCV_PACKET_NUM){
						memset(gprsRevBuf, 0, gprsIndex+1);	//缓冲区满，此包丢弃
						gprsIndex = 0;
						return;
					}
					
					for(t=0; t<(i+9+strlen(code)); t++){
						if((gprsRevBuf[t]==0x0d)||(gprsRevBuf[t]==0x0a))//找到0D 0A
							break;
					}
					t++;
					for(; t<(i+9+strlen(code)); t++){
						if((gprsRevBuf[t]!=0x0d)&&(gprsRevBuf[t]!=0x0a))//找到不是0D  0A的
							break;
					}
					gPacketRcvBuf[index].LinkFrame.ucFrameType = 0;
					memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&i, 2);
					memcpy(gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf+t, i);
					gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_
					debugstring("接收:");
					debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, i+3, 1);
#endif
					goto Gprsdealcmdend;
				}*/
			}
			return;
		}
		ret = strstr((char*)gprsRevBuf, "^SISW:");
		if(ret != 0){//需要发送数据
			t = 0;
			count = 0;
			while(1){
				ret++;
				if(*ret == ','){
					if(t != 0){
						t++;
						break;
					}
					t++;
					count = 0;//取最后逗号后面的值,这里清零
				}
				else if((*ret == 0x0d)||(*ret == 0x0a))
					break;
				else{
					if(t>0){
						count*=10;
						count+=(*ret-'0');
					}
				}
			}
#ifdef _debug_
			debugstring(",t=");
			debugdata((unsigned char*)&t, 1, 1);
			debugstring(",count=");
			debugdata((unsigned char*)&count, 1, 1);
#endif
			if(t == 0)
				goto Gprsdealcmdend;
			else if(t == 1){//^SISW: 0, 1  发送数据功能准备好
				if(count == 2){//后台关闭
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
					goto Gprsdealcmdend;
				}
				else if(count == 1){//模块准备好接收数据
					gTcpIpbusy = TcpIPFree;
					goto Gprsdealcmdend;
				}
			}
			//  t == 2  ^SISW: 0,78,78 发送数据
#ifdef _debug_
			debugstring("发送数据:");
			debugdata(gMC55iSendbuff, 14, 1);
#endif
			gprsAnswer = 'O';
			gGPRS_data_style.ucSendstyle = GPRS_SISW;
			gGPRS_data_style.uiNum = count;
// 			if(memcmp(gMC55iSendbuff, "数据有效", 8)==0){
// 				memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
// 				gprs_DATA_snd(i, gMC55iSendbuff+12);//串口中断中不能发送数据
// 				memset(gMC55iSendbuff, 0, 50);
// 			}
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "^SIS:");
		if(ret != 0){
//			ret += 8;
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			for(i=0;i<20;i++){
				if(*ret == ',')
					break;
				ret++;
			}
			ret++;
			if(i<20){
				i = 0xffffffff;
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					else if((*ret >= '0')&&(*ret <= '9')){
						if(i == 0xffffffff)
							i = 0;
						i = i*10 + *ret-'0';
					}
					else
						break;
					ret++;
				}
			}
#ifdef _debug_gprs
			debugstring("SIS:");
			debugdata((unsigned char*)&i, 4, 1);
#endif
			if(i == 48){//^SIS: 0, 0, 48, Remote Peer has closed the connection
				set_timer0(500, 1);//链接关闭,延时后重新连接
				if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				isNetOK[0] = 0;
				
				memset(gBusVerInfo.newbusBLKVer, 0, 2);
				gmissflag = 0;
#ifdef _debug_gprs
				debugstring("--IPCLOSE--");
				debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
				debugstring("gmissflag:");
				debugdata((unsigned char*)&gmissflag, 1, 1);
				debugstring("gBusVerInfo.newbusBLKVer:");
				debugdata(gBusVerInfo.newbusBLKVer, 2, 1);
#endif
			}
			else if(i == 21){//^SIS: 0, 0, 21, "Remote host has rejected the connection" 拒绝连接
				;
			}
			else
				IPERROR_DEAL(i);
			goto Gprsdealcmdend;
		}
		break;
	}
	ret = strstr((char*)gprsRevBuf, "+CSQ");
	if(ret != 0){
		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				gprsSingle = i;
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmderror:
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
		gprsErrorTimes++;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				IPERROR_DEAL(i);
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmdend:
	if(gprsErrorTimes > 20){//错误次数太多，重启模块
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//关闭1S
		//debugstring("--1 close\r\n");
		if((GPRSLinkProcess > 16)&&(GPRSLinkProcess < 0x90)){
			if(gmissflag & MISS_GJ){//如果正在登陆公交,登不上去,先暂停,做个标志
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //如果不是同时有问题,则退出重新找任务.
			}
		}
		if(GPRSLinkProcess == 20){//登陆不了后台
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		}
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
GPRSdealEnd:
	if(gprsReady != 0x55){//还在收新的数据
		gprsIndex = 0;
		memset(gprsRevBuf, 0, GPRSCOMMAX);
		gprsReady = 0;
	}
	return;
}

/*
	//PDP 场景异常反激活通知
	//PDP 场景受网络影响会被自动关闭，此时模块会上报URC 通知host。Host 执行AT+QIDEACT 会关闭指定场景及该场景下的所有连接。
	//+QIURC: “pdpdeact”,<contextID>		//<contextID> 整型，场景ID，范围：1-16
	//+QIURC: "closed",<connectID>   // <connectID>连接被断开
*/
void EC20_pdp_down(void)
{
	char *ret1;
	
	if ((GPRSLinkProcess>9)&&(GPRSLinkProcess<0xF0))		////激活PDP 场景后需要判断反激活 
	{
		ret1 = strstr((char*)gprsRevBuf, "+QIURC");
		if(ret1 != 0)
		{
			if (strstr((char*)gprsRevBuf, "pdpdeact")!=0)
			{
				isNetOK[0]=0;		//socket链接断开
				gprsErrorTimes = 0;
				GPRSLinkProcess= 2;	//查询SIM卡状态
				set_timer0(100, 1);
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				
				
			}
		}
	}

}

void gprsCmdDeal_ALL4g(void)
{
	char *ret,*rp;
	unsigned int i;
// 
// 	debugstring("4GReV:");
// 	debugstring((char*)gprsRevBuf);
// 	debugstring("\r\n");
// 	debugdata(gprsRevBuf, gprsIndex, 1);

	if(gprsIndex < 4)
		goto GPRSdealEnd;
 
//	debugstring((char*)gprsRevBuf);

	if ((GPRSLinkProcess<=11)||((GPRSLinkProcess>=0xF0)))		//模块关机成功会自报"POWERED DOWN"
	{
		if(strstr((char*)gprsRevBuf, "POWERED DOWN") != 0)//模块关机,使用PWRKEY管脚关机也会出现
		{
			GPRSLinkProcess = GPRS_LINK_CMD;
			gprsErrorTimes = 0;
			isNetOK[0] = 0;
			set_timer0(10000, 1);//延时10s
 			debugstring("EC20 SHUTDOWN\r\n");

			gprsErrorTimes = 0;
			goto GPRSdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "RDY") != 0){
			isNetOK[0] = 0;
#ifdef _debug_gprs
			debugstring("EC20 ready\r\n");
#endif
			gprsErrorTimes = 0;
			GPRS_MC55i_open();      //拉高POWN
			GPRSLinkProcess = GPRS_AT_WAIT;
			set_timer0(500, 1);	//开机后会自报一些信息
			goto GPRSdealEnd;
		}
	}


	if(GPRSLinkProcess == GPRS_AT_RESET){//EC20软件关闭中
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//关闭成功 
	//		GPRSLinkProcess = GPRS_LINK_CMD;
			set_timer0(15000, 1);//延时15秒等待POWERED DOWN
			isNetOK[0] = 0;
			gprsErrorTimes = 0;
 			gprsAnswer = 'O';
			goto GPRSdealEnd;
		}

		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//关闭不成功 
			GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			set_timer0(10, 1);//电源重启，重新开始测试
			isNetOK[0] = 0;
			gprsErrorTimes = 0;
 			gprsAnswer = 'O';
			goto GPRSdealEnd;
		}

		if(strstr((char*)gprsRevBuf, "POWERED DOWN") != 0)//模块关机
		{
			GPRSLinkProcess = GPRS_LINK_CMD;		
			gprsErrorTimes = 0;
			isNetOK[0] = 0;
			set_timer0(10000, 1);//延时10s
			debugstring("EC20 SHUTDOWN\r\n");
			
			gprsErrorTimes = 0;
			goto GPRSdealEnd;
			
		}

	}


	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 4:
	case 6:		//设置PS业务的状态改变后自报
	case 7:
	case 10:		//配置PDP 场景相关参数
	case 11:		//激活PDP 场景1, 通过移动网获取IP
	case 19:	//关闭PDP 场景1,没有加进来
	case GPRS_TCPCLOSE_CMD://TCPIP关闭中，等待OK返回
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			
		   if (GPRSLinkProcess==GPRS_TCPCLOSE_CMD)
			{	
				isNetOK[0]=0;		//socket链接断开
				gprsErrorTimes = 0;
				GPRSLinkProcess = 20;
			}
		    else if (GPRSLinkProcess == 19)
		    {
				isNetOK[0]=0;		
				gprsErrorTimes = 0;
				GPRSLinkProcess = 2;	//返回查询SIM卡信息
		    }
			else
				GPRSLinkProcess++;

			set_timer0(100, 1);
			gprsErrorTimes = 0;
 			gprsAnswer = 'O';

			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
 			gprsErrorTimes++;
			if(GPRSLinkProcess == 11)
				GPRSLinkProcess = 19;	//关闭PDP
#ifdef _debug_gprs
			debugstring("$#:");
			debugstring((char*)gprsRevBuf);
#endif
			set_timer0(1000, 1);
			goto Gprsdealcmdend;//Gprsdealcmdend;
		}

		break;

	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(200, 1);				//增加延时，目的是避免第一次查询SIM卡IMSI号码 (AT+CIMI )出现ERROR
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
 			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
	//	break;

	case 3:	//查询SIM卡IMSI号码  //AT+CIMI    460036261069293   OK		204046620134413
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			
			ugISMI_type = 0;
			ret = strstr((char*)gprsRevBuf, "460");
			if (ret != 0)	////460 中国代码
			{
				ret += 3;
				i = (*ret-'0')*10 + (*(ret+1)-'0');
				//debugdata((unsigned char*)&i,4,1);
				switch (i)
				{
				case 0:		//移动卡46000/46002/46004/46007
				case 2:
				case 4:
				case 7:
					ugISMI_type = ChinaMobile;	
					break;
				case 1:		//联通卡46001/46006/46009
				case 6:
				case 9:
					ugISMI_type = ChinaUnicom;	
					break;
				case 3:		//电信卡46003/46005/46011
				case 5:
				case 11:
					ugISMI_type = ChinaTelecom;
					GPRSLinkProcess = 5;  //pfquan 2017.09.08 电信的5模要跳过4，5步骤
					break;
				default:
					ugISMI_type = 0;
					break;
				}
					
			}

					
			if(ugISMI_type != 0){
				gprsErrorTimes = 0;
				GPRSLinkProcess++;
				set_timer0(0, 1);
			}

			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;
		
	//	break;
		
	case 5:	//查询CS业务（网络注册）的状态   //+CREG: 1,1   OK
	case 8:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
 			gprsAnswer = 'O';
			//ret = strstr((char*)gprsRevBuf, ",");
			ret = strchr((char*)gprsRevBuf, ',');
	
			if(ret != 0){
				if(*(ret+1) == '1')		
				{
 					gprsErrorTimes = 0;
					GPRSLinkProcess++;
					set_timer0(10, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else
			return;	//OK会在+CREG:1,1 \x0d\x0a 后出。0D0A2B435245473A20312C310D0A0D0A4F4B0D0AOK

	
//		break;

	case 12:	//+QIACT: 1,1,1,"100.101.73.205"       OK   
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsAnswer = 'O';
			//ret = strstr((char*)gprsRevBuf, "QIACT: ");
			ret = strchr((char*)gprsRevBuf, ',');
			//debugstring(ret);
			if(ret != 0){
				if(*(ret+1) == '1')		
				{
			//		debugstring(ret+1);	//获取IP地址成功
					gprsErrorTimes = 0;
					GPRSLinkProcess=20;		//设置服务器IP地址
					set_timer0(0, 1);
				}
				else if (*(ret+1) == '0')
				{
					gprsErrorTimes = 0;
					GPRSLinkProcess=19;		//反激活指定的PDP
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
			
		}
		else{
			EC20_pdp_down();		//PDP 场景异常反激活通知
			return;
		}
		
		
	//	break;

	//+QISTATE: <connectID>,<service_type>,<IP_address>,<remote_port>,<local_port>,<socket_state>,<contextID>,<serverID>,<access_mode>,<AT_port>
	// 		<socket_state> 整型，socket 服务状态
	// 			0 “初始化” Socket 还未被创建
	// 			1 “连接中” 客户端正在连接，服务器端正在建立监听
	// 			2 “连接成功” 客户端/Incomming 连接已经建立成功
	// 			3 “监听状态” 服务器正在监听
	// 			4 “等待关闭” 远端主动关闭TCP 链接，此时模块会处于等待关闭状态，需要MCU 执行QICLOSE 来释放socket 资源

	case GPRS_SOCKET_CHECK_CMD:	//查询SOCKET1打开状态,暂时没有使用。
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			ret = my_strstr((char*)gprsRevBuf, ",", 5);
			if(ret != 0){
				switch (*(ret+1))
				{
				case '0':			    //0 “初始化” Socket 还未被创建
					isNetOK[0]=0;		//socket链接断开
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					break;
				case '1':				//1 “连接中” 客户端正在连接
					isNetOK[0]=0;		
					gprsErrorTimes = 0;
					goto Gprsdealcmdend;
//					break;
				case '2':
					isNetOK[0]=3;		//socket链接存在
					GPRSLinkProcess=22;	//返回通信测试
					break;
				case '4':
					isNetOK[0]=0;		//socket链接断开
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					break;
				default:
					isNetOK[0]=0;		//socket链接断开
					gprsErrorTimes = 0;
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;
					goto Gprsdealcmdend;
//					break;
				}
				
				gprsErrorTimes = 0;
				set_timer0(100, 1);
			}
		
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			EC20_pdp_down();		//PDP 场景异常反激活通知
			return;
		}

	//	break;

	case 20:	//+QIOPEN: <connectID>,<err>	(接建立成功时，<err> 为0)		//+QIOPEN: 0,0		//+QIOPEN: 0,566
				/*
				  AT+QIOPEN=1,0,"TCP","113.88.229.174",8082,0,0
				  OK
				  +QIOPEN: 0,0
		        */

		if (strstr((char*)gprsRevBuf, "+QIOPEN")!=0)		////+QIOPEN: 0,0  TCP socket0链接成功
		{
			gprsAnswer = 'O';

			ret = strchr((char*)gprsRevBuf, ',');
			if(ret != 0){
				if(*(ret+1) == '0')		
				{
					isNetOK[0]=3;			//socket链接成功
					gprsErrorTimes = 0;
					GPRSLinkProcess++;		//进入传输状态
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			isNetOK[0]=0;		//socket链接失败
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			EC20_pdp_down();		//PDP 场景异常反激活通知
			return;
		}
	//	break;

	//当发送的Buffer 已满但host仍尝试发送数据，则返回“SEND FAIL”，建议host 延迟一段时间再发送数据, 如果发送数据时出错则返回“ERROR”，此时建议关闭socket。
// 	case 22:	//数据发送确认返回，数据发送成功返回"SEND OK”，否则返回“SEND FAIL” 或"ERROR"
// 	case 23:	//发送数据
// 	case 42:	//判断‘>’	
// 	case 24:    //接收数据
// 		
		
	default:
		//数据接收	如果指定连接有缓存数据，返回：+QIRD: <read_actual_length><CR><LF><data>	 OK
// 		if((strstr((char*)gprsRevBuf, "+QIRD")!=0)&&(strstr((char*)gprsRevBuf, "OK")==0))
// 			return;

 		ret = strstr((char*)gprsRevBuf, "OK");
 		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, "+QIRD");
			if(ret != 0)
			{
				ret += 7;
				if (*ret != '0')
				{
					i=0;
					
					while((*ret >= '0')&&(*ret <= '9'))
					{
						if (i>1500){		//异常？
							i=0;
							break;		
						}
						
						i = i*10 + (*ret-'0');
						ret++;
					}

					gGPRS_data_style.ucReadStyle = GPRS_SISR;		//这个时候别发数据了
					guiMC323_REV_LEN = i;		//接收到的数据长度，跟数据格式有关？需测试
					
					goto Gprsdealcmdend;
					
				}
				gprsErrorTimes = 0;
				goto GPRSdealEnd;
			}
			else if(strstr((char*)gprsRevBuf, "SEND") != 0){		//表明数据被模块正常处理，但不表明发送成功
				set_timer0(20000, 1);	//20秒再次发生,如心跳包
				gprsErrorTimes = 0;
				gprsAnswer = 'O';
				goto GPRSdealEnd;
			}
		}
		else if (strstr((char*)gprsRevBuf, "+QIURC")!=0)
		{
			ret = strstr((char*)gprsRevBuf, "recv");
			if (ret!=0)//+QIURC: "recv",0,20
			{
//				ret = strstr((char*)gprsRevBuf, "recv");
				i = 0;
				rp = (char *)gprsRevBuf;
				while((ret-rp) < gprsIndex){
					if(*ret == ','){
						i++;
						ret++;
						if(i >= 2) break;//找到第二个,号
					}
					else
						ret++;
				}
				if(i < 2){
					goto Gprsdealcmdend;	//没有第二个,号，退出.
				}

				i=0;
				
				while((*ret >= '0')&&(*ret <= '9'))
				{
					if (i>1500){		//异常？
						i=0;
						break;		
					}
					
					i = i*10 + (*ret-'0');
					ret++;
				}
				
				gGPRS_data_style.ucReadStyle = GPRS_SISR;		//这个时候别发数据了
				guiMC323_REV_LEN = i;							//接收到的数据长度，跟数据格式有关？需测试
//				printf("RL:%d\r\n", i);
				goto Gprsdealcmdend;
					
			}
			else if (strstr((char*)gprsRevBuf, "closed")!=0)
			{
				isNetOK[0]=0;		//socket链接断开
				GPRSLinkProcess= GPRS_NEED_CLOSEIP;	//关闭SOCKET后再重新链接
				set_timer0(100, 1);
			}
			else if (strstr((char*)gprsRevBuf, "pdpdeact")!=0)
			{
				isNetOK[0]=0;		//socket链接断开
				GPRSLinkProcess= 2;	//查询SIM卡状态
				set_timer0(100, 1);			
			}
			
			gprsErrorTimes = 0;
			goto GPRSdealEnd;
			
		}
// 		else if(strstr((char*)gprsRevBuf, "SEND OK") != 0){		//表明数据被模块正常处理，但不表明发送成功
// 				set_timer0(20000, 1);	//20秒再次发生,如心跳包
// 				gprsErrorTimes = 0;
// 				gprsAnswer = 'O';
// 				goto GPRSdealEnd;
// 			
// 		}
		else if(strstr((char*)gprsRevBuf, "SEND FAIL") != 0){		//模块发送BUFF满，建议延时一段时间后再发
			gprsErrorTimes = 0;
			set_timer0(1000, 1);	//延时1000ms，等待发送完成 或 采用AT+QISEND=<connectID>,0查询发送情况
			goto GPRSdealEnd;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes = 0;
			GPRSLinkProcess=GPRS_NEED_CLOSEIP;	//关闭SOCKET0
			set_timer0(100, 1);
			goto GPRSdealEnd;
		}
		else{
			//EC20_pdp_down();		//PDP 场景异常反激活通知 ,这里已有判断
			gprsReady = 0;
//			return;
		}

// 		break;

		break;
	}  //switch结束


	ret = strstr((char*)gprsRevBuf, "+CSQ");
	if(ret != 0){
 		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				gprsSingle = i;
			}
		}
		goto Gprsdealcmdend;
	}
	
//Gprsdealcmderror:
//	ret = strstr((char*)gprsRevBuf, "ERROR");
//	if(ret != 0){
//		gprsErrorTimes++;
//		ret = strstr((char*)gprsRevBuf, ":");
//		if(ret != 0){
//			ret++;
//			i = 0xff;
//			while(1){
//				if(*ret == ' '){
//					ret++;
//					continue;
//				}
//				if((*ret >= '0')&&(*ret <= '9')){
//					if(i == 0xff)
//						i = 0;
//					i = i*10 + *ret-'0';
//				}
//				else
//					break;
//				ret++;
//			}
//			if(i != 0xff){
//				IPERROR_DEAL(i);
//			}
//		}
//		goto Gprsdealcmdend;
//	}
Gprsdealcmdend:
	if(gprsErrorTimes > 20){//错误次数太多，重启模块
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//关闭1S
		//debugstring("--1 close\r\n");
		if((GPRSLinkProcess > 16)&&(GPRSLinkProcess < 0x90)){
#ifdef _debug_gprs
			debugstring("错误过多>16----\r\n");
#endif
			if(gmissflag & MISS_GJ){//如果正在登陆公交,登不上去,先暂停,做个标志
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //如果不是同时有问题,则退出重新找任务.
			}
		}
		if(GPRSLinkProcess >= 20)
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//
		else{
			GPRSLinkProcess = 0;
		}
	}
GPRSdealEnd:
	if(gprsReady != 0x55){//还在收新的数据

// #ifdef _debug_gprs
// 		debugstring("WW:\r\n");
// #endif
		gprsIndex = 0;
		memset(gprsRevBuf, 0, GPRSCOMMAX);
		gprsReady = 0;
	}
	return;
}



//GTM900C
void gprsCmdDeal(void)
{
	char *ret;
	unsigned int i;
	unsigned char index;
	
	if(gprsReady != 0xaa)
		return;

	if(gprsIndex < 4)
		goto GPRSdealEnd;

	if(strstr((char*)gprsRevBuf, "AT-Command Interpreter") != 0)
	{
		gprsErrorTimes = 0;
		GPRSLinkProcess = GPRS_AT_WAIT;	//GPRS模块重启了
		set_timer0(40000, 1);//链接关闭,延时后重新连接
		isNetOK[0] = 0;
#ifdef _debug_gprs
		debugstring("retart--AT-Command Interpreter ready\r\n");
#endif
		goto Gprsdealcmdend;
	}

	if(GPRSLinkProcess == GPRS_AT_WAIT)
		goto Gprsdealcmdend;

	if(strstr((char*)gprsRevBuf, "+CGREG") != 0){
		ret = strstr((char*)&gprsRevBuf, ",");
		if(ret != 0){
			ret++;
			if(*ret == '0'){
//				debugstring("--0 close\r\n");
				gprsSingle = 0;
				//set_timer0(1000, 1);//关闭1S
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
				goto Gprsdealcmdend;
			}
			else if(*ret == '2'){
				gprsSingle = 1;
				gprsReady = 0;
				return;//正在找网络，不管.
			}
		}
	}

	if(GPRSLinkProcess == GPRS_MC55_CLOSE){//MC55I关闭中
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){//关闭成功 
			GPRSLinkProcess = GPRS_LINK_CMD;
//			set_timer0(10000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		ret = strstr((char*)gprsRevBuf, "ERROR");
		if(ret != 0){//关闭不成功 
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			set_timer0(5000, 1);//延时10秒重新启动模块
			isNetOK[0] = 0;
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
	}

	switch (GPRSLinkProcess)
	{
	case 0:
	case 1:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 15:
	case 13:
	case 14:
	case 16:
	case 17:
	case 19:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			GPRSLinkProcess++;
			set_timer0(0, 1);
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmderror;//Gprsdealcmdend;
		}
		break;
	case GPRS_TCPCLOSE_CMD://TCPIP关闭中，等待OK返回
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			if(GPRSLinkProcess > 20)
				GPRSLinkProcess = 20;// 重新连接
			set_timer0(0, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		goto Gprsdealcmdend;
		//goto Gprsdealcmderror;
	case 12:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsErrorTimes = 0;
			for(i=0; i<15; i++){
				if((gprsRevBuf[i]>='0')&&(gprsRevBuf[i]<='9'))
					break;
			}
			memcpy(GprsCardList, gprsRevBuf+i, 15);
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 18:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			ret = strstr((char*)gprsRevBuf, "%ETCPIP");
			if(ret == 0)
				break;
			gprsSNDerro = 0;
			gprsErrorTimes = 0;
			GPRSLinkProcess++;
			gprsAnswer = 'O';
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 20:
		ret = strstr((char*)gprsRevBuf, "CONNECT");
		if(ret != 0){
			if((gmissflag & 0xf0)==0x10){//登陆移动IF1
				isNetOK[0] = 1;
			}
			else if((gmissflag & 0xf0)==0x20){//登陆移动IF2
				isNetOK[0] = 2;
			}
			else{//登陆公交数据中心
				isNetOK[0] = 3;
			}
			GPRSLinkProcess++;
			set_timer0(0, 1);
//			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			goto Gprsdealcmderror;
		}
		break;
	case 2:
		ret = strstr((char*)gprsRevBuf, "OK");
		if(ret != 0){
			gprsAnswer = 'O';
			gprsErrorTimes = 0;
			ret = strstr((char*)gprsRevBuf, "READY");
			if(ret == 0)
				break;
			GPRSLinkProcess++;
			set_timer0(0, 1);
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else{
			gprsReady = 0;
			return;
		}
//		break;
	case 5:
		if(strstr((char*)gprsRevBuf, "OK") != 0){
			gprsErrorTimes = 0;
			gprsAnswer = 'O';
			ret = strstr((char*)gprsRevBuf, "1,");
			if(ret != 0){
				if(*(ret+2) != '2'){
					GPRSLinkProcess++;
					set_timer0(0, 1);
				}
			}
			goto Gprsdealcmdend;
		}
		else if(strstr((char*)gprsRevBuf, "ERROR") != 0){
			gprsErrorTimes++;
			goto Gprsdealcmdend;
		}
		else {
			gprsReady = 0;
			return;
		}
//		break;
	default:
	ret = strstr((char*)gprsRevBuf, "%IPDATA:");//%IPDATA:1,3,"676637"
	if(ret != 0){
		gprsAnswer = 'O';
		gprsErrorTimes = 0;
		gprsSNDerro = 0;
		for(index=0;index<MAX_RCV_PACKET_NUM;index++){
			if(gPacketRcvBuf[index].ucItemStatus != 0xea)
				break;
		}
		if(index>=MAX_RCV_PACKET_NUM){
			if(gprsReady != 0x55){//还在收新的数据
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
				gprsReady = 0;
			}
			return;
		}
// #ifdef _debug_
// 		debugstring("处理:");
// 		debugdata((unsigned char*)&index, 1, 1);
// #endif
		i = PeckData(&gPacketRcvBuf[index].LinkFrame, ret);
		gPacketRcvBuf[index].ucItemStatus = 0xea;
// #ifdef _debug_
// 		debugstring("接收:");
// //		debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, i+3, 1);
// #endif
		goto Gprsdealcmdend;
	}
	
	ret = strstr((char*)gprsRevBuf, "OK");//棋它
	if(ret != 0){
		if(strstr((char*)gprsRevBuf, "IPSEND") != 0){//IPSENDX
			//gprsAnswer = 'I';
			gprsAnswer = 'O';
			if(strstr((char*)gprsRevBuf, "%IPSD") != 0){
				gprsSendDlyflag = 'T';	//说明是延时发送的数据
			}
			else
				gprsSendDlyflag = 'N';
		}
		else
			gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		goto Gprsdealcmdend;
	}
	break;
	}

	if(strstr((char*)gprsRevBuf, "IPSEND") != 0){//如果是发送数据中，希望在ＯＫ后判断
		if(strstr((char*)gprsRevBuf, "%IPSEND:1") != 0){//
			ret = strstr((char*)gprsRevBuf, ",");
			if(ret != 0){
				ret++;
				i = 0xffffffff;
				while(1){
					if(*ret == ' '){
						ret++;
						continue;
					}
					if((*ret >= '0')&&(*ret <= '9')){
						if(i == 0xffffffff)
							i = 0;
						i = i*10 + *ret-'0';
					}
					else
						break;
					ret++;
				}
// #ifdef _debug_gprs
// 				debugstring("发送IPSS:i:");
// 				debugdata((unsigned char*)&i,1,1);
// #endif
				if(i <=11){//以前的数据发送不成功
					memset(gBusVerInfo.newbusBLKVer, 0, 2);//断开链接后要重新登陆
					gmissflag = 0;
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//(0);
					goto Gprsdealcmdend;
				}
			}
		}
		gprsReady = 0;
		return;
	}
	if(strstr((char*)gprsRevBuf, "%IPSD") != 0){//如果是发送数据中，希望在ＯＫ后判断
		gprsReady = 0;
		return;
	}

	ret = strstr((char*)gprsRevBuf, "%IPSP");//查询延时发送状态返回
	if(ret != 0){
		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xffffffff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xffffffff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if((i >= 500)&&(i <= 60000)){//设置延时发送为500-60000 毫秒 0为关闭延时
				gprsSendDlyflag = 'T';
			}
			else if(i == 0){
				gprsSendDlyflag = 'N';
			}
		}
		goto Gprsdealcmdend;
	}

	ret = strstr((char*)gprsRevBuf, "+CSQ");
	if(ret != 0){
		gprsAnswer = 'O';
		//gprsErrorTimes = 0;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				gprsSingle = i;
			}
		}
		goto Gprsdealcmdend;
	}
Gprsdealcmderror:
	ret = strstr((char*)gprsRevBuf, "ERROR");
	if(ret != 0){
		gprsErrorTimes++;
		ret = strstr((char*)gprsRevBuf, ":");
		if(ret != 0){
			ret++;
			i = 0xff;
			while(1){
				if(*ret == ' '){
					ret++;
					continue;
				}
				if((*ret >= '0')&&(*ret <= '9')){
					if(i == 0xff)
						i = 0;
					i = i*10 + *ret-'0';
				}
				else
					break;
				ret++;
			}
			if(i != 0xff){
				IPERROR_DEAL(i);
			}
		}
		goto Gprsdealcmdend;
	}

	ret = strstr((char*)gprsRevBuf, "%IPCLOSE");
	if(ret != 0){
		set_timer0(500, 1);//链接关闭,延时后重新连接
//		gprsErrorTimes = 0;
		for(i=0;i<(strlen((char*)gprsRevBuf));i++)
		{
			if(gprsRevBuf[i] == '5'){
				GPRSLinkProcess = 0;
				break;
			}
			else if((gprsRevBuf[i] == 0x0a)||(gprsRevBuf[i] == 0x0d))
				break;
		}
		if((GPRSLinkProcess > 20)&&(GPRSLinkProcess < 0xF0))
			GPRSLinkProcess = 20;
		isNetOK[0] = 0;

		memset(gBusVerInfo.newbusBLKVer, 0, 2);
		gmissflag = 0;
// #ifdef _debug_
// 		debugstring("--IPCLOSE--");
// 		debugdata((unsigned char*)&gprsErrorTimes, 1, 1);
// #endif
		goto Gprsdealcmdend;
	}

	if(strstr((char*)gprsRevBuf, "AT-Command Interpreter") != 0){
		gprsErrorTimes = 0;
		GPRSLinkProcess = GPRS_AT_WAIT;	//GPRS模块重启了
		set_timer0(40000, 1);//链接关闭,延时后重新连接
		isNetOK[0] = 0;
// #ifdef _debug_
// 		debugstring("retart--AT-Command In ready\r\n");
// #endif
		goto Gprsdealcmdend;
	}

Gprsdealcmdend:
	if(gprsErrorTimes > 20){//错误次数太多，重启模块
		if(gmissflag == MISS_M_SINGIN){
			set_KEK_SN(0xff);
			gmissflag = MISS_M_KEK;
			gmissOldflag = MISS_M_DPARAM;
		}
		gprsErrorTimes = 0;
		//set_timer0(1000, 1);//关闭1S
		//debugstring("--1 close\r\n");
		if(GPRSLinkProcess > 16){
			if(gmissflag & MISS_GJ){//如果正在登陆公交,登不上去,先暂停,做个标志
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				gprsReady = 0;
				return; //如果不是同时有问题,则退出重新找任务.
			}
		}
		if(GPRSLinkProcess == 20){//登陆不了后台
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		}
		else
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
	}
GPRSdealEnd:
	if(gprsReady != 0x55){//还在收新的数据
		gprsIndex = 0;
		memset(gprsRevBuf, 0, 20);
		gprsReady = 0;
	}
	return;
}
	
void GPRS_handler(unsigned char value)
{
	unsigned char gGprsMode;
 	char *ret=NULL;
//	char *start_p = NULL;
	unsigned int i;

	static unsigned int count_comma=0;		//用于记录这个^IPRECV后面的‘，’出现的次数  第二个时候再去找第三个， 然后算出长度

	unsigned int index;

	
	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;

	gprsRevBuf[gprsIndex] = value;

#ifdef _debug_gprs
// 	if(CDMA_REV_LEN>0)
// 	{
// 		//debugstring("CDMA_REV_LEN:");
// 		debugdata((unsigned char *)&value, 1, 0);
// 	}
// 	else if(guiMC323_REV_LEN>0){
// 		//debugstring("guiMC323_REV_LEN:");
// 		debugdata((unsigned char *)&value, 1, 0);
// 	}
// 	else
 	com_snd(COM1, 1, (unsigned char*)&value);
	//debugdataCOM(COM4,(unsigned char*)&value,1,0);
//	debugdata(&value,1,0);
#endif



	if(gGprsMode == Gmode_MC323){
		if(gmissflag == MISS_G_MSG1){
			if(value == '>'){
				sendmsgBody();
				set_timer0(40000, 1);
				CDMA_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return;
			}
		}
		if((CDMA_REV_LEN>0)&&((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0))){
			gprsIndex++;
			if(gprsIndex >= GPRSCOMMAX){
				gprsIndex = 0;
				CDMA_REV_LEN = 0;//此数据太长，丢弃
				return;
			}
			if(gprsIndex >= (CDMA_REV_LEN+16)){//^IPDATA:1,10,1234567890 数据加上前面的字节，再加上0d 0a OK致少多了14字节
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(index=0;index<MAX_RCV_PACKET_NUM;index++){
					if(gPacketRcvBuf[index].ucItemStatus != 0xea)
						break;
				}
				if(index>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
					return;
				}
				for(i=0; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				for(; i<16; i++){
					if(gprsRevBuf[i] == ',')
						break;
				}
				i++;
				if(i >= 16){
#ifdef _debug_
					debugstring("...");
#endif
					CDMA_REV_LEN = 0;
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);
					return;
				}
				gPacketRcvBuf[index].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&CDMA_REV_LEN, 2);
				memcpy(&gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf+i, CDMA_REV_LEN);
				gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_gprs
				debugstring("接收:");
				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, CDMA_REV_LEN, 1);
#endif
				CDMA_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return;
			}
			return;
		}
	}
	else if(gGprsMode == Gmode_MG323){
		if((guiMC323_REV_LEN>0)&&((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0))){//有数据需要接收
			gprsIndex++;
			if(gprsIndex >= GPRSCOMMAX){
				gGPRS_data_style.ucReadStyle = 0;
				gprsIndex = 0;
				guiMC323_REV_LEN = 0;//此数据太长，丢弃
#ifdef _debug_gprs
				debugstring("too long\r\n");
#endif

				gprsIndex = FRAME_SIZE;
				guiMC323_REV_LEN = FRAME_SIZE;	//pfquan 别扔了，先拿过去看看
			}
			if(gprsIndex >= guiMC323_REV_LEN){
				gGPRS_data_style.ucReadStyle = 0;
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(index=0;index<MAX_RCV_PACKET_NUM;index++){
					if(gPacketRcvBuf[index].ucItemStatus != 0xea)
						break;
				}
				if(index>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
#ifdef _debug_gprs
					debugstring("pack full return\r\n");
#endif
					return;
				}
				gPacketRcvBuf[index].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[index].LinkFrame.uiFrameLen, (unsigned char*)&guiMC323_REV_LEN, 2);
				memcpy(gPacketRcvBuf[index].LinkFrame.FrameBuf, gprsRevBuf, guiMC323_REV_LEN);
				gPacketRcvBuf[index].ucItemStatus = 0xea;
#ifdef _debug_gprs
// 				debugstring("MG323接收:");
// 				debugdata((unsigned char*)&gPacketRcvBuf[index].LinkFrame, guiMC323_REV_LEN+3, 1);
#endif
				guiMC323_REV_LEN = 0;
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);
				return;
			}
			return;
		}
	}
	else if(gGprsMode == Gmode_CDMA2000){
		if((guiMC323_REV_LEN>0)&&  ((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0)))
		{					//有数据需要接收
			gprsIndex++;

//			debugstring("E:\r\n");

			if(gprsIndex >= GPRSCOMMAX){
#ifdef _debug_gprs
				debugstring("too long\r\n");
#endif				
				guiMC323_REV_LEN = GPRSCOMMAX;
		//		count = 0;		//为了再进去看看数据
			
			}

			if(gprsIndex >= guiMC323_REV_LEN+2)			//\r\b 也收完
			{


#ifdef _debug_gprs999
				debugstring("\r\n--over---\r\n");						
				MSG_LOG("gprsIndex:%d\r\n",gprsIndex);
				debugstring("gprsRevBuf:");
				debugdata(gprsRevBuf,gprsIndex,1);
#endif
				gTcpIpbusy = TcpIPFree;
				gTcpIpbusydly = 0;			//空闲
				gErrorFlag &= (~ERROR_BUS_CONNECT);// 清除错误标识 能进这里说明网络都正常
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(i=0;i<MAX_RCV_PACKET_NUM;i++){
					if(gPacketRcvBuf[i].ucItemStatus != 0xea)
						break;
				}
				if(i>=MAX_RCV_PACKET_NUM){
					memset(gprsRevBuf, 0, gprsIndex);	//缓冲区满，此包丢弃
					gprsIndex = 0;
#ifdef _debug_gprs
					debugstring("full !!!\r\n");
#endif
					return;
				}
		
			//^IPRECV:1,113.89.126.181,8082,9,564879!@#
				gPacketRcvBuf[i].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[i].LinkFrame.uiFrameLen, (unsigned char*)&guiMC323_REV_LEN, 2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf, gprsRevBuf, guiMC323_REV_LEN);
				//Ascii2BCD((unsigned char *)ret,gPacketRcvBuf[i].LinkFrame.FrameBuf,guiMC323_REV_LEN*2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf,gprsRevBuf,guiMC323_REV_LEN);
				gPacketRcvBuf[i].ucItemStatus = 0xea;
#ifdef _debug_gprs
// 				debugstring("dat:");
//	 			debugdata(gPacketRcvBuf[i].LinkFrame.FrameBuf,10,1);
#endif
				gGPRS_data_style.ucReadStyle = 0;
				guiMC323_REV_LEN = 0;
				memset(gprsRevBuf, 0, GPRSCOMMAX);
				gprsIndex = 0;
				return;
			}
	
			return;
		}
	//模块收到数据后回响		//^IPSEND:1, 18
	//ASCii 表里面拒绝接受字符0x15 NAK(negative)
		if(((value == '>')/*||(value == 0x15)*/)&&(gprsRevBuf[gprsIndex-1]==0x0a)&&(gprsRevBuf[gprsIndex-2]==0x0d)){		//\r\v>   这个是收到数据回响

			gprsIndex++;
			gprsRevBuf[gprsIndex+1] = 0;//在接收的数据后加上个结束符

#ifdef _debug_gprs
			debugstring("#>,\r\n");
#endif
			gGPRS_data_style.ucSendstyle  = GPRS_SISW;		//去发送数据吧
		
			gprsIndex -=3;

			memset(gprsRevBuf+gprsIndex,0,3);		//只清理3个看看效果
// 				memset(gprsRevBuf, 0, gprsIndex);
// 				gprsIndex = 0;

// 				gprsIndex = 0;
// 				memset(gprsRevBuf, 0, GPRSCOMMAX);

// 				gprsReady = GPRS_DATA_OK;//数据准备好,需要处理
// 				gprsCmdDeal_CDMA_3G();
// 				gprsReady = GPRS_GOON;//处理完后,置0
			return ;
		}
		
	//数据接收		//^IPRECV:1,113.89.126.181,8082,9,564879!@#
	//				  ^IPRECV:1,113.118.100.92,8811,27,
// 	if (  ((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0)))
// 	{
		
		ret = NULL;
		ret = strstr((const char*)gprsRevBuf, "RECV");
		if(ret != 0)
		{
//			start_p = ret; 
			gGPRS_data_style.ucReadStyle = GPRS_SISR;		//这个时候别发数据了

// 			debugstring("ret");
// 			debugstring(ret);
// 			debugstring("VV\r\n");

 			ret = NULL;

			if(value==',')count_comma++;

			if (count_comma <4)
			{
				gprsIndex++;
				return;
			}

			ret = NULL;
			ret = my_strstr((const char*)gprsRevBuf,",",3);			//找到前面的长度前面的，
			if(ret != 0)
			{	
// 				debugstring("eee:");
// 				debugdata(gprsRevBuf,gprsIndex,1);
// 				debugdata(ret,5,1);
// 				debugstring("rrr\r\n");
				i=0;
				ret++;
				while((*ret >= '0')&&(*ret <= '9'))
				{
					if (i>1024){		//异常？
						i=0;
						break;		
					}
					
					i = i*10 + (*ret-'0');
					ret++;
				}
				
				count_comma = 0;
				guiMC323_REV_LEN = i;
#ifdef _debug_gprs
				//MSG_LOG("len:%d\r\n",guiMC323_REV_LEN);
#endif
// 											
				gTcpIpbusydly = 3;			//3S后超时

				memset(gprsRevBuf, 0, GPRSCOMMAX);
				gprsIndex = 0;
// 
// 
				return;
// 
 			}	
 			
		}
//		}
	}
	else if(gGprsMode == Gmode_ALL4G){
		if((guiMC323_REV_LEN>0)&&  ((GPRSLinkProcess >= 0x15)&&(GPRSLinkProcess < 0xb0)))
		{					//有数据需要接收
			gprsIndex++;

//			debugstring("E:\r\n");

			if(gprsIndex >= GPRSCOMMAX){
#ifdef _debug_gprs
				debugstring("too long\r\n");
#endif				
				guiMC323_REV_LEN = GPRSCOMMAX;
//				count = 0;		//为了再进去看看数据
			}

			if(gprsIndex >= guiMC323_REV_LEN)//+2)			//\r\b 也收完
			{
#ifdef _debug_gprs999
				debugstring("\r\n--over---\r\n");						
				MSG_LOG("gprsIndex:%d\r\n",gprsIndex);
				debugstring("gprsRevBuf:");
				debugdata(gprsRevBuf,gprsIndex,1);
#endif
// 				gTcpIpbusy = TcpIPFree;
// 				gTcpIpbusydly = 0;			//不能空闲，还有在等发送数据的>号，一定不能清除
				gErrorFlag &= (~ERROR_BUS_CONNECT);// 清除错误标识 能进这里说明网络都正常
				gprsAnswer = 'O';
				gprsErrorTimes = 0;
				gprsSNDerro = 0;
				for(i=0;i<MAX_RCV_PACKET_NUM;i++){
					if(gPacketRcvBuf[i].ucItemStatus != 0xea)
						break;
				}
				if(i>=MAX_RCV_PACKET_NUM){
					memset(gprsRevBuf, 0, gprsIndex);	//缓冲区满，此包丢弃
					gprsIndex = 0;
#ifdef _debug_gprs
					debugstring("full !!!\r\n");
#endif
					return;
				}
		
			//^IPRECV:1,113.89.126.181,8082,9,564879!@#
				gPacketRcvBuf[i].LinkFrame.ucFrameType = 1;
				memcpy(gPacketRcvBuf[i].LinkFrame.uiFrameLen, (unsigned char*)&guiMC323_REV_LEN, 2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf, gprsRevBuf, guiMC323_REV_LEN);
				//Ascii2BCD((unsigned char *)ret,gPacketRcvBuf[i].LinkFrame.FrameBuf,guiMC323_REV_LEN*2);
				memcpy(gPacketRcvBuf[i].LinkFrame.FrameBuf,gprsRevBuf,guiMC323_REV_LEN);
				gPacketRcvBuf[i].ucItemStatus = 0xea;
// #ifdef _debug_gprs
// 				debugstring("dat:");
// 	 			debugdata(gPacketRcvBuf[i].LinkFrame.FrameBuf,10,1);
// #endif
				gGPRS_data_style.ucReadStyle = 0;
				guiMC323_REV_LEN = 0;
				memset(gprsRevBuf, 0, GPRSCOMMAX);
				gprsIndex = 0;
				return;
			}
	
			return;
		}
	//模块收到数据后回响		//^IPSEND:1, 18
	//ASCii 表里面拒绝接受字符0x15 NAK(negative)
//		if(((value == '>')/*||(value == 0x15)*/)&&(gprsRevBuf[gprsIndex-1]==0x0a)&&(gprsRevBuf[gprsIndex-2]==0x0d))
		if(value == '>')
		{		//\r\v>   这个是收到数据回响
			gprsIndex++;
			gprsRevBuf[gprsIndex+1] = 0;//在接收的数据后加上个结束符

// #ifdef _debug_gprs
// 			debugstring("4G>,\r\n");
// #endif
			gGPRS_data_style.ucSendstyle  = GPRS_SISW;		//去发送数据吧
		
			gprsIndex = 0;

			memset(gprsRevBuf+gprsIndex,0,3);		//只清理3个看看效果
// 				memset(gprsRevBuf, 0, gprsIndex);
// 				gprsIndex = 0;

// 				gprsIndex = 0;
// 				memset(gprsRevBuf, 0, GPRSCOMMAX);

// 				gprsReady = GPRS_DATA_OK;//数据准备好,需要处理
// 				gprsCmdDeal_CDMA_3G();
// 				gprsReady = GPRS_GOON;//处理完后,置0
			return ;
		}
	}
// 	else if(gGprsMode == Gmode_ALL4G){		
// 		if (GPRSLinkProcess==GPRS_LINK_UP)		//开机接收RDY，判断开机成功"0D 0A 52 44 59 0D 0A 0D 0A"		\r\nRDY\r\n\r\n
// 		{
// 			if ((value=='Y')&&(gprsIndex>2))
// 			{
// 				if ((gprsRevBuf[gprsIndex-2]=='R')&&(gprsRevBuf[gprsIndex-1]=='D'))
// 				{
// 					gprsIndex=0;
// 					isNetOK[0] = 0;
// 					//	debugstring("EC20 ready\r\n");
// 					gprsErrorTimes = 0;
// 					GPRS_MC55i_open();      //拉高POWN
// 					GPRSLinkProcess = GPRS_AT_WAIT;
// 					set_timer0(500, 1);	//开机后会自报一些信息
// //					gprsPowerState = 1;	//模块激活
// 				}
// 			}
// 			
// 			gprsIndex++;
// 			if(gprsIndex >= GPRSCOMMAX)
// 				gprsIndex = 0;
// 			return;
// 			
// 		}
// 		if ((GPRSLinkProcess==42)&&(value=='>'))
// 		{
// 			GPRSLinkProcess=23;
// 			set_timer0(0, 1);
// 			gprsIndex = 0;
// 			//	gprsErrorTimes = 0;			//不能清楚gprsErrorTimes，异常时候'>'会出现
// 			return;
// 		}
// 	}


	/******************************************************************************************************************/
	/******************************************************************************************************************/


	if((value == 0x0a)&&(gprsRevBuf[gprsIndex-1]==0x0d)){		//一个包收完
		gprsIndex++;
		gprsRevBuf[gprsIndex+1] = 0;//在接收的数据后加上个结束符
#ifdef _debug_gprs
// 		debugstring("dex=:");
// 		debugdata((unsigned char *)&gprsIndex,2,1);
		//	debugdata((unsigned char *)&gprsReady,1,1);
// 		debugstring("CMD:");
// 		debugstring((char*)gprsRevBuf);
#endif
		
		if(gGprsMode == Gmode_MC323){
			gprsReady = GPRS_DATA_OK;//数据准备好,需要处理
			gprsCmdDeal_CDMA();
		}
		else if(gGprsMode == Gmode_MG323){
			gprsReady = GPRS_DATA_OK;//数据准备好,需要处理
			gprsCmdDeal_MG323();
			gprsReady = GPRS_GOON;//处理完后,置0
		}
		else if(gGprsMode == Gmode_CDMA2000){
			//后台发送的数据里面如果有0D0A 则这里也进去处理了，但是要按照长度来接受
			gprsReady = GPRS_DATA_OK;//数据准备好,需要处理
			gprsCmdDeal_CDMA_3G();
			gprsReady = GPRS_GOON;//处理完后,置0
		}
		else if(gGprsMode == Gmode_ALL4G){
			gprsReady = GPRS_DATA_OK;
			gprsCmdDeal_ALL4g();
			gprsReady = GPRS_GOON;
		}
		else{
			gprsReady = GPRS_DATA_OK;//数据准备好,需要处理
			gprsCmdDeal();
			gprsReady = GPRS_GOON;//处理完后,置0
		}

		return;
	}


	gprsIndex++;
	if(gprsIndex >= GPRSCOMMAX)
		gprsIndex = 0;
	return;
}
//初始化GPRS端口及模块
void GPRS_INIT(unsigned int band)
{
	unsigned char gprcom;
//	unsigned char gGprsMode;


	
//	gGprsMode = in_getGprsModeStyle();
	gprcom = getGprsCom();
#ifdef _debug_gprs
	debugstring("INI gprcom:");
	debugdata((unsigned char*)&gprcom, 1, 0);
	debugstring(":");
	debugdata((unsigned char*)&band, 4, 0);
	debugstring("|\r\n");
#endif
// 	if(gGprsMode == Gmode_900C)
// 		com_init(gprcom, 9600);
// 	else if(gGprsMode == Gmode_WIFI_TLH)
// 		com_init(gprcom, 115200);
// 	else
	com_init(gprcom, band);

//	if(gGprsMode != Gmode_OTHER){//外接调度机，不用此口
		COM_it_enable(gprcom);
//	}
}

//V231 初始化放到GPRS库里面。
void GPRSConfig(void)
{
	if(in_getGprsModeStyle() != Gmode_Close){
		if((in_getGprsModeStyle() == Gmode_MODEM)||(in_getGprsModeStyle() == Gmode_CDMA2000)//){
			||(in_getGprsModeStyle() == Gmode_ALL4G)){
			GPRS_INIT(115200);
		}
		else{
			GPRS_INIT(9600);//此端口用于GPRS连接口
		}
	}
}

void gprs_par_init(void)
{
	unsigned char i;

	GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//0;
	
	gprsErrorTimes=0;
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gGPRS_data_style.ucSendstyle = 0;
	gGPRS_data_style.ucReadStyle = 0;
	gGPRS_data_style.uiNum = 0;
	guiMC323_REV_LEN = 0;
	
	initTCPIPnode();
	for(i=0;i<MAX_RCV_PACKET_NUM;i++)
		gPacketRcvBuf[i].ucItemStatus = 0;
	memset(GprsCardList, 0, 16);
	gpacSNo = 0;//包序号
}





//获得一个空闲的帧缓冲,返回空闲帧索引jgwei updata
unsigned char GetFrameBuf(void)
{
	unsigned char i;

	for(i=0; i<MAX_RCV_PACKET_NUM; i++) 
	{
		if(!(gPacketRcvBuf[i].ucItemStatus & MASK_RCV_BUF_USED))
		{	
			break;
		}
	}
	if(i == MAX_RCV_PACKET_NUM) 
	{								//没有可以获得的缓冲
		i = 0xff;
	} 
	else 
	{
		memset(gPacketRcvBuf[i].LinkFrame.FrameBuf, 0x00, sizeof(gPacketRcvBuf[i].LinkFrame.FrameBuf));	//清零将要使用的帧缓冲
	}
	return i;
}

//从接收缓冲中获得一个有效的数据帧 OK
unsigned char PeekFrame(void)
{
	unsigned char i;
	
	for(i=0; i<MAX_RCV_PACKET_NUM; i++) 
	{
		if(gPacketRcvBuf[i].ucItemStatus == 0xea) 
		{				//接收帧不空
			break;
		}
	}
	if(i == MAX_RCV_PACKET_NUM)
	{
		return 0xff;
	}
	else 
	{
		return i;	 //返回有效的接收帧索引
	}
}

void mainGprs(void)
{
	unsigned char buff[20];
	unsigned char i, ret;
	unsigned char gGprsMode;
	unsigned short plen=0;//,uslen=0;
#ifdef _debug_gprs
#endif
	
	gGprsMode = in_getGprsModeStyle();
	gGprsMode &= 0x0F;

	if(gGprsMode == Gmode_MG323){ //这里处理发送数据命令的数据部分
		if(gGPRS_data_style.ucSendstyle == GPRS_SISW){
#ifdef _debug_gprs
// 			debugstring("数据:");
// 			debugdata((unsigned char*)&gGPRS_data_style.uiNum, 4, 1);
#endif
		//		if(memcmp(gMC55iSendbuff, "数据有效", 8)==0){//不管是否有效，都应该发
		//			memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
			gprs_DATA_snd(gGPRS_data_style.uiNum, gMC55iSendbuff+12);//串口中断中不能发送数据
			memset(gMC55iSendbuff, 0, 50);
		//		}
			gGPRS_data_style.ucSendstyle = 0;
			gGPRS_data_style.uiNum = 0;
		}
		else if (gGPRS_data_style.ucReadStyle == GPRS_SISR)
		{
#ifdef _debug_gprs
			debugstring("&");
#endif
			if(gTcpIpbusy != TcpIPBusy){//正在发送数据，不能去取数据
#ifdef _debug_gprs
				debugstring("读取数据\r\n");
#endif
				MC55i_SISREAD();
				gGPRS_data_style.ucReadStyle = GPRS_SISRing;
			}
		}
		else{
			if(GPRSLinkProcess == TCPSTARTSTAT){//GPRS已经登陆，可以发数据
				gprs_send_data(1,0,buff);//处理缓存中是否还有数据发送。没事就查一次
			}
		}
	}//#ifdef Gmode_MG323
	else if((gGprsMode == Gmode_CDMA2000)||(gGprsMode == Gmode_ALL4G))
	{
		if(gGPRS_data_style.ucSendstyle == GPRS_SISW){
#ifdef _debug_gprs
			debugstring("*^");
//  			debugstring("3G数据:");
//  			debugdata((unsigned char*)&gGPRS_data_style.uiNum, 4, 1);
#endif
			//		if(memcmp(gMC55iSendbuff, "数据有效", 8)==0){//不管是否有效，都应该发
			//			memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
			
			ModeS_DataSend(gGPRS_data_style.uiNum, gMC55iSendbuff+12);//串口中断中不能发送数据
			memset(gMC55iSendbuff, 0, 50);
			gGPRS_data_style.uiNum = 0;	
 			gGPRS_data_style.ucSendstyle = 0;
 			gTcpIpbusy = TcpIPFree;
			gTcpIpbusydly = 0;


		}
		else if ((gGPRS_data_style.ucReadStyle != GPRS_SISR)&&(gTcpIpbusy != TcpIPBusy)){//在发数据时不能再发
			plen = TcpIPSd_Pick(gMC55iSendbuff+12);
			if (plen != 0xffff)
			{
				gGPRS_data_style.uiNum = plen;
				
				ModeS_IPSend(plen, gGprsMode);
			}
		}
#ifdef _debug_gprs
		else{
			debugstring(".");
		}
#endif
	}

	in_deal_ring();
//	gprsCmdDeal();//处理GPRS模块给的数据
	gprsReady = 0;//处理完后,置0
//	if((isNetOK[0] != 0)||(isNetOK[1] != 0)){
		i = PeekFrame();
		if(i != 0xff) 
		{
#ifdef _debug_gprs
// 			debugstring("RcvFrame: ");
// 			memcpy(&plen,gPacketRcvBuf[i].LinkFrame.uiFrameLen,2);
// 			debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf,plen,1);
#endif
//应用层数据帧
#ifdef	_debug_gprs 
			debugstring("TaskGprsRev-------:\r\n");
			debugdata((unsigned char *)&i,1,1);
			memcpy(&plen,gPacketRcvBuf[i].LinkFrame.uiFrameLen,2);
			debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf,plen,1);
#endif
			if((gmissflag & 0xf0)==0x10){//登陆移动IF1   isNetOK[0] = 1;
				gGPRS_Cmd_outtime = 0;
				in_IF18583Deal(i);
				gPacketRcvBuf[i].ucItemStatus = 0;
//				GPRSLinkProcess = 21;
			}
			else if((gmissflag & 0xf0)==0x20){//登陆移动IF2 isNetOK[0] = 2;
				gGPRS_Cmd_outtime = 0;
				in_IF2TLVDeal(i);
				gPacketRcvBuf[i].ucItemStatus = 0;
			}
			else{//登陆公交数据中心  此路空闲
//				DataPacketProcess(i); //应用层数据包处理 包括后台的响应和后台的命令
//				GDServerProcess();
				gGPRS_Cmd_outtime = 0;

				//debugstring("xxxx\r\n");

				ret = GJDataDeal((unsigned char*)&gPacketRcvBuf[i].LinkFrame);//公交数据处理			
				if(ret == 0){
					gPacketRcvBuf[i].ucItemStatus = 0;
				}
				else{
					gPacketRcvBuf[i].ucItemStatus = 0xea;
#ifdef _debug_gprs
					debugstring("MAIN:");
					debugdata((unsigned char*)&gPacketRcvBuf[i], 20, 1);
#endif
				}
			}
		}
//	}
}

//暂停当前的GPRS
void stopCCgprs(unsigned short mode)
{
	in_gErrortimes(20, mode);
	gErrorFlag |= mode;
	gmissflag = 0;
}

extern void readmobilsendStyle(void);
void GprsSingleCheck(void)//检测ＧＰＲＳ信号
{
//	static char gscDly;
	char buffer[20];

	if(GPRSLinkProcess <= 6){
		return;
	}
	
// 	if(gscDly++ < 8)
// 		return;
//	gscDly = 0;
	if((gprsAnswer == 'O')||(gprsAnswer == 'I')){
		strcpy(buffer, "AT+CSQ\r\n");//检查GSM网络注册情况
		gprs_snd(strlen(buffer), (char*)buffer);
	}
}

void GprsIPSPcheck(void)//检测多链接模式
{
	char buff[20];

	if(GPRSLinkProcess <= 20){
		return;
	}
	strcpy((char*)buff, "AT%IPSP?\r\n");//多链接模式
	gprs_snd(strlen((char*)buff), buff);
}


void GPRSModeBaudChage(unsigned char mOdduc)
{
	char gpscbuf[50];
	
#ifdef _debug_gprs
	debugstring("change bps---\r\n");
#endif
	if((mOdduc&0xF0) == 0x80){
		strcpy((char*)gpscbuf, "GT$POWERON:115200\r\n");  //通知模块CPU的端口改成115200
		gprs_snd(strlen((char*)gpscbuf), gpscbuf);
	}
	else{
		if((mOdduc == Gmode_CDMA2000) || (mOdduc == Gmode_ALL4G))					//错误修改-兼容
		//if(mOdduc == Gmode_CDMA2000)
			GPRS_INIT(9600);
		else
			GPRS_INIT(115200);	
	}
	delayxms(50);
	memset(gpscbuf,0,20);
	if(mOdduc == Gmode_CDMA2000)
		strcpy(gpscbuf, "AT+IPR=115200\r\n");//波特率改成115200
	else if(mOdduc == Gmode_ALL4G)
		strcpy(gpscbuf, "AT+IPR=115200;&W\r\n");		//AT+IPR=115200   + //AT&W
	else
		strcpy(gpscbuf, "AT+IPR=9600\r\n");//波特率改成9600

	gprs_snd(strlen(gpscbuf), gpscbuf);		//给模块发送指令
	delayxms(200);
	if(mOdduc == 0x80){
		strcpy((char*)gpscbuf, "GT$POWERON:9600\r\n");		//通知模块CPU的端口改成9600
		gprs_snd(strlen((char*)gpscbuf), gpscbuf);
	}
	else{
		if((mOdduc == Gmode_CDMA2000) || (mOdduc == Gmode_ALL4G))
		//if(mOdduc == Gmode_CDMA2000)
			GPRS_INIT(115200);
		else
			GPRS_INIT(9600);
	}
}

extern void getDefaultAPN(char *rAPN, unsigned char mode);//mode=0 公交，1 移动。。;
extern void US1_It( void );
extern void TaskRecWrite(void);
//黑名单应答
extern unsigned int Buid_GJ_DBLK(unsigned char *dat);
#ifdef _debug_
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);

#endif
void main_GpsRs(void)
{
	char gpsrsP[100];
	char APNnet[32];
	int ret;
	unsigned int t;
	unsigned char gprsMode;
	unsigned char Moddstyle;
//	static unsigned int cnt=0;

	gprsMode = in_getGprsModeStyle();
	Moddstyle = (gprsMode&0xF0);//是否用外部MODEM
	gprsMode &= 0x0F;

	clr_wdt();

	t = get_timer0(1);

	if(t != 0){
		goto maingprs1;	
	}

	if(gmissflag == MISS_G_MSG1){//要发送短信息
		if((GPRSLinkProcess > 14) && (GPRSLinkProcess <= TCPSTARTSTAT)){
			sendmsgHead();		
			set_timer0(10000, 1);
			return;
		}
	}

	if(in_checkRingstyle() == -1)
		goto maingprs2;

#ifdef _debug_gprs
// 	debugstring("GPRSLinkProcess:");
// 	debugdata((unsigned char*)&GPRSLinkProcess, 1, 1);
#endif
	if(gprsErrorTimes > 20){//错误次数太多，重启模块
		gprsErrorTimes = 0;
#ifdef _debug_gprs
		debugstring("err too much\r\n");
#endif
		if((GPRSLinkProcess == 20)||(GPRSLinkProcess == 21)){
			if((gmissflag & MISS_GJ)||(gmissflag == 0)){//如果正在登陆公交,登不上去,先暂停,做个标志
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if((gmissflag & MISS_IF1)||(gmissflag & MISS_IF2)){
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
// 			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
// 				return; //如果不是同时有问题,则退出重新找任务.
// 			}
		}
// 		if(gmissflag & MISS_GJ){
// 			gmissflag = 0;
// 			gmissOldflag = 0;
//			return;
// 		}
//		set_timer0(1000, 1);//关闭1S
#ifdef _debug_gprs
			debugstring("--4 close\r\n");
#endif
		if(GPRSLinkProcess == 20){
			if(gprsMode == Gmode_ALL4G)
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;
			else
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
		}
		else {
			if(gprsMode == Gmode_ALL4G)
				GPRSLinkProcess = 0;
			else
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//	
		}
		return;
	}

#ifdef _debug_gprs
 	if (GPRSLinkProcess < 22)
	{
		debugstring("stepxxx:");
		debugdata((unsigned char*)&GPRSLinkProcess,1,1);
	}
#endif


	switch(GPRSLinkProcess)
	{
	case GPRS_MODE_POWER_DOWN:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_DOWN!\r\n");
#endif
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		
		gmissOldflag = 0;
		gmissflag = 0;

		if(gprsMode == Gmode_900C){
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			break;
		}
		powerOff4V0();
		set_timer0(5000, 1);
		GPRSLinkProcess = GPRS_MODE_POWER_ON;
		if(Moddstyle == 0x80){
			strcpy((char*)gpsrsP, "GT$POWEROFF\r\n");
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		}
		break;
	case GPRS_MODE_POWER_ON:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_ON!\r\n");
#endif
		if(gprsMode == Gmode_900C){
			GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
			break;
		}
		
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		
		gmissOldflag = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_LINK_CMD;

		GPRS_open();//上电时，先拉低，再拉高。,一直是高的模块开不了机
		if(Moddstyle == 0x80){
			strcpy((char*)gpsrsP, "GT$POWERON\r\n");
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			powerON4V0(115200);
		}
		else{
			if((gprsMode == Gmode_MODEM)||(gprsMode == Gmode_CDMA2000)//){
				||(gprsMode == Gmode_ALL4G)){
				powerON4V0(115200);
			}
			else{
				powerON4V0(9600);//此端口用于GPRS连接口
			}
		}
		set_timer0(4000, 1);
		break;
	case GPRS_MODE_NEED_CLOSE://模块需要重启，发关闭模块命令.旧的板子，这里关闭电源
		if((gprsMode == Gmode_MG323)||(gprsMode == Gmode_MC323)){
			GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			break;
		}
#ifdef _debug_gprs
	debugstring("case==GPRS_MODE_NEED_CLOSE\r\n");
#endif
		GPRSLinkProcess = GPRS_MC55_CLOSE;
		if(gprsMode == Gmode_MG323){
			GPRS_MC55i_open();//GPRS_open();//再变成高电平，拉低POWN >1 秒关机 大于2秒，先关开再开机
			set_timer0(4000, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_MC323){
			GPRS_MC55i_open();//拉低2秒，关机，
			set_timer0(2500, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRS_open();////拉低POWN  //EC20 开机低电平>=100ms，关机低电平>=600ms;  关机命令AT+QPOWD
			set_timer0(30000, 1); //实验测试需拉
		}
		else if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode == Gmode_CDMA2000){
			;
		}
		else{
			GPRS_close_Gt900(5000);
		}
		gprsErrorTimes = 0;
		isNetOK[0] = 0;
		
		gmissOldflag = 0;
		gmissflag = 0;
		break;
	case GPRS_MC55_CLOSE:
#ifdef _debug_gprs
		debugstring("case==GPRS_MC55_CLOSE");
#endif

		if((gprsMode == Gmode_MG323)||(gprsMode == Gmode_MC323)){
			GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			break;
		}
		
		GPRSLinkProcess = GPRS_LINK_CMD;
		if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode != Gmode_900C){
			GPRS_open();//GPRS_MC55i_open();//拉高POWN
			set_timer0(5000, 1);
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRS_MC55i_open();     //拉高POWN
			GPRSLinkProcess = GPRS_LINK_CMD;
			gprsErrorTimes = 0;
			set_timer0(30000, 1);	//等待POWERED DOWN提示才算关机响应正常，但不是实际关机
			isNetOK[0] = 0;
		}
		break;
	case GPRS_LINK_CMD://关闭中，打开模块电源,低电平 800ms
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_CMD\r\n");
#endif
		GPRSLinkProcess = GPRS_LINK_UP;
		if(gprsMode == Gmode_MG323){
			GPRS_MC55i_open();//GPRS_open();//拉低POWN 拉低POWN >1 秒 打开模块，或关闭模块【本来是开的】
			set_timer0(4000, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_MC323){
			GPRS_MC55i_open();//GPRS_open();//拉低>100ms开机
			set_timer0(200, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else if(gprsMode == Gmode_ALL4G){
			gprsIndex = 0;
			gprsErrorTimes = 0;
			memset(gprsRevBuf, 0, 50);
			GPRS_open();			//拉低POWN
			set_timer0(30000, 1);	//长时间拉低，直到收到模块“RDY”提示后退出,目的是复位时候需要，如果正常开机只需>100ms即有效
		}
		else if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode == Gmode_CDMA2000){
			set_timer0(1000, 1);
		}
		else{
			GPRS_open();
			set_timer0(800, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONHI\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		break;
	case GPRS_LINK_UP:
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_UP\r\n");
#endif

		GPRSLinkProcess = GPRS_AT_WAIT;
		if(gprsMode == Gmode_OTHER){
			set_timer0(5, 1);
		}
		else if(gprsMode != Gmode_900C){
			GPRS_open();//GPRS_MC55i_open();//再变成高电平，拉低POWN >2秒关机 大于2秒，先关开再开机
			set_timer0(10000, 1);//10000
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONLO\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		else{
			GPRS_MC55i_open();//再变成高电平，启动模块.旧的板子，这里打开电源
			set_timer0(40000, 1);
			if(Moddstyle == 0x80){
				strcpy((char*)gpsrsP, "GT$PWONHI\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			}
		}
		break;
	case GPRS_NEED_CLOSEIP:
#ifdef _debug_gprs
		debugstring("case==GPRS_NEED_CLOSEIP\r\n");
#endif
		tcpipClose(0);
		gmissflag = 0;
		gprsErrorTimes += 2;
		set_timer0(5000, 1);
		break;
	case GPRS_AT_WAIT:
#ifdef _debug_gprs
		debugstring("case==GPRS_AT_WAIT\r\n");
#endif

		if(gprsMode == Gmode_OTHER){
			GPRSLinkProcess = TCPSTARTSTAT;
			isNetOK[0] = 3;
		}
		else{
			gprsErrorTimes = 0;
			GPRSLinkProcess = 0;
		}
		break;
	case GPRS_TCPCLOSE_CMD:
#ifdef _debug_gprs
		debugstring("case==GPRS_TCPCLOSE_CMD\r\n");
#endif

		if(gprsMode == Gmode_CDMA2000)
		{
			GPRSLinkProcess = 9;			//错误修改，这里为什么是0?,断了之后需要重新设置传输MODE。。从6始。
		}else
			GPRSLinkProcess = 20;
		break;
	case GPRS_Out_CMD:
		GPRSLinkProcess = 9;
		set_timer0(1000, 1);
		break;
	case 0:
 
		if((gprsErrorTimes >= 3)&&(gprsErrorTimes < 10)){ //如果不对，则出厂的波特率115200没改过来
			GPRSModeBaudChage(gprsMode);
			gprsErrorTimes ++;
			set_timer0(5000, 1);
			break;
		}
		else if(gprsErrorTimes == 8){
			if(gprsMode == Gmode_MG323){
				GPRSLinkProcess = GPRS_LINK_CMD;//再拉低PWON，开或关机一次
			}
			else if(gprsMode != Gmode_ALL4G){
				GPRSLinkProcess = GPRS_MODE_POWER_DOWN;
			}
			gprsErrorTimes+=2;
			break;
		}

		gprsIndex = 0;
		memset(gprsRevBuf, 0, 20);
		
		isNetOK[0] = 0;
		isNetOK[1] = 0;
		gAPN_Style = 0;

#ifdef _debug_gprs
		//debugdata((unsigned char*)&gprsErrorTimes, 1, 0);
#endif
		gprsErrorTimes++;
		set_timer0(3000, 1);
		memcpy((char*)gpsrsP, "AT\r\n", 4);
		gprs_snd(4, gpsrsP);
 		break;
	case 1:
		set_timer0(5000, 1);
		memcpy((char*)gpsrsP, "ATE0\r\n", 6);//关闭回显
		//memcpy((char*)gpsrsP, "AT\r\n", 4);//
		gprs_snd(6, gpsrsP);
		gprsErrorTimes+=5;
		break;
	case 2:
		set_timer0(6000, 1);
		memcpy((char*)gpsrsP, "AT+CPIN?\r\n", 10);//检查SIM工作是否正常
//		strcpy(gpsrsP, "AT+CPIN=?\r\n");
		gprs_snd(10, gpsrsP);
		gprsErrorTimes+=5;
		break;
	case 3:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		set_timer0(2000, 1);
		if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "AT^SWSPATH=1\r\n", 14);//设置语音通道
			gprs_snd(14, gpsrsP);
		}
		else if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT+CTA=120\r\n", 12);//进入休眠模式等待时长
			gprs_snd(12, gpsrsP);
		}
		else if(gprsMode == Gmode_CDMA2000){
			memcpy((char*)gpsrsP, "AT+CGMM\r\n", 9);//该命令的主要功能是查询产品名称信息。ATI
			//memcpy((char*)gpsrsP, "ATI\r\n", 5);		//查询具体版本  测试后时候可以使用
			gprs_snd(9, gpsrsP);
		}
		else if(gprsMode == Gmode_ALL4G){////查询SIM卡IMSI号码  //AT+CIMI   识别不同运营商
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			memcpy((char*)gpsrsP, "AT+CIMI\r\n", 9);//识别不同运营商
			gprs_snd(9, gpsrsP);
			set_timer0(5000, 1);
		}
		else{
			memcpy((char*)gpsrsP, "AT%SLEEP=0\r\n", 12);//关闭睡眠模式
			gprs_snd(12, gpsrsP);
		}
		gprsErrorTimes+=5;
		break;
	case 4:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		set_timer0(5000, 1);
		if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "AT+CLIP=1\r\n", 11);//设置来电显示
			gprs_snd(11, gpsrsP);
		}
		else if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT^MEID\r\n", 9);//插放RUIM 卡和不插RUIM 卡查到的号码是不同的
			gprs_snd(9, gpsrsP);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CREG=1\r\n", 11);//设置模块网络注册提示，当模块从GSM网络中掉线后，会自动上报
			gprs_snd(11, gpsrsP);
		}
		gprsErrorTimes++;
		break;
	case 5:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT+CREG=?\r\n", 11);//检查GSM网络注册情况
			gprs_snd(11, gpsrsP);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CREG?\r\n", 10);//检查GSM网络注册情况
			gprs_snd(10, gpsrsP);
		}
		gprsErrorTimes++;
		set_timer0(5000, 1);
		break;
	case 6:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "AT+CLVL=4\r\n", 11);//设置语音音量
			gprs_snd(11, gpsrsP);
		}else if (gprsMode == Gmode_CDMA2000){//模式设置移到下面9
			GPRSLinkProcess++;
			break;
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRSLinkProcess++;
			break;
		}
		else{
			memcpy((char*)gpsrsP, "AT+CSQ\r\n", 8);//检查GSM网络注册情况
			gprs_snd(8, gpsrsP);
		}
		set_timer0(5000, 1);
		gprsErrorTimes++;
		break;
	case 7:
		memset(gprsRevBuf,0,100);
		gprsIndex = 0;
		if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT+CREG=1\r\n", 11);//设置模块GPRS网络注册提示，当模块从GPRS网络中掉线后，会自动上报+CGREG: 1,0
			gprs_snd(11, gpsrsP);
		}else if (gprsMode == Gmode_CDMA2000){
			memcpy((char*)gpsrsP, "AT^SYSINFO\r\n", 12);//查询系统信息
			gprs_snd(12, gpsrsP);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CGREG=1\r\n", 12);//设置模块GPRS网络注册提示，当模块从GPRS网络中掉线后，会自动上报+CGREG: 1,0
			gprs_snd(12, gpsrsP);
		}
		gprsErrorTimes++;
		set_timer0(5000, 1);
		break;
	case 8:
		memset(gpsrsP,0,sizeof(gpsrsP));
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
// 			strcpy(gpsrsP, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");//设置短信存储地址
// 			gprs_snd(strlen(gpsrsP), gpsrsP);
		}else if (gprsMode == Gmode_CDMA2000){
#ifdef _debug_gprs
			debugstring("PPP无线连接\r\n");
#endif
			strcpy((char*)gpsrsP, "AT^IPCALL=0,\"CARD\",\"CARD\"\r\n");
			
			set_timer0(10000, 1);

		}
		else{
			memcpy((char*)gpsrsP, "AT+CGREG?\r\n", 11);//检查GPRS网络注册情况。建议该命令空闲时，循环发送，了解模块注册网络状态
//			gprsErrorTimes++;
			set_timer0(1000, 1);
		}
		gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		break;
	case 9:
		if(gprsMode == Gmode_MC323){
			set_timer0(1000, 1);
			strcpy(gpsrsP, "AT+CNMI=1,1,0,2,0\r\n");//设置短信上报
			gprs_snd(strlen(gpsrsP), gpsrsP);
		}
		else if (gprsMode == Gmode_CDMA2000){//模式设置移到下面9
			memcpy((char*)gpsrsP, "AT^IOMODE=1,3\r\n",15);	//mode 2字符格式，可发特殊字符，IPSEND 以长度结束，满字符发送,但是后台如果发送的数据里面有0d0A,它就认为完了，这个不能用，所以要用mode3
			gprs_snd(15, gpsrsP);			//错误修改 原来是17，有可能使此命令失效。
			set_timer0(1000, 1);
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRSLinkProcess++;
			break;
		}
		else{
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "at+cgatt=1\r\n", 12);//GTM900-A模块默认是不附着GPRS网络的，因此如果需要使用GPRS功能，必须手动设置附着
			gprs_snd(12, gpsrsP);
		}
		gprsErrorTimes++;
		break;
	case 10:
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
		}
		else if (gprsMode == Gmode_CDMA2000){
			GPRSLinkProcess = 20;
		}
		else if(gprsMode == Gmode_ALL4G){	//case 8: //配置PDP 场景相关参数			
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes+=5;
			
			switch (ugISMI_type)
			{
			case ChinaMobile:		//AT+QICSGP=1,1,"CMNET","","",0
				strcpy((char*)gpsrsP, "AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n");		//场景（配置表）1， CMNET
				//display(4, 0, "移动网络配置    ", 0);
				break;
				
			case ChinaUnicom:	//AT+QICSGP=1,1,"UNINET","","",1
				strcpy((char*)gpsrsP, "AT+QICSGP=1,1,\"3gnet\",\"\",\"\",1\r\n");		//场景（配置表）1，UNINET(2g), 3gnet(3G/4G,确认), TD-LTE/wonet(4G?)
				//display(4, 0, "联通网络配置    ", 0);
				break;
				
			case ChinaTelecom:	
				//AT+QICSGP=1,1,"0","CARD","CARD",0			//AT+QICSGP=1,1,"CTNET","","",0     //AT+QICSGP=1,1,"CTLTE","","",0
				strcpy((char*)gpsrsP, "AT+QICSGP=1,1,\"CTLTE\",\"\",\"\",0\r\n");	//场景（配置表）1, ctnet(3g), CTLTE(4g)
				//display(4, 0, "电信网络配置    ", 0);
				break;
				
			default:
				break;
			}
			
			set_timer0(8000, 1);
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		}
		else{
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "AT+CGATT?\r\n", 11);
			gprs_snd(11, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 11:
		if(gprsMode == Gmode_MC323){
			set_timer0(1000, 1);
			strcpy(gpsrsP, "AT^HSMSSS=0,0,6,0\r\n");//设置短信参数，UNICODE编码
			gprs_snd(strlen(gpsrsP), gpsrsP);
			gprsErrorTimes++;
		}
		else if(gprsMode == Gmode_ALL4G){	// case 9:
			//激活PDP 场景1, 通过移动网获取IP		AT+QIACT=<contextID>      （context ID, range is 1-16）
// 			GprsSingleCheck();
// 			delayxs(1);
// 			sprintf((char*)buff, "%02d ", gprsSingle);
// 			display(0, 0, (char*)buff, 0);
			
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes++;
			set_timer0(150000, 1);		//150 seconds,determined by network
			
//			strcpy((char*)gpsrsP, "AT+QIACT=1\r\n");		//场景（配置表）1， CMNET
			memcpy(gpsrsP, "AT+QIACT=1\r\n", 12);
			gprs_snd(12/*strlen((char*)gpsrsP)*/, gpsrsP);
//			display(4, 0, "登录移动网络    ", 0);
		}
		else{
			gprsErrorTimes+=3;
			set_timer0(3000, 1);
			memcpy((char*)gpsrsP, "AT+CMGF=0\r\n", 11);//设置短消息格式命令 PDU 模式
			gprs_snd(11, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 12:
		if(gprsMode == Gmode_ALL4G){//case 10;激活PDP 场景1, 通过移动网获取IP，AT+QIACT?
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes+=5;
			set_timer0(3000, 1);
			
			strcpy((char*)gpsrsP, "AT+QIACT?\r\n");		//场景（配置表）1， CMNET
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
//			 display(4, 0, "查询登录状态    ", 0);
		}
		else{
			memcpy((char*)gpsrsP, "AT+CIMI\r\n", 9);//SIM卡序列号 查询IMSI 号
			set_timer0(5000, 1);
			//		strcpy((char*)&gpsrsP, "AT+CGPADDR=?\r\n");
			//		strcpy((char*)&gpsrsP, "AT+CGDCONT=?\r\n");
			gprs_snd(9, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 13:	
		if(gprsMode == Gmode_MC323){
			memcpy((char*)gpsrsP, "AT^HWVER\r\n", 10);//^HWVER:MC323aME1MC323M VER B
			gprs_snd(10, gpsrsP);
		}
		else if(gprsMode == Gmode_ALL4G){
			GPRSLinkProcess = 20;
		}
		else{
			memcpy((char*)gpsrsP, "AT+CGMM\r\n", 9);//GTM900B
			gprs_snd(9, gpsrsP);
		}
		gprsErrorTimes++;
		set_timer0(5000, 1);
		break;
	case 14:
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
		}
		else{
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "AT+CGSN\r\n", 9);//030CNE7A69005499 查询IMEI 号
			gprs_snd(9, gpsrsP);
			gprsErrorTimes++;
		}
		break;
	case 15:// 
		if(gprsMode == Gmode_MC323){
			strcpy((char*)gpsrsP, "AT^IPINIT=,\"card\",\"card\"\r\n");//中国电信CDMA 数据业务缺省用户名：card,密码：card
			gprs_snd(strlen(gpsrsP), gpsrsP);//
			set_timer0(40000, 1);
		}
		else{
			set_timer0(1, 1);
			GPRSLinkProcess++;
		}
		break;
	case 16:
		if(gprsMode == Gmode_MC323){
			gAPN_Style = 'N';//公网
			memcpy((char*)gpsrsP, "AT^IPINIT?\r\n", 12);//中国电信CDMA 数据业务缺省用户名：card,密码：card
			gprs_snd(12, gpsrsP);//
		}
		else{
			if(((gmissflag & 0xf0)==0x20)||((gmissflag & 0xf0)==0x10)){//登陆移动专用网				
				memset(APNnet, 0, 32);
				getDefaultAPN(APNnet, 1);

				strcpy(gpsrsP, "AT+CGDCONT=1,\"IP\",\"");
				strcat(gpsrsP, APNnet);
				strcat(gpsrsP, "\"\r\n");//配置apn
				gprs_snd(strlen(gpsrsP), gpsrsP);//
				gAPN_Style = 'Y';//专网
			}
			else{
// #ifdef BUS_PINGXIANG_
// 				memcpy((char*)gpsrsP, "AT+CGDCONT=1,\"IP\",\"PXGJ.JX\"\r\n", 29);//用的是专网
// 				gprs_snd(29, gpsrsP);//
// 				gAPN_Style = 'N';//公网
//#else
				memset(gpsrsP, 0, sizeof(gpsrsP));
//				memcpy((char*)gpsrsP, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", 27);//配置apn
				strcpy((char*)gpsrsP, "AT+CGDCONT=1,\"IP\",\"");
				memset(APNnet, 0, 32);
				getDefaultAPN(APNnet, 0);
				strcat((char*)gpsrsP, APNnet);
				strcat((char*)gpsrsP, "\"\r\n");
				gprs_snd(strlen((char*)gpsrsP), gpsrsP);
				gAPN_Style = 'N';//公网
//#endif
			}
		}
		set_timer0(20000, 1);
		gprsErrorTimes++;
		break;
	case 17:
		if(gprsMode == Gmode_MC323){
			set_timer0(5000, 1);
			memcpy((char*)gpsrsP, "AT^IPDATMODE=1\r\n", 16);//设置自动上报
			gprs_snd(16, gpsrsP);
			gprsErrorTimes++;
		}
		else if(gprsMode == Gmode_MG323){
			memcpy((char*)gpsrsP, "at^sics=0,conType,GPRS0\r\n", 25);//进入tcpip 功能 //进行ppp拨号
			gprs_snd(25, gpsrsP);
			set_timer0(30000, 1);
			gprsErrorTimes+=5;
		}
		else{
			memcpy((char*)gpsrsP, "AT%ETCPIP\r\n", 11);//进入tcpip 功能 //进行ppp拨号
			gprs_snd(11, gpsrsP);
			set_timer0(30000, 1);
			gprsErrorTimes+=5;
		}
		break;
	case 18:
		if(gprsMode == Gmode_MC323){
			GPRSLinkProcess++;
		}
		else if(gprsMode == Gmode_MG323){
			memset(gpsrsP, 0, sizeof(gpsrsP));
// 			memcpy((char*)gpsrsP, "at^sics=0,apn,CMNET\r\n", 21);//
// 			memcpy((char*)gpsrsP, "at^sics=0,apn,CMNET\r\n", 21);//
			strcpy((char*)gpsrsP, "at^sics=0,apn,");
			memset(APNnet, 0, 32);
			getDefaultAPN(APNnet, 0);
			strcat((char*)gpsrsP, APNnet);
			strcat((char*)gpsrsP, "\r\n");
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
			set_timer0(15000, 1);
		}
		else{
			memcpy((char*)gpsrsP, "at%etcpip?\r\n", 12);//检查获得本地IP地址，以及DNS服务器的地址
			gprs_snd(12, gpsrsP);
			set_timer0(15000, 1);
		}
		gprsErrorTimes+=2;
		break;
 	case 19:
		if(gprsMode == Gmode_MG323){
			set_timer0(5000, 1);
 			strcpy(gpsrsP, "at^siss=0,srvType,socket\r\n");//设置Internet 服务
			gprs_snd(strlen(gpsrsP), gpsrsP);
		}
		else if(gprsMode == Gmode_ALL4G){//关闭PDP 场景1,  AT+QIDEACT=<contextID>      （context ID, range is 1-16）
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 50);
			gprsErrorTimes+=5;
			set_timer0(3000, 1);
			
			strcpy((char*)gpsrsP, "AT+QIDEACT=1\r\n");		//场景（配置表）1， CMNET
			gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		}
		else{
			GPRSLinkProcess++;
		}
// 		set_timer0(5000, 1);
// 		strcpy((char*)gpsrsP, "AT%IOMODE=,2\r\n");//设置多链接模式
// 		gprs_snd(strlen((char*)gpsrsP), gpsrsP);
		break;
	case 39:
		if(gprsMode == Gmode_MG323){
			set_timer0(15000, 1);
			strcpy(gpsrsP, "at^siss=0,conID,0\r\n");//设置Internet 服务
			gprs_snd(strlen(gpsrsP), gpsrsP);
		}
		else{
			GPRSLinkProcess = 40;
		}
		break;
	case 40:
// 		if(gprsMode == Gmode_MG323){
// 			set_timer0(1500, 1);
// //			strcpy(gpsrsP, "AT+CGPADDR=1\r\n");// 显示conProfileId 为0 的连接Profile 的信息  取不到。。^SICI: 0,0,0,"0.0.0.0"
// 			strcpy(gpsrsP, "AT^SICI?\r\n");
// 			gprs_snd(strlen(gpsrsP), gpsrsP);
// 		}
// 		else{
			GPRSLinkProcess = 20;
//		}
		break;
	case 20:
// 		if((memcmp(irda_rxBuf, "\"0.0.0.0\"", 8) != 0)&&(irda_rxBuf[0] == '\"')){
// 			irda_rxBuf[16] = 0;
// 			miniDispstr(6, 0, (char*)irda_rxBuf, DIS_CENTER);
// 		}
		gTcpIpbusy = TcpIPFree;
		if(gprsMode == Gmode_OTHER){
			GPRSLinkProcess = TCPSTARTSTAT;
			isNetOK[0] = 3;
		}
		else{
			ClientIP = getCpuIPaddr();
			ClientPort = getCpuPort();
			memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
			if(gprsMode == Gmode_CDMA2000){
				sprintf((char*)gpsrsP, "AT^IPOPEN=1,1200,\"%u.%u.%u.%u\",%u,0\r\n\x0", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else if(gprsMode == Gmode_MC323){
				sprintf((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"%u.%u.%u.%u\",%u,1000\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else if(gprsMode == Gmode_MG323){
				sprintf((char*)gpsrsP, "at^siss=0,address,\"socktcp://%u.%u.%u.%u:%u\"\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else if(gprsMode == Gmode_ALL4G){	//Push模式：AT+QIOPEN=1,0,"TCP","61.190.19.174",8705,0,1   收到数据后命令+QIURC: “recv”,0,4\r\n 后返回数据。
												//Buffer模式AT+QIOPEN=1,0,"TCP","61.190.19.174",8705,0,0   收到数据命令后要发送AT+QIRD=0,1500收数据
				sprintf((char*)gpsrsP, "AT+QIOPEN=1,0,\"TCP\",\"%u.%u.%u.%u\",%u,0,1\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			else{
				sprintf((char*)gpsrsP, "AT%%IPOPEN=\"TCP\",\"%u.%u.%u.%u\",%u\r\n", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53],ClientPort);
			}
			set_timer0(30000, 1);
			gprsErrorTimes+=5;

#ifdef _debug_gprs
			debugstring("20-14:");
		//	debugdata((unsigned char *)&gpsrsP, ret, 1);
			debugstring(gpsrsP);
#endif
			gprs_snd(strlen(gpsrsP), gpsrsP);
//		set_timer0(1000, 0);//1S钟进一次
		}
		break;
	case 21:
#ifdef _phone_test_//通话测试，不连接
		break;
#endif
		gTcpIpbusy = TcpIPFree;
		if(gprsMode == Gmode_MG323){
			strcpy(gpsrsP, "at^siso=0\r\n");//打开指定的Internet 服务
			gprs_snd(strlen(gpsrsP), gpsrsP);
			set_timer0(30000, 1);
			gprsErrorTimes+=5;
		}
		else if (gprsMode == Gmode_CDMA2000){
			GPRSLinkProcess++;
		}
		else
			GPRSLinkProcess++;
		initTCPIPnode(); //重新边接后，清除发送缓存中的数据
		break;
	case 22:
		break;
	default:
		break;
	}
maingprs1:
 	t = get_timer0(0);
	if(t == 0){
		set_timer0(1000, 0);//10ms钟进一次

 		TaskRecWrite();//处理要发送的数据
// 		if(omissDly++ > 10){
// 			omissDly = 0;	
		in_decgM_Senddll();
		in_dispGprsProcess();



//		Sendpincount++;
		gSendGLogin++;


			if(gSendOverTime > 0){
				gSendOverTime--;

			}

			if(tcpipPacket.TTL > 0)
				tcpipPacket.TTL--;
			if(tcpipPacket.TTL > 60)
				tcpipPacket.TTL = 0;
			if(gErrortimes[1]>0){
				gErrortimes[1]--;
				if((gErrortimes[1] == 0) && (gErrorFlag&ERROR_BUS_CONNECT)){//如果是连公交出错,重启
#ifdef _debug_gprs
					debugstring("~!!!~~\r\n");
#endif
					gErrorFlag &= (~ERROR_BUS_CONNECT);			//pfquan  要不然一直会进来，任务变成00 00 
					if(gprsMode == Gmode_ALL4G)
						GPRSLinkProcess = 0;
					else
						GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
				}
			}
			if(gErrortimes[0]>0){
				gErrortimes[0]--;
			}
			if(gBusDownPro.Miss_Time>0)
				gBusDownPro.Miss_Time--;
			//去掉吧，这个如果后台删除发布了，不能老发这个了
// 			if (gBusDownPro.Miss_Time == 0)
// 			{
// 				gBusDownPro.Miss_Time = GPRS_Login_dly;
// 				if(   ( memcmp(gBusVerInfo.newbusBLKVer, "\x00\x00", 2)!=0)		)
// 				{//GPRS已经登陆，可以发数据
// 					if (memcmp(gBusDownPro.Miss_ver,"\x00\x00",2) != 0)
// 					{
// 						ret  = Build_GJ_DPro((unsigned char *)gpsrsP);
// #ifdef _debug_gprs
// 						debugstring("send Pro agin\r\n");
// #endif
// 						gprs_send_data(1, ret, (unsigned char *)gpsrsP);//发送应答
// 					}
// 				}
// 			}

			
			if(gBusblkDownPro.Miss_time>0)
				gBusblkDownPro.Miss_time--;

			if ((gBusblkDownPro.Miss_time == 0))
			{
				gBusblkDownPro.Miss_time = GPRS_Login_dly;
				if(   ( memcmp(gBusVerInfo.newbusBLKVer, "\x00\x00", 2)!=0)		)
				{//GPRS已经登陆，可以发数据
					if (memcmp(gBusblkDownPro.Miss_ver,"\x00\x00",2) != 0)
					{
					ret  = Buid_GJ_DBLK((unsigned char *)gpsrsP);
#ifdef _debug_gprs
					debugstring("send DBLK agin\r\n");
#endif
					gprs_send_data(1, ret, (unsigned char *)gpsrsP);//发送应答
					}
				}
			}
			if(gTcpIpbusydly > 0){
		
				gTcpIpbusydly--;
				if (gTcpIpbusydly == 0)
				{
#ifdef _debug_gprs
					debugstring("超时--free--\r\n");
#endif
					if(gTcpIpbusy == TcpIPBusy){//应该有数据发送
						gGPRS_data_style.ucSendstyle  = GPRS_SISW;		//去发送数据吧
					}
					else{
 						gTcpIpbusy = TcpIPFree;
 						gGPRS_data_style.ucSendstyle = 0;
 						gGPRS_data_style.ucReadStyle = 0;

						if(gprsMode == Gmode_ALL4G){
							GPRSLinkProcess = 19;
						}
					}

				}
			}

			in_sendGPs();//不为0，已经发送一条GPS信息

			if(gprsMode == Gmode_MG323){
				if((GPRSLinkProcess == TCPSTARTSTAT)||(GPRSLinkProcess == 0xA0)){//GPRS已经登陆，可以发数据
					gprs_send_data(1,0,(unsigned char*)gpsrsP);//处理缓存中是否还有数据发送。每秒钟查一次？
				}
			}
//		}
 	}
maingprs2:
	mainGprs();//处理缓冲中的GPRS数据
}

//如果没有GPRS，调用这个函数据，主要是用到1秒钟计数功能
void main_Onsec(void)
{
	unsigned int t;

 	t = get_timer0(0);
	if(t == 0){
		set_timer0(1000, 0);//1S钟进一		
		in_dispGprsProcess();
		in_decgM_Senddll();
		in_sendGPs();//不为0，已经发送一条GPS信息

 	}
}



int getgprslibVer(void)		//获取库版本号
{
	return SOFT_VAL;
}

