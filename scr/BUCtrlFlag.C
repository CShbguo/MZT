
//---------后台控制车载机功能函数------------------
//	2013-10-8  KSLI
//-------------------------------------------------

#include "stdio.h"
#include "string.h"
#include "szct.h"

#ifdef Center_Ctrl_BU_ENABEL
#include "..\include\include.h"

unsigned char gBuCtrlFlag;

void BUCtrl_Save_Flag(unsigned char cflag)
{
	unsigned char bcsbuf[16];
	unsigned int bcsCrc32;

	MSG_LOG("gBuCtrlFlag:%02X, cflag:%02X", gBuCtrlFlag, cflag);

	if(gBuCtrlFlag == cflag){//标识一致，不用保存
		return;
	}
	sysferead(BIT_Center_Ctrl_BU_Data, 8, bcsbuf);
	bcsCrc32 = __cpuCrc32(bcsbuf, 4);
	if((memcmp((unsigned char*)&bcsCrc32, bcsbuf+4, 4) != 0)||
		(memcmp(bcsbuf, "启", 2) != 0))
	{//校验错，无效
		memset(bcsbuf, 0, 8);
	}
	
	MSG_LOG("bcsbuf[3]:%02X", bcsbuf[3]);

	memcpy(bcsbuf, "启", 2);
	bcsbuf[2] = 0;
	bcsbuf[3] = cflag;
	bcsCrc32 = __cpuCrc32(bcsbuf, 4);
	memcpy(bcsbuf, (unsigned char*)&bcsCrc32, 4);

	sysfewrite(BIT_Center_Ctrl_BU_Data, 8, bcsbuf);
	gBuCtrlFlag = cflag;
	MSG_LOG("save END gBuCtrlFlag:%02X", gBuCtrlFlag);
}

void BUCtrl_Get_Flag(void)
{
	unsigned char bcgbuf[16];
	unsigned int bcgCrc32;
	
	sysferead(BIT_Center_Ctrl_BU_Data, 8, bcgbuf);
	bcgCrc32 = __cpuCrc32(bcgbuf, 4);
	if((memcmp((unsigned char*)&bcgCrc32, bcgbuf+4, 4) != 0)||
		(memcmp(bcgbuf, "启", 2) != 0))
	{//校验错，无效
		memset(bcgbuf, 0, 8);
	}
	gBuCtrlFlag = bcgbuf[3];
	
	MSG_LOG("Get END gBuCtrlFlag:%02X", gBuCtrlFlag);
}

void BUCtrl_Dis_onScreen(unsigned char x, unsigned char y, unsigned char mode)
{
	char BuDdisbuf[16];

	sprintf(BuDdisbuf, "%02X", gBuCtrlFlag);

	if(mode){
		miniDispstr(x, y, BuDdisbuf, 0);
	}
	else{
		display(x, y, BuDdisbuf, 0);
	}
}
#endif




