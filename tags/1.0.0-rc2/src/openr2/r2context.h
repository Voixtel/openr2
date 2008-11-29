/*
 * OpenR2 
 * MFC/R2 call setup library
 *
 * Moises Silva <moy@sangoma.com>
 * Copyright (C) 2008 Moises Silva
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _OPENR2_CONTEXT_H_
#define _OPENR2_CONTEXT_H_

#include <pthread.h>
#include <inttypes.h>
#include <stdarg.h>
#include "r2proto.h"
#include "r2log.h"

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __OR2_COMPILING_LIBRARY__
struct openr2_chan_s;
#define openr2_chan_t struct openr2_chan_s
struct openr2_context_s;
#define openr2_context_t struct openr2_context_s
#else
#ifndef OR2_CHAN_AND_CONTEXT_DEFINED
#define OR2_CHAN_AND_CONTEXT_DEFINED
typedef void* openr2_chan_t;
typedef void* openr2_context_t;
#endif
#endif

#define OR2_MAX_PATH 255

/* MF interface */
typedef void *(*openr2_mf_read_init_func)(void *read_handle, int forward_signals);
typedef void *(*openr2_mf_write_init_func)(void *write_handle, int forward_signals);
typedef int (*openr2_mf_detect_tone_func)(void *read_handle, const int16_t buffer[], int samples);
typedef int (*openr2_mf_generate_tone_func)(void *write_handle, int16_t buffer[], int samples);
typedef int (*openr2_mf_select_tone_func)(void *write_handle, char signal);
typedef int (*openr2_mf_want_generate_func)(void *write_handle, int signal);
typedef void (*openr2_mf_read_dispose_func)(void *read_handle);
typedef void (*openr2_mf_write_dispose_func)(void *write_handle);
typedef struct {
	/* init routines to detect and generate tones */
	openr2_mf_read_init_func mf_read_init;
	openr2_mf_write_init_func mf_write_init;

	/* detect and generation routines */
	openr2_mf_detect_tone_func mf_detect_tone;
	openr2_mf_generate_tone_func mf_generate_tone;

	/* choose the tone to transmit */
	openr2_mf_select_tone_func mf_select_tone;

	/* whether or not want mf_generate_tone() called */
	openr2_mf_want_generate_func mf_want_generate;

	/* routines to dispose resources allocated by handles. (optional) */
	openr2_mf_read_dispose_func mf_read_dispose;
	openr2_mf_write_dispose_func mf_write_dispose;
} openr2_mflib_interface_t;

/* Event Management interface. Users should provide
   this interface to handle library events like call starting, new call, read audio etc. */
typedef void (*openr2_handle_new_call_func)(openr2_chan_t *r2chan);
typedef void (*openr2_handle_call_offered_func)(openr2_chan_t *r2chan, const char *ani, const char *dnis, openr2_calling_party_category_t category);
typedef void (*openr2_handle_call_accepted_func)(openr2_chan_t *r2chan, openr2_call_mode_t mode);
typedef void (*openr2_handle_call_answered_func)(openr2_chan_t *r2chan);
typedef void (*openr2_handle_call_disconnect_func)(openr2_chan_t *r2chan, openr2_call_disconnect_cause_t cause);
typedef void (*openr2_handle_call_end_func)(openr2_chan_t *r2chan);
typedef void (*openr2_handle_call_read_func)(openr2_chan_t *r2chan, const unsigned char *buf, int buflen);
typedef void (*openr2_handle_os_error_func)(openr2_chan_t *r2chan, int oserrorcode);
typedef void (*openr2_handle_hardware_alarm_func)(openr2_chan_t *r2chan, int alarm);
typedef void (*openr2_handle_protocol_error_func)(openr2_chan_t *r2chan, openr2_protocol_error_t error);
typedef void (*openr2_handle_line_blocked_func)(openr2_chan_t *r2chan);
typedef void (*openr2_handle_line_idle_func)(openr2_chan_t *r2chan);
typedef void (*openr2_handle_billing_pulse_received_func)(openr2_chan_t *r2chan);
typedef int (*openr2_handle_dnis_digit_received_func)(openr2_chan_t *r2chan, char digit);
typedef void (*openr2_handle_ani_digit_received_func)(openr2_chan_t *r2chan, char digit);
typedef void (*openr2_handle_context_logging_func)(openr2_context_t *r2context, openr2_log_level_t level, const char *fmt, va_list ap);
typedef struct {
	/* A new call has just started. We will start to 
	   receive the ANI and DNIS */
	openr2_handle_new_call_func on_call_init;

	/* New call is ready to be accepted or rejected */
	openr2_handle_call_offered_func on_call_offered;

	/* Call has been accepted */
	openr2_handle_call_accepted_func on_call_accepted;

	/* Call has been answered */
	openr2_handle_call_answered_func on_call_answered;

	/* The far end has sent a disconnect signal */
	openr2_handle_call_disconnect_func on_call_disconnect;

	/* Disconnection process has end completely */
	openr2_handle_call_end_func on_call_end;

	/* Call has something to say */
	openr2_handle_call_read_func on_call_read;

	/* Hardware interface alarm event */
	openr2_handle_hardware_alarm_func on_hardware_alarm;

	/* some operating system error ocurred. 
	   Usually all the user can do in this
	   event is display some error or log it */
	openr2_handle_os_error_func on_os_error;

	/* protocol error */
	openr2_handle_protocol_error_func on_protocol_error;

	/* line BLOCKED event */
	openr2_handle_line_blocked_func on_line_blocked;

	/* line IDLE event */
	openr2_handle_line_idle_func on_line_idle;

	/* logging handler */
	openr2_handle_context_logging_func on_context_log;

	/* New DNIS digit arrived. If the user return any non zero
	   value OpenR2 will request more DNIS (if max DNIS still not reached),
	   if 0 is returned no more dnis will be requested regardless of the max DNIS limit*/
	openr2_handle_dnis_digit_received_func on_dnis_digit_received;

	/* New ANI digit arrived */
	openr2_handle_ani_digit_received_func on_ani_digit_received;

	/* Billing pulse arrived */
	openr2_handle_billing_pulse_received_func on_billing_pulse_received;
} openr2_event_interface_t;

