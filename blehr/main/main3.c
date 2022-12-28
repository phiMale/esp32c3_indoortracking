#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
//#include "ble_gap.h"
//#include "esp_gap_ble_api"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "rssi3.h"

static const char *tag = "NimBLE_BLE_RSSI";

static xTimerHandle blerssi_tx_timer;

static bool notify_state;

static uint16_t conn_handle;

static const char *device_name = "blerssi_ESP_A_1.0";

//static int blerssi_gap_event(struct ble_gap_event *event, void *arg);

static uint8_t blerssi_addr_type;

// var to simulate distance when not found
static uint8_t distance = 0;




void print_addr(const void *addr){
	const uint8_t *u8p;

	u8p = addr;
	MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
            u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

static void blerssi_advertise(void){
	struct ble_gap_adv_params adv_params;
	struct ble_hs_adv_fields fields;
	int rc;

	memset(&fields, 0, sizeof(fields));

	// general discoverability flags WITHOUT basic rate/enhanced data rate
	fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

	fields.tx_pwr_lvl_is_present = 1;
	fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

	fields.name = (uint8_t *)device_name;
	fields.name_len = strlen(device_name);
	fields.name_is_complete = 1;

	// GAP
	rc = ble_gap_adv_set_fields(&fields);
	if (rc != 0){
		MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
		return;
	}

	// begin advertising
	memset(&adv_params, 0, sizeof(adv_params));
	adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
	adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
	rc = ble_gap_adv_start(blerssi_addr_type, NULL, BLE_HS_FOREVER, &adv_params, blerssi_gap_event, NULL);
	if (rc != 0){
		MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
		return;
	}
}

static void blerssi_tx_dist_stop(void){
	xTimerStop(blerssi_tx_timer, 1000 / portTICK_PERIOD_MS);
}

// reset measurement
static void blerssi_tx_dist_reset(void){
	int rc;
	if(xTimerReset(blerssi_tx_timer, 1000 / portTICK_PERIOD_MS ) == pdPASS) {
        rc = 0;
    } else {
        rc = 1;
	assert(rc == 0);
}

static void blerssi_tx_dist(xTimerHandle ev){
	static uint8_t distm[2];
	int rc;
	struct os_mbuf *om;
	if(!notify_state){
		blerssi_tx_dist_stop();
		distance = 0;
	}

	distm[0] = 0x06;
	distm[1] = distance; //storing dummy data

	// simulation of distance data
	distance++;
	if(distance == 100){
		distance = 0;
	}

    // om = dynamic copy of above buffer; sends data, frees buffer
	om = ble_hs_mbuf_from_flat(distm, sizeof(distm));

	// REMEMBER TO EDIT 2nd PARAM NAME IN gatt_svr.c FILE!!!
	rc = ble_gattc_notify_custom(conn_handle, hrs_hrm_handle, om);

	assert(rc == 0);

	blerssi_tx_dist_reset();
}

static int blerssi_gap_event(struct ble_gap_event *event, void *arg){
	switch (event->type){
	case BLE_GAP_EVENT_CONNECT:
		// attempt to connect
		MODLOG_DFLT(INFO, "connection %s; status=%d\n",
				event->connect.status == 0 ? "established" : "failed",
				event->connect.status);

		if(event->connect.status != 0){
			//connection failed, resume advertising
			blerssi_advertise();
		}
		conn_handle = event->connect.conn_handle;
		break;

	// if disconnected, restart advertising
	case BLE_GAP_EVENT_DISCONNECT:
		MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);
		blerssi_advertise();
		break;

	// central sends "notify" cmd, fn finds fitting handle for requested data
	case BLE_GAP_EVENT_SUBSCRIBE:
		MODLOG_DFLT(INFO, "subscribe event; cur_notify=%d\n value handle; " "val_handle=%d\n", event->subscribe.cur_notify, hrs_hrm_handle);

		if(event->subscribe.attr_handle == hrs_hrm_handle){
			notify_state = event->subscribe.cur_notify;
			blerssi_tx_dist_reset();
		}
		else if(event->subscribe.attr_handle != hrs_hrm_handle){
			notify_state = event->subscribe.cur_notify;
			blerssi_tx_dist_stop();
		}
		ESP_LOGI("BLE_GAP_SUBSCRIBE_EVENT", "conn_handle from subscribe=%d", conn_handle);
		break;

	case BLE_GAP_EVENT_MTU:
		MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n", event->mtu.conn_handle, event->mtu.value);
		break;
	}

	return 0;
}

// advertising
static void blerssi_on_sync(void){
	int rc;
	rc = ble_hs_id_infer_auto(0, &blerssi_addr_type);
	assert(rc == 0);

	uint8_t addr_val[6] = {0};
	rc = ble_hs_id_copy_addr(blerssi_addr_type, addr_val, NULL);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

    // begin advertising
    blerssi_advertise();
}

static void blerssi_on_reset(int reason){
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

void blerssi_host_task(void *param){
	ESP_LOGI(tag, "BLE Host Task Started");
	/* This function will return only when nimble_port_stop() is executed */
	nimble_port_run();
	nimble_port_freertos_deinit();
}






// esp_ble_gap_read_rssi


// #include "myheaderfile.h"




void app_main(void){
    int rc;

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());

    nimble_port_init();
    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = blerssi_on_sync;
    ble_hs_cfg.reset_cb = blerssi_on_reset;

    /* name, period/time,  auto reload, timer ID, callback */
    blerssi_tx_timer = xTimerCreate("blerssi_tx_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, blerssi_tx_dist);

    rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);

    /* Start the task */
    nimble_port_freertos_init(blerssi_host_task);
}
