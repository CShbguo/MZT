

#ifndef _E_GJGPRS_HEAD_FILE_
#define _E_GJGPRS_HEAD_FILE_


extern TCP_IP_PACKET1 tcpipPacket;

extern unsigned int Sendpincount;//�ط���Ӧ������������
extern unsigned char GPRSLinkProcess;

extern unsigned int ClientIP;
extern unsigned short ClientPort;
extern void GPRS_INIT(unsigned int band);

extern void GJRec_Send(void);
extern void find_G_new_mission(void);
//mode == 1 �ر�����1 mode==2:�ر�����2
extern void tcpipClose(unsigned char mode);

#endif

