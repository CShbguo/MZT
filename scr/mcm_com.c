
#include <string.h>
#include "stdio.h"
#include "include.h"
#include "public.h"

#define CMD_DEV_ID			0x02
#define CMD_DEV_VERSION		0x04

#define CMD_CONNECT			0x20
#define CMD_DISCONNECT		0x22
#define CMD_SET_TIME		0x24
#define CMD_DEV_INIT		0x26
#define CMD_TANSF_RECORD	0x28
#define CMD_TANSF_BACK_RECORD	0x2a
#define CMD_CLEAR_RECORD	0x2c
#define CMD_DOWN_BLACK		0x2e
#define CMD_DOWN_PARFILE	0x30
#define CMD_R_PARFILE		0x32
#define CMD_R_BLACKFILE		0x34
#define CMD_CHECK_STATUS	0x36
#define CMD_RESET_DEV		0xe0
#define CMD_SELEF_CHECK		0xe3

#define CMD_DEV_RST			0xE0
#define CMD_DEV_CHECKUP		0xE2

#define CMD_DOWNLOAD_KEY	0xF0
#define CMD_DESTRUCT_KEY	0xF2
#define CMD_INIT_SYS		0xF4
#define CMD_ACCESS_PSW		0xF6
#define CMD_KEY_STATUS		0xF8

//
#define CMD_DOWNLOAD_FONT		0xF1
#define CMD_SET_NAME			0xF3
#define CMD_SET_UID				0xF5
#define CMD_SET_PROHIBIT_DEEP	0xF7
#define CMD_SET_PROHIBIT_DELAY	0xF9
// ==================================================================
// response code
#define EXE_CMD_OK			0x00
#define ERR_UNKNOWN_CMD		0xE0
#define ERR_DISCONNECT		0xE1

// ==================================================================
// public variable declare
#define MCM_PREAMBLE		0x00
#define MCM_STX1			0x00
#define MCM_STX2			0xFF
#define MCM_POSTAMBLE		0x00

#define P_preamble	0
#define P_stx		1
#define P_nLen		3
#define P_saddr		5
#define P_taddr		10
#define P_snq		15
#define P_cmd		16
#define P_nPlen		17
#define P_nPno		19
#define P_inf		21
//__packed 
//typedef struct _mcm_com{
//	unsigned char preamble;				// 0
//	unsigned char stx[2];				// 1
//	unsigned short nLen;					// 3 (low first)
//	unsigned char saddr[5];				// 5
//	unsigned char taddr[5];				// 10
//	unsigned char snq;					// 15
//	unsigned char cmd;					// 16
//	unsigned short nPlen;					// 17
//	unsigned short nPno;					// 19
//	unsigned char inf[300];			// 21 (include lrc & postamble)
//	//unsigned char lrc;
//	//unsigned char postamble;
//}mcm_com;
#define COM1 1
#define MEM_COM COM1
// ==================================================================
// global variable declare

extern int com_rev(unsigned char com, unsigned char *RevBuf);
extern void com_snd(unsigned char com, unsigned int uiLen, unsigned char *SendBuf);

static unsigned char com_status;

//struct mcm_com mcm_send_buf, mcm_receive_buf;
// ==================================================================
// private function declare
// ==================================================================
// function: initialize mcm rs232 communication

void init_mcm_com(void)
{
	com_status = 0;
}


// ==================================================================
// function: send respone
// input   : 0---ACK
//           1---NACK

void SendByte(unsigned char port, unsigned char value)
{
	unsigned char buf[2];
	buf[0] = value;
	com_snd(port, 1, buf);
}
void mcm_send_ACK(unsigned char ucAck)
{
	SendByte(MEM_COM, 0x00);
	SendByte(MEM_COM, 0x00);
	SendByte(MEM_COM, 0xFF);
	if (ucAck == 0)
	{//ACK
		SendByte(MEM_COM, 0x00);
		SendByte(MEM_COM, 0xFF);
	}
	else
	{//NACK
		SendByte(MEM_COM, 0xFF);
		SendByte(MEM_COM, 0x00);
	}
	SendByte(MEM_COM, 0x00);
}

