#ifndef __CONVERSION_H__
#define __CONVERSION_H__

#define PPP 80         // число выборок на период 
#define WLEN (PPP * 4) // длина окна усреднения, максимум 80 * 4

typedef struct
{    
	float KI;          // калибровочный коэффициент по току (с учетом перевода из значений АЦП)
	float KU;          // калибровочный коэффициент по напряжению (с учетом перевода из значений АЦП)

	unsigned short DCI;// смещение постоянной составляющей по току
	unsigned short DCU;// смещение постоянной составляющей по напряжению

	short Kw;          // индекс в окне усреднения

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

	unsigned long time; // время на измерения (GLOBAL_TIMER)
}TConversion;

void ConversionInit(void);
void ConversionTest(void);

extern unsigned short vin;
extern TConversion conv;

#endif
