#include "ble.h"
#include "crypto.h"

void CPUcontextSave(void) {}
void CPUcontextRestore(void) {}

uint32_t mul32(uint32_t a, uint32_t b)
{
	uint32_t result = a * b;     // ”множение двух чисел

	uint32_t temp = (128 << 13); // Ёквивалент MOVS r2, #128; LSLS r2, r2, #13

	temp += result;              // Ёквивалент ADDS r2, r2, r0

	temp >>= 21;                 // Ёквивалент LSRS r2, r2, #21

	return temp;                 // Ёквивалент MOV r0, r2 и возврат результата
}

uint32_t blue_unit_conversion(uint32_t time, uint32_t period_freq, uint32_t thr)
{
	if(time <= thr)
		return mul32(time, period_freq);

	uint32_t low1 = period_freq & 0xFFFF;
	uint32_t high1 = period_freq >> 16;
	uint32_t low2 = time & 0xFFFF;
	uint32_t high2 = time >> 16;

	uint32_t mul1 = high2 * high1;
	uint32_t mul2 = low2 * high2;
	uint32_t mul3 = high1 * low2;
	uint32_t mul4 = low2 * low1;

	uint32_t mid = mul2 + mul3;
	uint32_t mid_low = mid << 16;
	uint32_t mid_high = mid >> 16;

	uint32_t result_low = mid_low + mul4;
	uint32_t result_high = mul1 + mid_high + (result_low < mul4);

	uint32_t adjust = (128 << 13);
	uint64_t result = ((uint64_t)result_high << 32) | result_low;
	result += adjust;
	result >>= 21;

	return (result_high << 11) | (uint32_t)result;
}

void BLE_STACK_Tick(void) {}
tBleStatus BLE_STACK_Init(const BLE_STACK_InitTypeDef* BLE_STACK_InitStruct) { return BLE_STATUS_SUCCESS; }
uint8_t BLE_STACK_SleepCheck(void) { return 0; }
uint8_t BLE_STACK_ReadNextRadioActivity(uint32_t* NextStateSysTime) { return 0; }
void BLE_STACK_RadioHandler(uint32_t BlueInterrupt) {}

int32_t AES_CMAC_Encrypt_Init(AESCMACctx_stt* P_pAESCMACctx) { return 0; }
int32_t AES_CMAC_Encrypt_Append(AESCMACctx_stt* P_pAESCMACctx, const uint8_t* P_pInputBuffer, int32_t P_inputSize) { return 0; }
int32_t AES_CMAC_Encrypt_Finish(AESCMACctx_stt* P_pAESCMACctx, uint8_t* P_pOutputBuffer, int32_t* P_pOutputSize) { return 0; }

void Osal_MemCpy(void* dest, const void* src, unsigned int size) {}

tBleStatus GAP_connection_procedure(uint8_t procedure_code, uint8_t phys, uint8_t peer_address_type, uint8_t peer_address[6]) { return 0; }
tBleStatus GAP_terminate_gap_procedure(uint8_t procedure_code) { return 0; }
void GAP_name_disc_proc_connected_check(uint16_t task_idx) {}
void GAP_LimDiscTimeoutcb(uint8_t timer_id) {}
void Controller_Process_Q(uint16_t task_idx) {}

void GAT_att_cmn_init(void) {}
void GAT_att_cmn_timer_expire_tsk(uint16_t task_idx) {}
void GAT_att_cmn_tx_pool_evt_tsk(uint16_t task_idx) {}
void GAT_att_srv_process_rx_pckt_tsk(uint16_t task_idx) {}
void GAT_srv_db_hash_tsk(uint16_t task_idx) {}
void GAT_srv_send_srv_change_tsk(uint16_t task_idx) {}

