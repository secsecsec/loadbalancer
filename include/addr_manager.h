#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#include <net/nic.h>
#define ADDRS		"net.lb.addrs" 

bool addr_alloc(NIC* ni, uint32_t addr);
bool addr_free(NIC* ni, uint32_t addr);

#endif /*__PRIVATE_H__*/
