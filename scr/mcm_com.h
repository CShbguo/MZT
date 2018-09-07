#ifndef MCM_COM_H
#define MCM_COM_H

// ==================================================================
// command code

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

extern unsigned char com_status;

#define MCM_PREAMBLE		0x00
#define MCM_STX1			0x00
#define MCM_STX2			0xFF
#define MCM_POSTAMBLE		0x00

// ==================================================================
// public function declare

extern void init_mcm_com(void);
extern unsigned char mcm_rs232(void);
//unsigned char mcm_collect(uchar port_no);

extern unsigned char mcm_receive_cmd(unsigned char port_no);

extern void mcm_send_respone(unsigned char port_no, unsigned char ucAck);
extern void mcm_send_ACK(unsigned char port_no, unsigned char ucAck);
extern void mcm_send_package(unsigned char port_no);

extern void mcm_send_psw(unsigned char port_no);


#endif


