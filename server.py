#!/usr/bin/env python
import zmq
import random
import logging

log = logging.getLogger(__name__)


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    ctx = zmq.Context()
    sck = ctx.socket(zmq.PUB)
    sck.bind('tcp://*:6667')

    rand = random.Random()

    i = 0
    while True:
        message = '%d %d' % (i % 30, rand.randint(1, 10**20))
        log.debug(message)
        sck.send(message)
        i += 1
