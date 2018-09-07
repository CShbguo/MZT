#include "Icon.h"
#include "string.h"
#include "Gszct.h"
#include "Main_City.h"
#ifdef GPRS_MG323_
extern void GprsSingleCheck(void);//���ǣУң��ź�
extern volatile stGPrsDataStyle gGPRS_data_style;
extern volatile unsigned char gTcpIpbusydly;//�ղ���������ʱ����
extern unsigned char gTcpIpbusy;//ģ���������ڷ����ݣ�ģ�鹤���Ƿ����Ȼ���Ӧ���ٷ����ݡ��ڵȻ�Ӧʱ���ܷ��������ݡ�
#endif
extern unsigned char GPRSLinkProcess;
extern unsigned char gprsSingle;//GPRS�ź�ǿ��
extern unsigned char gprsAnswer;//�ǣУң�ģ���Ӧ�ϣ˱�־
extern void gprs_snd(unsigned int len, char *dat);
extern void beep(unsigned char ucTimes, unsigned int ulBeepms, unsigned int ulNoms);
void DisIcon32(unsigned char x, unsigned char y,unsigned char *disbuf)//��ʾ16*16��ͼ��
{
	unsigned char i=0,k;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	
	//memcpy(disbuf,icon[0],128);

	for(k=0;k<4;k++)//16*16��ͼ��  ���Ĵ���ʾ
	{
	
		charcol = (y%16);
		charrow	= (x+4)%8;
	
		if(charcol>15)
		{
			charrow++;
			charcol = charcol - 16;
		}
	
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8����
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
void DisIcon16(unsigned char x, unsigned char y,unsigned char *disbuf)//��ʾ16*16��ͼ��
{
	unsigned char i=0, k;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	//memcpy(disbuf,icon[0],128);
	for(k=0;k<2;k++)//16*16��ͼ��  ���Ĵ���ʾ
	{
		
		charcol = (y%16);
		charrow	= (x+4)%8;
		
		if(charcol>15)
		{
			charrow++;
			charcol = charcol - 16;
		}
		
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8����
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
void DisIcon8(unsigned char x, unsigned char y,unsigned char *disbuf)//��ʾ8*8��ͼ��
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
		temp = charcol*8;//8*8����
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
void GprsSingleCheck2(void)//���ǣУң��ź�
{
	#ifdef GPRS_MG323_
	static char gscDly=0;

	char buffer[20];
	#endif
	if((GPRSLinkProcess <= 0x15||GPRSLinkProcess>0x2F)&&(GPRSLinkProcess!=0xA0)){//
		return;
	}
	//�������ض���  ��Ҫȥ��ѯ�ź�
// 	if((memcmp(gBusDownPro.Miss_ver,"\x00\x00",2)!=0)||(memcmp(gBusblkDownPro.Miss_ver,"\x00\x00",2)!=0)||(memcmp(gGPSDownLine.Miss_ver,"\x00\x00",2)!=0))
// 		return;
#ifdef GPRS_MG323_
	if(gscDly++ < 10)
		return;
	if(gTcpIpbusy == TcpIPBusy){//���ܷ��ͣ���ȴ�������
		//		debugstring("@");
		if(gTcpIpbusydly == 0){
			gTcpIpbusy = TcpIPFree;
		}
		return;
	}
	if ((gGPRS_data_style.ucReadStyle == GPRS_SISR)||(gGPRS_data_style.ucReadStyle == GPRS_SISRing)){//����ȡ���ݣ����ܷ�����
		return;
	}	

	if(((gprsAnswer == 'O')||(gprsAnswer == 'I'))){
		gscDly=0;
		strcpy(buffer, "AT+CSQ\r\n");//���GSM����ע�����
		gprs_snd(strlen(buffer), (char*)buffer);


	}
#else//������MG323  ��ʱ��û�� ������Ӧ�������źŵ� ��Ĭ�ϸ������
	gprsSingle=18;
#endif
}
void ShowGprsSinal(unsigned char x, unsigned char y,unsigned char Singnal)//��ʾ16*16��ͼ��
{
	unsigned char i=0;//,tab;
	unsigned char charcol,charrow;
	unsigned char temp;
	unsigned char disbuf[20];
	//memcpy(disbuf,icon[0],128);
//	for(k=0;k<2;k++)//16*16��ͼ��  ���Ĵ���ʾ
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
		temp = charrow*8;//8*8����
		wcomd(BLCD_Cmd_DispPage | charrow);
		temp = charcol*8;//8*8����
		wcomd(LCDColAddrH | ((temp>>4)&0x0f));
		wcomd(LCDColAddrL |(temp & 0x0f));
		for (i=0; i<16; i++)
		{		
			temp = disbuf[i];
			wdata(temp);
			//			debugdata((unsigned char*)&temp,1,0);
		}
		GprsSingleCheck2();//��ָ�����ź�
//		y++;
//	}
	
}

