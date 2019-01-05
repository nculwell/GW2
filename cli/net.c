// vim: nu et ts=8 sts=2 sw=2

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __MSWIN__
# include <netinet/in.h>
#else
# include <Winsock2.h>
#endif

#include <enet/enet.h>

#include "die.c"

#define USERNAME "gooduser"
#define SERVER_HOST "192.168.1.165"
//#define SERVER_HOST "tiamat"

#define NET_PORT 45812
#define NET_CONNECTIONS_TO_SUPPORT 2
#define NET_CHANNELS_TO_SUPPORT 2

#define NET_MESSAGE_QUEUE_SIZE (2 * LOGICAL_FRAMES_PER_SECOND)
#define NET_MESSAGE_MAX_SIZE 512

#define NET_MESSAGE_MAP_DIMENSION 17
#define NET_MESSAGE_MAP_DATA_SIZE \
  (NET_MESSAGE_MAP_DIMENSION * NET_MESSAGE_MAP_DIMENSION)

enum NetState {
  NET_STATE_INIT,
  NET_STATE_BROADCASTING,
  NET_STATE_CONNECTING,
  NET_STATE_LOGGING_IN,
  NET_STATE_LOADING,
  NET_STATE_PLAYING,
};

enum NetMessageType {
  NET_MSGTYPE_LOGIN_FAILED = 0x01,
  NET_MSGTYPE_MAP = 0x02,
};

__attribute__((packed))
struct NetMessageMap {
  uint8_t type;  // NetMessageType
  uint8_t padding;
  uint16_t frame; // frame number mod 2^8
  uint16_t timestamp; // time mod 2^16
  uint16_t messageId; // random 16-bit ID
  uint16_t status;
  uint16_t mapRegionID;
  uint16_t mapSectorID;
  uint16_t playerX;
  uint16_t playerY;
  uint8_t tiles[NET_MESSAGE_MAP_DATA_SIZE];
  uint8_t extra[205]; // XXX: use this for item/char/player info
};

// Number of header fields in the struct above.
// We use this to flip the byte order when they're read from the network.
#define NET_MESSAGE_MAP_HEADER_FIELD_COUNT 9

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

static void PrintMapMessage(NetMessageMap* msg) {
  printf("MAP MESSAGE: { type=%u, frame=%u, timestamp=%u, messageId=%04X, status=%u;\n",
      msg->type, msg->frame, msg->timestamp, msg->messageId, msg->status);
  for (int r=0; r < NET_MESSAGE_MAP_DIMENSION; ++r) {
    for (int c=0; c < NET_MESSAGE_MAP_DIMENSION; ++c) {
      printf(" %02X", msg->tiles[r * NET_MESSAGE_MAP_DIMENSION + c]);
    }
  }
  printf("\n}\n");
}

void CloseNet() {
  enet_host_destroy(_enetClient);
}

void PrintAddress(ENetAddress* address) {
  printf("%d.%d.%d.%d:%d"
      , (address->host >> 0x00) & 0xFF
      , (address->host >> 0x08) & 0xFF
      , (address->host >> 0x10) & 0xFF
      , (address->host >> 0x18) & 0xFF
      , address->port
      );
}

void NetConnect() {
  // Broadcast connect request across the network.
  ENetAddress address = { .host = ENET_HOST_BROADCAST, .port = NET_PORT };
  enet_address_set_host(&address, SERVER_HOST);
  printf("Connecting to host: "); PrintAddress(&address); printf("\n");
  _enetServer = enet_host_connect(_enetClient, &address,
      NET_CHANNELS_TO_SUPPORT, 0);
  _netState = NET_STATE_CONNECTING;
}

void InitNet() {
  assert(sizeof(NetMessageMap) == NET_MESSAGE_MAX_SIZE);
  if (0 != enet_initialize())
    die("Failed enet_initialize");
  atexit(enet_deinitialize);
  _enetClient = enet_host_create(NULL /* NULL=client */,
      NET_CONNECTIONS_TO_SUPPORT, NET_CHANNELS_TO_SUPPORT, 0, 0);
  if (!_enetClient)
    die("Failed to create ENet client");
  atexit(CloseNet);
  NetConnect();
}

