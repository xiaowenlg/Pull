#include "BspSound.h"
#include "APPTooL.h"
#include "application.h"
#include "BspConfig.h"

uint8_t sonundArray[6] = { HUAN_YING, BEN_XIANG_MU, WO_LI, CESHI, QING_YONG_LI_WO };
//uint8_t dalaytim[4] = { 0xf3,YIN_XIAO_61,0xf8,10};				//F3H + ��ַ + F8H + 10H
void Firstmuis()            //��ͷ����
{
	WTN6040_PlayArray(5, sonundArray);
	osDelay(300);
	WTN6040_PlayOneByte(QING_AN_KAISHI);
	//WTN6040_PlayArray(4, dalaytim);
	/**/
}
void BeginSound()
{
	WTN6040_PlayOneByte(YIN_XIAO_61);
	osDelay(100);
	WTN6040_PlayOneByte(CESHI_KAISHI);
}
void OverTest()					//���Խ���
{
	WTN6040_PlayOneByte(CESHI_JIESHU);
}
uint8_t ProcessHeight(double numdata)//���
{
	uint8_t dat[16] = { 0 };
	uint8_t reslen = 0, templen = 0;
	uint8_t tdat[16] = { 0 };
	templen = DataSeparation(numdata, tdat);
	dat[0] = NINDE;
	dat[1] = SHEN_GAO;
	dat[2] = GONG_FEN;
	reslen = insertArray(dat, 3, tdat, templen, 2);
	WTN6040_PlayArray(reslen, dat);
	return reslen;
}

uint8_t ProcessGrip(double numdata)//�ճ���
{
	uint8_t dat[16] = { 0 };
	uint8_t reslen = 0, templen = 0;
	uint8_t tdat[16] = { 0 };
	templen = DataSeparation(numdata, tdat);
	dat[0] = NINDE;
	dat[1] = WO_LI;
	dat[2] = GONG_JIN;
	reslen = insertArray(dat,3, tdat, templen, 2);
	WTN6040_PlayArray(reslen, dat);
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
	WTN6040_PlayArray(reslen, dat);
	return (reslen);
}

double Cal_BMI(double hei, double wei)        //���/���ص�ƽ��
{
	double h = hei*hei;
	return (wei/h);
}
//************************************
// ����:    PlayHei_Wei
// ����ȫ��:  PlayHei_Wei
// ��������:    public 
// ����ֵ:   void
// Qualifier: //���岥�ź���
// ����: double height      ���
// ����: double wei			����
//************************************
void PlayHei_Wei(double height, double wei)          //���岥�ź���
{
	double bmi = Cal_BMI(height / 100.00, wei);
	uint8_t len = ProcessHeight(height);
	osDelay(100);
	ProcessWeight(wei);
	osDelay(100);
	ProcessBMI(bmi);
	osDelay(300);
	WTN6040_PlayOneByte(TI_XING);
	osDelay(200);
	if (bmi <= 18.40)
	{
		WTN6040_PlayOneByte(PIAN_SHOU);
	}
	else if ((bmi > 18.50) && (bmi < 25.90))
	{
		WTN6040_PlayOneByte(ZHENG_CHANG);
	}
	else if (bmi >= 28.00)
		WTN6040_PlayOneByte(PIAN_PANG);
	osDelay(200);
	WTN6040_PlayOneByte(HUAN_ZAI_CI_YING);
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