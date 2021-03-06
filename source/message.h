#ifndef _MINOTAUR_MESSAGE_H_
#define _MINOTAUR_MESSAGE_H_
/**
 * @file message.h
 * @author Wolfhead
 */
#include <stdint.h>
#include <sstream>

namespace minotaur {

class ProtocolMessage;

class MessageType {
 public:
  enum {
    kUnknownEvent = 0,
    kIOEvent,
    kIOMessageEvent,
    kIOActiveCloseEvent,
    kLineMessage,
    kRapidMessage,
    kHttpMessage,
  };

  static const char* ToString(int type) {
    static const char* str[] = {
      "unknown event",
      "io event",
      "io message event",
      "io active close event",
      "line protocol message",
      "rapid protocol message",
      "Http protocol message",
    };

    if (type < 0 || type >= (int)(sizeof(str) / sizeof(const char*))) {
      return "unknown event";
    }

    return str[type];
  }
};

class MessageBase {
 public:
  MessageBase()
      : type_id(MessageType::kUnknownEvent) {
  }

  MessageBase(uint8_t type_id) 
      : type_id(type_id) {
  }
  virtual ~MessageBase() {};

  std::string ToString() const;
  virtual void Dump(std::ostream& os) const;


  uint8_t type_id;
};

class EventMessage : public MessageBase {
 public:
  EventMessage() 
      : MessageBase(MessageType::kUnknownEvent), descriptor_id(0) {
  }

  EventMessage(
      uint8_t type_id_, 
      uint64_t descriptor_id_, 
      uint64_t payload_ = 0) 
      : MessageBase(type_id_), 
      descriptor_id(descriptor_id_), 
      payload(payload_) {
  }

  void Destroy() const;
  virtual void Dump(std::ostream& os) const;

  uint64_t descriptor_id;
  uint64_t payload;

  ProtocolMessage* GetProtocolMessage() const {
    if (type_id == MessageType::kIOMessageEvent && payload) {
      return (ProtocolMessage*)payload;
    }
    return NULL;
  }
};

class MessageFactory {
 public:
  template<typename MessageType>
  static MessageType* Allocate() {
    return new MessageType();
  }

  template<typename MessageType, typename A1>
  static MessageType* Allocate(const A1& arg1) {
    return new MessageType(arg1);
  }

  template<typename MessageType, typename A1, typename A2>
  static MessageType* Allocate(const A1& arg1, const A2& arg2) {
    return new MessageType(arg1, arg2);
  }

  static void Destroy(MessageBase* message) {
    delete message;
  }
};

std::ostream& operator << (std::ostream& os, const EventMessage& message);

} //namespace minotaur

#endif // _MINOTAUR_MESSAGE_H_
