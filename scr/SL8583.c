
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "Mydes.h"
#include "SL8583.h"
#include "Gszct.h"
#include "QRCode.h"

#ifdef qPBOC_BUS
#include "qpboc_head.h"
#endif
#ifdef QR_CODE_COM
#include "QRCode.h"
#endif



unsigned char qboc_record_flag = 0;	//�ļ�����ʱ��������¼��ʶ��ɾ����¼������ʷ��¼ʱ��Ҫɾ��2�� ��ͬ����д��¼ʱ��Ҫд2�� һ��128*2�ֽ�


#define SL_8583_VER "\x02\x00"			// 2.0 Э��
	
//�ļ����صĲ����������ڼ�¼��ǰ�����������������
st8583filedown gsl8583FileDownPara;

unsigned char *gsl8583bitmapaddr;//8583λͼ����ַָ��
stsl8583Style gsl8583Style;	//ǩ������״̬
stsl8583filelist gsl8583filelist[sl8583fileNum];	//��ʱΪ6��,�ļ��汾Ϊ5��+1������汾(��0λ�����豸������������ʱ�豸�����������ء���


extern stPricetable NewPriceTab;
extern unsigned char PsamNo[6];
extern unsigned char restore_flag;
extern unsigned char gmissflag;//�����־ 0x00 ��½�ƶ�,ǩ��.
extern stcomtime SysTime;
extern pFistVary_1 pFistVary;
extern stBusVerIfo gBusVerInfo;
extern unsigned char GprsCardList[16];
extern stBusTicketV gBusTicketV;
extern unsigned char GPRSLinkProcess;
extern unsigned int gSendGLogin;//��½����

extern void delallBlacInbus(void);
extern unsigned char GetDateTime(void);
extern void getbusticketVer(void);
extern void getProVer(void);
extern unsigned char SYSgetbussinessNO(unsigned char *dat);
extern void sysSaveDevice(unsigned char *dbuf);
extern void getCpuInfo(stCpuInfo *inP);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern void savesysbupara(void);
extern void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat);

#include "Fatfiles.h"





const unsigned int CRC_32_Table[256]=
{
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
		0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
		0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
		0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
		0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
		
		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
		0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
		0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
		0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
		0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
		0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
		0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
		
		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
		0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
		0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
		0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
		0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
		
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
		0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
		0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
		0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
		0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

unsigned int CRC32_Cal(unsigned char *buf ,unsigned int len , unsigned int initial)
{
	unsigned int i;
	
	for(i=0;i<len;i++)
		initial=(initial >>8) ^ CRC_32_Table[(*(buf+i)) ^ (initial & 0x000000ff)];
	
	return ~initial;
}
//���ͨ322������ֲ������
int calFileCRC(unsigned int fileName,unsigned int fileLen)
{
//     int fd_file = open(fileName, O_RDWR | O_CREAT);
//     if(fd_file < 0)
//     {
//         return -1;
//     }
	  int i=0;
    unsigned int retVal = 0xffffffff;
    const unsigned int CAL_SIZE=512;
		unsigned int rest;
    unsigned char tmp[CAL_SIZE];
	
    if(fileLen <= 0) return -1;
	
	// 	MSG_LOG("fileLen:%d\r\n",fileLen);

    for(i = 0; i < fileLen / CAL_SIZE; i++)
    {
//         lseek(fd_file, i * CAL_SIZE, SEEK_SET);
//         read(fd_file, tmp, CAL_SIZE);
        flashread(fileName+i * CAL_SIZE,tmp,CAL_SIZE);
        retVal = CRC32_Cal(tmp, CAL_SIZE, retVal);
    }
	
     rest = fileLen % CAL_SIZE;
    if(rest > 0)
    {
//         lseek(fd_file, fileLen - rest, SEEK_SET);
//         read(fd_file, tmp, rest);
        flashread(fileName+ i* CAL_SIZE,tmp,rest);
        retVal = CRC32_Cal(tmp, rest, retVal);
    }
//    close(fd_file);
    return retVal;
}

void initsl8583para(void)
{
	memset((unsigned char*)&gsl8583Style, 0, sizeof(stsl8583Style));
	memset((unsigned char*)&gsl8583filelist, 0, sizeof(gsl8583filelist));

	memcpy(gsl8583Style.MAC_KEY, "sl8583testMACKEY", 16);
	gsl8583Style.crc_mackey = __cpuCrc32(gsl8583Style.MAC_KEY, 16);
}
//�����Ӧ���ļ��б��������������������汾������Ҳ���Բ����
void clrgsl8583filelist(unsigned char *fileflag)
{
	unsigned int i;
	for(i=0; i<sl8583fileNum; i++){
		if(memcmp(gsl8583filelist[i].filename, fileflag, 3) == 0){//����һ��
			memset(gsl8583filelist[i].filename, 0, 4);//���ƽ̨�ļ�����
		}
	}
}

//ǰ��0
void INT2BCD(unsigned int ii, unsigned char *bcdbuf, unsigned char bcdbuflen)
{
	unsigned int i, t;
	char buf[32];
	
	memset(buf, 0, 32);
	sprintf(buf, "%010u\0", ii);
	
	t = strlen(buf);
	i = t - (bcdbuflen*2);
	
	Ascii2BCD((unsigned char*)buf+i, bcdbuf, (bcdbuflen*2));
}


//
//mode=1,�̶�����CRC; mode=0,CRCУ�����ʱ���ؼ����CRC��һ���򷵻�0,
unsigned int checkgsl8583FileDownPara(unsigned char mode)
{
	unsigned char *buf;
	unsigned int cc;
	
	buf = (unsigned char *)&gsl8583FileDownPara;

	cc = __cpuCrc32(buf, sizeof(st8583filedown)-4);
	if(mode == 1)
		return cc;

	if(cc != gsl8583FileDownPara.CRC32){
		MSG_LOG(" CRC ERROR-CRC:%08X,PCR:%08X\r\n", cc, gsl8583FileDownPara.CRC32);
		return cc;
	}
	else
		return 0;
}

//�����������λ����λͼ
void setBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap)
{
	unsigned char fbyte,fbit;

	if(fieldID == 0)
		return;
	fbyte = ((fieldID-1)/8);
	fbit = ((fieldID-1)%8);

	sl8583bitmap[fbyte] |= (0x80>>fbit);

// 	MSG_LOG("bitmap_%02d:", fieldID);
// 	BCD_LOG(sl8583bitmap, 16, 1);
}
//�鿴��Ӧ�����Ƿ����,0����
unsigned char getBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap)
{
	unsigned char fbyte,fbit;
	
	if(fieldID <= 1)
		return ST_OK;
	fbyte = ((fieldID-1)/8);
	fbit = ((fieldID-1)%8);

// 	cv = (0x80>>fbit);
// 	MSG_LOG("ID:%d,fbyte:%02d, fbit:%d, value:%02X\r\n", fieldID, fbyte, fbit, sl8583bitmap[fbyte]);
// 	MSG_LOG("cv:%02X\r\n", cv);
// 	BCD_LOG(sl8583bitmap, 16, 1);
	
	if((sl8583bitmap[fbyte] & ((0x80>>fbit))) != 0){
		return ST_OK;
	}
	else{
		return ST_ERROR;
	}
}


//A��3���ֽڵĶ��������ַ��� 
unsigned int Build8583_7(unsigned char *dat)
{
	memcpy( dat, POS_Cand_FLAG,3);
	setBitmapBits(7, gsl8583bitmapaddr);
	return 3;
}
//N4��4���ֽڵĶ��������ַ���bcd 
//��������1.0.0����ʾΪ0100���������汾��Ҫ�ϴ�����汾v1.0.2��ʾ��0102��
//V2.0.0���ϵİ汾�У��ļ�����ʱ�����ļ�������
unsigned int Build8583_8(unsigned char *dat)
{
	memcpy( dat, SL_8583_VER,2);
	setBitmapBits(8, gsl8583bitmapaddr);
	return 2;
}



//N6��6���ֽڵĶ��������ַ��� ѹ��ʱ��BCD���ʾ��3���ֽڵĶ�����
unsigned int Build8583_11(unsigned char *dat, unsigned int pcno)
{
// 	INT2BCD(pcno, dat, 4);
	memcpy(dat,&pcno,4);
	MSG_LOG("send ADDR:%08X",pcno);
// 	BCD_LOG(dat,4,1);
	setBitmapBits(11, gsl8583bitmapaddr);
	return 4;
}
//9.1.3.6.	BM #12�յ����ն����ڵ�ʱ��
unsigned int Build8583_12(unsigned char *dat)
{
	memcpy(dat, (unsigned char*)&SysTime.hours, 3);
	setBitmapBits(12, gsl8583bitmapaddr);
	return 3;
}
//9.1.3.7.	BM #13�յ����ն����ڵ�����
unsigned int Build8583_13(unsigned char *dat)
{
	memcpy(dat, (unsigned char*)&SysTime.year_h, 4);
	setBitmapBits(13, gsl8583bitmapaddr);
	return 4;
}
//9.1.3.7.	BM #14��ǰ���μ�¼����
unsigned int Build8583_14(unsigned char *dat, unsigned char t)
{
	dat[0] = t;
	setBitmapBits(14, gsl8583bitmapaddr);
	return 1;
}

//9.1.3.21.	BM #41�յ����ն˱�ʶ��Ψһ��
unsigned int Build8583_41(unsigned char *dat)
{
	//Get_SerialNumF4(dat);
	Get_SerialNumF4(dat);
// 	MSG_LOG("SerialNumF4:");
// 	BCD_LOG(dat,4,1);
	setBitmapBits(41, gsl8583bitmapaddr);
	return 4;
}
//9.1.3.21.	BM #40�������� LLVAR
unsigned int Build8583_40(unsigned char *dat)
{
	unsigned int ii;
	ii = gsl8583Style.gcmdcode[0];
	memcpy(dat+1, gsl8583Style.gcmdcode+1, ii);
	
	INT2BCD(ii, dat, 1);//2�ֽڳ��ȵĳ���

	ii++;

	setBitmapBits(40, gsl8583bitmapaddr);
	return ii;
}
//9.1.3.22.	BM #42�̻���
unsigned int Build8583_42(unsigned char *dat)
{
	memset(dat, 0, 4);
	SYSgetbussinessNO(dat+2);
	setBitmapBits(42, gsl8583bitmapaddr);
	return 4;
}
//9.1.3.22.	BM #43��·���
unsigned int Build8583_43(unsigned char *dat)
{
	memset(dat,0, 8);
	BCD2Ascii( pFistVary.LineNo,dat, 2);
	setBitmapBits(43, gsl8583bitmapaddr);
	return 8;
}

//9.1.3.22.	BM #44�������
unsigned int Build8583_44(unsigned char *dat)
{
	memcpy(dat, pFistVary.DeviceNo, 8);
	setBitmapBits(44, gsl8583bitmapaddr);
	return 8;
}

//9.1.3.22.	BM #45�ļ����Ķ���  LLLVAR  ASCII ������ļ���(ASCII)3bytes+�汾(Hex)2bytes
//2.0.0 Э���ö�����ļ�����(ASCII)3bytes+�ļ�����(ASCII)8bytes+�汾(Hex)2bytes
unsigned int Build8583_45(unsigned char *dat)
{
	unsigned int pos;

	pos = 2;//ǰ2���ֽ���BCD���LLL
	
	getProVer();

	memcpy(dat+pos, SL8583FileFLAG_PRO, 3);	pos += 3;//�ļ�����
	memset(dat+pos,0,8);
	memcpy(dat+pos,POS_Cand_FLAG,3);  pos += 8;
	memcpy(dat+pos, gBusVerInfo.busProVer, 2);//����汾
	over_turn(2, dat+pos);
	pos += 2;
	
	memcpy(dat+pos, SL8583FileFLAG_BLK, 3); pos += 3;	
	memset(dat+pos,0,8); pos += 8;
	if(memcmp(gBusVerInfo.busBLKVer, "\xff\xff", 2) == 0)
		memset(gBusVerInfo.busBLKVer, 0, 2);
	memcpy(dat+pos, gBusVerInfo.busBLKVer, 2);//�������汾2B
	over_turn(2, dat+pos);
	MSG_LOG("�������汾:");
	BCD_LOG( dat+pos,2,1);
	pos += 2;



	

	//get_WHT_BankVer();
	memcpy(dat+pos, SL8583FileFLAG_WHT, 3); pos += 3;	
	memset(dat+pos,0,8); 
	memcpy(dat+pos,WHT_BANK,4);
	pos += 8;
	memcpy(dat+pos, gBusVerInfo.WHT_Bank, 2);//�����׵��汾2B
	over_turn(2, dat+pos);
	pos += 2;



	memcpy(dat+pos, SL8583FileFLAG_GPS, 3); pos += 3;
	memset(dat+pos,0,8); pos += 8;
	memcpy(dat+pos, gBusVerInfo.busLineVer, 2);//��λ������Ϣ�汾2B
	over_turn(2, dat+pos);
	pos += 2;
	
	
	memcpy(dat+pos , SL8583FileFLAG_PRI, 3); pos += 3;
	memset(dat+pos,0,8);	
	BCD2Ascii(pFistVary.LineNo,dat+pos,2); pos += 8;
	getbusticketVer();
	memcpy(dat+pos, gBusTicketV.busticketVer, 2);//Ʊ����Ϣ�汾2B
	over_turn(2, dat+pos);
	pos += 2;

	memcpy(dat+pos , SL8583FileFLAG_qpoc, 3); pos += 3;
	memset(dat+pos,0,8); 
	memcpy(dat+pos,WHT_BUS,3);
	pos += 8;
	feread(BIT_QPOC_,2, gBusVerInfo.QPOCVer);	//��Կ
	memcpy(dat+pos, gBusVerInfo.QPOCVer, 2);//��Կ��Ϣ�汾2B
	over_turn(2, dat+pos);
	pos += 2;

	memcpy(dat+pos , SL8583FileFLAG_SPK, 3); pos += 3;
	memset(dat+pos,0,8); pos += 8;
	feread(BIT_SPKVER,2, gBusVerInfo.busVoiceVer);	//��Կ
	memcpy(dat+pos, gBusVerInfo.busVoiceVer, 2);//��Կ��Ϣ�汾2B
	over_turn(2, dat+pos);
	pos += 2;


	memcpy(dat+pos , SL8583FileFLAG_TAC, 3); pos += 3;
	memset(dat+pos,0,8); pos += 8;
	feread(BIT_TACVER,2, gBusVerInfo.busTacVer);		//�Գ���Կ��Ϣ�汾2B
	memcpy(dat+pos, gBusVerInfo.busTacVer, 2);//�Գ���Կ��Ϣ�汾2B
	over_turn(2, dat+pos);
	pos += 2;
	
// 	memcpy(dat+pos , SL8583FileFLAG_PKI, 3); pos += 3;
// 	memset(dat+pos,0,8); 
// 	memcpy(dat+pos,PKI_UIID,7);
// 	pos += 8;
// 	LoadfileVer(FLASH_UIID,gBusVerInfo.UIID);
// 	memcpy(dat+pos, gBusVerInfo.UIID, 2);//uiid�汾2B
// 	over_turn(2, dat+pos);
// 	pos += 2;
// 
// 	memcpy(dat+pos , SL8583FileFLAG_PKI, 3); pos += 3;
// 	memset(dat+pos,0,8); 
// 	memcpy(dat+pos,PKI_MAC,6);
// 	pos += 8;
// 	LoadfileVer(FLASH_MAC_KEY,gBusVerInfo.MAC_KEY);
// 	memcpy(dat+pos, gBusVerInfo.MAC_KEY, 2);//mackey�汾2B
// 	over_turn(2, dat+pos);
// 	pos += 2;




	pos -= 2;	//���ݲ��ֵĳ��ȡ�
	
	INT2BCD(pos, dat, 2);//2�ֽڳ��ȵĳ���

	pos += 2;//�ռ�����LLL�����ڼ��ϳ��ȡ�
	
	MSG_LOG("bit45:%d\r\n",pos);
	BCD_LOG(dat, pos, 1);
	setBitmapBits(45, gsl8583bitmapaddr);
	return pos;
}

