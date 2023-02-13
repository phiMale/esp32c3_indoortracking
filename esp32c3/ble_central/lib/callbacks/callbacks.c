#include "callbacks.h"
#include "constants.h"
#include "host/ble_hs.h"
#include "esp_log.h"
#include "host/util/util.h"

static void blecent_scan(void)
{
    uint8_t own_addr_type;
    struct ble_gap_disc_params disc_params;
    int rc;

    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0)
    {
        MODLOG_DFLT(ERROR, "error determining address type; rc=%d\n", rc);
        return;
    }

    disc_params.filter_duplicates = 1;
    disc_params.passive = 1;
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    rc = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params,
                      blecent_gap_event, NULL);
    if (rc != 0)
    {
        MODLOG_DFLT(ERROR, "Error initiating GAP discovery procedure; rc=%d\n",
                    rc);
    }
}

static int blecent_gap_event(struct ble_gap_event *event, void *arg){
    struct ble_hs_adv_fields fields;
    int rc;

    rc = ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
    if (rc != 0)
    {
        return 0;
    }

    // if adv ESP was identified (69 as unique dummy value)
    if (fields.tx_pwr_lvl == 69)
    {
        MODLOG_DFLT(INFO, "[DEBUG]: Trying to print advertisements:\n");
        MODLOG_DFLT(INFO, "[DEBUG]: Name: %s | RSSI %d\n", fields.name, event->disc.rssi);
    }
    return 0;
}

void blecent_on_sync(void)
{
    int rc;
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);
    blecent_scan();
}

void blecent_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run();

    nimble_port_freertos_deinit();
}