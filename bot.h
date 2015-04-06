#ifndef BOT_H
#define BOT_H
#include "types.h"
#include <stdarg.h>

void handle_message(struct message m, void*);
void handle_backlog(struct message m, void*);
void handle_fd(int fd);

typedef enum {
	BufferSyncer,
	IrcChannel,
	IrcUser,
	Network,
} object_t;

typedef enum {
	/* BufferSyncer */
	Create,			//(int bufferid, int network, char *name);
	TempRemoved,		//(int bufferid);
	Removed,		//(int bufferid);
	Displayed,		//(int bufferid);
	MarkBufferAsRead,	//(int bufferid);
	SetLastSeenMsg,		//(int bufferid, int messageid);
	SetMarkerLine,		//(int bufferid, int messageid);
	DoneBuffersInit,	//(int infoid)
	/* IrcChannel */
	JoinIrcUsers,		//(char *network_number, char *channel, int size, char **users, char **modes);
	AddUserMode,		//(char *network_number, char *channel, char *user, char *mode);
	RemoveUserMode,		//(char *network_number, char *channel, char *user, char *mode);
	/* IrcUser */
	Quit,			//(char *network_number, char *nick);
	SetServer,		//(char *network_number, char *nick, char *server);
	SetRealName,		//(char *network_number, char *nick, char *realname);
	SetAway,		//(char *network_number, char *nick, char away); //=1 if away, 0 else
	SetNick2,		//(char *network_number, char *nick); //Only new nick available, not old one
	PartChannel,		//(char *network_number, char *nick, char *channel);
	SetNick,		//(char *network_number, char *old_nick, char *new_nick);
	/* Network */
	AddIrcUser,		//(char *network_number, char *fullname);
	SetLatency,		//(char *network_number, int latency); //unit ?
	MyNick,			//(char *network_number, char* nick);
} function_t;
void handle_sync(void* arg, object_t o, function_t f, ...);

typedef enum {
	ClientInitAck,
	SessionInit,
	TopicChange,
	ChanPreAddUser,
	ChanReady,
	// Errors start from 0x1000
	ClientLoginReject = 0x1000,
} event_t;
void handle_event(void* arg, GIOChannel* h, event_t t, ...);
#endif
