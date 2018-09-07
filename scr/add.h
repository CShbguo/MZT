
#ifndef _ADD_H_
#define _ADD_H_
#include "..\include\include.h"
#if 1


#define FLASH1_START_O  FLASH_M_REC_START
#define FLASH2_START1_O FLASH_M_REC_END


//记录到这说明可以清空当前数据区了可存条
#define FLASH_RCLR_O 0x68000
//记录到这条说明记录满了 和清除之间有条空间
#define FLASH_MAX_O  0x65000
// 1024K空间存贮当前数据，可存1条
#define	LEN_EEPROM_O 0x70000
// 最多存贮的条数 在Get_Record_point 中使用，
#define FLASH_REC_MAX_O 800

#define RECORD_LEN_O  512
#endif
#if 0


#define FLASH1_START_O 0
#define FLASH2_START1_O 0x200000
#define FLASH2_START2END_O 0x300000
#define FLASH2_BAKR_LEN_O 0x100000

//记录到这说明可以清空当前数据区了 0x80000 可存8192条
#define FLASH_RCLR_O 0x80000
//记录到这条说明记录满了 和清除之间有7616条空间
#define FLASH_MAX_O  0xF7000
// 1024K空间存贮当前数据，可存16384条
#define	LEN_EEPROM_O 0x100000
// 最多存贮的条数 在Get_Record_point 中使用，
#define FLASH_REC_MAX_O 16000

#define RECORD_LEN_O  512
#endif
extern int save_ODA_infor(unsigned char mode, unsigned char *re_infor);
extern unsigned int cal_space_O(void);
#endif


