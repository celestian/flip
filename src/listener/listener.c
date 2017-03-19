#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/nbus/nbus.h"
#include "src/url/url.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"

int main(void)
{
    TALLOC_CTX* mem_ctx;
    struct url_conn_ctx* url_conn_ctx;
    struct nbus_ctx* nbus_ctx;
    struct string_ctx* chunk;
    errno_t ret;

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        printf("Error [%s:%d]\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    ret = nbus_init(mem_ctx, "ipc:///tmp/listener_pubsub.ipc", &nbus_ctx);
    if (ret != EOK) {
        printf("Error [%d] [%s:%d]\n", ret, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    ret = url_global_init();

    ret = url_init_ctx(mem_ctx, "https://btc-e.com/api/3/ticker/btc_usd",
                       &url_conn_ctx);
    if (ret != EOK) {
        printf("Error [%d] [%s:%d]\n", ret, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    int i = 10;
    while (i > 0) {
        ret = url_get_data(mem_ctx, url_conn_ctx, &chunk);
        if (ret != EOK) {
            printf("Error [%d] [%s:%d]\n", ret, __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }

        ret = nbus_send(nbus_ctx, chunk->data, chunk->size);
        if (ret != EOK) {
            printf("Error [%d] [%s:%d]\n", ret, __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }

        sleep(1);
        i--;
    }

    ret = nbus_close(nbus_ctx);
    if (ret != EOK) {
        printf("Error [%d] [%s:%d]\n", ret, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    ret = url_global_cleanup();

    talloc_free(mem_ctx);

    exit(EXIT_SUCCESS);
}

/*
int main(int argc, char* argv[]) {
    FILE *fp = NULL;
    pid_t process_id = 0;
    pid_t sid = 0;
    // Create child process
    process_id = fork();
    // Indication of fork() failure
    if (process_id < 0) {
        printf("fork failed!\n");
        // Return failure in exit status
        exit(1);
    }
    // PARENT PROCESS. Need to kill it.
    if (process_id > 0) {
        printf("process_id of child process %d \n", process_id);
        // return success in exit status
        exit(EXIT_SUCCESS);
    }
    //unmask the file mode
    umask(0);
    //set new session
    sid = setsid();
    if (sid < 0) {
        // Return failure
        exit(1);
    }
    // Change the current working directory to root.
    chdir("/");
    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    // Open a log file in write mode.
    fp = fopen("Log.txt", "w+");
    while (1) {
        //Dont block context switches, let the process sleep for some time
        sleep(1);
        fprintf(fp, "Logging info...\n");
        fflush(fp);
        // Implement and call some function that does core work for this daemon.
    }
    fclose(fp);
    exit(EXIT_SUCCESS);
}
 */
