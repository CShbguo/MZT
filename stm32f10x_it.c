/******************** (C) COPYRIGHT 2009 Embest Info&Tech Co.,LTD. *************
* File Name          : stm32f10x_it.c
* Author             : SLZR-TECH
* Date First Issued  : 07/12/2010
* Description        : Main Interrupt Service Routines.
*             		   This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
********************************************************************************
* History:
* 07/12/2009		 : V1.0		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "..\include\include.h"
#include "..\scr\szct.h"
#include "..\scr\bluetooth.h"

#define COM1 1
#define COM2 2
#define COM3 3
#define COM4 4
#define COM5 5	
extern void debugstringCOM(unsigned char com, char *str);
extern void debugdataCOM(unsigned char com, unsigned char *value, unsigned int uclen, unsigned char mode);

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup FLASH_Program
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  debugstring("HardFault_Handler");
//	  return;
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
	  debugstring("MemManage_Handler");
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
	  debugstring("BusFault_Handler");
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
	  debugstring("UsageFault_Handler");
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	  debugstring("SVC_Handler");
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
	  debugstring("DebugMon_Handler");
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	  debugstring("PendSV_Handler");
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)	 在库函数里处理
//{
//	  debugstring("SysTick_Handler");
//}

// External Interrupts
//void  WWDG_IRQHandler           // Window Watchdog
//void  PVD_IRQHandler            // PVD through EXTI Line detect
//void  TAMPER_IRQHandler         // Tamper
//void  RTC_IRQHandler            // RTC
//void  FLASH_IRQHandler          // Flash
//void  RCC_IRQHandler            // RCC
// void  EXTI0_IRQHandler          // EXTI Line 0
// void  EXTI1_IRQHandler          // EXTI Line 1
// void  EXTI2_IRQHandler          // EXTI Line 2
// void  EXTI3_IRQHandler          // EXTI Line 3
// void  EXTI4_IRQHandler          // EXTI Line 4
// void  DMA1_Channel1_IRQHandler  // DMA1 Channel 1
// void  DMA1_Channel2_IRQHandler  // DMA1 Channel 2
// void  DMA1_Channel3_IRQHandler  // DMA1 Channel 3
// void  DMA1_Channel4_IRQHandler  // DMA1 Channel 4
// void  DMA1_Channel5_IRQHandler  // DMA1 Channel 5
// void  DMA1_Channel6_IRQHandler  // DMA1 Channel 6
// void  DMA1_Channel7_IRQHandler  // DMA1 Channel 7
// void  ADC1_2_IRQHandler         // ADC1 & ADC2
// void  USB_HP_CAN1_TX_IRQHandler  // USB High Priority or CAN1 TX
// void  USB_LP_CAN1_RX0_IRQHandler // USB Low  Priority or CAN1 RX0
// void  CAN1_RX1_IRQHandler       // CAN1 RX1
// void  CAN1_SCE_IRQHandler       // CAN1 SCE
// void  EXTI9_5_IRQHandler        // EXTI Line 9..5
// void  TIM1_BRK_IRQHandler       // TIM1 Break
// void  TIM1_UP_IRQHandler        // TIM1 Update
// void  TIM1_TRG_COM_IRQHandler   // TIM1 Trigger and Commutation
// void  TIM1_CC_IRQHandler        // TIM1 Capture Compare
// void  TIM2_IRQHandler           // TIM2
// void  TIM3_IRQHandler           // TIM3
// void  TIM4_IRQHandler           // TIM4
// void  I2C1_EV_IRQHandler        // I2C1 Event
// void  I2C1_ER_IRQHandler        // I2C1 Error
// void  I2C2_EV_IRQHandler        // I2C2 Event
// void  I2C2_ER_IRQHandler        // I2C2 Error
// void  SPI1_IRQHandler           // SPI1
// void  SPI2_IRQHandler           // SPI2
// void  USART1_IRQHandler         // USART1
// void  USART2_IRQHandler         // USART2
// void  USART3_IRQHandler         // USART3
// void  EXTI15_10_IRQHandler      // EXTI Line 15..10
// void  RTCAlarm_IRQHandler       // RTC Alarm through EXTI Line
// void  USBWakeUp_IRQHandler      // USB Wakeup from suspend
// void  TIM8_BRK_IRQHandler       // TIM8 Break
// void  TIM8_UP_IRQHandler        // TIM8 Update
// void  TIM8_TRG_COM_IRQHandler   // TIM8 Trigger and Commutation
// void  TIM8_CC_IRQHandler        // TIM8 Capture Compare
// void  ADC3_IRQHandler           // ADC3
// void  FSMC_IRQHandler           // FSMC
// void  SDIO_IRQHandler           // SDIO
// void  TIM5_IRQHandler           // TIM5
// void  SPI3_IRQHandler           // SPI3
// void  UART4_IRQHandler          // UART4
// void  UART5_IRQHandler          // UART5
// void  TIM6_IRQHandler           // TIM6
// void  TIM7_IRQHandler           // TIM7
// void  DMA2_Channel1_IRQHandler  // DMA2 Channel1
// void  DMA2_Channel2_IRQHandler  // DMA2 Channel2
// void  DMA2_Channel3_IRQHandler  // DMA2 Channel3
// void  DMA2_Channel4_5_IRQHandler // DMA2 Channel4 & Channel5
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
#ifdef _GPS_ENABLE_
extern void GPS_handler(unsigned char value);//GPS处理程序
#endif
extern void com232pc_hander(unsigned char value);
#ifdef _GPS_TEST_
extern void GPS_PC_Test_hand(unsigned char value);
#endif
#ifdef BUS_YICHUN
extern void YICHUNGpsHand(unsigned char value);
#endif
#ifdef OTHER_GPRS
extern void OTHER_GPRS_hand(unsigned char value);
#endif

#if defined KEYBOARD || defined _GPS_Station_ExKey_enable_
extern void KeyBoardDeal(unsigned char vdat);
#endif

extern void QR_Code_handler(unsigned char value);

//GPRS中断处理程序
#ifndef NO_GJ_SERVISE
#ifdef WIFI_TLH_
extern void wifi_handler(unsigned char value);
#else
extern void GPRS_handler(unsigned char value);
#endif	//#ifdef WIFI_TLH_
#endif //#ifndef NO_GJ_SERVISE

#ifdef COM_TEST_
extern void ComTest_hand(unsigned char value);
#endif
#ifdef COM2RJ45_	//直接收BDBDBD的数据
extern void GPRS_BDBD_Data_hand(unsigned char value);
#endif
//直接接收完8583数据，然后拷贝，新的通讯平台透传使用
extern void GPRS_8583_Data_hand(unsigned char value);
#ifdef _TTSSound_enable_
extern volatile unsigned char TTS_State ;
#endif

#ifdef _GPS_Station_ExKey_enable_
extern void GPS_StationEnable_hand(unsigned char value);
#endif
extern char PCrevCom_hand(unsigned char val);
volatile unsigned char USART1mode;//当前串口处于什么工作状态
//串口1或3中断处理程序
void USART1_IRQHandler(void)
{
	unsigned int value; 
	
	if(com_rev(COM1, &value) == 0)
	{
		if(USART1mode == 1){
			com232pc_hander(value&0xff); // 串口通讯程序，在库里
		}
		else if(USART1mode ==2){	//串口连PC机的文件通讯
			PCrevCom_hand(value&0xff);
		}
#ifdef _TTSSound_enable_
		else if(USART1mode == 3){
			if((value&0xff)==0x41){ // TTS模块初始化成功返回0X4A
				//miniDispstr(1, 32, "TTSOK", 1);
				TTS_State =1;
				return;
			}
		}
#endif

		else{
#if defined _GPS_TEST_ 
			GPS_PC_Test_hand(value&0xff);
#elif defined KEYBOARD && KEYB_COM==1
			KeyBoardDeal((unsigned char)(value&0xff));
#elif defined _GPS_Station_ExKey_enable_
			KeyBoardDeal((unsigned char)(value&0xff));
#elif defined BUS_YICHUN
			YICHUNGpsHand(value&0xff);
#elif defined OTHER_GPRS
	#ifdef COM2RJ45_	//直接收BDBDBD的数据
				//GPRS_BDBD_Data_hand(value&0xff);
				GPRS_8583_Data_hand(value&0xff);
	#else			//外部DVR机，有协议
				OTHER_GPRS_hand(value&0xff);
	#endif
#elif defined Modem_BUS_	//外部GPRS模块
			GPRS_handler((unsigned char)(value&0xff));
#else
#ifdef COM_TEST_//测试
			ComTest_hand(value&0xFF);
#endif
#endif
	//	OTHER_GPRS_hand(value&0xff);
		}
		
	}
}
//红外中断处理程序
extern void Irda_IRQHandler(unsigned char value);
unsigned char USART2mode;//当前串口处于什么工作状态
//串口2中断处理程序,和红外共用。
void USART2_IRQHandler(void)
{
	unsigned int value;
	
//	COM_it_disable(COM2);
	if(com_rev(COM2, &value) == 0)
	{
#ifdef NO_GJ_SERVISE
		Irda_IRQHandler(value);
#else
		if(USART2mode == _TEST_Irda)
			Irda_IRQHandler(value);
		else{
#ifdef WIFI_TLH_
			wifi_handler((unsigned char)(value&0xff));
#else
			GPRS_handler((unsigned char)(value&0xff));
#endif
		}
#endif
	}
//	COM_it_enable(COM2);
//  debugstringCOM(COM1," COM2:");
//  debugdataCOM(COM1,(unsigned char*)&value, 1, 0);
}

extern void G24Card_handler(unsigned char value);
#ifdef DX24G_CARD
extern G24DX_handler(unsigned char value);
#endif
#ifdef _XMSHDZ_24G_
extern void XMSH_Hand(unsigned char vl);
#endif
#ifdef ZKXL_24G_
extern void ZKXL_Hand(unsigned char vl);
#endif
/**
	串口3中断处理PSAM卡的异常事件
  */
