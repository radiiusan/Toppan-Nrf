
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "our_service.h"
#include "ble_srv_common.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "tlv.h"

#define	FRAMESIZE	20

uint32_t   err_code;
ble_gatts_hvx_params_t hvx_params;
ble_gap_conn_params_t conn_params_req;
// Declare buffer variable to hold received frame data. Should not be more than 20
char data_buffer[20];
char full_data[1024];
bool start_of_pkt = true;
bool cccd_msg_rcvd = false;
bool Tlvformed = false;
bool notif_indic_enabled = false;
uint16_t rem_pktlen;
uint16_t pktlen;
uint8_t fragment;

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
*/

void nstrcpy(char *dst, char *src, uint16_t n)
{
	uint16_t i;
	
	for (i=0; i<n; i++)
		*dst++ = *src++;
}

static void on_ble_write(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
		uint8_t i;
    // Populate ble_gatts_value_t structure to hold received data and metadata.
    ble_gatts_value_t rx_data;
    rx_data.len = sizeof(data_buffer);
    rx_data.offset = 0;
    rx_data.p_value = (uint8_t *)data_buffer;
    
    // Check if write event is performed on our characteristic or the CCCD
    if(p_ble_evt->evt.gatts_evt.params.write.handle == p_our_service->char_handles.value_handle)
    {
        // Get data
        sd_ble_gatts_value_get(p_our_service->conn_handle, p_our_service->char_handles.value_handle, &rx_data);
        // Print value
				
/*				printf("Value handle data received\r\n");
        printf("Value handle data: 0x");
				for (i=0; i<rx_data.len; i++)
					printf("%02x", rx_data.p_value[i]);
				printf("\r\n");
*/				
				cccd_msg_rcvd = false;
    }
    else if(p_ble_evt->evt.gatts_evt.params.write.handle == p_our_service->char_handles.cccd_handle)
    {
        // Get data
        sd_ble_gatts_value_get(p_our_service->conn_handle, p_our_service->char_handles.cccd_handle, &rx_data);
        // Print value
        //printf("CCCD handle data: 0x%02x%02x\r\n", rx_data.p_value[1], rx_data.p_value[0]);
				notif_indic_enabled = true;
        if(rx_data.p_value[0] == 0x01)
					printf("Notification enabled\r\n");
				else if(rx_data.p_value[0] == 0x02)
					printf("Indication enabled\r\n");
        else if(rx_data.p_value[0] == 0x00)
				{
					notif_indic_enabled = false;
					printf("Notification/Indication disabled\r\n");
				}
				cccd_msg_rcvd = true;
    }
		printf("\r\n");
}

void reassemble(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
		static uint16_t inx = 0;
		uint32_t time_from, time_to, time_diff;
	
		app_timer_cnt_get(&time_from);
    on_ble_write(p_our_service, p_ble_evt);
		if (start_of_pkt)
		{
			// data_buffer has the structure (tag, length, data) for the first pkt.
			// Subsequent pkts all have only data
			if (cccd_msg_rcvd)
			{
				rem_pktlen = 2;
				printf("Received CCCD message 0x%02x%02x\r\n", data_buffer[1], data_buffer[0]);
			}
			else
			{
				rem_pktlen = (data_buffer[1] << 8) + data_buffer[2] + 3;
				printf("Tag %#x Pkt len %#x\r\n", data_buffer[0], rem_pktlen);
			}
			pktlen = rem_pktlen;
			//printf("start_of_pkt %d inx %d rem_pktlen %d\r\n", start_of_pkt, inx, rem_pktlen);
			start_of_pkt = false;
			fragment = 1;
		}
		if (rem_pktlen >= FRAMESIZE)
		{
			//printf("start_of_pkt %d inx %d rem_pktlen %d\r\n", start_of_pkt, inx, rem_pktlen);
			nstrcpy(&full_data[inx], data_buffer, FRAMESIZE);
			rem_pktlen -= FRAMESIZE;
			if (rem_pktlen == 0)
			{
				inx = 0;
				start_of_pkt = true;
				printf("Received last frame %d\r\n", fragment);
				fragment++;
				printf("Data of length %d bytes received\r\n", pktlen);
			}
			else
			{
				//printf("Received frame %d Remaining data %d\r\n", fragment, rem_pktlen);
				fragment++;
				inx += FRAMESIZE;
			}
		}
		else
		{
			printf("Received last frame %d\r\n", fragment);
			fragment++;
			//printf("start_of_pkt %d inx %d rem_pktlen %d\r\n", start_of_pkt, inx, rem_pktlen);
			nstrcpy(&full_data[inx], data_buffer, rem_pktlen);
			printf("Data of length %d bytes received \r\n", pktlen);
			rem_pktlen = 0;
//			pktlen = 0;
			inx = 0;
			start_of_pkt = true;
			Tlvformed = true;
		}
		app_timer_cnt_get(&time_to);
		if (time_to <= time_from)
			time_to = 32768 - time_from + time_to;
		app_timer_cnt_diff_compute(time_to, time_from, &time_diff);
		printf("Time in microsecs for single frame receive %.2f\r\n", time_diff * 30.52);
}

