/*
   This file is part of QuasselC.

   QuasselC is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   QuasselC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with QuasselC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iconv.h>
#include "quasselc.h"
#include "export.h"

//Copy paste from irssi's write_io
static inline int write_io(GIOChannel *handle, const char* data, int len) {
	gsize ret;
	GIOStatus status;
	GError *err = NULL;

	g_return_val_if_fail(handle != NULL, -1);
	g_return_val_if_fail(data != NULL, -1);

	status = g_io_channel_write_chars(handle, (char *) data, len, &ret, &err);
	if (err != NULL) {
		g_warning("%s", err->message);
		g_error_free(err);
	}
	if (status == G_IO_STATUS_ERROR)
		return -1;

	return ret;

}

void quassel_mark_as_read(GIOChannel* h, int buffer_id) {
	char msg[2048];
	int size = 0;
	bzero(msg, sizeof(msg));

	//A list
	size += add_qvariant(msg+size, 9);
	//5 elements
	size += add_int(msg+size, 5);

	//A sync operation
	size += add_qvariant(msg+size, 2);
	size += add_int(msg+size, 1);

	//'BufferSyncer' bytearray
	size += add_qvariant(msg+size, 12);
	size += add_bytearray(msg+size, "BufferSyncer");

	//Empty string
	size += add_qvariant(msg+size, 10);
	size += add_int(msg+size, 0xffffffff);

	//'markBufferAsRead' bytearray
	size += add_qvariant(msg+size, 12);
	size += add_bytearray(msg+size, "requestMarkBufferAsRead");

	//BufferId type
	size += add_qvariant(msg+size, 127);
	size += add_bytearray(msg+size, "BufferId");
	size += add_int(msg+size, buffer_id);

	uint32_t v = htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void quassel_set_last_seen_msg(GIOChannel* h, int buffer_id, int msg_id) {
	char msg[2048];
	int size = 0;
	bzero(msg, sizeof(msg));

	//A list
	size += add_qvariant(msg+size, 9);
	//5 elements
	size += add_int(msg+size, 6);

	//A sync operation
	size += add_qvariant(msg+size, 2);
	size += add_int(msg+size, 1);

	//'BufferSyncer' bytearray
	size += add_qvariant(msg+size, 12);
	size += add_bytearray(msg+size, "BufferSyncer");

	//Empty string
	size += add_qvariant(msg+size, 10);
	size += add_int(msg+size, 0xffffffff);

	size += add_qvariant(msg+size, 12);
	size += add_bytearray(msg+size, "requestSetLastSeenMsg");

	//BufferId type
	size += add_qvariant(msg+size, 127);
	size += add_bytearray(msg+size, "BufferId");
	size += add_int(msg+size, buffer_id);

	//MsgId type
	size += add_qvariant(msg+size, 127);
	size += add_bytearray(msg+size, "MsgId");
	size += add_int(msg+size, msg_id);

	uint32_t v = htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void quassel_set_marker(GIOChannel* h, int buffer_id, int msg_id) {
	char msg[2048];
	int size = 0;
	bzero(msg, sizeof(msg));

	//A list
	size += add_qvariant(msg+size, 9);
	//5 elements
	size += add_int(msg+size, 6);

	//A sync operation
	size += add_qvariant(msg+size, 2);
	size += add_int(msg+size, 1);

	//'BufferSyncer' bytearray
	size += add_qvariant(msg+size, 12);
	size += add_bytearray(msg+size, "BufferSyncer");

	//Empty string
	size += add_qvariant(msg+size, 10);
	size += add_int(msg+size, 0xffffffff);

	size += add_qvariant(msg+size, 12);
	size += add_bytearray(msg+size, "requestSetMarkerLine");

	//BufferId type
	size += add_qvariant(msg+size, 127);
	size += add_bytearray(msg+size, "BufferId");
	size += add_int(msg+size, buffer_id);

	//MsgId type
	size += add_qvariant(msg+size, 127);
	size += add_bytearray(msg+size, "MsgId");
	size += add_int(msg+size, msg_id);

	uint32_t v = htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void quassel_login(GIOChannel* h, char *user, char *pass) {
	//HeartBeat
	char msg[2048];
	int size;
	int elements;

	size=0;
	elements=0;
	bzero(msg, sizeof(msg));

	size+=add_string_in_map(msg+size, "MsgType", "ClientLogin");
	elements++;

	size+=add_string_in_map(msg+size, "User", user);
	elements++;

	size+=add_string_in_map(msg+size, "Password", pass);
	elements++;

	//The message will be of that length
	uint32_t v=htonl(size+4/*QMap type*/+1/*valid*/+4/*n elements*/);
	write_io(h, (char*)&v, 4);
	//This is a QMap
	v=htonl(8);
	write_io(h, (char*)&v, 4);
	
	//QMap is valid
	v=0;
	write_io(h, (char*)&v, 1);
	//The QMap has <elements> elements
	v=htonl(elements);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void HeartbeatReply(GIOChannel* h) {
	//HeartBeat
	char msg[2048];
	int size;

	size=0;
	bzero(msg, sizeof(msg));

	//QVariant<QList<QVariant>> of 2 elements
	size+=add_qvariant(msg+size, 9);
	size+=add_int(msg+size, 2);

	//QVariant<Int> = HeartbeatReply
	size+=add_qvariant(msg+size, 2);
	size+=add_int(msg+size, 6);

	//QTime
	size+=add_qvariant(msg+size, 15);
	//I don't have any clue what time it is...
	size+=add_int(msg+size, 0);

	//The message will be of that length
	uint32_t v=htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void send_message(GIOChannel*h, struct bufferinfo b, const char *message) {
	//HeartBeat
	char msg[2048];
	int size;

	size=0;
	bzero(msg, sizeof(msg));

	//QVariant<QList<QVariant>> of 4 elements
	size+=add_qvariant(msg+size, 9);
	size+=add_int(msg+size, 4);

	//QVariant<Int> = RPC
	size+=add_qvariant(msg+size, 2);
	size+=add_int(msg+size, 2);

	//QVariant<Bytearray> = "2sendInput(BufferInfo,QString)"
	size+=add_qvariant(msg+size, 12);
	size+=add_bytearray(msg+size, "2sendInput(BufferInfo,QString)");

	//QVariant<BufferInfo>
	size+=add_qvariant(msg+size, 127);
	size+=add_bytearray(msg+size, "BufferInfo");
	size+=add_bufferinfo(msg+size, b);

	//String
	size+=add_qvariant(msg+size, 10);
	size+=add_string(msg+size, message);

	//The message will be of that length
	uint32_t v=htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void initRequest(GIOChannel* h, char *val, char *arg) {
	char msg[2048];
	int size;
	
	size=0;
	bzero(msg, sizeof(msg));

	//QVariant<QList<QVariant>> of 3 elements
	size+=add_qvariant(msg+size, 9);
	size+=add_int(msg+size, 3);

	//QVariant<Int> = InitRequest
	size+=add_qvariant(msg+size, 2);
	size+=add_int(msg+size, 3);

	size+=add_qvariant(msg+size, 10);
	size+=add_string(msg+size, val);

	size+=add_qvariant(msg+size, 10);
	size+=add_string(msg+size, arg);

	//The message will be of that length
	uint32_t v=htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void quassel_request_backlog(GIOChannel *h, int buffer, int first, int last, int limit, int additional) {
	char msg[2048];
	int size;
	
	size=0;
	bzero(msg, sizeof(msg));

	//QVariant<QList<QVariant>> of 9 elements
	size+=add_qvariant(msg+size, 9);
	size+=add_int(msg+size, 9);

	//QVariant<Int> = Sync
	size+=add_qvariant(msg+size, 2);
	size+=add_int(msg+size, 1);

	size+=add_qvariant(msg+size, 10);
	size+=add_string(msg+size, "BacklogManager");

	size+=add_qvariant(msg+size, 10);
	size+=add_string(msg+size, "");

	size+=add_qvariant(msg+size, 10);
	size+=add_string(msg+size, "requestBacklog");

	size+=add_qvariant(msg+size, 127);
	size+=add_bytearray(msg+size, "BufferId");
	size+=add_int(msg+size, buffer);

	size+=add_qvariant(msg+size, 127);
	size+=add_bytearray(msg+size, "MsgId");
	size+=add_int(msg+size, first);

	size+=add_qvariant(msg+size, 127);
	size+=add_bytearray(msg+size, "MsgId");
	size+=add_int(msg+size, last);

	size+=add_qvariant(msg+size, 2);
	size+=add_int(msg+size, limit);

	size+=add_qvariant(msg+size, 2);
	size+=add_int(msg+size, additional);


	//The message will be of that length
	uint32_t v=htonl(size);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}

void quassel_init_packet(GIOChannel* h, int ssl) {
	int size=0;
	int elements=0;
	char msg[2048];

	size+=add_bool_in_map(msg+size, "UseSsl", !!ssl);
	elements++;

	size+=add_bool_in_map(msg+size, "UseCompression", 0);
	elements++;

	size+=add_int_in_map(msg+size, "ProtocolVersion", 10);
	elements++;

	size+=add_string_in_map(msg+size, "MsgType", "ClientInit");
	elements++;

	size+=add_string_in_map(msg+size, "ClientVersion", "v0.6.1 (Quassel-IRSSI)");
	elements++;

	size+=add_string_in_map(msg+size, "ClientDate", "Oct 23 2011 18:00:00");
	elements++;

	//The message will be of that length
	unsigned int v=htonl(size+4/*QMap type*/+1/*valid*/+4/*n elements*/);
	write_io(h, (char*)&v, 4);
	//This is a QMap
	v=htonl(8);
	write_io(h, (char*)&v, 4);
	
	//QMap is valid
	v=0;
	write_io(h, (char*)&v, 1);
	//The QMap has <elements> elements
	v=htonl(elements);
	write_io(h, (char*)&v, 4);
	write_io(h, msg, size);
}
