
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include <string.h>
#include "esp_bt_device.h"



#define RSSI_TAG "RSSI"


#define ESP1



#ifdef ESP1
#define THIS_ESP "ESP11"
#define ESP_A "ESP22"
#define ESP_B "ESP33"
#endif

#ifdef ESP2
#define THIS_ESP "ESP22"
#define ESP_A "ESP33"
#define ESP_B "ESP11"
#endif

#ifdef ESP3
#define THIS_ESP "ESP33"
#define ESP_A "ESP11"
#define ESP_B "ESP22"
#endif




static uint8_t adv_config_done = 0;
#define adv_config_flag      (1 << 0)


static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x0500,
    .adv_int_max        = 0x0500,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x050,
    .max_interval = 0x060,
    .appearance = 0x00,
    .manufacturer_len = 2,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);


static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x0010,
    .scan_window            = 0x0010,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_ENABLE // culprit
};

static void start_scan(void)
{
    uint32_t duration = 0;
    esp_ble_gap_start_scanning(duration);
}

static bool which_esp(char* rxd_name, char* esp_a, char* esp_b, size_t name_len){

	if (rxd_name != NULL && strlen(esp_a) == name_len && strncmp((char *)rxd_name, esp_a, name_len) == 0) {
		return 0;
	}
	else if(rxd_name != NULL && strlen(esp_b) == name_len && strncmp((char *)rxd_name, esp_b, name_len) == 0){
		return 1;
	}
	else{
		return 3;
	}
}


static uint8_t rssi_values[2];


void set_adv_rssi_data(int8_t rxd_rssi, int slot){

	rssi_values[slot] = rxd_rssi;


	esp_ble_adv_data_t adv_data2 = {
	    .set_scan_rsp = false,
	    .include_name = true,
	    .include_txpower = false,
	    .min_interval = 0x0050,
	    .max_interval = 0x0060,
	    .appearance = 0x00,
	    .manufacturer_len = 0,
	    .p_manufacturer_data =  NULL,
	    .service_data_len = 2,
	    .p_service_data = &rssi_values[0],
	    .service_uuid_len = 0,
	    .p_service_uuid = NULL,
	    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
	};

	esp_err_t r = esp_ble_gap_config_adv_data(&adv_data2);

	if(r != ESP_OK){
		ESP_LOGI(RSSI_TAG, "could not set adv data. ");
	}
	else{
		ESP_LOGI(RSSI_TAG, "adv data set: %d | %d", *(adv_data2.p_service_data), *(adv_data2.p_service_data + sizeof(uint8_t)));
	}
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){

    int8_t adv_rssi = 0;
    uint8_t adv_name_len = 6;
    uint8_t* adv_name = malloc(adv_name_len);
    uint8_t adv_man_data_len = 2;
    uint8_t* adv_man_data = malloc(adv_name_len);

    if(event == ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT){
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0){
            esp_ble_gap_start_advertising(&adv_params);
        }
    }


    else if(event == ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT){
    	ESP_LOGI(RSSI_TAG, "ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT");
    }
	else if(event == ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT){
        start_scan();
        ESP_LOGI(RSSI_TAG, "ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT");
    }
	else if(event == ESP_GAP_BLE_SCAN_START_COMPLETE_EVT){
		ESP_LOGI(RSSI_TAG, "ESP_GAP_BLE_SCAN_START_COMPLETE_EVT");
	}
	else if(event == ESP_GAP_BLE_SCAN_RESULT_EVT){
			esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
			if(scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {

				adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
				adv_man_data = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv, ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE, &adv_man_data_len);


				adv_rssi = scan_result->scan_rst.rssi;
				int esp = which_esp((char*) adv_name, ESP_A, ESP_B, adv_name_len);
				if(esp != 3){
					vTaskDelay(20/portTICK_PERIOD_MS);
					set_adv_rssi_data(adv_rssi, esp);

				}
			}
	}
}



void app_main(void){
	while(1){
		esp_err_t ret = nvs_flash_init();
		if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
		}
		ESP_ERROR_CHECK( ret );

		ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BTDM));

		esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
		ret = esp_bt_controller_init(&bt_cfg);
		if (ret) {
			ESP_LOGE(RSSI_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
			return;
		}
		else{
			ESP_LOGI(RSSI_TAG, "init controller success");
		}

		ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
		if (ret) {
			ESP_LOGE(RSSI_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
			return;
		}

		ret = esp_bluedroid_init();
		if (ret) {
			ESP_LOGE(RSSI_TAG, "%s init bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
			return;
		}

		ret = esp_bluedroid_enable();
		if (ret) {
			ESP_LOGE(RSSI_TAG, "%s enable bluetooth failed: %s\n", __func__, esp_err_to_name(ret));
			return;
		}


		ret = esp_ble_gap_register_callback(esp_gap_cb);
		if (ret){
			ESP_LOGE(RSSI_TAG, "gap register error, error code = %x", ret);
			return;
		}

		esp_ble_gap_set_device_name(THIS_ESP);
		esp_ble_gap_config_adv_data(&adv_data);
		esp_ble_gap_set_scan_params(&ble_scan_params);

	}
}