static void SendNetMessage(
    ENetPeer* peer, int msgLen, const char* msg, bool reliable)
{
  ENetPacket* packet = enet_packet_create(msg,
      msgLen > 0 ? (unsigned)msgLen : 1 + strlen(msg),
      reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
  // Send the packet to the peer over channel id 0.
  // One could also broadcast the packet by
  // enet_host_broadcast(host, 0, packet);
  enet_peer_send(peer, 0, packet);
  enet_host_flush(_enetClient);
}

static void SendNetMessageFmt(ENetPeer* peer, bool reliable, const char* fmt, ...) {
#define MSG_FMT_BUF_SIZE 256
  char msg[MSG_FMT_BUF_SIZE];
  va_list args;
  va_start(args, fmt);
  int msgLen = vsnprintf(msg, MSG_FMT_BUF_SIZE, fmt, args);
  va_end(args);
  SendNetMessage(peer, msgLen, msg, reliable);
}

/*
static char* CopyPacketMessage(ENetPacket* packet) {
  // TODO: Save packet data in a ring buffer instead of using malloc.
  char* messageCopy = Alloc(packet->dataLength);
  memcpy(messageCopy, packet->data, packet->dataLength);
  return messageCopy;
}
*/

// Return true if the packet frame is greater than the previously saved frame,
// or if it's less and the numbers are close enough to the ends of the range to
// make it look like the frame count just wrapped around.
static bool IsPacketFrameLater(unsigned packetFrame, unsigned savedFrame) {
#define FRAME_OVERFLOW_THRESHOLD 0x20
  return packetFrame > savedFrame
    || (packetFrame < FRAME_OVERFLOW_THRESHOLD
        && savedFrame > 0x100 - FRAME_OVERFLOW_THRESHOLD);
}

static void ReceiveNetMessageMap(ENetPacket* pkt) {
  NetMessageMap* pktMap = (NetMessageMap*)&pkt->data;
  if (IsPacketFrameLater(pktMap->frame, _lastMapUpdate.msg.frame)) {
    // Copy packet data into _lastMapUpdate.
    _lastMapUpdate.len = pkt->dataLength;
    _lastMapUpdate.msg = *pktMap;
    // Flip multibyte ints to host order.
    uint16_t* header = (uint16_t*)&_lastMapUpdate.msg;
    for (int i=1; i < 9; ++i) {
      header[i] = ntohs(header[i]);
    }
    PrintMapMessage(&_lastMapUpdate.msg);
    //RaiseEvent(EVT_NET_MESSAGE, pkt->dataLength, messageCopy);
    // TODO: Parse the 'extra' part of the event.
  }
}

static void ReceiveNetMessage(ENetPacket* pkt) {
  if (pkt->dataLength == 0)
    return;
  uint16_t msgType = pkt->data[0];
  switch (msgType) {
    case NET_MSGTYPE_LOGIN_FAILED:
      // FIXME: Don't die, ask user to change login.
      die("Unable to log in, server rejected credentials");
      break;
    case NET_MSGTYPE_MAP:
      if (pkt->dataLength > sizeof(NetMessageMap))
        die("Network map message too large.");
      ReceiveNetMessageMap(pkt);
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
          printf("peer = _enetServer\n");
        // TODO: See if 'reliable' really works for this.
        SendNetMessageFmt(event.peer, true, "CONNECT %s", USERNAME);
        _netState = NET_STATE_LOGGING_IN;
        // Store any relevant client information here.
        // event.peer->data = "Client information";
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        ReceiveNetMessage(event.packet);
        enet_packet_destroy(event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("Disconnected.\n");
        // Reset the peer's client information.
        // event.peer->data = NULL;
        NetConnect();
        break;
      case ENET_EVENT_TYPE_NONE:
        // ignore
        break;
    }
  }
}

#ifdef NET_TEST
int main(int argc, char** argv) {
  InitNet();
  printf("Testing network code...\n");
  for (;;) {
    PollNet();
    fflush(stdout);
  }
  return 0;
}
#endif

