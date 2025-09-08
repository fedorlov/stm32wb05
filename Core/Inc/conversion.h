#ifndef __CONVERSION_H__
#define __CONVERSION_H__

#define PPP 80         // число выборок на период 
#define WLEN (PPP * 4) // длина окна усреднени€, максимум 80 * 4

typedef struct
{    
	float KI;          // калибровочный коэффициент по току (с учетом перевода из значений ј÷ѕ)
	float KU;          // калибровочный коэффициент по напр€жению (с учетом перевода из значений ј÷ѕ)

	unsigned short DCI;// смещение посто€нной составл€ющей по току
	unsigned short DCU;// смещение посто€нной составл€ющей по напр€жению

	short Kw;          // индекс в окне усреднени€
	short Init;        // при первом проходе инициализируем фильтры посто€нной составл€ющие

	float si;
	float su;
	int st;

	float sidc;
	float sudc;

	float Idc;
	float Udc;

	float I;
	float U;
	short T;
	short VCC;

	unsigned short NDTR; // счетчик обработанных данных - догон€ет счетчик DMA
	unsigned short CH;   // номер измеренного канала

	unsigned long time;  // врем€ на измерени€ (GLOBAL_TIMER)
}TConversion;

void ConversionMain(void);
void ConversionInit(void);
void ConversionTest(void);

extern unsigned short vin;
extern TConversion conv;

#endif
