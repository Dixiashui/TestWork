''' Define protocol for trigger telnet connection. This connection is used to exchange messages between TW and BoA for Triggers'''
import os
import pickle
import socket
import sys
from time import sleep

class TriggerMsg:
    """ defines Trigger messages type """ 
    ACK = 1000
    PRC = 1001

class TriggerAck(TriggerMsg):
    """ Ack message """
    def __init__(self):
        self.id = TriggerMsg.ACK
        self.status = -1

class TriggerPrc(TriggerMsg):
    """ Process message (execute) """
    def __init__(self):
        self.id = TriggerMsg.PRC

class TriggerSocket:
    """ Class created to define socket interface. """
    def __init__(self):
        pass

    def close(self):
        " Close connection "
        if self.s:
            self.s.close()

    def deserialize(self, data):
        " Created to deserialize objects using CPickle"
        return pickle.loads(data)

    def serialize(self, msg):
        " Created to serialize objects using CPickle"
        return pickle.dumps(msg)

class TriggerClientSocket(TriggerSocket):
    """ Client socket for triggers """
    def __init__(self, host, port, timeout):
        TriggerSocket.__init__(self)
        self.host = host
        self.port = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if timeout == -1:
            self.s.settimeout(7200)
        else:
            self.s.settimeout(timeout)
        try:
            self.s.connect((self.host, self.port))
        except socket.error:
            print("TriggerClientSocket Connection to BoA failed")
            sys.exit(1)

    def recv(self):
        " method to receive message "
        try:
            return self.deserialize(self.s.recv(1024))
        except socket.timeout:
            print("TriggerClientSocket Triggered command timed out")
            sys.exit(1)

    def send(self, msg):
        " method to send message "
        self.s.send(self.serialize(msg))

class TriggerServerSocket(TriggerSocket):
    """ Socket server for triggers. BoA works as a server to receive messages from TW. """
    def __init__(self, host, port):
        import logging
        TriggerSocket.__init__(self)
        self.host = host
        self.port = port
        self.c = None
        self.logger = logging.getLogger('execution')
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error, msg:
            self.logger.error(" Opening server socket failed")
            raise socket.error, msg
        try:
            self.s.settimeout(4)
        except socket.error, msg:
            self.logger.error(" Setting socket timeout failed: %s" % msg)
            raise socket.error, msg

        for i in range(1,11):
            try:
                self.s.bind((self.host, self.port))
                break
            except socket.error, msg:
                if i == 10:
                    self.logger.error(" bind failed: %s" % msg)
                    raise socket.error, msg
                else:
                    self.logger.info(" bind failed, it will be tried again")
            sleep(5)
        
        try:
            self.s.listen(1)
        except socket.error, msg:
            self.logger.error(" setting listening mode failed: %s" % msg)
            raise socket.error, msg

    def accept(self):
        " method for answer incoming connection request "
        self.c = None
        try:
            self.c, a = self.s.accept()
        except socket.timeout:
            pass
        except socket.error:
            self.logger.error("accept() socket error")
            raise socket.error, msg
        else:
            self.logger.debug("accept() connection from %s" % unicode(a))

    def is_connected(self):
        " return True if server is connected "
        if self.c is None:
            return False
        return True

    def disconnect(self):
        " disconnect clients. "
        if self.c:
            self.c.close()

    def recv(self):
        " receive message "
        return self.deserialize(self.c.recv(1024))

    def send(self, msg):
        " send message "
        if self.c:
            self.c.send(self.serialize(msg))
        else:
            self.logger.error("Trigger socket send :trying to send command %s", msg)


    def sendall(self, msg):
        " send all messaged in buffer "
        if self.c:
            self.c.sendall(self.serialize(msg))
        else:
            self.logger.error("Trigger socket sendall :trying to send command %s", msg)
        
    def tear_down(self):
        " clean up to be closed "
        self.disconnect()
        self.close()        
