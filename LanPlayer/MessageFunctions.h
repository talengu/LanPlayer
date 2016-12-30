//TEST_OK

#ifndef _GTCMASSAGEFUNCTION_
#define _GTCMASSAGEFUNCTION_


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define isTestMode//去掉Test的Message
#define isErrorMessage//去掉Error的Message

int __cdecl TestMessageprint(const char *format,...);
int __cdecl ErrorMessageprint(const char *format,...);

/*******************************************************************************
* 函数名        : int __cdecl TestMessageprint(const char *format,...)
* 函数描述    	: TestMode时的输出函数（自带"\n"换行）
* 输入参数      : const char *format,...与printf一样使用
* 输出结果     	: 打印字符串
* 返回值        : 总体字符串长度stringsize                            TEST_OK
*******************************************************************************/

int __cdecl TestMessageprint(const char *format,...)
{
#if defined(isTestMode)
	va_list args;char p[256];int stringsize;
	printf("TestMessage: ");
	//Here is a problem:when use 'char *p' ,it will be error.
	//Attention!!!MAX TestMessage SIZE is 256! 
	//Donnot need add '\n'.The function already owns.
	va_start(args,format);
	stringsize=vsprintf(p,format,args);
	va_end(args);
	printf("%s\n",p);
	//return stringsize;
#endif
	return 0;
}

int __cdecl ErrorMessageprint(const char *format,...)
{
#if defined(isErrorMessage)
	va_list args;char p[256];
	printf("ErrorMessage: ");
	//Here is a problem:when use 'char *p' ,it will be error.
	//Attention!!!MAX TestMessage SIZE is 256! 
	//Donnot need add '\n'.The function already owns.
	va_start(args,format);
	vsprintf(p,format,args);
	va_end(args);
	printf("%s!Program stop.\n",p);
	//enable errormessages to be seen
	//system("pause");
	//return -1;
#endif
	return 0;
}
#endif