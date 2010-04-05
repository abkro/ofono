/*
 *
 *  PPP library with GLib integration
 *
 *  Copyright (C) 2009-2010  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

struct pppcp_data;

/* option format */
struct ppp_option {
	guint8 type;
	guint8 length;
	guint8 data[0];
};

enum option_rval {
	OPTION_ACCEPT,
	OPTION_REJECT,
	OPTION_NAK,
	OPTION_ERR,
};

struct pppcp_action {
	void (*this_layer_up)(struct pppcp_data *data);
	void (*this_layer_down)(struct pppcp_data *data);
	void (*this_layer_started)(struct pppcp_data *data);
	void (*this_layer_finished)(struct pppcp_data *data);
	enum option_rval (*option_scan)(struct ppp_option *option,
						gpointer user_data);
	void (*option_process)(gpointer option, gpointer user_data);
};

struct pppcp_packet {
	guint8 code;
	guint8 identifier;
	guint16 length;
	guint8 data[0];
} __attribute__((packed));

struct pppcp_timer_data {
	struct pppcp_data *data;
	guint restart_counter;
	guint restart_interval;
	guint max_counter;
	guint restart_timer;
};

struct pppcp_data {
	unsigned char state;
	struct pppcp_timer_data config_timer_data;
	struct pppcp_timer_data terminate_timer_data;
	guint max_failure;
	guint failure_counter;
	guint32 magic_number;
	GList *config_options;
	GList *acceptable_options;
	GList *unacceptable_options;
	GList *rejected_options;
	GList *applied_options;
	GAtPPP *ppp;
	guint8 identifier;  /* don't think I need this now */
	guint8 config_identifier;
	guint8 terminate_identifier;
	guint8 reject_identifier;
	struct pppcp_action *action;
	guint16 valid_codes;
	guint8 (*packet_ops[11])(struct pppcp_data *data,
					struct pppcp_packet *packet);
	gpointer priv;
	guint16 proto;
	const char *prefix;
	const char **option_strings;
};

struct pppcp_data *pppcp_new(GAtPPP *ppp, guint16 proto);
void pppcp_free(struct pppcp_data *data);
void pppcp_add_config_option(struct pppcp_data *data,
				struct ppp_option *option);
void pppcp_set_valid_codes(struct pppcp_data *data, guint16 codes);
void pppcp_process_packet(gpointer priv, guint8 *new_packet);
void pppcp_send_protocol_reject(struct pppcp_data *data,
				guint8 *rejected_packet, gsize len);
void pppcp_signal_open(struct pppcp_data *data);
void pppcp_signal_close(struct pppcp_data *data);
void pppcp_signal_up(struct pppcp_data *data);
