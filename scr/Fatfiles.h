
#ifndef	__FFFILE__H__
#define __FFFILE__H__

#include "StructDef.h"




//�����ļ�ϵͳ������ֻ��Ҫ����һ��
extern void checkFlashDisFAT(void);


int close_rfile_Wrecord(void);
int open_rfile_Wrecord(void);
int write_rfile_Wrecord(unsigned char *buf);

//������ʱ����ļ�¼2*4K ����д��his�ļ���
int check_tmp_record(void);
//���ļ���д��¼���ļ���¼λ��0:/curdatĿ¼�¡�д��¼�ļ���ʱ�䣺172ms��������265ms
//ֻ��Ҫд����ʷ�ļ���31���ļ�(�ļ���Ϊhis01-his31)���µ�һ���¸��Ǿɵġ���ǰ�ļ�ģʽ����.��һ��д���ļ�����������ʱ�䣬������ǵ�����򸲸ǣ��ǵ�������д��
//buffer:Ҫд��ļ�¼ָ��
//mode:д�뷽ʽ��0=д��ʷ�ļ���1=д��ǰ�ļ�,����curdat�ļ����¡�
extern int WriteRecordFAT(unsigned char *buffer, unsigned char mode);


unsigned char save_bit_tmprec_point(unsigned int *haddr);
unsigned char get_bit_tmprec_point(unsigned int *haddr);


//��ʽ��FLASH FAT�ļ�ϵͳ��������curdatĿ¼
extern unsigned char format_disk_RW(void);
extern  unsigned char get_flahFatfree( unsigned int *total, unsigned int*free );


extern int Read_oneHis(unsigned char *recrord);
//�����û����Ҫ�ɼ��ļ�¼
// =0 ����Ϣ���󣬲�����
// 1 Ҫ������Ϣ
extern int Check_Send_FAT_Rcord(stFAT_hisRec *hisinfor);
//�ҵ�ĳ���ļ���д������Ϣ
//1 �м�¼�ļ��� �������ļ��ܳ�
// 0 ��
extern int Check_hisfile(stFAT_hisRec *hisinfor);
extern int check_free_flash(unsigned char  capacity_M);
//����һ���Ժ���ʷ��¼��ϢҪ���ӣ����޳��ѷ�������ļ� ֱ�������� ��������ʱ��д�ɷǷ�
//0  ����ʼ�ļ�
//1  ��ƫ����
extern int SaveFAT_hisRecInfor(unsigned char mode);
// int add_download_file(char *fileName, unsigned char *dfiledata, unsigned int datlen);
// int Great_download_file(char *fileNameTmp);
//�����ļ���������
extern int cp_file(char *old_path,char *new_path);

// int open_rfile(void);
#endif





