#ifndef __HTTP__
#define __HTTP__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>      
#include <sys/socket.h>

#define BUFSIZE 41000
#define URLSIZE 1024
#define INVALID_SOCKET -1

/**
* 类功能描述 HTTP请求方法
* @author    博客
* @time      2017年7月15日
*/
class Http
{
	public:
		void debugOut(const char *fmt, ...);
		
		int httpGet(const char* strUrl, char* strResponse);
		int httpPost(const char* strUrl, const char* strData, char* strResponse);

	private:
		int   httpRequestExec(const char* strMethod, const char* strUrl, const char* strData, char* strResponse);
		char* httpHeadCreate(const char* strMethod, const char* strUrl, const char* strData);
		char* httpDataTransmit(char *strHttpHead, const int iSockFd);
			
		int   getPortFromUrl(const char* strUrl);
		char* getIPFromUrl(const char* strUrl);
		char* getParamFromUrl(const char* strUrl);
		char* getHostAddrFromUrl(const char* strUrl);
		
		int   socketFdCheck(const int iSockFd);	
		
		static int m_iSocketFd;
};

#endif