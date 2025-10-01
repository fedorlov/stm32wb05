#include "main.h"
#include "system.h"
#include "conversion.h"
#include "window.h"

#include <math.h>

// 0 - CURRENT
// 1 - VOLTAGE
// 2 - VIP / TEMP EXT
#define ADC_CH_COUNT 5
#define DMA_BUF_COUNT (ADC_CH_COUNT * 50) // буффер для DMA на 25 мс

#define PI 3.14159265f

unsigned short vinraw;
unsigned short vin;

unsigned short dmadata[DMA_BUF_COUNT];
unsigned short rawdata[5];
unsigned short adcdata[3];

TConversion conv;

TIM_HandleTypeDef htim2 = { 0 };

#define ENABLE_TEMP      1
#define ENABLE_VCC       2
#define DISABLE_TEMP_VCC 3

void PinDisconnect(GPIO_TypeDef* port, uint32_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void TempVinConfig(int cmd)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	PinDisconnect(ADC_VREF_EN_PORT, ADC_VREF_EN_PIN);
	PinDisconnect(ADC_VREF_GND_PORT, ADC_VREF_GND_PIN);

	PinDisconnect(ADC_TEMP_VCC_PORT, ADC_TEMP_VCC_PIN);
	PinDisconnect(ADC_TEMP_GND_PORT, ADC_TEMP_GND_PIN);

	switch(cmd)
	{
		case ENABLE_TEMP: 
			GPIO_InitStruct.Pin = ADC_TEMP_VCC_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(ADC_TEMP_VCC_PORT, &GPIO_InitStruct);
			HAL_GPIO_WritePin(ADC_TEMP_VCC_PORT, ADC_TEMP_VCC_PIN, GPIO_PIN_SET);   // TEMP VDD Connect and ON

			GPIO_InitStruct.Pin = ADC_TEMP_GND_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(ADC_TEMP_GND_PORT, &GPIO_InitStruct);
			HAL_GPIO_WritePin(ADC_TEMP_GND_PORT, ADC_TEMP_GND_PIN, GPIO_PIN_RESET); // TEMP GND Connect and OFF
			break;

		case ENABLE_VCC:
			GPIO_InitStruct.Pin = ADC_VREF_EN_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(ADC_VREF_EN_PORT, &GPIO_InitStruct);
			HAL_GPIO_WritePin(ADC_VREF_EN_PORT, ADC_VREF_EN_PIN, GPIO_PIN_SET);     // VREF EN Connect and ON

			//GPIO_InitStruct.Pin = ADC_VREF_GND_PIN;
			//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			//GPIO_InitStruct.Pull = GPIO_NOPULL;
			//GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			//HAL_GPIO_Init(ADC_VREF_GND_PORT, &GPIO_InitStruct);
			//HAL_GPIO_WritePin(ADC_VREF_GND_PORT, ADC_VREF_GND_PIN, GPIO_PIN_RESET); // VREF GND Connect and OFF
			break;
	}
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

	// Temp / VREF - Analog
	GPIO_InitStruct.Pin = ADC_VREF_TEMP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VREF_TEMP_PORT, &GPIO_InitStruct);

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
	HAL_GPIO_WritePin(ADC_TEMP_VCC_PORT, ADC_TEMP_VCC_PIN, GPIO_PIN_RESET);

	// VREF_EN - PP (1 - active)
	GPIO_InitStruct.Pin = ADC_VREF_EN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VREF_EN_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VREF_EN_PORT, ADC_VREF_EN_PIN, GPIO_PIN_RESET);

	// VREF GND - OD
	GPIO_InitStruct.Pin = ADC_VREF_GND_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VREF_GND_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VREF_GND_PORT, ADC_VREF_GND_PIN, GPIO_PIN_SET);

	// GND для входов тока и напряжения включены, для отключения - записать в GND пины единицу
	// вход Temp / VIN настроен на VREF, для переключения - вызвать TempVinConfig()

	TempVinConfig(ENABLE_VCC);
}

