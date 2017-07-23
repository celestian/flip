#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <talloc.h>
#include <tevent.h>

#include "src/common/utils/daemon.h"
#include "src/common/utils/logs.h"
#include "src/common/utils/utils.h"

static const char *static_variable_pid_file;

static errno_t write_pid()
{
    FILE *fp;
    int ret;

    if (is_file_exist(static_variable_pid_file)) {
        LOG(LOG_CRIT, "PID file '%s' exists.", static_variable_pid_file);
        ret = EINVAL;
        goto done;
    }

    fp = fopen(static_variable_pid_file, "w+");
    if (fp == NULL) {
        LOG(LOG_CRIT, "fopen('%s') failed. [%d | %s]", static_variable_pid_file,
            errno, strerror(errno));
        ret = EIO;
        goto done;
    }

    ret = fprintf(fp, "%d\n", getpid());
    if (ret < 1) {
        LOG(LOG_CRIT, "Can't write PID into '%s'. [%d | %s]",
            static_variable_pid_file, errno, strerror(errno));
        fclose(fp);
        ret = EIO;
        goto done;
    }

    fflush(fp);
    fclose(fp);
    ret = EOK;

done:
    return ret;
}

static errno_t remove_pid()
{
    int ret;

    if (is_file_exist(static_variable_pid_file)) {
        ret = unlink(static_variable_pid_file);
        if (ret != 0) {
            LOG(LOG_CRIT, "unlink('%s') failed. [%d | %s]",
                static_variable_pid_file, errno, strerror(errno));
            ret = EIO;
            goto done;
        }
    }

    ret = EOK;

done:
    return ret;
}

errno_t init_main_context(enum daemon_type daemon_type,
                          struct main_context **_main_ctx)
{
    struct tevent_context *event_ctx;
    struct main_context *main_ctx;
    errno_t ret;

    log_init(daemon_type);

    event_ctx = tevent_context_init(talloc_autofree_context());
    if (event_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        ret = ENOMEM;
        goto done;
    }

    main_ctx = talloc(event_ctx, struct main_context);
    if (main_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        ret = ENOMEM;
        goto done;
    }

    main_ctx->event_ctx = event_ctx;

    *_main_ctx = main_ctx;
    ret = EOK;

done:
    return ret;
}

void run_daemon(char *pid_file)
{
    pid_t pid = 0;
    pid_t sid = 0;
    int ret;

    static_variable_pid_file = pid_file;

    pid = fork();
    if (pid < 0) {
        LOG(LOG_CRIT, "fork() failed.");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        LOG(LOG_CRIT, "setsid() failed.");
        exit(EXIT_FAILURE);
    }

    ret = write_pid();
    if (ret != EOK) {
        LOG(LOG_CRIT, "write_pid() failed. []");
        exit(EXIT_FAILURE);
    }

    ret = chdir("/tmp");
    if (ret < 0) {
        LOG(LOG_CRIT, "chdir('/tmp') failed. [%d | %s]", errno,
            strerror(errno));
        exit(EXIT_FAILURE);
    }

#ifndef DEBUG
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
#endif
}

void stop_daemon()
{
    int ret;

    ret = remove_pid();
    if (ret != EOK) {
        LOG(LOG_CRIT, "remove_pid() failed.");
    }
}
