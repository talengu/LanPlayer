
#ifndef _GTCCONF_
#define _GTCCONF_

#include<stdio.h>

#include <winsock2.h>//����socket�׽���
#define CONTROLPORT       9999 //�������˿�
#define MSGSIZE    1024 //�շ��������Ĵ�С
#pragma comment(lib, "ws2_32.lib") //���Ӿ�̬��

#include "pingip.h"
#include "MessageFunctions.h"
#include "Mp3Player.h"
#include "Mp3Tramsfer.h"
#include "Mp3PathListFun.h"
#endif