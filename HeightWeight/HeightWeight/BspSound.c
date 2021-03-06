#include "BspSound.h"
#include "APPTooL.h"
#include "application.h"
#include "BspConfig.h"
#include "math.h"
uint8_t sonundArray[5] = { HUAN_YING, BEN_XIANG_MU, WO_LI, CESHI, QING_YONG_LI_WO };
//uint8_t dalaytim[4] = { 0xf3,YIN_XIAO_61,0xf8,10};				//F3H + 地址 + F8H + 10H
void Firstmuis()            //开头语音
{
	WTN6040_PlayArray(5, sonundArray,5000);
	osDelay(300);
	WTN6040_PlayOneByte(QING_AN_KAISHI, 5000);
	//WTN6040_PlayArray(4, dalaytim);
	/**/
}
void BeginSound()
{
	WTN6040_PlayOneByte(YIN_XIAO_61, 1000);
	osDelay(100);
	WTN6040_PlayOneByte(CESHI_KAISHI, 1000);
}
void OverTest()					//测试结束
{
	WTN6040_PlayOneByte(CESHI_JIESHU, 1000);
}
uint8_t ProcessHeight(double numdata)//身高
{
	uint8_t dat[16] = { 0 };
	uint8_t reslen = 0, templen = 0;
	uint8_t tdat[16] = { 0 };
	templen = DataSeparation(numdata, tdat);
	dat[0] = NINDE;
	dat[1] = SHEN_GAO;
	dat[2] = GONG_FEN;
	reslen = insertArray(dat, 3, tdat, templen, 2);
	WTN6040_PlayArray(reslen, dat, 1000);
	return reslen;
}

uint8_t ProcessGrip(double numdata)//握持力
{
	uint8_t dat[16] = { 0 };
	uint8_t reslen = 0, templen = 0;
	uint8_t tdat[16] = { 0 };
	templen = DataSeparation(numdata, tdat);
	dat[0] = NINDE;
	dat[1] = WO_LI;
	dat[2] = GONG_JIN;
	reslen = insertArray(dat,3, tdat, templen, 2);
	WTN6040_PlayOneByte(YIN_XIAO_61, 1000);           //音效
	osDelay(100);
	WTN6040_PlayArray(reslen, dat, 1000);
	osDelay(200);
	WTN6040_PlayOneByte(HUAN_ZAI_CI_YING, 1000);
	return reslen;
}

uint8_t ProcessBMI(double numdata)//bmi
{
	uint8_t dat[16] = {0};
	uint8_t reslen = 0, templen = 0;
	uint8_t tdat[16] = { 0 };
	templen = DataSeparation(numdata, tdat);
	dat[0] = SOUND_BIM;
	dat[1] = 0;
	reslen = insertArray(dat, 1, tdat, templen, 1);
	WTN6040_PlayArray(reslen, dat, 1000);
	return (reslen);
}

double Cal_BMI(double hei, double wei)        //身高/体重的平方
{
	double h = hei*hei;
	return (wei/h);
}
//************************************
// 函数:    PlayHei_Wei
// 函数全名:  PlayHei_Wei
// 函数类型:    public 
// 返回值:   void
// Qualifier: //总体播放函数
// 参数: double height      身高
// 参数: double wei			体重
//************************************
void PlayHei_Wei(double height, double wei)          //总体播放函数
{
	double bmi = Cal_BMI(height / 100.00, wei);
	uint8_t len = ProcessHeight(height);
	osDelay(100);
	ProcessWeight(wei);
	osDelay(100);
	ProcessBMI(bmi);
	osDelay(300);
	WTN6040_PlayOneByte(TI_XING, 1000);
	osDelay(200);
	if (bmi <= 18.40)
	{
		WTN6040_PlayOneByte(PIAN_SHOU, 1000);
	}
	else if ((bmi > 18.50) && (bmi < 25.90))
	{
		WTN6040_PlayOneByte(ZHENG_CHANG, 1000);
	}
	else if (bmi >= 28.00)
		WTN6040_PlayOneByte(PIAN_PANG, 1000);
	osDelay(200);
	WTN6040_PlayOneByte(HUAN_ZAI_CI_YING, 1000);
}
double Cal_BMI_TFT(uint32_t wi, uint16_t hi)
{
	double h = 0.00, w = 0.00, res = 0.00;
	h = hi / 1000.00;
	w = abs(wi) / 1000.00;
	res = w / (h*h);
	//uint16_t TFT_bmi = (uint16_t)(bmi_1 * 100);
	return  res;
}

uint16_t Average_arr(uint32_t *arr, uint8_t len)
{
	uint8_t i = 0;
	uint32_t sum = 0;
	for (i = 0; i < len;i++)
	{
		sum += arr[i];
	}
	return sum / len;
}