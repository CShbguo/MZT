
#ifndef NO_MOBILE_SERVISE
extern unsigned char G24Profile(void);
extern unsigned char G24DealCard(void);
extern void G24MainCard(void);
extern unsigned char G24PsamInitialize(void);
extern void setG24Reader(void);
extern void G24ReadStyle(void);
extern void Check24Card(void);
extern void Rest24Card(void);
extern unsigned char GetMobilStarBAK(unsigned char *stradd);
extern void writeMobilStarBAK(unsigned char *stradd);
extern unsigned int g24GDisFlash;//24G卡界面刷新

extern unsigned char gPsamErrorFlag;//PSAM是否出错。
extern unsigned char g24PurError;
extern unsigned char gG24Ver;//读头版本标识

extern unsigned char g24ComFlag;//串口接收标志
extern unsigned char g24ComRBuf[500];
extern unsigned int g24ComRindex;
extern unsigned short com1Rlen;
#elif defined _CPU_24G_
extern unsigned int g24GDisFlash;//24G卡界面刷新

extern unsigned char gPsamErrorFlag;//PSAM是否出错。
extern unsigned char g24PurError;
extern unsigned char gG24Ver;//读头版本标识

extern unsigned char g24ComFlag;//串口接收标志
extern unsigned char g24ComRBuf[500];
extern unsigned int g24ComRindex;
extern unsigned short com1Rlen;
#endif
