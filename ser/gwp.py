#!/usr/bin/python3
# vim: et ts=8 sts=4 sw=4

import sys, os, uuid, binascii, random, time, struct
import enet
from enet import Host, Address, Peer, Event, Packet

def BinToHex(binary):
    hex = binascii.b2a_hex(binary).upper()
    return hex.decode('ascii')

def HexToBin(hex):
    binary = binascii.a2b_hex(hex)
    return binary

FPS = 10
ENC = 'utf8'
PORT = 45812
#HOSTNAME = "localhost".encode(ENC)
HOSTNAME = "0.0.0.0".encode(ENC)
SERVICE_TIMEOUT = 500
CHANNEL = 0
PEER_ID = BinToHex(os.urandom(4))

MSG_TYPE_LOGIN_FAILED = 0x01
MSG_TYPE_MAP_DATA = 0x02

TERRAIN = [
        { 'name': 'wall', 'pass': False },
        { 'name': 'grass', },
        { 'name': 'sand', },
        ]

class GameState:
    def __init__(self):
        self.startTime = time.time()
    def frame(self):
        elapsed = time.time() - self.startTime
        return round(elapsed * FPS)
    def timestamp(self):
        return round((time.time() - self.startTime) * 1000)

gs = GameState()

def generateMap(terrain, size):
    w, h = size
    top = [ 1 for i in range(w) ]
    rows = [ [1]
        + [ random.randrange(2, len(terrain)) for i in range(w-2) ]
        + [1] for i in range(h-2) ]
    return [top] + rows + [top]

theMap = generateMap(TERRAIN, (25, 25))

def prepTerrain(terrain):
    for i in range(len(terrain)):
        t = terrain[i]
        t['id'] = i
        if not 'pass' in t:
            t['pass'] = True

prepTerrain(TERRAIN)

def getMapRect(m, x, y, w, h):
    mr = []
    for r in range(y, y+h):
        if r < 0 or r >= len(m):
            mr.append([ 0 for i in range(w) ])
        else:
            row = []
            mr.append(row)
            for c in range(x, x+w):
                if c < 0 or c >= len(m[r]):
                    col = 0
                else:
                    col = m[r][c]
                row.append(col)
    return mr

def sendMessageToPeer(peer, message):
    print("SENDING:", message)
    if isinstance(message, str):
        encMsg = message.encode(ENC)
    else:
        encMsg = message
    peer.send(CHANNEL, Packet(encMsg))

def server():
    addr = Address(HOSTNAME, PORT)
    host = Host(addr, 99, 99)
    print("Listening: %s:%d" % (addr.host, addr.port))
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
        args = message.split()[1:]
        connectNewUser(event.peer, args)
        #sendMessageToPeer(event.peer, "HELLO "+username)
    elif message.startswith("MOVETO "):
        pass
    else:
        print("Unknown message type, nothing to do.")

def connectNewUser(peer, msg):
    clientId = username = password = None
    if len(msg)==1:
        username, = msg
    elif len(msg)==2:
        username, password = msg
    elif len(msg)==3:
        clientId, username, password = msg
    print("CONNECT: [%s] %s:%s" % (clientId, username, password))
    user = auth(username, password)
    if not user:
        sendMessageToPeer(peer, bytes([MSG_TYPE_LOGIN_FAILED]))
    else:
        px, py = user.position
        header = [
            MSG_TYPE_MAP_DATA,
            gs.frame() % (1 << 16),
            gs.timestamp() % (1 << 16),
            random.randrange(0x10000),
            user.status, 0, user.mapRegionID, user.mapSectorID,
            px-7, py-7, px, py,
        ]
        mapData = user.getMapData()
        mapNumbers = [ x for y in mapData for x in y ]
        print(mapNumbers)
        mapBytes = bytes(mapNumbers)
        extraData = []
        headerFormat = "!BBHHBBHHBBBB"
        print(headerFormat, header)
        reply = struct.pack(headerFormat, *header)
        reply += mapBytes
        reply += bytes(extraData)
        sendMessageToPeer(peer, reply)

def auth(username, password):
    # TODO: Verify username and password for real.
    if username == "gooduser":
        return User(username)
    return False

class User:

    def __init__(self, username):
        self.username = username
        self.status = 1
        self.mapRegionID = 1
        self.mapSectorID = 1
        self.position = [ random.randrange(3, 20) for i in range(2) ]

    def getMapData(self):
        x, y = self.position
        return getMapRect(theMap, x-7, y-7, 15, 15)

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
    elif isinstance(x, Host):
        return str(x)
    else:
        return str(x)

if __name__ == '__main__':
    try:
        server()
    except KeyboardInterrupt:
        print(" Interrupted.")

