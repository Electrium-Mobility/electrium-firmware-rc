#ifndef PAIR_H
#define PAIR_H
#include "settings.h"

#ifdef CONTROLLER
  void pairController();
#elif defined(REMOTE)
  void pairRemote();
#endif

#endif