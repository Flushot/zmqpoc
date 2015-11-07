#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <zmq.h>

static int is_running = 1;

static void die_signal_handler(int signum) {
    is_running = 0;
}

int main(int argc, char **argv) {
    unsigned i, randn, msg_ctr;
    char message[100];
    void *ctx, *sck;  /* zmq socket */
    clock_t begin, end;  /* perf timing */
    double elapsed;

    /* printf number formatting with commas */
    setlocale(LC_NUMERIC, "");

    ctx = zmq_ctx_new();
    assert(ctx);

    sck = zmq_socket(ctx, ZMQ_PUB);
    assert(sck);

    if (zmq_bind(sck, "tcp://*:6667") == -1) {
        perror("zmq_bind()");
        goto cleanup;
    }

    srand(time(NULL));
    signal(SIGINT, die_signal_handler);
    signal(SIGTERM, die_signal_handler);

    for (i = 0, msg_ctr = 0, begin = clock(); is_running; ++i, ++msg_ctr) {
      randn = rand() * 2 ^ (8 * sizeof(randn));
      snprintf(message, sizeof(message), "%d %u", i % 30, randn);

      if (zmq_send(sck, &message, strlen(message), 0) == -1) {
        perror("zmq_send()");
        break;
      }

      end = clock();
      elapsed = (double)(end - begin) / CLOCKS_PER_SEC;
      if (elapsed >= 1.0) {
        printf("Published: %'d messages/sec\n", msg_ctr);
        msg_ctr = 0;
        begin = clock();
      }
    }

cleanup:
    printf("Cleaning up...\n");
    zmq_close(sck);
    zmq_ctx_destroy(ctx);

    return 0;
}