void LL_cpe_init_phy_update(void) {}
void LL_cpe_init_conn_update(void) {}
void LL_cpe_init_chmap_update(void) {}
void LL_cpe_init(void) {}
void LL_conn_upd_max_tx_time_coded(void* params) {}
void LL_conn_upd_data_length_change_event(void* params) {}
uint8_t LL_phy_upd_pending(uint8_t conn_idx) { return 0; }
tBleStatus LL_phy_update_init(void) { return 0; }
tBleStatus LL_phy_update_init_per_st_data(uint8_t conn_idx) { return 0; }
void LL_phy_upd_evt_pending_check_isr(uint8_t conn_idx) {}
void LL_eadv_EauxIsr_connect_response_sent(void* pointer) {}
void LL_eadv_EauxIsr_connect_request_received(void* pointer,
	uint8_t* calibration_required) {
}
void LL_init(uint8_t dataLenExt,
	uint8_t PhyUpd,
	uint8_t ExtAdvScan,
	uint8_t CtrlPriv,
	uint8_t ScanSupp,
	uint8_t PerAdvScan,
	uint8_t PerAdvScanWr,
	uint8_t Cte,
	uint8_t Pcl,
	uint8_t Cns,
	uint8_t Chc) {
}

tBleStatus L2C_init(uint8_t cos_enabled) { return 0; }
tBleStatus llc_conn_multi_link_connection(uint8_t enable) { return 0; }
void llc_conn_peripheral_roll_back_params_tsk(uint16_t task_idx) {}
void LLC_authenticated_payload_timeout_processing(uint16_t task_idx) {}
uint8_t LLC_pcl_get_number_of_phys(void) { return 0; }
void LLC_offline_control_procedures_processing(uint16_t task_idx) {}

uint32_t phy_upd_csr(void) { return 0; }
uint32_t conn_supp_csr(void) { return 0; }
tBleStatus ACL_pkt_init(void) { return 0; }
tBleStatus MBM_init(void) { return 0; }
uint32_t secure_connections_csr(void) { return 0; }
void ADV_ISR_connect_request_received(void* pointer,
	uint8_t* packet,
	void* PeerIDAddress_p) {
}
uint8_t EADV_start_request_radio_tasks(void* pointer) { return 0; }

tBleStatus aci_hal_get_link_status(uint8_t Bank_index, uint8_t Link_Status[8], uint16_t Link_Connection_Handle[16 / 2]) { return 0; }
tBleStatus aci_hal_read_config_data(uint8_t Offset, uint8_t* Data_Length, uint8_t Data[]) { return 0; }
tBleStatus aci_hal_set_tx_power_level(uint8_t En_High_Power, uint8_t PA_Level) { return 0; }
tBleStatus aci_gap_init(uint8_t Privacy_Type, uint8_t Identity_Address_Type) { return 0; }
tBleStatus aci_gap_set_advertising_configuration(uint8_t Advertising_Handle,
	uint8_t Discoverable_Mode,
	uint16_t Advertising_Event_Properties,
	uint32_t Primary_Advertising_Interval_Min,
	uint32_t Primary_Advertising_Interval_Max,
	uint8_t Primary_Advertising_Channel_Map,
	uint8_t Peer_Address_Type,
	uint8_t Peer_Address[6],
	uint8_t Advertising_Filter_Policy,
	int8_t Advertising_Tx_Power,
	uint8_t Primary_Advertising_PHY,
	uint8_t Secondary_Advertising_Max_Skip,
	uint8_t Secondary_Advertising_PHY,
	uint8_t Advertising_SID,
	uint8_t Scan_Request_Notification_Enable) {
	return 0;
}

