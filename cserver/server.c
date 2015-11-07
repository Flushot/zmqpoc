#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <signal.h>
#include <locale.h>
#include <zmq.h>

int is_running = 1;

void die_signal_handler(int signum) {
    is_running = 0;
}

int main(int argc, char **argv) {
    int rc;
    unsigned i, randn;
    char message[100];
    void *ctx, *sck;  /* zmq socket */
	clock_t begin, end;  /* perf timing */
	double elapsed;
	int msg_ctr = 0;

	/* printf number formatting with commas */
	setlocale(LC_NUMERIC, "");

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
    signal(SIGINT, die_signal_handler);
    signal(SIGTERM, die_signal_handler);

	begin = clock();
    while (is_running) {
      randn = rand() * 2 ^ (8 * sizeof(randn));
      sprintf(message, "%d %u", i++ % 30, randn);

      rc = zmq_send(sck, &message, strlen(message) + 1, 0);
      if (rc == -1) {
        perror("zmq_send()");
        is_running = 0;
      }

	  ++msg_ctr;

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
