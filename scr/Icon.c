#include "Icon.h"
#include "string.h"
#include "Gszct.h"
#include "Main_City.h"
#ifdef GPRS_MG323_
extern void GprsSingleCheck(void);//检测ＧＰＲＳ信号
extern volatile stGPrsDataStyle gGPRS_data_style;
extern volatile unsigned char gTcpIpbusydly;//收不到空闲延时处理
extern unsigned char gTcpIpbusy;//模块是正正在发数据，模块工作是发命令，然后回应后再发数据。在等回应时不能发其它数据。
#endif
extern unsigned char GPRSLinkProcess;
extern unsigned char gprsSingle;//GPRS信号强度
extern unsigned char gprsAnswer;//ＧＰＲＳ模块回应ＯＫ标志
extern void gprs_snd(unsigned int len, char *dat);
extern void beep(unsigned char ucTimes, unsigned int ulBeepms, unsigned int ulNoms);
void DisIcon32(unsigned char x, unsigned char y,unsigned char *disbuf)//显示16*16的图标
{
	unsigned char i=0,k;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	
	//memcpy(disbuf,icon[0],128);

	for(k=0;k<4;k++)//16*16的图标  分四次显示
	{
	
		charcol = (y%16);
		charrow	= (x+4)%8;
	
		if(charcol>15)
		{
			charrow++;
			charcol = charcol - 16;
		}
	
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8点阵
		wcomd(LCDColAddrH | ((temp>>4)&0x0f));
		wcomd(LCDColAddrL |(temp & 0x0f));
		for (i=0; i<32; i++)
		{		
			temp = disbuf[i*4+k];
			wdata(temp);
			//debugdata((unsigned char*)&temp,1,0);
		}
		x++;
	}

}
void DisIcon16(unsigned char x, unsigned char y,unsigned char *disbuf)//显示16*16的图标
{
	unsigned char i=0, k;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	//memcpy(disbuf,icon[0],128);
	for(k=0;k<2;k++)//16*16的图标  分四次显示
	{
		
		charcol = (y%16);
		charrow	= (x+4)%8;
		
		if(charcol>15)
		{
			charrow++;
			charcol = charcol - 16;
		}
		
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8点阵
		wcomd(LCDColAddrH | ((temp>>4)&0x0f));
		wcomd(LCDColAddrL |(temp & 0x0f));
		for (i=0; i<16; i++)
		{		
			temp = disbuf[i*2+k];
			wdata(temp);
//			debugdata((unsigned char*)&temp,1,0);
		}
		x++;
	}
	
}
void DisIcon8(unsigned char x, unsigned char y,unsigned char *disbuf)//显示8*8的图标
{
	unsigned char i=0;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	//memcpy(disbuf,icon[0],128);	
		charcol = (y%16);
		charrow	= (x+4)%8;
		
		if(charcol>15)
		{
			charrow++;
			charcol = charcol - 16;
		}
		
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8点阵
		wcomd(LCDColAddrH | ((temp>>4)&0x0f));
		wcomd(LCDColAddrL |(temp & 0x0f));
		for (i=0; i<8; i++)
		{		
			temp = disbuf[i];
			wdata(temp);
//			debugdata((unsigned char*)&temp,1,0);
		}
		x++;
}
void GprsSingleCheck2(void)//检测ＧＰＲＳ信号
{
	#ifdef GPRS_MG323_
	static char gscDly=0;

	char buffer[20];
	#endif
	if((GPRSLinkProcess <= 0x15||GPRSLinkProcess>0x2F)&&(GPRSLinkProcess!=0xA0)){//
		return;
	}
	//正在下载东西  不要去查询信号
// 	if((memcmp(gBusDownPro.Miss_ver,"\x00\x00",2)!=0)||(memcmp(gBusblkDownPro.Miss_ver,"\x00\x00",2)!=0)||(memcmp(gGPSDownLine.Miss_ver,"\x00\x00",2)!=0))
// 		return;
#ifdef GPRS_MG323_
	if(gscDly++ < 10)
		return;
	if(gTcpIpbusy == TcpIPBusy){//不能发送，需等待，保存
		//		debugstring("@");
		if(gTcpIpbusydly == 0){
			gTcpIpbusy = TcpIPFree;
		}
		return;
	}
	if ((gGPRS_data_style.ucReadStyle == GPRS_SISR)||(gGPRS_data_style.ucReadStyle == GPRS_SISRing)){//正在取数据，不能发数据
		return;
	}	

	if(((gprsAnswer == 'O')||(gprsAnswer == 'I'))){
		gscDly=0;
		strcpy(buffer, "AT+CSQ\r\n");//检查GSM网络注册情况
		gprs_snd(strlen(buffer), (char*)buffer);


	}
#else//可能是MG323  暂时还没做 到这里应该是有信号的 先默认给三格吧
	gprsSingle=18;
#endif
}
void ShowGprsSinal(unsigned char x, unsigned char y,unsigned char Singnal)//显示16*16的图标
{
	unsigned char i=0;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	unsigned char disbuf[20];
	//memcpy(disbuf,icon[0],128);
//	for(k=0;k<2;k++)//16*16的图标  分四次显示
//	{
		if(Singnal>31)
			Singnal=0;

		memcpy(disbuf,(unsigned char*)signal16[Singnal/6],16);
		charcol = (y%16);
	   // charrow	= (x+4%8);
		charrow=(x+4)%8;
		if(charcol>15)
		{
			charrow++;
			charcol = charcol - 16;
		}
		temp = charrow*8;//8*8点阵
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8点阵
		wcomd(LCDColAddrH | ((temp>>4)&0x0f));
		wcomd(LCDColAddrL |(temp & 0x0f));
		for (i=0; i<16; i++)
		{		
			temp = disbuf[i];
			wdata(temp);
			//			debugdata((unsigned char*)&temp,1,0);
		}
		GprsSingleCheck2();//发指令检测信号
//		y++;
//	}
	
}

