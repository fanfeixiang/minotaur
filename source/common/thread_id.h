#ifndef MINOTAUR_COMMON_THREAD_ID_H_
#define MINOTAUR_COMMON_THREAD_ID_H_
/**
 * @file thread_id.h
 * @author Wolfhead
 */
#include <stdint.h>
#include <sys/syscall.h>

namespace minotaur {

class ThreadId {
 public:
  static uint32_t Get() {
#ifdef SYS_gettid
    static thread_local uint32_t tid = syscall(SYS_gettid);
    return tid;
#else
  #error "SYS_gettid unavailable on this system"
#endif  
  }
};

} //namespace minotaur

#endif // MINOTAUR_COMMON_THREAD_ID_H_
