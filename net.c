// vim: nu et ts=8 sts=2 sw=2

#include <enet/enet.h>

#define USERNAME "ncmadcity"

#define NET_CONNECTIONS_TO_SUPPORT 2
#define NET_CHANNELS_TO_SUPPORT 2

enum NetState {
  NETSTATE_INIT,
  NETSTATE_BROADCASTING,
  NETSTATE_CONNECTING,
  NETSTATE_LOGGING_IN,
  NETSTATE_LOADING,
  NETSTATE_PLAYING,
};

typedef struct NetMessageHeader {
  uint16_t timestamp; // time mod 2^16
  uint16_t messageId;
  uint8_t status;
  uint8_t frame;
  uint8_t mapW, mapH;
  uint8_t map[0];
} NetMessageHeader;

static ENetHost* _enetClient;
static ENetPeer* _enetServer;
static enum NetState _netState = NETSTATE_INIT;

void CloseNet() {
  enet_host_destroy(_enetClient);
}

void InitNet() {
  if (0 != enet_initialize())
    die("Failed enet_initialize");
  atexit(enet_deinitialize);
  _enetClient = enet_host_create(NULL /* NULL=client */,
      NET_CONNECTIONS_TO_SUPPORT, NET_CHANNELS_TO_SUPPORT, 0, 0);
  if (!_enetClient)
    die("Failed to create ENet client");
  // Broadcast connect request across the network.
  _enetServer = enet_host_connect(_enetClient, ENET_HOST_BROADCAST,
      NET_CHANNELS_TO_REQUEST, enet_uint32 data)
  _netState = NETSTATE_CONNECTING;
  atexit(CloseNet);
}

// _enetServer = enet_host_connect(_enetClient, ENET_HOST_BROADCAST, NET_CHANNELS_TO_REQUEST, enet_uint32 data)

void SendNetMessage(ENetPeer* peer, int msgLen, const char* msg, bool reliable) {
  ENetPacket* packet = enet_packet_create(
      msg, msgLen > 0 ? msgLen : 1 + strlen(msg), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
  // Send the packet to the peer over channel id 0.
  // One could also broadcast the packet by
  // enet_host_broadcast(host, 0, packet);
  enet_peer_send(peer, 0, packet);
  enet_host_flush(host);
}

void SendNetMessageFmt(ENetPeer* peer, bool reliable, const char* fmt, ...) {
#define MSG_FMT_BUF_SIZE
  char msg[MSG_FMT_BUF_SIZE];
  va_list args;
  va_start(args, format);
  int msgLen = vsnprintf(msg, MSG_FMT_BUF_SIZE, fmt, args);
  va_end(args);
  SendNetMessage(peer, msgLen, msg, reliable);
}

char* CopyPacketMessage(ENetPacket* packet) {
  // TODO: Save packet data in a ring buffer instead of using malloc.
  char* messageCopy = Alloc(packet->dataLength);
  memcpy(messageCopy, packet->data, packet->dataLength);
  return messageCopy;
}

void ReceiveNetMessage(ENetEvent* event) {
  switch (_netState) {
    case NETSTATE_LOGGING_IN:
      if (MsgEq(packet, "WELCOME")) {
        SendNetMessageFmt(event->peer, true, "LOAD");
        _netState = NETSTATE_LOADING;
      }
      else {
        die("Unable to log in, server rejected credentials");
      }
      break;
    case NETSTATE_LOADING:
    case NETSTATE_PLAYING:
      {
        char* messageCopy = CopyPacketMessage(packet);
        RaiseEvent(EVT_NET_MESSAGE, packet->dataLength, messageCopy);
      }
      break;
    default:
      die("Invalid NetState for message: %d", _netState);
  }
  enet_packet_destroy(event.packet);
}

void PollNet() {
  ENetEvent event;
  while (enet_host_service(_enetClient, &event, 0) > 0) {
    switch (event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
        printf("Connected to server: %X port %u.\n",
            event.peer->address.host, event.peer->address.port);
        if (_enetServer == event.peer)
          printf("peer = _enetServer");
        // TODO: See if 'reliable' really works for this.
        SendNetMessageFmt(event->peer, true, "CONNECT %s", USERNAME);
        _netState = NETSTATE_LOGGING_IN;
        // Store any relevant client information here.
        // event.peer->data = "Client information";
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        ReceiveNetMessage(&event);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("Disconnected.\n");
        // Reset the peer's client information.
        // event.peer->data = NULL;
        break;
    }
  }
}

