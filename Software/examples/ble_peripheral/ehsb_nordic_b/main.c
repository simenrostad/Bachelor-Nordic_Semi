#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_nvic.h"                  //Include for use of Radio Notification Software Interrupts.

#define APP_BLE_CONN_CFG_TAG             1                                /**< A tag identifying the SoftDevice BLE configuration. */
#define EHSB_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN       /**< UUID type for the Nordic UART Service (vendor specific). */  
#define EHSB_BASE_UUID                   {{0x9F, 0xCA, 0xDC, 0x24,\
                                           0x0E, 0xE5, 0xA9, 0xE0,\
                                           0x93, 0xF3, 0xA3, 0xB5,\
                                           0x00, 0x00, 0x40, 0x6F}}       /**< Base UUID */  //0x9F, 0xCA, 0xDC, 0x24,/ 0x0E, 0xE5, 0xA9, 0xE0,/ 0x93, 0xF3, 0xA3, 0xB5,/ 0x00, 0x00, 0x40, 0x6F

#define BLE_UUID_EHSB_SERVICE             0x0001                          /**< The UUID of the Nordic UART Service. */

uint8_t counter = 0;

static ble_uuid_t m_adv_uuids[]          =                                /**< Universally unique service identifier. */
{
    {BLE_UUID_EHSB_SERVICE, EHSB_SERVICE_UUID_TYPE}
};

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

    ble_uuid128_t ehsb_base_uuid = EHSB_BASE_UUID;
    err_code = sd_ble_uuid_vs_add(&ehsb_base_uuid, &m_adv_uuids[0].type);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    memset(&m_adv_params, 0, sizeof(m_adv_params));

    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;          /**< Scannable undirected event, only allowed to respond to scan requests. */
    m_adv_params.p_peer_addr = NULL;                                      /**< Undirected advertisement. */
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;                        /**< Allow scan requests and connect requests from any device. */
    m_adv_params.interval    = BLE_GAP_ADV_INTERVAL_MIN;                  /**< Setting min advertising interval 20ms */
    m_adv_params.timeout     = 0;                                         /**< Never time out. Want to send as soon as the chip is powered */

    err_code = sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the BLE stack. */
static void ble_stack_init(void)                                          
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

/**@brief Function for doing power management. */
static void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/**@brief Software interrupt 1 IRQ Handler, handles radio notification interrupts.
 This gets called everytime an advertisement event has happened and will stop advertising
 after 5 advertising events*/
void SWI1_IRQHandler(bool radio_evt)
{
    uint32_t err_code;
    if (radio_evt)
    {
        counter += 1;

        if(counter == 5)
        {
            err_code = sd_ble_gap_adv_stop();
            APP_ERROR_CHECK(err_code);
        }
        
    }
}

/**@brief Function for initializing Radio Notification Software Interrupts.
 We will use this to limit the number of advertisement events and get a more realistic test
 while testing with the development kits*/
uint32_t radio_notification_init(uint32_t irq_priority, uint8_t notification_type, uint8_t notification_distance)
{
    uint32_t err_code;

    err_code = sd_nvic_ClearPendingIRQ(SWI1_IRQn);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = sd_nvic_SetPriority(SWI1_IRQn, irq_priority);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = sd_nvic_EnableIRQ(SWI1_IRQn);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Configure the event
    return sd_radio_notification_cfg_set(notification_type, notification_distance);
}

int main(void)
{
    // Initialize.
    log_init();
    ble_stack_init();
//    advertising_init();
    
    uint32_t err_code;
    err_code = radio_notification_init(5, NRF_RADIO_NOTIFICATION_TYPE_INT_ON_INACTIVE, NRF_RADIO_NOTIFICATION_DISTANCE_NONE);   //Initializing Radio Notification Software Interrupts.
    APP_ERROR_CHECK(err_code);

    advertising_init();

    NRF_LOG_INFO("Starting EHSB project");
/**< Sleep between advertising intervals */
    for (;; )
    {
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}
