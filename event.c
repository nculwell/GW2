// vim: nu et ts=8 sts=2 sw=2

enum EventType {
  EVT_NET_MESSAGE,
};

typedef struct EventNetMessage {
  EventType type;
  uint32_t time;
  size_t size;
  char* data;
}

typedef struct Event {
  EventType type;
  size_t size;
  void* data;
} Event;

void RaiseEvent(EventType type, 
