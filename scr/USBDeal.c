
//USB���ܺ���
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "..\fathead/ff.h"
#include "usbFile.h"

extern stcomtime SysTime;
extern stBusVerIfo gBusVerInfo;
extern pFistVary_1 pFistVary;
extern stPricetable NewPriceTab;

extern unsigned char GetDateTime(void);

#define HISIfoLen 26

//�������ļ�ϵͳ��
int _Fread(void *rbuf, UINT rlen, FIL*rfile)
{
	unsigned int reLn;
	FRESULT res;
	res = f_read(rfile, rbuf, rlen, &reLn);
	if(res != FR_OK)
		return -1;
	return reLn;
}

int _Fwrite(void *rbuf,UINT rlen,FIL*rfile)
{
	unsigned int reLn;
	FRESULT res;
	res = f_write(rfile, rbuf, rlen, &reLn);
	if(res != FR_OK)
		return -1;
	return reLn;
}

int _Access(const char *filn, int dd)
{
	FILINFO finf;
	FRESULT ret;
	ret = f_stat(filn, &finf);
	
//	MSG_LOG("access f_stat:%s:%d\r\n", filn, ret);
	return ret;
}


unsigned char keywait(unsigned char CKey, unsigned int dly50xms)
{
	unsigned char ret;
	unsigned char outdly=0;
	while(1){
		if(outdly++ > dly50xms)
			return 0;
		ret = getkey(1);
		if((ret==KEY_ESC)||((ret==CKey)&&(CKey!=0)))
			break;
		else if((ret != 0) && (CKey == 0))
			break;
		delayxms(50);
	}
//	debugdata(&ret, 1, 1);
	return ret;
}

#define USB_DIS_DEAD "U�̲ɼ�"
void getFATerror(char *ostr, unsigned char code)
{
	char EInfo[20][20]={ {"��������"},{"���̴���"},{"��ʼ������"},{"����û׼����"},{"�Ҳ����ļ�"},
						{"�Ҳ���·��"},{"�ļ�������"},{"�ܾ�����"},{"�ܾ�����"},{"�ļ���Ч"},
						{"�ļ�ֻ��"},{"������Ч"},{"������Ч"},{"���ļ�ϵͳ"},{"��������"},
						{"���ʳ�ʱ"},{"�ļ�ռ��"},{"LFN����"},{"���ļ�����"},{"��������" }};
	if(code < 20)
		strcpy(ostr, EInfo[code]);
// 	else
// 		strcpy(ostr, "δ֪����");
}
void dis_messgebox(char *istr, int code, unsigned char row)
{
	char buf[64];
	sprintf(buf, "%s:%d", istr, code);
	MSG_LOG("do %s,%s\r\n",__FUNCTION__,buf);
	getFATerror(buf+strlen(buf), code);
	display(row, 0, buf, DIS_CENTER);
	delayxs(2);
}

