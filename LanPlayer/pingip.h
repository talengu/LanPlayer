
#include<stdio.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <Windows.h>

//定义默认缓冲区长度
#define DEF_BUF_SIZE 1024
#define IP_HEADER_SIZE 20
#define ICMP_HEADER_SIZE (sizeof(ICMP_HEADER))
#define ICMP_DATA_SIZE 32
#define ICMP_PACK_SIZE (ICMP_HEADER_SIZE + ICMP_DATA_SIZE)
#define Gfault 1000

///////////////////////
#define TimeSpace 200
#define NumAll 3
//////////////////////////
typedef struct _ICMP_HEADER{
    BYTE nType;
    BYTE nCode;
    USHORT nCheckSum;
    USHORT nId;
    USHORT nSequence;
    UINT nTimeStamp;
}ICMP_HEADER,*PICMP_HEADER;
 
char szInfo[DEF_BUF_SIZE] = {0};

USHORT GetCheckSum(LPBYTE lpBuf, DWORD dwSize);
int Ping2(char* lpDestIp);
//int main(int argc, char* argv[])
//{
//    char szDestIp[DEF_BUF_SIZE] = {0} ;
//    while ( scanf ( "%s", szDestIp) )
//       printf("%d",Ping2 ( szDestIp ))  ;
//     
//    return 0;
//}

USHORT GetCheckSum(LPBYTE lpBuf, DWORD dwSize)
{
    DWORD dwCheckSum = 0;
    USHORT* lpWord = (USHORT*)lpBuf;

    while( dwSize > 1)
    {
        dwCheckSum += *lpWord++;
        dwSize -= 2;
    }

    if(1 == dwSize)
        dwCheckSum += *((USHORT*)lpBuf);

    dwCheckSum = ( dwCheckSum >> 16) + ( dwCheckSum & 0xffff);
    return (USHORT)(~dwCheckSum);
}

int Ping2(char* lpDestIp)
{
    SOCKADDR_IN DestAddr;
    DestAddr.sin_family = AF_INET;
    DestAddr.sin_addr.S_un.S_addr = inet_addr(lpDestIp);
    DestAddr.sin_port = htons(0);

    //创建ICMP请求包
    char ICMPPack[ICMP_PACK_SIZE] = {0};
    PICMP_HEADER pICMPHeader = (PICMP_HEADER)ICMPPack;
    pICMPHeader->nType = 8;
    pICMPHeader->nCode = 0;
    pICMPHeader->nId = (USHORT)::GetCurrentProcessId();
    pICMPHeader->nCheckSum = 0;
    pICMPHeader->nTimeStamp = 0;
    memset(&(ICMPPack[ICMP_HEADER_SIZE]),'E',ICMP_DATA_SIZE);

    //初始化WinSock
    WORD wVersionRequested = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(wVersionRequested,&wsaData) != 0)
    {
        return Gfault;
    }

    //创建初始套接字
    SOCKET RawSock = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
    if(INVALID_SOCKET == RawSock)
    {
        printf("create raw socket error\n");
        return Gfault;
    }

    int nTime = 1000;
	int alltime=0;
    int nRet = ::setsockopt( RawSock, SOL_SOCKET, SO_RCVTIMEO,(char*)(&nTime),sizeof(nTime));

    char szRecvBuf [ DEF_BUF_SIZE] ;
    SOCKADDR_IN    SourSockAddr ;

    for(int i = 0; i < NumAll; i++)
    {
        pICMPHeader->nCheckSum = 0;
        pICMPHeader->nTimeStamp = ::GetTickCount();
        pICMPHeader->nSequence = i;

        pICMPHeader->nCheckSum = GetCheckSum ( (LPBYTE)ICMPPack, ICMP_PACK_SIZE ) ;

        int nRet = ::sendto( RawSock, ICMPPack, ICMP_PACK_SIZE, 0, (SOCKADDR*)&DestAddr, sizeof(DestAddr));
        if ( nRet == SOCKET_ERROR )
        {
            printf ( "sendto error!\n" ) ;
            return Gfault ;
        }

        // 接收ICMP响应
        int nLen = sizeof(SourSockAddr) ;
        nRet = ::recvfrom ( RawSock, szRecvBuf, DEF_BUF_SIZE,0,(SOCKADDR*)&SourSockAddr, &nLen ) ;
        if ( nRet == SOCKET_ERROR )
        {
            if ( ::WSAGetLastError() == WSAETIMEDOUT )
            {
                printf ( "Request Timeout\n" ) ;
                 return Gfault ;
            }
            else
            {
                printf ( "recvfrom error!\n" ) ;
                return Gfault ;
            }
        }

        int nTime = ::GetTickCount() - pICMPHeader->nTimeStamp ;

        int nRealSize = nRet - IP_HEADER_SIZE - ICMP_HEADER_SIZE ;
        if ( nRealSize < 0  )
        {
            printf ( "To less recv bytes!\n" ) ;
            continue ;
        }

        // 检测是否当前所发出的ICMP响应包
        PICMP_HEADER pRecvHeader = (PICMP_HEADER)(szRecvBuf+IP_HEADER_SIZE) ;
        if ( pRecvHeader->nType != 0 )
        {
            printf ( "Not ICMP respond type!\n" ) ;
            return Gfault ;
        }

        if ( pRecvHeader->nId != ::GetCurrentProcessId () )
        {
            printf ( "not valid id!\n" ) ;
            return Gfault ;
        }

      //  printf ( "%d bytes replay from %s : bytes=%d time=%dms\n", \
          //  nRet, inet_ntoa(SourSockAddr.sin_addr), nRealSize, nTime ) ;
		alltime=alltime+nTime;
        ::Sleep (TimeSpace ) ;
    }

    closesocket ( RawSock ) ;
    WSACleanup () ;

    return alltime/ NumAll ;
}