//9.1.3.22.	BM #47�ļ��������	ans��20	LLVAR	ASCII	�ļ���(ASCII)3bytes+�汾(Hex)2bytes +offset 4bytes + length 4bytes
// unsigned int Build8583_47(unsigned char *dat)
// {
// 	unsigned int pos, i;
// 	
// 	pos  = 0;
// 	
// 	memcpy(dat+pos, gsl8583FileDownPara.Miss_Fileflag, 3); //�����µ��ļ���ʶ
// 	pos += 3;
// 
// 	memcpy(dat+pos, gsl8583FileDownPara.Miss_ver, 2);		//��Ҫ���ص��ļ��汾2B
// 	over_turn(2, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
// 	pos += 2;
// 	
// 	memcpy(dat+pos, (unsigned char*)&gsl8583FileDownPara.Miss_offset, 4);//��Ҫ���ص�ƫ����
// 	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
// 	pos += 4;
// 
// 	if((gsl8583FileDownPara.Miss_ALL_LEn-gsl8583FileDownPara.Miss_offset)<slFileDonwLenPer)
// 		i = gsl8583FileDownPara.Miss_ALL_LEn-gsl8583FileDownPara.Miss_offset;
// 	else
// 		i = slFileDonwLenPer;	//һ��ȡ��
// 	memcpy(dat+pos, (unsigned char*)&i, 4);
// 	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
// 	pos += 4;
// 
// 	MSG_LOG("47:");
// 	BCD_LOG(dat,pos,1);
// 	setBitmapBits(47, gsl8583bitmapaddr);
// 	return pos;
// }

//LLV
//2.0.0 Э���ö�����ļ�����(ASCII)3bytes+�ļ�����(ASCII)8bytes+�汾(Hex)2bytes
unsigned int Build8583_50(unsigned char *dat)
{
	unsigned int pos;
	int i=0;
	
	pos = 1;//ǰ1���ֽ���BCD���LL
	
	
	memcpy(dat+pos, gsl8583FileDownPara.Miss_Fileflag, 3); //�����µ��ļ���ʶ
	pos += 3;

	memcpy(dat+pos,gsl8583FileDownPara.Miss_FileVer2,8);
	pos += 8;

	memcpy(dat+pos, gsl8583FileDownPara.Miss_ver, 2);		//��Ҫ���ص��ļ��汾2B
	over_turn(2, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 2;
	
	memcpy(dat+pos, (unsigned char*)&gsl8583FileDownPara.Miss_offset, 4);//��Ҫ���ص�ƫ����
	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 4;
	
	if((gsl8583FileDownPara.Miss_ALL_LEn-gsl8583FileDownPara.Miss_offset)<slFileDonwLenPer)
		i = gsl8583FileDownPara.Miss_ALL_LEn-gsl8583FileDownPara.Miss_offset;
	else
		i = slFileDonwLenPer;	//һ��ȡ��
	memcpy(dat+pos, (unsigned char*)&i, 4);
	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 4;
	
	MSG_LOG("����file:��%d��%d\r\n",gsl8583FileDownPara.Miss_offset,i);

	pos -= 1;	//���ݲ��ֵĳ��ȡ�
	
	INT2BCD(pos, dat, 1);//1�ֽڳ��ȵĳ���
	
	pos += 1;//�ռ�����LL�����ڼ��ϳ��ȡ�
	
// 	MSG_LOG("bit50:%d\r\n",pos);
// 	BCD_LOG(dat, pos, 1);
	setBitmapBits(50, gsl8583bitmapaddr);
	return pos;
}

//9.1.3.22.	BM #49�ն��豸�����汾
unsigned int Build8583_49(unsigned char *dat)
{
	getbusticketVer();
	memcpy(dat, gBusTicketV.busticketVer, 2);//Ʊ����Ϣ�汾2B		
	setBitmapBits(49, gsl8583bitmapaddr);
	return 2;
}
//51 �����豸������Ϣ	B10		HEX	10	M	M	�汾(2bytes HEX)+����(4bytes HEX)+��ʼλ��(4bytes HEX)+���󳤶�(4bytes HEX)
//����ǰ��Ҫ�ѱ���gsl8583FileDownPara��ʼ����
unsigned int Build8583_51(unsigned char *dat)
{
	unsigned int pos, i;
	
	pos  = 0;
	
	memcpy(dat+pos, gsl8583FileDownPara.Miss_ver, 2);//��Ҫ���ص��ļ��汾2B
	over_turn(2, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 2;
	
	memcpy(dat+pos, (unsigned char*)&gsl8583FileDownPara.Miss_ALL_LEn, 4);//�ļ�����
	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 4;
	
	memcpy(dat+pos, (unsigned char*)&gsl8583FileDownPara.Miss_offset, 4);//��Ҫ���ص�ƫ����
	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 4;
	
	if((gsl8583FileDownPara.Miss_ALL_LEn-gsl8583FileDownPara.Miss_offset)<slFileDonwLenPer)
		i = gsl8583FileDownPara.Miss_ALL_LEn-gsl8583FileDownPara.Miss_offset;
	else
		i = slFileDonwLenPer;	//һ��ȡ256�ֽڡ�
	memcpy(dat+pos, (unsigned char*)&i, 4);
	over_turn(4, dat+pos);//�Ϸ���ƽ̨���Ǹ�λ��ǰ��
	pos += 4;
	
	setBitmapBits(51, gsl8583bitmapaddr);
	return pos;
}

//LLV
//2.0.0 Э���ö�����ļ�����(ASCII)3bytes+�ļ�����(ASCII)8bytes+�汾(Hex)2bytes
unsigned int Build8583_52(unsigned char *dat,stsl8583msg_ack * ack)
{
	unsigned int pos;
	int i=0;
	unsigned short lenth=0;
	pos = 2;//ǰ2���ֽ���BCD���LLL
	
	

	for (i=0;i<1;i++)	//��д��һ����¼ȷ�ϰɣ��е��鷳
	{
		memcpy(&lenth,ack->msg_len,2);
		INT2BCD(lenth+8, dat+pos, 2);// //type2 + 4 id+ 2 code
		pos += 2;
		memcpy(dat+pos,ack->msg_type,2);
		pos += 2;
		memcpy(dat+pos,ack->msg_id,4);
		pos += 4;
		memcpy(dat+pos,ack->msg_ack_code,2);
		pos += 2;
		memcpy(dat+pos,ack->msg_buff,lenth );
		pos +=lenth ;

	}
	
	pos -= 2;	//���ݲ��ֵĳ��ȡ�
	
	INT2BCD(pos, dat, 2);//2�ֽڳ��ȵĳ���
	
	pos += 2;//�ռ�����LLL�����ڼ��ϳ��ȡ�
	
// 	MSG_LOG("bit52:%d\r\n",pos);
// 	BCD_LOG(dat, pos, 1);
	setBitmapBits(52, gsl8583bitmapaddr);
	return pos;
}


extern unsigned char StufferNO[4];
extern unsigned char DriveCardNo[4];

//9.1.3.22.	BM #54�ն�������
unsigned int Build8583_54(unsigned char *dat)
{
	unsigned int pos, i;
	unsigned int temp=0;

	pos = 2;	//ǰ�����ֽ�Ϊ����
	i = strlen(SYS_HEAD_STR);
	dat[pos++] = 0x01;					//01(tag)	�̻�����
	INT2BCD(i, dat+pos, 1); pos += 1;
	memcpy(dat+pos, SYS_HEAD_STR, i);
	pos += i;

	dat[pos++] = 0x02;					//02(tag)	˾�����
	dat[pos++] = 0x08;//L
#ifdef BUS_Cloud_
	BCD2Ascii(StufferNO, dat+pos, 4); //��Ա
#else
	temp = 0;
	memcpy(&temp,DriveCardNo,4);
	sprintf((char *)(dat+pos), "%08d", temp);//˾�����Ź̶�8λ
#endif
	pos += 8;

	dat[pos++] = 11;//11(tag)	SIM��
	dat[pos++] = 0x16;//L	ΪBCD��
	memcpy(dat+pos, GprsCardList, 16);	//���ﴫ���ֻ�����
	pos += 16;
	
	dat[pos++] = 12;//12(tag)	PSAM����
	dat[pos++] = 0x12;//L ΪBCD��
	BCD2Ascii(PsamNo, dat+pos, 6);
	pos += 12;

	
	INT2BCD(pos-2, dat, 2);//2�ֽڳ��ȵĳ���

	setBitmapBits(54, gsl8583bitmapaddr);
	return pos;
}


//9.1.3.22.	BM #����������� LLVAR	
unsigned int Build8583_55(unsigned char *dat)
{


	unsigned int pos;
//	int i=0;
//	unsigned short lenth=0;
	unsigned int uitemp=0;
	pos = 1;//ǰ1���ֽ���BCD���LLL
	
	memcpy(dat+pos,"\x03\x00",2);//��������  ��ά�������
	pos += 2;

	uitemp = get_QR_blkVer();
	MSG_LOG("��������:uitemp ID:%d :50��\r\n",uitemp);
	over_turn(4,(unsigned char *)&uitemp);
	memcpy(dat+pos ,&uitemp,4);
	pos +=4;

	//���ܴ������ slFileDonwLenPer
	uitemp = 50;	// 50���� *16 �ֽ�

	over_turn(4,(unsigned char *)&uitemp);
	memcpy(dat+pos ,&uitemp,4);
	pos +=4;

	
	pos -= 1;	//���ݲ��ֵĳ��ȡ�
	
	INT2BCD(pos, dat, 1);//1�ֽڳ��ȵĳ���
	
	pos += 1;//�ռ�����LL�����ڼ��ϳ��ȡ�

	setBitmapBits(55, gsl8583bitmapaddr);

	MSG_LOG("bit55:%d\r\n",pos);
	BCD_LOG(dat, pos, 1);

	return pos;

}


//ԭʼ���Ѽ�¼
//61��3����¼�Ļ���������  ���ȣ�2byte)+����(1byte)+data1 +���ȣ�2byte)+����(1byte)+data2 +���ȣ�2byte)+����(1byte)+data3
//���� 000301AABB  



int Build8583_61(unsigned char *oDat, unsigned int headP, unsigned char mode ,unsigned int alen)
{
	unsigned int pos=0,i=0;
	int ret=0;
	
	pos = 2;	//ǰ�����ֽ��ǳ���
	
	if(alen > 1){//ÿ�δ�1����¼
		alen = 1;
	}
	
	//	alen *= (RECORD_LEN+3);	//��Ҫ�����ֽ���  2������¼����+����+dat
	

	qboc_record_flag = 0;
	
	for (i=0;i<alen;i++)	
	{
		INT2BCD(DATALEN+1, oDat+pos, 2);// ����1�ֽ�������ʱ���� +256����
		pos += 2;
		oDat[pos++] = 0x30;				//���ͣ�������0x3E,����322   ��˾ ������ 0x30
		if (mode == 1)
		{
			MSG_LOG("��ʷ�ɼ�---\r\n");
			ret = Read_oneHis(oDat+pos);	
			
			if(ret == qPbocRECORD_LEN){//
				MSG_LOG("��������2\r\n");
				qboc_record_flag = 1;
				INT2BCD(qPbocRECORD_LEN+1, oDat+2, 2);// ����1�ֽ�������ʱ���� +256����  ���¸�ֵ����


			}
			
			pos += ret;
			headP += ret;	//3000e �Ƕ���洢

			
		}else{


			flashread(headP, oDat+pos, DATALEN);
			if(memcmp(oDat+pos, "��������", 8) == 0){//
				MSG_LOG("��������1\r\n");
				INT2BCD(qPbocRECORD_LEN+1, oDat+2, 2);// ����1�ֽ�������ʱ���� +256����  ���¸�ֵ����

				qboc_record_flag = 1;
				flashread(headP, oDat+pos, qPbocRECORD_LEN);

				BCD_LOG( oDat+pos,qPbocRECORD_LEN,1);

				pos += qPbocRECORD_LEN;
				headP += qPbocRECORD_LEN;	//3000e �Ƕ���洢


			}
			else if(memcmp(oDat+pos, "��ά��", 6) == 0){//
				MSG_LOG("��ά��\r\n");
				INT2BCD(qPbocRECORD_LEN+1, oDat+2, 2);// ����1�ֽ�������ʱ���� +256����  ���¸�ֵ����

				qboc_record_flag = 1;
				flashread(headP, oDat+pos, qPbocRECORD_LEN);

				BCD_LOG( oDat+pos,qPbocRECORD_LEN,1);

				pos += qPbocRECORD_LEN;
				headP += qPbocRECORD_LEN;	//3000e �Ƕ���洢


			}
			else{
				MSG_LOG("ͨ������\r\n");

				BCD_LOG( oDat+pos,DATALEN,1);
				pos += DATALEN;
				headP += RECORD_LEN;	//3000e �Ƕ���洢

			}

		}
	}
	
	
	
	INT2BCD(pos-2, oDat, 2);//2�ֽڳ��ȵĳ���   �ܳ�

	MSG_LOG("do %s len:%d\r\n",__FUNCTION__,pos);
	BCD_LOG(oDat,pos,1);
	
	setBitmapBits(61, gsl8583bitmapaddr);
	return pos;
}

int Build8583_64(unsigned char *iDat, unsigned int iDatLen, unsigned char *oDat)
{
	unsigned int cc;
	unsigned char radom[8];
	unsigned char omac[8];

	setBitmapBits(64, gsl8583bitmapaddr);	//��ΪMAC���������λԪ������Ҫ�Ȱѵ�64���λ���ϣ��ټ���MAC

	cc = __cpuCrc32(gsl8583Style.MAC_KEY, 16);
	if(cc == gsl8583Style.crc_mackey){//��MAC KEY��ǩ��ʱ�·�����ÿ�ο�������ǩ�������Բ���Ҫ������������ 
		memset(radom, 0, 8);
		//	CountMac1(gMobileParameter.KEK+(gMobileParameter.KEK_SN*16), dat, len, omac);
		CountMac1(gsl8583Style.MAC_KEY, radom, iDat, iDatLen, omac);
		memcpy(oDat, omac, 4);	//ȡǰ4���ֽ�
	}
	else{
		memset(oDat, 0xFF, 4);
	}
	return 4;
}

//���ա���������POS�ն˽���淶v2.00.docx�� �޶�ͨѶ��ʽ 

int build8583HEART2001(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
//	int curp, ltemp, headp;
	int iret, ilen;
	
	MSG_LOG("do %s\r\n",__FUNCTION__);
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x08\x80", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x20\x01", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
// 	memset(oDat+ilen, 0, 4);	ilen+=4;			//��9�� �ն˹�����
// 	setBitmapBits(9, gsl8583bitmapaddr);
// 	
// 	curp = Get_Record_point((unsigned char*)&headp, 0);
// 	ltemp=(curp - headp)/RECORD_LEN;
// 	memcpy(oDat+ilen, (unsigned char*)&ltemp, 3);	ilen+=3;//10�� �ն�δ����¼����
// 	setBitmapBits(10, gsl8583bitmapaddr);
	

	iret = Build8583_7(oDat+ilen); ilen+=iret;	//ÿһ�����Ķ�Ҫ��
	iret = Build8583_8(oDat+ilen); ilen+=iret;		//�汾	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen);	ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	
	return ilen;
}
//ȡ��Ϣָ��
int build8583GET_MSG_5001(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;

	MSG_LOG("ȡ��Ϣdo %s\r\n",__FUNCTION__);
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x05\x00", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x50\x01", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	iret = Build8583_7(oDat+ilen); ilen+=iret;	//ÿһ�����Ķ�Ҫ��
	iret = Build8583_8(oDat+ilen); ilen+=iret;		//�汾	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen);	ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	
	return ilen;

}

//������Ϣȷ��ָ��
int build8583_MSGACK_5002(unsigned char *oDat,stsl8583msg_ack *msg)
{
	DEF8583MSG_6001A *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;
	
	MSG_LOG("��Ϣȷ��do %s\r\n",__FUNCTION__);
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x05\x20", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x50\x02", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	iret = Build8583_7(oDat+ilen); ilen+=iret;	//ÿһ�����Ķ�Ҫ��
	iret = Build8583_8(oDat+ilen); ilen+=iret;		//�汾	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen);	ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	iret = Build8583_52(oDat+ilen,msg);	ilen+=iret;
	
	return ilen;
	
}



int build8583SINGIN6001(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
	int curp, ltemp, headp;
	int iret, ilen;

	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);

	memcpy(oDat+ilen, "\x08\x00", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x60\x01", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);

	iret = Build8583_7(oDat+ilen); ilen+=iret;		//����ʶ���룬
	iret = Build8583_8(oDat+ilen); ilen+=iret;		//Э��汾

	memset(oDat+ilen, 0, 4);	ilen+=4;			//��9�� �ն˹�����
	setBitmapBits(9, gsl8583bitmapaddr);
	
	curp = Get_Record_point((unsigned char*)&headp, 0);
	ltemp=(curp - headp)/RECORD_LEN;
	memcpy(oDat+ilen, (unsigned char*)&ltemp, 3);	ilen+=3;//10�� �ն�δ����¼����
	setBitmapBits(10, gsl8583bitmapaddr);
	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	MSG_LOG("ilen:%d\r\n", ilen);
	iret = Build8583_45(oDat+ilen);	ilen+=iret;
	iret = Build8583_49(oDat+ilen);	ilen+=iret;
	iret = Build8583_54(oDat+ilen);	ilen+=iret;

	return ilen;
}

//1.3.2���ն�ǩ��-6002
int build8583SingOUT6002(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
	int iret, ilen;
	
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x08\x00", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x60\x02", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen); ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	iret = Build8583_64(oDat, ilen, oDat+ilen);	ilen+=iret;
	
	return ilen;
}

//�ļ�����-6003
int build8583DownFile6003(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
	int iret, ilen;
	
	if(checkgsl8583FileDownPara(0) != 0){//���ؽṹû�г�ʼ���ã��������ɱ��ġ�
		MSG_LOG("6003 CRCERROR!!\r\n");
		return 0;
	}

	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x08\x40", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x60\x03", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	
	iret = Build8583_7(oDat+ilen); ilen+=iret;		//����ʶ���룬
	iret = Build8583_8(oDat+ilen); ilen+=iret;		//Э��汾

	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen); ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;

//	iret = Build8583_47(oDat+ilen);	ilen+=iret;
	iret = Build8583_50(oDat+ilen);	ilen+=iret;
	iret = Build8583_64(oDat, ilen, oDat+ilen);	ilen+=iret;
	
	return ilen;
}

