#!/usr/bin/env python
import datetime
import logging

import zmq
import msgpack

log = logging.getLogger(__name__)


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    ctx = zmq.Context()
    sck = ctx.socket(zmq.SUB)
    sck.connect('tcp://localhost:6667')

    filter_str = ''
    if isinstance(filter_str, bytes):
        # python 2.x: ascii -> unicode
        filter_str = filter_str.decode('ascii')

    sck.setsockopt_string(zmq.SUBSCRIBE, filter_str)

    last = []
    while True:
        b = sck.recv(copy=False)
        print repr(b)

        message = msgpack.unpackb(b)
        print message
        break

        i, num = message.split(' ', 1)
        last.append(int(num))
        if len(last) == 10**4:
            average = reduce(lambda x, y: x + y, last, 0) / len(last)
            log.debug(average)
            last = []
