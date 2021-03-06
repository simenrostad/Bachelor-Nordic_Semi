
/**<Defines*/
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define APP_BLE_CONN_CFG_TAG               1                                /**< A tag identifying the SoftDevice BLE configuration. */
#define EHSB_SERVICE_UUID_TYPE             0x02                             /**< Vendor spesific UUID.*/  
#define EHSB_BASE_UUID                   {{0x9F, 0xCA, 0xDC, 0x24, \
                                           0x0E, 0xE5, 0xA9, 0xE0, \
                                           0x93, 0xF3, 0xA3, 0xB5, \
                                           0x00, 0x00, 0x40, 0x6E}}         /**< Base UUID */

#define BLE_UUID_EHSB_SERVICE             0x0001                            /**< The UUID of the EHSB Service. */

static ble_uuid_t m_adv_uuids[]          =                                  /**< Universally unique service identifier. */
{
    {BLE_UUID_EHSB_SERVICE, EHSB_SERVICE_UUID_TYPE}
};

/**< Parameters to be passed to the stack when starting advertising. */
static void advertising_init(void)
{
    uint32_t             err_code;
    ble_advdata_t        advdata;
    uint8_t              flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    ble_gap_adv_params_t m_adv_params;  

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.flags                   = flags;
    advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = m_adv_uuids;

    // Add UUID to softdevice
    ble_uuid128_t ehsb_base_uuid = EHSB_BASE_UUID;
    err_code = sd_ble_uuid_vs_add(&ehsb_base_uuid, &m_adv_uuids[0].type);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND; /**< Non connectable undirected. Device is only broadcasting its information.  */
    m_adv_params.p_peer_addr = NULL;                             /**< Undirected advertisement. */
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;               /**< Allow scan requests and connect requests from any device. */
    m_adv_params.interval    = BLE_GAP_ADV_INTERVAL_MIN;         /**< Setting min advertising interval 20ms. Want to send as many packets possible in the shortest amount of time. */
    m_adv_params.timeout     = 0;                                /**< Never time out. Want to send as soon as the chip is powered. */

    // Start advertising 128-bit UUID.
    err_code = sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the BLE stack. */
static void ble_stack_init(void)                                          
{
    ret_code_t err_code;

    //Enable the softdevice
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
    
    // Enable DCDC converter in softdevice
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

}

/**@brief Function for doing power management. */
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

int main(void)
{
    // Initialize.
    ble_stack_init();
    advertising_init();

/**< Sleep between advertising intervals */
    for (;;)
    {
      power_manage();
    }
}
