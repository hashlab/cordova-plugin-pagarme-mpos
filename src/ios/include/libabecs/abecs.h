/* abecs.h
 * libabecs
 *
 * (c) 2015 Pagar.me
 */

#ifndef __LIBABECS_H
#define __LIBABECS_H

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	ST_OK = 0,

	ST_NOSEC = 3,

	ST_F1 = 4,
	ST_F2 = 5,
	ST_F3 = 6,
	ST_F4 = 7,
	ST_BACKSP = 8,

	ST_ERRPKTSEC = 9,
	ST_INVCALL = 10,
	ST_INVPARM = 11,
	ST_TIMEOUT = 12,
	ST_CANCEL = 13,
	ST_NOTOPEN = 15,
	ST_MANDAT = 19,

	ST_TABVERDIF = 20,
	ST_TABERR = 21,

	ST_INTERR = 40,
	ST_MCDATAERR = 41,
	ST_ERRKEY = 42,
	ST_NOCARD = 43,
	ST_PINBUSY = 44,
	ST_RSPOVRFL = 45,
	ST_NOSAM = 51,

	ST_DUMBCARD = 60,
	ST_ERRCARD = 61,
	ST_CARDINVALIDAT = 67,
	ST_CARDPROBLEMS = 68,
	ST_CARDINVDATA = 69,
	ST_CARDAPPNAV = 70,
	ST_CARDAPPNAUT = 71,
	
	ST_ERRFALLBACK = 76,

	ST_CTLSSMULTIPLE = 80,
	ST_CTLSSCOMMERR = 81,
	ST_CTLSSINVALIDAT = 82,
	ST_CTLSSPROBLEMS = 83,
	ST_CTLSSAPPNAV = 84,
	ST_CTLSAPPNAUT = 85,
	
	ST_MFNFOUND = 100,
	ST_MFERRFMT = 101,
	ST_MFERR = 102
} abecs_stat_t;

typedef enum {
	ABECS_NO_PARSER = -2,
	ABECS_BUFFER_OVERFLOW = -1,
	
	ABECS_OK = 0,
	ABECS_OK_NTM = 1,
	ABECS_OK_ERR = 2,
	ABECS_OK_COMPLETED = 3,

	ABECS_COMM_ERROR = 10,
	ABECS_BAD_SENT_CRC = 11,
	ABECS_BAD_RECV_CRC = 12,
	ABECS_BAD_RECV_LEN = 13,
	ABECS_TIMEOUT = 14,
	ABECS_NO_CMD = 15,

	ABECS_FILE_ERROR = 20,
	ABECS_INTERNAL_ERROR = 21
} abecs_error_t;

#define kPAX "/dev/tty.PAX-50346373-BluetoothS"

#define kEOTByte 0x04
#define kACKByte 0x06
#define kDC2Byte 0x12
#define kDC3Byte 0x13
#define kNAKByte 0x15
#define kSYNByte 0x16
#define kETBByte 0x17
#define kCANByte 0x18

#define kAbecsByteTimeout 2 //(s)
#define kAbecsNonBlockingResponseTimeout 10 //(s)
#define kAbecsSilenceTimeout 500000 //(µs)

// Forward declarations
typedef struct abecs abecs_t;
typedef struct abecs_stream abecs_stream_t;

// Stream
typedef abecs_error_t (*abecs_stream_open_fn)(abecs_stream_t *stream, void *opendata);
typedef abecs_error_t (*abecs_stream_write_fn)(abecs_stream_t *stream, void *data, size_t len);
typedef abecs_error_t (*abecs_stream_data_received_fn)(abecs_stream_t *stream, void *data, size_t len);
typedef abecs_error_t (*abecs_stream_close_fn)(abecs_stream_t *stream);

typedef void (*abecs_packet_process_fn)(abecs_t *abecs, int cmd, void *packet);
typedef void (*abecs_error_callback)(abecs_t *abecs, abecs_error_t error);

struct abecs_stream {
	abecs_t *abecs;
	
	abecs_stream_open_fn open; /* Callback which opens a connection to the stream. */
	abecs_stream_write_fn write; /* Callback which writes to the stream. */
	abecs_stream_data_received_fn data_received; /* Callback which handles received data */
	abecs_stream_close_fn close; /* Callback which closes the connection to the stream. */
};

// General stream functions
abecs_stream_t *abecs_stream_new(abecs_stream_open_fn open, abecs_stream_write_fn write, abecs_stream_data_received_fn data_received, abecs_stream_close_fn close);
abecs_error_t abecs_stream_open(abecs_stream_t *stream, void *opendata);
abecs_error_t abecs_stream_write(abecs_stream_t *stream, void *data, size_t len);
abecs_error_t abecs_stream_data_received(abecs_stream_t *stream, void *data, size_t len);
abecs_error_t abecs_stream_close(abecs_stream_t *stream);
abecs_t *abecs_stream_get_abecs(abecs_stream_t *stream);
intptr_t abecs_stream_get_user_info(abecs_stream_t *stream);
void abecs_stream_free(abecs_stream_t *stream);

// Abecs
abecs_t *abecs_new(abecs_stream_t *stream, abecs_packet_process_fn handler, void *user_info);
void *abecs_get_user_info(abecs_t *abecs);
void abecs_set_user_info(abecs_t *abecs, void *user_info);
abecs_stream_t *abecs_get_stream(abecs_t *abecs);
void abecs_free(abecs_t *abecs);
abecs_error_t abecs_send_cancel(abecs_t *abecs);

#ifdef __cplusplus
}
#endif

#endif