//���ͨ������������ѯ 5003
int build8583_ADD_Flie5003(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
	int iret, ilen;
	
	
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x05\x40", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x50\x03", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	
	iret = Build8583_7(oDat+ilen); ilen+=iret;		//����ʶ���룬
 	iret = Build8583_8(oDat+ilen); ilen+=iret;		//Э��汾
	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen); ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	
	//	iret = Build8583_47(oDat+ilen);	ilen+=iret;
	iret = Build8583_55(oDat+ilen);	ilen+=iret;
	
	return ilen;
}


int sl8583ECHO5003( unsigned char *dfiledata, unsigned int datlen)
{

	int ret=0;
	unsigned int blk_num=0;


	if (memcmp(dfiledata,"\x03\x00",2) == 0)
	{
// 		memcpy(&blk_num,dfiledata+10,4);
// 		over_turn(4,(unsigned char *)&blk_num);

		blk_num = BCD2int(dfiledata+10,4);

		if (datlen != blk_num*sizeof(st_QR_BLK) +14 )//��������(2B)+ƽ̨�汾(4B)+��ǰ�汾(4B)+��ǰ��С(4B)
		{
			MSG_LOG("QR��ʽ��%d-:%d-\r\n",datlen,blk_num*sizeof(st_QR_BLK) +14);
			return -1;
		}
		ret = write_BLK_QR(dfiledata,dfiledata+14,blk_num);
		if (ret != 0)
		{
			save_QR_blkVer(0);
		}

	}else{
		MSG_LOG("ʲô����--\r\n");
		return -1;
	}



	return 0;

}


//1.3.4�����������·Ʊ�۸���-6004
int build8583DownBUinfo6004(unsigned char *oDat)
{
	DEF8583MSG_6001A *msg6001A;
	int iret, ilen;
	
	if(checkgsl8583FileDownPara(0) != 0)//���ؽṹû�г�ʼ���ã��������ɱ��ġ�
		return 0;
	
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x08\x60", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x60\x04", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	
	iret = Build8583_12(oDat+ilen); ilen+=iret;
	iret = Build8583_13(oDat+ilen); ilen+=iret;
	iret = Build8583_40(oDat+ilen); ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;
	
	iret = Build8583_51(oDat+ilen);	ilen+=iret;
	iret = Build8583_64(oDat, ilen, oDat+ilen);	ilen+=iret;
	
	return ilen;
}


//���ͼ�¼��
int build8583UPREC7001(unsigned char *oDat)
{
	struct{
		unsigned int shadd;
		unsigned int sLen;
		unsigned char sdate[8];
		unsigned int scrc32;
	}hisda;
	DEF8583MSG_6001A *msg6001A;
	unsigned int sum;
	int curp, ltemp, headp;
	int iret, ilen;
	unsigned char t;
	stFAT_hisRec hismsg;
	unsigned char mode=0;
	
	ilen = 0;
	msg6001A = (DEF8583MSG_6001A*)oDat;
	gsl8583bitmapaddr = msg6001A->bitMap;
	memset(gsl8583bitmapaddr, 0, 16);
	
	memcpy(oDat+ilen, "\x03\x00", 2); ilen+=2;
	ilen += 16;	//������λԪ������
	memcpy(oDat+ilen, "\x80\x70\x01", 3); ilen+=3;	//��3�� ���״�����
	setBitmapBits(3, gsl8583bitmapaddr);
	
	iret = Build8583_7(oDat+ilen); ilen+=iret;		//����ʶ���룬	
	iret = Build8583_8(oDat+ilen); ilen+=iret;		//�汾	

	
	//�ȴ����Ƿ�����ʷ��¼����
	sysferead(BIT_HISREC_SND, 20, (unsigned char *)&hisda);
	sum = __cpuCrc32((unsigned char *)&hisda, 16);
#ifdef _debug_
	debugstring("GJRec xxxxSend his data:");
	debugdata((unsigned char *)&hisda, 20, 1);
#endif
	if((hisda.scrc32 == sum)&&(hisda.sLen != 0)&&(hisda.sLen <= (FLASH2_BAKR_LEN/RECORD_LEN))&&(hisda.shadd < FLASH2_BAKR_LEN)){//����ʷ��¼
		headp = FLASH2_START1+hisda.shadd;
		ltemp = hisda.sLen;
	}
	else{
		curp = Get_Record_point((unsigned char*)&headp, 0);
		ltemp=(curp - headp)/RECORD_LEN;
	}


	mode = 0;

		
	sum = Check_hisfile(&hismsg);
	if (sum == 1 )
	{
		ltemp = 1;// 1��	
		
		memcpy(hisda.sdate,hismsg.sfilename+2,2);
		memcpy(hisda.sdate+2,&hismsg.offset,2);
		memcpy(&headp,hisda.sdate,4);		//ƴ�ӵ�ַ
		MSG_LOG("����ʷ��¼headpaddr:");
		BCD_LOG(hisda.sdate,4,1);
		mode = 1;
	}
	


	if(ltemp == 0){//û�м�¼
		return 0;
	}

	if(ltemp>1)
		t = 1;
	else
		t = ltemp;		//һ���������1��


	
	INT2BCD(ltemp, oDat+ilen, 3);//10�� �ն�δ����¼���� BCD	memcpy(oDat+ilen, (unsigned char*)&ltemp, 3); 
	ilen+=3;
	setBitmapBits(10, gsl8583bitmapaddr);

	iret = Build8583_11(oDat+ilen, headp); ilen+=iret;	//���͵�ǰ����ĳ�ʼ��ַ.
	iret = Build8583_14(oDat+ilen, t); ilen+=iret;
	iret = Build8583_40(oDat+ilen); ilen+=iret;
	iret = Build8583_41(oDat+ilen);	ilen+=iret;
	iret = Build8583_42(oDat+ilen);	ilen+=iret;
	iret = Build8583_43(oDat+ilen);	ilen+=iret;
	iret = Build8583_44(oDat+ilen);	ilen+=iret;

	iret = Build8583_61(oDat+ilen, headp,mode, t);	ilen+=iret;

	iret = Build8583_64(oDat, ilen, oDat+ilen);	ilen+=iret;
	
	return ilen;

}

//���뱨��ͷ
int Build8583TPDUHEAD(unsigned char *Odat)
{
	unsigned int pos =0;
	Odat[pos++] = 0x60;
	memset(Odat+pos, 0, 4);
	pos += 4;
	memcpy(Odat+pos, "\x61\x22\x00\x00\x00\x00", 6);
	pos += 6;

	return pos;
}

