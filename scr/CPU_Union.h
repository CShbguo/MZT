#ifndef _CPUCARDJSB_H_
#define _CPUCARDJSB_H_




typedef struct
{
	unsigned char rPublishNumb_H[4];	//CPU应用序列号前4字节
	unsigned char rPublishNumb_L[4];		//发行卡流水号
	unsigned char rDeviceNo[4];			//设备号
	unsigned char rCardType;			//卡类
	unsigned char rDealType;			//交易类型
	
	unsigned char city_code[2];			//城市代码
	unsigned char rAfterMoney[4];		//额
	unsigned char rDealMoney[3];		//交易金额
	unsigned char rDealTime[7];			//交易时间YYYY/MM/DD/HH/MM/SS
	unsigned char rKeyEn;				//算法标识
	unsigned char rdealtpye_JSB;			//建设部交易标识
	unsigned char rDriverNo[4];			//司机号
	unsigned char rPSAM_DealNumb[4];		//M1本机交易流水号 CPU终端交易序号
	unsigned char rLineNo[2];			//线路号
	unsigned char rST_Serial[4];		//stm 本身序列号
	unsigned char record_Flag[2];	//新记录标识：55AA
	unsigned char rPSAM_NO[6];		//psam卡号
	unsigned char rBussinessNo[2];		//商户编号
	unsigned char rCPUCard_DealNumb[2];	//CPU卡片交易序号
	unsigned char rProVer[2];			//程序版本
	unsigned char rTAC[4];				//校验码
}Recode_CPU_JSB;



typedef struct 
{
	unsigned char city_code[2];
	unsigned char card_serial[8];
	unsigned char ckeyEn;		//算法标识
	unsigned char deal_money[4];
	unsigned char deal_type;		//=6
	unsigned char psam_num[6];		//8050文件返回6-11
	unsigned char deal_serial[4];	//8070返回的1-4
	unsigned char card_deal_serial[2];
	unsigned char deal_time[7];
	unsigned char TAC[4];
}jsb_rec;






unsigned char CPUPsamInitialize_JSB(void);
unsigned char CPUcardType_JSB(void);
unsigned int load_white_list(void);

unsigned char CPUDealCard_JSB(unsigned char mode, unsigned char cool);

//新的记录格式  64 字节
void BuildRecorde_CPU_JSB(unsigned char delType, unsigned char *recBuf);
//显示64字节记录
unsigned char CPU_JSB_history(unsigned char *dat,unsigned int Index);

#endif
