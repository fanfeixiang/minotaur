/**
 * @file acceptor.cpp
 * @author Wolfhead
 */
#include "acceptor.h"
#include "socket_op.h"
#include "service_channel.h"
#include "../io_service.h"
#include "io_descriptor_factory.h"
#include "protocol/protocol.h"

namespace minotaur {

LOGGER_CLASS_IMPL_NAME(logger, Acceptor, "net.Acceptor");

Acceptor::Acceptor(
    IOService* io_service,
    const std::string& host,
    int port,
    Service* service) 
    : Socket(io_service, false)
    , host_(host)
    , port_(port) 
    , service_(service) {
}

Acceptor::~Acceptor() {
}

int Acceptor::Start() {
  struct sockaddr_in sa;

  if (0 != SocketOperation::GetSocketAddress(GetHost(), GetPort(), &sa)) {
    MI_LOG_ERROR(logger, "Acceptor::Start GetSocketAddress failed"
        << ", host:" << GetHost()
        << ", port:" << GetPort());
    return -1;
  }

  int fd = SocketOperation::CreateTcpSocket();
  if (fd < 0) {
    MI_LOG_ERROR(logger, "Acceptor::Start CreateTcpSocket, failed with:" 
        << SystemError::FormatMessage());
    return -1;
  }

  if (0 != SocketOperation::SetReuseAddr(fd)) {
    MI_LOG_ERROR(logger, "Acceptor::Start SetReuseAddr, failed with:" 
        << SystemError::FormatMessage());
    close(fd);
    return -1;
  }

  if (0 != SocketOperation::SetNonBlocking(fd)) {
    MI_LOG_ERROR(logger, "Acceptor::Start SetNonBlocking, failed with:" 
        << SystemError::FormatMessage());
    close(fd);
    return -1;
  }

  if (0 != SocketOperation::Bind(fd, sa)) {
    MI_LOG_ERROR(logger, "Acceptor::Start bind, failed with:" 
        << SystemError::FormatMessage());
    close(fd);
    return -1;
  }
  
  if (0 != SocketOperation::Listen(fd)) {
    MI_LOG_ERROR(logger, "Acceptor::Start listen, failed with:"
        << SystemError::FormatMessage());
    close(fd);
    return -1;
  }

  SetFD(fd);

  if (0 != RegisterRead()) {
    MI_LOG_ERROR(logger, "Acceptor::Start RegisterRead fail");
    SetFD(-1);
    close(fd);
    return -1;
  }

  MI_LOG_TRACE(logger, "Acceptor::Start Regsitered:" << *this);

  return 0;
}

int Acceptor::Stop() {
  RegisterClose(); 
  return -1;
}

void Acceptor::OnRead() {
  struct sockaddr_in sa;

  while (true) {
    memset(&sa, 0, sizeof(sa));
    int client_fd = SocketOperation::Accept(GetFD(), &sa);
    if (client_fd < 0) {
      if (SocketOperation::WouldBlock(SystemError::Get())) {
        return;
      }
      MI_LOG_ERROR(logger, "Acceptor::OnRead accept failed with:"
          << SystemError::FormatMessage());  
      return;
    }

    char client_ip_buffer[32] = {0};
    int port = ntohs(sa.sin_port);
    inet_ntop(AF_INET, &sa.sin_addr, client_ip_buffer, INET_ADDRSTRLEN);

    ServiceChannel* channel = IODescriptorFactory::Instance()
        .CreateServiceChannel(GetIOService(), client_fd, GetService());
    if (!channel) {
      MI_LOG_ERROR(logger, "Acceptor::OnRead Create channel failed");
      continue;
    }
    channel->SetIp(client_ip_buffer);
    channel->SetPort(port);
    channel->SetProtocol(GetProtocol()->Clone());

    MI_LOG_TRACE(logger, "Acceptor::OnRead client connected on channel:"
        << channel->GetDiagnositicInfo());

    if (0 != channel->Start()) {
      MI_LOG_WARN(logger, "Acceptor::OnRead Channel Start fail");
      channel->Destroy();
    }
  }
}

void Acceptor::OnClose() {
  MI_LOG_INFO(logger, "Acceptor closed: " << host_ << ":" << port_);
  IODescriptor::OnClose();
}

} //namespace minotaur
