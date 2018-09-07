
//USB功能函数
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

//下面是文件系统。
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

#define USB_DIS_DEAD "U盘采集"
void getFATerror(char *ostr, unsigned char code)
{
	char EInfo[20][20]={ {"操作正常"},{"磁盘错误"},{"初始化错误"},{"磁盘没准备好"},{"找不到文件"},
						{"找不到路径"},{"文件名错误"},{"拒绝访问"},{"拒绝访问"},{"文件无效"},
						{"文件只读"},{"磁盘无效"},{"扇区无效"},{"无文件系统"},{"参数错误"},
						{"访问超时"},{"文件占用"},{"LFN错误"},{"打开文件过多"},{"参数错误" }};
	if(code < 20)
		strcpy(ostr, EInfo[code]);
// 	else
// 		strcpy(ostr, "未知错误");
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
//U盘更新程序 0:没有程序更新，1:程序没到时间更新
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
	display(2, 0, "程序更新", DIS_CENTER);
	
	res = f_open(&rf1, "0:/downpro/Pindex.txt", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
//		dis_messgebox("程序索引文件错", res, 12);
		return 0;
	}
	
	res = f_stat("0:/downpro/Pindex.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("程序索引文件信息错", res, 12);
		return 0;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("Pindex.txt->len:%d,%lld\r\n", length, finf.fsize);
	if(length < 8){
		dis_messgebox("程序索引长度错误", length, 12);
		return -1;
	}
	
	if(length > 64)
		length = 64;	//只取最前面64字节，就是只认第一行。

	memset(buff, 0, sizeof(filename));
	if(_Fread(buff, length, &rf1) != length){	//读出索引文件
		MSG_LOG("读程序索引文件错\r\n");
		return -2;
	}

	p1 = strchr((char*)buff, '[');
	p2 = strchr((char*)buff, ']');

	if((p1 == NULL)||(p2 == NULL)){
		dis_messgebox("程序索引内容错", 0xFF, 12);
		return -2;
	}

	memset(filename, 0, sizeof(filename));
	memcpy(filename, p1+1, p2-p1-1);
	MSG_LOG(filename);
	display(6,0,filename, DIS_CENTER);
	p1 = strchr(filename, '.');//从文件名中取版本号
	if(p1 == NULL){
		dis_messgebox("程序文件名错", 0xFF, 12);
		return -2;
	}
	p1 -= 4;	//指向版本号

	if(memcmp(buff, "确认更新", 8) == 0){//需要按确定键进行更新
		display(4,0,"需要更新程序吗?", DIS_CENTER);
		if(keywait(KEY_ENT, 100) != KEY_ENT)
			return 0;
	}
	else if(memcmp(buff, "日期更新", 8) == 0){//判断是否到更新日期
		Ascii2BCD(buff+8, buf1, 8);
		GetDateTime();
		
		memcpy((unsigned char*)&nowTime, (unsigned char*)&SysTime, 4);
		over_turn(4, (unsigned char*)&nowTime);
		memcpy((unsigned char*)&endTime, buf1, 4);
		over_turn(4, (unsigned char*)&endTime);
		if(nowTime < endTime){
			memcpy(irda_rxBuf, buff+8, 8);
			irda_rxBuf[8] = 0;
			return 1;	//没到时间更新
		}
	}
	else if(memcmp(buff, "立刻更新", 8) == 0){//
		;
	}
	else{
		dis_messgebox("程序索引内容错", 0xFF, 12);
		return -3;
	}

	strcpy((char*)buf1, "0:/downpro/");
	strcat((char*)buf1, filename);
	MSG_LOG((char*)buf1);

	//下面更新

	res=f_open(&rf1, (char*)buf1, FA_READ);
	if(res!=FR_OK)
	{
		dis_messgebox("程序文件打开错误", length, 12);
		return -4;
	}
	
	res = f_stat((char*)buf1, &finf);
	if(res != FR_OK){
		dis_messgebox("程序文件信息错", res, 12);
		return -1;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("bcardlst.blk len:%d,%lld\r\n", length, finf.fsize);

	addr=FLASH_PRO_START;
	
    for(i=0;i<7;i++)	//F407, 448K用于程序空间
	{	
		flasherase(ERASE64KBYTESECTOR,(FLASH_PRO_START+65536*i));
		Delay(100);
	}

	flasherase(ERASE64KBYTESECTOR,(FLASH_PRO_FLAG));//擦除"程序需要更新"
	f_lseek(&rf1,0);
	count = length;
	while(1)
	{
		clr_wdt();
		if(Isdevice_connected() == 0){//U盘没有连接了，退出
			f_close(&rf1);
			return -3;
		}

		if(count > 512){
			res=f_read(&rf1,buff,512,&nowTime);
			if(res!=FR_OK)
			{
				dis_messgebox("程序文件读取错误", length, 12);
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
				dis_messgebox("程序文件读取错误", length, 12);
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
	strcpy((char*)buff,"程序需要更新");
	Ascii2BCD((unsigned char*)p1, buff+12, 4);
	flashwrite(FLASH_PRO_FLAG,(unsigned char*)buff,64);

//	display(12, 0, "拷贝完成请重启", DIS_CENTER);

	return length;
}

extern unsigned short gBlackBlockNum[16];
//U盘取黑名单，返回0：无黑名单文件，返回1：不需要要更新，返回<0:失败，返回>0成功
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
	display(2, 0, "更新黑名单", DIS_CENTER);

	res = f_open(&rf1, "0:/pra/bcardlst.blk", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("黑名单文件错", res, 12);
		return 0;
	}

	res = f_stat("0:/pra/BCARDLST.BLK", &finf);
	if(res != FR_OK){
		dis_messgebox("黑名单文件信息错", res, 12);
		return -1;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("bcardlst.blk len:%d,%lld\r\n", length, finf.fsize);
	if(length < (4+BLK_SNO_LEN)){
		dis_messgebox("黑名单长度错误", length, 12);
		return -2;
	}

	if(_Fread(buff, 4, &rf1) != 4){	//读出前4个字节（是黑名单数量），作为版本号，如果相同就不再更新了
		MSG_LOG("读黑名单文件错\r\n");
		return -2;
	}

	addr = FLASH_BLK_START;
	flashread(addr, buf1, 4);
	if(memcmp(buf1, buff, 4) == 0){
		#warning "test blck ver"
		return 1;			//版本一致，不再更新,这里还需要后台进行版本处理。
	}
	flasherase(ERASE64KBYTESECTOR, addr);	//擦除。开始更新
	memset(buf1, 0xff, 14);	//设备内黑名单区前14字节用于版本号信息存贮
	memcpy(buf1, buff, 4);	//保存黑名单文件的前4字节。
	memcpy(gBusVerInfo.busBLKVer, buf1, 2);	//只有前两位用于显示上传版本

	flashwrite(addr, buf1, 14);
	addr += 14;
	count = length - 4;//指示读取多少文件数据，前面已经读了4字节
	while(1){
		clr_wdt();
		if(Isdevice_connected() == 0){//U盘没有连接了，退出
			f_close(&rf1);
			return -81;
		}
		if(count > 256){
			if(_Fread(buff, 256, &rf1) != 256){
				dis_messgebox("黑名单长度错误!", 0xff, 12);
				return -2;
			}
			flashwrite(addr, buff, 256);
			addr += 256;
			count -= 256;
		}
		else{//最后一次
			if(_Fread(buff, count, &rf1) != count){
				dis_messgebox("黑名单长度错误!", 0xff, 12);
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

//认证U盘的合法性
int auth_USB(void)
{
	typedef struct{
		unsigned char SLflag[2];	//固定为AA 55
		unsigned char BUSSINESSNO[2];	//商户号
		unsigned char startdatetime[7];	//启用日期时间
		unsigned char enddate[4];		//有效期
		unsigned char XOR;				//校验
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
		MSG_LOG("打开S文件错:%d\r\n", res);
		return -1;
	}
	if(_Fread(buff, 16, &rf1) != 16){
		MSG_LOG("读S文件错\r\n");
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
	if(buff[0] != 0){//校验错误
		return -1;
	}

	auP = (stauthDat*)obuf;
	if(memcmp(auP->SLflag, "\xAA\x55", 2) != 0)
		return -2;
	SYSgetbussinessNO(Buss_id);
	if(memcmp(auP->BUSSINESSNO, Buss_id, 2) != 0)
		return -3;
	GetDateTime();
	memcpy((unsigned char*)&nowTime, (unsigned char*)&SysTime, 4);	//有效日期,处理后的。
	over_turn(4, (unsigned char*)&nowTime);
	memcpy((unsigned char*)&endTime, auP->enddate, 4);//有效日期，处理前的
	over_turn(4, (unsigned char*)&endTime);
	memcpy((unsigned char*)&startTime, auP->startdatetime, 4);//启动日期
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
//U盘采集历史记录,返回<0：不正确，==0：不需要采集, ==1没找到历史数据, >1采集错误
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

	display(2, 0, "历史数据采集", DIS_CENTER);

	MSG_LOG("历史数据采集\r\n");

//-------------处理历史文件信息-------------
	
	res = f_opendir(&tdir, "0:/curdat");
	if(res != FR_OK){
		dis_messgebox("采集目录错", res, 12);
		return 0;
	}
	
	MSG_LOG("cell data 1\r\n");
	//没有索引文件，自己建立
	res = f_open(&filIndexHandle, "0:/curdat/HISCEL.txt", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{//没有历史文件，不用采集
		return 0;
	}
	MSG_LOG("cell data 2\r\n");
	
	//	length = f_size(&filIndexHandle);//取出来的长度不对。。。。。
	res = f_stat("0:/curdat/HISCEL.txt", &finf);//改用这种方法取
	if(res != FR_OK){
		dis_messgebox("历史文件信息错", res, 12);
		return -2;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);
	
	MSG_LOG("Rindex len:%d\r\n", length);
	
	if (length<26)
	{
		f_close(&filIndexHandle);
		dis_messgebox("历史文件长度错", length, 12);
		return -5;
	}
	count = length;
	while(1){//找出文件中需要此设备的历史信息,一行信息26字节（加回车换行）
		clr_wdt();
		memset(buff, 0, sizeof(buff));
		res = f_read(&filIndexHandle, buff, HISIfoLen, &itmp);
		if((res != FR_OK)||(itmp != HISIfoLen)){
			dis_messgebox("读历史文件错", res, 12);
			f_close(&filIndexHandle);
			return -5;
		}
		
		MSG_LOG("f_read hiscel.txt:%s \r\n", buff);
		if(memcmp(buff, "AAAAAAAA", 8) == 0){//所有的刷卡机都要采历史记录
			break;
		}
		else if((buff[0] == 'B')&&(buff[1] == 'B')){//BB开头的为以线路号为准,比较线路号
			Ascii2BCD(buff+2, buf2, 6);	//线路
			if(memcmp(buf2, pFistVary.LineNo, 2) == 0)
				break;   //是同一条线路 
		}
		else{//否则比较设备号
			if(memcmp(buff, pFistVary.DeviceNo, 8) == 0)
				break;   //是同个设备号 
		}
		count -= HISIfoLen;
		if(count == 0){//整个文件找完，没有对应的采集文件。
			f_close(&filIndexHandle);
			return 0;
		}
	}
	f_close(&filIndexHandle);

	Ascii2BCD(buff+8, startTime, 8);//起始日期
	Ascii2BCD(buff+16, endTime, 8);//终止日期
	if((startTime[0] != 0x20)||(endTime[0] != 0x20)){//大致判断下日期是否合法
		dis_messgebox("历史信息日期错", 0xFF, 12);
		return -1;
	}
	
	if(memcmp(startTime, "\x20\x20\x20\x20", 4) == 0){//采集备份区所有数据
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
		memcpy((unsigned char*)&addr, buf2, 3);	//地址

		memcpy((unsigned char*)&datalen, information.length, 4);	//数据长度
		display(4, 0, "全量采集", DIS_CENTER);
	}
	else{//按日期采集
		memset(buf2, 0, sizeof(buf2));
		memcpy(buf2, buff+8, 8);
		buf2[8] = '-';
		memcpy(buf2+9, buff+16, 8);
		display(4, 0, (char*)buf2, DIS_CENTER);
		if(get_hisdat_info(startTime, endTime, (unsigned char *)buf2) != 0)	//取采集数据的地址
			return 1;	//没找到数据
		else{
			addr = 0;
			memcpy((unsigned char*)&addr, buf2, 4);
			memcpy((unsigned char*)&datalen, buf2+4, 4);
		}
	}
//-------------处理历史文件信息-------------
//-----------------------------------------------------------------------------------

	res = f_opendir(&tdir, "0:/curdat");
	if(res != FR_OK){
		dis_messgebox("采集目录错", res, 12);
		return -1;
	}
	
	MSG_LOG("cell data 1\r\n");
	//没有索引文件，自己建立
	res = f_open(&filIndexHandle, "0:/curdat/Rindex.txt", FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("创建索引错", res, 12);
		return -2;
	}
	MSG_LOG("cell data 2\r\n");
	
//	length = f_size(&filIndexHandle);//取出来的长度不对。。。。。
	res = f_stat("0:/curdat/Rindex.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("索引信息错", res, 12);
		return -2;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);

	MSG_LOG("Rindex len:%d\r\n", length);

	if ((length%14) != 0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("索引长度错", length, 12);
		return -5;
	}

	if((length) > (999*14)){
		f_close(&filIndexHandle);
		dis_messgebox("数据采集满", (length/14), 12);
		return -5;
	}
	
	f_lseek(&filIndexHandle, length);//lseek(IndexHandle, 0, SEEK_END);
	
	sprintf((char*)buf2, "记录条数:%d", (datalen/RECORD_LEN));
	display(6, 0, (char*)buf2, DIS_CENTER);
	
	if(datalen == 0){//没有记录要采集
		f_close(&filIndexHandle);
		display(8, 0, "无历史记录", DIS_CENTER);
		beep(3, 50, 50);
		return 0;
	}

	itmp = 0;
	memcpy(buff+itmp, (unsigned char *)&information, 28);//文件名
	itmp += 28;
	sprintf((char*)buff+itmp, "%04d", RECORD_LEN);	//单条记录长度
	itmp += 4;
	sprintf((char*)buff+itmp, "%05d", (datalen/RECORD_LEN));	//总记录条数
	itmp += 5;
	Get_SerialNumF4(buf2);
	BCD2Ascii(buf2, buff+itmp, 4);
	itmp += 8;
	
	memset(fileName, 0, sizeof(fileName));
	memcpy(fileName,"0:/curdat/",10);
	sprintf((char*)fileName+10,"shis%03d.rec",((length/14)+1));	//当前要写入的文件名

	strcpy((char*)buf2, "文件名:");
	strcat((char*)buf2, (char*)fileName+10);
	display(14, 0, (char*)buf2, DIS_CENTER);
	res = f_open(&rfile, (char*)fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	if(res != FR_OK)//if (_dos_creat(fileName, _A_NORMAL, &RecordHandle)!=0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("创建记录文件错", res, 12);
		return -6;
	}
	
	if(_Fwrite(buff,RECORD_LEN,&rfile)!=RECORD_LEN)
	{
		f_close(&filIndexHandle);
		f_close(&rfile);
		dis_messgebox("写记录文件错", 0xFF, 12);
		return -7;
	}
	count = datalen;

	sprintf((char*)buf2, " 0%% ");
	display(8, 0, (char*)buf2, DIS_CENTER);
	while(count>0){
		clr_wdt();
		if(Isdevice_connected() == 0){//U盘没有连接了，退出			
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
				dis_messgebox("写记录文件错", 0xFF, 12);
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
				dis_messgebox("写记录文件错", 0xFF, 12);
				return -8;
			}
			count = 0;
		}
		sprintf((char*)buf2, " %d%% ", (((datalen-count)*100)/datalen));
		display(8, 0, (char*)buf2, DIS_CENTER);
	}
	f_close(&rfile);	//采集数据完成

	buff[0] = '<';
	memcpy(buff+1, fileName+10, 11);
	memcpy(buff+12,"\x0d\x0a", 2);
	buff[14] = 0;
	if(_Fwrite(buff, 14, &filIndexHandle) != 14)	//写索引文件
	{
		f_close(&filIndexHandle);
		f_unlink(fileName);				//写失败，删除已经写的文件
		dis_messgebox("写索引文件错", 0xFF, 12);
		return -9;
	}
	f_close(&filIndexHandle);
	beep(1, 100, 50);
	lib_clr_Record_point();	//清除当前记录
//	delayxs(2);
	return (datalen/RECORD_LEN);
}

//U盘采当前记录 返回记录长条数
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

	display(2, 0, "当前数据采集", DIS_CENTER);

	clr_wdt();
	res = f_opendir(&tdir, "0:/curdat");
	if(res != FR_OK){
		dis_messgebox("当前目录错", res, 12);
		return -1;
	}
	
	MSG_LOG("cell data 1\r\n");
	//没有索引文件，自己建立
	res = f_open(&filIndexHandle, "0:/curdat/Rindex.txt", FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("创建索引错", res, 12);
		return -2;
	}
	MSG_LOG("cell data 2\r\n");
	
//	length = f_size(&filIndexHandle);//取出来的长度不对。。。。。
	res = f_stat("0:/curdat/Rindex.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("索引信息错", res, 12);
		return -2;
	}
	length = finf.fsize;//f_size(&filIndexHandle);//filIndexHandle.obj.objsize;//
	MSG_LOG("stat len:%d,%lld\r\n", length, finf.fsize);

	MSG_LOG("Rindex len:%d\r\n", length);

	if ((length%14) != 0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("索引长度错", length, 12);
		return -5;
	}

	if((length) > (999*14)){
		f_close(&filIndexHandle);
		dis_messgebox("数据采集满", (length/14), 12);
		return -5;
	}
	
	f_lseek(&filIndexHandle, length);//lseek(IndexHandle, 0, SEEK_END);
	
	get_current_info((unsigned char *)&information);
	clr_wdt();

	memcpy((unsigned char*)&datalen, information.length, 4);//数据长度

	memset(buf2,0,sizeof(buf2));
	sprintf((char*)buf2, "记录条数:%d", (datalen/RECORD_LEN));
	display(5, 0, (char*)buf2, DIS_CENTER);
	MSG_LOG("%s\r\n",buf2);
	
	if(datalen == 0){//没有记录要采集
		f_close(&filIndexHandle);
		display(7, 0, "无当前记录", DIS_CENTER);
		beep(3, 50, 50);
		return 0;
	}

	addr = 0;
	memcpy(buf2, information.offset, 2);
	buf2[2] = information.page;
	memcpy((unsigned char*)&addr, buf2, 3);	//数据起始地址

	itmp = 0;
	memcpy(buff+itmp, (unsigned char *)&information, 28);//文件名
	itmp += 28;
	sprintf((char*)buff+itmp, "%04d", RECORD_LEN);	//单条记录长度
	itmp += 4;
	sprintf((char*)buff+itmp, "%05d", (datalen/RECORD_LEN));	//总记录条数
	itmp += 5;
	Get_SerialNumF4(buf2);
	BCD2Ascii(buf2, buff+itmp, 4);
	itmp += 8;
	
	memset(fileName, 0, sizeof(fileName));
	memcpy(fileName,"0:/curdat/",10);
	sprintf((char*)fileName+10,"scur%03d.rec",((length/14)+1));	//当前要写入的文件名
	MSG_LOG("%s\r\n",fileName);

	memset(buf2,0,sizeof(buf2));
	strcpy((char*)buf2, "文件名:");
	strcat((char*)buf2, (char*)fileName+10);
	display(14, 0, (char*)buf2, DIS_CENTER);
	MSG_LOG("%s\r\n",buf2);

	res = f_open(&rfile, (char*)fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	if(res != FR_OK)//if (_dos_creat(fileName, _A_NORMAL, &RecordHandle)!=0)
	{
		f_close(&filIndexHandle);
		dis_messgebox("创建记录文件错", res, 12);
		return -6;
	}
	MSG_LOG("1111\r\n");
	clr_wdt();


// 	f_close(&filIndexHandle);
// 	res = f_open(&rfile, (char*)fileName, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
// 	if(res != FR_OK)//if (_dos_creat(fileName, _A_NORMAL, &RecordHandle)!=0)
// 	{
// 		f_close(&filIndexHandle);
// 		dis_messgebox("创建记录文件错", res, 12);
// 		return -6;
// 	}
// 
// 	MSG_LOG("重新打开--\r\n");

	if(_Fwrite(buff,RECORD_LEN,&rfile)!=RECORD_LEN)
	{
		f_close(&filIndexHandle);
		f_close(&rfile);
		dis_messgebox("写记录文件错", 0xFF, 12);
		return -7;
	}
	MSG_LOG("2222\r\n");
	count = datalen;

	memset(buf2,0,sizeof(buf2));
	sprintf((char*)buf2, " 0%% ");
	display(7, 0, (char*)buf2, DIS_CENTER);
	while(count>0){
		clr_wdt();
		if(Isdevice_connected() == 0){//U盘没有连接了，退出			
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
				dis_messgebox("写记录文件错", 0xFF, 12);
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
				dis_messgebox("写记录文件错", 0xFF, 12);
				return -8;
			}
			count = 0;
		}
		sprintf((char*)buf2, " %d%% ", (((datalen-count)*100)/datalen));
		display(7, 0, (char*)buf2, DIS_CENTER);
	}
	f_close(&rfile);	//采集数据完成

	MSG_LOG("3333\r\n");

	buff[0] = '<';
	memcpy(buff+1, fileName+10, 11);
	memcpy(buff+12,"\x0d\x0a", 2);
	buff[14] = 0;
	if(_Fwrite(buff, 14, &filIndexHandle) != 14)	//写索引文件
	{
		f_close(&filIndexHandle);
		f_unlink(fileName);				//写失败，删除已经写的文件
		dis_messgebox("写索引文件错", 0xFF, 12);
		return -9;
	}
	f_close(&filIndexHandle);
	display(10,0,"清除当前记录", DIS_CENTER);
	lib_clr_Record_point();	//清除当前记录
	beep(1, 100, 50);
//	delayxs(2);
	return (datalen/RECORD_LEN);
}


extern unsigned char restore_flag;
unsigned char USB_deal(void);
//U盘采集主程序
int main_usb_collect(void)
{
	unsigned char ret;
//	unsigned int delayms=1;
	cls();

	display(0, 0, USB_DIS_DEAD, DIS_CENTER);
	display(2, 0, " 检测U盘中 ", DIS_CENTER);
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
		
		
		if((AppState==USH_USR_FS_FREE)||(ret == KEY_ESC))//控制退出USH_User_App测试程序
		{
			disable_host( );
			break;
		}
	}
	return 0;
}

//USB等待
int usb_end_Waite(void)
{
	while(1){
		delayxms(20);
		if(getkey(1) == KEY_ESC)
			return 0;
		if(Isdevice_connected() == 0)//U盘没有连接了，退出
			return 1;
	}
}

/***********************************************
*HOST 模式主程序入口 所有的操作在这个函数数中执行
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

	res=f_mount_usb(); 	//挂载U盘到FAT文件系统
	if(res!=0)
	{
		#ifdef _debug_
		debugstring("U盘挂载失败\r\n");
		#endif
		state |= _USB_State_mount_;
		goto usbdealEnd;
	}
	
	display(2, 0, "  U盘连接中  ", DIS_CENTER);
  	#ifdef _debug_
	debugstring("设备连接中\r\n");
	#endif
	res=get_usbfree(&total,&free);//获取U盘的总容量和剩余空间
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
		debugstring("U盘容量获取失败\r\n");
#endif
		state |= _USB_State_getspace_;
		goto usbdealEnd;
	}

	i = auth_USB();
	if(i != 0){
		unf_mount_usb();
		AppState = USH_USR_FS_FREE;
		sprintf(testbuf, "  U盘认失败:%d  ", i);
		display(2, 2, testbuf, DIS_CENTER);
		display(14, 2, " 请拔出U盘 ", DIS_CENTER|DIS_CONVERT);
		usb_end_Waite();
		return 0;
	}

#ifdef _debug_
	debugstring("认证通过\r\n");
#endif

	dealprocess = _USB_BLK_;
	while(Isdevice_connected())//设备连接成功之后会一直卡在下面的while()循环
	{
		if(flag == 1){
			flag = 0;
			cls();
			display(15, 0, "设备连接成功", DIS_CENTER);
#ifdef _debug_
			debugstring("设备连接成功！！！\r\n");
#endif
		}

		switch(dealprocess){
		case _USB_BLK_:	//处理参数
			dealprocess = _USB_CURR_;
			IgetBlk = usb_loadBlk();
			
			Igetline = usb_loadlineinfor();

			break;
		case _USB_CURR_:	//处理当前记录
			IgetcurdatLen = usb_cell_currdat();
			dealprocess = _USB_HIS_;
			break;
		case _USB_HIS_:		//处理历史记录
			Icellhis = usb_cell_hisdat();
			dealprocess = _USB_Pro_;
			break;
		case _USB_Pro_:	//菜单选择，是否需要更新程序或字库
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
			Updata_pro("0:/TESTBIN");//升级程序，参数必须是U盘下存放程序文件的文件夹名称，也可以不加盘符直接写文件夹名称Updata_pro("/BIN");
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
	//显示处理结果
	cls();
	display(0, 0, "USB采集", DIS_CENTER);
	res = 2;
	if(state & _USB_State_mount_){
		display(res, 0, "●U盘挂载失败", 0);
		usb_end_Waite();
		return 0;
	}
	if(state & _USB_State_getspace_){
		display(res, 0, "●获取U盘空间失败", 0);
		usb_end_Waite();
		return 0;
	}
	if(IgetBlk < 0){
		sprintf(testbuf, "●黑名单更新失败:%d", IgetBlk);
		display(res, 0, testbuf, 0);
	}
	else{
		if(IgetBlk == 0)
			display(res, 0, "●无黑名单文件", 0);
		else if (IgetBlk == 1)
			display(res, 0, "●黑名单已经最新", 0);
		else
			display(res, 0, "●黑名单更新成功", 0);
	}
	res += 2;
	if(Igetline < 0){
		sprintf(testbuf, "●线路更新失败:%d", Igetline);
		display(res, 0, testbuf, 0);
	}
	else{
		if(Igetline == 0)
			display(res, 0, "●无线路文件", 0);
		else if (Igetline == 1)
			display(res, 0, "●线路已经最新", 0);
		else
			display(res, 0, "●线路更新成功", 0);
	}
	res += 2;

	if(IgetcurdatLen < 0){
		sprintf(testbuf, "●当前数据采集失败:%d", IgetcurdatLen);
		display(res, 0, testbuf, 0);
	}
	else{
		sprintf(testbuf, "●当前数据采集成功:%d条", IgetcurdatLen);
		display(res, 0, testbuf, 0);
	}
	res += 2;
	if(Icellhis != -1){//-1时不需要采，不用提示
		if(Icellhis < 0){
			sprintf(testbuf, "●历史数据采集失败:%d", Icellhis);
			display(res, 0, testbuf, 0);
		}
		else{
			sprintf(testbuf, "●历史数据采集成功:%d条", Icellhis);
			display(res, 0, testbuf, 0);
		}
		res += 2;
	}
	
	if(IgePro != 0){//没有程序更新，不需要提示
		if(IgePro < 0){
			sprintf(testbuf, "●更新程序失败:%d", IgePro);
			display(res, 0, testbuf, 0);
		}
		else if(IgePro == 1){
			strcpy(testbuf, "●U盘程序要在");
			strcat(testbuf, (char*)irda_rxBuf);
			strcat(testbuf, "后更新");
			display(res, 0, testbuf, 0);
		}
		else{
			display(res, 0, "●更新程序成功", 0); res+=2;
			display(res, 0, "退出后自动重启", 0);
		}
	}
	res += 2;
	
	display(14, 2, " 请拔出U盘 ", DIS_CENTER|DIS_CONVERT);
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


