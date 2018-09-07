#if 0
//mobile
extern stMobileParameter gMobileParameter;//移动中心参数
extern stMobilStyle gMobilStyle;//签到状态，包括其它任务信息，KEK 程序 参数下载状态，开机需检测
extern unsigned int gMobilPOSNS;//收单方终端系统跟踪号（POS流水号）
//0x01 下载参数
//0x02 程序下载请求
//0x03 程序下载结果通知
//0x04 批结算
//0x05 批上送交易
//0xF0 登陆公交后台
extern unsigned char gMACKEY_M[16];//解密的MAC KEY明文
extern unsigned short gSendOverTime;//发送后收应答超时计数器

extern unsigned int gMobilREC_Start;//记录指针头
extern unsigned int gMobilREC_End;//移动指针尾

// extern unsigned int gMobilSendPoint;//要发送的记录指针
// extern unsigned int gMobilSendPointBAK;//备份
extern unsigned short g8583ACK;//8083包的应答码
// extern unsigned int gMTrad63MSG1;//消费总次数
// extern unsigned int gMTrad63MSG2;//消费总金额

extern stdownProgPara downProgPara;//程序下载任务情况
extern unsigned char gmobileProVer[14];//移动程序版本，从程序存贮器取出
extern unsigned char gmissNotTrad;
extern unsigned int gMobilConnectErr;//连移动平台错误。如是0没错误，否刚是错误
extern unsigned int gSendSeril;//上传移动数据时的序列号。
extern unsigned int gmobileBakSeril;
extern unsigned char gseflag;
// extern unsigned int gMTrad63MSG1BAK;
// extern unsigned int gMTrad63MSG2BAK;

extern stMobilSend mobilsendStyle;
extern unsigned char gM_Senddll;//延时传输
extern unsigned char gmobileErrTimes;//移动连接错误次数据,包括应答.

extern void uploadMobileData(void);
extern void uploadSettleData(void);
extern unsigned int dealPiciNo(void);
extern void load_fe_SendSeril(void);
extern void save_FE_SendSeril(void);
extern void IF2TLVDeal(unsigned char *pakege);
#endif