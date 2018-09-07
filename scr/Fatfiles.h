
#ifndef	__FFFILE__H__
#define __FFFILE__H__

#include "StructDef.h"




//挂载文件系统。开机只需要调用一次
extern void checkFlashDisFAT(void);


int close_rfile_Wrecord(void);
int open_rfile_Wrecord(void);
int write_rfile_Wrecord(unsigned char *buf);

//查找临时区域的记录2*4K 并回写在his文件里
int check_tmp_record(void);
//向文件内写记录，文件记录位于0:/curdat目录下。写记录文件的时间：172ms，跨扇区265ms
//只需要写入历史文件，31个文件(文件名为his01-his31)，新的一个月覆盖旧的。当前文件模式不变.第一行写入文件建立的日期时间，如果不是当天的则覆盖，是当天的向后写。
//buffer:要写入的记录指针
//mode:写入方式，0=写历史文件；1=写当前文件,都在curdat文件夹下。
extern int WriteRecordFAT(unsigned char *buffer, unsigned char mode);


unsigned char save_bit_tmprec_point(unsigned int *haddr);
unsigned char get_bit_tmprec_point(unsigned int *haddr);


//格式化FLASH FAT文件系统。并建立curdat目录
extern unsigned char format_disk_RW(void);
extern  unsigned char get_flahFatfree( unsigned int *total, unsigned int*free );


extern int Read_oneHis(unsigned char *recrord);
//检测有没有需要采集的记录
// =0 有信息错误，不操作
// 1 要处理信息
extern int Check_Send_FAT_Rcord(stFAT_hisRec *hisinfor);
//找到某个文件并写发送信息
//1 有记录文件了 并返回文件总长
// 0 无
extern int Check_hisfile(stFAT_hisRec *hisinfor);
extern int check_free_flash(unsigned char  capacity_M);
//发送一条以后历史记录信息要增加，并剔除已发送完的文件 直到发送完 发送完后把时间写成非法
//0  改起始文件
//1  改偏移量
extern int SaveFAT_hisRecInfor(unsigned char mode);
// int add_download_file(char *fileName, unsigned char *dfiledata, unsigned int datlen);
// int Great_download_file(char *fileNameTmp);
//拷贝文件并重命名
extern int cp_file(char *old_path,char *new_path);

// int open_rfile(void);
#endif





