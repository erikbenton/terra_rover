#include "server.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "sd_card.h"
#include "camera.h"
#include "esp_log.h"
#include "cJSON.h"

#define PHOTO_NAME_LEN 200
#define PHOTO_PATH_LEN 255

static const char *TAG = "SITE";
static httpd_handle_t server = NULL;
static sd_card_t *sd_card;
static camera_t *camera;

void start_mdns_service(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        mdns_init());
    mdns_hostname_set("terra");
    mdns_instance_name_set("Terra Rover");
}

static esp_err_t take_photo(camera_t *camera, sd_card_t *sd_card, const char *photo_name)
{
    esp_err_t err;

    // path to save the photo
    char photo_path[PHOTO_PATH_LEN];
    memset(photo_path, 0, sizeof(photo_path));

    sprintf(photo_path, "/store/photos/%s.jpg", photo_name);

    ESP_LOGI(TAG, "Take photo path: %s", photo_path);

    err = camera->grab_photo(camera);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Unable to get photo from frame buffer");
        return err;
    }

    err = sd_card->write_data_file(sd_card, photo_path, camera->current_photo->buf, camera->current_photo->len);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Unable to save photo to SD card");
        return err;
    }

    err = camera->release_photo(camera);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Unable to get release photo to frame buffer");
        return err;
    }

    return ESP_OK;
}

static esp_err_t on_take_photo_url(httpd_req_t *req)
{
    ESP_LOGI(TAG, "URL: %s", req->uri);

    esp_err_t err;

    // buffer for the body
    char buffer[PHOTO_NAME_LEN];
    memset(buffer, 0, sizeof(buffer));

    // get photo name from body
    if (req->content_len < PHOTO_NAME_LEN)
    {
        httpd_req_recv(req, buffer, req->content_len);
        cJSON *payload = cJSON_Parse(buffer);
        if (payload != NULL)
        {
            cJSON *photo_name_json = cJSON_GetObjectItem(payload, "photo_name");
            const char *photo_name = cJSON_GetStringValue(photo_name_json);
            ESP_LOGI(TAG, "Photo name: %s", photo_name);
            err = take_photo(camera, sd_card, photo_name);
            if (err != ESP_OK)
            {
                // send Bad Request
            }

            cJSON *body_json = cJSON_CreateObject();
            cJSON_AddStringToObject(body_json, "photo_name", photo_name);
            char *body = cJSON_Print(body_json);
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_send(req, body, HTTPD_RESP_USE_STRLEN);
            cJSON_Delete(body_json);
        }

        cJSON_Delete(payload);
    }

    return ESP_OK;
}

static esp_err_t on_get_photo_url(httpd_req_t *req)
{
    ESP_LOGI(TAG, "URL: %s", req->uri);
    const char *photo_name = strrchr(req->uri, '/');
    ESP_LOGI(TAG, "photo name: %s", photo_name);

    sd_card->stream_web_files(sd_card, "photos", photo_name, req);

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(TAG, "URL: %s", req->uri);

    sd_card->stream_web_files(sd_card, "site", req->uri, req);

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

void server_init(sd_card_t *sd_card_ptr, camera_t *camera_ptr)
{
    sd_card = sd_card_ptr;
    camera = camera_ptr;

    start_mdns_service();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_ERROR_CHECK(httpd_start(&server, &config));

    httpd_uri_t take_photo_url = {
        .uri = "/api/takephoto",
        .method = HTTP_POST,
        .handler = on_take_photo_url};

    httpd_uri_t get_photo_url = {
        .uri = "/api/photos/*",
        .method = HTTP_GET,
        .handler = on_get_photo_url};

    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url};

    httpd_register_uri_handler(server, &take_photo_url);
    httpd_register_uri_handler(server, &get_photo_url);
    httpd_register_uri_handler(server, &default_url);
}