bool fragment_write(ble_os_t *p_our_service)
{
		static uint16_t inx = 0;

		if (start_of_pkt)
		{
			rem_pktlen = (full_data[1] << 8) + full_data[2];
			rem_pktlen += 3;
			start_of_pkt = false;
			fragment = 1;
		}
		//printf("start_of_pkt %d rem_pktlen %d inx %d\r\n", start_of_pkt, rem_pktlen, inx);
		if (rem_pktlen >= FRAMESIZE)
		{
			nstrcpy(data_buffer, &full_data[inx], FRAMESIZE);
			our_characteristic_update(p_our_service);
			inx += FRAMESIZE;
			rem_pktlen -= FRAMESIZE;
			if (rem_pktlen == 0)
			{
				inx = 0;
				printf("Sent last frame %d\r\n", fragment);
				fragment++;
				start_of_pkt = true;
				return(false);
			}
			//printf("Sent frame %d\r\n", fragment);
			fragment++;
			return(true);
		}
		else
		{
			//printf("start_of_pkt %d rem_pktlen %d inx %d\r\n", start_of_pkt, rem_pktlen, inx);
			nstrcpy(data_buffer, &full_data[inx], rem_pktlen);
			our_characteristic_update(p_our_service);
			printf("Sent last frame %d\r\n", fragment);
			fragment++;
			rem_pktlen = 0;
			inx = 0;
			start_of_pkt = true;
			return(false);
		}
}
uint16_t get_ble_data(void)
{
		Tlvformed = false;
		while (true)
		{
			if (cccd_msg_rcvd)
				// Ignore. This would have been handled in on_ble_write
				;
			if (Tlvformed)
			{
//				uint8_t i;
//				for(i=0;i<pktlen;i++)
//					printf("%x ",full_data[i]);				
				ProcessTLV();				
				return(pktlen);
			}
			else
				power_manage();
		}
}

void put_ble_data(ble_os_t *p_our_service)
{
		uint16_t len = 97, i;
		uint32_t time_from, time_to, time_diff, time_interval;
	
		app_timer_cnt_get(&time_from);
//		full_data[0] = 0xE2;
//		full_data[1] = (len & 0xFF00) >> 8;
//		full_data[2] = len & 0x00FF;
//		for (i=3; i<len+3; i++)
//			full_data[i] = i-2;
		len=full_data[2]+3;
		printf("\r\n");	
		for(i=0;i<len;i++)
			printf("%x ",full_data[i]);	
		printf("Sending from device, len %#x\r\n", len);
		// for Notifications
		if (! notif_indic_enabled)
		{
			printf("Notification/Indication not yet enabled\r\n");
			return;
		}
		while (fragment_write(p_our_service))
			;
		// for Indications
		//fragment_write(p_our_service)
		app_timer_cnt_get(&time_to);
		if (time_to <= time_from)
			time_to = 32768 - time_from + time_to;
		app_timer_cnt_diff_compute(time_to, time_from, &time_diff);
		time_interval = (float) time_diff / 32768.0 * 1000.0;
		printf("Time in microsecs for packet send %.2f\r\n", time_diff * 30.52);
}