unsigned int Buildsl8583Packge(unsigned char *revBuf, unsigned char mode)
{
	unsigned int blen=0, len=0;

	revBuf[len++] = 0;	//ǰ����ǰ�����ȡ�
	revBuf[len++] = 0;	//ǰ����ǰ�����ȡ�

	blen = Build8583TPDUHEAD(revBuf+len);
	len += blen;

	switch (mode)
	{
	case MISS_G_LOGINGJ://ǩ��
		blen = build8583SINGIN6001(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	case MISS_G_SINGOUT://ǩ��
		blen = build8583SingOUT6002(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	case MISS_G_FILES://�����ļ�
		blen = build8583DownFile6003(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
// 	case MISS_G_PRICE:	//�����豸������Ϣ
// 		blen = build8583DownBUinfo6004(revBuf+len);
// 		if(blen == 0)
// 			return 0;
// 		len+=blen;
// 		revBuf[0] = (len-2)/256;
// 		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
// 		break;
	case MISS_G_UREC:	//1.4.1�����Ѽ�¼�ϴ�-7001
		blen = build8583UPREC7001(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	case MISS_G_TOO:	//2.0.0������
		blen = build8583HEART2001(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;
	
	case MISS_G_ALAM:		//ȡ��Ϣ
		blen = build8583GET_MSG_5001(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;

	case MISS_G_DBLKI:		//��ѯQR������
		blen = build8583_ADD_Flie5003(revBuf+len);
		if(blen == 0)
			return 0;
		len+=blen;
		revBuf[0] = (len-2)/256;
		revBuf[1] = (len-2)%256;//��ǰ������ܳ���
		break;


	default:
		len = 0;
		break;
	}
	return len;
}

//����ǩ��ʱ�·����ļ��汾�������ڱ���gsl8583filelist�С��ļ���(ASCII)3bytes+�汾(Hex)2bytes +�ļ�����(Hex)4bytes
//����ǩ��ʱ�·����ļ��汾�������ڱ���gsl8583filelist�С��ļ�����(ASCII)3+�ļ�����(ASCII)8B+�汾(Hex)2bytes +�ļ�����(Hex)4bytes +CRC32
void dealsl8583filelist(unsigned char *idat, unsigned char len)
{
	unsigned char pos=0;	//
	unsigned char i=0;

	MSG_LOG("do %s\r\n",__FUNCTION__);
	while(1){
		if((len - i)<21)
			break;		//û����
		if(pos > sl8583fileNum)
			break;	//������
		memcpy(gsl8583filelist[pos].filename, idat+i, 3);//
		i += 3;
 		memcpy(gsl8583filelist[pos].fileVer2, idat+i, 8);//���ա���������POS�ն˽���淶v2.00.docx�� �޶�ͨѶ��ʽ 
 		i += 8;

		memcpy((unsigned char*)&gsl8583filelist[pos].fileVer, idat+i, 2);//
		over_turn(2, (unsigned char*)&gsl8583filelist[pos].fileVer);//ƽ̨�·����Ǹ�λ��ǰ��
		i += 2;
		memcpy((unsigned char*)&gsl8583filelist[pos].filelen, idat+i, 4);//
		over_turn(4, (unsigned char*)&gsl8583filelist[pos].filelen);//ƽ̨�·����Ǹ�λ��ǰ��
		i += 4;
		memcpy((unsigned char*)&gsl8583filelist[pos].crc32, idat+i, 4);//
		over_turn(4, (unsigned char*)&gsl8583filelist[pos].crc32);//ƽ̨�·����Ǹ�λ��ǰ��
		i += 4;
		MSG_LOG("gsl8583filelist[%d],Name:%c%c%c  Ver:0x%04X\r\n",pos,gsl8583filelist[pos].filename[0],gsl8583filelist[pos].filename[1],gsl8583filelist[pos].filename[2],gsl8583filelist[pos].fileVer);
		MSG_LOG("fileVer2:%s\r\n",gsl8583filelist[pos].fileVer2);
		MSG_LOG("crc32:%08X\r\n",gsl8583filelist[pos].crc32);

		pos++;
	}
}



//�����Ѿ��ϴ��ļ�¼
void sl8583delECHO7001(unsigned int addr, unsigned int leng)
{
	struct{
		unsigned int shadd;
		unsigned int sLen;
		unsigned char sdate[8];
		unsigned int scrc32;
	}hisda;
	int curp, ltemp, headp;
	unsigned char dbuf[RECORD_LEN];
	unsigned int sum;
	stFAT_hisRec hismsg;

	curp = curp;
	ltemp = ltemp;

	
	
	sum = Check_Send_FAT_Rcord(&hismsg);
	if (sum == 1 )
	{
		
		memcpy(hisda.sdate,hismsg.sfilename+2,2);
		memcpy(hisda.sdate+2,&hismsg.offset,2);
		
		memcpy(&headp,hisda.sdate,4);
		

		MSG_LOG("������ַ:%08X,%08X\r\n", addr, headp);

		if (hismsg.offset == 0)
		{
			MSG_LOG("����ͷ%d--\r\n",DATALEN);
			hismsg.offset += DATALEN;	//
			SaveFAT_hisRecInfor(1);		//������һ����¼������ʱ�������ˣ� ����ʱ��ҲҪ���� ������			
		}
		
		if(headp == addr)
		{
			SaveFAT_hisRecInfor(1);
			if (qboc_record_flag==1)
			{
				qboc_record_flag = 0;
				//������128*2 �ֽڣ�����̨Ҳ����256�ֽ�
				MSG_LOG("2������256\r\n");
				SaveFAT_hisRecInfor(1);

			}
		}

		return;
	}
	

	leng =1;		//ֻ��һ��������һ��
	
	MSG_LOG("��Ӧ�ĵ�ַ:%08X,��������:%08X\r\n", addr, leng);
	//�������ʷ��¼�����ļ�����2�ֽڵ��ļ�ƫ������Ϊaddr
	if((addr >= FLASH2_START1)&&(addr < (FLASH2_BAKR_LEN+FLASH2_START1))){//��ʷ��¼
		//�ȴ����Ƿ�����ʷ��¼����
		sysferead(BIT_HISREC_SND, 20, (unsigned char *)&hisda);	//�����Ѿ����͵�
		if(hisda.sLen < leng)
			leng = hisda.sLen;
		hisda.shadd -= (leng*RECORD_LEN);
		hisda.sLen -= leng;
		sum = __cpuCrc32((unsigned char *)&hisda, 16);
		hisda.scrc32 = sum;
		sysfewrite(BIT_HISREC_SND, 20, (unsigned char*)&hisda);
	}
	else{
		curp = Get_Record_point((unsigned char*)&headp, 0);
		MSG_LOG("�����׵�ַ:%08X\r\n", headp);
		if(headp == addr){//�ش��ĵ�ַ�����͵��׵�ַһ�£������ɾ��

			if (qboc_record_flag==1)
			{
				qboc_record_flag = 0;

				MSG_LOG("1������256\r\n");

				leng = 2;		//������2��
			}
			memset(dbuf, 0, RECORD_LEN);
			for(sum=0; sum<leng; sum++){//��ǰ�Ѿ��ϴ���¼ɾ����
				MSG_LOG("Del record-\r\n");

				flashwrite(addr, dbuf, RECORD_LEN);//������д��ȫ0��
				addr += RECORD_LEN;
			}
		}
		else{
			MSG_LOG("�ش��ļ�¼��ַ��һ��:%08X,%08X\r\n", addr, headp);
		}
	}

}

int sl8583ECHO6002(void)
{
	gsl8583Style.ISOK = 0;	//��ǩ���ɹ���ʶ
	return 0;
}



#ifdef debug_GJ_TLVDeal_
//24 ���ֽڣ�Ϊunix ����ǰ����ʼʱ��4+����ʱ��4+16Bkey  ����24 �ֽ�
extern unsigned char Get_oneMACkey(unsigned char index,unsigned char outkey[24]);
#endif
extern void initTCPIPnode(void);
extern void saveBlackInfo(void);
extern unsigned char checkNewPrice_new(void);
//mode=2ȫ�����أ���������������,������յ��ĺ�������ȫ������ֱ�ӷŵ������������������ŵ���ʱ��
extern void DealGPRSBlack(unsigned char mode, unsigned int len, unsigned char *dat);
//dfileinfo:�ļ���(ASCII)3bytes+�汾(Hex)2bytes +offset 4bytes + length 4bytes
int sl8583ECHO6003(unsigned char *dfileinfo, unsigned char *dfiledata, unsigned int datlen)
{
	unsigned char buftemp[1024];
	unsigned int addr;//,startaddr=0;
	unsigned int Tack, i, itemp, iLdat;
//	unsigned char flag = 0;
	int crc32 =0;

	clr_wdt();


#ifdef _debug_
	MSG_LOG("infor:");
	BCD_LOG(dfileinfo,21,1);
// 	MSG_LOG("ver:0x%02X%02X",dfile->Miss_ver[0],dfile->Miss_ver[1]);
// 	MSG_LOG("v2:%s\r\n",dfile->Miss_Fileflag);
// 	MSG_LOG("allen:%d\r\n",dfile->Miss_ALL_LEn);
#endif

	over_turn(2,dfileinfo+11);



// 	over_turn(2, dfileinfo+3);
	if((memcmp(gsl8583FileDownPara.Miss_Fileflag, dfileinfo, 3) != 0)||
		(memcmp(gsl8583FileDownPara.Miss_ver, dfileinfo+11, 2) != 0)){//�汾���ļ�����Ҫһ��
		MSG_LOG("%02X%02X\r\n",gsl8583FileDownPara.Miss_ver[0],gsl8583FileDownPara.Miss_ver[1]);
		MSG_LOG("�汾��һ��:%02X%02X,%02X%02X",gsl8583FileDownPara.Miss_ver[0],gsl8583FileDownPara.Miss_ver[1],dfileinfo[11],dfileinfo[12]);
		return 1;
	}

	if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_BLK, 3) == 0){//�Ǻ�����
		iLdat = datlen/(BLK_SNO_LEN+2);//������Ϊ10��������
		MSG_LOG("������:");
		BCD_LOG(dfiledata, datlen, 1);
//find_G_new_mission
		if(gsl8583FileDownPara.Miss_offset == 10){	//��һ���������   ��find_G_miss �����Ѿ�����ǰ10���ֽڣ�������10�ж�
			
			delallBlacInbus();//������ں�����

			addr = FLASH_TEMP_START;
			flasherase(ERASE64KBYTESECTOR, addr);//�����ʱ�� ���������á�
		}
// 		if(gsl8583FileDownPara.Miss_offset == 0){
// 			DealGPRSBlack(2, iLdat, dfiledata+8);//��ǰΪȫ������,���������  �����汾
// 		}else{
			DealGPRSBlack(2, iLdat, dfiledata);//��ǰΪȫ������,���������
// 
// 		}


		gsl8583FileDownPara.Miss_offset += datlen;
		
		Tack = gsl8583FileDownPara.Miss_offset;
		itemp = gsl8583FileDownPara.Miss_ALL_LEn;
		sprintf((char*)buftemp, "B:%d%% ",  (((Tack+datlen)*100)/itemp));
		miniDispstr(13, 18, (char*)buftemp, 0);

		if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn){//�������
			addr = FLASH_TEMP_START;
			flasherase(ERASE64KBYTESECTOR,addr);

			gsl8583Style.ISOK = 0;	//��ǩ��
#ifdef _debug_
			debugstring("�깤����:FLASH_TEMP_START---\r\n");
#endif
			memcpy(gBusVerInfo.busBLKVer, gsl8583FileDownPara.Miss_ver, 2);//�º������汾�������ڵġ�
			memcpy(gBusVerInfo.newbusBLKVer, gsl8583FileDownPara.Miss_ver, 2);//�º������汾�������ڵġ�

			gmissflag= MISS_G_FREE;//
			restore_flag = 3;//ˢ�½���
			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
			saveBlackInfo();  //ֻ���°汾��

			clrgsl8583filelist(SL8583FileFLAG_BLK);
#ifdef debug_GJ_TLVDeal_
			debugstring("�������������:");
			debugstring("\r\n");
#endif
		}
		//--------------------------------------------------------------------------
	}
	else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_GPS, 3) == 0){//��λ������Ϣ
	}
	else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_qpoc, 3) == 0){//��Կ��Ϣ

		
		MSG_LOG("���й�Կ����:\r\n");
		Tack = gsl8583FileDownPara.Miss_offset;
		if (Tack == 0)//��һ����
		{
			
			
			addr = FLASH_TEMP_START;//д��ĵ�ַ 
#ifdef debug_GJ_TLVDeal_
			debugstring("�������:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 64K
			
			}
			
			gsl8583FileDownPara.Miss_offset += datlen;
			
			itemp = gsl8583FileDownPara.Miss_ALL_LEn;
			sprintf((char*)buftemp, "qpoc:%d%% ",  ((Tack*100)/itemp));
			miniDispstr(14, 0, (char*)buftemp, 0);
			
			addr = (FLASH_TEMP_START +12 + Tack);//д��ĵ�ַ��
			if(addr > FLASH_TEMP_END){//��ַ�����˰���д�룬��������˰�����
				restore_flag = 1;
				return 0;
			}else{
				for(i =0;i<3;i++){
					flashwrite(addr, dfiledata, datlen);//д��
					flashread(addr, buftemp, datlen);
					if(memcmp(dfiledata, buftemp, datlen) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("����д�����\r\n");
						debugdata(dfiledata, datlen, 1);
						debugdata(buftemp, datlen, 1);
#endif
						continue;
					}
				}
				
				if(i >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("��ͷ��ʼ\r\n");
#endif
					addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
					
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
					initTCPIPnode();
#endif
				}
			}
			
			if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
			{		
// 				crc32 = calFileCRC(FLASH_TEMP_START+12,gsl8583FileDownPara.Miss_ALL_LEn);				
// 				MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32,gsl8583FileDownPara.filecrc);
// 				if (crc32 != gsl8583FileDownPara.filecrc)
// 				{
// 					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
// 					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
// 					restore_flag=1;
// 					gsl8583Style.ISOK = 0;	//��ǩ��
// 					gmissflag= MISS_G_FREE;//
// 					GPRSLinkProcess =TCPSTARTSTAT;
// 					
// 					return 0;	//��������������
// 				}


				addr = FLASH_TEMP_START;

				memset(buftemp,0,20);
				memcpy(buftemp,gsl8583FileDownPara.Miss_ver,2);
				memcpy(buftemp+4,&gsl8583FileDownPara.Miss_ALL_LEn,4);
				memcpy(buftemp+8,&gsl8583FileDownPara.filecrc,4);
				MSG_LOG("qpocinfor:");
				BCD_LOG(buftemp,12,1);
				flashwrite(addr, buftemp, 12);//д��

				if (saveqpoc (FLASH_TEMP_START) != ST_OK)
				{
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
				//	gsl8583Style.ISOK = 0;	//��ǩ��
					gmissflag= MISS_G_FREE;//
					GPRSLinkProcess =TCPSTARTSTAT;
				
					return 0;	//��������������

				}
				
				clrgsl8583filelist(SL8583FileFLAG_SPK);
				
			//	gsl8583Style.ISOK = 0;		//����ǩ����
				gmissflag= MISS_G_FREE;//
				GPRSLinkProcess = TCPSTARTSTAT;
				cls();
				restore_flag=3;
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
				
			}
			
			
		}
		else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_SPK, 3) == 0){//��Կ��Ϣ

		
		MSG_LOG("��Կ����:\r\n");
		Tack = gsl8583FileDownPara.Miss_offset;
		if (Tack == 0)//��һ����
		{
			
			
			addr = FLASH_TEMP_START;//д��ĵ�ַ 
#ifdef debug_GJ_TLVDeal_
			debugstring("�������:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 64K
			
			}
			
			gsl8583FileDownPara.Miss_offset += datlen;
			
			itemp = gsl8583FileDownPara.Miss_ALL_LEn;
			sprintf((char*)buftemp, "SPK:%d%% ",  ((Tack*100)/itemp));
			miniDispstr(14, 0, (char*)buftemp, 0);
			
			addr = (FLASH_TEMP_START +12 + Tack);//д��ĵ�ַ��
			if(addr > FLASH_TEMP_END){//��ַ�����˰���д�룬��������˰�����
				restore_flag = 1;
				return 0;
			}else{
				for(i =0;i<3;i++){
					flashwrite(addr, dfiledata, datlen);//д��
					flashread(addr, buftemp, datlen);
					if(memcmp(dfiledata, buftemp, datlen) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("����д�����\r\n");
						debugdata(dfiledata, datlen, 1);
						debugdata(buftemp, datlen, 1);
#endif
						continue;
					}
				}
				
				if(i >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("��ͷ��ʼ\r\n");
#endif
					addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
					
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
					initTCPIPnode();
#endif
				}
			}
			
			if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
			{		
// 				crc32 = calFileCRC(FLASH_TEMP_START+12,gsl8583FileDownPara.Miss_ALL_LEn);				
// 				MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32,gsl8583FileDownPara.filecrc);
// 				if (crc32 != gsl8583FileDownPara.filecrc)
// 				{
// 					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
// 					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
// 					restore_flag=1;
// 					gsl8583Style.ISOK = 0;	//��ǩ��
// 					gmissflag= MISS_G_FREE;//
// 					GPRSLinkProcess =TCPSTARTSTAT;
// 					
// 					return 0;	//��������������
// 				}


				addr = FLASH_TEMP_START;

				memset(buftemp,0,20);
				memcpy(buftemp,gsl8583FileDownPara.Miss_ver,2);
				memcpy(buftemp+4,&gsl8583FileDownPara.Miss_ALL_LEn,4);
				memcpy(buftemp+8,&gsl8583FileDownPara.filecrc,4);
				MSG_LOG("SPKinfor:");
				BCD_LOG(buftemp,12,1);
				flashwrite(addr, buftemp, 12);//д��

				if (saveSPKInfo (FLASH_TEMP_START) != ST_OK)
				{
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
					gsl8583Style.ISOK = 0;	//��ǩ��
					gmissflag= MISS_G_FREE;//
					GPRSLinkProcess =TCPSTARTSTAT;
				
					return 0;	//��������������

				}
				
				clrgsl8583filelist(SL8583FileFLAG_SPK);
				
				gsl8583Style.ISOK = 0;		//����ǩ����
				gmissflag= MISS_G_FREE;//
				GPRSLinkProcess = TCPSTARTSTAT;
				cls();
				restore_flag=3;
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
				
			}
			
			
		}
		else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_TAC, 3) == 0){//�Գ���Կ��Ϣ

		
		MSG_LOG("�Գ���Կ����:\r\n");
		Tack = gsl8583FileDownPara.Miss_offset;
		if (Tack == 0)//��һ����
		{
			
			
			addr = FLASH_TEMP_START;//д��ĵ�ַ 
#ifdef debug_GJ_TLVDeal_
			debugstring("�������:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 64K
			
			}
			
			gsl8583FileDownPara.Miss_offset += datlen;
			
			itemp = gsl8583FileDownPara.Miss_ALL_LEn;
			sprintf((char*)buftemp, "TAC:%d%% ",  ((Tack*100)/itemp));
			miniDispstr(14, 0, (char*)buftemp, 0);
			
			addr = (FLASH_TEMP_START +12 + Tack);//д��ĵ�ַ��
			if(addr > FLASH_TEMP_END){//��ַ�����˰���д�룬��������˰�����
				restore_flag = 1;
				return 0;
			}else{
				for(i =0;i<3;i++){
					flashwrite(addr, dfiledata, datlen);//д��
					flashread(addr, buftemp, datlen);
					if(memcmp(dfiledata, buftemp, datlen) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("����д�����\r\n");
						debugdata(dfiledata, datlen, 1);
						debugdata(buftemp, datlen, 1);
#endif
						continue;
					}
				}
				
				if(i >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("��ͷ��ʼ\r\n");
#endif
					addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
					
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
					initTCPIPnode();
#endif
				}
			}
			
			if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
			{		
// 				crc32 = calFileCRC(FLASH_TEMP_START+12,gsl8583FileDownPara.Miss_ALL_LEn);				
// 				MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32,gsl8583FileDownPara.filecrc);
// 				if (crc32 != gsl8583FileDownPara.filecrc)
// 				{
// 					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
// 					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
// 					restore_flag=1;
// 					gsl8583Style.ISOK = 0;	//��ǩ��
// 					gmissflag= MISS_G_FREE;//
// 					GPRSLinkProcess =TCPSTARTSTAT;
// 					
// 					return 0;	//��������������
// 				}


				addr = FLASH_TEMP_START;

				memset(buftemp,0,20);
				memcpy(buftemp,gsl8583FileDownPara.Miss_ver,2);
				memcpy(buftemp+4,&gsl8583FileDownPara.Miss_ALL_LEn,4);
				memcpy(buftemp+8,&gsl8583FileDownPara.filecrc,4);
				MSG_LOG("TACinfor:");
				BCD_LOG(buftemp,12,1);
				flashwrite(addr, buftemp, 12);//д��

				if (saveTACInfo (FLASH_TEMP_START) != ST_OK)
				{
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
				///	gsl8583Style.ISOK = 0;	//��ǩ��
					gmissflag= MISS_G_FREE;//
					GPRSLinkProcess =TCPSTARTSTAT;
				
					return 0;	//��������������

				}
				
				clrgsl8583filelist(SL8583FileFLAG_TAC);
				
			//	gsl8583Style.ISOK = 0;		//����ǩ����
				gmissflag= MISS_G_FREE;//
				GPRSLinkProcess = TCPSTARTSTAT;
				cls();
				restore_flag=3;
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
				
			}
			
			
		}
	

	else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_WHT, 3) == 0){//��������Ϣ


#ifdef qPBOC_BUS

		if(memcmp(gsl8583FileDownPara.Miss_FileVer2, WHT_BANK,4) == 0)
		{

			Tack = gsl8583FileDownPara.Miss_offset;
			if (Tack == 0)//��һ����
			{
				
				
				addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
#ifdef debug_GJ_TLVDeal_
				debugstring("����ʱ������:");
				debugdata((unsigned char*)&addr, 4, 1);
#endif
				flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 64K
				
			}
			
			gsl8583FileDownPara.Miss_offset += datlen;
			
			itemp = gsl8583FileDownPara.Miss_ALL_LEn;
			sprintf((char*)buftemp, "WB:%d%% ",  (((Tack+datlen)*100)/itemp));
			miniDispstr(13, 18, (char*)buftemp, 0);


			

			addr = (FLASH_TEMP_START+12 + Tack);//д��ĵ�ַ��
			if(addr > FLASH_TEMP_END){//��ַ�����˰���д�룬��������˰�����
				restore_flag = 1;
				return 0;
			}
			else{
				for(i =0;i<3;i++){
					flashwrite(addr, dfiledata, datlen);//д��
					flashread(addr, buftemp, datlen);
					if(memcmp(dfiledata, buftemp, datlen) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("����д�����\r\n");
						debugdata(dfiledata, datlen, 1);
						debugdata(buftemp, datlen, 1);
#endif
						continue;
					}
				}
				
				if(i >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("��ͷ��ʼ\r\n");
#endif
					addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
					
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
					initTCPIPnode();
#endif
				}
			}
			

			
			if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
			{		
// 				crc32 = calFileCRC(FLASH_TEMP_START+12,gsl8583FileDownPara.Miss_ALL_LEn);				
// 				MSG_LOG("cal crc32:%08X,crd32%08X\r\n",crc32,gsl8583FileDownPara.filecrc);
// 				if (crc32 != gsl8583FileDownPara.filecrc)
// 				{
// 					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
// 					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
// 					gsl8583Style.ISOK = 0;	//��ǩ��
// 					gmissflag= MISS_G_FREE;//
// 					GPRSLinkProcess =TCPSTARTSTAT;
// 					return 0;	//��������������
// 				}
// 				
			
								


				addr = FLASH_TEMP_START;

				memset(buftemp,0,20);
				memcpy(buftemp,gsl8583FileDownPara.Miss_ver,2);
				memcpy(buftemp+4,&gsl8583FileDownPara.Miss_ALL_LEn,4);
				memcpy(buftemp+8,&gsl8583FileDownPara.filecrc,4);
// 				MSG_LOG("whtinfor:");
// 				BCD_LOG(buftemp,12,1);
				flashwrite(addr, buftemp, 12);//д��

				if (WHT_BANK_Save (FLASH_TEMP_START) != ST_OK)
				{
					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
					gsl8583Style.ISOK = 0;	//��ǩ��
					gmissflag= MISS_G_FREE;//
					GPRSLinkProcess =TCPSTARTSTAT;
					return 0;	//��������������

				}

				clrgsl8583filelist(SL8583FileFLAG_WHT);
				
				gsl8583Style.ISOK = 0;		//����ǩ����
				GPRSLinkProcess = TCPSTARTSTAT;
				
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));

			}

			
		}else
		{
			debugstring("ʲô������?\r\n");
		}


