/* mpos.h
 * libmpos
 *
 * (c) 2015 Pagar.me
 * Created by Daniel Ferreira
 */

#ifndef __LIBMPOS_H
#define __LIBMPOS_H

#include "abecs.h"
#include "packet.h"

// MSVC crazyness
#ifdef _MSC_VER
#define MPOS_EXPORT _declspec(dllexport)
#else
#define MPOS_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	MERR_OK,
	MERR_ABECS,
	MERR_BAD_PARAMS
} mpos_error_t;

typedef enum {
	MFT_OK = 0,
	MFT_ERROR = 1,
	MFT_ARC_NON_ZERO = 9
} mpos_finish_transaction_status_t;

typedef enum mpos_decision {
	MD_APPROVED = 0,
	MD_REFUSED,
	MD_GO_ONLINE
} mpos_decision_t;

typedef enum {
	MCM_MAGSTRIPE = 0,
	MCM_EMV = 3
} mpos_capture_method_t;

typedef enum {
	MPM_CREDIT = 1,
	MPM_DEBIT = 2
} mpos_payment_method_t;

typedef struct {
	mpos_capture_method_t capture_method;

	mpos_decision_t decision;

	int amount;
	int acquirer_number;
	int record_index;
	int application_type;
	int pan_sequence_number;
	
	unsigned char expiration_date[6];
	
	unsigned char holder_name[26];
	size_t holder_name_length;

	unsigned char pan[19];
	size_t pan_length;

	unsigned char track1[76];
	size_t track1_len;

	unsigned char track2[40];
	size_t track2_len;

	unsigned char track3[104];
	size_t track3_len;

	unsigned char emv_data[512];
	size_t emv_data_length;
	
	int pin_required;
	int is_online_pin;

	unsigned char pin[16];
	unsigned char pin_kek[20];
} mpos_payment_info;

typedef struct {
	unsigned int acquirer_number;
	
	unsigned int cryptography_method;
	unsigned int key_index;
	unsigned char session_key[32];
} mpos_acquirer_data;

typedef struct {
	unsigned int acquirer_number;
	unsigned int record_index;
	
	bool must_risk_management;
	unsigned int floor_limit;
	unsigned int brs_percentage;
	unsigned int brs_threshold;
	unsigned int max_brs_percentage;
} mpos_risk_management_data;

typedef struct {
	mpos_payment_method_t payment_method;
	char card_brand[100];

	unsigned int acquirer_number;
	unsigned int record_index;
	
	unsigned int emv_tags_length;
	unsigned int emv_tags[256];
} mpos_application_data;

// Mpos
typedef struct mpos mpos_t;

typedef void (*mpos_notification_callback)(mpos_t *mpos, const char *notification);
typedef void (*mpos_operation_completed_callback)(mpos_t *mpos);
typedef void (*mpos_operation_cancelled_callback)(mpos_t *mpos);

typedef mpos_error_t (*mpos_initialized_callback)(mpos_t *mpos, abecs_stat_t error);
typedef mpos_error_t (*mpos_closed_callback)(mpos_t *mpos, abecs_stat_t error);
typedef mpos_error_t (*mpos_tables_loaded_callback)(mpos_t *mpos, abecs_stat_t error, bool loaded);
typedef mpos_error_t (*mpos_get_table_version_callback)(mpos_t *mpos, abecs_stat_t error, unsigned char *version);
typedef mpos_error_t (*mpos_payment_callback)(mpos_t *mpos, abecs_stat_t error, mpos_payment_info *info);
typedef mpos_error_t (*mpos_finish_transaction_callback)(mpos_t *mpos, abecs_stat_t error);
typedef mpos_error_t (*mpos_extract_keys_callback)(mpos_t *mpos, abecs_stat_t error, unsigned int *keys, unsigned int keys_length);

mpos_t MPOS_EXPORT *mpos_new(abecs_stream_t *stream,
	mpos_notification_callback notification_callback,
	mpos_operation_completed_callback op_completed_callback,
	mpos_operation_cancelled_callback op_cancelled_callback);

mpos_error_t MPOS_EXPORT mpos_initialize(mpos_t *mpos,
	void *stream_data,
	mpos_initialized_callback callback);

mpos_error_t MPOS_EXPORT mpos_close(
	mpos_t *mpos,
	const char *string,
	mpos_closed_callback callback);

mpos_error_t MPOS_EXPORT mpos_process_payment(
	mpos_t *mpos,
	int amount,
	mpos_application_data *emv_application_list,
	unsigned int emv_application_list_length,
	mpos_acquirer_data *acquirer_list,
	unsigned int acquirer_list_length,
	mpos_risk_management_data *risk_management_list,
	unsigned int risk_management_list_length,
	mpos_payment_method_t magstripe_payment_method,
	mpos_payment_callback callback);

mpos_error_t MPOS_EXPORT mpos_finish_transaction(
	mpos_t *mpos,
	mpos_finish_transaction_status_t status,
	int arc,
	unsigned char *emv_data,
	unsigned int emv_length,
	mpos_finish_transaction_callback callback);

mpos_error_t MPOS_EXPORT mpos_display(
	mpos_t *mpos,
	const char *string);

mpos_error_t MPOS_EXPORT mpos_get_table_version(
	mpos_t *mpos,
	mpos_get_table_version_callback callback);

mpos_error_t MPOS_EXPORT mpos_update_tables(
	mpos_t *mpos,
	abecs_table_load_record_cmd_t **tables,
	size_t count,
	unsigned char *version,
	bool force_update,
	mpos_tables_loaded_callback callback);

mpos_error_t MPOS_EXPORT mpos_extract_keys(
	mpos_t *mpos,
	mpos_extract_keys_callback callback);

mpos_error_t MPOS_EXPORT mpos_cancel(mpos_t *mpos);

abecs_t MPOS_EXPORT *mpos_get_abecs(mpos_t *mpos);
void MPOS_EXPORT *mpos_get_user_info(mpos_t *mpos);
void MPOS_EXPORT mpos_set_user_info(mpos_t *mpos, void *user_info);

mpos_error_t MPOS_EXPORT mpos_free(mpos_t *mpos);

#ifdef __cplusplus
}
#endif

#endif
