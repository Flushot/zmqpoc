#!/usr/bin/env python
import zmq
import datetime
import logging

log = logging.getLogger(__name__)


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG)

    ctx = zmq.Context()
    sck = ctx.socket(zmq.SUB)
    sck.connect('tcp://localhost:6667')

    filter_str = '10'
    if isinstance(filter_str, bytes):
        # python 2.x: ascii -> unicode
        filter_str = filter_str.decode('ascii')

    sck.setsockopt_string(zmq.SUBSCRIBE, filter_str)

    while True:
        message = sck.recv_string()
        i, num = message.split(' ', 1)
        log.debug('%s -> %s' % (i, num))
