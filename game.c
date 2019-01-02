// vim: nu et ts=8 sts=2 sw=2

typedef struct GameState {
  bool isPaused;
  Map* map;
} GameState;

// Game state setup.
void load(Environment* env, GameState* gs) {
  gs->map = Map_Load();
}

void update(GameState* gs) {
  if (gs->isPaused)
    return;
  // Scan for player input.
  // Send input to server.
  // Dequeue next incoming frame from server buffer.
  // Implement next incoming frame.
}

