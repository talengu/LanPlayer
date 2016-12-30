#include<stdio.h>
#include <windows.h>

#include<io.h>//使用io.h头文件中声明的_findfirst(),_findnext()函数

#define PATH "*.mp3"//调试时用
//#define PATH "*.mp3"

////////////////////////////////////重要函数
void Mp3FinalList(char *p[200]);
//////////////////////////////////


//获取当前目录下mp3的名字
int Nameofmp3CurrentPath(char *mp3currentpathall[200])
{
	//最多200个MP3文件
	long file;
	struct _finddata_t find;
	int num=0;

	while((file=_findfirst(PATH, &find))==-1L)
	{
		printf("Null!No mp3\n");
		printf("Put mp3 in current path,then press Enter\n");
		fflush(stdin);
		getchar();
	}

	mp3currentpathall[0]=(char *)malloc(sizeof(find.name));
	strcpy(mp3currentpathall[0], find.name);
	while(_findnext(file, &find)==0)
	{
		num++;
		mp3currentpathall[num]=(char *)malloc(sizeof(find.name));
		strcpy(mp3currentpathall[num], find.name);
		//printf("%d:%s\n", num,find.name);

	}
	_findclose(file);
	return num;
}
//将文件打印
void Mp3printf(int num,char*p[])
{
	printf("---------------------\n");
	for(int i=0;i<=num;i++)
		printf("%d.%s\n",i,p[i]);
	printf("---------------------\n");
}
//对mp3顺序进行排序
void Mp3LisPpai(int ch,int num,char*p[])
{
	if(ch<num&&ch>=0)
	{
		char *temp=p[ch];
	for(int i=0;i<ch;i++)
		p[ch-i]=p[ch-1-i];
	p[0]=temp;
	}
	else
		printf("0-%d\n",num);
}
//重要的函数
void Mp3FinalList(char *p[200])
{
	int num=Nameofmp3CurrentPath(p);
	int ch=0;
	Mp3printf(num,p);
	while(ch!=-1)
	{    
		fflush(stdin);
		printf("Input -1 confirm>>");
		scanf("%d",&ch);
		Mp3LisPpai(ch,num,p);
		Mp3printf(num,p);
	}
}