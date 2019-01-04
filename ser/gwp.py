#!/usr/bin/python3

import sys, os, uuid, binascii
import enet
from enet import Host, Address, Peer, Event, Packet

def BinToHex(binary):
    hex = binascii.b2a_hex(binary).upper()
    return hex.decode('ascii')

def HexToBin(hex):
    binary = binascii.a2b_hex(hex)
    return binary

ENC = 'utf8'
PORT = 45812
HOSTNAME = "localhost".encode(ENC)
SERVICE_TIMEOUT = 500
CHANNEL = 0
PEER_ID = BinToHex(os.urandom(4))

def Send(peer, message):
    print("SENDING:", message)
    encMsg = message.encode(ENC)
    peer.send(CHANNEL, Packet(encMsg))

def server():
    host = Host(Address(HOSTNAME, PORT), 99, 99)
    print("Listening...")
    while True:
        event = host.service(SERVICE_TIMEOUT)
        if not event or event.type == enet.EVENT_TYPE_NONE:
            #print("SERVER: NONE")
            pass
        elif event.type == enet.EVENT_TYPE_CONNECT:
            print("Connected:", STR(event.peer))
        elif event.type == enet.EVENT_TYPE_DISCONNECT:
            print("Disconnected:", STR(event.peer))
        elif event.type == enet.EVENT_TYPE_RECEIVE:
            handleMessage(event)
        else:
            print("Unhandled event.")
            print(STR(event))

def handleMessage(event):
    print("Received message:", STR(event))
    message = event.packet.data.decode(ENC)
    if message.startswith("CONNECT "):
        _, clientId, username, password = message.split()
        print("CONNECT: [%s] %s:%s" % (clientId, username, password))
        # TODO: Verify username and password.
        Send(event.peer, "HELLO "+username)
    elif message.startswith("MOVETO "):
        pass
    else:
        print("Unknown message type, nothing to do.")

def STR(x):
    if isinstance(x, Event):
        return ("EVENT(type=%d, channelID=%d, peer=%s, packet=%s)"
            % (x.type, x.channelID, STR(x.peer), STR(x.packet)))
    elif isinstance(x, Peer):
        return ("PEER(state=%d, incomingPeerID=%d)" % (x.state, x.incomingPeerID))
    elif isinstance(x, Packet):
        if x.is_valid:
            return ("PACKET(data=\"%s\", flags=%d)" % (x.data, x.flags))
        else:
            return "PACKET(invalid)"
    else:
        return str(x)

if __name__ == '__main__':
    server()

