#ifndef SMTP_CODES_H
#define SMTP_CODES_H

#include "sys/types.h"

#define SMTP_CODE_WRITE(fn, dst, name) \
    fn(dst, SMTP_CODE_##name, SMTP_CODE_SIZE_##name)

#define SMTP_CODE(name, code, msg) \
    static const char SMTP_CODE_##name[] = #code " " msg "\n"; \
    static const size_t SMTP_CODE_SIZE_##name = sizeof(SMTP_CODE_##name) - 1;

SMTP_CODE(READY, 220, "Service ready")
SMTP_CODE(BYE, 221, "Bye")
SMTP_CODE(OK, 250, "OK")
SMTP_CODE(QUEUED, 250, "Queued")

SMTP_CODE(START_MAIL_INPUT, 354, "Start mail input; end with <CRLF>.<CRLF>")

SMTP_CODE(SYNTAX_ERROR, 500, "Syntax error")
SMTP_CODE(COMMAND_UNRECOGNIZED, 500, "Command unrecognized")
SMTP_CODE(LINE_TOO_LONG, 500, "Line too long")

SMTP_CODE(COMMAND_NOT_IMPLEMENTED, 502, "Command not implemented")

SMTP_CODE(SEND_HELO_FIRST, 503, "Error: send HELO/EHLO first")
SMTP_CODE(ALREADY_IN_MAIL_TRANSACTION, 503, "Error: already in mail transaction")
SMTP_CODE(BAD_SEQUENCE_OF_COMMANDS, 503, "Bad sequence of commands")

#endif // SMTP_CODES_H
