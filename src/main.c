
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "log.h"
#include "smtp.h"
#include "smtp_types.h"

void server_exit(int sig) {
    exit(0);
}

int main(int argc, char *argv[]) {
    log_init(NULL);

    char *pwd = getcwd(NULL, 0);
    char mailsdir[512];
    sprintf(mailsdir, "%s/mails", pwd);

    smtp_opt_t opt = {
        25,
        10,
        10,
        4,
        512,
        mailsdir
    };

    signal(SIGINT, server_exit);

    smtp_t *s = smtp_new(&opt);
    int err = smtp_start(s);
    if (err < 0) {
        return -1;
    }

    for(;;) {}

    return 0;
}
