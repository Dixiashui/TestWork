#! /usr/bin/env python

import sys

def trigger(host, port, timeout):
    from trigger_protocol import TriggerClientSocket, TriggerMsg, TriggerPrc, TriggerAck
    cs = TriggerClientSocket(host, int(port), int(timeout))
    prc = TriggerPrc()
    cs.send(prc)
    ack = cs.recv()
    cs.close()
    if ack.id != TriggerMsg.ACK:
        print('Trigger request timed out')
        sys.exit(1)

if __name__ == '__main__':
    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print('Usage automation_trigger.py host port [timeout]')
        sys.exit(1)

    if not sys.argv[2].isdigit():
        print('Port must be numeric')
        sys.exit(1)

    sys.path.append('/opt/testworks/lib')

    timeout = 120
    host = sys.argv[1]
    port = sys.argv[2]
    
    if len(sys.argv) == 4:
        if not sys.argv[3].isdigit():
            if sys.argv[3] == '-1':
                timeout = sys.argv[3]
            else:
                print('Timeout must be numeric')
                sys.exit(1)
        else:
            timeout = sys.argv[3]

    trigger(host, port, timeout)
