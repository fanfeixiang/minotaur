#ifndef _MINOTAUR_EVENT_LOOP_KQUEUE_H_
#define _MINOTAUR_EVENT_LOOP_KQUEUE_H_
/**
  @file event_loop_kqueue.h
  @author Wolfhead
*/

namespace minotaur {
namespace event {

struct EventLoopData;
class EventLoopKqueue {
 public:
  static int Init(EventLoopData* el_data);
  static int Destroy(EventLoopData* el_data);
  static int AddEvent(EventLoopData* el_data, int fd, int mask);
  static int RemoveEvent(EventLoopData* el_data, int fd, int mask);
  static int Poll(EventLoopData* el_data, int timeout);
  static const char* GetImplement();
};

typedef EventLoopKqueue EventLoopImpl;

} //namespace event
} //namespace minotaur

#endif // _MINOTAUR_EVENT_LOOP_KQUEUE_H_