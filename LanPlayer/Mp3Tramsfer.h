//TEST_OK
#include <stdio.h>
#include "MessageFunctions.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include<atlconv.h>//其中p为char *型数据  wp为转换后的wchar_t


#define TRANSFERPORT   8000//传输文件端口
WSADATA wsaData;

SOCKET bServerSocket=NULL;//service的监听套接字

SOCKET bAcceptSocket=NULL;

SOCKADDR_IN bservice;


SOCKADDR_IN bclientService;
SOCKET bclient;
///////////////////////////////////////////////重要函数
//接收文件 存储名 接收打开端口
int travalreceiver(char *mp3name,int mp3receiveport);
//发送文件 文件名 目标地址 目标端口
int travalsender(char *mp3name,char *targetipaddress,int targetipport);
///////////////////////////////////////////////
void ReceiveFile(char *mp3name,SOCKET mSocket);

bool NowReceiveRun()
{
	return bServerSocket==NULL?1:0;
}
//接收文件
int travalreceiver(char *mp3name) {
	
	if( bServerSocket==NULL)
		{// 初始化
		if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR )
			ErrorMessageprint("Error at WSAStartup()");

		// 建立socket server 创建服务器监听套节字,TCP协议，及报错机制
		bServerSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if ( bServerSocket == INVALID_SOCKET ) {
			ErrorMessageprint( "Error at socket(): %ld\n", WSAGetLastError() );
			WSACleanup();
			return 0;
		}

		// sockaddr_in service协议族，地址，端口
		bservice.sin_family = AF_INET;//AF_INET指明使用TCP/IP协议族
		bservice.sin_addr.S_un.S_addr = INADDR_ANY;//自动获取本机地址
		bservice.sin_port = htons(TRANSFERPORT);//指明连接服务器的端口号

		//地址绑定到套接字，及报错机制
		if ( SOCKET_ERROR ==bind( bServerSocket, (SOCKADDR*) &bservice, sizeof(SOCKADDR_IN) )) {
			ErrorMessageprint( "bind() failed.\n" );
			closesocket(bServerSocket);
			return 0;
		}

		// service开始监听 socket，及报错机制
		if ( listen( bServerSocket, 1 ) == SOCKET_ERROR )
			ErrorMessageprint( "Error listening on socket.\n");
	}
	
	
	while (true) {
		// 接受连接
		TestMessageprint( "Waiting for server mp3 data...\n" );
		bAcceptSocket = SOCKET_ERROR;
		bAcceptSocket = accept( bServerSocket, NULL, NULL );
		if(SOCKET_ERROR!=bAcceptSocket)
		{
			printf( "Date Bus Connected Success.\n");

			ReceiveFile(mp3name,bAcceptSocket);
		}
		
	}

	
	//system("pause");
	closesocket(bServerSocket);
	closesocket(bAcceptSocket);
	WSACleanup();

	return 0;
}
void ReceiveFile(char *mp3name,SOCKET mSocket)
{
	char recvbuf[1024];//发送缓冲区
	int read;//接收文件的大小
	DWORD        dwWrite;
	BOOL         bRet;
	int Mp3DataCount=0;
	// 发送并接收数据.
	USES_CONVERSION; //转换格式
	HANDLE hFile=CreateFile(A2W(mp3name),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	printf("Receive:_______");
	while(true)
	{
		read=recv( mSocket, recvbuf, 1024, 0 );//接收
		if(read==0)break;

		bRet=WriteFile(hFile,recvbuf,read,&dwWrite,NULL);//写入
		if(bRet==FALSE)
		{
			ErrorMessageprint("Write Buf ERROR!",L"Error");
			break;
		}
		Mp3DataCount=Mp3DataCount+read;
		printf("\b\b\b\b\b\b\b%0.7d",Mp3DataCount);

	}
	printf( "Receive file OK!\n");
	CloseHandle(hFile);//关闭打开文件的线程
}

//发送文件
int travalsender(char *mp3name,char *targetipaddress) {

	// 初始化
	if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");

	// 建立socket client.
	bclient = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( bclient == INVALID_SOCKET ) {
		ErrorMessageprint( "Error at socket(): %ld\n", WSAGetLastError() );
		WSACleanup();
		return 0;
	}

	// 连接到服务器.
	bclientService.sin_family = AF_INET;
	bclientService.sin_addr.s_addr = inet_addr( targetipaddress );
	bclientService.sin_port = htons( TRANSFERPORT );

	TestMessageprint( "trying connect...\n" );

	while ( SOCKET_ERROR==connect( bclient, (SOCKADDR*) &bclientService, sizeof(bclientService) ) ) {
		if(SOCKET_ERROR!=connect( bclient, (SOCKADDR*) &bclientService, sizeof(bclientService) ))
		{printf( "Success.\n");
		break;}

	}

	//发送接受数据
	char sendbuf[1024];//发送缓冲区
	DWORD dwRead;//DWORD dwFileSize文件大小
	BOOL bRet;//定义在读取过程中的错误



	USES_CONVERSION;
	HANDLE hFile=CreateFile(A2W(mp3name),GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);//打开文件的线程
	LARGE_INTEGER mp3size;
	long long int Mofmp3=0;
	GetFileSizeEx(hFile,&mp3size);
	printf("Send:______");
	while(true)
	{
		bRet=ReadFile(hFile,sendbuf,1024,&dwRead,NULL);//读入数据
		if(bRet==FALSE)
		{
			ErrorMessageprint("Read Buf ERROR!");
			break;
		}
		else if(dwRead==0)
		{
			printf("Send file OK!\n");
			break;
		}
		else
		{
			send(bclient,sendbuf,dwRead,0);//发送数据
			Mofmp3=Mofmp3+1024;
			printf("\b\b\b\b\b\b%5.1f%%",Mofmp3*100.0/mp3size.QuadPart);
		}
	};
	CloseHandle(hFile);//关闭打开文件的线程

	closesocket(bclient);
	WSACleanup();
	return 0;

}

