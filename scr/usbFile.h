#ifndef __USB_H__
#define __USB_H__

#define updata_pro 	 1
#define updata_hzk16 2
#define updata_hzk32 3
#define write_read   4

#define USH_USR_FS_INIT       0
#define USH_USR_FS_TEST   		1  
#define USH_USR_FS_FREE   		2 

extern char host_flag;	//��ģʽ��־λ
extern char function;    //USB���Թ���ѡ��
extern char AppState;  	//USB���ڿ���״̬ ��ȡ����״̬
extern char connect_sta; //UBS������״̬

//�ļ���������





//#define USB_DEBUG 1  //��ӡUSB��ص�����Ϣ�ĺ�

/***********************************************
*HOST ģʽ��������� ���еĲ����������������ִ��
***********************************************/
extern unsigned char USH_User_App(void); 

/******д���ݵ�U�� ����ļ����ڣ�����ļ������ļ�β������ݣ�����ļ��������򴴽��ļ�
*path:·�����ļ���
*buff:���ݴ洢��
*len�����ݵĳ���
**************************************/
extern void usb_write(char *path,char *buff,unsigned int len,unsigned int *w_len);

/******��U���е�����  ��ֻ������ʽ���ļ���ȡ����
*path:·�����ļ���
*buff:���ݴ洢��
*offset:�ļ�ָ���ƫ����
*len�����ݵĳ���
*����ֵ:ʵ�ʶ�ȡ�������ݳ���
**************************************/
extern char usb_read(char *path,char *buff,unsigned int offset,unsigned int len);

/*************************
*����U�̵�FAT�ļ�ϵͳ
**************************/
extern unsigned char f_mount_usb(void);

/****************************************
*���U��
******************************************/
extern unsigned char unf_mount_usb(void);

/**************************************
*��ȡU�̵��ܿռ��ʣ��ռ� U�̱Ƚϴ� ���total free��unsigned int���͵�
***************************************/
extern unsigned char get_usbfree( unsigned int *total, unsigned int*free );

/**********************************************
*�ж�U���Ƿ�������
***********************************************/
extern unsigned char Isdevice_connected(void);

/***************************************************************************************************
*���¹̼� ����������U���´�ų����ļ����ļ������ƣ�Ҳ���Բ����̷�ֱ��д�ļ�������Updata_pro("/BIN");
***************************************************************************************************/
extern char Updata_pro(char *bin_dir);


/******�����ֿ�******************************
*������chdrive �Ǵ����̷� ��Ҫ�����ֱ���U�̻�SPI_FLASH�µ�Ŀ¼
*ziku_dir: �ֿ��·��  Ҳ�����ֿ����ڵ��ļ������� Updata_Hzk( "0:/ZIKU16"); Updata_Hzk( "/ZIKU16");�����̷�Ҳ����
*addr_start:�ֿ�ռ�Ŀ�ʼ��ַ
*addr_end:�ֿ�ռ�Ľ�����ַ
*********************************************/
extern char Updata_Hzk(char *ziku_dir, unsigned int addr_start);

/********************************************************************
*���·��ذ����򷵻�1�����յ�1�˳�USB��ģʽU�������ӵ�while(1)ѭ��
*********************************************************************/
extern unsigned char esc_usb_key(void);//���������˳�USB


/************************************************************
*������ļ���Ŀ¼�����ļ���������������U�̹��ص�FAT�ļ�ϵͳ
*�����ж�U�������ӵ������ʹ�ã�Ҳ������USH_User_App(void);Host���������
******************************************************/

/******�����ļ� ����ļ����ڣ�����ļ������ļ�β������ݣ�����ļ��������򴴽��ļ�
*file_name:�ļ���
*buff:���ݴ洢��
*len�����ݵĳ���
**************************************//******�����ļ� ����ļ����ڣ�����ļ������ļ�β������ݣ�����ļ��������򴴽��ļ�
*file_name:�ļ���
*buff:���ݴ洢��
*len�����ݵĳ���
**************************************/
extern void makefile(char *file_name,char *buff,unsigned int len);

/*************************
*�����ļ���
*file_dir���ļ�������
**************************/
extern void make_dir(char * file_dir);

/*************************
*�����������ļ���
*blackman_name���������ļ�������
**************************/
extern void make_blackman_dir(char * blackman_dir);

/*********************************************
*�����ļ���
*bin_dir�������ļ�������
*ziku_dir:�ֿ��ļ�������
*blackman_dir:�������ļ�������
*********************************************/
extern void make_file_dir(char * bin_dir,char * ziku_dir,char * blackman_dir);//���� �����ֿ⡢�������ļ���

/***********************************
*д������
*blackman_dir���������ļ������ƣ�����ļ��в������򴴽��ļ���
*blackman_name���������ļ����ƣ�����blackman.txt
*buff:��д��������ļ������ݻ�����
*len����д������ݳ���
************************************/
extern void write_blackman(char * blackman_dir,char *blackman_name,char *buff,unsigned int len);//д����������



/********************************
*��ʼ��USBΪHostģʽ
*********************************/
extern void usb_host_Inst(void);

/****************************************
*USB���̺�������ʼ��ΪUSB��Hostģʽ֮��
*�����������ŵ�һ��while(1)ѭ���У�����
*�޷����USB�豸��������Ͽ�����
*****************************************/

extern void usbh_process(void);
/**********************************************************
*����USB��Hostģʽ��Ҳ���ǹر�USB����
*Ϊ�˽��͹��ĺͱ���USB�ĸ��ţ�ÿ���˳�USB����ʱ��Ҫ�ر�USB
*********************************************************/
extern void disable_host(void);
extern void usb_readwrite_test(void);

/**********************������ʹ��USB�����û����ú���***********************/
extern void usb_init(void);//��ʼ��USB
extern void USB_LoadFont(void);//�����ֿ�
extern void USB_UpdataPRO(void);//�����̼�
extern void USB_WriteRead(void);//U�̵Ķ�д����


/***************************���³�������ֿ�**********************
*���ôκ������Խ�U��ZIKU16�ļ����µ��ֿ��ļ����µõ����ػ�
*�������ֿ������������USB��Hostģʽ��ں���USH_User_App()�пɼ�
*bin_or_ziku: 
*           updata_pro    ��������
*						updata_hzk14  ����14*14���ֿ�
*           updata_hzk32  ����32*32���ֿ�
****************************************************************/
extern void USB_Updata(unsigned char bin_or_ziku);	

extern void dis_messgebox(char *istr, int code, unsigned char row);

	
#endif