// ==================================================================
// function: rs232 respone
void mcm_send_package(unsigned char* pp)
{
	unsigned int nSend;
	unsigned char *pChar;
	unsigned char Xsum;
	unsigned char buf[20];
	unsigned int i;

	buf[0] = 0;
	buf[1] = 0x00;
	buf[2] = 0xFF;

	memcpy(buf+3, pp+P_nLen, 2);
	nSend = (unsigned int)pp[P_nLen]<<8;
	nSend += pp[P_nLen+1];
	// get checksum
//	mcm_send_buf.inf[nSend-(7+16)] = mcm_checksum(&mcm_send_buf);
	// postamble
//	mcm_send_buf.inf[nSend-(7+15)] = 0x00;

	Xsum = 0;
	// send
	com_snd(MEM_COM, 5, buf);		//send package head
	for(i = 0; i<5; i++)
		Xsum += pp[P_saddr+i];
	com_snd(MEM_COM, 5, pp+P_saddr);	//send saddr
	for(i = 0; i<5; i++)
		Xsum += pp[P_taddr+i];
	com_snd(MEM_COM, 5, pp+P_taddr);	//send taddr
	buf[0] = pp[P_snq];
	buf[1] = pp[P_cmd];
	memcpy(buf+2, pp+P_nPlen, 2);
	memcpy(buf+4, pp+P_nPno, 2);
	for(i=0; i<6; i++)
		Xsum += buf[i];
	com_snd(MEM_COM, 6, buf);
	nSend -= 16;
	pChar = pp+P_inf;
	do
	{
		com_snd(MEM_COM, 1, pChar);
		Xsum += *pChar++;
	}while(--nSend);
	Xsum = (~Xsum) + 1;
	com_snd(MEM_COM, 1, (unsigned char*)&Xsum);	//checksum
	buf[0] = 0;
	com_snd(MEM_COM, 1, buf);	//postamble
}

//void mcm_send_respone(unsigned char status, mcm_com *pp)
//{
//	pp->nLen = 0x11;
//	memcpy(pp->taddr, pp->saddr, 5);
//	pp->saddr[0] = 0x01;
////	memcpy(&mcm_send_buf.saddr[1], ext_para.DeveiveID, 4);
//	memcpy(&pp->saddr[1], "\x12\x34\x56\x78", 4);
////	mcm_send_buf.snq = mcm_receive_buf.snq;
//	pp->cmd += 1;
//	pp->nPlen = 1;
//	pp->nPno = 1;
//	pp->inf[0] = status;
//	mcm_send_package(pp);
//}
// ==================================================================
// function: mcm protocol communication with host
//00 00 FF 00 10 06 FF FF FF FF 01 FF FF FF FF 01 20 00 01 00 01 DE 00 
//00 00 FF 00 10 06 FF FF FF FF 01 FF FF FF FF 01 22 00 01 00 01 DC 00 

#define LCD_TYPE 0
#define LED_TYPE 1
#define ERASE64KBYTESECTOR 3
#define MASK64K 0x10000
extern void flashwritefont(unsigned int addr, unsigned char *writebuf, unsigned int length);
extern void flasherasefont(unsigned char mode, unsigned int addr);
extern void miniDispstr(unsigned char x, unsigned char y, char *lcd_string, unsigned char mode);
extern void LED_cls(void);
extern void LED_display(unsigned char Line,char *buffer,unsigned char mode);

