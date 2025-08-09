#include "main.h"
#include "system.h"
#include "conversion.h"
#include "window.h"

#include <math.h>

// 0 - TEMP CPU
// 1 - CURRENT
// 2 - VOLTAGE
// 3 - VIP / TEMP EXT
#define ADC_CH_COUNT 2

#define PI 3.14159265f

unsigned short vinraw;
unsigned short vin;

unsigned short dmadata[ADC_CH_COUNT];
int T;
int Traw;
int Tindex;

TConversion conv;

TIM_HandleTypeDef htim2 = { 0 };

#define ENABLE_TEMP      1
#define ENABLE_VIN       2
#define DISABLE_TEMP_VIN 3

void TempVinConfig(int cmd)
{

}

void ConversionInitTimer(void)
{	
	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };	

	__HAL_RCC_TIM2_CLK_ENABLE();

	// частота таймера 32 МГц
	// таймер считает 250 мкс (2000 Гц = 0,004 мгц, 40 точек на период)
	// делитель = 32 мгц / 0,002 мгц = 16000
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = TimClock / 2000 - 1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if(HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim2, TIM_IT_UPDATE);

	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_Base_Start(&htim2);
}

void ConversionInitGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// Current IN - Analog
	GPIO_InitStruct.Pin = ADC_CURRENT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_CURRENT_PORT, &GPIO_InitStruct);

	// Current GND - OD
	GPIO_InitStruct.Pin = ADC_CURRENT_GND_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_CURRENT_GND_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_CURRENT_GND_PORT, ADC_CURRENT_GND_PIN, GPIO_PIN_RESET);

	// Voltage IN - Analog
	GPIO_InitStruct.Pin = ADC_VOLTAGE_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VOLTAGE_PORT, &GPIO_InitStruct);

	// Voltage GND - OD
	GPIO_InitStruct.Pin = ADC_VOLTAGE_GND_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VOLTAGE_GND_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VOLTAGE_GND_PORT, ADC_VOLTAGE_GND_PIN, GPIO_PIN_RESET);

	// Temp / VIN - Analog
	GPIO_InitStruct.Pin = ADC_VIN_TEMP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VIN_TEMP_PORT, &GPIO_InitStruct);

	// Temp GND - OD
	GPIO_InitStruct.Pin = ADC_TEMP_GND_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_TEMP_GND_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_TEMP_GND_PORT, ADC_TEMP_GND_PIN, GPIO_PIN_RESET);

	// Temp VCC - PP -> 1
	GPIO_InitStruct.Pin = ADC_TEMP_VCC_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_TEMP_VCC_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_TEMP_VCC_PORT, ADC_TEMP_VCC_PIN, GPIO_PIN_SET);

	// GND для входов тока и напряжения включены, для отключения - записать в GND пины единицу
	// вход Temp / VIN настроен на температуру, для переключения - вызвать TempVinConfig()
}

