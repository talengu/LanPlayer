#include"HandlesServer.h"
#include"HandlesClient.h"
void gprint()
{
	printf("Server  OR  Client\n");
	printf("---------------------------\n");
	printf("s   server                 \n");
	printf("c   client                \n");
	printf("@   pass                   \n");
	printf("q   prepare                \n");
	printf("w   start or resume        \n");
	printf("e   pause                  \n");
	printf("r   set point              \n");
	printf("g   get position           \n");
	printf("l   get length             \n");
	printf("---------------------------\n");
}
int main()
{
	//printf("**\n本程序不用设定服务器的地址（局域网的地址)\n不用改mp3名字\nStep1:选择server或client\nStep2:p传输数据\n      z读入内存准备播放\n      a播放\n");
	//printf("      k各个client的播放点\n      x关闭音乐\n      e使client退出\n**\n");
	gprint();
	char key=' ';
	while(key!='s'&&key!='c')
	{    
		fflush(stdin);
		
		printf("Choose :[s/c]");
		scanf("%c",&key);
	}
	if(key=='s')
	{
		printf("Start Server...\nWaiting Client...\n");
		lserver();
	}
	if(key=='c')
	{
		printf("Start Client...\nWaiting Server...\n");
		lclient();
	}

	return 0;
}




