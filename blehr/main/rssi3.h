
#ifndef H_BLEHR_SENSOR_
#define H_BLEHR_SENSOR_

#include "nimble/ble.h"
#include "modlog/modlog.h"

#define GATT_RSSI_UUID                          0x69b7
#define GATT_RSSI_MEASUREMENT_UUID              0x4ddf
#define GATT_DEVICE_INFO_UUID                   0x5d54
#define GATT_MANUFACTURER_NAME_UUID             0xf04f
#define GATT_MODEL_NUMBER_UUID                  0x8162

extern uint16_t hrs_hrm_handle;

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);

#endif
