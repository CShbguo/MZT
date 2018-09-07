#ifndef ISO8583__H_
#define ISO8583__H_

#define MAXBUFFERLEN     2048 /*����ISO_data�ṹ����Ļ�����*/
//struct  ISO_8583 /* ISO 8583 Message Structure Definitions */
//{              
//    int				len;   /* data element max length */
//    unsigned char   type;  /* bit0,bit1���� */
//						   /*bit2:Ϊ0ʱ�����,Ϊ1ʱ�Ҷ���*/
//						   /* bit3:Ϊ0ʱBCD��,Ϊ1ʱASC��*/
//							/*type:0,1,2,3����*/
//    unsigned char   flag;  /* length field length: 0--FIX_LEN�� 1--LLVAR_LEN�� 2--LLLVAR_LEN��*/
//};

struct data_element_flag 
{
	short bitf;			/*field map if 1 true  0 false*/
    short len;			/*field length*/
    int   dbuf_addr;	/*field data's start address*/
};


// typedef struct  
// {
//     struct  data_element_flag f[128];
//     short   off;
//     char    dbuf[MAXBUFFERLEN];
//     char    message_id[10];
// } ISO_data;

void  DES(unsigned char *key,unsigned char *s_text,unsigned char *d_text);
void _DES(unsigned char *key,unsigned char *s_text,unsigned char *d_text);

#endif

