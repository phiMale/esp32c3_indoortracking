

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "rssi3.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *manuf_name = "Espressif ESP32C3 devkitM1";
static const char *model_num = "ESP BLE RSSI Demo";
uint16_t hrs_hrm_handle;

static int gatt_svr_chr_access_rssi(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static int gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
	{
		// service: RSSI
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = BLE_UUID16_DECLARE(GATT_RSSI_UUID),
		.characteristics = (struct ble_gatt_chr_def[])
		{ {
				// characteristic: RSSI measurement
				.uuid = BLE_UUID16_DECLARE(GATT_RSSI_MEASUREMENT_UUID),
				.access_cb = gatt_svr_chr_access_rssi,
				.val_handle = &hrs_hrm_handle,
				.flags = BLE_GATT_CHR_F_NOTIFY,
			}, {
			0, // add characteristics, e.g. RSSI measurements from different esps
			},
		}
	},

	{
		// service: device info
		.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
		.characteristics = (struct ble_gatt_chr_def[])
		{ {
				// characteristic: manufacturer name
				.uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
				.access_cb = gatt_svr_chr_access_device_info,
				.flags = BLE_GATT_CHR_F_READ,
			}, {
				// characteristic: model number str
				.uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
				.access_cb = gatt_svr_chr_access_device_info,
				.flags = BLE_GATT_CHR_F_READ,
			}, {
				0, // no more characteristics
			},
		}

	},

	{
		0, // no more services
	},
};


static int gatt_svr_chr_access_rssi(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

// if central requests device info, this fn sends out corr. data, fedched from conn_handle, attr_handle
static int gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == GATT_MODEL_NUMBER_UUID) {
        rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg){
	char buf[BLE_UUID_STR_LEN];

	switch (ctxt->op) {
	case BLE_GATT_REGISTER_OP_SVC:
		MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
					ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
					ctxt->svc.handle);
		break;

	case BLE_GATT_REGISTER_OP_CHR:
		MODLOG_DFLT(DEBUG, "registering characteristic %s with "
					"def_handle=%d val_handle=%d\n",
					ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
					ctxt->chr.def_handle,
					ctxt->chr.val_handle);
		break;

	case BLE_GATT_REGISTER_OP_DSC:
		MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
					ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
					ctxt->dsc.handle);
		break;

	default:
		assert(0);
		break;
	}
}

int gatt_svr_init(void){
	int rc;
	ble_svc_gap_init();
	ble_svc_gatt_init();
	rc = ble_gatts_count_cfg(gatt_svr_svcs);
	if (rc != 0){
		return rc;
	}
	return 0;
}










