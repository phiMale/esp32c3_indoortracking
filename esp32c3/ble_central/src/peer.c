

#include <assert.h>
#include <string.h>
#include "host/ble_hs.h"
#include "blecent.h"

static void *peer_svc_mem;
static struct os_mempool peer_svc_pool;

static void *peer_chr_mem;
static struct os_mempool peer_chr_pool;

static void *peer_dsc_mem;
static struct os_mempool peer_dsc_pool;

static void *peer_mem;
static struct os_mempool peer_pool;
static SLIST_HEAD(, peer) peers;

static struct peer_svc *
peer_svc_find_range(struct peer *peer, uint16_t attr_handle);
static struct peer_svc *
peer_svc_find(struct peer *peer, uint16_t svc_start_handle,
              struct peer_svc **out_prev);
int
peer_svc_is_empty(const struct peer_svc *svc);

uint16_t
chr_end_handle(const struct peer_svc *svc, const struct peer_chr *chr);
int
chr_is_empty(const struct peer_svc *svc, const struct peer_chr *chr);
static struct peer_chr *
peer_chr_find(const struct peer_svc *svc, uint16_t chr_def_handle,
              struct peer_chr **out_prev);
static void
peer_disc_chrs(struct peer *peer);

static int
peer_dsc_disced(uint16_t conn_handle, const struct ble_gatt_error *error,
                uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                void *arg);


static void
peer_free_mem(void)
{
    free(peer_mem);
    peer_mem = NULL;

    free(peer_svc_mem);
    peer_svc_mem = NULL;

    free(peer_chr_mem);
    peer_chr_mem = NULL;

    free(peer_dsc_mem);
    peer_dsc_mem = NULL;
}

int
peer_init(int max_peers, int max_svcs, int max_chrs, int max_dscs)
{
    int rc;

    /* Free memory first in case this function gets called more than once. */
    peer_free_mem();

    peer_mem = malloc(
                   OS_MEMPOOL_BYTES(max_peers, sizeof (struct peer)));
    if (peer_mem == NULL) {
        rc = BLE_HS_ENOMEM;
        goto err;
    }

    rc = os_mempool_init(&peer_pool, max_peers,
                         sizeof (struct peer), peer_mem,
                         "peer_pool");
    if (rc != 0) {
        rc = BLE_HS_EOS;
        goto err;
    }

    peer_svc_mem = malloc(
                       OS_MEMPOOL_BYTES(max_svcs, sizeof (struct peer_svc)));
    if (peer_svc_mem == NULL) {
        rc = BLE_HS_ENOMEM;
        goto err;
    }

    rc = os_mempool_init(&peer_svc_pool, max_svcs,
                         sizeof (struct peer_svc), peer_svc_mem,
                         "peer_svc_pool");
    if (rc != 0) {
        rc = BLE_HS_EOS;
        goto err;
    }

    peer_chr_mem = malloc(
                       OS_MEMPOOL_BYTES(max_chrs, sizeof (struct peer_chr)));
    if (peer_chr_mem == NULL) {
        rc = BLE_HS_ENOMEM;
        goto err;
    }

    rc = os_mempool_init(&peer_chr_pool, max_chrs,
                         sizeof (struct peer_chr), peer_chr_mem,
                         "peer_chr_pool");
    if (rc != 0) {
        rc = BLE_HS_EOS;
        goto err;
    }

    peer_dsc_mem = malloc(
                       OS_MEMPOOL_BYTES(max_dscs, sizeof (struct peer_dsc)));
    if (peer_dsc_mem == NULL) {
        rc = BLE_HS_ENOMEM;
        goto err;
    }

    rc = os_mempool_init(&peer_dsc_pool, max_dscs,
                         sizeof (struct peer_dsc), peer_dsc_mem,
                         "peer_dsc_pool");
    if (rc != 0) {
        rc = BLE_HS_EOS;
        goto err;
    }

    return 0;

err:
    peer_free_mem();
    return rc;
}
