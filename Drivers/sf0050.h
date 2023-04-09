#ifndef __DRIVERS_SF0050_H__
#define __DRIVERS_SF0050_H__

#ifdef __cplusplus
extern "C"
{
#endif

void sf0050_usart_init(void * restrict handler);
void sf0050_usart_deinit(void * restrict handler);
void sf0050_usart_data(void * restrict handler, uint8_t data);
uint32_t sf0050_get_peak_depth(void * restrict handler);
uint32_t sf0050_get_first_depth(void * restrict handler);
uint32_t sf0050_get_last_depth(void * restrict handler);

#ifdef __cplusplus
}
#endif

#endif
