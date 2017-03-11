/* packet.h
 * libabecs
 *
 * (c) 2015 Pagar.me
 * Created by Daniel Ferreira
 */

#include <string.h>
#include <stdbool.h>
#include "abecs.h"

#ifndef __LIBABECS_PACKET_H
#define __LIBABECS_PACKET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _abecs_cmd_t {
	ABECS_CMD_CAN = -1,

	ABECS_CMD_OPN = 0,
	ABECS_CMD_CLO,
	ABECS_CMD_DSP,
	ABECS_CMD_CKE,
	ABECS_CMD_GCR,
	ABECS_CMD_GOC,
	ABECS_CMD_FNC,
	ABECS_CMD_GTS,
	ABECS_CMD_TLI,
	ABECS_CMD_TLR,
	ABECS_CMD_TLE,
	ABECS_CMD_GDU,
	ABECS_CMD_GPN,
	ABECS_CMD_NTM,
	ABECS_CMD_ERR
} abecs_cmd_t;

typedef void (*abecs_packet_write_fn)(abecs_t *abecs, void *packet);
typedef void *(*abecs_data_parse_fn)(abecs_t *abecs, unsigned char *buffer, size_t length);
typedef struct {
	abecs_data_parse_fn parse;
	int cmd;
} abecs_packet_callback_data;

#define DATA_PROCESS(name) \
	void abecs_process_ ## name ## _response(abecs_t *abecs, void *packet)
