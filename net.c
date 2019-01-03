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
  NET_MSGTYPE_MAP,
};

__attribute__((packed))
struct NetMessageMap {
  uint8_t type;  // NetMessageType
  uint8_t frame; // frame number mod 2^8
  uint16_t timestamp; // time mod 2^16
  uint16_t messageId; // random 16-bit ID
  uint8_t status;
  uint8_t unused1; // XXX: use this byte if possible
  uint8_t offsetX;
  uint8_t offsetY;
  uint8_t tiles[289]; // 17 * 17
  uint8_t extra[213]; // XXX: use this for item/char/player info
};

typedef struct NetMessageMap NetMessageMap;

//_Static_assert(sizeof(struct NetMessageMap) > NET_MESSAGE_MAX_SIZE,
//    "Network message struct is larger than the queue items allocated for it.");

//unsigned char _netMessageQueue[NET_MESSAGE_QUEUE_SIZE][NET_MESSAGE_MAX_SIZE];

struct {
  int len;
  NetMessageMap msg;
} _lastMapUpdate = {0};

static ENetHost* _enetClient;
static ENetPeer* _enetServer;
static enum NetState _netState = NET_STATE_INIT;

void CloseNet() {
  enet_host_destroy(_enetClient);
}

void InitNet(Environment* env) {
  assert(sizeof(NetMessageMap) == NET_MESSAGE_MAX_SIZE);
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
      msg, msgLen > 0 ? (unsigned)msgLen : 1 + strlen(msg), reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
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
  va_start(args, fmt);
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
  ENetPacket* pkt = event->packet;
  if (pkt->dataLength == 0)
    return;
  uint8_t msgType = pkt->data[0];
  switch (msgType) {
    case NET_MSGTYPE_LOGIN_FAILED:
      // FIXME: Don't die, ask user to change login.
      die("Unable to log in, server rejected credentials");
      break;
    case NET_MSGTYPE_MAP:
      {
        if (pkt->dataLength > sizeof(NetMessageMap))
          die("Network map message too large.");
        _lastMapUpdate.len = pkt->dataLength;
        memcpy(&_lastMapUpdate.msg, pkt->data, pkt->dataLength);
        //char* messageCopy = CopyPacketMessage(pkt);
        //RaiseEvent(EVT_NET_MESSAGE, pkt->dataLength, messageCopy);
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

