#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_BLE_CONN_CFG_TAG             1                                /**< A tag identifying the SoftDevice BLE configuration. */
#define EHSB_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN       /**< UUID type for the Nordic UART Service (vendor specific). */  
#define EHSB_BASE_UUID                   {{0x9F, 0xCA, 0xDC, 0x24, \
                                           0x0E, 0xE5, 0xA9, 0xE0, \
                                           0x93, 0xF3, 0xA3, 0xB5, \
                                           0x00, 0x00, 0x40, 0x6E}}       /**< Base UUID */

#define BLE_UUID_EHSB_SERVICE             0x0001                          /**< The UUID of the Nordic UART Service. */

static ble_uuid_t m_adv_uuids[]          =                                /**< Universally unique service identifier. */
{
    {BLE_UUID_EHSB_SERVICE, EHSB_SERVICE_UUID_TYPE}
};

static ble_gap_adv_params_t m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */

static void advertising_init(void)
{
    uint32_t        err_code;
    ble_advdata_t   advdata;
    uint8_t         flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.flags                   = flags;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    ble_uuid128_t ehsb_base_uuid = EHSB_BASE_UUID;
    err_code = sd_ble_uuid_vs_add(&ehsb_base_uuid, &m_adv_uuids[0].type);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;          /**< Scannable undirected event, only allowed to respond to scan requests. */
    m_adv_params.p_peer_addr = NULL;                                      /**< Undirected advertisement. */
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;                        /**< Allow scan requests and connect requests from any device. remove, possible problem */
    m_adv_params.interval    = BLE_GAP_ADV_INTERVAL_MIN;                  /**< Setting min advertising interval 20ms */
    m_adv_params.timeout     = 0;                                         /**< Never time out. Want to send as soon as the chip is powered */
}

static void advertising_start(void)                                       /**@brief Function for starting advertising.  */
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

static void ble_stack_init(void)                                          /**@brief Function for initializing the BLE stack. */
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing logging. */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
    // Initialize.
    log_init();
    ble_stack_init();
    advertising_init();
    advertising_start();

    // Start execution.
    NRF_LOG_INFO("Energy Harvesting Stop Button");
}