#define DATA_REGISTER(cmd, name) \
	abecs_register_callback( ABECS_CMD_ ## cmd , abecs_process_ ## name ## _response );
#define DATA_UNREGISTER(cmd) \
	abecs_unregister_callback( ABECS_CMD_ ## cmd );

#define DATA_COMMAND(name) \
	typedef struct abecs_ ## name abecs_ ## name ## _cmd_t; \
	void abecs_write_ ## name (abecs_t *abecs, void *packet); \
	struct abecs_ ## name {

#define DATA_RESPONSE(name) char __abecs_noempty; }; \
	typedef struct abecs_ ## name ## _response abecs_ ## name ## _response_t; \
	void *abecs_parse_ ## name ## _response(abecs_t *abecs, unsigned char *buffer, size_t length); \
	struct abecs_ ## name ## _response { \
		abecs_stat_t stat;

#define DATA_END };

#define DATA_BEGIN_ASSOC \
	static abecs_packet_callback_data *abecs_get_packet_callback_data(unsigned char *cmd) { \
		abecs_packet_callback_data *r = (abecs_packet_callback_data *)malloc(sizeof(abecs_packet_callback_data));

#define DATA_CMD_ASSOC(command, name) \
	if (memcmp( #command , cmd, 3) == 0) { \
		r->parse = abecs_parse_ ## name ## _response; \
		r->cmd = ABECS_CMD_ ## command ; \
		return r; \
	}

#define DATA_END_ASSOC free(r); return NULL; }

DATA_COMMAND(open)
	unsigned char rsa[512];
	unsigned int exp;
DATA_RESPONSE(open)
	unsigned int crk_length;
	unsigned char crksec[512];
DATA_END

DATA_COMMAND(close)
	unsigned char message[32];
DATA_RESPONSE(close)
DATA_END

DATA_COMMAND(display)
	unsigned char message[32];
DATA_RESPONSE(display)
DATA_END

DATA_COMMAND(check_event)
	bool check_key;
	bool check_magnetic_card;
	bool check_icc;
	bool check_ctls;
DATA_RESPONSE(check_event)
	unsigned int event;
	union {
		unsigned int key;
		struct {
			unsigned int track1_length;
			unsigned char *track1;
			unsigned int track2_length;
			unsigned char *track2;
			unsigned int track3_length;
			unsigned char *track3;
		} track;
		bool has_icc;
		bool has_ctls;
	} data;
DATA_END

DATA_COMMAND(get_card)
	bool after_table_reload;
	unsigned int table_acq;
	unsigned int app_type; //1=credit; 2=debit
	unsigned int payment_amount;
	unsigned char emv_table_version[10];
	unsigned int emv_app_number;
	struct {
		unsigned int emv_acq;
		unsigned int emv_recidx;
	} application_list[100];

	bool enable_ctls;
DATA_RESPONSE(get_card)
	unsigned int card_type;
	unsigned int chip_stat;
	unsigned int app_type;
	unsigned int emv_acq;
	unsigned int emv_recidx;
	
	struct {
		unsigned int track1_length;
		unsigned char track1[76];
		unsigned int track2_length;
		unsigned char track2[40];
		unsigned int track3_length;
		unsigned char track3[104];
	} track;
	
	unsigned int pan_length;
	unsigned char pan[19];
	
	unsigned int app_pan_sequence_number;
	unsigned char app_label[16];
	unsigned int srvcode;

	unsigned char cardholder[26];
	unsigned char card_expiration_date[6];
	unsigned int card_issuer_country_code;

	unsigned int emv_acq_data_length;
	unsigned char emv_acq_data[66];
DATA_END

DATA_COMMAND(go_on_card)
	unsigned int payment_amount;
	unsigned int payment_cashback;
	bool is_blacklisted;
	bool must_online;
	
	unsigned int cryptography_method;
	unsigned int keyidx;
	unsigned char working_key[32];

	bool must_risk_management;
	unsigned int floor_limit;
	unsigned int brs_percentage;
	unsigned int brs_threshold;
	unsigned int max_brs_percentage;

	unsigned int emv_acq;
	
	unsigned int tags_number;
	unsigned short tags[256];
DATA_RESPONSE(go_on_card)
	unsigned int decision;
	bool must_signature;
	bool pin_verified_offline;
	unsigned int pin_error_number;
	bool offline_pin_blocked;
	bool online_pin_captured;
	
	unsigned char encrypted_pin[16];
	unsigned char encrypted_key_serial_number[20];
	
	unsigned int emv_data_length;
	unsigned char emv_data[512];
DATA_END

DATA_COMMAND(finish_card)
	unsigned int comm_status;
	unsigned int emv_arc;
	unsigned int emv_transaction_data_length;
	unsigned char emv_transaction_data[512];

	unsigned int tags_number;
	unsigned short tags[256];
DATA_RESPONSE(finish_card)
	unsigned int decision;
	
	unsigned int emv_data_length;
	unsigned char emv_data[512];

	unsigned int issuer_script_results_length;
	unsigned char issuer_script_results[100];
DATA_END

DATA_COMMAND(get_table_version)
	unsigned int table_acq;
DATA_RESPONSE(get_table_version)
	unsigned char emv_table_version[10];
DATA_END

DATA_COMMAND(table_load_init)
	unsigned int table_acq;
	unsigned char emv_table_version[10];
DATA_RESPONSE(table_load_init)
DATA_END

DATA_COMMAND(table_load_record)
	bool is_aid;
	unsigned int emv_acq;
	unsigned int emv_recidx;
	
	union {
		struct {
			unsigned int emv_aid_length;
			unsigned char emv_aid[32];
			unsigned int app_type;
			unsigned int app_name_length;
			unsigned char app_name[16];
			unsigned char app_version_1[4];
			unsigned char app_version_2[4];
			unsigned char app_version_3[4];
			unsigned int country_code;
			unsigned int currency_code;
			unsigned int currency_exponent;
			unsigned char merchant_id[15];
			unsigned int mcc;
			unsigned char terminal_id[8];
			unsigned char terminal_capabilities[6];
			unsigned char additional_terminal_capabilities[10];
			unsigned int terminal_type;
			unsigned char tac_default[10];
			unsigned char tac_denial[10];
			unsigned char tac_online[10];
			unsigned int floor_limit;
			unsigned char tcc;

			bool ctls_zeroam;
			unsigned int ctls_mode;
			unsigned int ctls_trnlim;
			unsigned int ctls_flrlim;
			unsigned int ctls_cvmlim;
			unsigned char ctls_appver[4];

			unsigned int tdol_length;
			unsigned char tdol[40];
			unsigned int ddol_length;
			unsigned char ddol[40];
		} aid_data;
		struct {
			unsigned char rid[10];
			unsigned int capk_idx;
			unsigned int exp_length;
			unsigned char exp[6];
			unsigned int mod_length;
			unsigned char mod[496];
			bool checksum_stat;
			unsigned char checksum[40];
		} capk_data;
	} table_data;
DATA_RESPONSE(table_load_record)
DATA_END

DATA_COMMAND(table_load_end)
DATA_RESPONSE(table_load_end)
DATA_END

DATA_COMMAND(get_pin)
	unsigned int cryptography_method;
	unsigned int keyidx;
	unsigned char working_key[32];

	unsigned int pan_length;
	unsigned char pan[19];

	unsigned int entries_length;
	struct {
		unsigned int min_length;
		unsigned int max_length;
		unsigned char message[32];
	} entries[9];
DATA_RESPONSE(get_pin)
	unsigned char encrypted_pin[16];
	unsigned char encrypted_key_serial_number[20];	
DATA_END

DATA_COMMAND(get_dukpt)
	unsigned int cryptography_method;
	unsigned int keyidx;
DATA_RESPONSE(get_dukpt)
	unsigned char encrypted_key_serial_number[20];
DATA_END

DATA_COMMAND(notify)
DATA_RESPONSE(notify)
	size_t message_length;
	unsigned char message[32];
DATA_END

DATA_COMMAND(error)
DATA_RESPONSE(error)
DATA_END

DATA_BEGIN_ASSOC
DATA_CMD_ASSOC(OPN, open)
DATA_CMD_ASSOC(CLO, close)

DATA_CMD_ASSOC(DSP, display)
DATA_CMD_ASSOC(CKE, check_event)

DATA_CMD_ASSOC(GCR, get_card)
DATA_CMD_ASSOC(GOC, go_on_card)
DATA_CMD_ASSOC(FNC, finish_card)

DATA_CMD_ASSOC(GTS, get_table_version)
DATA_CMD_ASSOC(TLI, table_load_init)
DATA_CMD_ASSOC(TLR, table_load_record)
DATA_CMD_ASSOC(TLE, table_load_end)

DATA_CMD_ASSOC(GPN, get_pin)
DATA_CMD_ASSOC(GDU, get_dukpt)

DATA_CMD_ASSOC(NTM, notify)
DATA_CMD_ASSOC(ERR, error)
DATA_END_ASSOC

#ifdef __cplusplus
}
#endif

#endif
