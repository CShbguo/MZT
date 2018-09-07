
#ifndef _CPU_CARD_HEAD_FILE_
#define _CPU_CARD_HEAD_FILE_
extern void CPUMainCard(void);
extern unsigned char CPUPsamInitialize(void);
extern unsigned char gPsamErrorFlag;//PSAMÊÇ·ñ³ö´í¡£

extern unsigned char CpuCardSetMac2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuBlackMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuCardCheck(unsigned char PsamID,unsigned char *pMac, unsigned char *pData, unsigned char len);
#endif
