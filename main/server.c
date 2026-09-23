#include "server.h"
#include "esp_http_server.h"
#include "mdns.h"
#include "sd_card.h"
#include "esp_log.h"

static const char *TAG = "SITE";
static httpd_handle_t server = NULL;
static sd_card_t *sd_card;

void start_mdns_service(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        mdns_init());
    mdns_hostname_set("terra");
    mdns_instance_name_set("Terra Rover");
}

static esp_err_t on_default_url(httpd_req_t *req)
{
    ESP_LOGI(TAG, "URL: %s", req->uri);

    sd_card->stream_web_files(sd_card, req);

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

void server_init(sd_card_t *sd_card_ptr)
{
    sd_card = sd_card_ptr;

    start_mdns_service();

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_ERROR_CHECK(httpd_start(&server, &config));

    httpd_uri_t default_url = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = on_default_url};

    httpd_register_uri_handler(server, &default_url);
}