
#ifndef _ICON_H
#define _ICON_H
static const unsigned char icon[8][16]={//这个是英文版的图标
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x70,0x00,0x30,0x00,0x50},
	{0x00,0x78,0x00,0x00,0x00,0xCC,0x00,0x70,0x00,0x86,0x01,0x70,0x00,0x33,0x03,0x20},
	{0x80,0x79,0x06,0x30,0xC0,0xE0,0x0C,0x70,0x60,0xCE,0x19,0x00,0x30,0x9F,0x33,0x70},
	{0x98,0x3F,0x67,0x40,0xCC,0x71,0xCE,0x40,0xE6,0xE0,0x9C,0x71,0x73,0xCE,0x39,0x03},
	{0x73,0x9C,0x39,0x73,0xE6,0x38,0x9C,0x41,0xCC,0x71,0xCE,0x40,0x98,0xE3,0x67,0x00},
	{0x30,0xC7,0x33,0x70,0x60,0xCE,0x19,0x70,0xC0,0x1C,0x0C,0x00,0x80,0x39,0x06,0x60},
	{0x00,0x33,0x03,0x10,0x00,0x86,0x01,0x60,0x00,0xCC,0x00,0x40,0x00,0x78,0x00,0x40},
	{0x00,0x30,0x00,0x70,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x70}
};
static const unsigned char ant_mask[9][8]={
				{ 0x18,0x24,0x66,0x99,0x99,0x66,0x24,0x18},	// ""=00
				{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80},	// ""=00
				{ 0x00,0x00,0x00,0x00,0x00,0x00,0x40,0xC0},	// ""=00
				{ 0x00,0x00,0x00,0x00,0x00,0x20,0x60,0xE0},	// ""=00
				{ 0x00,0x00,0x00,0x00,0x10,0x30,0x70,0xF0},	// ""=00
				{ 0x00,0x00,0x00,0x08,0x18,0x38,0x78,0xF8},	// ""=00
				{ 0x00,0x00,0x04,0x0C,0x1C,0x3C,0x7C,0xFC},	// ""=00
				{ 0x00,0x02,0x06,0x0E,0x1E,0x3E,0x7E,0xFE},	// ""=00
				{ 0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF}	// ""=00
};

static const unsigned char signal8[5][8]={
				{0x18,0x24,0x66,0x99,0x99,0x66,0x24,0x18},/*"未命名文件",0*/
				{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"未命名文件",0*/
				{0xC0,0x00,0xF0,0x00,0x00,0x00,0x00,0x00},/*"未命名文件",0*/
				{0xC0,0x00,0xF0,0x00,0xFC,0x00,0x00,0x00},/*"未命名文件",0*/
				{0xC0,0x00,0xF0,0x00,0xFC,0x00,0xFF,0x00},/*"未命名文件",0*/
};

static const unsigned char signal16[8][16]={//gprs信号
				{0x01,0x03,0xFF,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"无信号",0*/
				{0x01,0x03,0xFF,0x83,0xC1,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"一格信号",0*/	
				{0x01,0x03,0xFF,0x83,0xC1,0xE0,0x00,0xF0,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"两个",0*/
				{0x01,0x03,0xFF,0x83,0xC1,0xE0,0x00,0xF0,0xF8,0x00,0xFC,0xFC,0x00,0x00,0x00,0x00},/*"三个",0*/
				{0x01,0x03,0xFF,0x83,0xC1,0xE0,0x00,0xF0,0xF8,0x00,0xFC,0xFE,0x00,0xFF,0xFF,0x00},/*"四格信号",0*/
				{0x01,0x03,0xFF,0xC3,0xC1,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFE,0xFE,0xFF},/*"满格",0*/
				{0x81,0x03,0xFF,0xC3,0xC1,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFE,0xFF,0xFF},/*"未命名文件",0*/
				{0x81,0x03,0xFF,0xC3,0xC1,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFE,0xFF,0xFF},/*"未命名文件",0*/
};
#define	BLCD_Status_Reset	    0x10
#define	BLCD_Status_DispOff	    0x20
#define	BLCD_Status_Busy	    0x80
#define BLCD_Cmd_DispOn		    0x3f
#define BLCD_Cmd_DispOff	    0x3e
#define BLCD_Cmd_DispStartRow	0xc0	//must or the Row value.
#define BLCD_Cmd_DispPage	    0xb0	//b8 must or the Page value.
#define BLCD_Cmd_DispCol	    0x40	//must or the column value.

#define LCDColAddrH				0x10
#define LCDColAddrL				0x00

#define	BLCD_MaxPage		 8	//There are eight page in BLCD BLCD_Dispbin ram.
#define	BLCD_MaxColumnEvyPage	64	//Maxmum column of BLCD.
#define BLCD_MaxCharEvyPage		16//8
#define BLCD_ColumnEvyChar	   8

extern void wcomd(unsigned char cdat);
extern void wdata(unsigned char cdat);
extern void antDispstr2(unsigned char x, unsigned char y, unsigned char antNum, unsigned char mode);
extern void ShowGprsSinal(unsigned char x, unsigned char y,unsigned char Singnal);//GPRS信号显示
#endif
