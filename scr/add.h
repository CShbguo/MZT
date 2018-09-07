
#ifndef _ADD_H_
#define _ADD_H_
#include "..\include\include.h"
#if 1


#define FLASH1_START_O  FLASH_M_REC_START
#define FLASH2_START1_O FLASH_M_REC_END


//��¼����˵��������յ�ǰ�������˿ɴ���
#define FLASH_RCLR_O 0x68000
//��¼������˵����¼���� �����֮�������ռ�
#define FLASH_MAX_O  0x65000
// 1024K�ռ������ǰ���ݣ��ɴ�1��
#define	LEN_EEPROM_O 0x70000
// ������������ ��Get_Record_point ��ʹ�ã�
#define FLASH_REC_MAX_O 800

#define RECORD_LEN_O  512
#endif
#if 0


#define FLASH1_START_O 0
#define FLASH2_START1_O 0x200000
#define FLASH2_START2END_O 0x300000
#define FLASH2_BAKR_LEN_O 0x100000

//��¼����˵��������յ�ǰ�������� 0x80000 �ɴ�8192��
#define FLASH_RCLR_O 0x80000
//��¼������˵����¼���� �����֮����7616���ռ�
#define FLASH_MAX_O  0xF7000
// 1024K�ռ������ǰ���ݣ��ɴ�16384��
#define	LEN_EEPROM_O 0x100000
// ������������ ��Get_Record_point ��ʹ�ã�
#define FLASH_REC_MAX_O 16000

#define RECORD_LEN_O  512
#endif
extern int save_ODA_infor(unsigned char mode, unsigned char *re_infor);
extern unsigned int cal_space_O(void);
#endif


