#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/utils/daemon.h"
#include "src/utils/logs.h"

void run_daemon(const char *identity_tag)
{
    pid_t pid = 0;
    pid_t sid = 0;
    int ret;

    log_init(identity_tag);

    pid = fork();
    if (pid < 0) {
        LOG(LOG_CRIT, "Start of daemon '%s' failed. [fork()]", identity_tag);
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        LOG(LOG_CRIT, "Start of daemon '%s' failed. [setsid()]", identity_tag);
        exit(EXIT_FAILURE);
    }

    ret = chdir("/tmp");
    if (ret < 0) {
        LOG(LOG_CRIT, "Start of daemon '%s' failed. [chdir() [%i] [%s]]",
            identity_tag, ret, strerror(ret));
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
