#ifndef UTIL_H
#define UTIL_H

#include <ctype.h>
#include <inttypes.h>

#define D(c) ((c) - '0')
#define DDD(s) \
    (D(*s) * 100 + D(*(s + 1)) * 10 + D(*(s + 2)))

#define iscrlf(s) \
    ((*(s) == '\r') && (*(s + 1) == '\n'))

#define isat(c) ((c) == '@')
#define iscr(c) ((c) == '\r')
#define islf(c) ((c) == '\n')
#define islt(c) ((c) == '<')
#define isrt(c) ((c) == '>')
#define issp(c) ((c) == ' ')
#define ispt(c) ((c) == '.')
#define isnull(c) ((c) == '\0')
#define iscolon(c) ((c) == ':')
#define iscomma(c) ((c) == ',')
#define isbslash(c) ((c) == '\\')
#define isdquote(c) ((c == '"'))
#define ishyphen(c) ((c) == '-')
#define islquare(c) ((c) == '[')
#define isrquare(c) ((c) == ']')

#define isbetw(c, l, r) (((c) >= l) && ((c) <= r))

#define islt256(s, n) \
    ( \
        ( \
            (n == 1) || \
            (n == 2) || \
            ( \
                (n == 3) && (DDD(s) < 256) \
            ) \
        ) \
        ? 1 : 0 \
    )

#define toupper(c) ((c) - 'a' + 'A')

static int isatext(char c) {
    if (
        isalnum(c) ||
        (c == '!') ||
        (c == '#') ||
        (c == '$') ||
        (c == '%') ||
        (c == '&') ||
        (c == '\'') ||
        (c == '*') ||
        (c == '+') ||
        (c == '-') ||
        (c == '/') ||
        (c == '=') ||
        (c == '?') ||
        (c == '^') ||
        (c == '_') ||
        (c == '`') ||
        (c == '{') ||
        (c == '|') ||
        (c == '}') ||
        (c == '~')
        ) {
        return 1;
    }

    return 0;
}

char *string_new(const char *s, const char *e);
void string_copy(char **d, char *s);
void string_free(char **str);

int64_t unix_timestamp();

#endif // UTIL_H