void ble_our_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
		//static uint32_t	comm_limit = 0;
		uint8_t i;
		static int8_t count = 1;
		//uint16_t len = 7;
	
    switch (p_ble_evt->header.evt_id)
    {   
        case BLE_GAP_EVT_CONNECTED:
						printf("Received event BLE_GAP_EVT_CONNECTED\n\r\n\r");
            p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
						count = 1;
						break;
				
        case BLE_GATTS_EVT_WRITE:
						//printf("Received event BLE_GATTS_EVT_WRITE\n\r");
						reassemble(p_our_service, p_ble_evt);
						/*
						if (start_of_pkt && comm_limit < 10)
						{
							printf("Sending from device, len %#x, count %d\r\n", len, count);
							full_data[0] = 0xE2;
							full_data[1] = (len & 0xFF00) >> 8;
							full_data[2] = len & 0x00FF;
							for (i=3; i<len+3; i++)
								full_data[i] = i-2;
							put_ble_data(p_our_service);
							//while (fragment_write(p_our_service)) ;
							//fragment_write(p_our_service);
							comm_limit++;
							//comm_limit = 0;	// disabling the limit
							count++;
						}
						*/

            break;

        case BLE_GAP_EVT_DISCONNECTED:
						printf("Received event BLE_GAP_EVT_DISCONNECTED\n\r\n\r");
            p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GATTS_EVT_HVC:
						printf("Received event BLE_GATTS_EVT_HVC\n\r\n\r");
						if (! start_of_pkt) fragment_write(p_our_service);
            break;

				case BLE_GAP_EVT_CONN_PARAM_UPDATE:
						/*
						typedef struct
						{
							uint16_t min_conn_interval;         // Minimum Connection Interval in 1.25 ms units
							uint16_t max_conn_interval;         // Maximum Connection Interval in 1.25 ms units
							uint16_t slave_latency;             // Slave Latency in number of connection events
							uint16_t conn_sup_timeout;          // Connection Supervision Timeout in 10 ms units
						} ble_gap_conn_params_t;
						*/
						//printf("Received event BLE_GAP_EVT_CONN_PARAM_UPDATE\n\r");
						conn_params_req = p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params;
						//printf("min_conn_interval %#06x\r\n", conn_params_req.min_conn_interval);
						//printf("max_conn_interval %#06x\r\n", conn_params_req.max_conn_interval);
						//printf("\r\n");
						break;

				case BLE_EVT_TX_COMPLETE:
						//printf("Received event BLE_EVT_TX_COMPLETE\n\r\n\r");
						break;

        default:
            // No implementation needed.
						printf("Received event %#x\n\r\n\r", p_ble_evt->header.evt_id);
            break;
    }
}

/**@brief Function for adding our new characterstic to "Our service". 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */

static uint32_t our_char_add(ble_os_t *p_our_service)
{   
    // Add a custom characteristic UUID
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_CHARACTERISTIC_BASE_UUID;
    char_uuid.uuid      = BLE_UUID_OUR_CHARACTERISTC_UUID;
		char_uuid.type			= BLE_UUID_TYPE_VENDOR_BEGIN;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
		//printf("Char add: %#x %#x\n\r", err_code, char_uuid.type);
    APP_ERROR_CHECK(err_code);
    
    // Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;
    
    // Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;
    
    // Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md)); 
    attr_md.vloc        = BLE_GATTS_VLOC_STACK;   
 
    // Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // Configure the characteristic value attribute
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));        
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    
    // Set characteristic length in number of bytes
    attr_char_value.max_len     = 20;
    attr_char_value.init_len    = 2;
    uint8_t value[2]            = {0xE0,0x00};
    attr_char_value.p_value     = value;
		
    // Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_our_service->char_handles);
		//printf("Char add: %#x\n\r", err_code);
    APP_ERROR_CHECK(err_code);
    
    //printf("Service handle: %#x\n\r", p_our_service->service_handle);
    //printf("Service connection handle: %#x\n\r", p_our_service->conn_handle);
    printf("Characteristic value handle: %#x\r\n", p_our_service->char_handles.value_handle);
    printf("Characteristic CCCD handle: %#x\r\n\r\n", p_our_service->char_handles.cccd_handle);

    return NRF_SUCCESS;
}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void our_service_init(ble_os_t *p_our_service)
{
    // Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_SERVICE_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
		//printf("Service add: %#x %#x\n\r", err_code, service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    // Set our service connection handle to default value. i.e., an invalid handle since we are not yet in a connection.
    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add our service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_our_service->service_handle);  
		//printf("Service add: %#x\n\r", err_code);
    APP_ERROR_CHECK(err_code);
    
    // Call the function our_char_add() to add our new characteristic to the service. 
   our_char_add(p_our_service);
}

// Function to be called when updating characteristic value
void our_characteristic_update(ble_os_t *p_our_service)
{
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t						len = 20;
			
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_our_service->char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
				hvx_params.p_data = (uint8_t*)data_buffer;  

				err_code = sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
				//printf("Error code for writing by device: %x\n\r", err_code);
				APP_ERROR_CHECK(err_code);
    }   
}
