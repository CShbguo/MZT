#ifndef _CPUCARDJTB_H_
#define _CPUCARDJTB_H_

#define  ON_BUS			3	//钱包上车
#define	 OFF_BUS		4	//钱包下车
#define  COVER_BUS		8 //钱包补票
#define  ON_Nomal		6//普通消费

#define FeRC_Write	1
#define FeRC_Dlelt	2
#define FeRC_Check	3

#define NO_START		0x81//未启用
#define NO_Ech			0x83//不是互联卡
#define NO_APP			0x84//未启用
#define PursAgian		0x85//补票

#define LocakCard		0x86//本地卡		//市内卡
#define YiDiCard		0x87//异地卡		//省外卡
#define autoregionCard  0x88//自治区卡

#define ID_REC_HJY_REC		100			//灰交易记录类型
#define RECORD_JTB_LEN		128

#define WHITELEN		 1000
#define WHITE_SNO_LEN		4 
#define WTACKSUM 16000		//16块总数

//在车站上车必须是2 6  其他不允许上车
#define BUS_Star			(1<<0) //1 3 5 7刷发车卡 1 0XFE单数刷了发车	进站要清掉
#define BUS_InStation		(1<<1) //在车站了 2 5 6在车站  0XFD
#define BUS_ON				(1<<2) //有标识为 4 上车记录 0XFB
#define BUS_TES				(1<<7) //测试记录

#define BUS_CL_Star			0xFE //清掉发车卡
#define BUS_CL_InStation	0xFD//不在车站
#define BUS_CL_ON			0xFB//下车
#define BUS_CL_TES			0x7F//测试记录清除


typedef struct
{
	unsigned char cRand[4];				/* 随机数 */
	unsigned char cSnq[2];				/* 电子钱包交易序列号 */
	unsigned char cMoney[4];			/* 交易金额 */
	unsigned char cType;				/* 交易类型 */
	unsigned char cDateTime[7];			/* 交易日期时间 */
	unsigned char cKeyVer;				/* 密钥版本 */
	unsigned char cKeyEn;				/* 密钥标识 */
	unsigned char cPublisherSign[8];	/* 8卡应用序列号卡号后8字节*/
	unsigned char cKeyFactor[8];		/* 8发卡机构编码 */
	unsigned char cReaderMAC[12];		//读头MAC,用于PSAM卡校验读头是否合法
}pboc_sam_init_purchaseJTB;
typedef struct//有数值大小区分的 一律为小端模式
{
	//29以下是当次刷卡的数据
	unsigned char rPublishNumb[10];		//BCD发行卡流水号卡号
	unsigned char rCardType;			//卡类
	unsigned char rCardCand;			//卡型:卡类型  M11卡  2 CPU卡 11 现金
	unsigned char rSAMxulie[4];			//PSAM终端交易序列号 (如果是云平台 填 我们设备自动生成的流水)
	unsigned char rPSAMNO[6];			//PSAM卡号72  BCD
	unsigned char rDeviceNo[4];			//设备号我们 机器的设备号（车牌号）(HEX)
	unsigned char rDealType;			//交易类型	HEX
	unsigned char rPiaoJia[2];			//票价 大端
	//20
	unsigned char rMoneyDealSum[2];		//钱包累计交易次数    应用交易计数器 9F36HEX
	unsigned char rDealMoney[3];		//交易金额 HEX 9F02
	unsigned char rAfterMoney[4];		//余额23大端
	unsigned char rDealTime[7];			//交易时间YYYY/MM/DD/HH/MM/SS 22个字节 
	unsigned char rCPUTAC[4];			//CPU交易认证码 *交易CPU卡返回的认证码
	//20//
	unsigned char rDriverNo[4];			//司机号 大端
	unsigned char rLineNo[3];			//线路号 大端
	unsigned char rGPSzhandian;			//站点编号
	unsigned char rGPSfangxian;			//GPS方向 上行/下行
	unsigned char rFactorFlg[8];		//下车机构标识
	unsigned char rCityCode[2];			//城市代码
	unsigned char rHUIJIAOYI;			//灰交易 0正常 1不正常 63
	//43//记录文件的上笔数据 (上车记录)
	unsigned char rDealTimeOLD[7];		//上车交易时间YYYY/MM/DD/HH/MM/SS* BCD
	unsigned char rPSAMNOOLD[6];		//上笔交易终端 BCD 就是PSAM卡号
	unsigned char rDriverNoOLD[4];		//上车司机号 
	unsigned char rLineNoOLD[3];		//上车线路号 
	unsigned char rGPSzhandianOld;		//上车站点编号
	unsigned char rCityCodeOLD[2];		//上车城市代码
	unsigned char rFactorFlgOLD[8];		//上车机构标识
	unsigned char rProviceOLD[3];		//上车的车牌 一个汉字+一个字母(暂时不用)
	unsigned char rDeviceNoOLD[4];		//上车车牌号(HEX)
	//......................................................
	unsigned char rProvice[3];			//下车的车牌 一个汉字+一个字母(暂时不用)
	unsigned char rZwyNum[4];			//下车站务员编号
	unsigned char rBusTC[2];			//趟次
	//以上一共104字节
	//
	//	QPBOC_RC	  rQpboc_Rc;			//QPBOC专有数据 70个字节
	unsigned char rBusFlag;				//不同状态的刷卡记录 
	unsigned char reasea[5];			//保留
	unsigned char rProVer[2];			//程序版本号
	unsigned char rTTTT[4];				//CRC校验四个字节AA
	
	
}DEALRECODE_JTB;

