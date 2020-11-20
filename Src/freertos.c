/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "usart.h"
#include "ultrasonic.h"
#include "BspConfig.h"
#include "button.h"
#include "stmflash.h"
#include "gpio.h"
#include "application.h"
#include "BspSound.h"
#include "adc.h"
#include "APPTooL.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//按键用变量
uint16_t KeyValue_t = 0xffff; uint16_t lastvalue_t = 0xffff;
Key_Message keys[5] = { 0 };
uint8_t Key1_flag = 0;  //按键按下标志
//毛坯重量
uint32_t Weight_Skin = 0;
uint16_t Skin_arr[2] = {0};   //毛皮重量数组，为存到mcuflash中
uint32_t Pull_arr[50] = {0};		//拉力测量值数组
uint8_t	pi = 0;
uint8_t no_grip_k = 10; //提示握住用的计次变量
double  Grip_Res = 0;//存储计数完的握力

//发送到TFT屏的数据
uint32_t TFT_Grip_Res = 0;		//显示计数后的最大值
uint32_t TFT_Grip = 0;		//拉力时时值
uint8_t  TFT_DownTime = 0; //显示倒计时
uint8_t  back_tim = 0;
//flash中
uint32_t Weight_flash = 0;
uint16_t Weight_flash_array[2] = {0};
//语音播报地址数组
uint8_t height_array[16] = {0};
uint32_t sound_weight = 0;
uint16_t tip_play_num = 0;   //提示计数
uint8_t  tip_flag = 0;		//提示标志
uint8_t  clear_num = 0;

//互斥量
extern SemaphoreHandle_t xSemaphore_WTN6_TFT;//串口，语音播放互斥量
//线程句柄
osThreadId SensorDriveHandle;//传感器驱动线程
osThreadId ButtonProcessHandle;//按键处理线程
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void SensorDrive_CallBack(void const *argument);
void  ButtonProcess_CallBack(void const *argument);

//按键回调函数
void  Key_CallBack(Key_Message index);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	STMFLASH_Read(FLASH_BEGIN, Weight_flash_array, 2);
	Weight_flash = Weight_flash_array[1];
	Weight_flash = (Weight_flash << 16) + Weight_flash_array[0];
	Weight_Skin = Weight_flash;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
#if DEBUG_PRINT
	Uartx_printf(&huart1, "The Weight_skin is %d\r\n", Weight_flash);
#endif
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	//unsigned long val = 0;
	//unsigned int  Weight_Shiwu = 0;
 // for(;;)
 // {
	//  val = Read_Weigh_1(1000);
	//  val = val / 100;
	//  Weight_Shiwu = val;
	//  Weight_Shiwu = (unsigned int)((float)Weight_Shiwu / 0.0214);
	//  printf("The Weight is:%dg", GetRealWeight(0)); fflush(stdout);//必须刷新输出流**************************************
	// //printf("The Weight is:%ldg", Weight_Shiwu); fflush(stdout);//必须刷新输出流
 //   osDelay(500);
	//
 // }
	taskENTER_CRITICAL();//进入临界区
	osDelay(500);     //等待系统稳定
	//传感器驱动线程
	osThreadDef(SensorDrive, SensorDrive_CallBack, 4, 0, 128);
	SensorDriveHandle = osThreadCreate(osThread(SensorDrive), NULL);
	//按键处理线程
	osThreadDef(ButtonProcess, ButtonProcess_CallBack, 5, 0, 128);
	ButtonProcessHandle = osThreadCreate(osThread(ButtonProcess), NULL);
#if DEBUG_PRINT
	Uart_printf(&huart1, "Start sub stask\r\n");