#define FLASH_ZK_START 0x3C0000		//字库空间:256K
#define FLASH_ZK_END 0x400000
unsigned char mcm_rs232(void)
{
	unsigned char ret;
//	unsigned char *pTmp;
	unsigned int nCnt, nLen;
	unsigned char revflag=0;
//	mcm_com mcm_receive_buf;//*mcm_send_buf, mcm_receive_buf;
	unsigned int addr=0;
	unsigned char responchar=0;
	unsigned char endflag=0;
	
	unsigned char revbuf[4000];

//	if (RecveiceByte(port_no, &ret) == 0) return 0;
	if(com_rev(MEM_COM, &ret) != 0) return 0;
	if (ret != 0) return 0;

//	memset(revbuf, 0, 20);
	nLen = 0;
	nCnt = 1;
//	SetExTimer(0, 200/TIMER_TICK);
	set_timer(200);
	while(1)
	{
		if (get_timer() == 0){
			revflag = 1;
			break;
		}

		if(com_rev(MEM_COM, &ret)==0)
		{
			set_timer(200);
			revbuf[nCnt] = ret;
			if(nCnt <399)
				nCnt++;
			if (nCnt == 2){
				if (ret != 0x00){
					revflag = 1;
					break;
				}
			}
			if (nCnt == 3){
				if (ret != 0xFF){
					revflag = 1;
					break;
				}
			}
			if (nCnt == 5){
				nLen = (unsigned int)revbuf[P_nLen];//mcm_receive_buf.nLen + 7;
				nLen <<=8;
				nLen += revbuf[P_nLen+1];
			}
			if ((nCnt>4)&&(nCnt == nLen+7)){
				revflag = 0;
				break;
			}
		}
	}
	if(revflag == 0)
	{
		mcm_send_ACK(0);
//		mcm_rs232_cmd(mcm_receive_buf);
		if ( (com_status == 0) && (revbuf[P_cmd] != CMD_CONNECT) )
		{
			revbuf[P_cmd] = CMD_CONNECT;
			revbuf[P_nLen] = 0x00;
			revbuf[P_nLen+1] = 0x11;
//			mcm_respone_address(1, 1);
			memcpy(revbuf+P_taddr, revbuf+P_saddr, 5);
			revbuf[P_saddr] = 0x01;
			revbuf[P_cmd] += 1;
			revbuf[P_nPlen] = 0;
			revbuf[P_nPlen+1] = 1;
			revbuf[P_nPno] = 0;
			revbuf[P_nPno+1] = 1;
			memset(revbuf+P_saddr+1, 0xFF, 4);
			revbuf[P_inf] = ERR_DISCONNECT;
			mcm_send_package(revbuf);
			return ERR_DISCONNECT;
		}
		switch(revbuf[P_cmd])
		{
			case CMD_CONNECT:
			{
				com_status = 1;
				responchar = EXE_CMD_OK;
//				mcm_send_respone(EXE_CMD_OK, (mcm_com*)&mcm_receive_buf);
				break;
			}
			case CMD_DISCONNECT:
			{
				com_status = 0;
	//			init_felica_key(0);
				responchar = EXE_CMD_OK;
//				mcm_send_respone(EXE_CMD_OK, (mcm_com*)&mcm_receive_buf);
	//			DelayXms(20);
	//			SoftReset();
				break;
			}
			case CMD_DOWNLOAD_KEY:
			case CMD_DESTRUCT_KEY:
	//			mcm_send_respone(port_no, process_key((struct key_parameter *)mcm_receive_buf.inf ));
				break;
			case CMD_DEV_VERSION:
				break;
			case CMD_INIT_SYS:
	//			process_key((struct key_parameter *)mcm_receive_buf.inf);
				break;
			case CMD_ACCESS_PSW:
	//			mcm_send_psw();
				break;
			case CMD_KEY_STATUS:
//				mcm_send_key_status();
				break;
			case CMD_DEV_INIT:
				break;
			case CMD_DEV_ID:
			case CMD_DEV_RST:
			case CMD_DEV_CHECKUP:
				break;
			case CMD_DOWNLOAD_FONT:
			{
				if(memcmp(revbuf+P_nPno, revbuf+P_nPlen, 2)==0)
					endflag = 1;
	//			if ( FlhSysWrite((unsigned long)(mcm_receive_buf.nPno-1)*256U, mcm_receive_buf.inf, mcm_receive_buf.nLen-16) )
				addr = (unsigned int)revbuf[P_nPno] << 8;
				addr += revbuf[P_nPno+1];
				addr = (addr -1)*256;
				addr += FLASH_ZK_START;
				if((addr%MASK64K)==0){
					flasherasefont(ERASE64KBYTESECTOR, addr);
				}
				flashwritefont(addr, revbuf+P_inf, nLen-16);//256
				responchar = EXE_CMD_OK;
				sprintf((char*)revbuf, "%d||%d", (addr-FLASH_ZK_START)/256, nLen);
				miniDispstr(6, 5, (char*)revbuf, 0);
				break;
			}
			case CMD_SET_NAME:
	//			SetCompanyName(mcm_receive_buf.inf);
				debugdata(revbuf+P_inf, 10, 1);
			case CMD_SET_UID:
	//			SetDeviceUID(mcm_receive_buf.inf);
			case CMD_SET_TIME:
	//			if( set_ds5250rtc((struct com_time*)mcm_receive_buf.inf ))
			case CMD_SET_PROHIBIT_DEEP:
	//			SetProhibitDeep(mcm_receive_buf.inf[0]);
			case CMD_SET_PROHIBIT_DELAY:
	//			SetProhibitDelay( *(int*)mcm_receive_buf.inf );
				responchar = EXE_CMD_OK;
	//			mcm_send_respone(0, (mcm_com*)&mcm_receive_buf);
				break;
			default:
				responchar = ERR_UNKNOWN_CMD;
//				mcm_send_respone(ERR_UNKNOWN_CMD, (mcm_com*)&mcm_receive_buf);
//				return ERR_UNKNOWN_CMD;
				break;
		}
		revbuf[P_nLen] = 0;
		revbuf[P_nLen+1] = 0x11;
		memcpy(revbuf+P_taddr, revbuf+P_saddr, 5);
		revbuf[P_saddr] = 0x01;
	//	memcpy(&mcm_send_buf.saddr[1], ext_para.DeveiveID, 4);
		memcpy(revbuf+P_saddr+1, "\xff\xff\xff\xff", 4);
	//	mcm_send_buf.snq = mcm_receive_buf.snq;
		revbuf[P_cmd] += 1;
		revbuf[P_nPlen] = 0;
		revbuf[P_nPlen+1] = 1;
		revbuf[P_nPno] = 0;
		revbuf[P_nPno+1] = 1;
		revbuf[P_inf] = responchar;
		mcm_send_package(revbuf);
//		if((addr >= 0x3EF00+FLASH_ZK_START) && (responchar == EXE_CMD_OK))
		if(endflag == 1)
			return 0xee;	//下载完成
		else
			return responchar;
	}
	else
	{//time out
		mcm_send_ACK(1);
	}
	return 0;
}
//connect command
//00 00 FF 00 10   06 FF FF FF FF   01 FF FF FF FF   01   20 00 01 00 01   AA 00
