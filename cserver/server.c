#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <signal.h>
#include <zmq.h>

int is_running = 1;

void handle_sigint(int signum) {
    is_running = 0;
}

int main(int argc, char **argv) {
    int rc;
    unsigned i, randn;
    char message[100];
    void *ctx, *sck;

    ctx = zmq_ctx_new();
    assert(ctx);

    sck = zmq_socket(ctx, ZMQ_PUB);
    assert(sck);

    rc = zmq_bind(sck, "tcp://*:6667");
    if (rc == -1) {
        perror("zmq_bind()");
        goto cleanup;
    }

    srand(time(NULL));
    signal(SIGINT, handle_sigint);

    while (is_running) {
      randn = rand() * 2 ^ (8 * sizeof(randn));
      sprintf(message, "%d %u", i++ % 30, randn);
      printf("%s\n", message);

      rc = zmq_send(sck, &message, strlen(message) + 1, 0);
      if (rc == -1) {
        perror("zmq_send()");
        is_running = 0;
      }
    }

cleanup:
    printf("Cleaning up...\n");
    zmq_close(sck);
    zmq_ctx_destroy(ctx);

    return 0;
}
