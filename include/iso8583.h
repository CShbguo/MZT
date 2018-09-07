#ifndef ISO8583__H_
#define ISO8583__H_

#define MAXBUFFERLEN     2048 /*定义ISO_data结构中最长的缓冲区*/
//struct  ISO_8583 /* ISO 8583 Message Structure Definitions */
//{              
//    int				len;   /* data element max length */
//    unsigned char   type;  /* bit0,bit1暂留 */
//						   /*bit2:为0时左对齐,为1时右对齐*/
//						   /* bit3:为0时BCD码,为1时ASC码*/
//							/*type:0,1,2,3三种*/
//    unsigned char   flag;  /* length field length: 0--FIX_LEN型 1--LLVAR_LEN型 2--LLLVAR_LEN型*/
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

