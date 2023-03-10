

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
// BLE
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "blerssi_sens.h"

//#define ADV_TIME_ITVL (uint16_t) 2000


static const char *tag = "NimBLE_BLE_RSSI";

static const char *device_name = "IT_ESP_4";
static const uint16_t adv_time_itvl = 2000;
static uint8_t blerssi_addr_type;

void print_bytes(const uint8_t *bytes, int len){
    for (int i = 0; i < len; i++){
        MODLOG_DFLT(INFO, "%s0x%02x", i != 0 ? ":" : "", bytes[i]);
    }
}

void print_addr(const void *addr){
    const uint8_t *u8p = addr;
    MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
                u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

/*
 * Enables advertising with parameters:
 *     o General discoverable mode
 *     o Undirected connectable mode
 */

// ENABLE ADVERTISING
static void blerssi_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info)
     *     o Advertising tx power
     *     o Device name
     */
    memset(&fields, 0, sizeof(fields)); // memset to set multiple bits (e.g. of a struct to zero)

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    
    fields.flags = BLE_HS_ADV_F_DISC_GEN * BLE_HS_ADV_F_BREDR_UNSUP; // basic rate (bt 1.x), enhanced data rate (bt 2.1) DISABLED

    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    fields.adv_itvl_is_present = 1;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = 69;
    fields.adv_itvl = adv_time_itvl;
    // fields.svc_data_uuid16_len = 4;
    // fields.svc_data_uuid16 = 

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));   // sets all bits of adv_params to zero
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON; // set to NON instead of UND
    adv_params.disc_mode = BLE_GAP_DISC_MODE_NON;

    
    adv_params.itvl_max = 0;
    adv_params.itvl_min = 0;
    rc = ble_gap_adv_start(blerssi_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, blerssi_advertise, NULL);
    if (rc != 0)
    {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}

// ADVERTISING
static void blerssi_on_sync(void)
{
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

static void blerssi_on_reset(int reason)
{
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

// runs in background
void blerssi_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void app_main(void)
{
    int rc;
    const uint8_t m[] = "I do be running doe\n";

    print_bytes(m, 21);

    /* Initialize NVS ??? used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());

    // inits all services, creates timer, sets name, starts task

    nimble_port_init();
    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = blerssi_on_sync;
    ble_hs_cfg.reset_cb = blerssi_on_reset;

    //    /* name, period/time,  auto reload, timer ID, callback */
    //    blerssi_tx_timer = xTimerCreate("blerssi_tx_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, blerssi_tx_hrate);

    rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);

    /* Start the task */
    nimble_port_freertos_init(blerssi_host_task);
}