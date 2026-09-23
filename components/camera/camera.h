#ifndef __ONBOARD_CAMERA_H__
#define __ONBOARD_CAMERA_H__

#include "esp_camera.h"
#include "esp_err.h"

typedef struct CAMERA_STRUCT camera_t;

struct CAMERA_STRUCT
{
    camera_fb_t *current_photo;                 // current photo from frame buffer
    esp_err_t (*init)(void);                    // initialize camera
    esp_err_t (*grab_photo)(camera_t *self);    // get the current photo from the frame buffer
    esp_err_t (*release_photo)(camera_t *self); // release the current photo from the frame buffer
    void *ctx;
};

esp_err_t camera_init(camera_t *camera, void *ctx);

#endif