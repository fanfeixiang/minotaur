/**
 * @file rapid_protocol.cpp
 * @author Wolfhead
 */
#include "rapid_protocol.h"
#include <string.h>
#include "../../io_buffer.h"
#include "../../io_message.h"

namespace minotaur {

namespace {
struct RapidHeader {
  uint32_t size; // total package length in bytes
  uint32_t seqid;
  uint8_t  type; // 
  uint8_t  version;
  uint16_t cmdid;
  uint32_t extra;
};

} //namespace

RapidProtocol::RapidProtocol() 
    : Protocol(ProtocolType::kRapidProtocol, false) {
}

ProtocolMessage* RapidProtocol::Decode(
      IODescriptor* descriptor, 
      IOBuffer* buffer, 
      int* result) {
  if (buffer->GetReadSize() < sizeof(RapidHeader)) {
    *result = Protocol::kResultContinue;
    return NULL;
  }

  RapidHeader* header = (RapidHeader*)buffer->GetRead();
  if (buffer->GetReadSize() < header->size) {
    *result = Protocol::kResultContinue;
    return NULL;
  }

  RapidMessage* message = MessageFactory::Allocate<RapidMessage>();

  message->sequence_id = header->seqid;
  message->cmd_id = header->cmdid;
  message->extra = header->extra;
  message->body.assign(
      buffer->GetRead() + sizeof(RapidHeader), 
      header->size - sizeof(RapidHeader));
  
  buffer->Consume(header->size);
  *result = Protocol::kResultDecoded;
  return message;
}

bool RapidProtocol::Encode(
      IODescriptor* descriptor,
      IOBuffer* buffer,
      ProtocolMessage* message) {
  RapidMessage* rapid_message = 
    static_cast<RapidMessage*>(message);

  uint32_t package_size = 
      sizeof(RapidHeader) + rapid_message->body.size();

  RapidHeader header = {
    .size = package_size,
    .seqid = rapid_message->sequence_id,
    .type = RapidMessage::kDataType,
    .version = 0,
    .cmdid = rapid_message->cmd_id,
    .extra = rapid_message->extra,
  };

  buffer->Write((const char*)&header, sizeof(RapidHeader));
  buffer->Write(rapid_message->body.data(), rapid_message->body.size());
  return true;
}

} //namespace minotaur
