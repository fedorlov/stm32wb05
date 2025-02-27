#include "main.h"

#define ADC_CH_COUNT 2

unsigned short dmadata[ADC_CH_COUNT];
int T;
int Traw;
int Tindex;

TIM_HandleTypeDef htim2 = { 0 };

void ConversionInitTimer(void)
{
	
	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };	

	__HAL_RCC_TIM2_CLK_ENABLE();

	// קאסעמעא עאילונא 64 Ãצ
	// עאילונ סקטעאוע 250 לךס (4000 Ãצ = 0,004 לדצ)
	// הוכטעוכü = 64 לדצ / 0,004 לדצ = 16000
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 16000 - 1;
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
	hadc1.Init.SampleRate = ADC_SAMPLE_RATE_16;
	hadc1.Init.InvertOutputMode = ADC_DATA_INVERT_NONE;
	hadc1.Init.Overrun = ADC_NEW_DATA_IS_LOST;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DownSamplerConfig.DataWidth = ADC_DS_DATA_WIDTH_16_BIT;
	hadc1.Init.DownSamplerConfig.DataRatio = ADC_DS_RATIO_32;
	if(HAL_ADC_Init(&hadc1) != HAL_OK) { Error_Handler(); }

	ch.Channel = ADC_CHANNEL_TEMPSENSOR;
	ch.Rank = ADC_RANK_1;
	ch.VoltRange = ADC_VIN_RANGE_1V2;
	ch.CalibrationPoint.Number = ADC_CALIB_POINT_1;
	ch.CalibrationPoint.Gain = 0;
	ch.CalibrationPoint.Offset = 0;
	if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) { Error_Handler(); }

	ch.Channel = ADC_CHANNEL_VINP0;
	ch.VoltRange = ADC_VIN_RANGE_2V4;
	ch.CalibrationPoint.Number = ADC_CALIB_POINT_2;
	ch.CalibrationPoint.Gain = LL_ADC_GET_CALIB_GAIN_FOR_VINPX_2V4();
	ch.CalibrationPoint.Offset = LL_ADC_GET_CALIB_OFFSET_FOR_VINPX_2V4();
	if(ch.CalibrationPoint.Gain == 0xFFF)
	{
		ch.CalibrationPoint.Gain = LL_ADC_DEFAULT_RANGE_VALUE_2V4;
		ch.CalibrationPoint.Offset = 0UL;
	}

	ch.Rank = ADC_RANK_2; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();
	ch.Rank = ADC_RANK_3; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();
	ch.Rank = ADC_RANK_4; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();
	ch.Rank = ADC_RANK_5; if(HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();

	ADC1->CONF |= ADC_CONF_DMA_DS_ENA;

	HAL_ADC_Start(&hadc1);
}

void ConversionInit(void)
{
	ConversionInitADC();
	ConversionInitTimer();
}

volatile unsigned long delta;
volatile unsigned long timer;
volatile unsigned long point;
unsigned short rawvalues[400];

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_IT_UPDATE;
	
	Traw += __LL_ADC_CALC_TEMPERATURE(dmadata[0], LL_ADC_DS_DATA_WIDTH_16_BIT);
	
	Tindex++;
	
	if(Tindex >= 4000)
	{
		T = Traw / 400;
		Traw = 0;
		Tindex = 0;
	}

	rawvalues[point++] = dmadata[1];

	if(point >= 400)
		point = 0;

	delta = GLOBAL_DELTA_TIME(timer);
	timer = GLOBAL_TIMER;

	DMA1_Channel1->CCR &= ~DMA_CCR_EN;
	DMA1->IFCR = DMA_IFCR_CTCIF1;
	DMA1_Channel1->CNDTR = ADC_CH_COUNT;
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	ADC1->IRQ_STATUS = ADC_IRQ_FLAG_EODS | ADC_IRQ_FLAG_EOS | ADC_IRQ_FLAG_OVRDS;
	ADC1->CTRL |= ADC_CTRL_START_CONV;
}