tBleStatus aci_gap_set_advertising_enable(uint8_t Enable, uint8_t Number_of_Sets, Advertising_Set_Parameters_t Advertising_Set_Parameters[]) { return 0; }
tBleStatus aci_gap_configure_filter_accept_and_resolving_list(uint8_t Lists) { return 0; }
tBleStatus aci_gap_set_advertising_data(uint8_t Advertising_Handle, uint8_t Operation, uint16_t Advertising_Data_Length, uint8_t Advertising_Data[]) { return 0; }
tBleStatus aci_gap_terminate_api(uint16_t Connection_Handle, uint8_t Reason) { return 0; }
tBleStatus aci_gap_set_io_capability_api(uint8_t IO_Capability) { return 0; }
tBleStatus aci_gap_set_security_requirements_api(uint8_t Bonding_Mode,
	uint8_t MITM_Mode,
	uint8_t SC_Support,
	uint8_t KeyPress_Notification_Support,
	uint8_t Min_Encryption_Key_Size,
	uint8_t Max_Encryption_Key_Size,
	uint8_t Pairing_Response) {
	return 0;
}
tBleStatus aci_gap_set_security_api(uint16_t Connection_Handle, uint8_t Security_Level, uint8_t Force_Pairing) { return 0; }
tBleStatus aci_gap_pairing_resp_api(uint16_t Connection_Handle, uint8_t Accept) { return 0; }
tBleStatus aci_gap_passkey_resp_api(uint16_t Connection_Handle, uint32_t Passkey) { return 0; }
tBleStatus aci_gap_passkey_input_api(uint16_t Connection_Handle, uint8_t Input_Type) { return 0; }
tBleStatus aci_gap_get_oob_data_api(uint8_t OOB_Data_Type,
	uint8_t* Address_Type,
	uint8_t Address[6],
	uint8_t* OOB_Data_Len,
	uint8_t OOB_Data[16]) {
	return 0;
}
tBleStatus aci_gap_set_oob_data_api(uint8_t Device_Type,
	uint8_t Address_Type,
	uint8_t Address[6],
	uint8_t OOB_Data_Type,
	uint8_t OOB_Data_Len,
	uint8_t OOB_Data[16]) {
	return 0;
}
tBleStatus aci_gap_numeric_comparison_value_confirm_yesno_api(uint16_t Connection_Handle, uint8_t Confirm_Yes_No) { return 0; }
tBleStatus aci_gap_get_security_level_api(uint16_t Connection_Handle, uint8_t* Security_Mode, uint8_t* Security_Level) { return 0; }
tBleStatus aci_gap_clear_security_db_api(void) { return 0; }
tBleStatus aci_gap_get_bonded_devices_api(uint8_t Offset, uint8_t Max_Num_Of_Addresses, uint8_t* Num_of_Addresses, Bonded_Device_Entry_t* Bonded_Device_Entry) { return 0; }
tBleStatus aci_gap_is_device_bonded_api(uint8_t Peer_Address_Type, uint8_t Peer_Address[6]) { return 0; }
tBleStatus aci_gap_remove_bonded_device_api(uint8_t peerIdentityAddressType, uint8_t peerIdentityDeviceAddress[6]) { return 0; }
tBleStatus aci_gatt_srv_add_service_api(ble_gatt_srv_def_t* Serv_p) { return 0; }
tBleStatus aci_gatt_srv_rm_service_api(uint16_t Serv_Attr_H) { return 0; }
uint16_t aci_gatt_srv_get_service_handle_api(ble_gatt_srv_def_t* Serv_p) { return 0; }
tBleStatus aci_gatt_srv_include_service_api(uint16_t Serv_Attr_H, uint16_t Incl_Serv_Attr_H) { return 0; }
tBleStatus aci_gatt_srv_rm_include_service_api(uint16_t Incl_Serv_Attr_H) { return 0; }
uint16_t aci_gatt_srv_get_include_service_handle_api(uint16_t Serv_Attr_H, ble_gatt_srv_def_t* Included_Srv_p) { return 0; }
tBleStatus aci_gatt_srv_add_char_api(ble_gatt_chr_def_t* Char_p, uint16_t Serv_Attr_H) { return 0; }
tBleStatus aci_gatt_srv_rm_char_api(uint16_t Char_Decl_Attr_H) { return 0; }
uint16_t aci_gatt_srv_get_char_decl_handle_api(ble_gatt_chr_def_t* Char_p) { return 0; }
tBleStatus aci_gatt_srv_add_char_desc_api(ble_gatt_descr_def_t* Descr_p, uint16_t Char_Attr_H) { return 0; }
uint16_t aci_gatt_srv_get_descriptor_handle_api(ble_gatt_descr_def_t* Descr_p) { return 0; }
tBleStatus aci_gatt_srv_notify_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle,
	uint8_t Flags,
	uint16_t Val_Length,
	uint8_t* Val_p) {
	return 0;
}
tBleStatus aci_gatt_srv_multi_notify_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint8_t Flags,
	uint8_t Num_Of_Attr,
	Gatt_Srv_Notify_Attr_t* Gatt_Srv_Notify_Attr) {
	return 0;
}
tBleStatus aci_gatt_srv_resp_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle,
	uint8_t Error_Code,
	uint16_t Data_Len,
	uint8_t* Data_p) {
	return 0;
}
tBleStatus aci_gatt_srv_read_handle_value_api(uint16_t Attr_Handle,
	uint16_t* Val_Length_p,
	uint8_t** Val_pp) {
	return 0;
}
tBleStatus aci_gatt_srv_read_multiple_instance_handle_value_api(uint16_t Connection_Handle,
	uint16_t Attr_Handle,
	uint16_t* Val_Length_p,
	uint8_t** Val_pp) {
	return 0;
}
tBleStatus aci_gatt_srv_write_multiple_instance_handle_value_api(uint16_t Connection_Handle,
	uint16_t Attr_Handle,
	uint16_t Char_Value_Length,
	uint8_t* Char_Value) {
	return 0;
}
tBleStatus aci_gatt_set_event_mask_api(uint32_t GATT_Evt_Mask) { return 0; }
tBleStatus aci_gatt_clt_exchange_config_api(uint16_t Connection_Handle) { return 0; }
tBleStatus aci_gatt_clt_disc_all_primary_services_api(uint16_t Connection_Handle,
	uint16_t CID) {
	return 0;
}
tBleStatus aci_gatt_clt_disc_primary_service_by_uuid_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint8_t UUID_Type,
	UUID_t* UUID) {
	return 0;
}
tBleStatus aci_gatt_clt_disc_all_char_of_service_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Start_Handle,
	uint16_t End_Handle) {
	return 0;
}
tBleStatus aci_gatt_clt_disc_char_by_uuid_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Start_Handle,
	uint16_t End_Handle,
	uint8_t UUID_Type,
	UUID_t* UUID) {
	return 0;
}
tBleStatus aci_gatt_clt_disc_all_char_desc_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Char_Handle,
	uint16_t End_Handle) {
	return 0;
}
tBleStatus aci_gatt_clt_find_included_services_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Start_Handle,
	uint16_t End_Handle) {
	return 0;
}
tBleStatus aci_gatt_clt_read_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle) {
	return 0;
}
tBleStatus aci_gatt_clt_read_long_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle,
	uint16_t Val_Offset) {
	return 0;
}
tBleStatus aci_gatt_clt_read_using_char_uuid_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Start_Handle,
	uint16_t End_Handle,
	uint8_t UUID_Type,
	UUID_t* UUID) {
	return 0;
}
tBleStatus aci_gatt_clt_read_multiple_char_value_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint8_t Number_of_Handles,
	uint16_t* Handle_Entry) {
	return 0;
}
tBleStatus aci_gatt_clt_read_multiple_var_len_char_value_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint8_t Number_of_Handles,
	uint16_t* Handle_Entry) {
	return 0;
}
tBleStatus aci_gatt_clt_write_without_resp_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle,
	uint16_t Attribute_Val_Length,
	uint8_t* Attribute_Val) {
	return 0;
}
tBleStatus aci_gatt_clt_signed_write_without_resp_api(uint16_t Connection_Handle,
	uint16_t Attr_Handle,
	uint16_t Attribute_Val_Length,
	uint8_t* Attribute_Val) {
	return 0;
}
tBleStatus aci_gatt_clt_write_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle,
	uint16_t Attribute_Val_Length,
	uint8_t* Attribute_Val) {
	return 0;
}
tBleStatus aci_gatt_clt_write_long_api(uint16_t Connection_Handle,
	uint16_t CID,
	ble_gatt_clt_write_ops_t* Write_Ops_p) {
	return 0;
}
tBleStatus aci_gatt_clt_write_char_reliable_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint8_t Num_Attrs,
	ble_gatt_clt_write_ops_t* Write_Ops_p) {
	return 0;
}
tBleStatus aci_gatt_clt_prepare_write_req_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint16_t Attr_Handle,
	uint16_t Val_Offset,
	uint16_t Attribute_Val_Length,
	uint8_t* Attribute_Val) {
	return 0;
}
tBleStatus aci_gatt_clt_execute_write_req_api(uint16_t Connection_Handle,
	uint16_t CID,
	uint8_t Execute) {
	return 0;
}
tBleStatus aci_gatt_clt_confirm_indication_api(uint16_t Connection_Handle,
	uint16_t CID) {
	return 0;
}
tBleStatus aci_gatt_clt_add_subscription_security_level_api(ble_gatt_clt_sec_level_st* sec_level_p) { return 0; }
tBleStatus aci_hal_peripheral_latency_enable_api(uint16_t Connection_Handle,
	uint8_t Enable) {
	return 0;
}
tBleStatus aci_hal_get_anchor_point_api(uint16_t connection_handle,
	uint16_t* event_counter,
	uint32_t* anchor_point) {
	return 0;
}
tBleStatus aci_l2cap_connection_parameter_update_req_api(uint16_t Connection_Handle,
	uint16_t Connection_Interval_Min,
	uint16_t Connection_Interval_Max,
	uint16_t Peripheral_Latency,
	uint16_t Timeout_Multiplier) {
	return 0;
}

