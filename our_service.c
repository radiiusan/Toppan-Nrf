
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "our_service.h"
#include "ble_srv_common.h"
#include "app_error.h"

ble_gattc_evt_write_rsp_t mob_rsp;
ble_gattc_evt_hvx_t mob_data;

/**@brief Function for handling BLE GATTS EVENTS
 * 
 * This function prints out data that is received when you try to write to your characteristic or CCCD. 
 * In general it is a bad idea to do so much printf stuff and UART transfer inside the BLE handler,
 * but this is just for demonstrate purposes.
 *
 * @param[in]   p_our_service        Our Service structure.
 * @param[in]   p_ble_evt            BLE event passed from BLE stack
 *
 */
/*
static void on_ble_write(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
    // Declare buffer variable to hold received data. The data can only be 32 bit long.
    uint32_t data_buffer;
    // Populate ble_gatts_value_t structure to hold received data and metadata.
    ble_gatts_value_t rx_data;
    rx_data.len = sizeof(uint32_t);
    rx_data.offset = 0;
    rx_data.p_value = (uint8_t*)&data_buffer;
    
    // Check if write event is performed on our characteristic or the CCCD
    if(p_ble_evt->evt.gatts_evt.params.write.handle == p_our_service->char_handles.value_handle)
    {
        // Get data
        sd_ble_gatts_value_get(p_our_service->conn_handle, p_our_service->char_handles.value_handle, &rx_data);
        // Print handle and value 
        printf("Value received on handle %#06x: %#010x\r\n", p_ble_evt->evt.gatts_evt.params.write.handle, data_buffer);
    }
    else if(p_ble_evt->evt.gatts_evt.params.write.handle == p_our_service->char_handles.cccd_handle)
    {
        // Get data
        sd_ble_gatts_value_get(p_our_service->conn_handle, p_our_service->char_handles.cccd_handle, &rx_data);
        // Print handle and value 
        printf("Value received on handle %#06x: %#06x\r\n", p_ble_evt->evt.gatts_evt.params.write.handle, data_buffer);
        if(data_buffer == 0x0001)
        {
            printf("Notification enabled\r\n");
        }
        else if(data_buffer == 0x0000)
        {
            printf("Notification disabled\r\n");
        }
    }
}
*/
	/*
typedef struct
{
  ble_evt_hdr_t header;                 // Event header.
  union
  {
    ble_common_evt_t  common_evt;         // Common Event, evt_id in BLE_EVT_* series.
    ble_gap_evt_t     gap_evt;            // GAP originated event, evt_id in BLE_GAP_EVT_* series.
    ble_l2cap_evt_t   l2cap_evt;          // L2CAP originated event, evt_id in BLE_L2CAP_EVT* series.
    ble_gattc_evt_t   gattc_evt;          // GATT client originated event, evt_id in BLE_GATTC_EVT* series.
    ble_gatts_evt_t   gatts_evt;          // GATT server originated event, evt_id in BLE_GATTS_EVT* series.
  } evt;
} ble_evt_t;
typedef struct
{
  uint16_t conn_handle;                                       // Connection Handle on which the event occurred.
  union
  {
    ble_gatts_evt_write_t                 write;              // Write Event Parameters.
    ble_gatts_evt_rw_authorize_request_t  authorize_request;  // Read or Write Authorize Request Parameters.
    ble_gatts_evt_sys_attr_missing_t      sys_attr_missing;   // System attributes missing.
    ble_gatts_evt_hvc_t                   hvc;                // Handle Value Confirmation Event Parameters.
    ble_gatts_evt_timeout_t               timeout;            // Timeout Event.
  } params;                                                   // Event Parameters.
} ble_gatts_evt_t;
typedef struct
{
  uint16_t            conn_handle;                // Connection Handle on which event occured.
  uint16_t            gatt_status;                // GATT status code for the operation, see @ref BLE_GATT_STATUS_CODES.
  uint16_t            error_handle;               // In case of error: The handle causing the error. In all other cases @ref BLE_GATT_HANDLE_INVALID.
  union
  {
    ble_gattc_evt_prim_srvc_disc_rsp_t          prim_srvc_disc_rsp;         // Primary Service Discovery Response Event Parameters.
    ble_gattc_evt_rel_disc_rsp_t                rel_disc_rsp;               // Relationship Discovery Response Event Parameters.
    ble_gattc_evt_char_disc_rsp_t               char_disc_rsp;              // Characteristic Discovery Response Event Parameters.
    ble_gattc_evt_desc_disc_rsp_t               desc_disc_rsp;              // Descriptor Discovery Response Event Parameters.
    ble_gattc_evt_attr_info_disc_rsp_t          attr_info_disc_rsp;         // Attribute Information Discovery Event Parameters.
    ble_gattc_evt_char_val_by_uuid_read_rsp_t   char_val_by_uuid_read_rsp;  // Characteristic Value Read by UUID Response Event Parameters.
    ble_gattc_evt_read_rsp_t                    read_rsp;                   // Read Response Event Parameters.
    ble_gattc_evt_char_vals_read_rsp_t          char_vals_read_rsp;         // Characteristic Values Read Response Event Parameters.
    ble_gattc_evt_write_rsp_t                   write_rsp;                  // Write Response Event Parameters.
    ble_gattc_evt_hvx_t                         hvx;                        // Handle Value Notification/Indication Event Parameters.
    ble_gattc_evt_timeout_t                     timeout;                    // Timeout Event Parameters.
  } params;                                                                 // Event Parameters. @note Only valid if @ref gatt_status == @ref BLE_GATT_STATUS_SUCCESS.
} ble_gattc_evt_t;
typedef struct
{
  uint16_t            handle;         // Handle to which the HVx operation applies.
  uint8_t             type;           // Indication or Notification, see @ref BLE_GATT_HVX_TYPES.
  uint16_t            len;            // Attribute data length.
  uint8_t             data[];         // Attribute data, variable length.
} ble_gattc_evt_hvx_t;

*/

