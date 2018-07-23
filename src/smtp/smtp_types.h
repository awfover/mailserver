#ifndef SMTP_TYPES_H
#define SMTP_TYPES_H

#include <pthread.h>
#include <inttypes.h>
#include <sys/types.h>

#include "conn.h"
#include "linked.h"
#include "listener.h"

typedef enum {
    SMTP_CMD_TOKEN_NULL,
    SMTP_CMD_TOKEN_DOMAIN,
    SMTP_CMD_TOKEN_ADDR_V4,
    SMTP_CMD_TOKEN_PATH,
    SMTP_CMD_TOKEN_DOMAIN_LIST,
    SMTP_CMD_TOKEN_MAILBOX,
    SMTP_CMD_TOKEN_DOT_STR,
    SMTP_CMD_TOKEN_QUOTED_STR
} smtp_cmd_token_enum_t;

typedef struct {
    smtp_cmd_token_enum_t k;
    void *v;
} smtp_cmd_token_t;

typedef struct {
    size_t n;
    smtp_cmd_token_t *ts;
} smtp_cmd_token_list_t;

typedef void smtp_cmd_para_t;

typedef char * smtp_addr_v4_t;
typedef char * smtp_domain_t;
typedef char * smtp_dot_str_t;
typedef char * smtp_quoted_str_t;

typedef struct {
    smtp_cmd_token_t *lp;
    smtp_cmd_token_t *droa;
} smtp_mailbox_t;

typedef struct {
    smtp_cmd_token_t *ds;
    smtp_cmd_token_t *mb;
} smtp_path_t;

static const char *SMTP_CMD_NAMES[] = {
    "EHLO",
    "HELO",
    "MAIL",
    "RCPT",
    "DATA",
    "RSET",
    "NOOP",
    "QUIT",
    "VRFY"
};
static const size_t SMTP_CMD_NAME_SIZE = 4;
static const size_t SMTP_CMD_NAMES_SIZE = sizeof(SMTP_CMD_NAMES) / sizeof(SMTP_CMD_NAMES[0]);

typedef enum {
    SMTP_CMD_NULL,
    SMTP_CMD_EHLO,
    SMTP_CMD_HELO,
    SMTP_CMD_MAIL,
    SMTP_CMD_RCPT,
    SMTP_CMD_DATA,
    SMTP_CMD_RSET,
    SMTP_CMD_NOOP,
    SMTP_CMD_QUIT,

    SMTP_CMD_NOT_IMP,
    SMTP_CMD_VRFY
} smtp_cmd_enum_t;

typedef struct {
    smtp_cmd_enum_t k;
    smtp_cmd_token_t *t;
} smtp_cmd_t;

typedef struct {
    in_port_t port;
    uint16_t backlog;
    size_t maxevents;
    size_t maxworkers;
    size_t stream_bsize;
    const char *mailsdir;
} smtp_opt_t;

typedef struct {
    size_t id;
    void *s;
    pthread_t *thread;
    void (*conn_close)(void *s, conn_t *c);
} smtp_worker_t;

typedef struct {
    int fd;
    int started;
    listener_t *l;
    smtp_opt_t *opt;
    linked_t *conns;
    pthread_mutex_t *mux;
    pthread_cond_t *cond;
    smtp_worker_t *workers;
} smtp_t;

typedef enum {
    SMTP_SESSION_NULL,
    SMTP_SESSION_GREETED,
    SMTP_SESSION_INITIALIZED,
    SMTP_SESSION_IN_MAIL,
    SMTP_SESSION_IN_RECIPIENTS,
    SMTP_SESSION_IN_DATA,
    SMTP_SESSION_CLOSED
} smtp_session_state_enum_t;

static const char *smtp_session_state_enum_text[] = {
    "SMTP_SESSION_NULL",
    "SMTP_SESSION_GREETED",
    "SMTP_SESSION_INITIALIZED",
    "SMTP_SESSION_IN_MAIL",
    "SMTP_SESSION_IN_RECIPIENTS",
    "SMTP_SESSION_IN_DATA",
    "SMTP_SESSION_CLOSED"
};

typedef struct {
    smtp_t *s;
    conn_t *c;
    smtp_session_state_enum_t state;

    char *mailpath;

#define SMTP_CMD_LINE_LENGTH 512
    char cmdln[SMTP_CMD_LINE_LENGTH];

#define SMTP_DOMAIN_LENGTH 255
    char client[SMTP_DOMAIN_LENGTH + 1];

    smtp_cmd_token_t *rvrs_path;
    linked_t *forw_path_list;
} smtp_session_t;

#endif // SMTP_TYPES_H