tBleStatus hci_acl_data_tx_cmpl_event_int_cb(void* header_p,
	uint8_t* buff_p) {
	return 0;
}
tBleStatus hci_acl_data_ind_event_int_cb(void* header_p,
	uint8_t* buff_p) {
	return 0;
}
tBleStatus hci_write_authenticated_payload_timeout_api(uint16_t Connection_Handle,
	uint16_t Authenticated_Payload_Timeout) {
	return 0;
}
tBleStatus hci_le_transmitter_test_v2_api(uint8_t TX_Frequency,
	uint8_t Length_Of_Test_Data,
	uint8_t Packet_Payload,
	uint8_t Phy) {
	return 0;
}
tBleStatus hci_le_rand(uint8_t Random_Number[8]) { return 0; }
tBleStatus hci_disconnection_complete_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_encryption_change_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_encryption_key_refresh_complete_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_le_connection_complete_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_le_enhanced_connection_complete_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_le_long_term_key_request_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_le_read_local_p256_public_key_complete_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_le_generate_dhkey_complete_event_int_cb(void* header_p, uint8_t* buff_p) { return 0; }
tBleStatus hci_le_set_host_feature_api(uint8_t Bit_Number, uint8_t Bit_Value) { return 0; }
tBleStatus hci_read_remote_version_information_api(uint16_t Connection_Handle) { return 0; }
tBleStatus hci_le_read_remote_features_api(uint16_t Connection_Handle) { return 0; }
tBleStatus hci_read_transmit_power_level_api(uint16_t Connection_Handle, uint8_t Type, int8_t* Transmit_Power_Level) { return 0; }
tBleStatus hci_read_rssi_api(uint16_t Connection_Handle, int8_t* RSSI) { return 0; }
tBleStatus hci_le_read_channel_map_api(uint16_t Connection_Handle, uint8_t LE_Channel_Map[LLC_MIN_NUM_DATA_CHAN_MAP_BYTES]) { return 0; }
tBleStatus hci_disconnect_api(uint16_t Connection_Handle, uint8_t Reason) { return 0; }
tBleStatus hci_le_create_connection_cancel_api(void) { return 0; }
tBleStatus hci_le_long_term_key_request_reply_api(uint16_t Connection_Handle, uint8_t Long_Term_Key[16]) { return 0; }
tBleStatus hci_le_long_term_key_request_negative_reply_api(uint16_t Connection_Handle) { return 0; }
tBleStatus hci_le_read_local_p256_public_key_api(void) { return 0; }
tBleStatus hci_le_generate_dhkey_api(uint8_t Remote_P256_Public_Key[64]) { return 0; }
tBleStatus hci_read_authenticated_payload_timeout_api(uint16_t Connection_Handle, uint16_t* Authenticated_Payload_Timeout) { return 0; }
tBleStatus hci_le_read_phy_api(uint16_t Connection_Handle, uint8_t* TX_PHY, uint8_t* RX_PHY) { return 0; }
tBleStatus hci_le_set_default_phy_api(uint8_t ALL_PHYS, uint8_t TX_PHYS, uint8_t RX_PHYS) { return 0; }
tBleStatus hci_le_set_phy_api(uint16_t Connection_Handle, uint8_t ALL_PHYS, uint8_t TX_PHYS, uint8_t RX_PHYS, uint16_t PHY_options) { return 0; }
tBleStatus hci_le_receiver_test_v2_api(uint8_t RX_Frequency, uint8_t Phy, uint8_t Modulation_index) { return 0; }

