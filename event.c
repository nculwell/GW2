// vim: nu et ts=8 sts=2 sw=2

typedef enum EventType {
  EVT_NET_MESSAGE = 0x0001,
  EVT_TYPE_COUNT = 1,
} EventType;

typedef struct EventNetMessage {
  EventType type;
  uint32_t time;
  size_t size;
  char* data;
} EventNetMessage;

typedef struct Event {
  union {
    EventType type;
    struct {
      EventType type;
      uint32_t time;
    } common;
    EventNetMessage net;
  };
} Event;

typedef struct Mailbox {
} Mailbox;

typedef void EventListener(Event* event);

#define EVENT_LISTENER_REGISTRY_SIZE 1024

struct EventListenerRegistry {
  EventType type;
  EventListener* listener;
};

static struct EventListenerRegistry _eventListenerRegistry[EVENT_LISTENER_REGISTRY_SIZE];
static int _eventListenerCount = 0;

static Event _eventQueue[EVENT_QUEUE_SIZE];
static int _eventQueueHead = 0;
static int _eventQueueTail = 0;

static int AdvanceQueuePtr(int ptr) {
  if (ptr == EVENT_QUEUE_SIZE - 1)
    return 0;
  else
    return ptr + 1;
}

// XXX: Will need to lock on queue if doing threading.
// TODO: Filter events so that each thread processes the appropriate events.

void ProcessEvent(Event* event) {
  for (int i=0; i < _eventListenerCount; ++i) {
    if (event->type & _eventListenerRegistry[i].type) {
      _eventListenerRegistry[i].listener(event);
    }
  }
  // TODO: Destroy event.
}

void RaiseEvent(EventType type, int len, void* data) {
  _eventQueueTail = AdvanceQueuePtr(_eventQueueTail);
  if (_eventQueueTail == _eventQueueHead)
    die("Event queue full");
  _eventQueue[_eventQueueTail] = (struct Event){ type, len, data };
}

void ProcessEvents() {
  while (_eventQueueHead != _eventQueueTail) {
    ProcessEvent(&_eventQueue[_eventQueueHead]);
    _eventQueueHead = AdvanceQueuePtr(_eventQueueHead);
  }
}

void RegisterEventListener(EventType type, EventListener* listener) {
  _eventListenerRegistry[_eventListenerCount] =
    (struct EventListenerRegistry){ type, listener };
  ++_eventListenerCount;
}

