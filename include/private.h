#ifndef __PRIVATE_H__
#define __PRIVATE_H__

#include <net/ni.h>
#include <util/list.h>
#include <util/map.h>

#include "endpoint.h"

#define PRIVATES	"net.lb.privates"

typedef struct _Private {
	Endpoint	endpoint;
	uint16_t	pointing_count;
} Private;

Private* private_alloc(Endpoint* private_endpoint);
void private_free(Private* _private);

#endif /*__PRIVATE_H__*/
