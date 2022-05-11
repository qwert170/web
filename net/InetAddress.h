
#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <netinet/in.h>
#include <string>
namespace web
{
using std::string;
namespace net
{
namespace sockets
{
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
}

class InetAddress 
{
 public:

  explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);
  InetAddress(string ip, uint16_t port);
  explicit InetAddress(const struct sockaddr_in& addr): addr_(addr){}

  sa_family_t family() const { return addr_.sin_family; }
  string toIp() const;
  string toIpPort() const;
  uint16_t port() const;

  const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr_); }
  void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }

  uint32_t ipv4NetEndian() const;
  uint16_t portNetEndian() const { return addr_.sin_port; }

  static bool resolve(string hostname, InetAddress* result);

 private:
  struct sockaddr_in addr_;
};

}  // namespace net
}  

#endif
