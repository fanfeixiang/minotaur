/**
 * @file service_channel.cpp
 * @author Wolfhead
 */
#include "service_channel.h"
#include "io_message.h"
#include "../io_service.h"
#include "../stage.h"
#include "../service/service_handler.h"

namespace minotaur {

LOGGER_CLASS_IMPL(logger, ServiceChannel);

ServiceChannel::ServiceChannel(IOService* io_service, int fd, Service* service) 
    : Channel(io_service, fd)
    , service_(service) {
}

void ServiceChannel::OnDecodeMessage(ProtocolMessage* message) {
  message->status = ProtocolMessage::kStatusOK;
  message->direction = ProtocolMessage::kIncomingRequest;
  message->handler_id = Handler::kUnspecifiedId;
  message->descriptor_id = GetDescriptorId();
  message->payload.service = GetService();

  if (!GetIOService()->GetServiceStage()->Send(message)) {
    MI_LOG_WARN(logger, "Channel::DecodeMessage Send message fail");
    MessageFactory::Destroy(message);
  }
}


} //namespace minotaur