void ConversionInitADC(void)
{
	ADC_HandleTypeDef hadc1 = { 0 };
	ADC_ChannelConfTypeDef ch = { 0 };
	DMA_HandleTypeDef hdma = { 0 };

	// An IO booster block has been added to boost the voltage on the command of those analog
	// switches when the VBAT goes below a threshold(2.7V) to guarantee the good behavior of
	// those switches.
	LL_RCC_IOBOOST_Enable();

	__HAL_RCC_DMA_CLK_ENABLE();
	
	hdma.Instance = DMA1_Channel1;
	hdma.Init.Request = DMA_REQUEST_ADC1_DS;
	hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma.Init.MemInc = DMA_MINC_ENABLE;
	hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma.Init.Mode = DMA_CIRCULAR;
	hdma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	if(HAL_DMA_Init(&hdma) != HAL_OK) { Error_Handler(); }

	DMA1_Channel1->CCR &= ~DMA_CCR_EN;

	DMA1->IFCR = DMA_IFCR_CTCIF1;

	DMA1_Channel1->CNDTR = DMA_BUF_COUNT;
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DS_DATAOUT;
	DMA1_Channel1->CMAR = (uint32_t)dmadata;
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	// SampleRate, DownSamplerConfig.DataRatio и число каналов подобрано так, 
	// чтобы между выборками всех каналов было 500 мкс
	// ADC CLK = 32000000
	// us = 1000000 / (CLK / SampleRate / DownSampler / Channels)
	hadc1.Instance = ADC1;
	hadc1.Init.ConversionType = ADC_CONVERSION_WITH_DS;
	hadc1.Init.SequenceLength = ADC_CH_COUNT;
	hadc1.Init.SamplingMode = ADC_SAMPLING_AT_START;
	hadc1.Init.SampleRate = ADC_SAMPLE_RATE_100;
	hadc1.Init.InvertOutputMode = ADC_DATA_INVERT_NONE;
	hadc1.Init.Overrun = ADC_NEW_DATA_IS_LOST;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_12_BIT;
	hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_32;
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
		ch.CalibrationPoint.Gain = LL_ADC_DEFAULT_RANGE_VALUE_3V6;
		ch.CalibrationPoint.Offset = 0UL;
	}

	ch.Channel = ADC_CURRENT_CH;
	ch.Rank = ADC_RANK_1; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ch.Channel = ADC_CURRENT_CH;
	ch.Rank = ADC_RANK_2; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ch.Channel = ADC_VOLTAGE_CH;
	ch.Rank = ADC_RANK_3; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ch.Channel = ADC_VOLTAGE_CH;
	ch.Rank = ADC_RANK_4; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ch.Channel = ADC_VREF_TEMP_CH;
	ch.Rank = ADC_RANK_5; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ADC1->CONF |= ADC_CONF_DMA_DS_ENA;

	HAL_ADC_Start(&hadc1);
}

void ConversionGetPowerVoltage(void)
{
	ADC_HandleTypeDef hadc1 = { 0 };
	ADC_ChannelConfTypeDef ch = { 0 };
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// VREF GND - OD
	GPIO_InitStruct.Pin = ADC_VREF_GND_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VREF_GND_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VREF_GND_PORT, ADC_VREF_GND_PIN, GPIO_PIN_RESET);

	// VIN_EN (1 - active)
	GPIO_InitStruct.Pin = ADC_VREF_EN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VREF_EN_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_VREF_EN_PORT, ADC_VREF_EN_PIN, GPIO_PIN_SET);

	LL_PWR_SetNoPullA(LL_PWR_GPIO_BIT_8 | LL_PWR_GPIO_BIT_9);

	// VIN_TEMP - Analog
	GPIO_InitStruct.Pin = ADC_VREF_TEMP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ADC_VREF_TEMP_PORT, &GPIO_InitStruct);

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
	hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_64;
	if(HAL_ADC_Init(&hadc1) != HAL_OK) { Error_Handler(); }
	
	ch.Channel = ADC_VREF_TEMP_CH;
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

	conv.U = urms[0];
	conv.Ufast = udc[0];
}

