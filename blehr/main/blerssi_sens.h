#ifndef H_BLEHR_SENSOR_
#define H_BLEHR_SENSOR_

#include "nimble/ble.h"
#include "modlog/modlog.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Heart-rate configuration */
#define GATT_HRS_BODY_SENSOR_LOC_UUID 0x2A38
#define GATT_RSSI_UUID 0xBBBB
#define GATT_RSSI_MEASUREMENT_UUID 0x0600
#define GATT_DEVICE_INFO_UUID 0xCCCC
#define GATT_MANUFACTURER_NAME_UUID 0xDDDD
#define GATT_MODEL_NUMBER_UUID 0xEEEE

    extern uint16_t hrs_hrm_handle;

    struct ble_hs_cfg;
    struct ble_gatt_register_ctxt;

    void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
    int gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