#endif	
	vTaskDelete(defaultTaskHandle); //删除任务
	taskEXIT_CRITICAL();//推出临界区
  /* USER CODE END StartDefaultTask */
	
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void SensorDrive_CallBack(void const *argument)             //传感器操作线程---------拉力检测
{ 
	//变量声明
	uint8_t i = 0;  //力值数组下标
	uint8_t k = 0;	//控制播放力的数值的 次数        控制播放次数
	uint8_t j = 0;//  倒计时计数
	uint8_t no_grip_i = 0;     //力度不够提示用
	pi = 101;
	xSemaphoreTake(xSemaphore_WTN6_TFT, portMAX_DELAY);
	{
		WTN6040_PlayOneByte(SOUND_VALUE, 1000);//调节音量
		HAL_Delay(20);
		Firstmuis();					//播放开始音乐
		write_variable_store_82_1word(TFT_RES_VAL_ADRESS, 0);//发送测试结果
		write_variable_store_82_1word(TFT_INSTANTANEOUS_FORCE_ADRESS, 0);
	}
	xSemaphoreGive(xSemaphore_WTN6_TFT);

	for (;;)
	{
		if (Key1_flag == 1) //开始播放标志
		{
			
			sound_weight = GetRealWeight(Weight_Skin);  //拉力检测
			//printf("PI is:%dg\r\n", sound_weight); fflush(stdout);          //打印瞬时力**********************
			if (sound_weight>WEIGHT_MIN)     //大于阀值说明已经握住握力器，开始测试
			{
				write_variable_store_82_1word(TFT_TEST_ERROR_ADRESS, 0);
				no_grip_i = 0;
				if (pi < TEST_TIME_LONG(10) / SENSOR_PERIOD)  //循环100次 取100次的值
				{
					//此处向串口屏输出握力时时数据
					TFT_Grip = sound_weight;  //此处向串口屏输出握力时时数据
					//printf("PI is:%dg\r\n", TFT_Grip); fflush(stdout);//打印及时握力************************
					xSemaphoreTake(xSemaphore_WTN6_TFT, portMAX_DELAY);
					{
						write_variable_store_82_1word(TFT_INSTANTANEOUS_FORCE_ADRESS, TFT_Grip / 100); //发送数据到TFT       发送瞬时值
					}
					xSemaphoreGive(xSemaphore_WTN6_TFT);
					
					k = 0;
					if (pi % 2)
					{
						if (i < 50)
						{
							Pull_arr[i] = TFT_Grip;   //50次数据存储
							i++;
						}
						else
						{
							i = 0;
						}
					}
					if (j++ > 8) //0~9
					{
						j = 0;
						back_tim++;
						TFT_DownTime = COUNT_DOWN - back_tim;//倒计时输出到屏
						xSemaphoreTake(xSemaphore_WTN6_TFT, portMAX_DELAY);
						{
							write_variable_store_82_1word(TFT_BACK_TIM_ADRESS, TFT_DownTime);//发送倒计时
						}
						xSemaphoreGive(xSemaphore_WTN6_TFT);
						
						//printf("TFT num is ===============%d-----------------%d\r\n", COUNT_DOWN-back_tim,back_tim);//打印倒计时，时间***********************
					}
					pi++;

				}
				else       //测试结束代码
				{      
					if (k <= 0)
					{
						xSemaphoreTake(xSemaphore_WTN6_TFT, portMAX_DELAY);
						{
							write_variable_store_82_1word(TFT_START_GIT_ADRESS, 0); //关闭测试开始动画
							TFT_Grip_Res = GetMax(Pull_arr, 50); //向TFT屏输出测试结果
							write_variable_store_82_1word(TFT_RES_VAL_ADRESS, TFT_Grip_Res / 100);//发送测试结果
							Grip_Res = (double)TFT_Grip_Res / 1000; //取最大值
							//printf("Time is outed :%dg\r\n", GetMax(Pull_arr, 50)); fflush(stdout);//打印测试结果**********************
							//printf("average is %dg\r\n",Average_arr(Pull_arr, 50)); fflush(stdout);
							write_variable_store_82_1word(TFT_SPEAK_GIF_ADRESS, 1);  //喇叭动画开始
							ProcessGrip(Grip_Res);//播放握力   //播放测试结果
							write_variable_store_82_1word(TFT_SPEAK_GIF_ADRESS, 0);//喇叭动画结束
						}
						xSemaphoreGive(xSemaphore_WTN6_TFT);
						
						Key1_flag = 0;
						k++;
					}
					//printf("PI_OUT is :%dg\r\n", pi); fflush(stdout);//必须刷新输出流**************************************
				}
			}
			else
			{
				//力度不够或没有抓握提示                     每20s提示一次，共提示2次
				if (no_grip_i++>NO_GRIP_NUM(5) / SENSOR_PERIOD)
				{
					no_grip_i = 0;
					if (no_grip_k<TIP_COUNT)
					{
						WTN6040_PlayOneByte(QING_YONG_LI_WO,1000);
						write_variable_store_82_1word(TFT_TEST_ERROR_ADRESS, 1);
						no_grip_k++;
					}
					
				}
			}

		}
		
		
		//sound_weight = GetRealWeight(Weight_Skin);
	  // printf("PI_OUT is :%dg\r\n", sound_weight); fflush(stdout);//必须刷新输出流**************************************
		HAL_GPIO_TogglePin(LED_LEFT_PORT, LED_LEFT_PIN);//线程活动指示灯
		osDelay(SENSOR_PERIOD);                                                             //周期T=100ms 频率F = 10Hz
	}
}
void  ButtonProcess_CallBack(void const *argument)
{
	uint16_t battery_i = 0;
	for (;;)
	{
	
		ScanKeys(&KeyValue_t, &lastvalue_t, keys, Key_CallBack);
		HAL_GPIO_TogglePin(LED_RITHT_PORT, LED_RIGHT_PIN);                  //线程活动指示灯
		if (battery_i++>TEST_TIME_LONG(1) / BUTTON_SCAN_CYCLE)
		{
			battery_i = 0;
			xSemaphoreTake(xSemaphore_WTN6_TFT, portMAX_DELAY);
			{
				write_variable_store_82_1word(TFT_BATTERY_GIT_ADRESS, ADC_GetValue(&hadc1, 10));//向TFT屏传输电量数据
			}
			xSemaphoreGive(xSemaphore_WTN6_TFT);

		}
		///Uart_printf(&huart1, "Task2\r\n");
		osDelay(BUTTON_SCAN_CYCLE);

	}
}
void Key_Regist()
{
	//重量清零
	keys[0].GPIOx = WEIGHT_RES_GPIO_Port;
	keys[0].GPIO_Pin = WEIGHT_RES_Pin;
	keys[0].Key_count = 5;

	//身高清零按键
	keys[1].GPIOx = DISTANCE_RES_GPIO_Port;
	keys[1].GPIO_Pin = DISTANCE_RES_Pin;
	keys[1].Key_count = 5;

	//备用1
	keys[2].GPIOx = KEY1_GPIO_Port;
	keys[2].GPIO_Pin = KEY1_Pin;
	keys[2].Key_count = 5;

	keys[3].GPIOx = KEY2_GPIO_Port;
	keys[3].GPIO_Pin = KEY2_Pin;
	keys[3].Key_count = 5;

	keys[4].GPIOx = KEY3_GPIO_Port;
	keys[4].GPIO_Pin = KEY3_Pin;
	keys[4].Key_count = 5;
}