//void USART3_IRQHandler(void)//在库函数里
#ifdef KEY_PAD
extern void KeyPad_hander(unsigned char val);
#endif


//串口4中断处理程序
void UART4_IRQHandler(void)
{					  
	unsigned int value;

	if(com_rev(COM4, &value) == 0)
//	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) //若接收数据寄存器满
	{
#ifdef KEY_PAD
		KeyPad_hander((unsigned char)(value&0xff));
#endif
#if defined KEYBOARD && KEYB_COM==4	//新车载机的外接键盘在COM4
		KeyBoardDeal((unsigned char)(value&0xff));
#endif
#ifdef _GPS_TEST_
		GPS_PC_Test_hand(value&0xff);
#endif
#if defined  OTHER_GPRS && defined _debug_
	#ifdef COM2RJ45_	//直接收BDBDBD的数据
			//GPRS_BDBD_Data_hand(value&0xff);
			GPRS_8583_Data_hand(value&0xff);
	#else			//外部DVR机，有协议
			OTHER_GPRS_hand(value&0xff);
	#endif
#endif


// #if defined _XMSHDZ_24G_ && !defined _New_Bu_mode_	//新车载机在COM6，旧车载机在COM5
// 		XMSH_Hand((unsigned char)(value&0xff));
// #elif defined ZKXL_24G_ && !defined _New_Bu_mode_	//新车载机在COM6，旧车载机在COM5
// 		ZKXL_Hand((unsigned char)(value&0xff));
// #endif
// #ifndef NO_GJ_SERVISE
// 		GPRS_handler((unsigned char)(value&0xff));
// #endif
	}
}

