#if 0
//mobile
extern stMobileParameter gMobileParameter;//�ƶ����Ĳ���
extern stMobilStyle gMobilStyle;//ǩ��״̬����������������Ϣ��KEK ���� ��������״̬����������
extern unsigned int gMobilPOSNS;//�յ����ն�ϵͳ���ٺţ�POS��ˮ�ţ�
//0x01 ���ز���
//0x02 ������������
//0x03 �������ؽ��֪ͨ
//0x04 ������
//0x05 �����ͽ���
//0xF0 ��½������̨
extern unsigned char gMACKEY_M[16];//���ܵ�MAC KEY����
extern unsigned short gSendOverTime;//���ͺ���Ӧ��ʱ������

extern unsigned int gMobilREC_Start;//��¼ָ��ͷ
extern unsigned int gMobilREC_End;//�ƶ�ָ��β

// extern unsigned int gMobilSendPoint;//Ҫ���͵ļ�¼ָ��
// extern unsigned int gMobilSendPointBAK;//����
extern unsigned short g8583ACK;//8083����Ӧ����
// extern unsigned int gMTrad63MSG1;//�����ܴ���
// extern unsigned int gMTrad63MSG2;//�����ܽ��

extern stdownProgPara downProgPara;//���������������
extern unsigned char gmobileProVer[14];//�ƶ�����汾���ӳ��������ȡ��
extern unsigned char gmissNotTrad;
extern unsigned int gMobilConnectErr;//���ƶ�ƽ̨��������0û���󣬷���Ǵ���
extern unsigned int gSendSeril;//�ϴ��ƶ�����ʱ�����кš�
extern unsigned int gmobileBakSeril;
extern unsigned char gseflag;
// extern unsigned int gMTrad63MSG1BAK;
// extern unsigned int gMTrad63MSG2BAK;

extern stMobilSend mobilsendStyle;
extern unsigned char gM_Senddll;//��ʱ����
extern unsigned char gmobileErrTimes;//�ƶ����Ӵ��������,����Ӧ��.

extern void uploadMobileData(void);
extern void uploadSettleData(void);
extern unsigned int dealPiciNo(void);
extern void load_fe_SendSeril(void);
extern void save_FE_SendSeril(void);
extern void IF2TLVDeal(unsigned char *pakege);
#endif