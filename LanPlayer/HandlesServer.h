
#include"Conf.h"//���ÿ�����

DWORD WINAPI server_broadcastthread(LPVOID);                 


DWORD WINAPI server_listerthread(LPVOID); 
DWORD WINAPI servicereceive(LPVOID lpParamter);


DWORD WINAPI server_senderthread(LPVOID);   

void kongzhi(char Message[MSGSIZE],int cliendId);
DWORD WINAPI mp3passsender(LPVOID lpParamter);//�����ļ��߳�
int lserver();

/*******************************************************************************
* ������        : DWORD WINAPI server_broadcastthread(LPVOID)
* ��������    	: �������㲥�̣߳����ض˿ڶ�̬��Ŀ�Ķ˿�7001
* �������      : ��
* ������     	: ��
* ����ֵ        : �� ��server��һֱ����                           TEST_OK
*******************************************************************************/
DWORD WINAPI server_broadcastthread(LPVOID)
{         

	WSADATA     s_broadcastwsaData; 

	// ��ʼ��winsock�⣬ʹ��socket��ǰ��
	if ( WSAStartup( MAKEWORD(2,2), &s_broadcastwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");


	SOCKET s_broadcastsocket;
	SOCKADDR_IN s_localsockaddr,s_targetsockaddr;//�㲥�ı��غ�Ŀ�Ķ˿ڵ��׽���
	BOOL fBroadcast = TRUE;
	char sendBuff[MSGSIZE];
	int ncount=0;
	// ����socket
	s_broadcastsocket=socket(PF_INET,SOCK_DGRAM,0);

	// �򿪹㲥ѡ�ʹsocket���Թ㲥��Ϣ
	if (SOCKET_ERROR==setsockopt ( s_broadcastsocket,SOL_SOCKET,SO_BROADCAST, (CHAR *)&fBroadcast,sizeof ( BOOL )))
         ErrorMessageprint("setsockopt() failed");

	// ��socket�󶨵����ض˿�
	s_localsockaddr.sin_family = AF_INET;
	s_localsockaddr.sin_port = htons(0);
	s_localsockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//��ַ�󶨵��׽��֣����������
	if (SOCKET_ERROR==bind( s_broadcastsocket, (SOCKADDR*) &s_localsockaddr, sizeof(SOCKADDR_IN) ))
		ErrorMessageprint( "bind() failed." );

	// �趨�㲥��Ŀ�Ķ˿�
	s_targetsockaddr.sin_family = AF_INET;
	s_targetsockaddr.sin_addr.s_addr = htonl ( INADDR_BROADCAST );
	s_targetsockaddr.sin_port = htons (7001);//�����õĶ˿ڣ����Ը�����Ҫ����

	while(TRUE)
	{
		// �㲥��Ϣ
		//��Ϣ���ݵĸ�ֵ����ʾ

		sprintf(sendBuff,"Message %d",ncount++);
		sendto (s_broadcastsocket,sendBuff,strlen(sendBuff),0,(SOCKADDR *) &s_targetsockaddr,sizeof ( SOCKADDR_IN ));
		// printf("%s\n",sendBuff);//�����õ�printf����
		//ÿ���뷢һ��
		Sleep(3000);
	}
	// �ͷ����Ӻͽ��н�������
	closesocket(s_broadcastsocket);
	WSACleanup();
	return 0;
}
/*******************************************************************************
* ������        : DWORD WINAPI server_listerthread(LPVOID)
* ��������    	: ���߳����ڼ���server�˿ڽ��ո���client��������Ϣ(tcp)
* �������      : ��
* ������     	: 
* ����ֵ        : ��
*******************************************************************************/

#define MAX 15//�ͻ������
int ClientId=0;//��¼���ٸ��ͻ�
SOCKET ServerSocket;//!!!���òſ�ִ������Ľ���
SOCKET      New_ClientSocket[MAX+1];
SOCKADDR_IN clientaddr[MAX+1];//�洢�ͻ�������
DWORD WINAPI server_listerthread(LPVOID)
{
	WSADATA     serversenderwsaData;
	SOCKADDR_IN s_localaddr;
	

	// ��ʼ��winsock�⣬ʹ��socket��ǰ��
	if ( WSAStartup( MAKEWORD(2,2), &serversenderwsaData ) != NO_ERROR )
		ErrorMessageprint("Error at WSAStartup()");

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//���������������׽��֡�TCPЭ��

	s_localaddr.sin_family = AF_INET; //AF_INETָ��ʹ��TCP/IPЭ����
	s_localaddr.sin_port = htons(CONTROLPORT);//ָ�����ӷ������Ķ˿ں�
	s_localaddr.sin_addr.s_addr = htonl(INADDR_ANY);//�Զ���ȡ������ַ

	//��ַ�󶨵��׽���
	if(SOCKET_ERROR==bind(ServerSocket, (struct sockaddr *)&s_localaddr, sizeof(SOCKADDR_IN)))

		ErrorMessageprint("Can't bind socket to local port");//��ʼ��ʧ�ܷ���-1
		

	//��ʼ����
	if(SOCKET_ERROR==listen(ServerSocket, 1))
		ErrorMessageprint("Can't listen ServerSocket.");//��ʼ��ʧ�ܷ���-1


	/////////////////////////////////////////////////////////////////////////////////
	HANDLE server_sender_handle=CreateThread(NULL,0,server_senderthread,0,0,0);
	CloseHandle(server_sender_handle);
	///////////////////////////////////////////////////////////////////////////////



	SOCKADDR_IN s_clientaddr;//�洢�ͻ�������
	int iaddrSize=sizeof ( SOCKADDR_IN );
	while(TRUE)
	{
		if( New_ClientSocket[ClientId] = accept(ServerSocket, (struct sockaddr *)&s_clientaddr, &iaddrSize))//��������
		{
			HANDLE hThread=CreateThread(NULL,0,servicereceive,(LPVOID)&ClientId,0,0);//���������߳�
			CloseHandle(hThread);
			clientaddr[ClientId]=s_clientaddr;//��������

			ClientId++;
			printf("New Client Connected!\n");
			TestMessageprint("Client ID:%d\nClient IP_and_PORT:%s:%d",ClientId,inet_ntoa(s_clientaddr.sin_addr), ntohs(s_clientaddr.sin_port));
			if(ClientId>MAX)
				break;
		}
	}return 0;
}

//���ڶ��̵߳Ĵ�������
//Ϊserver_listerthread�ṩ�ͻ��˼�������
DWORD WINAPI servicereceive(LPVOID lpParamter)//���ڶ��̵߳Ĵ�������
{
	int Thread_ClientId=*((int*)lpParamter);
	//
	TestMessageprint("Client %d connected and thread turn on OK.",Thread_ClientId);
	char c_Message[MSGSIZE]; //�շ�������
	int ret; //�����ֽڵĸ���
	while (TRUE)
	{ 
		ret = recv( New_ClientSocket[Thread_ClientId-1], c_Message, MSGSIZE, 0);//��������
	if(ret==SOCKET_ERROR)
	{
		printf("client%d is closed!\n",Thread_ClientId);
		break;//client�Ͽ��˳��ÿͻ��˵Ľ����߳�
	}
	c_Message[ret] = '\0';

	printf("client%d: %s\n",Thread_ClientId, c_Message); 
	}
	return 0; 
}
/*******************************************************************************
* ������        : DWORD WINAPI server_senderthread(LPVOID)
* ��������    	: 
* �������      : ��
* ������     	: 
* ����ֵ        : ��
*******************************************************************************/

char *mname[200];
DWORD WINAPI server_senderthread(LPVOID)
{  
	char s_Message[MSGSIZE]; //�շ�������
	
	while (TRUE)
	{
		fflush(stdin);
		//printf(">>");
		//�Ӽ�������
		gets(s_Message);  
			if(s_Message[0]=='@')
			{ Mp3FinalList(mname);
			fflush(stdin);}
		//��������
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
	
	// �ͷ����Ӻͽ��н�������
	closesocket( ServerSocket);
	WSACleanup();
	getchar();
	return 0;
}
/*******************************************************************************
* ������        : lserver()
* ��������    	: ����������
* �������      : ��
* ������     	: ��
* ����ֵ        : ��
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
	{}
	getchar();
	return 0;
}

//server��������
void kongzhi(char Message[MSGSIZE],int clientId)
{
	char outmessage[1024];
		if(Message[0]=='@')//����
	{
		send(New_ClientSocket[clientId], "x", strlen("x"), 0);
			     
		Sleep(500);//��ʱʹclient�Ĳ����̹߳ر�
		travalsender(mname[0],inet_ntoa(clientaddr[clientId].sin_addr));
			//	travalsender("1.mp3",inet_ntoa(clientaddr[clientId].sin_addr),clientId+5200);
			//HANDLE hThread=CreateThread(NULL,0,mp3passsender,(LPVOID)&clientId,0,0);//���������߳�
			//CloseHandle(hThread);	
		
	}
		 Mp3ChooseFun3(mname[0],Message[0],outmessage,0);
		if(Message[0]=='r')
		{
			int a=Ping2(inet_ntoa(clientaddr[clientId].sin_addr));
			sprintf(outmessage,"r%d",a);
			send(New_ClientSocket[clientId], outmessage, strlen(outmessage), 0);
		}
		 printf("Server : %s\n",outmessage);//��ӡ������������
		 memset(outmessage, 0, strlen(outmessage) );
}
// #q==����mp3
// @pxxxx==����mp3�Ͷ˿�
//
DWORD WINAPI mp3passsender(LPVOID lpParamter)//�����ļ��߳�
{
	int Thread_ClientId=*((int*)lpParamter);
	if(mname[0]!=NULL)
	travalsender(mname[0],inet_ntoa(clientaddr[Thread_ClientId].sin_addr));
	return 0;
}