/* Transcoding interface. Users should provide this interface
   to provide transcoding services from linear to alaw and 
   viceversa */
typedef int16_t (*openr2_alaw_to_linear_func)(uint8_t alaw);
typedef uint8_t (*openr2_linear_to_alaw_func)(int linear);
typedef struct {
	openr2_alaw_to_linear_func alaw_to_linear;
	openr2_linear_to_alaw_func linear_to_alaw;
} openr2_transcoder_interface_t;

/* Library errors */
typedef enum {
	/* Failed system call */
	OR2_LIBERR_SYSCALL_FAILED,
	/* Invalid channel signaling when creating it */
	OR2_LIBERR_INVALID_CHAN_SIGNALING,
	/* cannot set to IDLE the channel when creating it */
	OR2_LIBERR_CANNOT_SET_IDLE
} openr2_liberr_t;

int openr2_context_get_time_to_next_event(openr2_context_t *r2context);
openr2_context_t *openr2_context_new(openr2_mflib_interface_t *mflib, openr2_event_interface_t *callmgmt, 
		              openr2_transcoder_interface_t *transcoder, openr2_variant_t variant, int max_ani, int max_dnis);
void openr2_context_delete(openr2_context_t *r2context);
openr2_liberr_t openr2_context_get_last_error(openr2_context_t *r2context);
const char *openr2_context_error_string(openr2_liberr_t error);
openr2_variant_t openr2_context_get_variant(openr2_context_t *r2context);
int openr2_context_get_max_ani(openr2_context_t *r2context);
int openr2_context_get_max_dnis(openr2_context_t *r2context);
void openr2_context_set_ani_first(openr2_context_t *r2context, int ani_first);
int openr2_context_get_ani_first(openr2_context_t *r2context);
void openr2_context_set_immediate_accept(openr2_context_t *r2context, int immediate_accept);
int openr2_context_get_immediate_accept(openr2_context_t *r2context);
void openr2_context_set_log_level(openr2_context_t *r2context, openr2_log_level_t level);
openr2_log_level_t openr2_context_get_log_level(openr2_context_t *r2context);
void openr2_context_set_mf_threshold(openr2_context_t *r2context, int threshold);
int openr2_context_get_mf_threshold(openr2_context_t *r2context);
int openr2_context_set_log_directory(openr2_context_t *r2context, char *directory);
char *openr2_context_get_log_directory(openr2_context_t *r2context, char *directory, int len);
void openr2_context_set_mf_back_timeout(openr2_context_t *r2context, int ms);
int openr2_context_get_mf_back_timeout(openr2_context_t *r2context);
void openr2_context_set_metering_pulse_timeout(openr2_context_t *r2context, int ms);
int openr2_context_get_metering_pulse_timeout(openr2_context_t *r2context);
void openr2_context_set_double_answer(openr2_context_t *r2context, int enable);
int openr2_context_get_double_answer(openr2_context_t *r2context);
int openr2_context_configure_from_advanced_file(openr2_context_t *r2context, const char *filename);

#ifdef __OR2_COMPILING_LIBRARY__
#undef openr2_chan_t 
#undef openr2_context_t
#endif

#if defined(__cplusplus)
} /* endif extern "C" */
#endif

#endif /* endif defined _OPENR2_CONTEXT_H_ */
