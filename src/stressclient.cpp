/**
 **	\file stressclient.cpp
 **	\date  2006-10-02
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2006  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifdef _WIN32
#pragma warning(disable:4786)
#endif
#include <StdoutLog.h>
#include <ListenSocket.h>
#include <SocketHandler.h>
#include <TcpSocket.h>
#include <Utility.h>
#ifndef _WIN32
#include <signal.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#else
typedef __int64 int64_t;
#endif
#include <HttpGetSocket.h>


static	double min_time = 10000;
static	double max_time = 0;
static	double tot_time = 0;
static	int ant = 0;
static	double min_time2 = 10000;
static	double max_time2 = 0;
static	double tot_time2 = 0;
static	int ant2 = 0;
static	bool quit = false;
static	size_t gnn = 0;
static	std::string gHost = "localhost";
static	port_t gPort = 2222;
static	bool g_b_flood = false;
static	bool g_b_off = false;
static	bool g_b_limit = false;
static	int64_t gBytesIn = 0;
static	int64_t gBytesOut = 0;
static	bool g_b_repeat = false;
static	std::string data;
static	size_t data_size = 1024;
static	bool g_b_stop = false;
static	bool g_b_ssl = false;
static	bool g_b_instant = false;
static	time_t rTime = 10;


void printreport()
{
  printf("\n");
  if (ant)
    printf("connect; ant: %5d min: %.4f max: %.4f med: %.4f\n", ant, min_time, max_time, tot_time / ant);
  if (ant2)
    printf("reply;   ant: %5d min: %.4f max: %.4f med: %.4f\n", ant2, min_time2, max_time2, tot_time2 / ant2);
  double mbi = (double)gBytesIn / 1024;
  mbi /= 1024;
  mbi /= (double)rTime;
  double mbo = (double)gBytesOut / 1024;
  mbo /= 1024;
  mbo /= (double)rTime;
  printf("bytes in: %lld", gBytesIn);
  printf(" (%.2f MB/sec)", mbi);
  printf("  bytes out: %lld", gBytesOut);
  printf(" (%.2f MB/sec)\n", mbo);
}


void tempreport()
{
  printreport();
  //
  min_time = 10000;
  max_time = 0;
  tot_time = 0;
  ant = 0;
  min_time2 = 10000;
  max_time2 = 0;
  tot_time2 = 0;
  ant2 = 0;
  gBytesIn = gBytesOut = 0;
}


void gettime(struct timeval *p, struct timezone *)
{
#ifdef _WIN32
	FILETIME ft; // Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
	GetSystemTimeAsFileTime(&ft);
	uint64_t tt;
	memcpy(&tt, &ft, sizeof(tt));
	tt /= 10;
	p->tv_sec = tt / 1000000;
	p->tv_usec = tt % 1000000;
#else
	gettimeofday(p, NULL);
#endif
}


double Diff(struct timeval t0,struct timeval t)
{
  t.tv_sec -= t0.tv_sec;
  t.tv_usec -= t0.tv_usec;
  if (t.tv_usec < 0)
  {
    t.tv_usec += 1000000;
    t.tv_sec -= 1;
  }
  return t.tv_sec + (double)t.tv_usec / 1000000;
}


class MySocket : public TcpSocket
{
public:
  MySocket(ISocketHandler& h,bool one) : TcpSocket(h), m_b_client(false), m_b_one(one), m_b_created(false), m_b_active(false) {
    gettime(&m_create, NULL);
    SetLineProtocol();
    if (g_b_ssl)
      EnableSSL();
    if (gnn && !m_b_one && Handler().GetCount() >= gnn)
    {
      fprintf(stderr, "\nConnection limit reached: %d, continuing in single connection stress mode\n", gnn);
      if (g_b_off)
        tempreport();
      g_b_limit = true;
      m_b_one = true;
      //
      g_b_flood = g_b_repeat;
    }
    if (!m_b_one && Handler().GetCount() >= FD_SETSIZE - 17)
    {
      fprintf(stderr, "\nFD_SETSIZE connection limit reached: %d, continuing in single connection stress mode\n", Handler().GetCount());
      if (g_b_off)
        tempreport();
      g_b_limit = true;
      m_b_one = true;
      //
      g_b_flood = g_b_repeat;
    }
  }
  ~MySocket() {
  }

  void OnConnect() {
    gettime(&m_connect, NULL);
    m_b_active = true;
    {
      double tconnect = Diff(m_create, m_connect);
      //
      min_time = tconnect < min_time ? tconnect : min_time;
      max_time = tconnect > max_time ? tconnect : max_time;
      tot_time += tconnect;
      ant += 1;
    }
    SendBlock();
    m_b_client = true;
  }

  void SendBlock() {
    gettime(&m_send, NULL);
    Send(data + "\n");
  }

  void OnLine(const std::string& line) {
    gettime(&m_reply, NULL);
    m_b_active = true;
    {
      double treply = Diff(m_send, m_reply);
      //
      min_time2 = treply < min_time2 ? treply : min_time2;
      max_time2 = treply > max_time2 ? treply : max_time2;
      tot_time2 += treply;
      ant2 += 1;
    }
    //
    if (line != data)
    {
      fprintf(stderr, "\n%s\n%s\n", line.c_str(), data.c_str());
      fprintf(stderr, "(reply did not match data - exiting)\n");
      exit(-1);
    }
    //
    gBytesIn += GetBytesReceived(true);
    gBytesOut += GetBytesSent(true);
    if (m_b_one)
    {
      SetCloseAndDelete();
    }
    else
    if (g_b_repeat && g_b_limit)
    {
      SendBlock();
    }
    // add another
    if (!m_b_created && (!g_b_limit || !g_b_off) && !g_b_instant)
    {
      MySocket *p = new MySocket(Handler(), m_b_one);
      p -> SetDeleteByHandler();
      p -> Open(gHost, gPort);
      Handler().Add(p);
      m_b_created = true;
    }
  }

  bool IsActive() {
    bool b = m_b_active;
    m_b_active = false;
    return b;
  }

private:
  bool m_b_client;
  bool m_b_one;
  bool m_b_created;
  bool m_b_active;
  struct timeval m_create;
  struct timeval m_connect;
  struct timeval m_send;
  struct timeval m_reply;
};


class MyHttpSocket : public HttpGetSocket
{
public:
  MyHttpSocket(ISocketHandler& h,const std::string& url) : HttpGetSocket(h,url), m_url(url) {
    gettime(&m_create, NULL);
  }
  ~MyHttpSocket() {}

  void OnConnect() {
    gettime(&m_connect, NULL);
    {
      double tconnect = Diff(m_create, m_connect);
      //
      min_time = tconnect < min_time ? tconnect : min_time;
      max_time = tconnect > max_time ? tconnect : max_time;
      tot_time += tconnect;
      ant += 1;
    }
    gettime(&m_send, NULL);
    HttpGetSocket::OnConnect();
  }

  void OnContent() {
    gettime(&m_reply, NULL);
    {
      double treply = Diff(m_send, m_reply);
      //
      min_time2 = treply < min_time2 ? treply : min_time2;
      max_time2 = treply > max_time2 ? treply : max_time2;
      tot_time2 += treply;
      ant2 += 1;
    }
    CreateNew();
  }
  void CreateNew() {
    if (g_b_off)
      return;
    MyHttpSocket *p = new MyHttpSocket(Handler(), m_url);
    p -> SetDeleteByHandler();
    Handler().Add(p);
    SetCloseAndDelete();
  }

private:
  std::string m_url;
  struct timeval m_create;
  struct timeval m_connect;
  struct timeval m_send;
  struct timeval m_reply;
};


#ifndef _WIN32
void sigint(int)
{
  printreport();
  quit = true;
}


void sigusr1(int)
{
  g_b_flood = true;
}


void sigusr2(int)
{
  tempreport();
}
#endif


class MyHandler : public SocketHandler
{
public:
  MyHandler() : SocketHandler() {
  }
  MyHandler(StdoutLog *p) : SocketHandler(p) {
  }
  ~MyHandler() {
  }
  void Flood() {
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
      Socket *p0 = it -> second;
      MySocket *p = dynamic_cast<MySocket *>(p0);
      if (p)
      {
        p -> SendBlock();
      }
    }
  }
  void Report() {
    size_t ant = 0;
    size_t act = 0;
    for (socket_m::iterator it = m_sockets.begin(); it != m_sockets.end(); it++)
    {
      MySocket *p = dynamic_cast<MySocket *>(it -> second);
      if (p)
      {
        ant++;
        if (p -> IsActive())
        {
          act++;
        }
      }
    }
    printf("Number of //stress// sockets: %d  Active: %d\n", ant, act);
  }
};


int main(int argc,char *argv[])
{
  bool many = false;
  bool one = false;
  bool enableLog = false;
  bool http = false;
  std::string url;
  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-many"))
      many = true;
    if (!strcmp(argv[i], "-one"))
      one = true;
    if (*argv[i] == '-' && strlen(argv[i]) > 1 && isdigit(argv[i][1]) )
      gnn = atoi(argv[i] + 1);
    if (!strcmp(argv[i], "-host") && i < argc - 1)
    {
      gHost = argv[++i];
    }
    if (!strcmp(argv[i], "-port") && i < argc - 1)
    {
      gPort = atoi(argv[++i]);
    }
    if (!strcmp(argv[i], "-off"))
      g_b_off = true;
    if (!strcmp(argv[i], "-repeat"))
      g_b_repeat = true;
    if (!strcmp(argv[i], "-size") && i < argc - 1)
    {
      data_size = atoi(argv[++i]);
    }
    if (!strcmp(argv[i], "-log"))
      enableLog = true;
    if (!strcmp(argv[i], "-time") && i < argc - 1)
      rTime = atoi(argv[++i]);
    if (!strcmp(argv[i], "-stop"))
      g_b_stop = true;
    if (!strcmp(argv[i], "-ssl"))
      g_b_ssl = true;
    if (!strcmp(argv[i], "-instant"))
      g_b_instant = true;
    if (!strcmp(argv[i], "-http"))
      http = true;
    if (!strcmp(argv[i], "-url") && i < argc - 1)
      url = argv[++i];
  }
  if (argc < 2 || (!many && !one && !gnn) )
  {
    printf("Usage: %s [-many|-one|-nn|-off|-repeat|-size nn|-log|-time nn] [-host <hostname>] [-port <port number>]\n", *argv);
    printf("  -many      start max number of connections\n");
    printf("  -one       open - close - repeat\n");
    printf("  -nn        open nn connections, then start -one mode\n");
    printf("  -off       turn off new connections when connection limit reached\n");
    printf("  -repeat    send new block when reply is received\n");
    printf("  -size nn   size of block to send, default is 1024 bytes\n");
    printf("  -log       enable debug log\n");
    printf("  -time nn   time between reports, default 10s\n");
    printf("  -stop      stop after time elapsed\n");
    printf("  -ssl       use ssl\n");
    printf("  -instant   create all sockets at once\n");
    exit(-1);
  }
  fprintf(stderr, "Using data size: %d bytes\n", data_size);
  std::string chars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  while (data.size() < data_size)
  {
    data += chars[rand() % chars.size()];
  }
#ifndef _WIN32
  signal(SIGINT, (__sighandler_t)sigint);
  signal(SIGUSR1, (__sighandler_t)sigusr1);
  signal(SIGUSR2, (__sighandler_t)sigusr2);
  signal(SIGPIPE, SIG_IGN);
#endif
  StdoutLog *log = enableLog ? new StdoutLog() : NULL;
  MyHandler h(log);
  if (http)
  {
    if (!url.size())
    {
      url = "http://" + gHost + ":" + Utility::l2string(gPort) + "/";
    }
    MyHttpSocket *s = new MyHttpSocket(h, url);
    s -> SetDeleteByHandler();
    h.Add(s);
  }
  else
  if (g_b_instant)
  {
    for (size_t i = 0; i < gnn; i++)
    {
      MySocket *s = new MySocket(h, one);
      s -> SetDeleteByHandler();
      s -> Open(gHost, gPort);
      h.Add(s);
    }
    g_b_limit = true;
  }
  else
  {
    MySocket *s = new MySocket(h, one);
    s -> SetDeleteByHandler();
    s -> Open(gHost, gPort);
    h.Add(s);
  }
  time_t t = time(NULL);
  while (!quit)
  {
    h.Select(1, 0);
    if (g_b_flood)
    {
      fprintf(stderr, "\nFlooding\n");
      h.Flood();
      g_b_flood = false;
    }
    if (time(NULL) - t >= rTime) // report
    {
      t = time(NULL);
      tempreport();
      h.Report();
      if (g_b_stop)
      {
        quit = true;
      }
/*
      struct sysinfo info;
      sysinfo(&info);
      printf("Free mem: %ld / %ld\n", info.freeram, info.totalram);
*/
    }
  }
  fprintf(stderr, "\nExiting...\n");
  if (log)
  {
//    delete log;
  }
  return 0;
}


