#ifndef __ONBOARD_CAMERA_H__
#define __ONBOARD_CAMERA_H__

#include "esp_camera.h"
#include "esp_err.h"

typedef struct CAMERA_STRUCT camera_t;

typedef esp_err_t (*init_camera_t)(void);
typedef esp_err_t (*grab_photo_t)(camera_t *self);
typedef esp_err_t (*release_photo_t)(camera_t *self);

struct CAMERA_STRUCT
{
    camera_fb_t *current_photo;    // current photo from frame buffer
    init_camera_t init;            // initialize camera
    grab_photo_t grab_photo;       // get the current photo from the frame buffer
    release_photo_t release_photo; // release the current photo from the frame buffer
    void *ctx;
};

esp_err_t camera_init(camera_t *camera, void *ctx);

#endif