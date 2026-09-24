#ifndef __SERVER_H__
#define __SERVER_H__

#include "sd_card.h"
#include "camera.h"

void server_init(sd_card_t *sd_card_ptr, camera_t *camera_ptr);

#endif