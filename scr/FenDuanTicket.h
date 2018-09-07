


extern void initFDpara_ALL(void);
extern void FengDuan_disp(void);
extern void FengDuan_BUS_card(void);
extern void FengDuan_main(void);

//---------------------------------
extern void FD_Check_devMode(void);
extern int FDParaCheck(void);

#define FDMODE_GPS 0xA0
#define FDMODE_NORMAL 0x5E
extern unsigned char gFDMode;	//标识分段收费中的设备类型，0xA1为前门机（具有GPS，GPRS），0xA2为后门机。通过检测GPS模块确认。


#define FDParChange_Line 0x01			//线路
#define FDParChange_Device (0x01<<1)	//设备号
#define FDParChange_Piaojia (0x01<<2)	//票价
#define FDParChange_Program (0x01<<4)	//前门程序
extern void FDParChange_QM(unsigned char mode);	//参数是否变化，0无变化，其它有变化

//分段程序中设置的标志，如果标志=0x5E为后门机，标志为0xA0为前门机，如要标志都无效内通过硬件来判断。
//在CPUINFO中设置设备的类型。此设置优先判断
extern void SETCpuFDMode(void);



