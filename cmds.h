#ifndef CMDS_H
#define CMDS_H
#include "types.h"

void quassel_login(GIOChannel *h, char *user, char *pass);
void HeartbeatReply(GIOChannel*);
void send_message(GIOChannel*, struct bufferinfo b, const char *message);
void initRequest(GIOChannel*, char *val, char *arg);
void quassel_request_backlog(GIOChannel *h, int buffer, int first, int last, int limit, int additional);
#endif
