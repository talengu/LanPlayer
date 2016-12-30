
#ifndef _GTCCONF_
#define _GTCCONF_

#include<stdio.h>

#include <winsock2.h>//调用socket套接字
#define CONTROLPORT       9999 //服务器端口
#define MSGSIZE    1024 //收发缓冲区的大小
#pragma comment(lib, "ws2_32.lib") //链接静态库

#include "pingip.h"
#include "MessageFunctions.h"
#include "Mp3Player.h"
#include "Mp3Tramsfer.h"
#include "Mp3PathListFun.h"
#endif