short ConversionFindTemperature(int adc)
{
	static const short temper[53] =
	{
		3373, 3360, 3342, 3318, 3287, 3245, 3192, 3126, 3044, 2946, 
		2832, 2701, 2554, 2395, 2226, 2051, 1874, 1700, 1531, 1371, 
		1222, 1084, 959, 847, 746, 657, 579, 510, 450, 397, 351, 
		311, 276, 245, 218, 195, 174, 156, 140, 126, 114, 103, 
		93, 85, 77, 70, 64, 59, 54, 49, 45, 42, 39, 
	};

	int index = 1;
	int t_delta; // размер диапазона
	int t_value; // значение от начала диапазона
	short t_min; // начало диапазона

	if(adc >= temper[0])
		return (-800);
	if(adc <= temper[array_length(temper) - 1])
		return (2200);

	// найдем диапазон в который попала температура
	while(adc < temper[index])
	{
		if(index + 10 < array_length(temper) && adc < temper[index + 10])
			index += 10;
		else
			index++;
	}

	// кусочно-линейная интерполяция
	t_min = -600 + index * 50;
	t_value = (adc - temper[index]) * 50;
	t_delta = (temper[index] - temper[index - 1]);
	return t_min + divr(t_value, t_delta / 2, t_delta);
}

void ConversionProcessAverage(void)
{
	if(++conv.Kslow >= PPP)
	{
		conv.Kslow = 0;

		int N = 30;
		float rN = 1.0f / N;

		int KavN = conv.Kav - N; if(KavN < 0) KavN += AV_P_CNT; // k - N
		int Kav = conv.Kav; // индекс для макроса AVERAGE (x[k])
		int Ks = conv.KavS + 1 >= N; // флаг стабилизации усредняющих фильтров 

		// усреднение скользящим окном
		// x[k] = x0 / N; y[k] = x[k] - x[k - N] + y[k - 1], 
		// один раз за период усреднения сумма вычисляется напрямую
		// и подставляется в рекурсивный фильтр - это устраняет накопление ошибки
#define AVERAGE(Y, X0, X, S) { \
			float x = X0 * rN; S += x; \
			if(Ks)                     \
				{ Y = S; S = 0; }      \
			else                       \
				Y = x - X[KavN] + Y;   \
			X[Kav] = x; }

		int Kws = conv.Kws;

		conv.Ufast = sqrtf(conv.SU[Kws]); conv.SU[Kws] = 0;
		conv.Ifast = sqrtf(conv.SI[Kws]); conv.SI[Kws] = 0;

		AVERAGE(conv.Uraw, conv.Ufast, conv.UPrev, conv.US);
		AVERAGE(conv.Iraw, conv.Ifast, conv.IPrev, conv.IS);

		conv.U = conv.Uraw * 10;
		conv.I = conv.Iraw * 0.202f;

		if(++conv.Kws >= WS_CNT) conv.Kws = 0;
		if(++conv.Kav >= AV_P_CNT) conv.Kav = 0;
		if(++conv.KavS >= N) conv.KavS = 0;
	}
}