void ConversionInitADC(void)
{
	ADC_HandleTypeDef hadc1 = { 0 };
	ADC_ChannelConfTypeDef ch = { 0 };
	DMA_HandleTypeDef hdma = { 0 };

	__HAL_RCC_DMA_CLK_ENABLE();
	
	hdma.Instance = DMA1_Channel1;
	hdma.Init.Request = DMA_REQUEST_ADC1_DS;
	hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma.Init.MemInc = DMA_MINC_ENABLE;
	hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma.Init.Mode = DMA_NORMAL;
	hdma.Init.Priority = DMA_PRIORITY_HIGH;
	if(HAL_DMA_Init(&hdma) != HAL_OK) { Error_Handler(); }

	DMA1_Channel1->CCR &= ~DMA_CCR_EN;

	DMA1->IFCR = DMA_IFCR_CTCIF1;

	DMA1_Channel1->CNDTR = ADC_CH_COUNT;
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DS_DATAOUT;
	DMA1_Channel1->CMAR = (uint32_t)dmadata;
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	hadc1.Instance = ADC1;
	hadc1.Init.ConversionType = ADC_CONVERSION_WITH_DS;
	hadc1.Init.SequenceLength = ADC_CH_COUNT;
	hadc1.Init.SamplingMode = ADC_SAMPLING_AT_START;
	hadc1.Init.SampleRate = ADC_SAMPLE_RATE_128;
	hadc1.Init.InvertOutputMode = ADC_DATA_INVERT_NONE;
	hadc1.Init.Overrun = ADC_NEW_DATA_IS_LOST;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_12_BIT;
	hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_8;
	HAL_ADC_DeInit(&hadc1);
	if(HAL_ADC_Init(&hadc1) != HAL_OK) { Error_Handler(); }

	ch.Channel = ADC_CHANNEL_TEMPSENSOR;
	ch.Rank = ADC_RANK_1;
	ch.VoltRange = ADC_VIN_RANGE_1V2;
	ch.CalibrationPoint.Number = ADC_CALIB_POINT_1;
	ch.CalibrationPoint.Gain = LL_ADC_GET_CALIB_GAIN_FOR_VINPX_1V2();
	ch.CalibrationPoint.Offset = LL_ADC_GET_CALIB_OFFSET_FOR_VINPX_1V2();
	if(ch.CalibrationPoint.Gain == 0xFFF)
	{
		ch.CalibrationPoint.Gain = LL_ADC_DEFAULT_RANGE_VALUE_1V2;
		ch.CalibrationPoint.Offset = 0UL;
	}
	//if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) { Error_Handler(); }

	
	ch.VoltRange = ADC_VIN_RANGE_3V6;
	ch.CalibrationPoint.Number = ADC_CALIB_POINT_2;
	ch.CalibrationPoint.Gain = LL_ADC_GET_CALIB_GAIN_FOR_VINPX_3V6();
	ch.CalibrationPoint.Offset = LL_ADC_GET_CALIB_OFFSET_FOR_VINPX_3V6();
	if(ch.CalibrationPoint.Gain == 0xFFF)
	{
		ch.CalibrationPoint.Gain = LL_ADC_DEFAULT_RANGE_VALUE_2V4;
		ch.CalibrationPoint.Offset = 0UL;
	}

	ch.Channel = ADC_CURRENT_CH;
	ch.Rank = ADC_RANK_1; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ch.Channel = ADC_VOLTAGE_CH;
	ch.Rank = ADC_RANK_2; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	//ch.Channel = ADC_VIN_TEMP_CH;
	//ch.Rank = ADC_RANK_4; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ADC1->CONF |= ADC_CONF_DMA_DS_ENA;

	HAL_ADC_Start(&hadc1);
}

void ConversionGetPowerVoltage(void)
{
	ADC_HandleTypeDef hadc1 = { 0 };
	ADC_ChannelConfTypeDef ch = { 0 };
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// VIN GND - OD
	GPIO_InitStruct.Pin = ADC_VIN_GND_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VIN_GND_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VIN_GND_PORT, ADC_VIN_GND_PIN, GPIO_PIN_RESET);

	// VIN_EN (1 - active)
	GPIO_InitStruct.Pin = ADC_VIN_EN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VIN_EN_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VIN_EN_PORT, ADC_VIN_EN_PIN, GPIO_PIN_SET);

	LL_PWR_SetNoPullA(LL_PWR_GPIO_BIT_8 | LL_PWR_GPIO_BIT_9);

	// VIN_TEMP - Analog
	GPIO_InitStruct.Pin = ADC_VIN_TEMP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VIN_TEMP_PORT, &GPIO_InitStruct);

	LL_PWR_SetNoPullB(LL_PWR_GPIO_BIT_1);

	hadc1.Instance = ADC1;
	hadc1.Init.ConversionType = ADC_CONVERSION_WITH_DS;
	hadc1.Init.SequenceLength = 1;
	hadc1.Init.SamplingMode = ADC_SAMPLING_AT_START;
	hadc1.Init.SampleRate = ADC_SAMPLE_RATE_16;
	hadc1.Init.InvertOutputMode = ADC_DATA_INVERT_NONE;
	hadc1.Init.Overrun = ADC_NEW_DATA_IS_LOST;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_16_BIT;
	hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_128;
	if(HAL_ADC_Init(&hadc1) != HAL_OK) { Error_Handler(); }
	
	ch.Channel = ADC_VIN_TEMP_CH;
	ch.VoltRange = ADC_VIN_RANGE_3V6;
	ch.CalibrationPoint.Number = ADC_CALIB_POINT_2;
	ch.CalibrationPoint.Gain = LL_ADC_GET_CALIB_GAIN_FOR_VINPX_3V6();
	ch.CalibrationPoint.Offset = LL_ADC_GET_CALIB_OFFSET_FOR_VINPX_3V6();
	if(ch.CalibrationPoint.Gain == 0xFFF)
	{
		ch.CalibrationPoint.Gain = LL_ADC_DEFAULT_RANGE_VALUE_3V6;
		ch.CalibrationPoint.Offset = 0UL;
	}

	ch.Rank = ADC_RANK_1; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();
	
	HAL_ADC_Start(&hadc1);

	HAL_ADC_PollForConversion(&hadc1, 1000);

	int data = HAL_ADC_GetValue(&hadc1);
	
	vin = ((data * 41) >> 8) - 1719; // v = ADC * 0.1603 - 1719
	vinraw = vin * 255;
	
	//while(1)
	//{
	//	ADC1->IRQ_STATUS = ADC_IRQ_FLAG_EODS | ADC_IRQ_FLAG_EOS | ADC_IRQ_FLAG_OVRDS;
	//	ADC1->CTRL |= ADC_CTRL_START_CONV;
	//	
	//	HAL_ADC_PollForConversion(&hadc1, 1000);
	//	
	//	vin = ((HAL_ADC_GetValue(&hadc1) * 41) >> 8) - 1719; 

	//	// для калибровки: vin сделать float, усреднить фильтром калмана
	//	// vin = vin * 0.999f + HAL_ADC_GetValue(&hadc1) * 0.001f;
	//}

	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET); // DISABLE VBAT
	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   // DISCONNECT VDD
}

