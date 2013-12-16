#ifndef QUASSEL_EXPORT_H
#define QUASSEL_EXPORT_H

/* quassel plugin */
extern void quassel_irssi_init_ack(void *server);
extern void quassel_irssi_set_last_seen_msg(void *arg, int buffer_id, int msgid);
extern void quassel_irssi_backlog(void* server, int msg_id, int timestamp, int bufferid, int network, char* buffer_id, char* sender, int type, int flags, char* content);
extern void quassel_irssi_join(void* arg, int network, char *chan, char* nick, char* mode);
extern void quassel_irssi_joined(void* arg, int network, char *chan);
extern void quassel_irssi_handle(void* r, int msg_id, int bufferid, int network, char* buffer_id, char* sender, int type, int flags, char* content);
extern void quassel_irssi_topic(void* server, int network, char *chan, char *topic);
extern void quassel_irssi_handle_connected(void*);
extern void quassel_irssi_hide(void* arg, int net, const char* chan);

/* quassel lib */
extern void quassel_send_message(GIOChannel* h, int buffer, const char *message);
extern void quassel_mark_as_read(GIOChannel* h, int buffer_id);
extern void quassel_set_last_seen_msg(GIOChannel* h, int buffer_id, int msg_id);
extern void quassel_set_marker(GIOChannel* h, int buffer_id, int msgid);
extern void quassel_login(GIOChannel* h, char *user, char *pass);
extern void quassel_init_packet(GIOChannel* h, int ssl);
extern int quassel_parse_message(GIOChannel* h, char *buf, void *arg);
extern void quassel_request_backlog(GIOChannel *h, int buffer, int first, int last, int limit, int additional);
extern int quassel_find_buffer_id(const char *name, uint32_t network);
#endif
