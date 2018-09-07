#ifndef _CITYFILE_HEAD_
#define _CITYFILE_HEAD_

//定义各城市的自定义函数

#include "../szct.h"

#ifdef BUC_FuZhou_
	#include "FuZhou.h"
#elif defined BUS_GUANGXI
	#include "GuangXi.h"
#elif defined BUS_GUILIN_
	#include "GuiLin.h"
#elif defined BUS_MZT
	#include "Mzt.h"
#else
	#include "SLZR_TEST.h"	
	
#endif

#endif
