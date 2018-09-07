

#ifndef _E_GJGPRS_HEAD_FILE_
#define _E_GJGPRS_HEAD_FILE_


extern TCP_IP_PACKET1 tcpipPacket;

extern unsigned int Sendpincount;//重发响应或心跳包计数
extern unsigned char GPRSLinkProcess;

extern unsigned int ClientIP;
extern unsigned short ClientPort;
extern void GPRS_INIT(unsigned int band);

extern void GJRec_Send(void);
extern void find_G_new_mission(void);
//mode == 1 关闭链接1 mode==2:关闭链接2
extern void tcpipClose(unsigned char mode);

#endif