void  Key_CallBack(Key_Message index)
{

	if (index.GPIO_Pin == WEIGHT_RES_Pin) //秤重清零
	{
		//sound_weight = 1001;
		//WTN6040_PlayOneByte(QING_AN_KAISHI);
		//
		Weight_Skin = GetRealWeight(0);
		Skin_arr[0] = Weight_Skin & 0x0000ffff;
		Skin_arr[1] = Weight_Skin >> 16;
		STMFLASH_Write(FLASH_BEGIN, Skin_arr, 2);//把清零数据存储到flash中
	}
	if (index.GPIO_Pin==DISTANCE_RES_Pin) //距离清零
	{
		Uart_printf(&huart1, "000000000000000000000000000000\r\n");
		//pi = 0;
	}
	if (index.GPIO_Pin==KEY1_Pin)
	{
		//Uartx_printf(&huart1, "*****************************\r\n");
		write_register_80_1byte(TFT_BUTTON, 1);//开屏
		write_variable_store_82_1word(TFT_START_GIT_ADRESS, 1);//测试开始动画开始播放
		write_variable_store_82_1word(TFT_TEST_ERROR_ADRESS, 0);//关闭错误动画
		write_variable_store_82_1word(TFT_RES_VAL_ADRESS, 0);//发送测试结果
		write_variable_store_82_1word(TFT_INSTANTANEOUS_FORCE_ADRESS, 0);
		write_variable_store_82_1word(TFT_BACK_TIM_ADRESS, 10);//发送倒计时
		BeginSound();
		//Uart_printf(&huart1, "*****************************\r\n");           //按键按下测试*******************************
		pi = 0;
		Key1_flag = 1;
		no_grip_k = 0;  //提示于播放次数清零
		back_tim = 0;	//倒计时归零
		for (uint8_t i = 0; i < 50;i++)   //数组清零
		{
			Pull_arr[i] = 0;
		}
		
	}
	//Uartx_printf(&huart1, "Key===%d\r\n", index);
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