tBleStatus smp_pp1_cei_rxp_pairing_response_excerpt_scp(void* params) { return 0; }
void smp_pp2_cei_auth_stage1_start_excerpt_scp(void* params) {}
void smp_pp2_per_auth_stage1_start_excerpt_scp(void* params) {}
void smp_pp2_cei_send_pairing_confirm_excerpt_scp(void* params) {}
tBleStatus smp_pp2_cei_rx_process_pairing_random_excerpt1_scp(void* params) { return 0; }
void smp_pp2_cei_rx_process_pairing_random_excerpt2_scp(void* params) {}
void smp_pp2_per_rx_process_pairing_confirm_excerpt_scp(void* params) {}
void smp_pp2_per_rx_process_pairing_random_excerpt_scp(void* params) {}
void smp_scp_init(uint8_t use_debug_key) {}
tBleStatus smp_scp_continue_on_exclusive_sc_pairing(void* params) { return 0; }
tBleStatus smp_scp_public_key_prepare_and_exchange(void* params) { return 0; }
tBleStatus smp_scp_oob_generate_new_local_data(void) { return 0; }
void smp_scp_stats_update_on_pairing_complete(void* params) {}
void smp_fsm_process_actions_wrt_state_tsk(uint16_t task_idx) {}
void smp_fsm_execute_actions_scp_phase2as2(void* params) {}
void smp_pka_process_ecc_computation_tsk(uint16_t task_idx) {}
void smp_rxp_process_rxed_packets_tsk(uint16_t task_idx) {}
tBleStatus smp_rxp_process_exception_cases_excerpt_scp(void* params) { return 0; }
tBleStatus smp_rxp_process_wrt_current_fsm_excerpt_scp(void* params) { return 0; }
void smp_init(void) {}
tBleStatus smp_debug_trudy__set_config(uint32_t config) { return 0; }
tBleStatus smp_sap_scp_force_debug_key_usage(uint8_t config_val) { return 0; }
void smp_sap_hci_le_read_local_p256_public_key_complete_evt_hndl(uint8_t status,
	uint8_t local_p256_public_key[64]) {
}
void smp_sap_hci_le_generate_dhkey_complete_evt_hndl(uint8_t status,
	uint8_t dhkey[32]) {
}