/* tms.h
 * libtms
 *
 * (c) 2016 Pagar.me. All rights reserved.
 */

#ifndef __LIBTMS_H
#define __LIBTMS_H

#include "abecs.h"
#include "packet.h"
#include "mpos.h"

// MSVC crazyness
#ifdef _MSC_VER
#define TMS_EXPORT _declspec(dllexport)
#else
#define TMS_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int tms_error_t;

typedef tms_error_t (*tms_store_write_cb)(
	const char *version,
	abecs_table_load_record_cmd_t **tables,
	unsigned int table_length,
	mpos_application_data **applications,
	unsigned int app_length,
	mpos_risk_management_data **risk_management_data,
	unsigned int risk_management_length,
	mpos_acquirer_data **acquirers,
	unsigned int acq_length,
	void *user_data);

tms_error_t TMS_EXPORT tms_get_tables(unsigned char *payload, unsigned int length, tms_store_write_cb write_cb, void *user_data);

#ifdef __cplusplus
}
#endif

#endif
