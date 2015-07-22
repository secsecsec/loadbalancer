#ifndef __LOADBALANCER_H__
#define __LOADBALANCER_H__

#include <stdint.h>
#include <util/map.h>
#include <net/ni.h>
#include <stdbool.h>

int lb_ginit();
void lb_destroy();
bool lb_process(Packet* packet);
bool lb_is_all_destroied();
void lb_remove(uint64_t wait);
void lb_remove_force();

#endif /* __LOADBALANCER_H__ */