#endif		//endof qPBOC_BUS



	}

	else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_PRI, 3) == 0){//Ʊ����Ϣ�汾
//		���ա���������POS�ն˽���淶v2.00.docx�� �޶�ͨѶ��ʽ 


		Ascii2BCD(gsl8583FileDownPara.Miss_FileVer2,buftemp,4);
		if (memcmp(buftemp,pFistVary.LineNo,2) != 0)
		{
			MSG_LOG("��·�汾��һ��---\r\n");
			BCD_LOG(pFistVary.LineNo,3,1);
			BCD_LOG(buftemp,3,1);
			return 1;

		}

		Tack = gsl8583FileDownPara.Miss_offset;
		memcpy((unsigned char*)&addr, dfileinfo+5+8, 4);
		if(Tack != addr){	//���İ���ַ��һ��
			return 0;	//��������������
		}
		
		if (Tack == 0)//��һ����
		{
			addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
#ifdef debug_GJ_TLVDeal_
			debugstring("����ʱ������:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 256K
		}
		itemp = gsl8583FileDownPara.Miss_ALL_LEn;
		sprintf((char*)buftemp, "T:%d%% ",  (((Tack+datlen)*100)/itemp));
		miniDispstr(13, 18, (char*)buftemp, 0);
		
		addr = (FLASH_TEMP_START + Tack);//д��ĵ�ַ��
		if(addr > FLASH_TEMP_END){//��ַ�����˰���д�룬��������˰�����
			restore_flag = 1;
			return 0;
		}
		else{
			for(i =0;i<3;i++){
				flashwrite(addr, dfiledata, datlen);//д��
				flashread(addr, buftemp, datlen);
				if(memcmp(dfiledata, buftemp, datlen) == 0){
					break;
				}
				else{
#ifdef debug_GJ_TLVDeal_
					debugstring("����д�����\r\n");
					debugdata(dfiledata, datlen, 1);
					debugdata(buftemp, datlen, 1);
#endif
					continue;
				}
			}
			
			if(i >=3){
#ifdef debug_GJ_TLVDeal_
				debugstring("��ͷ��ʼ\r\n");
#endif
				addr = FLASH_TEMP_END;//д��ĵ�ַ ������ʼ��ַ
				
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
				initTCPIPnode();
#endif
			}
		}
		
		gsl8583FileDownPara.Miss_offset += datlen;
		if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
		{		
#warning: --��Ҫ�����������Ϣ�ļ�--

			
// 			flashread(FLASH_TEMP_START,buftemp,112);
// 
// 			MSG_LOG("PRI �������100%%\r\n");
// 			//set_line_GFT(1,buftemp);
// 			
// 			memcpy(NewPriceTab.lineVer,gsl8583FileDownPara.Miss_ver,2);
// 			over_turn(2,NewPriceTab.lineVer);
// 
// 			MSG_LOG("NewPriceTab.lineVer:");
// 			BCD_LOG(NewPriceTab.lineVer,2,1);
// 
// 
// 			getbusticketVer();
// 			
// 			savesysbupara();
// 			saveCpuInfo(1, buftemp);
// 			restore_flag = 3;//ˢ�½���
// 			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
// 			clrgsl8583filelist(SL8583FileFLAG_PRI);
//	gsl8583Style.ISOK = 0;		//����ǩ����
//			GPRSLinkProcess = TCPSTARTSTAT;
			gsl8583Style.ISOK = 0;	//��ǩ��
			gmissflag = MISS_G_FREE;//
			restore_flag = 3;//ˢ�½���
			memcpy(NewPriceTab.lineVer, gsl8583FileDownPara.Miss_ver, 2);//�汾�������ڵġ�
		//	over_turn(2,NewPriceTab.lineVer);

			MSG_LOG("NewPriceTab.lineVer:");
			BCD_LOG(NewPriceTab.lineVer, 2, 1);

			memcpy(gBusVerInfo.busLineVer, gsl8583FileDownPara.Miss_ver, 2);//�汾�������ڵġ�
			memcpy(gBusTicketV.busticketVer, gsl8583FileDownPara.Miss_ver, 2);//�°汾�������ڵ�

			checkNewPrice_new();
			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
			clrgsl8583filelist(SL8583FileFLAG_PRI);
		}

		

	}
	else if(memcmp(gsl8583FileDownPara.Miss_Fileflag, SL8583FileFLAG_PRO, 3) == 0){//���س���


		if (memcmp(gsl8583FileDownPara.Miss_FileVer2,POS_Cand_FLAG,3) != 0)
		{
			return 1;	//��Ҫ����
		}
		
		Tack = gsl8583FileDownPara.Miss_offset;
		memcpy((unsigned char*)&addr, dfileinfo+5+8, 4);
		over_turn(4, (unsigned char*)&addr);
		if(Tack != addr){	//���İ���ַ��һ��
			MSG_LOG("������ַ���ԣ�");
			BCD_LOG(dfileinfo+5+8, 4, 1);
			BCD_LOG((unsigned char*)&Tack, 4, 1);
			return 0;	//��������������
		}

		if (Tack == 0)//��һ����
		{
			addr = FLASH_PRO_START;//д��ĵ�ַ ������ʼ��ַ
			for(i=0; i<7; i++){
#ifdef debug_GJ_TLVDeal_
				debugstring("��մ�����:");
				debugdata((unsigned char*)&addr, 4, 1);
#endif
				flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 256K
				addr += 0x10000;//��һ��64K�Ŀ�
			}
			flasherase(ERASE4KBYTESECTOR, FLASH_PRO_FLAG);//�Ȳ��� ��־4k
		}
		itemp = gsl8583FileDownPara.Miss_ALL_LEn;
		sprintf((char*)buftemp, "P:%d%% ",  (((Tack+datlen)*100)/itemp));
		miniDispstr(13, 18, (char*)buftemp, 0);
		
		addr = (FLASH_PRO_START + Tack);//д��ĵ�ַ��
		if(addr > FLASH_PRO_END){//��ַ�����˰���д�룬��������˰�����
			restore_flag = 1;
			return 0;
		}
		else{
			for(i =0;i<3;i++){
				flashwrite(addr, dfiledata, datlen);//д��
				flashread(addr, buftemp, datlen);
				if(memcmp(dfiledata, buftemp, datlen) == 0){
					break;
				}
				else{
#ifdef debug_GJ_TLVDeal_
					debugstring("����д�����\r\n");
					debugdata(dfiledata, datlen, 1);
					debugdata(buftemp, datlen, 1);
#endif
					continue;
				}
			}
			
			if(i >=3){
#ifdef debug_GJ_TLVDeal_
				debugstring("��ͷ��ʼ\r\n");
#endif
				addr = FLASH_PRO_START;//д��ĵ�ַ ������ʼ��ַ
				
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
				initTCPIPnode();
#endif
			}
		}

		gsl8583FileDownPara.Miss_offset += datlen;
		if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
		{
			//���ͨ322������ֲ������
			crc32 = calFileCRC(FLASH_PRO_START,gsl8583FileDownPara.Miss_offset);
			MSG_LOG("file len:%d\r\n",gsl8583FileDownPara.Miss_offset);
			MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32,gsl8583FileDownPara.filecrc);
			if (crc32 != gsl8583FileDownPara.filecrc)
			{
				memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
				sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
				gsl8583Style.ISOK = 0;	//��ǩ��
				gmissflag= MISS_G_FREE;//
				GPRSLinkProcess =TCPSTARTSTAT;
				return 0;	//��������������
			}


			memset(buftemp, 0, 64);
			memcpy(buftemp,"������Ҫ����", 12);
			memcpy(buftemp+12,gsl8583FileDownPara.Miss_ver, 2);
			memcpy(buftemp+14, (unsigned char*)&SysTime, 7);
#ifdef debug_GJ_TLVDeal_
			debugstring("�����������:");
			debugdata(buftemp, 21, 1);
#endif
#ifdef _AREA_ENG_
			display(3,0,"Restart.....",DIS_CENTER);
#else
			display(3,0,"���,������...",DIS_CENTER);
#endif
			addr = FLASH_PRO_FLAG;
		
#ifdef debug_GJ_TLVDeal_
			debugstring("�����������2:");
			debugdata(buftemp, 34, 1);
#endif
			flashwrite(addr, buftemp, 64);
			
			clrgsl8583filelist(POS_Cand_FLAG);
			
			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
			sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
			beep(1,10,10);
			delayxms(400);
			ReStartCPU();//������ɣ������豸�Ա���³���
			while(1);
		}
		//------------------------------------------------
	}
	else{
		return 2;//��֪����ʲô.
	}

	sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);
	return 0;
}

