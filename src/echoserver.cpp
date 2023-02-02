/**
 **	\file echoserver.cpp
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
#ifndef _WIN32
#include <signal.h>
#include <sys/sysinfo.h>
#endif
#include <HttpDebugSocket.h>


static	bool g_b_detach = false;
static	bool g_b_ssl = false;
static	std::string gFilename = "server.pem";
static	std::string gPw;
static	bool quit = false;
static	bool g_b_http = false;
static	bool g_b_nobuf = false;
static	bool g_b_detach2 = false;

#define DEB(x) 


class MySocket : public TcpSocket
{
public:
  MySocket(ISocketHandler& h) : TcpSocket(h) {
    SetLineProtocol();
    if (g_b_ssl)
      EnableSSL();
    if (g_b_nobuf)
      DisableInputBuffer();
  }
  ~MySocket() {
  }
  
  void OnAccept() {
    if (g_b_detach)
    {
      if (!Detach())
        fprintf(stderr, "\nDetach failed\n");
    }
  }

  void OnRawData(const char *buf,size_t len) {
    if (g_b_nobuf)
    {
      SendBuf(buf, len);
    }
  }

  void OnLine(const std::string& line) {
    if (g_b_detach2 && !IsDetach())
    {
      m_line = line;
      if (!Detach())
      {
        fprintf(stderr, "\nDetach failed\n");
      }
      return;
    }
DEB(printf("fd %d OnLine; %s\n", GetSocket(), Handler().IsSlave() ? "slave" : "master");)
    Send(line + "\n");
  }

  void OnDetached() {
DEB(printf("fd %d OnDetached; %s\n", GetSocket(), Handler().IsSlave() ? "slave" : "master");)
//    fprintf(stderr, "-");
//    fflush(stderr);
    if (g_b_detach2)
    {
      Send(m_line + "\n");
    }
  }

  void InitSSLServer() {
    InitializeContext(gFilename, gPw, SSLv23_method());
  }

private:
  std::string m_line;
};


#ifndef _WIN32
void sigint(int)
{
  quit = true;
}
#endif

int main(int argc,char *argv[])
{
#ifndef _WIN32
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, (__sighandler_t)sigint);
#endif
  port_t port = 2222;
  bool enableLog = false;
  int queue = 20;
  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-detach"))
      g_b_detach = true;
    if (!strcmp(argv[i], "-detach2"))
      g_b_detach2 = true;
    if (!strcmp(argv[i], "-ssl"))
      g_b_ssl = true;
    if (!strcmp(argv[i], "-port") && i < argc - 1)
      port = atoi(argv[++i]);
    if (!strcmp(argv[i], "-file") && i < argc - 1)
      gFilename = argv[++i];
    if (!strcmp(argv[i], "-pw") && i < argc - 1)
      gPw = argv[++i];
    if (!strcmp(argv[i], "-log"))
      enableLog = true;
    if (!strcmp(argv[i], "-queue") && i < argc - 1)
      queue = atoi(argv[++i]);
    if (!strcmp(argv[i], "-http"))
      g_b_http = true;
    if (!strcmp(argv[i], "-nobuf"))
      g_b_nobuf = true;
    if (!strcmp(argv[i], "-h"))
    {
    	printf("Usage: %s [-port nn] [-detach] [-ssl]\n", *argv);
    	printf(" -port nn   listen on port nn\n");
    	printf(" -detach    detach each socket on accept\n");
    	printf(" -detach2   detach when line received\n");
    	printf(" -ssl       run as ssl server, .pem file needed\n");
    	printf(" -file xx   .pem filename, default is \"server.pem\"\n");
    	printf(" -pw xx     private key password\n");
    	printf(" -log       enable sockethandler debug log\n");
    	printf(" -queue nn  listen queue size (default 20)\n");
    	printf(" -http      enable http server with dummy response\n");
    	printf(" -nobuf     echo raw data\n");
    	exit(-1);
    }
  }
  if (g_b_http && g_b_nobuf)
  {
    printf("Nobuf and Http does not work together\n");
    exit(-1);
  }
  StdoutLog *log = enableLog ? new StdoutLog() : NULL;
  SocketHandler h(log);
  ListenSocket<MySocket> l(h);
  ListenSocket<HttpDebugSocket> l2(h);
  if (!g_b_http)
  {
    if (l.Bind(port, queue))
    {
      fprintf(stderr, "Bind to port %d failed\n", port);
      return -1;
    }
    fprintf(stderr, "Listening on port %d\n", port);
    h.Add(&l);
  }
  else
  {
    printf("Enable HttpDebugSocket\n");
    if (l2.Bind(port, queue))
    {
      fprintf(stderr, "Bind to port %d failed\n", port);
      return -1;
    }
    fprintf(stderr, "Listening on port %d\n", port);
    h.Add(&l2);
  }
  if (g_b_detach)
    fprintf(stderr, "Will detach each incoming socket\n");
  if (g_b_ssl)
    fprintf(stderr, "Using SSL\n");
//  time_t t = time(NULL);
  while (!quit)
  {
    h.Select(1, 0);
/*
    if (time(NULL) - t > 10)
    {
      t = time(NULL);
      struct sysinfo info;
      sysinfo(&info);
      printf("Free mem: %ld / %ld\n", info.freeram, info.totalram);
    }
*/
  }
/*
  if (g_b_detach)
  {
    fprintf(stderr, "Waiting for threads to exit...\n");
#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
  }
*/
  fprintf(stderr, "\nExiting...\n");
  if (log)
  {
//    delete log;
  }
  return 0;
}