typedef struct{
	unsigned char falg[4];//有效标识
	unsigned char RCE[128];//记录
	unsigned char CRC[4];	//CRC
}FeUnKnowREC;
/* 记录ID标识 */
// 01公交
// 02 轨道交通
// 03 旅游
// 04公共自行车
// 05停车收费
// 06出租车
// 07长途客运	
// 08轮渡
// 09城际铁路
// 10高速公路
typedef struct
{
	unsigned char RcID;					/* BCD记录ID标识 */
	unsigned char RcLen;				/* 	记录长度 固定1E*/
	unsigned char LockFlag;				/* 锁定标志 固定0x00*/
	unsigned char Qtype;				/* BCD拓展类型1-99,1、成人月票2学生优惠......*/
	unsigned char AppAid[8];			/* 拓展应用AID */
	unsigned char YHtype;				/*BCD 拓展优惠类型 0 金额优惠  1次数优惠 */
	unsigned char StarData[4];			/* BCD拓展应用启用日期 */
	unsigned char EndData[4];			/* BCD拓展应结束日期 */
	unsigned char YearMoney[3];			//hex拓展应用年费
	unsigned char ResearX[8];			//预留
	
}stJTBA6;		//交通部钱包的06文件结构      32byte
typedef struct
{
	unsigned char Qtype;				/*BCD 拓展类型1-99,1、成人月票2学生优惠......*/
	unsigned char Ctype;				/* BCD记次类型 0 钱包计次 1文件计次*/
	unsigned char Ttype;				/* BCD时间类型 0 日  1 月*/
	unsigned char TimeCycle;			/* HEX时间类型 */
	unsigned char RateTimes[2];			/* hex定额次数  */
	unsigned char ResearX[10];			//预留
	
}stJTBC6;		//交通部次数文件的06文件结构      16e

typedef struct
{
	unsigned char RcID;					/* 记录ID标识 与记录号一致 */
	unsigned char RcLen;				/* 	记录长度 固定0x0c*/
	unsigned char LockFlag;				/* 锁定标志 固定0x00*/
	unsigned char StarData[4];			/* 启用日期 */
	unsigned char EndData[4];			/* 结束日期 */
	unsigned char Times[2];				//剩余次数
	unsigned char ResearX[2];			//预留	
}stJTBC7;		//交通部次数文件的07文件结构      15e

typedef struct
{
	unsigned char cPublisherSign[8];	/* 发卡机构标识 */
	unsigned char cAppType;				/* 应用类型标识 02 其他不正常 */
	unsigned char cStarFlag;			/* 发卡机构应用版本 *启用标识*/
	unsigned char cApp_sn[10];			/* 应用序列号*/
	unsigned char cIssueDate[4];		/* 应用启用日期 */
	unsigned char cValidDate[4];		/* 应用有效日期 */
	unsigned char cFCI[2];				/* 发卡机构自定义FCI数据 */
}stJTB15;		//交通部95文件结构      30byte


typedef struct
{
	unsigned char cCardType;			/* 卡类型标识 */
	unsigned char cStuffcode	;		/*本行职工标识*/
	unsigned char cName[20]	;			//持卡人姓名
	unsigned char cCredent[32];			/* 证件号 */
	unsigned char cCretype;				//证件类型
}stJTB16;		//交通部96文件结构     55byte
typedef struct
{
	unsigned char intercode[4];			/* 国际代码 */
	unsigned char provcode[2];			/*省级代码*/
	unsigned char Citycode[2]	;		/*城市代码*/
	unsigned char car_type[2];			//互通卡种  00非互通卡 01 互通卡
	unsigned char card_catalog;			//卡种类型
	unsigned char reser[49];			//预留
}stJTB17;		//交通部97文件:管理信息文件  60byte

typedef struct
{
	unsigned char rEPsearl[2];			/* EP联机或者脱机交易序列号 */
	unsigned char rOverdraft[3];		/*透支限额*/
	unsigned char rPursPrice[4];		//交易金额
	unsigned char rPursType	;			/*交易类型标识*/
	unsigned char rPSAMnum[6];			//终端机编号	
	unsigned char rPursTime[7];			//交易时间
}stJTB18;		//交通部98文件:  23

