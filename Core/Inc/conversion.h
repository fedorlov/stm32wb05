#ifndef __CONVERSION_H__
#define __CONVERSION_H__

#define PPP 80         // число выборок на период 
#define WLEN (PPP * 4) // длина окна усреднения, максимум 80 * 4

#define array_length(x) ((sizeof(x)) / sizeof(x[0]))
#define sign(x) ((x) < 0 ? -1 : 1)
#define divr(x, halfdiv, div) (((x) + sign(x) * (halfdiv)) / (div)) // знаковое деление с округлением

#define WS_CNT   4 
#define VCOUNT   4
#define AV_P_CNT 32

typedef struct
{    
	float KI;          // калибровочный коэффициент по току (с учетом перевода из значений АЦП)
	float KU;          // калибровочный коэффициент по напряжению (с учетом перевода из значений АЦП)
	
	short Kv;          // индекс фильтра постоянной составляющей
	short Kslow;       // для обновления данных раз в период
	short Init;        // при первом проходе инициализируем фильтры постоянной составляющей

	short Kw;          // индекс в окне усреднения
	short Kws;         // номер текущей суммы  
	short Kav;         // индекс текущего сэмпла в усреднении
	short KavS;        // число усредненных точек в КИХ фильтре

	// константы для фильтра постоянной составляющей
	float A;
	float B;
	float M;
	float T;

	float is0, is1, is2;
	float us0, us1, us2;

	float vi[VCOUNT];
	float vu[VCOUNT];

	float SU[WS_CNT];
	float SI[WS_CNT];

	int st;
	int svcc;

	float I;
	float Iraw;
	float Ifast;
	float IS;
	float IPrev[AV_P_CNT];

	float U;
	float Uraw;
	float Ufast;
	float US;
	float UPrev[AV_P_CNT];

	short Temp;
	short VCC;
	short VCCraw;

	unsigned short NDTR;      // счетчик обработанных данных - догоняет счетчик DMA
	unsigned short CH;        // номер измеренного канала
	unsigned short TVCCstate; // для измерения T и VCC через один канал

	unsigned long time;  // время на измерения (GLOBAL_TIMER)
}TConversion;

void ConversionMain(void);
void ConversionInit(void);
void ConversionTest(void);

extern unsigned short vin;
extern TConversion conv;

#endif
