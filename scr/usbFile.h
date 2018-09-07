#ifndef __USB_H__
#define __USB_H__

#define updata_pro 	 1
#define updata_hzk16 2
#define updata_hzk32 3
#define write_read   4

#define USH_USR_FS_INIT       0
#define USH_USR_FS_TEST   		1  
#define USH_USR_FS_FREE   		2 

extern char host_flag;	//主模式标志位
extern char function;    //USB测试功能选择
extern char AppState;  	//USB处于空闲状态 获取测试状态
extern char connect_sta; //UBS的连接状态

//文件操作函数





//#define USB_DEBUG 1  //打印USB相关调试信息的宏

/***********************************************
*HOST 模式主程序入口 所有的操作在这个函数数中执行
***********************************************/
extern unsigned char USH_User_App(void); 

/******写数据到U盘 如果文件存在，则打开文件，在文件尾添加数据，如果文件不存在则创建文件
*path:路径加文件名
*buff:数据存储区
*len：数据的长度
**************************************/
extern void usb_write(char *path,char *buff,unsigned int len,unsigned int *w_len);

/******读U盘中的数据  以只读的形式打开文件读取数据
*path:路径加文件名
*buff:数据存储区
*offset:文件指针的偏移量
*len：数据的长度
*返回值:实际读取到的数据长度
**************************************/
extern char usb_read(char *path,char *buff,unsigned int offset,unsigned int len);

/*************************
*挂载U盘到FAT文件系统
**************************/
extern unsigned char f_mount_usb(void);

/****************************************
*解挂U盘
******************************************/
extern unsigned char unf_mount_usb(void);

/**************************************
*获取U盘的总空间和剩余空间 U盘比较大 最好total free是unsigned int类型的
***************************************/
extern unsigned char get_usbfree( unsigned int *total, unsigned int*free );

/**********************************************
*判断U盘是否已连接
***********************************************/
extern unsigned char Isdevice_connected(void);

/***************************************************************************************************
*更新固件 参数必须是U盘下存放程序文件的文件夹名称，也可以不加盘符直接写文件夹名称Updata_pro("/BIN");
***************************************************************************************************/
extern char Updata_pro(char *bin_dir);


/******更新字库******************************
*参数：chdrive 是磁盘盘符 主要是区分遍历U盘还SPI_FLASH下的目录
*ziku_dir: 字库的路径  也就是字库所在的文件夹名称 Updata_Hzk( "0:/ZIKU16"); Updata_Hzk( "/ZIKU16");不加盘符也可以
*addr_start:字库空间的开始地址
*addr_end:字库空间的结束地址
*********************************************/
extern char Updata_Hzk(char *ziku_dir, unsigned int addr_start);

/********************************************************************
*按下返回按键则返回1，接收到1退出USB主模式U盘已连接的while(1)循环
*********************************************************************/
extern unsigned char esc_usb_key(void);//按键按下退出USB


/************************************************************
*下面对文件和目录操作的几条函数必须是在U盘挂载到FAT文件系统
*并且判断U盘已连接的情况下使用，也就是在USH_User_App(void);Host的入后函数中
******************************************************/

/******创建文件 如果文件存在，则打开文件，在文件尾添加数据，如果文件不存在则创建文件
*file_name:文件名
*buff:数据存储区
*len：数据的长度
**************************************//******创建文件 如果文件存在，则打开文件，在文件尾添加数据，如果文件不存在则创建文件
*file_name:文件名
*buff:数据存储区
*len：数据的长度
**************************************/
extern void makefile(char *file_name,char *buff,unsigned int len);

/*************************
*创建文件夹
*file_dir：文件夹名字
**************************/
extern void make_dir(char * file_dir);

/*************************
*创建黑名单文件夹
*blackman_name：黑名单文件夹名字
**************************/
extern void make_blackman_dir(char * blackman_dir);

/*********************************************
*创建文件夹
*bin_dir：程序文件夹名称
*ziku_dir:字库文件夹名称
*blackman_dir:黑名单文件夹名称
*********************************************/
extern void make_file_dir(char * bin_dir,char * ziku_dir,char * blackman_dir);//创建 程序、字库、黑名单文件夹

/***********************************
*写黑名单
*blackman_dir：黑名单文件夹名称，如果文件夹不存在则创建文件夹
*blackman_name：黑名单文件名称，比如blackman.txt
*buff:欲写入黑名单文件的数据缓存区
*len：欲写入的数据长度
************************************/
extern void write_blackman(char * blackman_dir,char *blackman_name,char *buff,unsigned int len);//写黑名单函数



/********************************
*初始化USB为Host模式
*********************************/
extern void usb_host_Inst(void);

/****************************************
*USB进程函数，初始化为USB的Host模式之后，
*这个函数必须放到一个while(1)循环中，否则
*无法检测USB设备的连接与断开连接
*****************************************/

extern void usbh_process(void);
/**********************************************************
*除能USB的Host模式，也就是关闭USB功能
*为了降低功耗和避免USB的干扰，每次退出USB功能时都要关闭USB
*********************************************************/
extern void disable_host(void);
extern void usb_readwrite_test(void);

/**********************以下是使用USB功能用户调用函数***********************/
extern void usb_init(void);//初始化USB
extern void USB_LoadFont(void);//更新字库
extern void USB_UpdataPRO(void);//升级固件
extern void USB_WriteRead(void);//U盘的读写测试


/***************************更新程序或者字库**********************
*调用次函数可以将U盘ZIKU16文件夹下的字库文件更新得到车载机
*真正对字库的升级操作在USB的Host模式入口函数USH_User_App()中可见
*bin_or_ziku: 
*           updata_pro    升级程序
*						updata_hzk14  升级14*14的字库
*           updata_hzk32  升级32*32的字库
****************************************************************/
extern void USB_Updata(unsigned char bin_or_ziku);	

extern void dis_messgebox(char *istr, int code, unsigned char row);

	
#endif