//
extern void get_current_info(unsigned char *buffer);
//U�̸��³��� 0:û�г�����£�1:����û��ʱ�����
int usb_loadPro(void)
{
	FRESULT res;
	char filename[20];
	unsigned char buff[1025];
	unsigned char buf1[50];
	FILINFO finf;
	FIL rf1;
	unsigned int length, addr, count,nowTime,endTime, i;
	char *p1, *p2;
	
	cls();
	display(0, 0, USB_DIS_DEAD, DIS_CENTER);
	display(2, 0, "�������", DIS_CENTER);
	
	res = f_open(&rf1, "0:/downpro/Pindex.txt", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
//		dis_messgebox("���������ļ���", res, 12);
		return 0;
	}
	
	res = f_stat("0:/downpro/Pindex.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("���������ļ���Ϣ��", res, 12);
		return 0;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("Pindex.txt->len:%d,%lld\r\n", length, finf.fsize);
	if(length < 8){
		dis_messgebox("�����������ȴ���", length, 12);
		return -1;
	}
	
	if(length > 64)
		length = 64;	//ֻȡ��ǰ��64�ֽڣ�����ֻ�ϵ�һ�С�

	memset(buff, 0, sizeof(filename));
	if(_Fread(buff, length, &rf1) != length){	//���������ļ�
		MSG_LOG("�����������ļ���\r\n");
		return -2;
	}

	p1 = strchr((char*)buff, '[');
	p2 = strchr((char*)buff, ']');

	if((p1 == NULL)||(p2 == NULL)){
		dis_messgebox("�����������ݴ�", 0xFF, 12);
		return -2;
	}

	memset(filename, 0, sizeof(filename));
	memcpy(filename, p1+1, p2-p1-1);
	MSG_LOG(filename);
	display(6,0,filename, DIS_CENTER);
	p1 = strchr(filename, '.');//���ļ�����ȡ�汾��
	if(p1 == NULL){
		dis_messgebox("�����ļ�����", 0xFF, 12);
		return -2;
	}
	p1 -= 4;	//ָ��汾��

	if(memcmp(buff, "ȷ�ϸ���", 8) == 0){//��Ҫ��ȷ�������и���
		display(4,0,"��Ҫ���³�����?", DIS_CENTER);
		if(keywait(KEY_ENT, 100) != KEY_ENT)
			return 0;
	}
	else if(memcmp(buff, "���ڸ���", 8) == 0){//�ж��Ƿ񵽸�������
		Ascii2BCD(buff+8, buf1, 8);
		GetDateTime();
		
		memcpy((unsigned char*)&nowTime, (unsigned char*)&SysTime, 4);
		over_turn(4, (unsigned char*)&nowTime);
		memcpy((unsigned char*)&endTime, buf1, 4);
		over_turn(4, (unsigned char*)&endTime);
		if(nowTime < endTime){
			memcpy(irda_rxBuf, buff+8, 8);
			irda_rxBuf[8] = 0;
			return 1;	//û��ʱ�����
		}
	}
	else if(memcmp(buff, "���̸���", 8) == 0){//
		;
	}
	else{
		dis_messgebox("�����������ݴ�", 0xFF, 12);
		return -3;
	}

	strcpy((char*)buf1, "0:/downpro/");
	strcat((char*)buf1, filename);
	MSG_LOG((char*)buf1);

	//�������

	res=f_open(&rf1, (char*)buf1, FA_READ);
	if(res!=FR_OK)
	{
		dis_messgebox("�����ļ��򿪴���", length, 12);
		return -4;
	}
	
	res = f_stat((char*)buf1, &finf);
	if(res != FR_OK){
		dis_messgebox("�����ļ���Ϣ��", res, 12);
		return -1;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("bcardlst.blk len:%d,%lld\r\n", length, finf.fsize);

	addr=FLASH_PRO_START;
	
    for(i=0;i<7;i++)	//F407, 448K���ڳ���ռ�
	{	
		flasherase(ERASE64KBYTESECTOR,(FLASH_PRO_START+65536*i));
		Delay(100);
	}

	flasherase(ERASE64KBYTESECTOR,(FLASH_PRO_FLAG));//����"������Ҫ����"
	f_lseek(&rf1,0);
	count = length;
	while(1)
	{
		clr_wdt();
		if(Isdevice_connected() == 0){//U��û�������ˣ��˳�
			f_close(&rf1);
			return -3;
		}

		if(count > 512){
			res=f_read(&rf1,buff,512,&nowTime);
			if(res!=FR_OK)
			{
				dis_messgebox("�����ļ���ȡ����", length, 12);
				f_close(&rf1);
				return -4;
			}

//			debugdata(buff, nowTime, 1);
			flashwrite(addr,buff,512);
			count -= 512;
			addr += 512;
		}
		else{
			res=f_read(&rf1,buff,count,&nowTime);
			if(res!=FR_OK)
			{
				dis_messgebox("�����ļ���ȡ����", length, 12);
				f_close(&rf1);
				return -4;
			}
			
			//			debugdata(buff, nowTime, 1);
			flashwrite(addr,buff,count);
			count = 0;
			addr += count;
		}

		sprintf((char*)buf1, "%d%%", ((length-count)*100)/length);
		display(8, 0, (char*)buf1, DIS_CENTER);

		if(count == 0)
		{
			break;
		}	
	}
	f_close(&rf1);

// 	addr = FLASH_PRO_START;
// 	for(i=0; i<length; ){
//		clr_wdt();
// 		flashread(addr, buff, 256);
// 		debugdata(buff, 256, 1);
// 		addr += 256;
// 		i += 256;
// 	}

	memset(buff,0,50);
	strcpy((char*)buff,"������Ҫ����");
	Ascii2BCD((unsigned char*)p1, buff+12, 4);
	flashwrite(FLASH_PRO_FLAG,(unsigned char*)buff,64);

//	display(12, 0, "�������������", DIS_CENTER);

	return length;
}

extern unsigned short gBlackBlockNum[16];
//U��ȡ������������0���޺������ļ�������1������ҪҪ���£�����<0:ʧ�ܣ�����>0�ɹ�
int usb_loadBlk(void)
{	
	FRESULT res;
	unsigned char buff[256];
	unsigned char buf1[16];
	FILINFO finf;
	FIL rf1;
	unsigned int length, addr, count;
	
	cls();
	display(0, 0, USB_DIS_DEAD, DIS_CENTER);
	display(2, 0, "���º�����", DIS_CENTER);

	res = f_open(&rf1, "0:/pra/bcardlst.blk", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("�������ļ���", res, 12);
		return 0;
	}

	res = f_stat("0:/pra/BCARDLST.BLK", &finf);
	if(res != FR_OK){
		dis_messgebox("�������ļ���Ϣ��", res, 12);
		return -1;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("bcardlst.blk len:%d,%lld\r\n", length, finf.fsize);
	if(length < (4+BLK_SNO_LEN)){
		dis_messgebox("���������ȴ���", length, 12);
		return -2;
	}

	if(_Fread(buff, 4, &rf1) != 4){	//����ǰ4���ֽڣ��Ǻ���������������Ϊ�汾�ţ������ͬ�Ͳ��ٸ�����
		MSG_LOG("���������ļ���\r\n");
		return -2;
	}

	addr = FLASH_BLK_START;
	flashread(addr, buf1, 4);
	if(memcmp(buf1, buff, 4) == 0){
		#warning "test blck ver"
		return 1;			//�汾һ�£����ٸ���,���ﻹ��Ҫ��̨���а汾����
	}
	flasherase(ERASE64KBYTESECTOR, addr);	//��������ʼ����
	memset(buf1, 0xff, 14);	//�豸�ں�������ǰ14�ֽ����ڰ汾����Ϣ����
	memcpy(buf1, buff, 4);	//����������ļ���ǰ4�ֽڡ�
	memcpy(gBusVerInfo.busBLKVer, buf1, 2);	//ֻ��ǰ��λ������ʾ�ϴ��汾

	flashwrite(addr, buf1, 14);
	addr += 14;
	count = length - 4;//ָʾ��ȡ�����ļ����ݣ�ǰ���Ѿ�����4�ֽ�
	while(1){
		clr_wdt();
		if(Isdevice_connected() == 0){//U��û�������ˣ��˳�
			f_close(&rf1);
			return -81;
		}
		if(count > 256){
			if(_Fread(buff, 256, &rf1) != 256){
				dis_messgebox("���������ȴ���!", 0xff, 12);
				return -2;
			}
			flashwrite(addr, buff, 256);
			addr += 256;
			count -= 256;
		}
		else{//���һ��
			if(_Fread(buff, count, &rf1) != count){
				dis_messgebox("���������ȴ���!", 0xff, 12);
				return -2;
			}
			flashwrite(addr, buff, count);
			addr += count;
			count = 0;
			break;
		}
		
		sprintf((char*)buf1, " %d%% ", (((length-count)*100)/length));
		display(7, 0, (char*)buf1, DIS_CENTER);
	}
	
	pFistVary.BlackListNum=((length-4)/BLK_SNO_LEN);
	sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
	length = pFistVary.BlackListNum;
	for(addr=0;addr<16;addr++)
		gBlackBlockNum[addr] = 0;
	for(addr=0;addr<16;addr++){
		if(length > BLACKLEN){
			gBlackBlockNum[addr] = BLACKLEN;
			length-=BLACKLEN;
		}
		else{
			gBlackBlockNum[addr] = length;
			break;
		}
	}
	fewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);

	MSG_LOG("usb load blk end!\r\n");
	return pFistVary.BlackListNum+2;
}


extern unsigned char SYSgetbussinessNO(unsigned char *dat);

//��֤U�̵ĺϷ���
int auth_USB(void)
{
	typedef struct{
		unsigned char SLflag[2];	//�̶�ΪAA 55
		unsigned char BUSSINESSNO[2];	//�̻���
		unsigned char startdatetime[7];	//��������ʱ��
		unsigned char enddate[4];		//��Ч��
		unsigned char XOR;				//У��
	}stauthDat;
	stauthDat *auP;
	FRESULT res;
	unsigned char buff[32];
	unsigned char Key3DES[16];
	unsigned char obuf[16];
	unsigned char Buss_id[2];
	FIL rf1;
	unsigned int endTime, startTime, nowTime;
	
	res = f_open(&rf1, "0:/auth.sys", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		MSG_LOG("��S�ļ���:%d\r\n", res);
		return -1;
	}
	if(_Fread(buff, 16, &rf1) != 16){
		MSG_LOG("��S�ļ���\r\n");
		return -2;
	}
	
	memcpy(Key3DES, "\x53\x4C\xAF\xC7\x9B\x1F\xB5\x5A\xD0\x83\xFC\x78\xE1\x58\x07\x55", 16);
	DES3_decrypt(buff, Key3DES, obuf);
	DES3_decrypt(buff+8, Key3DES, obuf+8);
	BCD_LOG(buff, 16, 1);
	BCD_LOG(obuf, 16, 1);

	buff[0] = 0;
	for(nowTime=0; nowTime< 16; nowTime++){
		buff[0] ^= obuf[nowTime];
	}
	if(buff[0] != 0){//У�����
		return -1;
	}

	auP = (stauthDat*)obuf;
	if(memcmp(auP->SLflag, "\xAA\x55", 2) != 0)
		return -2;
	SYSgetbussinessNO(Buss_id);
	if(memcmp(auP->BUSSINESSNO, Buss_id, 2) != 0)
		return -3;
	GetDateTime();
	memcpy((unsigned char*)&nowTime, (unsigned char*)&SysTime, 4);	//��Ч����,�����ġ�
	over_turn(4, (unsigned char*)&nowTime);
	memcpy((unsigned char*)&endTime, auP->enddate, 4);//��Ч���ڣ�����ǰ��
	over_turn(4, (unsigned char*)&endTime);
	memcpy((unsigned char*)&startTime, auP->startdatetime, 4);//��������
	over_turn(4, (unsigned char*)&startTime);
	if((nowTime<startTime)||(nowTime>endTime)){
		BCD_LOG((unsigned char*)&nowTime, 4, 1);
		BCD_LOG((unsigned char*)&startTime, 4, 1);
		BCD_LOG((unsigned char*)&endTime, 4, 1);
		return -4;
	}
	
	return 0;
}

extern void get_history_info(unsigned char file_no, unsigned char *buffer);
extern int get_hisdat_info(unsigned char *stime, unsigned char *etime, unsigned char *buffer);
//U�̲ɼ���ʷ��¼,����<0������ȷ��==0������Ҫ�ɼ�, ==1û�ҵ���ʷ����, >1�ɼ�����
int usb_cell_hisdat(void)
{
	FRESULT res;
	FILINFO finf;
	FIL filIndexHandle, rfile;
	int length;
	DIR tdir;

	unsigned int itmp, datalen, count, addr;
	History_Information information;
	unsigned char buff[RECORD_LEN];
	unsigned char buf2[64];
	char fileName[50];
	unsigned char startTime[4];
	unsigned char endTime[4];

	cls();
	display(0, 0, USB_DIS_DEAD, DIS_CENTER);

	display(2, 0, "��ʷ���ݲɼ�", DIS_CENTER);

	MSG_LOG("��ʷ���ݲɼ�\r\n");

//-------------������ʷ�ļ���Ϣ-------------
	
	res = f_opendir(&tdir, "0:/curdat");
	if(res != FR_OK){
		dis_messgebox("�ɼ�Ŀ¼��", res, 12);
		return 0;
	}
	
	MSG_LOG("cell data 1\r\n");
	//û�������ļ����Լ�����
	res = f_open(&filIndexHandle, "0:/curdat/HISCEL.txt", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{//û����ʷ�ļ������òɼ�
		return 0;
	}
	MSG_LOG("cell data 2\r\n");
	
	//	length = f_size(&filIndexHandle);//ȡ�����ĳ��Ȳ��ԡ���������
	res = f_stat("0:/curdat/HISCEL.txt", &finf);//�������ַ���ȡ
	if(res != FR_OK){
		dis_messgebox("��ʷ�ļ���Ϣ��", res, 12);
		return -2;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);
	
	MSG_LOG("Rindex len:%d\r\n", length);
	
	if (length<26)
	{
		f_close(&filIndexHandle);
		dis_messgebox("��ʷ�ļ����ȴ�", length, 12);
		return -5;
	}
	count = length;
	while(1){//�ҳ��ļ�����Ҫ���豸����ʷ��Ϣ,һ����Ϣ26�ֽڣ��ӻس����У�
		clr_wdt();
		memset(buff, 0, sizeof(buff));
		res = f_read(&filIndexHandle, buff, HISIfoLen, &itmp);
		if((res != FR_OK)||(itmp != HISIfoLen)){
			dis_messgebox("����ʷ�ļ���", res, 12);
			f_close(&filIndexHandle);
			return -5;
		}
		
		MSG_LOG("f_read hiscel.txt:%s \r\n", buff);
		if(memcmp(buff, "AAAAAAAA", 8) == 0){//���е�ˢ������Ҫ����ʷ��¼
			break;
		}
		else if((buff[0] == 'B')&&(buff[1] == 'B')){//BB��ͷ��Ϊ����·��Ϊ׼,�Ƚ���·��
			Ascii2BCD(buff+2, buf2, 6);	//��·
			if(memcmp(buf2, pFistVary.LineNo, 2) == 0)
				break;   //��ͬһ����· 
		}
		else{//����Ƚ��豸��
			if(memcmp(buff, pFistVary.DeviceNo, 8) == 0)
				break;   //��ͬ���豸�� 
		}
		count -= HISIfoLen;
		if(count == 0){//�����ļ����꣬û�ж�Ӧ�Ĳɼ��ļ���
			f_close(&filIndexHandle);
			return 0;
		}
	}
	f_close(&filIndexHandle);

	Ascii2BCD(buff+8, startTime, 8);//��ʼ����
	Ascii2BCD(buff+16, endTime, 8);//��ֹ����
	if((startTime[0] != 0x20)||(endTime[0] != 0x20)){//�����ж��������Ƿ�Ϸ�
		dis_messgebox("��ʷ��Ϣ���ڴ�", 0xFF, 12);
		return -1;
	}
	
	if(memcmp(startTime, "\x20\x20\x20\x20", 4) == 0){//�ɼ���������������
		itmp = buff[23];		
		if(((itmp-'0')>9)||((itmp-'0')<1))
			itmp = 1;
		else
			itmp = (itmp-'0');
		get_history_info(itmp,(unsigned char *)&information);
		
		addr = 0;
		datalen = 0;
		memcpy(buf2, information.offset, 2);
		buf2[2] = information.page;
		memcpy((unsigned char*)&addr, buf2, 3);	//��ַ

		memcpy((unsigned char*)&datalen, information.length, 4);	//���ݳ���
		display(4, 0, "ȫ���ɼ�", DIS_CENTER);
	}
	else{//�����ڲɼ�
		memset(buf2, 0, sizeof(buf2));
		memcpy(buf2, buff+8, 8);
		buf2[8] = '-';
		memcpy(buf2+9, buff+16, 8);
		display(4, 0, (char*)buf2, DIS_CENTER);
		if(get_hisdat_info(startTime, endTime, (unsigned char *)buf2) != 0)	//ȡ�ɼ����ݵĵ�ַ
			return 1;	//û�ҵ�����
		else{
			addr = 0;
			memcpy((unsigned char*)&addr, buf2, 4);
			memcpy((unsigned char*)&datalen, buf2+4, 4);
		}
	}
//-------------������ʷ�ļ���Ϣ-------------
//-----------------------------------------------------------------------------------

	res = f_opendir(&tdir, "0:/curdat");
	if(res != FR_OK){
		dis_messgebox("�ɼ�Ŀ¼��", res, 12);
		return -1;
	}
	
	MSG_LOG("cell data 1\r\n");
	//û�������ļ����Լ�����
	res = f_open(&filIndexHandle, "0:/curdat/Rindex.txt", FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("����������", res, 12);
		return -2;
	}
	MSG_LOG("cell data 2\r\n");
	
//	length = f_size(&filIndexHandle);//ȡ�����ĳ��Ȳ��ԡ���������
	res = f_stat("0:/curdat/Rindex.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("������Ϣ��", res, 12);
		return -2;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);

	MSG_LOG("Rindex len:%d\r\n", length);

	if ((length%14) != 0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("�������ȴ�", length, 12);
		return -5;
	}

	if((length) > (999*14)){
		f_close(&filIndexHandle);
		dis_messgebox("���ݲɼ���", (length/14), 12);
		return -5;
	}
	
	f_lseek(&filIndexHandle, length);//lseek(IndexHandle, 0, SEEK_END);
	
	sprintf((char*)buf2, "��¼����:%d", (datalen/RECORD_LEN));
	display(6, 0, (char*)buf2, DIS_CENTER);
	
	if(datalen == 0){//û�м�¼Ҫ�ɼ�
		f_close(&filIndexHandle);
		display(8, 0, "����ʷ��¼", DIS_CENTER);
		beep(3, 50, 50);
		return 0;
	}

	itmp = 0;
	memcpy(buff+itmp, (unsigned char *)&information, 28);//�ļ���
	itmp += 28;
	sprintf((char*)buff+itmp, "%04d", RECORD_LEN);	//������¼����
	itmp += 4;
	sprintf((char*)buff+itmp, "%05d", (datalen/RECORD_LEN));	//�ܼ�¼����
	itmp += 5;
	Get_SerialNumF4(buf2);
	BCD2Ascii(buf2, buff+itmp, 4);
	itmp += 8;
	
	memset(fileName, 0, sizeof(fileName));
	memcpy(fileName,"0:/curdat/",10);
	sprintf((char*)fileName+10,"shis%03d.rec",((length/14)+1));	//��ǰҪд����ļ���

	strcpy((char*)buf2, "�ļ���:");
	strcat((char*)buf2, (char*)fileName+10);
	display(14, 0, (char*)buf2, DIS_CENTER);
	res = f_open(&rfile, (char*)fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	if(res != FR_OK)//if (_dos_creat(fileName, _A_NORMAL, &RecordHandle)!=0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("������¼�ļ���", res, 12);
		return -6;
	}
	
	if(_Fwrite(buff,RECORD_LEN,&rfile)!=RECORD_LEN)
	{
		f_close(&filIndexHandle);
		f_close(&rfile);
		dis_messgebox("д��¼�ļ���", 0xFF, 12);
		return -7;
	}
	count = datalen;

	sprintf((char*)buf2, " 0%% ");
	display(8, 0, (char*)buf2, DIS_CENTER);
	while(count>0){
		clr_wdt();
		if(Isdevice_connected() == 0){//U��û�������ˣ��˳�			
			f_close(&filIndexHandle);
			f_close(&rfile);
			return -81;
		}
		if(count >= RECORD_LEN){
			flashread(addr, buff, RECORD_LEN);
			addr += RECORD_LEN;
			if(_Fwrite(buff,RECORD_LEN,&rfile)!=RECORD_LEN)
			{
				f_close(&filIndexHandle);
				f_close(&rfile);
				dis_messgebox("д��¼�ļ���", 0xFF, 12);
				return -8;
			}
			count -= RECORD_LEN;
		}
		else{
			flashread(addr, buff, count);
			addr += count;
			if(_Fwrite(buff,count,&rfile)!=count)
			{
				f_close(&filIndexHandle);
				f_close(&rfile);
				dis_messgebox("д��¼�ļ���", 0xFF, 12);
				return -8;
			}
			count = 0;
		}
		sprintf((char*)buf2, " %d%% ", (((datalen-count)*100)/datalen));
		display(8, 0, (char*)buf2, DIS_CENTER);
	}
	f_close(&rfile);	//�ɼ��������

	buff[0] = '<';
	memcpy(buff+1, fileName+10, 11);
	memcpy(buff+12,"\x0d\x0a", 2);
	buff[14] = 0;
	if(_Fwrite(buff, 14, &filIndexHandle) != 14)	//д�����ļ�
	{
		f_close(&filIndexHandle);
		f_unlink(fileName);				//дʧ�ܣ�ɾ���Ѿ�д���ļ�
		dis_messgebox("д�����ļ���", 0xFF, 12);
		return -9;
	}
	f_close(&filIndexHandle);
	beep(1, 100, 50);
	lib_clr_Record_point();	//�����ǰ��¼
//	delayxs(2);
	return (datalen/RECORD_LEN);
}

//U�̲ɵ�ǰ��¼ ���ؼ�¼������
int usb_cell_currdat(void)
{
	FRESULT res;
	FILINFO finf;
	FIL filIndexHandle, rfile;
	int length;
	DIR tdir;

	unsigned int itmp, datalen, count, addr;
	Current_Information information;
	unsigned char buff[RECORD_LEN];
	unsigned char buf2[64];
	char fileName[50];

	MSG_LOG("do %s\r\n",__FUNCTION__);
	cls();
	display(0, 0, USB_DIS_DEAD, DIS_CENTER);

	display(2, 0, "��ǰ���ݲɼ�", DIS_CENTER);

	clr_wdt();
	res = f_opendir(&tdir, "0:/curdat");
	if(res != FR_OK){
		dis_messgebox("��ǰĿ¼��", res, 12);
		return -1;
	}
	
	MSG_LOG("cell data 1\r\n");
	//û�������ļ����Լ�����
	res = f_open(&filIndexHandle, "0:/curdat/Rindex.txt", FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("����������", res, 12);
		return -2;
	}
	MSG_LOG("cell data 2\r\n");
	
//	length = f_size(&filIndexHandle);//ȡ�����ĳ��Ȳ��ԡ���������
	res = f_stat("0:/curdat/Rindex.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("������Ϣ��", res, 12);
		return -2;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);

	MSG_LOG("Rindex len:%d\r\n", length);

	if ((length%14) != 0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("�������ȴ�", length, 12);
		return -5;
	}

	if((length) > (999*14)){
		f_close(&filIndexHandle);
		dis_messgebox("���ݲɼ���", (length/14), 12);
		return -5;
	}
	
	f_lseek(&filIndexHandle, length);//lseek(IndexHandle, 0, SEEK_END);
	
	get_current_info((unsigned char *)&information);
	clr_wdt();

	memcpy((unsigned char*)&datalen, information.length, 4);//���ݳ���

	memset(buf2,0,sizeof(buf2));
	sprintf((char*)buf2, "��¼����:%d", (datalen/RECORD_LEN));
	display(5, 0, (char*)buf2, DIS_CENTER);
	MSG_LOG("%s\r\n",buf2);
	
	if(datalen == 0){//û�м�¼Ҫ�ɼ�
		f_close(&filIndexHandle);
		display(7, 0, "�޵�ǰ��¼", DIS_CENTER);
		beep(3, 50, 50);
		return 0;
	}

	addr = 0;
	memcpy(buf2, information.offset, 2);
	buf2[2] = information.page;
	memcpy((unsigned char*)&addr, buf2, 3);	//������ʼ��ַ

	itmp = 0;
	memcpy(buff+itmp, (unsigned char *)&information, 28);//�ļ���
	itmp += 28;
	sprintf((char*)buff+itmp, "%04d", RECORD_LEN);	//������¼����
	itmp += 4;
	sprintf((char*)buff+itmp, "%05d", (datalen/RECORD_LEN));	//�ܼ�¼����
	itmp += 5;
	Get_SerialNumF4(buf2);
	BCD2Ascii(buf2, buff+itmp, 4);
	itmp += 8;
	
	memset(fileName, 0, sizeof(fileName));
	memcpy(fileName,"0:/curdat/",10);
	sprintf((char*)fileName+10,"scur%03d.rec",((length/14)+1));	//��ǰҪд����ļ���
	MSG_LOG("%s\r\n",fileName);

	memset(buf2,0,sizeof(buf2));
	strcpy((char*)buf2, "�ļ���:");
	strcat((char*)buf2, (char*)fileName+10);
	display(14, 0, (char*)buf2, DIS_CENTER);
	MSG_LOG("%s\r\n",buf2);

	res = f_open(&rfile, (char*)fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	if(res != FR_OK)//if (_dos_creat(fileName, _A_NORMAL, &RecordHandle)!=0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("������¼�ļ���", res, 12);
		return -6;
	}
	MSG_LOG("1111\r\n");
	clr_wdt();


// 	f_close(&filIndexHandle);
// 	res = f_open(&rfile, (char*)fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
// 	if(res != FR_OK)//if (_dos_creat(fileName, _A_NORMAL, &RecordHandle)!=0)
// 	{
// 		f_close(&filIndexHandle);
// 		dis_messgebox("������¼�ļ���", res, 12);
// 		return -6;
// 	}
// 
// 	MSG_LOG("���´�--\r\n");

	if(_Fwrite(buff,RECORD_LEN,&rfile)!=RECORD_LEN)
	{
		f_close(&filIndexHandle);
		f_close(&rfile);
		dis_messgebox("д��¼�ļ���", 0xFF, 12);
		return -7;
	}
	MSG_LOG("2222\r\n");
	count = datalen;

	memset(buf2,0,sizeof(buf2));
	sprintf((char*)buf2, " 0%% ");
	display(7, 0, (char*)buf2, DIS_CENTER);
	while(count>0){
		clr_wdt();
		if(Isdevice_connected() == 0){//U��û�������ˣ��˳�			
			f_close(&filIndexHandle);
			f_close(&rfile);
			return -81;
		}
		if(count >= RECORD_LEN){
			flashread(addr, buff, RECORD_LEN);
			addr += RECORD_LEN;
			if(_Fwrite(buff,RECORD_LEN,&rfile)!=RECORD_LEN)
			{
				f_close(&filIndexHandle);
				f_close(&rfile);
				dis_messgebox("д��¼�ļ���", 0xFF, 12);
				return -8;
			}
			count -= RECORD_LEN;
		}
		else{
			flashread(addr, buff, count);
			addr += count;
			if(_Fwrite(buff,count,&rfile)!=count)
			{
				f_close(&filIndexHandle);
				f_close(&rfile);
				dis_messgebox("д��¼�ļ���", 0xFF, 12);
				return -8;
			}
			count = 0;
		}
		sprintf((char*)buf2, " %d%% ", (((datalen-count)*100)/datalen));
		display(7, 0, (char*)buf2, DIS_CENTER);
	}
	f_close(&rfile);	//�ɼ��������

	MSG_LOG("3333\r\n");

	buff[0] = '<';
	memcpy(buff+1, fileName+10, 11);
	memcpy(buff+12,"\x0d\x0a", 2);
	buff[14] = 0;
	if(_Fwrite(buff, 14, &filIndexHandle) != 14)	//д�����ļ�
	{
		f_close(&filIndexHandle);
		f_unlink(fileName);				//дʧ�ܣ�ɾ���Ѿ�д���ļ�
		dis_messgebox("д�����ļ���", 0xFF, 12);
		return -9;
	}
	f_close(&filIndexHandle);
	display(10,0,"�����ǰ��¼", DIS_CENTER);
	lib_clr_Record_point();	//�����ǰ��¼
	beep(1, 100, 50);
//	delayxs(2);
	return (datalen/RECORD_LEN);
}


extern unsigned char restore_flag;
unsigned char USB_deal(void);
//U�̲ɼ�������
int main_usb_collect(void)
{
	unsigned char ret;
//	unsigned int delayms=1;
	cls();

	display(0, 0, USB_DIS_DEAD, DIS_CENTER);
	display(2, 0, " ���U���� ", DIS_CENTER);
	AppState=USH_USR_FS_TEST;
	restore_flag = 3;
	MSG_LOG("main USS1\r\n");
	usb_host_Inst();
	MSG_LOG("main USS2\r\n");


	while(1)
	{
		clr_wdt();
		usbh_process();
		ret = getkey(1);
		
		
		if((AppState==USH_USR_FS_FREE)||(ret == KEY_ESC))//�����˳�USH_User_App���Գ���
		{
			disable_host( );
			break;
		}
	}
	return 0;
}

//USB�ȴ�
int usb_end_Waite(void)
{
	while(1){
		delayxms(20);
		if(getkey(1) == KEY_ESC)
			return 0;
		if(Isdevice_connected() == 0)//U��û�������ˣ��˳�
			return 1;
	}
}

/***********************************************
*HOST ģʽ��������� ���еĲ����������������ִ��
***********************************************/
unsigned char USH_User_App(void)		
{ 
#define _USB_BLK_ 10
#define _USB_CURR_ 11
#define _USB_HIS_ 12
#define _USB_Pro_ 13

#define _USB_State_mount_ (1<<0)
#define _USB_State_getspace_ (1<<1)

	unsigned int total,free;
	int IgetcurdatLen=-1,IgetBlk=0,IgePro=0,Icellhis=-1,Igetline=-1,i;
	char  res=0;
	unsigned char state=0;
	char dealprocess=0;
//	char timebuf[50],r_buff[50];
	static int flag=1;
	char testbuf[50];//,timebuff[50];
//	char test[]="you are blackman\r\n";
	
	dealprocess = 0;

	MSG_LOG("USH User deal\r\n");

	res=f_mount_usb(); 	//����U�̵�FAT�ļ�ϵͳ
	if(res!=0)
	{
		#ifdef _debug_
		debugstring("U�̹���ʧ��\r\n");
		#endif
		state |= _USB_State_mount_;
		goto usbdealEnd;
	}
	
	display(2, 0, "  U��������  ", DIS_CENTER);
  	#ifdef _debug_
	debugstring("�豸������\r\n");
	#endif
	res=get_usbfree(&total,&free);//��ȡU�̵���������ʣ��ռ�
	if(res==0)
	{
#ifdef _debug_
		 sprintf(testbuf,"total=%dMB\r\n",total>>10);
		 debugstring(testbuf);
		 memset(testbuf,0,50);
		 sprintf(testbuf,"free=%dMB\r\n",free>>10);
		 debugstring(testbuf);
#endif
	} 
	else
	{
#ifdef _debug_
		debugstring("U��������ȡʧ��\r\n");
#endif
		state |= _USB_State_getspace_;
		goto usbdealEnd;
	}

	i = auth_USB();
	if(i != 0){
		unf_mount_usb();
		AppState = USH_USR_FS_FREE;
		sprintf(testbuf, "  U����ʧ��:%d  ", i);
		display(2, 2, testbuf, DIS_CENTER);
		display(14, 2, " ��γ�U�� ", DIS_CENTER|DIS_CONVERT);
		usb_end_Waite();
		return 0;
	}

#ifdef _debug_
	debugstring("��֤ͨ��\r\n");
#endif

	dealprocess = _USB_BLK_;
	while(Isdevice_connected())//�豸���ӳɹ�֮���һֱ���������while()ѭ��
	{
		if(flag == 1){
			flag = 0;
			cls();
			display(15, 0, "�豸���ӳɹ�", DIS_CENTER);
#ifdef _debug_
			debugstring("�豸���ӳɹ�������\r\n");
#endif
		}

		switch(dealprocess){
		case _USB_BLK_:	//�������
			dealprocess = _USB_CURR_;
			IgetBlk = usb_loadBlk();
			
			Igetline = usb_loadlineinfor();

			break;
		case _USB_CURR_:	//����ǰ��¼
			IgetcurdatLen = usb_cell_currdat();
			dealprocess = _USB_HIS_;
			break;
		case _USB_HIS_:		//������ʷ��¼
			Icellhis = usb_cell_hisdat();
			dealprocess = _USB_Pro_;
			break;
		case _USB_Pro_:	//�˵�ѡ���Ƿ���Ҫ���³�����ֿ�
			IgePro = usb_loadPro();
			dealprocess = 0;
			break;
		default:
			dealprocess = 0;
			break;
		}
		
		if(esc_usb_key()==0)
			break;
		if(dealprocess == 0){
			AppState = USH_USR_FS_FREE;
			break;
		}
/*
		if(function==updata_pro)
		{
			Updata_pro("0:/TESTBIN");//�������򣬲���������U���´�ų����ļ����ļ������ƣ�Ҳ���Բ����̷�ֱ��д�ļ�������Updata_pro("/BIN");
		}
		else if(function==updata_hzk16)
		{
			Updata_Hzk("0:/ZIKU16",FLASH_ZK_START);
		}
		else if(function==updata_hzk32)
		{
			Updata_Hzk("0:/ZIKU32",FLASH_ZK_START_3232);
		}
		*/
	}
usbdealEnd:
	if(state != 0)
		beep(3, 50, 50);
	else
		beep(1, 100, 100);

	unf_mount_usb();
	AppState = USH_USR_FS_FREE;
	//��ʾ������
	cls();
	display(0, 0, "USB�ɼ�", DIS_CENTER);
	res = 2;
	if(state & _USB_State_mount_){
		display(res, 0, "��U�̹���ʧ��", 0);
		usb_end_Waite();
		return 0;
	}
	if(state & _USB_State_getspace_){
		display(res, 0, "���ȡU�̿ռ�ʧ��", 0);
		usb_end_Waite();
		return 0;
	}
	if(IgetBlk < 0){
		sprintf(testbuf, "�����������ʧ��:%d", IgetBlk);
		display(res, 0, testbuf, 0);
	}
	else{
		if(IgetBlk == 0)
			display(res, 0, "���޺������ļ�", 0);
		else if (IgetBlk == 1)
			display(res, 0, "��������Ѿ�����", 0);
		else
			display(res, 0, "����������³ɹ�", 0);
	}
	res += 2;
	if(Igetline < 0){
		sprintf(testbuf, "����·����ʧ��:%d", Igetline);
		display(res, 0, testbuf, 0);
	}
	else{
		if(Igetline == 0)
			display(res, 0, "������·�ļ�", 0);
		else if (Igetline == 1)
			display(res, 0, "����·�Ѿ�����", 0);
		else
			display(res, 0, "����·���³ɹ�", 0);
	}
	res += 2;

	if(IgetcurdatLen < 0){
		sprintf(testbuf, "��ǰ���ݲɼ�ʧ��:%d", IgetcurdatLen);
		display(res, 0, testbuf, 0);
	}
	else{
		sprintf(testbuf, "��ǰ���ݲɼ��ɹ�:%d��", IgetcurdatLen);
		display(res, 0, testbuf, 0);
	}
	res += 2;
	if(Icellhis != -1){//-1ʱ����Ҫ�ɣ�������ʾ
		if(Icellhis < 0){
			sprintf(testbuf, "����ʷ���ݲɼ�ʧ��:%d", Icellhis);
			display(res, 0, testbuf, 0);
		}
		else{
			sprintf(testbuf, "����ʷ���ݲɼ��ɹ�:%d��", Icellhis);
			display(res, 0, testbuf, 0);
		}
		res += 2;
	}
	
	if(IgePro != 0){//û�г�����£�����Ҫ��ʾ
		if(IgePro < 0){
			sprintf(testbuf, "����³���ʧ��:%d", IgePro);
			display(res, 0, testbuf, 0);
		}
		else if(IgePro == 1){
			strcpy(testbuf, "��U�̳���Ҫ��");
			strcat(testbuf, (char*)irda_rxBuf);
			strcat(testbuf, "�����");
			display(res, 0, testbuf, 0);
		}
		else{
			display(res, 0, "����³���ɹ�", 0); res+=2;
			display(res, 0, "�˳����Զ�����", 0);
		}
	}
	res += 2;
	
	display(14, 2, " ��γ�U�� ", DIS_CENTER|DIS_CONVERT);
	usb_end_Waite();

	if(IgePro > 1){
		ReStartCPU();
	}

	return 0;

	
#undef  _USB_BLK_
#undef _USB_CURR_
#undef _USB_HIS_
#undef _USB_Pro_
	

} 


