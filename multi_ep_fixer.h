#ifndef MULTI_EP_FIXER_H__
#define MULTI_EP_FIXER_H__

#include "zboss_api.h"

/**@brief Function which initializes fixer methods and hooks.
 *
 * Call just this function after the Device context is registered
 * and fixer is initialised.
 * It overrides write attribute cluster hooks and endpoint handlers.
 * If application uses mentioned hooks or endpoint handlers,
 * generic method should be changed for given cluster/endpoint.
 */
void init_multi_ep_fixers();

#endif /* MULTI_EP_FIXER_H__ */