static void on_recv_data(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
    // Declare buffer variable to hold received data
    uint8_t *data_buffer;
		uint32_t i, tmp;
    
    printf("Characteristic handle for which data is received %#x\r\n", p_ble_evt->evt.gattc_evt.params.hvx.handle);
    printf("Characteristic handle in the client %#x\r\n", p_our_service->char_handles.value_handle);
		printf("CCCD handle in the client %#x\r\n", p_our_service->char_handles.cccd_handle);
		data_buffer = p_ble_evt->evt.gattc_evt.params.hvx.data;
    // Check if write event is performed on our characteristic or the CCCD
    //if(p_ble_evt->evt.gattc_evt.params.hvx.handle == p_our_service->char_handles.value_handle)
    {
				printf("Received length %x\r\n", p_ble_evt->evt.gattc_evt.params.hvx.len);
				for (i=4; i > 0; i--)
				{
					printf("Value received %#x\r\n", data_buffer[i]);
				}
    }
    //else if(p_ble_evt->evt.gattc_evt.params.hvx.handle == p_our_service->char_handles.cccd_handle)
    {
			  tmp = *((uint32_t *) data_buffer);
        if(tmp == 0x0001)
        {
            printf("Notification enabled\r\n");
        }
        else if(tmp == 0x0000)
        {
            printf("Notification disabled\r\n");
        }
    }
}