/*
//������Ϣָ��
int sl8583ECHO_MSG_5001( unsigned char *dfiledata, unsigned int datlen)
{
	int i=0,msglen=0;
	int pos=0;
	BCD_LOG(dfiledata,datlen,1);
	stsl8583msg_ack msg_ack;
	unsigned char buffer[256];
	


	
	return ST_OK;
}

int sl8583ECHO6004(unsigned char *dfileinfo, unsigned char *dfiledata, unsigned int datlen)
{
	unsigned char buftemp[300];
	unsigned int addr;
	unsigned int Tack, i, itemp;


	if((memcmp(gsl8583FileDownPara.Miss_Fileflag, dfileinfo, 3) != 0)||
		(memcmp(gsl8583FileDownPara.Miss_ver, dfileinfo+2, 2) != 0)){//�汾���ļ�����Ҫһ��
		return 1;
	}

	Tack = gsl8583FileDownPara.Miss_offset;
	memcpy((unsigned char*)&addr, dfileinfo+5, 4);
	if(Tack != addr){	//���İ���ַ��һ��
		return 0;	//��������������
	}
	
	if (Tack == 0)//��һ����
	{
		addr = FLASH_TEMP_START;//д��ĵ�ַ ������ʼ��ַ
#ifdef debug_GJ_TLVDeal_
		debugstring("����ʱ������:");
		debugdata((unsigned char*)&addr, 4, 1);
#endif
		flasherase(ERASE64KBYTESECTOR, addr);//�Ȳ��� 256K
	}
	itemp = gsl8583FileDownPara.Miss_ALL_LEn;
	sprintf((char*)buftemp, "T:%d%% ",  ((Tack*100)/itemp));
	miniDispstr(13, 18, (char*)buftemp, 0);
	
	addr = (FLASH_TEMP_START + Tack);//д��ĵ�ַ��
	if(addr > FLASH_TEMP_END){//��ַ�����˰���д�룬��������˰�����
		restore_flag = 1;
		return 0;
	}
	else{
		for(i =0;i<3;i++){
			flashwrite(addr, dfiledata, datlen);//д��
			flashread(addr, buftemp, datlen);
			if(memcmp(dfiledata, buftemp, datlen) == 0){
				break;
			}
			else{
#ifdef debug_GJ_TLVDeal_
				debugstring("����д�����\r\n");
				debugdata(dfiledata, datlen, 1);
				debugdata(buftemp, datlen, 1);
#endif
				continue;
			}
		}
		
		if(i >=3){
#ifdef debug_GJ_TLVDeal_
			debugstring("��ͷ��ʼ\r\n");
#endif
			addr = FLASH_TEMP_END;//д��ĵ�ַ ������ʼ��ַ
			
			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#ifdef GPRS_MG323_
			initTCPIPnode();
#endif
		}
	}
	
	gsl8583FileDownPara.Miss_offset += datlen;
	if(gsl8583FileDownPara.Miss_offset >= gsl8583FileDownPara.Miss_ALL_LEn)//�������
	{		
		memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
#warning: --��Ҫ�����������Ϣ�ļ�--
		
		//clrgsl8583filelist(SL8583FileFLAG_PPP);
	}
	sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
	return 0;
}

*/
//
unsigned char GJDataDeal(unsigned char *pakege)
{
	struct ISO_8583 iso8583[128] = 
	{
		{  8, L_ASC, FIX_LEN},  //1
		{ 19, L_ASC, LLVAR_LEN}, //2
		{  6, L_BCD, FIX_LEN},	//3
		{  12, L_BCD, FIX_LEN},	//4
		{  8, L_BCD, FIX_LEN},	//5
		{  8, L_BCD, FIX_LEN},	//6
		{ 99, R_BCD, LLVAR_LEN},//7
		{  8, R_BCD, FIX_LEN},	//8
		{  8, R_BCD, FIX_LEN}, //9
		{  6, R_BCD, FIX_LEN}, //10
		{  8, R_BCD, FIX_LEN},	//11
		{  6, R_BCD, FIX_LEN},	//12
		{  8, R_BCD, FIX_LEN},	//13
		{  2, R_BCD, FIX_LEN},	//14
		{  4, R_BCD, FIX_LEN},	//15
		{  4, R_BCD, FIX_LEN},	//16
		{  4, R_BCD, FIX_LEN},	//17
		{  4, R_BCD, FIX_LEN},	//18
		{  4, R_BCD, FIX_LEN},	//19
		{  4, R_BCD, FIX_LEN},	//20
		{  4, R_BCD, FIX_LEN},	//21
		{  4, R_BCD, FIX_LEN}, 	//22
		{  4, R_BCD, FIX_LEN},	//23
		{  4, R_BCD, FIX_LEN},	//24
		{  2, R_BCD, FIX_LEN},	//25
		{  2, R_BCD, FIX_LEN},	//26
		{  99, L_BCD, LLVAR_LEN},	//27
		{  8, L_BCD, FIX_LEN},	//28
		{  99, L_BCD, LLVAR_LEN},	//29
		{  99, L_BCD, LLVAR_LEN},	//30
		{  99, L_BCD, LLVAR_LEN},	//31
		{  11, L_BCD, LLVAR_LEN},	//32
		{  99, L_BCD, LLVAR_LEN},	//33
		{ 999, L_BCD, LLLVAR_LEN},	//34
		{ 37,  L_BCD, LLVAR_LEN},	//35
		{104,  L_BCD, LLLVAR_LEN},	//36
		{ 12,  L_ASC, FIX_LEN},		//37
		{  999,  L_ASC, LLLVAR_LEN},//38
		{  4,  L_BCD, FIX_LEN},		//39
		{ 99,  L_ASC, LLVAR_LEN},	//40
		{  8,  L_BCD, FIX_LEN},		//41
		{ 8,  L_BCD, FIX_LEN},		//42
		{ 8,  L_ASC, FIX_LEN},	//43
		{ 8,  L_ASC, FIX_LEN},	//44
		{ 999,  L_BCD, LLLVAR_LEN},	//45
		{999, R_BCD, LLLVAR_LEN},	//46
		{13, L_ASC, FIX_LEN},	//47   �����ļ���Ϣ  V2��0 ����
		{999, R_BCD, LLLVAR_LEN},	//48
		{  2, R_ASC, FIX_LEN},		//49
		{ 99,  L_BCD, LLVAR_LEN},	//50	 V2��0 �¼�
		{  999, L_BCD, LLLVAR_LEN},	//51		V2��0 �¼�
		{999, L_ASC, LLLVAR_LEN},	//52
		{ 16, R_ASC, FIX_LEN},		//53
		{999, R_ASC, LLLVAR_LEN},	//54
		{  99, L_BCD, LLLVAR_LEN},	//55		V2��0 �¼� �����·�
		{999, R_BCD, LLLVAR_LEN},	//56	V2��0 �¼� �����·�
		{999, R_BCD, LLLVAR_LEN},	//57
		{999, R_BCD, LLLVAR_LEN},	//58
		{999, R_BCD, LLLVAR_LEN},	//59
		{999, R_ASC, LLLVAR_LEN},	//60
		{999, L_ASC, LLVAR_LEN},	//61
		{999, R_BCD, LLLVAR_LEN},	//62
		{999, R_ASC, LLLVAR_LEN},	//63
		{ 8, R_BCD, FIX_LEN},		//64
		{  8, R_BCD, FIX_LEN},
		{  1, R_ASC, FIX_LEN},
		{  2, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  4, R_ASC, FIX_LEN},
		{  4, R_ASC, FIX_LEN},
		{  6, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 42, R_ASC, FIX_LEN},
		{  1, R_BCD, FIX_LEN},
		{  2, R_BCD, FIX_LEN},
		{  5, R_BCD, FIX_LEN},
		{  7, R_BCD, FIX_LEN},
		{ 42, R_BCD, FIX_LEN},
		{  8, R_BCD, FIX_LEN},
		{ 16, R_BCD, FIX_LEN},
		{ 25, R_BCD, FIX_LEN},
		{ 11, R_ASC, LLVAR_LEN},
		{ 11, R_ASC, LLVAR_LEN},
		{ 17, R_BCD, LLVAR_LEN},
		{ 28, R_BCD, LLVAR_LEN},
		{ 28, R_BCD, LLVAR_LEN},
		{10,  R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{  8, R_BCD, FIX_LEN}
	};

	unsigned short stemp;
	unsigned short sAlen=0, Alen;//�յ����ܵİ��Ĵ�С
	unsigned short msgCmd=0;//��Ϣ����
	unsigned short dealCode=0;//���״�����
	unsigned short sfi;
	unsigned int lc;
	unsigned char getbuff[256];
	

	unsigned char *msgpoint;
	int ret=0, i=0,len=0;
//	unsigned char rdt[7];
	DEF8583Head *sl8583head;

	unsigned short nestpLen;//�������ĳ���
	unsigned char *nestpP;//�Ƿ����ذ�������о�ָ����һ����
//	unsigned char MSGerrflag=0;
	data_element_flag msgf[129];	//128����ķֽ����.

	stsl8583msg_ack  msg_ack;		//��Ϣȷ��ָ�� V2.0��
	stsl8583msg_down *msg_down;
	unsigned short msg_type=0;
	unsigned int templen=0;
//	unsigned int temp=0;

	stFAT_hisRec *hismsg;		//

	nestpP = 0;
	nestpLen = 0;

	clr_wdt();


	for(i=0; i<129; i++){	//��ʼ��
		msgf[i].bitf = 0;
		msgf[i].len = 0;
	}

	memcpy((unsigned char*)&Alen, pakege+1, 2);//�������ܳ���
	//	unsigned char msgtype;
	memcpy((unsigned char*)&sAlen, pakege+3, 2);//8583���ӵ�3�ֽڿ�ʼ��֡�ܳ��ȣ�//ǰ�����ֽ��� ֡���ͺ�֡���ȣ��������޹�
	over_turn(2, (unsigned char*)&sAlen);
#ifdef _debug_
	debugstring("deal sl8583 CMD!\r\n");
	debugstring("Alen:");
	debugdata((unsigned char *)&Alen, 2, 1);
	debugstring("sAlen:");
	debugdata((unsigned char *)&sAlen, 2, 1);
	debugdata(pakege, Alen, 1);
#endif
	
	if((sAlen+2) < Alen){//������������
		nestpP = pakege+sAlen+2;
		nestpLen = (Alen +3 - sAlen - 2);

		MSG_LOG("  ������һ����::");
		BCD_LOG(nestpP, nestpLen, 1);
	}
	
	sl8583head = (DEF8583Head*)(pakege+3);//ָ����Ϣ���� TPDU 5�ֽ�

	MSG_LOG("sl8583head:");
	BCD_LOG(&sl8583head->ID,11,1);


	if(sl8583head->ID != 0x60){
		MSG_LOG("TPDU ID error:%02X\r\n", sl8583head->ID);
		return 0;
	}
	if(memcmp(sl8583head->desAddr, "\x00\x00", 2) != 0){
		MSG_LOG("TPDU DESAddr error:%02X%02X\r\n", sl8583head->desAddr[0], sl8583head->desAddr[1]);
		return 0;
	}
	if(memcmp(sl8583head->sourceAddr, "\x00\x00", 2) != 0){
		MSG_LOG("TPDU DESAddr error:%02X%02X\r\n", sl8583head->sourceAddr[0], sl8583head->sourceAddr[1]);
		return 0;
	}

	MSG_LOG("dealType:%02X\r\n",sl8583head->posStatedealType&0x0F);

	if((sl8583head->posStatedealType&0x0F) == 0x03){//����Ҫ����Ҫ����ǩ��

		MSG_LOG("��̨Ҫ������ǩ��--\r\n");
		gsl8583Style.ISOK = 0;			//������ٴ����˳�����ǩ��
		
		gmissflag = MISS_G_FREE;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		return 0;
	}
	else if ((sl8583head->posStatedealType&0x0F) == 0x05)
	{
		
		if (gmissflag != MISS_G_ALAM)//��̨���ʱ�򲻻᷵����������룬����Ϊ�˱��գ����ϣ���ֹ�ٽ�������ѭ��
		{
			MSG_LOG("��̨����Ϣ---\r\n");
			gmissflag = MISS_G_ALAM;	//ȡ��Ϣ
			if(GPRSLinkProcess == GPRS_SENDING_CMD)
				GPRSLinkProcess = TCPSTARTSTAT;
			if (gsl8583Style.ISOK != 0)		//ûǩ��������
			{
				return 0;
			}
			
		}

	}
	
	memcpy((unsigned char*)&msgCmd, sl8583head->MSGtype, 2);//�˰���Ϣ����
	over_turn(2, (unsigned char *)&msgCmd);
	memcpy((unsigned char *)&dealCode, sl8583head->MSGdealCode+1, 2);//���״���������ֽ�
	over_turn(2, (unsigned char *)&dealCode);
	
	MSG_LOG("bitmap:");
	BCD_LOG(sl8583head->MSGbitmap, 16, 1);
	MSG_LOG("dealCode:");
	BCD_LOG( sl8583head->MSGdealCode, 3, 1);
	MSG_LOG("msgCmd:%04X\r\n",msgCmd);


	msgpoint = sl8583head->MSGdealCode;	//Ӧ������ָ�룬�ӽ��״�����
	msgpoint += 3;	//������ǰ���Ѿ�ȡ�������ٴ���
//**********����8583��******************************************************
//��������ֵ�msgf�С�
	for(sfi=4; sfi<=64; sfi++){//�ӵ�4��ʼ��0~63��

		if(getBitmapBits(sfi, sl8583head->MSGbitmap) != ST_OK){
			continue;//��λ�����ڣ�������һλ
		}

		lc = iso8583[sfi-1].len;
		if(iso8583[sfi-1].type == L_BCD )	/*�����BCD��*/
		{
			lc=(lc+1)/2;
		}
		else if( iso8583[sfi-1].type == L_ASC )  /*�����ASC*/
		{
			lc = lc;
		}
		else if( iso8583[sfi-1].type == R_BCD )	/*�Ҷ���BCD��*/
		{
			lc=(lc+1)/2;
		}
		else if( iso8583[sfi-1].type == R_ASC )	/*�Ҷ���ASC��*/
		{
			lc = lc;
		}

		if(iso8583[sfi-1].flag == FIX_LEN)/*�̶�����*/
		{
			;
		}
		else if(iso8583[sfi-1].flag == LLVAR_LEN)/*��λ�䳤 1Byte*/
		{
			lc = 0;
			lc = (((msgpoint[0]>>4) * 10)+(msgpoint[0]&0x0f));
//			memcpy((unsigned char *)&lc, pakege+index, 2);//
//			over_turn(2, (unsigned char*)&lc);
			msgpoint++;
		}
		else/*��λ�䳤*/
		{
			lc = 0;
			lc = (((msgpoint[0]>>4) * 10)+(msgpoint[0]&0x0f));
			stemp = (((msgpoint[1]>>4) * 10)+(msgpoint[1]&0x0f));
			lc *= 100;
			lc += stemp;
			msgpoint += 2;
		}
//		MSG_LOG("field lc2:%d\r\n", lc);

		if(lc >= 1000){//���ȴ���
			MSG_LOG("--Fild:%d,len:%d,LEN ERROR!!!\r\n", sfi, lc);
			goto GJ8583dataDealEnd;
		}
//		memcpy(getbuff, msgpoint, lc);
		msgf[sfi].bitf = 1;
		msgf[sfi].len = lc;
		msgf[sfi].dbuf_addr = msgpoint-pakege;

#ifdef debug_GJ_TLVDeal_
		MSG_LOG("--Fild:%d,len:%d,dat:", sfi, lc);
		BCD_LOG(pakege+msgf[sfi].dbuf_addr, lc, 1);//��ӡ���Ǵ��������
#endif

		msgpoint += lc;
		
#ifdef debug_GJ_TLVDeal_
//		BCD_LOG(msgpoint, 16, 1);//��ӡ���Ǵ��������
#endif

	}
//**************************************************************************************
	if(msgf[field_ack].bitf ==  0){	//û��Ӧ���룬�˰�������
		MSG_LOG("û��Ӧ���롣\r\n");
		goto GJ8583dataDealEnd;	//û��Ӧ����
	}
	if(memcmp(pakege+msgf[field_ack].dbuf_addr, "\x00\x00", 2) != 0){//Ӧ���벻��
		if(msgf[field_errmsg].bitf == 1){//�д�����Ϣ
			memcpy(getbuff, pakege+msgf[field_errmsg].dbuf_addr, msgf[field_errmsg].len);
			getbuff[msgf[field_errmsg].len] = 0;
			MessageBox(1, (char*)getbuff);
			MSG_LOG("msg38:%s\r\n",getbuff);
			delayxms(1000);
			restore_flag = 3;
		}
		sprintf((char*)getbuff, "A:%02X%02X", pakege+msgf[field_ack].dbuf_addr, pakege[msgf[field_ack].dbuf_addr+1]);
		miniDispstr(13, 18, (char*)getbuff, 0);

		MSG_LOG((char*)getbuff);
		memcpy((unsigned char*)&stemp, pakege+msgf[field_ack].dbuf_addr, 2);
		over_turn(2, (unsigned char*)&stemp);
		switch(stemp)
		{
		case 0x0004:	//��Ҫ��ƽ̨ǩ��
		case 0xA001:	//MACУ���
		case 0xA002:	//ǿ��ǩ��
			MSG_LOG("������Ҫ����ǩ��:%04X\r\n", stemp);
			gmissflag = MISS_G_FREE;
			if(GPRSLinkProcess == GPRS_SENDING_CMD)
				GPRSLinkProcess = TCPSTARTSTAT;
			gsl8583Style.ISOK = 0;
			break;

		case 0x0001:	//��Ч�̻�	���̻��ں������л򲻴���
		case 0x0002:	//�ն˺��Ҳ���,ƽ̨û�еǼ��ն˺�
		case 0x0003:	//���ĸ�ʽ����
			break;
		case 0x0006:
			MSG_LOG("�����ļ�������!\r\n");
			//	memset((unsigned char *)gsl8583filelist,0,sizeof(stsl8583filelist)*sl8583fileNum);
			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
			sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������0
			gmissflag=MISS_G_FREE;
			if(GPRSLinkProcess==GPRS_SENDING_CMD)
				GPRSLinkProcess=TCPSTARTSTAT;
		//	gsl8583Style.ISOK=0;
			break;
		default:
			break;
		}
		
		if(gmissflag == MISS_G_FILES){//��������ļ���������ֹ
			clrgsl8583filelist(gsl8583FileDownPara.Miss_Fileflag);
			memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
			sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//�����������
		}
		goto GJ8583dataDealEnd;
	}

	if((msgf[field_date].bitf)&&(msgf[field_time].bitf)){//��ʱ������
		memcpy(getbuff, pakege+msgf[field_date].dbuf_addr, 4);
		memcpy(getbuff+4, pakege+msgf[field_time].dbuf_addr, 3);
		if(time_invalid(getbuff) == 1){//ʱ����Ч��Уʱ
			GetDateTime();
			//if(memcmp((unsigned char*)&SysTime.year_h, getbuff, 5) != 0){//���Ӳ�һ�£�Уʱ
			ret = BCDTime2Long(getbuff);
			i = BCDTime2Long(&SysTime.year_h);
			ret -=i;
			if( ret >  10|| ret <-10)
			{
				MSG_LOG("Уʱ---\r\n");
				timewrite(getbuff);
				//���ʱ�䲻�ǽ��죬���Ҫ���´򿪻����½�һ���ļ������ﶼ��һ��
// #ifdef _FASH_FATs_SYS_
// 				close_rfile_Wrecord();
// 				open_rfile_Wrecord();
// #endif

			}
// 			if (ret >86400 || ret <-86400)		//������
// 			{
// 				MSG_LOG("Уʱ��1��\r\n");
// 				ReStartCPU();
// 			}

			//}
		}
	}

	if(msgf[field_posserial].bitf){//��Ψһ��
		Get_SerialNumF4(getbuff);
		if(memcmp(getbuff, pakege+msgf[field_posserial].dbuf_addr, 4) != 0){//������̨�豸��Ӧ��
			MSG_LOG("POS serial Error:\r\n");
			BCD_LOG(getbuff, 4, 1);
			BCD_LOG(pakege+msgf[field_posserial].dbuf_addr, 4, 1);
			goto GJ8583dataDealEnd;
		}
	}
	else {//û��Ψһ���´�������
		MSG_LOG("No 41fild POS serial no exist!\r\n");
		goto GJ8583dataDealEnd;
	}

	MSG_LOG(" gj1.. ");
	//���涼�ǳɹ���.

	gSendGLogin = 0;//��Ӧ�𣬲���������

	MSG_LOG("dealcode:%04X\r\n",dealCode);

	switch(dealCode){
	case 0x6001://ǩ���Ļ�Ӧ
		if(msgCmd != 0x0810){//��Ϣ���Ͳ���
			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
			break;
		}
		if(msgf[field_cmdcode].bitf == 0){//����Ҫ�з�������
			break;
		}
		memset(gsl8583Style.gcmdcode, 0, 100);
		if(msgf[field_cmdcode].len >= 100)
			lc = 99;
		else
			lc = msgf[field_cmdcode].len;	//��������
		gsl8583Style.gcmdcode[0] = lc;
		memcpy(gsl8583Style.gcmdcode+1, pakege+msgf[field_cmdcode].dbuf_addr, lc);

		if(msgf[field_kek].bitf == 0){//û��KEK��
			break;
		}
		memcpy(gsl8583Style.MAC_KEY, pakege+msgf[field_kek].dbuf_addr, 16);//KEK
		gsl8583Style.crc_mackey = __cpuCrc32(gsl8583Style.MAC_KEY, 16);	//����MAC KEYУ��
		if(msgf[field_severfilelist].bitf != 0){//���ļ��汾
			dealsl8583filelist(pakege+msgf[field_severfilelist].dbuf_addr, msgf[field_severfilelist].len);
		}
		
// 		if(msgf[field_severparaver].bitf != 0){//��ƽ̨�豸�����汾,�������һ�� 
// 			memcpy(gsl8583filelist[0].filename, SL8583FileFLAG_PPP, 3);//
// 			memcpy((unsigned char*)&gsl8583filelist[0].fileVer, pakege+msgf[field_severparaver].dbuf_addr, 2);//
// 			memcpy((unsigned char*)&gsl8583filelist[0].filelen, pakege+msgf[field_severparaver].dbuf_addr+2, 4);//
// 			over_turn(4, (unsigned char*)&gsl8583filelist[0].filelen);//ƽ̨�·����Ǹ�λ��ǰ��
// 		}

		MSG_LOG("��½�ɹ�:KEK:");
		BCD_LOG(gsl8583Style.MAC_KEY, 16, 1);
#ifdef _debug_
		for (i=0;i< sl8583fileNum;i++)
		{

			MSG_LOG("%s %s,VER:0x%04X\r\n",gsl8583filelist[i].filename,gsl8583filelist[i].fileVer2,gsl8583filelist[i].fileVer);
		}

#endif
		MSG_LOG("��½�ɹ�:KEKCRC:%08X\r\n", gsl8583Style.crc_mackey);
		MSG_LOG("  :����%d:", gsl8583Style.gcmdcode[0]);
		BCD_LOG(gsl8583Style.gcmdcode+1, gsl8583Style.gcmdcode[0], 1);
		
		gsl8583Style.ISOK = 'O';	//��ǩ���ɹ���ʶ
		gmissflag = MISS_G_FREE;
		memcpy(getbuff, pakege+msgf[field_date].dbuf_addr, 4);
		memcpy(getbuff+4, pakege+msgf[field_time].dbuf_addr, 3);
		
		if(time_invalid(getbuff) == 1){//ʱ����Ч��Уʱ
			MSG_LOG("ǩ��Уʱ---\r\n");
			timewrite(getbuff);
// #ifdef _FASH_FATs_SYS_
// 			//���ʱ�䲻�ǽ��죬���Ҫ���´򿪻����½�һ���ļ������ﶼ��һ��
// 			close_rfile_Wrecord();
// 			open_rfile_Wrecord();
// #endif
			
		}
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x6002://�ն�ǩ�˻�Ӧ	
		if(msgCmd != 0x0810){//��Ϣ���Ͳ���
			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
			break;
		}	
		sl8583ECHO6002();
		gmissflag = MISS_G_FREE;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x6003://�ļ����»�Ӧ
		if(msgCmd != 0x0850){//��Ϣ���Ͳ���
			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
			break;
		}
		if((msgf[field_severparaver].bitf == 0)||(msgf[field_filedata].bitf == 0))
		{
			break;	//����������
		}
		if(sl8583ECHO6003(pakege+msgf[field_severparaver].dbuf_addr, pakege+msgf[field_filedata].dbuf_addr, msgf[field_filedata].len) != 0)	
			break;
		
		gmissflag = MISS_G_FREE;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
// 	case 0x6004://�豸������·Ʊ�۸��»�Ӧ
// 		if(msgCmd != 0x0870){//��Ϣ���Ͳ���
// 			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
// 			break;
// 		}
// 		if((msgf[field_POSpara].bitf == 0)||(msgf[field_ackpospara].bitf == 0))
// 		{
// 			break;	//����������,û���·������豸������Ϣ51��
// 		}
// 		if(sl8583ECHO6004(pakege+msgf[field_POSpara].dbuf_addr, pakege+msgf[field_ackpospara].dbuf_addr, msgf[field_ackpospara].len) != 0)	
// 			break;
// 
// 		gmissflag = MISS_G_FREE;
// 		if(GPRSLinkProcess == GPRS_SENDING_CMD)
// 			GPRSLinkProcess = TCPSTARTSTAT;
// 		break;


	case 0x7001://�����ϴ���Ӧ
		if(msgCmd != 0x0310){//��Ϣ���Ͳ���
			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
			break;
		}
		if((msgf[field_recAdd].bitf == 0)||(msgf[field_recupnum].bitf == 0))//�ϴ����Σ��ϴ������ݵ�ַ�� �� ����һ��Ҫ�С�
			break;


		BCD2Ascii(pakege+msgf[field_recAdd].dbuf_addr, getbuff, 4);
		getbuff[8] = 0;

		MSG_LOG("Recaddr:");
		BCD_LOG(pakege+msgf[field_recAdd].dbuf_addr,4,1);

		memcpy(&i,pakege+msgf[field_recAdd].dbuf_addr,4);
		//i = __atol((char*)getbuff);	//���͵����Σ�Ҳ������ʼ��ַ����BCDת��HEX

		ret = pakege[msgf[field_recupnum].dbuf_addr];	//�ϴ�������,1���ֽ�HEX
		
		sl8583delECHO7001(i, ret);

		gmissflag = MISS_G_FREE;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;

	case 0x2001:		//����

#ifdef debug_GJ_TLVDeal_
		debugstring("��������:");
#endif
		if(msgCmd != 0x0890){//��Ϣ���Ͳ���
			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", dealCode);
			break;
		}


		//�����Ѿ�����
// 		if((msgf[field_date].bitf)&&(msgf[field_time].bitf)){//��ʱ������
// 			memcpy(getbuff, pakege+msgf[field_date].dbuf_addr, 4);
// 			memcpy(getbuff+4, pakege+msgf[field_time].dbuf_addr, 3);
// 			if(time_invalid(getbuff) == 1){//ʱ����Ч��Уʱ
// 				GetDateTime();
// 				if(memcmp((unsigned char*)&SysTime.year_h, getbuff, 5) != 0){//���Ӳ�һ�£�Уʱ
// 					timewrite(getbuff);
// 				}
// 			}
// 		}

		gmissflag = MISS_G_FREE;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;

	case 0x5001:		//�����Ϣ
		if(msgCmd != 0x0510){//��Ϣ���Ͳ���
			MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
			break;
		}

		if((msgf[field_POSpara].bitf == 0))
		{
			break;	//����������
		}

		
		
		if (msgf[field_POSpara].len < 10)
		{
			MSG_LOG("���Ȳ���--\r\n");
			break;
		}

		msg_down = (stsl8583msg_down *)(pakege+msgf[field_POSpara].dbuf_addr);

		
		templen = BCD2int(msg_down->msg_len,2);	//��ֻ����һ���ɡ����鷳����
		
		//	pos = BCD2int(dfiledata,2);
		

		templen = BCD2int(msg_down->msg_len,2);
		
		MSG_LOG("msglen:%d\r\n",templen);

		memset(&msg_ack,0,sizeof(stsl8583msg_down));

		memcpy(msg_ack.msg_type,msg_down->msg_type,2);
		MSG_LOG("msg_type:");
		BCD_LOG(msg_ack.msg_type,2,1);
		memcpy(msg_ack.msg_id,msg_down->msg_id,4);
		MSG_LOG("msgid:");
		BCD_LOG(msg_ack.msg_id,4,1);
		MSG_LOG("msg_down:");
		BCD_LOG(msg_down->msg_buff,templen-6,1);
		// gj1.. msglen:14
		//msg_type:0001
		// msgid:00000007
		// msg_down:2017010120170102

		msg_type  = 0;
		memcpy(&msg_type,msg_down->msg_type,2);
		over_turn(2,(unsigned char *)&msg_type);
		switch(msg_type)
		{
		case 1://	���ɼ� bcd


			memcpy(getbuff,msg_down->msg_buff,4);
			memcpy(getbuff+4,"\x01\x59\x59",3);
			
			if(time_invalid(getbuff) != 1){
				break;
			}

			memcpy(getbuff,msg_down->msg_buff+4,4);
			memcpy(getbuff+4,"\x01\x59\x59",3);
			
			if(time_invalid(getbuff) != 1){
				break;
			}
			
			
			hismsg = (stFAT_hisRec *)getbuff;

			memcpy(hismsg->startdate,msg_down->msg_buff,4);
			memcpy(hismsg->enddate,msg_down->msg_buff+4,4);
			memcpy(hismsg->sfilename,msg_down->msg_buff,4);
			hismsg->offset = 0;
			hismsg->filelen = 0;
			hismsg->scrc32 = __cpuCrc32((unsigned char *)hismsg, BIT_REORD_HIS_LEN-4);
			sysfewrite(BIT_REORD_HIS,BIT_REORD_HIS_LEN,getbuff);

#ifdef debug_GJ_TLVDeal_
			MSG_LOG("д���ɼ�:");
			memset(getbuff,0,30);
			sysferead(BIT_REORD_HIS,BIT_REORD_HIS_LEN,getbuff);
			BCD_LOG(getbuff,BIT_REORD_HIS_LEN,1);
#endif

						

			break;
		case 2://�����̻�	bcd
			
			if (memcmp(msg_down->msg_buff+2,"\x00\x00",2) != 0)
			{

				saveCpuInfo(7, msg_down->msg_buff+2);//�̻���  00009998
			}

			break;
		case 3:	//������· asc

			Ascii2BCD(msg_down->msg_buff,getbuff,6);

			memcpy(pFistVary.LineNo,getbuff,3);
			sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//�տ�ʼʱ������·������·һ��
			
			memset(NewPriceTab.lineVer ,0,2);
			
			savesysbupara();
			saveCpuInfo(1, getbuff);


			break;
		case 4: //���ƺ� asc
			memcpy(pFistVary.DeviceNo,msg_down->msg_buff,7);
			sysSaveDevice(msg_down->msg_buff);

			break;
		default:
			break;
		}




	
		memset(msg_ack.msg_ack_code , 0,2);	//��ȷ
		memset(msg_ack.msg_buff,0,sizeof(msg_ack.msg_buff));
		memset(getbuff,0,sizeof(getbuff));
		templen = 0;
		memcpy(msg_ack.msg_len , &templen,2);



		getbuff[len++] = 0;	//ǰ����ǰ�����ȡ�
		getbuff[len++] = 0;	//ǰ����ǰ�����ȡ�
		
		templen = Build8583TPDUHEAD(getbuff+len);
		len += templen;
		//Ӧ�ÿ��Է���ȥ��
		templen = build8583_MSGACK_5002(getbuff+len,&msg_ack);
		len +=templen ;

		getbuff[0] = (len-2)/256;
		getbuff[1] = (len-2)%256;//��ǰ������ܳ���

		
		restore_flag = 3;
		gsl8583Style.ISOK = 0;		//����ǩ��
		gmissflag = MISS_G_FREE;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;

		MSG_LOG("Send ACK:");
		BCD_LOG(getbuff,len,1);
		gprs_send_data(1, len, getbuff);


		
		break;




		case 0x5003:		//�����·�
			if(msgCmd != 0x0550){//��Ϣ���Ͳ���
				MSG_LOG("��Ϣ���Ͳ�һ��%04X\r\n", msgCmd);
				break;
			}
			
			if((msgf[field_add_file_rev].bitf == 0))
			{
				break;	//����������
			}
			
			
			
			if (msgf[field_add_file_rev].len < 14)//��������(2B)+ƽ̨�汾(4B)+��ǰ�汾(4B)+��ǰ��С(4B) ����
			{
				break;
			}



			memcpy(&lc,pakege+msgf[field_add_file_rev].dbuf_addr+2,4);//�͵�ǰ�����汾һ��
			over_turn(4, (unsigned char *)&lc);
			MSG_LOG("QRBLKVer:%u,%u\r\n",lc,get_QR_blkVer());

			if (lc == get_QR_blkVer())
			{
				gmissflag = MISS_G_FREE;
				if(GPRSLinkProcess == GPRS_SENDING_CMD)
					GPRSLinkProcess = TCPSTARTSTAT;
				MSG_LOG("һ��,������\r\n");
				break;
			}



			
			
			sl8583ECHO5003(pakege+msgf[field_add_file_rev].dbuf_addr, msgf[field_add_file_rev].len);

			memcpy(&templen,pakege+msgf[field_add_file_rev].dbuf_addr+2,4);
			over_turn(4, (unsigned char *)&templen);
			memcpy(&lc,pakege+msgf[field_add_file_rev].dbuf_addr+2+4,4);//
			over_turn(4, (unsigned char *)&lc);

			if (  templen ==lc)// ���ߴ�������ƽ̨idһ��
			{

				MSG_LOG("QR������:VER:%u--\r\n",get_QR_blkVer());
				restore_flag = 3;
				gmissflag = MISS_G_FREE;
				if(GPRSLinkProcess == GPRS_SENDING_CMD)
					GPRSLinkProcess = TCPSTARTSTAT;

			}


			if(GPRSLinkProcess == GPRS_SENDING_CMD)
				GPRSLinkProcess = TCPSTARTSTAT;

			break;

		

		
	default:
		MSG_LOG("��֪��ʲô����:%04X\r\n",dealCode);
		break;
	}

	clr_wdt();
GJ8583dataDealEnd:
	MSG_LOG(" gjend..%d ", nestpLen);
	if((nestpP != 0) && (nestpLen > 0))//���ذ�����Ҫ�Ƶ�ǰ��
	{
		pakege[0] = 0x01;//��������
		memcpy(pakege+1, (unsigned char*)&nestpLen, 2);//ʣ��ĳ���
		memmove(pakege+3, nestpP, nestpLen);
		return 1;
	}
	else
		return 0;
}








