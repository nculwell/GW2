// vim: nu et ts=2 sts=2 sw=2

void PollEvents(Environment* env) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
      case SDL_AUDIODEVICEADDED:
      case SDL_AUDIODEVICEREMOVED:
        //HandleEvent_AudioDevice(&e.adevice);
        break;
      case SDL_CONTROLLERAXISMOTION:
        //HandleEvent_ControllerAxis(&e.caxis);
        break;
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        //HandleEvent_ControllerButton(&e.cbutton);
        break;
      case SDL_CONTROLLERDEVICEADDED:
      case SDL_CONTROLLERDEVICEREMOVED:
      case SDL_CONTROLLERDEVICEREMAPPED:
        //HandleEvent_ControllerDevice(&e.cdevice);
        break;
      case SDL_DOLLARGESTURE:
      case SDL_DOLLARRECORD:
        //HandleEvent_DollarGesture(&e.dgesture);
        break;
      case SDL_DROPFILE:
      case SDL_DROPTEXT:
      case SDL_DROPBEGIN:
      case SDL_DROPCOMPLETE:
        //HandleEvent_Drop(&e.drop);
        break;
      case SDL_FINGERMOTION:
      case SDL_FINGERDOWN:
      case SDL_FINGERUP:
        //HandleEvent_TouchFinger(&e.tfinger);
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        HandleEvent_Keyboard(&e.key);
        break;
      case SDL_JOYAXISMOTION:
        //HandleEvent_JoyAxis(&e.jaxis);
        break;
      case SDL_JOYBALLMOTION:
        //HandleEvent_JoyBall(&e.jball);
        break;
      case SDL_JOYHATMOTION:
        //HandleEvent_JoyHat(&e.jhat);
        break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        //HandleEvent_JoyButton(&e.jbutton);
        break;
      case SDL_JOYDEVICEADDED:
      case SDL_JOYDEVICEREMOVED:
        //HandleEvent_JoyDevice(&e.jdevice);
        break;
      case SDL_MOUSEMOTION:
        //HandleEvent_MouseMotion(&e.motion);
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        //HandleEvent_MouseButton(&e.button);
        break;
      case SDL_MOUSEWHEEL:
        //HandleEvent_MouseWheel(&e.wheel);
        break;
      case SDL_MULTIGESTURE:
        //HandleEvent_MultiGesture(&e.mgesture);
        break;
      case SDL_QUIT:
        //HandleEvent_Quit(&e.quit);
        break;
      case SDL_SYSWMEVENT:
        //HandleEvent_SysWM(&e.syswm);
        break;
      case SDL_TEXTEDITING:
        //HandleEvent_TextEditing(&e.edit);
        break;
      case SDL_TEXTINPUT:
        //HandleEvent_TextInput(&e.text);
        break;
      case SDL_USEREVENT:
        //HandleEvent_User(&e.user);
        break;
      case SDL_WINDOWEVENT:
        //HandleEvent_Window(&e.window);
        break;
    }
  }
}
