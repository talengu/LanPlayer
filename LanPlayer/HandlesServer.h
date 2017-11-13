
#include"Conf.h"//引用库配置

DWORD WINAPI server_broadcastthread(LPVOID);                 


DWORD WINAPI server_listerthread(LPVOID); 
DWORD WINAPI servicereceive(LPVOID lpParamter);


DWORD WINAPI server_senderthread(LPVOID);   

void kongzhi(char Message[MSGSIZE],int cliendId);
DWORD WINAPI mp3passsender(LPVOID lpParamter);//发送文件线程
int lserver();

/*******************************************************************************
* 函数名        : DWORD WINAPI server_broadcastthread(LPVOID)
* 函数描述    	: 服务器广播线程，本地端口动态，目的端口7001
* 输入参数      : 无
* 输出结果     	: 无
* 返回值        : 无 在server端一直运行                           TEST_OK
*******************************************************************************/
DWORD WINAPI server_broadcastthread(LPVOID)
{         

	WSADATA     s_broadcastwsaData; 

	// 初始化winsock库，使用socket的前提
	if ( WSAStartup( MAKEWORD(2,2), &s_broadcastwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");


	SOCKET s_broadcastsocket;
	SOCKADDR_IN s_localsockaddr,s_targetsockaddr;//广播的本地和目的端口的套节字
	BOOL fBroadcast = TRUE;
	char sendBuff[MSGSIZE];
	int ncount=0;
	// 创建socket
	s_broadcastsocket=socket(PF_INET,SOCK_DGRAM,0);

	// 打开广播选项，使socket可以广播消息
	if (SOCKET_ERROR==setsockopt ( s_broadcastsocket,SOL_SOCKET,SO_BROADCAST, (CHAR *)&fBroadcast,sizeof ( BOOL )))
         ErrorMessageprint("setsockopt() failed");

	// 将socket绑定到本地端口
	s_localsockaddr.sin_family = AF_INET;
	s_localsockaddr.sin_port = htons(0);
	s_localsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//地址绑定到套接字，及报错机制
	if (SOCKET_ERROR==bind( s_broadcastsocket, (SOCKADDR*) &s_localsockaddr, sizeof(SOCKADDR_IN) ))
		ErrorMessageprint( "bind() failed." );

	// 设定广播的目的端口
	s_targetsockaddr.sin_family = AF_INET;
	s_targetsockaddr.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	s_targetsockaddr.sin_port = htons (7001);//发送用的端口，可以根据需要更改

	while(TRUE)
	{
		// 广播消息
		//消息内容的赋值和显示

		sprintf(sendBuff,"Message %d",ncount++);
		sendto (s_broadcastsocket,sendBuff,strlen(sendBuff),0,(SOCKADDR *) &s_targetsockaddr,sizeof ( SOCKADDR_IN ));
		// printf("%s\n",sendBuff);//测试用的printf（）
		//每三秒发一次
		Sleep(3000);
	}
	// 释放连接和进行结束工作
	closesocket(s_broadcastsocket);
	WSACleanup();
	return 0;
}
/*******************************************************************************
* 函数名        : DWORD WINAPI server_listerthread(LPVOID)
* 函数描述    	: 此线程用于监听server端口接收各个client传来的信息(tcp)
* 输入参数      : 无
* 输出结果     	: 
* 返回值        : 无
*******************************************************************************/

#define MAX 15//客户端最多
int ClientId=0;//记录多少个客户
SOCKET ServerSocket;//!!!需获得才可执行下面的进程
SOCKET      New_ClientSocket[MAX+1];
SOCKADDR_IN clientaddr[MAX+1];//存储客户端数据
DWORD WINAPI server_listerthread(LPVOID)
{
	WSADATA     serversenderwsaData;
	SOCKADDR_IN s_localaddr;
	

	// 初始化winsock库，使用socket的前提
	if ( WSAStartup( MAKEWORD(2,2), &serversenderwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建服务器监听套节字。TCP协议

	s_localaddr.sin_family = AF_INET; //AF_INET指明使用TCP/IP协议族
	s_localaddr.sin_port = htons(CONTROLPORT);//指明连接服务器的端口号
	s_localaddr.sin_addr.s_addr = htonl(INADDR_ANY);//自动获取本机地址

	//地址绑定到套接字
	if(SOCKET_ERROR==bind(ServerSocket, (struct sockaddr *)&s_localaddr, sizeof(SOCKADDR_IN)))

		ErrorMessageprint("Can't bind socket to local port");//初始化失败返回-1
		

	//开始监听
	if(SOCKET_ERROR==listen(ServerSocket, 1))
		ErrorMessageprint("Can't listen ServerSocket.");//初始化失败返回-1


	/////////////////////////////////////////////////////////////////////////////////
	HANDLE server_sender_handle=CreateThread(NULL,0,server_senderthread,0,0,0);
	CloseHandle(server_sender_handle);
	///////////////////////////////////////////////////////////////////////////////



	SOCKADDR_IN s_clientaddr;//存储客户端数据
	int iaddrSize=sizeof ( SOCKADDR_IN );
	while(TRUE)
	{
		if( New_ClientSocket[ClientId] = accept(ServerSocket, (struct sockaddr *)&s_clientaddr, &iaddrSize))//建立连接
		{
			HANDLE hThread=CreateThread(NULL,0,servicereceive,(LPVOID)&ClientId,0,0);//创建接收线程
			CloseHandle(hThread);
			clientaddr[ClientId]=s_clientaddr;//保存起来

			ClientId++;
			printf("New Client Connected!\n");
			TestMessageprint("Client ID:%d\nClient IP_and_PORT:%s:%d",ClientId,inet_ntoa(s_clientaddr.sin_addr), ntohs(s_clientaddr.sin_port));
			if(ClientId>MAX)
				break;
		}
	}return 0;
}

//关于多线程的处理待解决
//为server_listerthread提供客户端监听服务
DWORD WINAPI servicereceive(LPVOID lpParamter)//关于多线程的处理待解决
{
	int Thread_ClientId=*((int*)lpParamter);
	//
	TestMessageprint("Client %d connected and thread turn on OK.",Thread_ClientId);
	char c_Message[MSGSIZE]; //收发缓冲区
	int ret; //接收字节的个数
	while (TRUE)
	{ 
		ret = recv( New_ClientSocket[Thread_ClientId-1], c_Message, MSGSIZE, 0);//接收数据
	if(ret==SOCKET_ERROR)
	{
		printf("client%d is closed!\n",Thread_ClientId);
		break;//client断开退出该客户端的接收线程
	}
	c_Message[ret] = '\0';

	printf("client%d: %s\n",Thread_ClientId, c_Message); 
	}
	return 0; 
}
/*******************************************************************************
* 函数名        : DWORD WINAPI server_senderthread(LPVOID)
* 函数描述    	: 
* 输入参数      : 无
* 输出结果     	: 
* 返回值        : 无
*******************************************************************************/

char *mname[200];
DWORD WINAPI server_senderthread(LPVOID)
{  
	char s_Message[MSGSIZE]; //收发缓冲区
	
	while (TRUE)
	{
		fflush(stdin);
		//printf(">>");
		//从键盘输入
		gets(s_Message);  
			if(s_Message[0]=='@')
			{ Mp3FinalList(mname);
			fflush(stdin);}
		//发送数据
		for(int i=0;i<ClientId;i++)
			{
			
				if(SOCKET_ERROR==send(New_ClientSocket[i], s_Message, strlen(s_Message), 0))
			       ErrorMessageprint("send() to client%d failed",i);
				else
					////////////////////////////
					//////////////////////////
                kongzhi(s_Message,i);
				/////////////////////////////
				///////////////////////
		}
		
	}
	
	// 释放连接和进行结束工作
	closesocket( ServerSocket);
	WSACleanup();
	getchar();
	return 0;
}
/*******************************************************************************
* 函数名        : lserver()
* 函数描述    	: 服务器生成
* 输入参数      : 无
* 输出结果     	: 无
* 返回值        : 无
*******************************************************************************/
int lserver(){
	HANDLE server_broadcast_handle=CreateThread(NULL,0,server_broadcastthread,0,0,0);
	CloseHandle(server_broadcast_handle);
	HANDLE server_lister_handle=CreateThread(NULL,0,server_listerthread,0,0,0);
	CloseHandle(server_lister_handle);

	/////////////////////////////////////////////////////////////
	/*HANDLE server_sender_handle=CreateThread(NULL,0,server_senderthread,0,0,0);
	CloseHandle(server_sender_handle);*/
	/////////////////////////////////////////////////////////////
	while(1)
	{
		Sleep(5000);//bug fix
	}
	getchar();
	return 0;
}

//server控制命令
void kongzhi(char Message[MSGSIZE],int clientId)
{
	char outmessage[1024];
		if(Message[0]=='@')//传输
	{
		send(New_ClientSocket[clientId], "x", strlen("x"), 0);
			     
		Sleep(500);//延时使client的播放线程关闭
		travalsender(mname[0],inet_ntoa(clientaddr[clientId].sin_addr));
			//	travalsender("1.mp3",inet_ntoa(clientaddr[clientId].sin_addr),clientId+5200);
			//HANDLE hThread=CreateThread(NULL,0,mp3passsender,(LPVOID)&clientId,0,0);//创建接收线程
			//CloseHandle(hThread);	
		
	}
		 Mp3ChooseFun3(mname[0],Message[0],outmessage,0);
		if(Message[0]=='r')
		{
			int a=Ping2(inet_ntoa(clientaddr[clientId].sin_addr));
			sprintf(outmessage,"r%d",a);
			send(New_ClientSocket[clientId], outmessage, strlen(outmessage), 0);
		}
		 printf("Server : %s\n",outmessage);//打印服务器端数据
		 memset(outmessage, 0, strlen(outmessage) );
}
// #q==加载mp3
// @pxxxx==接收mp3和端口
//
DWORD WINAPI mp3passsender(LPVOID lpParamter)//发送文件线程
{
	int Thread_ClientId=*((int*)lpParamter);
	if(mname[0]!=NULL)
	travalsender(mname[0],inet_ntoa(clientaddr[Thread_ClientId].sin_addr));
	return 0;
}
