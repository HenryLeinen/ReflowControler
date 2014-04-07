/*
 * adc.h
 *
 * Created: 29.03.2014 19:27:23
 *  Author: Henry
 */ 


#ifndef ADC_H_
#define ADC_H_


#if __cplusplus
extern "C" {
#endif

typedef enum { 
	Adc_Channel0=0, 
	Adc_Channel1, 
	Adc_Channel2, 
	Adc_Channel3,
	Adc_Channel4,
	Adc_Channel5,
	Adc_Channel6,
	Adc_Channel7,
	Adc_Channel0_Gain10,
	Adc_Channel_Diff1_0_Gain10,
	Adc_Channel0_Gain200,
	Adc_Channel_Diff1_0_Gain200,
	Adc_Channel2_Gain10,
	Adc_Channel_Diff3_2_Gain10,
	Adc_Channel2_Gain200,
	Adc_Channel_Diff3_2_Gain200,
	Adc_Channel_Diff0_1_Gain1,
	Adc_Channel_Diff1_1_Gain1,
	Adc_Channel_Diff2_1_Gain1,
	Adc_Channel_Diff3_1_Gain1,
} AdcChannel_t;

extern void adc_init(void);
extern uint16_t adc_get_value(AdcChannel_t);

#if __cplusplus
};
#endif

#endif /* ADC_H_ */