void ConversionTest(void)
{
	// summ = 1
	float summ = 0;
	for(int i = 0; i < WLEN; i++)
		summ += window[i];

	// измерение идеальной синусоиды
	#define TN 16
	float urms[TN] = { 0 };
	float udc[TN] = { 0 };
	double dt = 1.0 / 50 / PPP;

	for(int t = 0; t < TN; t++)
	{
		float rms = 0;
		float dc = 0;
		for(int i = 0; i < WLEN; i++)
		{	
			float sample = (float)(100 * sqrt(2.0) * sin(2 * PI * 50.003 * dt * (t * WLEN + i)) + 0);

			rms += sample * sample * window[i];

			dc += sample * window[i];
		}

		urms[t] = sqrtf(rms);
		udc[t] = dc;
	}
}

void ConversionProcess(void)
{
	Traw += __LL_ADC_CALC_TEMPERATURE(dmadata[0], LL_ADC_DS_DATA_WIDTH_16_BIT);

	Tindex++;

	if(Tindex >= 4000)
	{
		T = Traw / 400;
		Traw = 0;
		Tindex = 0;
	}

	float w = window[conv.Kw];

	float di = ((short)(dmadata[0] - conv.DCI)) * conv.KI;
	float du = ((short)(dmadata[1] - conv.DCU)) * conv.KU;

	conv.si += di * di * w;
	conv.su += du * du * w;

	conv.sidc += di * w;
	conv.sudc += du * w;

	conv.st += dmadata[0];

	if(++conv.Kw >= WLEN)
	{
		conv.I = sqrtf(conv.si);
		conv.U = sqrtf(conv.su);
		conv.T = conv.st * 10 / WLEN;

		conv.Idc = conv.Idc * 0.9f + conv.sidc * 0.1f;
		conv.Udc = conv.Udc * 0.9f + conv.sudc * 0.1f;

		conv.si = 0;
		conv.su = 0;

		conv.sidc = 0;
		conv.sudc = 0;

		conv.st = 0;

		conv.Kw = 0;
	}

	vin = (vin * 255 + ((dmadata[0] * 41) >> 8) - 1719) >> 8;
}

void ConversionInit(void)
{
	conv.KI = 1.0f;// / 65535;
	conv.KU = 1.0f;// / 65535;

	conv.DCI = 1226;
	conv.DCU = 1748;

	ConversionGetPowerVoltage();
	
	ConversionInitGPIO();
	ConversionInitADC();
	ConversionInitTimer();
}

volatile unsigned long dmacnt;
volatile unsigned long adcstatus;
volatile unsigned long delta;
volatile unsigned long timer;
volatile unsigned long point;
unsigned short iraw[400];
unsigned short uraw[400];

void TIM2_IRQHandler(void)
{
	delta = GLOBAL_DELTA_TIME(timer);
	timer = GLOBAL_TIMER;

	TIM2->SR &= ~TIM_IT_UPDATE;

	adcstatus = ADC1->IRQ_STATUS;
	dmacnt = DMA1_Channel1->CNDTR;

	if (!(adcstatus & ADC_IRQ_FLAG_EOS))
	{
		dmacnt++;
	}
	
	iraw[point] = dmadata[0] - conv.DCI;
	uraw[point] = dmadata[1] - conv.DCU;

	if(++point >= 400) point = 0;

	ConversionProcess();

	DMA1_Channel1->CCR &= ~DMA_CCR_EN;
	DMA1->IFCR = DMA_IFCR_CTCIF1;
	DMA1_Channel1->CNDTR = ADC_CH_COUNT;
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	ADC1->IRQ_STATUS = ADC_IRQ_FLAG_EODS | ADC_IRQ_FLAG_EOS | ADC_IRQ_FLAG_OVRDS;
	ADC1->CTRL |= ADC_CTRL_START_CONV;

	conv.time = GLOBAL_DELTA_TIME(timer);
}
