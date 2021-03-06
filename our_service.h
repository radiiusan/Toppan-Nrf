
#ifndef OUR_SERVICE_H__
#define OUR_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

// FROM_SERVICE_TUTORIAL: Defining 16-bit service and 128-bit base UUIDs
// 128-bit base UUID
#define BLE_UUID_OUR_SERVICE_BASE_UUID              {{0x00, 0x00, 0x96, 0x76, 0x0B, 0xCD, 0x5A, 0xA9, 0x3F, 0x4E, 0x9C, 0x8B, 0x00, 0x00, 0x47, 0x11}}
#define BLE_UUID_OUR_SERVICE_UUID					0xC156
// 128-bit base UUID 
#define BLE_UUID_OUR_CHARACTERISTIC_BASE_UUID       {{0x01, 0x00, 0x96, 0x76, 0x0B, 0xCD, 0x5A, 0xA9, 0x3F, 0x4E, 0x9C, 0x8B, 0x00, 0x00, 0x47, 0x11}}
#define	BLE_UUID_OUR_CHARACTERISTC_UUID		0xC156

// This structure contains various status information for our service. 
// The name is based on the naming convention used in Nordics SDKs. 
// 'ble' indicates that it is a Bluetooth Low Energy relevant structure and 
// 'os' is short for Our Service.

typedef struct
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
    // OUR_JOB: Step 2.D, Add handles for the characteristic attributes to our struct
    ble_gatts_char_handles_t    char_handles;
} ble_os_t;

extern ble_os_t							 						 m_our_service;
/**@brief Function for handling BLE Stack events related to our service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to Our Service.
 *
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_our_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void our_service_init(ble_os_t * p_our_service);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_our_service                     Our Service structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void our_characteristic_update(ble_os_t *p_our_service);
uint16_t get_ble_data(void);
void put_ble_data(ble_os_t *p_our_service);
void power_manage(void);
void nstrcpy(char *dst, char *src, uint16_t n);

#endif  /* _ OUR_SERVICE_H__ */
