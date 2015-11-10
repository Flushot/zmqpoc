#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include <zmq.h>
#include <msgpack.h>

static int is_running = 1;

static void die_signal_handler(int signum) {
    is_running = 0;
}

int main(int argc, char **argv) {
    int message_length;
    unsigned i, randn, msg_ctr;
    msgpack_sbuffer sbuf;
    msgpack_packer pk;
    msgpack_zone mempool;
    zmq_msg_t message;

    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    msgpack_zone_init(&mempool, 2048);

    void *ctx, *sck;  /* zmq socket */

    clock_t begin, end;  /* perf timing */
    double elapsed;

    /* printf number formatting with commas */
    setlocale(LC_NUMERIC, "");

    ctx = zmq_ctx_new();
    assert(ctx);

    sck = zmq_socket(ctx, ZMQ_PUB);
    assert(sck);

    int hwm = 0;
    if (zmq_setsockopt(sck, ZMQ_SNDHWM, &hwm, sizeof(hwm)) == -1) {
        perror("zmq_setsockopt()");
        goto cleanup;
    }

    if (zmq_bind(sck, "tcp://*:6667") == -1) {
        perror("zmq_bind()");
        goto cleanup;
    }

    srand(time(NULL));
    signal(SIGINT, die_signal_handler);
    signal(SIGTERM, die_signal_handler);

    for (i = 0, msg_ctr = 0, begin = clock(); is_running; ++i, ++msg_ctr) {
      randn = rand() * 2 ^ (8 * sizeof(randn));

      msgpack_sbuffer_clear(&sbuf);
      msgpack_pack_unsigned_int(&pk, i % 30);
      msgpack_pack_unsigned_int(&pk, randn);

      if (zmq_msg_init_data(&message, sbuf.data, sbuf.size, NULL, NULL) == -1) {
        perror("zmq_msg_init_data()");
        break;
      }

      if (zmq_sendmsg(sck, &message, 0) == -1) {
        perror("zmq_sendmsg()");
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

    msgpack_zone_destroy(&mempool);
    msgpack_sbuffer_destroy(&sbuf);

    return 0;
}
