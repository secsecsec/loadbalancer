#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <net/nic.h>
#include <util/map.h>

#include "addr_manager.h"

bool addr_alloc(NIC* ni, uint32_t addr) {
	Map* addrs = nic_config_get(ni, ADDRS);
	if(!addrs) {
		addrs = map_create(16, NULL, NULL, ni->pool);
		if(!addrs)
			return false;

		if(!nic_config_put(ni, ADDRS, addrs)) {
			map_destroy(addrs);
			return false;
		}
	}

	uint64_t count = (uint64_t)map_get(addrs, (void*)(uintptr_t)addr);

	if(!count) {
		if(!nic_ip_add(ni, addr)) {
			printf("Can'nt allocate address\n");
			return false;
		}

		count++;
		return map_put(addrs, (void*)(uintptr_t)addr, (void*)(uintptr_t)count);
	}

	count++;
	return map_update(addrs, (void*)(uintptr_t)addr, (void*)(uintptr_t)count);
}

bool addr_free(NIC* ni, uint32_t addr) {
	Map* addrs = nic_config_get(ni, ADDRS);
	if(!addrs)
		return false;

	uint64_t count = (uint64_t)map_get(addrs, (void*)(uintptr_t)addr);
	count--;

	if(!count) {
		nic_ip_remove(ni, addr);
		map_remove(addrs, (void*)(uintptr_t)addr);
		if(map_is_empty(addrs)) {
			map_destroy(addrs);
			nic_config_remove(ni, ADDRS);
		}
	} else
		return map_update(addrs, (void*)(uintptr_t)addr, (void*)(uintptr_t)count);

	return true;
}
