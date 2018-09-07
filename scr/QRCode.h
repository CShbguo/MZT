

#ifndef _SLZR_QR_
#define _SLZR_QR_

#define TACLEN			5      //tac条数
#define TAC_DATA_LEN    29    //TAC数据长度


typedef struct
{
	unsigned char UIID[3];//账户发行方ID
    unsigned char UserID[8];//账户ID
	unsigned char DealType[1];			//00公交消费01地铁入闸02地铁出闸消费
	unsigned char EenTime[4];// 过期 时间戳，16进制字符串，高字节在前，大写
	
}st_QR_BLK;  //16 字节



typedef struct  
{
	unsigned char TagCode[2];			//标识码ASC  固定为 SL
	unsigned char CompanyCode[2];		//运营方代码BCD
	unsigned char AccountNo[10];		//帐号编码BCD
	unsigned char Accounttype;			//账户类型Hex
	unsigned char Counter[4];			//计数器Hex
	unsigned char ExpiredTime[7];		//过期时间BCD
	unsigned char MaxDealMoney[4];		//单笔交易最大金额Hex
	unsigned char Balance[4];			//余额Hex
	unsigned char TicketMAC[4];			//票据Mac Hex
	unsigned char TacVersion[4];		//密钥版本Hex 
	unsigned char TAC[4];				//TAC	Hex	
}Card_QRC_infor;


typedef struct
{
	unsigned char rCardDealNumb[4];		//设备流水号0-3
	unsigned char rSerialNum[4];		//序列号4-7
	unsigned char rCardType;			//卡类8
	unsigned char rDealType;			//交易类型9
	unsigned char rPublishNumb[10];		//帐号编号10-19
	unsigned char rAfterMoney[4];		//原额20-23
	unsigned char rDealMoney[3];		//交易金额24-26
	unsigned char rDealTime[7];			//交易时间YYYY/MM/DD/HH/MM/SS 27-33
	unsigned char rRemain;				//保留或M1卡司机卡号高位34
	//	unsigned char rTradCode[2];			//行业代码 用于区分相同密钥的不同城市或应用-------
	unsigned char rDriverNo[4];			//司机卡号35-38
	unsigned char rCityCode[2];		    //运营方代码39-40
	unsigned char rLineNo[2];			//线路号41-42
	
	unsigned char rCounter[4];			//计数器43-46
	unsigned char rTicketMac[4];		//票据Mac47-50
	unsigned char rTacVer[4];			//TAC版本51-54
	unsigned char rDeviceNo[4];			//设备号55-58
	unsigned char rTAC[4];				//校验码59-62
	unsigned char rReseverX;			//保留 00交通部标准卡 01交通部机构的公司卡（软算）63
	unsigned char rProVer[2];			//程序版本号64-65
	unsigned char rEwmData[62];			//二维码明文66-127
}DEALRECODE_QRC;

//其它的二维码记录格式
typedef struct{
	unsigned char rQrcrecHead[6];		//记录标识，0-5
	unsigned char rQrcrecHead2[2];		//记录标识，无线上传时不传记录标识6-7
	
	unsigned char rCardDealNumb[4];		//设备交易流水8-11
	unsigned char rDevSerial[4];		//设备唯一序列号 4  12-15
	unsigned char rCardType;			//卡类 16
	unsigned char rDealType;			//交易类型  17
	unsigned char ruserTimes[2];		//使用次数 司机号高位  [分段 全程时间] 如果是中控的记录和下面共18字节为卡片的身份证号 18-19
	unsigned char rAfterMoney[4];		//原额  20-23
	unsigned char rDealMoney[3];		//交易金额 24-26
	unsigned char rDealTime[7];			//交易时间YYYY/MM/DD/HH/MM/SS 27-33
	unsigned char rDriverNo[4];			//司机卡号 34-37
	unsigned char rDeviceNo[4];			//设备号 38-41
	unsigned char rLineNo[2];			//线路号 42-43
	unsigned char rProVer[2];			//程序版本号 44-45
	unsigned char rTicket[2];			//票价 46-47
	unsigned char rQRCdat[204];			//二维码信息。 48-251
	unsigned char rTAC[4];				//  252-255
}stOtherQRCrecord;


extern void QR_Code_Test_4500R(void);
extern void QR_Code_init_par(unsigned char Ccom);


unsigned char saveTACInfo(unsigned int flash_Addr);

#define QR_BLK_MAX 1000
extern unsigned int QR_BLK_Ver;	//羊城通二维码黑名单的 版本
// extern unsigned int gGprs_Check_QR_BLK;	// 15分钟左右查询一次黑名单 
unsigned char save_QR_blkVer(unsigned int a);
unsigned int get_QR_blkVer(void);

//1 黑名单  0 正常  uiid  mode 0开机调用  1,正常调用
unsigned char find_BLK_QR(unsigned char *UIID,unsigned char *UserID ,unsigned char mode);
int write_BLK_QR(unsigned char *filehead,unsigned char *wbuf,unsigned int datlen);

//void show_QR_Ver(void);
//取 版本 
unsigned char LoadfileVer(unsigned int flash_Addr,unsigned char *outver);

unsigned char saveSPKInfo(unsigned int flash_Addr);

void show_QR_Ver(void);
//菜单调用，避免每次都设置
void set_QR_para(void);

#endif




