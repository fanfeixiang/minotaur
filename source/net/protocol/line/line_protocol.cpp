/**
 * @file line_protocol.cpp
 * @author Wolfhead
 */
#include "line_protocol.h"
#include <string.h>
#include "../../io_buffer.h"
#include "../../io_message.h"

namespace minotaur {

LOGGER_CLASS_IMPL_NAME(logger, LineProtocol, "net.LineProtocol");

LineProtocol::LineProtocol() 
    : Protocol(ProtocolType::kLineProtocol, true) {
}

ProtocolMessage* LineProtocol::Decode(
      IODescriptor* descriptor, 
      IOBuffer* buffer, 
      int* result) {

  buffer->EnsureCStyle();
  char* begin = buffer->GetRead();
  char* end = strchr(begin, '\n');

  if (!end) {
    *result = Protocol::kResultContinue;
    return NULL;
  }

  buffer->Consume(end - begin + 1);
  *result = Protocol::kResultDecoded;
  return MessageFactory::Allocate<LineMessage>(
      std::string(begin, end - begin - 1));
}

bool LineProtocol::Encode(
      IODescriptor* descriptor,
      IOBuffer* buffer,
      ProtocolMessage* message) {
  LineMessage* line_message = static_cast<LineMessage*>(message);
  const std::string& body = line_message->body;

  char* begin = buffer->EnsureWrite(body.size() + 1);

  memcpy(begin, body.data(), body.size());
  *(begin + body.size()) = '\n';

  buffer->Produce(body.size() + 1);
  return true;
}



} //namespace minotaur
