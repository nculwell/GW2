// vim: nu et ts=8 sts=2 sw=2

#include <enet/enet.h>

static ENetHost* enetClient;

void InitNet() {
  if (0 != enet_initialize())
    die("Failed enet_initialize");
  atexit(enet_deinitialize);
  enetClient = enet_host_create(
      NULL, // NULL=client
      1, // connections
      2, // channels
      0, // bandwidth
      0); // bandwidth
  if (!enetClient)
    die("Failed to create ENet client");
}

void DeinitNet() {
  enet_host_destroy(enetClient);
}

void PollNet() {
  ENetEvent event;
  while (enet_host_service(enetClient, &event, 0) > 0) {
    switch (event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
        printf("Connected to server: %X port %u.\n", 
            event.peer->address.host, event.peer->address.port);
        // Store any relevant client information here.
        // event.peer->data = "Client information";
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        printf("A packet of length %u containing %s was received from %s on channel %u.\n",
            event.packet->dataLength, event.packet->data, event.peer->data,
            event.channelID);
        enet_packet_destroy(event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("%s disconnected.\n", event.peer->data);
        /* Reset the peer's client information. */
        event.peer->data = NULL;
    }
  }
}

