#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/utils/daemon.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"
#include "src/utils/utils.h"

// static const char *pidfile;

/*
static errno_t write_pid()
{
    FILE *f;
    int ret;

    if (is_file_exist(pidfile)) {
        LOG(LOG_CRIT, "PID file '%s' exists.", pidfile);
        ret = EINVAL;
        goto done;
    }

    f = fopen(pidfile, "w+");
    if (f == NULL) {
        LOG(LOG_CRIT, "fopen('%s') failed. [%d | %s]", pidfile, errno,
            strerror(errno));
        ret = EIO;
        goto done;
    }

    ret = fprintf(f, "%d\n", getpid());
    if (ret < 1) {
        LOG(LOG_CRIT, "Can't write PID into '%s'. [%d | %s]", pidfile, errno,
            strerror(errno));
        fclose(f);
        ret = EIO;
        goto done;
    }

    fflush(f);
    fclose(f);
    ret = EOK;

done:
    return ret;
}
*/

/*
static errno_t remove_pid()
{
    int ret;

    ret = unlink(pidfile);
    if (ret != 0) {
        LOG(LOG_CRIT, "unlink('%s') failed. [%d | %s]", pidfile, errno,
            strerror(errno));
        ret = EIO;
        goto done;
    }

    ret = EOK;

done:
    return ret;
}
*/

void run_daemon(const char *identity_tag, char *work_dir)
{
    pid_t pid = 0;
    pid_t sid = 0;
    // TODO: This name have to be generic
    // pidfile = "/tmp/flip/daemon.pid";
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

    /*
        ret = write_pid();
        if (ret != EOK) {
            LOG(LOG_CRIT, "Start of daemon '%s' failed. [write_pid()]",
                identity_tag);
            exit(EXIT_FAILURE);
        }
    */

    ret = chdir(work_dir);
    if (ret < 0) {
        LOG(LOG_CRIT, "Start of daemon '%s' failed: chdir('%s') [%d | %s]",
            identity_tag, work_dir, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

/*
void stop_daemon()
{
    int ret;

    ret = remove_pid(pidfile);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Daemon crashed: remove_pid('%s')", pidfile);
    }
}
*/
