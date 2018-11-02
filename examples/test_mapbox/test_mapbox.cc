#if defined (_MSC_VER)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> //hostent
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#endif
#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WStringListModel.h>
#include <Wt/WTemplate.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTimer.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <thread>
#include <chrono>
#include "extensions/WLeaflet.hh"
using namespace Wt;

unsigned short port = 2000;

marker_icon_t marker_violet(
  "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-2x-violet.png",
  "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png",
  icon_size_t(25, 41),
  icon_size_t(12, 41),
  icon_size_t(1, -34),
  icon_size_t(41, 41));

///////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t
///////////////////////////////////////////////////////////////////////////////////////

class tcp_server_t
{
public:
  tcp_server_t() :
    m_socket_fd(-1)
  {
#if defined (_MSC_VER)
    WSADATA ws_data;
    if (WSAStartup(MAKEWORD(2, 0), &ws_data) != 0)
    {
    }
#endif
  };
  ~tcp_server_t()
  {
#if defined (_MSC_VER)
    WSACleanup();
#endif
  };
  int listen(const unsigned short server_port);
  int accept_client();
  int read_all(int socket_client_fd, void *_buf, int size_buf);
  void close_socket()
  {
#if defined (_MSC_VER)
    closesocket(m_socket_fd);
#else
    close(m_socket_fd);
#endif
  }
private:
  int m_socket_fd; // socket descriptor 
};

///////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::listen
///////////////////////////////////////////////////////////////////////////////////////

int tcp_server_t::listen(const unsigned short server_port)
{
  // local address
  sockaddr_in server_addr;

  // create TCP socket for incoming connections
  if ((m_socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    std::cout << "socket error: " << std::endl;
    return -1;
  }

  // construct local address structure
  memset(&server_addr, 0, sizeof(server_addr));     // zero out structure
  server_addr.sin_family = AF_INET;                 // internet address family
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // any incoming interface
  server_addr.sin_port = htons(server_port);        // local port

   // bind to the local address
  if (::bind(m_socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
  {
    //bind error: Permission denied
    //probably trying to bind a port under 1024. These ports usually require root privileges to be bound.
    std::cout << "bind error: " << std::endl;
    return -1;
  }

  // mark the socket so it will listen for incoming connections
  if (::listen(m_socket_fd, 10) < 0)
  {
    std::cout << "listen error: " << std::endl;
    return -1;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::accept_client
///////////////////////////////////////////////////////////////////////////////////////

int tcp_server_t::accept_client()
{
  int socket_client_fd; // socket descriptor for client
  sockaddr_in addr_client; // client address
#if defined (_MSC_VER)
  int len_addr; // length of client address data structure
#else
  socklen_t len_addr;
#endif

  // set length of client address structure (in-out parameter)
  len_addr = sizeof(addr_client);

  // wait for a client to connect
  if ((socket_client_fd = ::accept(m_socket_fd, (struct sockaddr *) &addr_client, &len_addr)) < 0)
  {
    std::cout << "accept error " << std::endl;
    return -1;
  }

  return socket_client_fd;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//tcp_server_t::read_all
//read SIZE_BUF bytes of data from SOCKET_CLIENT_FD into buffer BUF 
//return total size read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int tcp_server_t::read_all(int socket_client_fd, void *_buf, int size_buf)
{
  char *buf = static_cast<char *>(_buf); // can't do pointer arithmetic on void* 
  int recv_size; // size in bytes received or -1 on error 
  size_t size_left; // size in bytes left to send 
  const int flags = 0;
  int total_recv_size = 0;

  size_left = size_buf;

  while (size_left > 0)
  {
    //read the data, being careful of interrupted system calls and partial results
    do
    {
      recv_size = recv(socket_client_fd, buf, size_left, flags);
    } while (-1 == recv_size && EINTR == errno);

    if (-1 == recv_size)
    {
      std::cout << "recv error: " << strerror(errno) << std::endl;
    }

    //everything received, exit
    if (0 == recv_size)
    {
      break;
    }

    size_left -= recv_size;
    buf += recv_size;
    total_recv_size += recv_size;
  }

  return total_recv_size;
}

//run
//./leaflet_mapbox.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. -c wt_config.xml

///////////////////////////////////////////////////////////////////////////////////////
//MapApplication
//38.9072, -77.0369, 14 DC
//[37.0902, -95.7129], 5 US
///////////////////////////////////////////////////////////////////////////////////////

typedef WLeaflet WMap;

class MapApplication : public WApplication
{
public:
  MapApplication(const WEnvironment& env) :
    WApplication(env)
  {
    int test = 0;
    m_hbox = root()->setLayout(cpp14::make_unique<WVBoxLayout>());
    m_text = m_hbox->addWidget(cpp14::make_unique<WText>(Wt::asString(100)));
    m_leaflet = m_hbox->addWidget(cpp14::make_unique<WMap>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13));
    if (test)
    {
      m_leaflet->Circle(38.9072, -77.0369, 100, "#ff0000");
      m_leaflet->Marker(38.9072, -77.0369, "test", marker_violet);
    }

    enableUpdates(true);
    if (m_listen_thread.joinable())
    {
      m_listen_thread.join();
    }
    m_server.listen(port);
    m_listen_thread = std::thread(std::bind(&MapApplication::listen, this));
  }
  virtual ~MapApplication()
  {
    m_server.close_socket();
    if (m_listen_thread.get_id() != std::this_thread::get_id() && m_listen_thread.joinable())
    {
      m_listen_thread.join();
    }
  }

private:
  WVBoxLayout *m_hbox;
  WText *m_text;
  WMap *m_leaflet;
  std::thread m_listen_thread;
  tcp_server_t m_server;

  void listen()
  {
    while (true)
    {
      char buf[255];
      int socket_client_fd = m_server.accept_client();
      int recv_size = m_server.read_all(socket_client_fd, buf, sizeof(buf));
      std::string str(buf);
      str.resize(recv_size);

      Json::Array result;
      Json::parse(str, result);
      std::string name = result[0];
      int id = result[1];
      double lat = result[2];
      double lon = result[3];
      long long time = result[4];
      int level = result[5];

      std::this_thread::sleep_for(std::chrono::seconds(2));

      WApplication::UpdateLock uiLock(this);
      if (uiLock)
      {
        m_text->setText(Wt::asString(level));
        m_leaflet->removeFromParent();
        m_leaflet = m_hbox->addWidget(cpp14::make_unique<WMap>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13));
        m_leaflet->Circle(lat, lon, level * 12, "#ff0000");
        m_leaflet->Marker(lat, lon, "test", marker_violet);
        triggerUpdate();
      }
      else
      {
        return;
      }
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<WApplication> create_application(const WEnvironment& env)
{
  return cpp14::make_unique<MapApplication>(env);
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &create_application);
}