//串口5中断处理程序
void UART5_IRQHandler(void)
{					 
	unsigned int value;
	
	if(com_rev(COM5, &value) == 0)
//	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //若接收数据寄存器满
	{
// #ifdef _BlueTooth_COM
// 		bluetooth_hand((unsigned char)(value&0xff));
// 		return;
// #endif
#ifdef _GPS_ENABLE_
		GPS_handler((unsigned char)(value&0xff));			//GPS
#elif defined _CPU_24G_ || !defined NO_MOBILE_24G_
		G24Card_handler((unsigned char)(value&0xff));
#elif defined DX24G_CARD  //只读卡号，和 _XMSHDZ_24G_ 用的读头是一样的。
		G24DX_handler((unsigned char)(value&0xff));
#elif defined ZKXL_24G_
		ZKXL_Hand((unsigned char)(value&0xff));
#elif defined _XMSHDZ_24G_
		XMSH_Hand((unsigned char)(value&0xff));
#endif

	}
}

void USART6_IRQHandler(void)
{					  
	unsigned int value;
	
	if(com_rev(COM_QR, &value) == 0)
	{
#ifdef QR_CODE_COM
		QR_Code_handler((unsigned char)(value&0xff));
#endif

	}
	
}

/**
  * @brief  This function handles CAN1 Handler.
  * @param  None
  * @retval None
  */
#ifdef FengDuan_BU_
extern void CAN_RxDataHander(void);
#endif
//void USB_LP_CAN1_RX0_IRQHandler(void)
void CAN1_RX0_IRQHandler(void)
{
#ifdef FengDuan_BU_
	CAN_RxDataHander();
#endif
// 	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
// 	if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD)&&(RxMessage.DLC == 1))
// 	{
// 		LED_Display(RxMessage.Data[0]);
// 		Key_Pressed_Number = RxMessage.Data[0];
// 	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/************* (C) COPYRIGHT 2009 Wuhan R&D Center, Embest *****END OF FILE****/
