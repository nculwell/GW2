// vim: nu et ts=8 sts=2 sw=2

#include <enet/enet.h>

#define USERNAME "ncmadcity"

#define NET_PORT 45812
#define NET_CONNECTIONS_TO_SUPPORT 2
#define NET_CHANNELS_TO_SUPPORT 2

#define NET_MESSAGE_QUEUE_SIZE (2 * LOGICAL_FRAMES_PER_SECOND)
#define NET_MESSAGE_MAX_SIZE 512

enum NetState {
  NET_STATE_INIT,
  NET_STATE_BROADCASTING,
  NET_STATE_CONNECTING,
  NET_STATE_LOGGING_IN,
  NET_STATE_LOADING,
  NET_STATE_PLAYING,
};

enum NetMessageType {
  NET_MSGTYPE_LOGIN_FAILED,
  NET_MSGTYPE_
};

__attribute__((packed))
struct NetMessageLayout {
  uint8_t type;  // NetMessageType
  uint8_t frame; // frame number mod 2^8
  uint16_t timestamp; // time mod 2^16
  uint16_t messageId; // random 16-bit ID
  uint8_t status;
  uint8_t unused1;
  union {
    struct {
      uint8_t offsetX;
      uint8_t offsetY;
      uint8_t tiles[17*17];
    } map;
  };
};

typedef struct NetMessageLayout NetMessageLayout;

_Static_assert(sizeof(struct NetMessageLayout) > NET_MESSAGE_MAX_SIZE,
    "Network message struct is larger than the queue items allocated for it.");

unsigned char _netMessageQueue[NET_MESSAGE_QUEUE_SIZE][NET_MESSAGE_MAX_SIZE];

static ENetHost* _enetClient;
static ENetPeer* _enetServer;
static enum NetState _netState = NET_STATE_INIT;

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
  ENetAddress address = { .host = ENET_HOST_BROADCAST, .port = NET_PORT };
  _enetServer = enet_host_connect(_enetClient, &address,
      NET_CHANNELS_TO_SUPPORT, 0);
  _netState = NET_STATE_CONNECTING;
  atexit(CloseNet);
}

void SendNetMessage(ENetPeer* peer, int msgLen, const char* msg, bool reliable) {
  ENetPacket* packet = enet_packet_create(
      msg, msgLen > 0 ? msgLen : 1 + strlen(msg), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
  // Send the packet to the peer over channel id 0.
  // One could also broadcast the packet by
  // enet_host_broadcast(host, 0, packet);
  enet_peer_send(peer, 0, packet);
  enet_host_flush(_enetClient);
}

void SendNetMessageFmt(ENetPeer* peer, bool reliable, const char* fmt, ...) {
#define MSG_FMT_BUF_SIZE 256
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
    case NET_STATE_LOGGING_IN:
      if (MsgEq(packet, "WELCOME")) {
        SendNetMessageFmt(event->peer, true, "LOAD");
        _netState = NET_STATE_LOADING;
      }
      else {
        die("Unable to log in, server rejected credentials");
      }
      break;
    case NET_STATE_LOADING:
    case NET_STATE_PLAYING:
      {
        char* messageCopy = CopyPacketMessage(packet);
        //RaiseEvent(EVT_NET_MESSAGE, packet->dataLength, messageCopy);
        // TODO: Parse event.
      }
      break;
    default:
      die("Invalid NetState for message: %d", _netState);
  }
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
        SendNetMessageFmt(event.peer, true, "CONNECT %s", USERNAME);
        _netState = NET_STATE_LOGGING_IN;
        // Store any relevant client information here.
        // event.peer->data = "Client information";
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        ReceiveNetMessage(&event);
        enet_packet_destroy(event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("Disconnected.\n");
        // Reset the peer's client information.
        // event.peer->data = NULL;
        break;
      case ENET_EVENT_TYPE_NONE:
        // ignore
        break;
    }
  }
}

