//TEST_OK
#include <stdio.h>
#include "MessageFunctions.h"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include<atlconv.h>//����pΪchar *������  wpΪת�����wchar_t


#define TRANSFERPORT   8000//�����ļ��˿�
WSADATA wsaData;

SOCKET bServerSocket=NULL;//service�ļ����׽���

SOCKET bAcceptSocket=NULL;

SOCKADDR_IN bservice;


SOCKADDR_IN bclientService;
SOCKET bclient;
///////////////////////////////////////////////��Ҫ����
//�����ļ� �洢�� ���մ򿪶˿�
int travalreceiver(char *mp3name,int mp3receiveport);
//�����ļ� �ļ��� Ŀ���ַ Ŀ��˿�
int travalsender(char *mp3name,char *targetipaddress,int targetipport);
///////////////////////////////////////////////
void ReceiveFile(char *mp3name,SOCKET mSocket);

bool NowReceiveRun()
{
	return bServerSocket==NULL?1:0;
}
//�����ļ�
int travalreceiver(char *mp3name) {
	
	if( bServerSocket==NULL)
		{// ��ʼ��
		if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR )
			ErrorMessageprint("Error at WSAStartup()");

		// ����socket server ���������������׽���,TCPЭ�飬���������
		bServerSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if ( bServerSocket == INVALID_SOCKET ) {
			ErrorMessageprint( "Error at socket(): %ld\n", WSAGetLastError() );
			WSACleanup();
			return 0;
		}

		// sockaddr_in serviceЭ���壬��ַ���˿�
		bservice.sin_family = AF_INET;//AF_INETָ��ʹ��TCP/IPЭ����
		bservice.sin_addr.S_un.S_addr = INADDR_ANY;//�Զ���ȡ������ַ
		bservice.sin_port = htons(TRANSFERPORT);//ָ�����ӷ������Ķ˿ں�

		//��ַ�󶨵��׽��֣����������
		if ( SOCKET_ERROR ==bind( bServerSocket, (SOCKADDR*) &bservice, sizeof(SOCKADDR_IN) )) {
			ErrorMessageprint( "bind() failed.\n" );
			closesocket(bServerSocket);
			return 0;
		}

		// service��ʼ���� socket�����������
		if ( listen( bServerSocket, 1 ) == SOCKET_ERROR )
			ErrorMessageprint( "Error listening on socket.\n");
	}
	
	
	while (true) {
		// ��������
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
	char recvbuf[1024];//���ͻ�����
	int read;//�����ļ��Ĵ�С
	DWORD        dwWrite;
	BOOL         bRet;
	int Mp3DataCount=0;
	// ���Ͳ���������.
	USES_CONVERSION; //ת����ʽ
	HANDLE hFile=CreateFile(A2W(mp3name),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	printf("Receive:_______");
	while(true)
	{
		read=recv( mSocket, recvbuf, 1024, 0 );//����
		if(read==0)break;

		bRet=WriteFile(hFile,recvbuf,read,&dwWrite,NULL);//д��
		if(bRet==FALSE)
		{
			ErrorMessageprint("Write Buf ERROR!",L"Error");
			break;
		}
		Mp3DataCount=Mp3DataCount+read;
		printf("\b\b\b\b\b\b\b%0.7d",Mp3DataCount);

	}
	printf( "Receive file OK!\n");
	CloseHandle(hFile);//�رմ��ļ����߳�
}

//�����ļ�
int travalsender(char *mp3name,char *targetipaddress) {

	// ��ʼ��
	if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");

	// ����socket client.
	bclient = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( bclient == INVALID_SOCKET ) {
		ErrorMessageprint( "Error at socket(): %ld\n", WSAGetLastError() );
		WSACleanup();
		return 0;
	}

	// ���ӵ�������.
	bclientService.sin_family = AF_INET;
	bclientService.sin_addr.s_addr = inet_addr( targetipaddress );
	bclientService.sin_port = htons( TRANSFERPORT );

	TestMessageprint( "trying connect...\n" );

	while ( SOCKET_ERROR==connect( bclient, (SOCKADDR*) &bclientService, sizeof(bclientService) ) ) {
		if(SOCKET_ERROR!=connect( bclient, (SOCKADDR*) &bclientService, sizeof(bclientService) ))
		{printf( "Success.\n");
		break;}

	}

	//���ͽ�������
	char sendbuf[1024];//���ͻ�����
	DWORD dwRead;//DWORD dwFileSize�ļ���С
	BOOL bRet;//�����ڶ�ȡ�����еĴ���



	USES_CONVERSION;
	HANDLE hFile=CreateFile(A2W(mp3name),GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);//���ļ����߳�
	LARGE_INTEGER mp3size;
	long long int Mofmp3=0;
	GetFileSizeEx(hFile,&mp3size);
	printf("Send:______");
	while(true)
	{
		bRet=ReadFile(hFile,sendbuf,1024,&dwRead,NULL);//��������
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
			send(bclient,sendbuf,dwRead,0);//��������
			Mofmp3=Mofmp3+1024;
			printf("\b\b\b\b\b\b%5.1f%%",Mofmp3*100.0/mp3size.QuadPart);
		}
	};
	CloseHandle(hFile);//�رմ��ļ����߳�

	closesocket(bclient);
	WSACleanup();
	return 0;

}

