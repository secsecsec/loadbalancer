#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define DONT_MAKE_WRAPPER
#include <_malloc.h>
#undef DONT_MAKE_WRAPPER
#include "private.h"

static Private* get_private(Endpoint* _private_endpoint) {
	Map* private_endpoints = ni_config_get(_private_endpoint->ni, PRIVATES);
	if(!private_endpoints)
		return NULL;

	return map_get(private_endpoints, (void*)(uintptr_t)_private_endpoint->addr);
}

Private* private_alloc(Endpoint* _private_endpoint) {
	bool add_private(Private* private) {
		Map* private_endpoints = ni_config_get(private->endpoint.ni, PRIVATES);
		if(!private_endpoints) {
			private_endpoints = map_create(16, NULL, NULL, private->endpoint.ni->pool);
			if(!private_endpoints)
				return false;

			if(!ni_config_put(private->endpoint.ni, PRIVATES, private))
				goto private_endpoints_free;
		}

		if(!map_put(private_endpoints, (void*)(uintptr_t)private->endpoint.addr, private)) {
			if(map_is_empty(private_endpoints))
				goto private_endpoints_remove;
		}

		return true;

private_endpoints_remove:
		ni_config_remove(private->endpoint.ni, PRIVATES);
private_endpoints_free:
		map_destroy(private_endpoints);

		return false;
	}

	Private* private = get_private(_private_endpoint);
	if(!private) {
		if(!ni_ip_add(_private_endpoint->ni, _private_endpoint->addr)) {
			printf("Can'nt add private address\n");
			return NULL;
		}

		ssize_t size = sizeof(Private);
		private = __malloc(size, _private_endpoint->ni->pool);
		if(!private) {
			printf("Can'nt allocate private endpoint\n");
			return NULL;
		}

		memcpy(&private->endpoint, _private_endpoint, size);
		private->pointing_count = 0;

		if(!add_private(private)) {
			__free(private, private->endpoint.ni->pool);
			return NULL;
		}
	}

	private->pointing_count++;

	return private;
}

void private_free(Private* _private) {
	bool remove_private(Private* private) {
		Map* privates = ni_config_get(private->endpoint.ni, PRIVATES);
		map_remove(privates, (void*)(uintptr_t)private->endpoint.addr);

		if(map_is_empty(privates)) {
			ni_config_remove(private->endpoint.ni, PRIVATES);
			map_destroy(privates);
		}
		
		return true;
	}

	_private->pointing_count--;

	if(!_private->pointing_count) {
		remove_private(_private);
		ni_ip_remove(_private->endpoint.ni, _private->endpoint.addr);
		__free(_private, _private->endpoint.ni->pool);
	}
}
