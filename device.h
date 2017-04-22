#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef void (*device_rx_cb)(void*, size_t, size_t);

bool device_init(void);

bool device_destroy(void);

bool device_set_frequency(uint64_t freq);

bool device_set_sample_rate(uint64_t sample_rate);

bool device_set_gain(uint32_t gain);

bool device_is_alive();

bool device_rx(device_rx_cb cb_fn);


#endif //__DEVICE_H__
