#ifndef _GMOBILE_HEAD_FILE_
#define _GMOBILE_HEAD_FILE_
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "Gszct.h"
#if 0
extern stcomtime SysTime;
extern unsigned char DriveCardNo[4];
//extern stLcdCmd gLcdCmd;

extern unsigned char gmissOldflag;//�ϸ�����������־ 0x00 ��½�ƶ�,ǩ��.
extern unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.
//0x01 ���ز���
//0x02 ������������
//0x03 �������ؽ��֪ͨ
//0x04 ������
//0x05 �����ͽ���
//0xF0 ��½������̨
//0xF1 ���غ�����
stMobileParameter gMobileParameter;//�ƶ����Ĳ���
stMobilStyle gMobilStyle;//ǩ��״̬����������������Ϣ��KEK ���� ��������״̬����������
extern unsigned short gSendOverTime;//���ͺ���Ӧ��ʱ������
unsigned int gMobilPOSNS;//�յ����ն�ϵͳ���ٺţ�POS��ˮ�ţ�
unsigned int gMobilREC_Start;//��¼ָ��ͷ
unsigned int gMobilREC_End;//�ƶ�ָ��β

//unsigned int gMobilSendPoint;//Ҫ���͵ļ�¼ָ��
//unsigned int gMobilSendPointBAK;//����
//unsigned int gMTrad63MSG1BAK;
//unsigned int gMTrad63MSG2BAK;

//unsigned int gMTrad63MSG1;//�����ܴ���
//unsigned int gMTrad63MSG2;//�����ܽ��
stdownProgPara downProgPara;//���������������

unsigned short g8583ACK;//8083����Ӧ����

unsigned char gmobileProVer[14];//�ƶ�����汾���ӳ��������ȡ��

unsigned char gmissNotTrad;//�˱���ָʾ�Ƿ���������,���Ѿ�û��ʹ��

unsigned int gMobilConnectErr;//���ƶ�ƽ̨��������0û���󣬷���Ǵ���

unsigned int gSendSeril;//�ϴ��ƶ�����ʱ�����кš�
unsigned int gmobileBakSeril;//�������ĸ��ٺ�
unsigned char gseflag;

stMobilSend mobilsendStyle;
unsigned char gM_Senddll;//��ʱ����

unsigned char gmobileErrTimes;//�ƶ����Ӵ��������,����Ӧ��.

unsigned char gmobileHeadFlag;//�ƶ���Ӧ���Ƿ��ȷ��ܳ���
#endif
#endif
