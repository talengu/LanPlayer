
#include"Conf.h"//引用库配置

DWORD WINAPI client_broadcastthread(LPVOID);

DWORD WINAPI client_listerthread(LPVOID);

DWORD WINAPI client_senderthread(LPVOID);

//解析控制命令
void jiexi(char Message[MSGSIZE]);

DWORD WINAPI mp3receiver(LPVOID lpParamter);//发送文件线程
int lclient();

/*******************************************************************************
* 函数名        : DWORD WINAPI client_broadcastthread(LPVOID)
* 函数描述    	: 客户端在7001口接收广播,获得serverIP后开启client_senderthread线程
* 输入参数      : 无
* 输出结果     	: 服务器的SOCKADDR_IN  c_serverbroadcastaddr
* 返回值        : 无                                                   TEST_OK
*******************************************************************************/
SOCKADDR_IN Client_Broadcast_Geted_Serveraddr;//client从广播里获得的serveraddr
DWORD WINAPI client_broadcastthread(LPVOID)
{  
	//首先监听service的broadcast
	WSADATA     c_broadcastwsaData; 
	SOCKET c_broadcastsocket;
	SOCKADDR_IN c_localsockaddr;
	char cRecvBuff[MSGSIZE];
	int nSize,nbSize;

	// 初始化winsock库，使用socket的前提
	if ( WSAStartup( MAKEWORD(2,2), &c_broadcastwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");


	c_broadcastsocket=socket(AF_INET, SOCK_DGRAM,0);

	c_localsockaddr.sin_family = AF_INET;
	c_localsockaddr.sin_port = htons(7001);//发送端使用的发送端口，可以根据需要更改
	c_localsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 绑定到7001端口，以监听来自网络的数据
	if(bind( c_broadcastsocket, (SOCKADDR FAR *)&c_localsockaddr, sizeof(SOCKADDR_IN))==SOCKET_ERROR)
		ErrorMessageprint("Can't bind socket to local port!Program stop.\n");

	//接受消息
	while(TRUE)
	{
		nSize = sizeof ( SOCKADDR_IN );

		//由nbSize记录接收到的数组大小
		//recvfrom(
		//  _In_ SOCKET s,
		//  _Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR * buf,
		//  _In_ int len,
		//  _In_ int flags,
		//  _Out_writes_bytes_to_opt_(*fromlen, *fromlen) struct sockaddr FAR * from,
		//  _Inout_opt_ int FAR * fromlen
		//  );
		if((nbSize=recvfrom (c_broadcastsocket,cRecvBuff,MSGSIZE,0,(SOCKADDR FAR *) &Client_Broadcast_Geted_Serveraddr,&nSize))==SOCKET_ERROR)
		{
			ErrorMessageprint("Broadcast recive Error");
			break;
			//exit(1);
		}
		cRecvBuff[nbSize] = '\0';
		TestMessageprint("%s",cRecvBuff);
		if(nbSize>2)//因为服务器广播的信息大于2 “Message X”
			//打印出发广播服务器的地址和端口
		{ 
			printf("Find A Server!\n");
			TestMessageprint("Server IP_and_PORT:%s:%d", inet_ntoa(Client_Broadcast_Geted_Serveraddr.sin_addr), ntohs(Client_Broadcast_Geted_Serveraddr.sin_port));

			//开启client发送线程
			//////////////////////////////////////////////////////////////////////////////////
			HANDLE client_sender_handle=CreateThread(NULL,0,client_senderthread,0,0,0);
			CloseHandle(client_sender_handle);

			///////////////////////////////////////////////////////////////////////////////////
			break;
			//退出client接收广播的线程
		}
	}

	// 释放连接和进行结束工作
	closesocket(c_broadcastsocket);
	WSACleanup();
	return 0;
}
/*******************************************************************************
* 函数名        : DWORD WINAPI client_senderthread(LPVOID)
* 函数描述    	: 客户端发送消息给服务器
* 输入参数      : 无
* 输出结果     	: SOCKADDR_IN Client_Broadcast_Geted_Serveraddr
* 返回值        : 无
*******************************************************************************/

SOCKET ClientSocket;
DWORD WINAPI client_senderthread(LPVOID)
{  

	WSADATA     c_listerwsaData; 

	SOCKADDR_IN serveraddr;

	char      c_Message[MSGSIZE];

	// 初始化winsock库，使用socket的前提
	if ( WSAStartup( MAKEWORD(2,2), &c_listerwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");


	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    

	memset(&serveraddr, 0, sizeof(SOCKADDR_IN));               //先将保存地址的server置为全0
	serveraddr.sin_family = PF_INET;                           //声明地址格式是TCP/IP地址格式
	serveraddr.sin_port = htons(CONTROLPORT);                         //指明连接服务器的端口号
	//printf("Accepte server:%s:%d\n", inet_ntoa(saClient.sin_addr), ntohs(saClient.sin_port));//检测用句
	serveraddr.sin_addr= Client_Broadcast_Geted_Serveraddr.sin_addr;    //指明连接服务器的IP地址
	if (SOCKET_ERROR==connect( ClientSocket, (struct sockaddr *)&serveraddr, sizeof(SOCKADDR_IN))) //连到刚才指明的服务器上
	{
		ErrorMessageprint("Can't connect Server.");//初始化失败返回-1
		//可能server端的客户已到最大值
	}
	TestMessageprint("connect server success");

	/////////////////////////////////////////////////////////////////////////
	HANDLE client_lister_handle=CreateThread(NULL,0,client_listerthread,0,0,0);
	CloseHandle(client_lister_handle);
	////////////////////////////////////////////////////////////////////////



	while (TRUE)
	{
		fflush(stdin);
		//printf(">>");
		gets(c_Message); 
		if(SOCKET_ERROR==send( ClientSocket, c_Message, strlen(c_Message), 0))
			ErrorMessageprint("send() failed");

		// printf("[SendMessage_OK]\n");
		// if(!strcmp(c_Message,"exit"))//client自身退出
		//  exit(1);
	}

	// 释放连接和进行结束工作
	closesocket( ClientSocket);
	WSACleanup();
	getchar();
	return 0;
}

/*******************************************************************************
* 函数名        : DWORD WINAPI client_listerthread(LPVOID)
* 函数描述    	: 需要有ClientSocket
* 输入参数      : 无
* 输出结果     	: 
* 返回值        : 无
*******************************************************************************/
DWORD WINAPI client_listerthread(LPVOID)
{
	char   s_Message[MSGSIZE];
	int    ret;
	while (TRUE)
	{
		ret = recv(ClientSocket, s_Message, MSGSIZE, 0);
		if(SOCKET_ERROR==ret)
		{
			printf("Server is closed!\n");
			printf("Waiting for another server...\n");
			//////////////////////////////
			//////////////////////////
			//////////////////probrom
			lclient();
			////////////////////////
			/////////////////
			//////////////////
		}
		s_Message[ret] = '\0';  
		printf("Server: %s\n", s_Message);
		//////////////////////////////////////
		jiexi(s_Message);
		//////////////////////////////////////
	}
	closesocket(ClientSocket);
	WSACleanup();
}
/*******************************************************************************
* 函数名        : lclient()
* 函数描述    	: 客户端生成
* 输入参数      : 无
* 输出结果     	: 无
* 返回值        : 无
*******************************************************************************/
int lclient(){
	HANDLE client_broadcast_handle=CreateThread(NULL,0,client_broadcastthread,0,0,0);
	CloseHandle(client_broadcast_handle);


	///////////////////////////////////////////////////////////////////
	/*HANDLE client_lister_handle=CreateThread(NULL,0,client_listerthread,0,0,0);
	CloseHandle(client_lister_handle);*/
	////////////////////////////////////////////////////////////////////
	while(1)
	{}
	getchar();
	return 0;
}
//解析控制命令
void jiexi(char Message[MSGSIZE])
{
	char outmessage[1024];
	     //播放命令
	      // Mp3ChooseFun3("temp.mp3",Message[0],outmessage,atoi(Message+1));
		   Mp3ChooseFun3("temp.mp3",Message[0],outmessage,atoi(Message+1));
		   if(outmessage!=NULL)
		if(SOCKET_ERROR==send( ClientSocket, outmessage, strlen(outmessage), 0))
			ErrorMessageprint("send() failed");
		    memset(outmessage, 0, strlen(outmessage) );//清空数据
		if(Message[0]=='@')//传输
	{
		        // Mp3Close();
				 if(NowReceiveRun())
				{HANDLE hThread=CreateThread(NULL,0,mp3receiver,0,0,0);//创建接收线程
				 CloseHandle(hThread);}	
		
	}
		if(Message[0]=='x')//传输impottant
	{
				 Mp3Close();
	}
}
// #q==加载mp3
// @pxxxx==接收mp3和端口
//

// #q==加载mp3
// @pxxxx==接收mp3和端口
//
DWORD WINAPI mp3receiver(LPVOID)//发送文件线程
{
	travalreceiver("temp.mp3");
	return 0;
}