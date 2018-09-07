
#include "string.h"
#include "stdio.h"
#include "../include/include.h"
#include "szct.h"

#ifdef _BlueTooth_COM

extern unsigned char restore_flag;

/////////////////////////////bluetooth///////////////////////////////////////
/*
//蓝牙模块采用HC-08，主机、从机透传通信时,9600bps 波特率以下每个数据包请不要超出500 个字节
//通信波特率建议设置为9600，最大一次发送不超过500 字节 
//发送间隔规则：		发500 字节，需间隔时间1000(ms)，否则容易丢码
//				        发300 字节,   间隔时间600(ms)
//				        发100 字节,   间隔时间160(ms)
						发80  字节,   间隔时间120(ms)
						发60  字节,   间隔时间100(ms)
						发20  字节,   间隔时间 20(ms)
*/
unsigned char BlueRevdat;
unsigned char BlueRevFlag=0;

//初始化串口及参数
void bluetoothINIT(void)
{
	BlueRevFlag = 0;
	com_init(_BlueTooth_COM, 9600);
	COM_it_enable(_BlueTooth_COM);
}

void bluetooth_hand(unsigned char value)
{
	com_snd(COM1, 1, (unsigned char*)&value);
		
	BlueRevdat = value;
	BlueRevFlag = 1;
// //  	if (BlueIndex>1)
// //  	{
// 		//if((value == 0x0a)&&(BlueRevBuf[BlueIndex-1]==0x0d)){		//一个包收完
// 		if(value == 0x0a){		//一个包收完, 手机APP测试换行输出是“0X0A”，只能是一个0X0A
// 			BlueIndex++;
// 			BlueRevBuf[BlueIndex] = '\0';
// 			BlueRevBuf[BlueIndex+1] = '\0';//在接收的数据后加上个结束符
// 			memcpy(BlueRevBuf_dis, BlueRevBuf, BlueIndex+1);
// 			BlueRevFlag =1;
//  			BlueIndex = 0;	
// 			return;
// 		}
// //	}
// 	BlueIndex++;
// 	if(BlueIndex >= 1000)
// 		BlueIndex = 0;
// 		
}

/*
void Bluetooth_LCDisplay(void)
{
	
	display(0,0,"Bluetooth显示",DIS_CENTER|DIS_Cls);
	
	BlueRevFlag=0;
	BlueIndex =0;

	while(1)
	{
		if (BlueRevFlag)				//接收进行中
		{
			BlueRevFlag=0;
			//debugstring((char*)BlueRevBuf_dis);
			//com_snd(COM4,3,"B:");
			com_snd(COM1,strlen((char*)BlueRevBuf_dis),BlueRevBuf_dis);
			com_snd(COM6,strlen((char*)BlueRevBuf_dis),BlueRevBuf_dis);//com_snd(COM4,strlen((char*)BlueRevBuf_dis),BlueRevBuf_dis);
			//com_snd(COM4,2,"\r\n");
			
			display(0,0,"Bluetooth显示",DIS_CENTER|DIS_Cls);
			if(strlen((char*)BlueRevBuf_dis) > 560)
			{	BlueRevBuf_dis[560]='\0';
			BlueRevBuf_dis[561]='\0';
			}
			miniDispstr(2,0,(char *)BlueRevBuf_dis,0);	
			
		}
		
		if (getkey(1)==KEY_ESC)
			break;
		else
			delayxms(50);
	}
	
	
}
*/

//COM6:	  //串口5转换到J9 ,  TTL
void Bluetooth_main(void)
{
	char strbuf0[100];
	unsigned int revIndex=0, length, temp1, crup;
	unsigned char revBuf[100];
	unsigned char k;
	
	display(0,0,"蓝牙测试",DIS_CENTER|DIS_Cls);
	display(2,0,"键UP 数据发送",DIS_CENTER);

	restore_flag = 3;
	
	crup = Get_Record_point((unsigned char *)&temp1, 0);
	
	length=0;
	length=(crup-temp1);

	strcpy(strbuf0, "slzr-working: bluetooth test!");
	sprintf(strbuf0+strlen(strbuf0), "Record Length:%d\r\n", (length/RECORD_LEN));
	
	bluetoothINIT();

	set_timer0(500, 2);
	while(1){
		clr_wdt();
		if(BlueRevFlag != 0){
			BlueRevFlag = 0;
			set_timer0(500, 2);
			revBuf[revIndex++] = BlueRevdat;
			if((BlueRevdat == 0x0A)||(revIndex > 64)){//收完一行
				if(revIndex > 1){//只发回车时不显示。
					revBuf[revIndex-1] = 0;
					display(0,0,"Bluetooth测试",DIS_CENTER|DIS_Cls);
					display(2,0,"键UP 数据发送",DIS_CENTER);
					debugstring("S1:");
					debugdata(revBuf, revIndex, 1);
					display(6, 0, (char*)revBuf, 0);
				}
				revIndex = 0;
			}
		
		}

		if((get_timer0(2) == 0)&&(revIndex > 0)){
			revBuf[revIndex] = 0;
			display(0,0,"Bluetooth测试",DIS_CENTER|DIS_Cls);
			display(2,0,"键UP 数据发送",DIS_CENTER);
			debugstring("S2:");
			debugdata(revBuf, revIndex, 1);
			display(6, 0, (char*)revBuf, 0);
			revIndex = 0;
			set_timer0(500, 2);
		}

		if((get_timer0(2) % 10)==0)
		{
			k = getkey(1);
			switch (k)
			{
			case KEY_UP:	
				com_snd(_BlueTooth_COM, strlen(strbuf0), (unsigned char*)strbuf0);
				break;
			case KEY_ESC:
				COM_it_disable(_BlueTooth_COM);
				return;
			default:
				break;
			}

		}
	}
}

#endif



