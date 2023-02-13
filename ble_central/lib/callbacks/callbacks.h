#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "nvs_flash.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "blecent.h"

static void blecent_scan(void);
static int blecent_gap_event(struct ble_gap_event *event, void *arg);
void blecent_on_sync(void);
void blecent_host_task(void *param);

#endif