void ConversionProcess(void)
{
	if(conv.TVCCstate < 1 * WLEN / 4) { if(conv.TVCCstate == 0) TempVinConfig(ENABLE_TEMP); }
	else if(conv.TVCCstate < 2 * WLEN / 4) conv.st += adcdata[2];
	else if(conv.TVCCstate < 3 * WLEN / 4) { if(conv.TVCCstate == WLEN / 2) TempVinConfig(ENABLE_VCC); }
	else if(conv.TVCCstate < 4 * WLEN / 4) conv.svcc += adcdata[2];

	conv.TVCCstate++;

	conv.is2 = conv.is1; conv.is1 = conv.is0;
	conv.us2 = conv.us1; conv.us1 = conv.us0;

	conv.is0 = ((short)adcdata[0]) * conv.KI;
	conv.us0 = ((short)adcdata[1]) * conv.KU;

	if(conv.Init == 0)
	{
		conv.is2 = conv.is0; conv.is1 = conv.is0;
		conv.us2 = conv.us0; conv.us1 = conv.us0;

		for(int i = 0; i < VCOUNT; i++)
		{
			conv.vi[i] = conv.is0;
			conv.vu[i] = conv.us0;
		}

		conv.Init = 1;
	}

	// фильтр постоянной составляющей
	// x[k] - 2 * x[k-1] + x[k-2] + 2.00008 * y[k-1] - 0.9862 * y[k-2]
	int K0 = conv.Kv;
	int K1 = conv.Kv - 1; if(K1 < 0) K1 += VCOUNT; // K - 1
	int K2 = conv.Kv - 2; if(K2 < 0) K2 += VCOUNT; // K - 2

	conv.vi[K0] = conv.M * (conv.is0 - 2 * conv.is1 + conv.is2 + conv.A * conv.vi[K1] - conv.B * conv.vi[K2]);
	conv.vu[K0] = conv.M * (conv.us0 - 2 * conv.us1 + conv.us2 + conv.A * conv.vu[K1] - conv.B * conv.vu[K2]);

	float vi = conv.vi[K0];
	float vu = conv.vu[K0];

	for(int s = 0; s < WS_CNT; s++)
	{
		// окна усреднения сдвинуты друг от друга на N точек, 
		// каждый период по очереди выбирается одно из окон, после чего обнуляется
		float w = window[(conv.Kw - (s - (WS_CNT - 1)) * PPP) % WLEN];

		// фильтры RMS
		conv.SI[s] += vi * vi * w;
		conv.SU[s] += vu * vu * w;
	}

	ConversionProcessAverage();

	if(++conv.Kv >= VCOUNT) conv.Kv = 0;

	if(++conv.Kw >= WLEN)
	{
		conv.Kw = 0;

		conv.VCCraw = conv.svcc / (WLEN / 4);
		conv.VCC = conv.VCCraw * 3120 / 3498; // 3.120 -> 3498
		conv.Temp = ConversionFindTemperature(conv.st / (WLEN / 4) * 2900 / conv.VCCraw); // 2900 - подгоночный параметр

		conv.st = 0;
		conv.svcc = 0;

		conv.TVCCstate = 0;
	}
}

unsigned long delta;
unsigned long timer;
unsigned long point;
int pointCount;
unsigned short iraw[400];
unsigned short uraw[400];

void ConversionMain(void)
{
	int pcount = 0;

	while(conv.NDTR != DMA1_Channel1->CNDTR)
	{
		pcount++;
		rawdata[conv.CH] = dmadata[DMA_BUF_COUNT - conv.NDTR];

		conv.CH++;
		if(conv.CH >= ADC_CH_COUNT)
		{
			conv.CH = 0;

			delta = GLOBAL_DELTA_TIME(timer);
			timer = GLOBAL_TIMER;

			adcdata[0] = (rawdata[0] + rawdata[1]) * 3500 / 2 / conv.VCCraw;
			adcdata[1] = (rawdata[2] + rawdata[3]) * 3500 / 2 / conv.VCCraw;
			adcdata[2] = rawdata[4];

			iraw[point] = adcdata[1];
			uraw[point] = adcdata[1];

			if(++point >= 400)
				point = 0;

			ConversionProcess();

			conv.time = GLOBAL_DELTA_TIME(timer);
		}

		conv.NDTR--;
		if(conv.NDTR == 0) 
			conv.NDTR = DMA_BUF_COUNT;
	}

	if(pcount > 1)
		pointCount = pcount;
}

void ConversionInit(void)
{
	conv.NDTR = DMA_BUF_COUNT;

	conv.VCCraw = 3500;

	conv.KI = 1.0f;
	conv.KU = 1.0f;

	#define T0 (0.0005f)
	#define A0 (280 / (9971 * T0))
	#define B0 (4 / (9971 * T0 * T0))
	#define M0 (49848.0f / 49855)

	float T = 1.0f / 50 / PPP;

	conv.A = (8 - 2 * B0 * T * T) / (4 * M0);
	conv.B = (B0 * T * T + 4 - 2 * A0 * T) / (4 * M0);
	conv.M = (4 * M0) / (B0 * T * T + 2 * A0 * T + 4);
	conv.T = T;

	//ConversionGetPowerVoltage();
	
	ConversionInitGPIO();
	ConversionInitADC();
}
