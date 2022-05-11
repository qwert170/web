#include "../net/InetAddress.h"
#include "../base/Logging.h"
#include "../net/SocketsOps.h"

#include <netdb.h>
#include <netinet/in.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

using namespace web;
using namespace web::net;
using std::string;

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in),"InetAddress is same size as sockaddr_in");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");

InetAddress::InetAddress(uint16_t portArg, bool loopbackOnly)
{
  static_assert(offsetof(InetAddress, addr_) == 0, "addr_ offset 0");
  memZero(&addr_, sizeof addr_);
  addr_.sin_family = AF_INET;
  in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
  addr_.sin_addr.s_addr = htobe32(ip);
  addr_.sin_port = htobe16(portArg);
}

InetAddress::InetAddress(string ip, uint16_t portArg)
{
  
  memZero(&addr_, sizeof addr_);
  sockets::fromIpPort(ip.c_str(), portArg, &addr_);
}

string InetAddress::toIpPort() const
{
  char buf[64] = "";
  sockets::toIpPort(buf, sizeof buf, getSockAddr());
  return buf;
}

string InetAddress::toIp() const
{
  char buf[64] = "";
  sockets::toIp(buf, sizeof buf, getSockAddr());
  return buf;
}

uint32_t InetAddress::ipv4NetEndian() const
{
  assert(family() == AF_INET);
  return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::port() const
{
  return be64toh(portNetEndian());
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(string hostname, InetAddress* out)
{
  assert(out != NULL);
  struct hostent hent;
  struct hostent* he = NULL;
  int herrno = 0;
  memZero(&hent, sizeof(hent));

  int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
  if (ret == 0 && he != NULL)
  {
    assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
    out->addr_.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
    return true;
  }
  else
  {
    if (ret)
    {
      LOG_SYSERR << "InetAddress::resolve";
    }
    return false;
  }
}