typedef struct//w文件号 2702   128字节
{	
	unsigned char rID[2];				//1-2交易标识  2702  公共汽车应用信息
	unsigned char rlen;					//3记录长度
	unsigned char rOk;					//4有效标识  固定01
	unsigned char rpursFlag;			//5交易标识1 采用分是分段扣费/复合消费    2 预授权消费
	unsigned char rLock;				//6锁应用标识 0没有锁 1锁定
	unsigned char rPursSearl[8];		//7-14交易流水BCD
	unsigned char rPursState;			//15交易状态标识1	00：初始值 01：上车 02：下车
	unsigned char rCityCodeON[2];		//16-17上车城市代码
	unsigned char rFactoryOn[8];		//18-25上车机构代码
	unsigned char rYunYinCode[2];		//26-27-上车运行代码
	unsigned char rLineNum[2];			//28～29上车线路号	2	HEX（高字节在前）
	unsigned char rStationOn;			//30上车站点
	unsigned char rBusNum[8];			//31-38上车车辆号Ascii
	unsigned char rPsamOn[8];			//39-46上车终端号BCD
	unsigned char rPursTimeOn[7];		//47-53上车时间
	unsigned char rPriceMax[4];			//54～57	标注金额4用于逃票追缴 HEX（高字节在前）
	unsigned char rDirection;			//58方向
	unsigned char rCityCodeOff[2];		//59-60下车城市代码
	unsigned char rFactoryOff[8];		//61-68下车机构代码
	unsigned char rYunYinCodeOff[2];	//69-70下车运营代码HEX
	unsigned char rLineNumOff[2];		//71～72下车线路号	2	HEX（高字节在前）
	unsigned char rStationOff;			//73下车站点
	unsigned char rBusNumOff[8];		//74-81下车车辆号Ascii
	unsigned char rPsamOff[8];			//82-89下车终端
	unsigned char rPursTimeOff[7];		//90-96下车时间
	unsigned char rPrice[4];			//97-100交易金额
	unsigned char rMileage[2];			//101-102乘车里程 HEX
	//30								//103-128预留26

	unsigned char rCiytCodeY[2];		//103-104预留信息城市代码
//	unsigned char rlenY;				//105预留信息长度 
	//16
	unsigned char rDriverNum[4];		//106-19上车司机号
	unsigned char rCwuyuanNum[4];		//110-113 上车乘务员编号
	unsigned char rTangCi[2];			//114-115趟次
	unsigned char rTimeMax[2];			//116-117最长时间,单位 分钟
	unsigned char rName[10];			//119-128站点名称Ascii  五个汉字
	unsigned char rCH[2];				//118 预留
	
//	unsigned char rResear[8];			//预留信息内容
}stJTB1A;		//交通部1A变长记录文件结构     55byte

typedef struct//1E   48字节
{
	unsigned char eDealType;	//00初始值  01：EC入站/上车/进场等；02：EC出站/下车/出场等；03 EP入 04 EP出站/下车/出场等05：EC单次消费；   
								// 	06：EP单次消费；07：EC补票08：EP补票；09：EC充值；0A：EP充值；其他预留。						
	unsigned char eDeviceNum[8];//终端编号BCD
	unsigned char eTracode;		//10行业代码 01地铁 02公交 03城市水上客运 04出租汽车 05租赁汽车 06公共自行车 07停车收费 
								//08长途客运 09轮渡 0A城际高速铁路 0B民航 0C高速公路收费 0E市郊城铁（慢速） 0F～12 本规范预留
	unsigned char eLineNUm[4];	//11-14线路号 BCD
	unsigned char eRF[3];		//15-17RF
	unsigned char eDmoney[4];	//18-21交易金额 大端HEX
	unsigned char eYuE[4];		//22-25余额
	unsigned char eDealTime[7]; //26-32交易日期时间
	unsigned char eCityco[2];	//33-34受理方城市代码
	unsigned char eFactoryco[8];//35-42机构代码
	unsigned char eErrPriceMax[4];//43-46不完整交易最大金额
	unsigned char eTimeMax[2];	//47-48最长时间,单位 分钟大



	unsigned char N[6];			//预留
}stJTB1E;		//1E 48字节

typedef struct
{
	unsigned char kid;
	unsigned char PublisherSign[8];
	unsigned char ReceiverSign[8];
	unsigned char startDate[4];
	unsigned char endDate[4];
}sam_pubJTB;
extern stJTB1A	JTB_1A;//1A文件结构
extern stJTB15	jtb15file;//15
extern stJTB16	jtb16file;//16
extern stJTB17	jtb17file;//17
extern stJTB18	jtb18file;//18
extern stJTB1E	JTB_1E;//1A文件结构
extern pboc_sam_init_purchaseJTB stuSamInitPurchaseJTB;
extern unsigned char PsamNumJTB[6];
extern unsigned char getDafang_CPUKey(unsigned char *Sdata, unsigned char *Rkey);


extern unsigned char  operat_Black(void);


#endif
