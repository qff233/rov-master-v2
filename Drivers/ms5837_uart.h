#ifndef __DRIVERS_MS5837_H__
#define __DRIVERS_MS5837_H__

#ifdef __cplusplus
extern "C"
{
#endif

void ms5837_usart_init(void * restrict handler);
void ms5837_usart_deinit(void * restrict handler);
void ms5837_usart_data(void * restrict handler, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
