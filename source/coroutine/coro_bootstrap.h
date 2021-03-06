#ifndef _MINOTAUR_CORO_BOOTSTRAP_H_
#define _MINOTAUR_CORO_BOOTSTRAP_H_
/**
 * @file coro_bootstrap.h
 * @author Wolfhead
 */
#include "coroutine.h"

namespace minotaur {

class CoroBootstrap : public Coroutine {
 public:
  void Start(Coroutine* coro) {
    coro_create(this, NULL, NULL, NULL, 0);
    Transfer(coro);
  }
};

} //namespace minotaur

#endif //_MINOTAUR_CORO_BOOTSTRAP_H_