// ALREADY_DONE_FOR_YOU: Declaration of a function that will take care of some housekeeping of ble connections related to our service and characteristic
void ble_our_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
		static uint8_t  resp[] = { 'K', 'O', 0x02, 0xE0 };
		uint8_t i;

    // OUR_JOB: Step 3.D Implement switch case handling BLE events related to our service. 
    switch (p_ble_evt->header.evt_id)
    {   
			
        case BLE_GATTC_EVT_HVX:
						printf("Received event BLE_GATTC_EVT_HVX\n\r");
						mob_data = p_ble_evt->evt.gattc_evt.params.hvx;
            printf("Attribute handle is %x:\r\n", mob_data.handle);
            printf("Type is %x:\r\n", mob_data.type);
            printf("length is %x:\r\n", mob_data.len);
						for (i=0; i<mob_data.len; i++)
							printf("Data is %x:\r\n", mob_data.data[i]);
            //on_recv_data(p_our_service, p_ble_evt);
						sd_ble_gattc_hv_confirm(p_our_service->conn_handle, p_our_service->char_handles.value_handle);
						our_temperature_characteristic_update(p_our_service, (int32_t *)resp);
						resp[0]++;
            break;

        case BLE_GAP_EVT_CONNECTED:
						printf("Received event BLE_GAP_EVT_CONNECTED\n\r");
            p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
						printf("Connected\n\r");
						our_temperature_characteristic_update(p_our_service, (int32_t *)resp);
						resp[0] = 0;
						resp[1] = 0;
						resp[2] = 0x02;
						resp[3] = 0;
						//printf("Connection handle %#x:\r\n", p_ble_evt->evt.gap_evt.conn_handle);
						break;
        case BLE_GAP_EVT_DISCONNECTED:
						printf("Received event BLE_GAP_EVT_DISCONNECTED\n\r");
            p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
				/*
				typedef struct
				{
					uint16_t            handle;           // Attribute Handle
					uint8_t             write_op;         // Type of write operation, see @ref BLE_GATT_WRITE_OPS
					uint16_t            offset;           // Data offset
					uint16_t            len;              // Data length
					uint8_t             data[];           // Data, variable length
				} ble_gattc_evt_write_rsp_t;
				*/
				
        case BLE_GATTC_EVT_WRITE_RSP:
						printf("Received event BLE_GATTC_EVT_WRITE_RSP\n\r");					
						mob_rsp = p_ble_evt->evt.gattc_evt.params.write_rsp;
            printf("Attribute handle is %x:\r\n", mob_rsp.handle);
            printf("write_op is %x:\r\n", mob_rsp.write_op);
            printf("offset is %x:\r\n", mob_rsp.offset);
            printf("length is %x:\r\n", mob_rsp.len);
						for (i=0; i<mob_rsp.len; i++)
							printf("Data is %x:\r\n", mob_rsp.data[i]);
						
            break;
				
        default:
						printf("Received event %#x\n\r", p_ble_evt->header.evt_id);
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */

static uint32_t our_char_add(ble_os_t *p_our_service)
{
    uint32_t   err_code = 0; // Variable to hold return codes from library and softdevice functions
    
    // OUR_JOB: Step 2.A, Add a custom characteristic UUID
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_CHARACTERISTIC_BASE_UUID;
    char_uuid.uuid      = BLE_UUID_OUR_CHARACTERISTC_UUID;
		char_uuid.type			= BLE_UUID_TYPE_VENDOR_BEGIN;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
		//printf("Char add: %#x %#x\n\r", err_code, char_uuid.type);
    APP_ERROR_CHECK(err_code);
    
    // OUR_JOB: Step 2.F Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    // OUR_JOB: Step 3.A, Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
    
    // OUR_JOB: Step 2.B, Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md)); 
    attr_md.vloc        = BLE_GATTS_VLOC_STACK;   
 
    // OUR_JOB: Step 2.G, Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // OUR_JOB: Step 2.C, Configure the characteristic value attribute
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));        
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    
    // OUR_JOB: Step 2.H, Set characteristic length in number of bytes
    attr_char_value.max_len     = 4;
    attr_char_value.init_len    = 4;
    uint8_t value[4]            = {0x12,0x34,0x56,0x78};
    attr_char_value.p_value     = value;
		
    // OUR_JOB: Step 2.E, Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_our_service->char_handles);
		//printf("Char add: %#x\n\r", err_code);
    APP_ERROR_CHECK(err_code);
    
    printf("Service handle: %#x\n\r", p_our_service->service_handle);
    printf("Service connection handle: %#x\n\r", p_our_service->conn_handle);
    printf("Char value handle: %#x\r\n", p_our_service->char_handles.value_handle);
    printf("Char cccd handle: %#x\r\n\r\n", p_our_service->char_handles.cccd_handle);

    return NRF_SUCCESS;
}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void our_service_init(ble_os_t *p_our_service)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_SERVICE_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
		//printf("Service add: %#x %#x\n\r", err_code, service_uuid.type);

    APP_ERROR_CHECK(err_code);    
    
    // OUR_JOB: Step 3.B, Set our service connection handle to default value. I.e. an invalid handle since we are not yet in a connection.
    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // FROM_SERVICE_TUTORIAL: Add our service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_our_service->service_handle);
    
		//printf("Service add: %#x\n\r", err_code);
    APP_ERROR_CHECK(err_code);
    
    // OUR_JOB: Call the function our_char_add() to add our new characteristic to the service. 
   our_char_add(p_our_service);

}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
void our_temperature_characteristic_update(ble_os_t *p_our_service, int32_t *temperature_value)
{
	/*
		typedef struct
		{
			uint16_t                    handle;             // Attribute Handle
			uint8_t                     op;                 // Type of write operation, see @ref BLE_GATTS_OPS
			uint8_t                     auth_required;      // Writing operation deferred due to authorization requirement. Application may use @ref sd_ble_gatts_value_set to finalise the writing operation
			ble_gatts_attr_context_t    context;            // Attribute Context
			uint16_t                    offset;             // Offset for the write operation
			uint16_t                    len;                // Length of the received data
			uint8_t                     data[];             // Received data, variable length
		} ble_gatts_evt_write_t;
	*/
    // OUR_JOB: Step 3.E, Update characteristic value
		//printf("service handle: %#x\n\r", p_our_service->conn_handle);
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        //uint16_t               len = 4;
        //ble_gatts_hvx_params_t hvx_params;
        //memset(&hvx_params, 0, sizeof(hvx_params));

        //hvx_params.handle = p_our_service->char_handles.value_handle;
        //hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        //hvx_params.offset = 0;
        //hvx_params.p_len  = &len;
        //hvx_params.p_data = (uint8_t*)temperature_value;  

        //sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);

				uint32_t                 err_code;
				ble_gattc_write_params_t write_params;

				write_params.write_op = BLE_GATT_OP_WRITE_REQ;
				write_params.handle   = p_our_service->char_handles.value_handle;
				write_params.offset   = 0;
				write_params.len      = sizeof(temperature_value);
				write_params.p_value  = (uint8_t*)temperature_value;

				err_code = sd_ble_gattc_write(p_our_service->conn_handle, &write_params);
				APP_ERROR_CHECK(err_code);
				printf("Value transmitted: %#04x\n\r", *temperature_value);
			
    }   
}
