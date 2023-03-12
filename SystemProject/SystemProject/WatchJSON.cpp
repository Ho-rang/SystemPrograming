#include "WatchJSON.h"

#include <iostream>

#include <cstdlib>
#include <fstream>
#include <conio.h>

bool WatchJSON::CheckUpdateDirectory()
{
	HANDLE hFile = CreateFileA(
		path.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "디렉토리 불러오기 실패" << path << std::endl;
		return false;
	}

	// 디렉토리 시간 알아오기
	FILETIME lastWriteTime = { 0 };
	GetFileTime(hFile, NULL, NULL, &lastWriteTime);

	if (CompareFileTime(&this->lastDirectoryWriteTime, &lastWriteTime) != 0)
	{
		this->lastDirectoryWriteTime = lastWriteTime;
		this->lastJsonFileWriteTime = { 0 };
		CloseHandle(hFile);
		return false;
	}
	else
	{
		this->lastDirectoryWriteTime = lastWriteTime;
		CloseHandle(hFile);
		return true;
	}
}

WatchJSON::WatchJSON()
	:path(""), nowSelectIndex(-1),
	lastJsonFileWriteTime({ 0 }),
	lastDirectoryWriteTime({ 0 })
{

}

WatchJSON::~WatchJSON()
{

}

void WatchJSON::LoadIni()
{
	char value[256];
	//						section		key											path
	GetPrivateProfileStringA("JSON", "directory", "error", value, sizeof(value), "./setting.ini");

	this->path = value;
	std::cout << "Path : " << path << std::endl;

	if (path.length() == 0)
	{
		exit(1);
	}
}

void WatchJSON::LoadDirectory()
{
	CheckUpdateDirectory();

	std::string directoryPath = path + "*.*";

	this->jsonList.clear();
	/// 읽어온 경로의 Directory 목록 하나씩 불러오기
	WIN32_FIND_DATAA fileData;
	HANDLE hDirectory = FindFirstFileA(directoryPath.c_str(), &fileData); // 검색을 시작합니다.

	if (hDirectory == INVALID_HANDLE_VALUE)
	{
		std::cout << "디렉토리 목록 불러오기 실패. path : " << directoryPath << std::endl;
		exit(1);
	}

	std::cout << std::endl << "디렉토리 목록" << std::endl;

	do
	{
		std::string name = fileData.cFileName;

		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::cout << name << std::endl; // 이건 디렉토리
			continue;
		}

		std::cout << name << std::endl; // 파일 이름 출력

		// .json 파일 따로 저장
		if (name.length() > 5 && name.substr(name.length() - 5) == ".json")
		{
			jsonList.push_back(name);
		}

	} while (FindNextFileA(hDirectory, &fileData) != 0); // 다음 파일 검색

	FindClose(hDirectory);
}

void WatchJSON::SelectJsonFile()
{
	if (jsonList.empty())
	{
		return;
		std::cout << ".json 파일이 없습니다." << std::endl;
		exit(1);
	}

	std::cout << std::endl << "===== .json 목록 =====" << std::endl;
	for (auto i = 0; i < jsonList.size(); i++)
	{
		std::cout << i + 1 << ". " << jsonList[i] << std::endl;
	}

	do
	{
		std::cout << std::endl << "로드할 파일 선택 : ";
		std::cin >> nowSelectIndex;
	} while (nowSelectIndex <= 0 || nowSelectIndex > jsonList.size());

	//index에 맞춰서 1빼주기
	nowSelectIndex -= 1;
}

void WatchJSON::LoadJsonFile()
{
	using namespace rapidjson;

	WIN32_FIND_DATAA fileData;
	HANDLE hFile = NULL;

	std::string fileName = path + jsonList[nowSelectIndex];
	hFile = CreateFileA(
		fileName.c_str(),
		GENERIC_READ ,
		FILE_SHARE_READ ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
		std::cout << "json 파일 열기 실패" << fileName << std::endl;
		exit(1);
	}

	FILETIME lastWriteTime = { 0 };
	if (!GetFileTime(hFile, NULL, NULL, &lastWriteTime))
	{
		std::cout << ".json 파일 시간 불러오기 실패 : " << GetLastError() << std::endl;
		CloseHandle(hFile);
		return;
	}

	CloseHandle(hFile);

	if (CompareFileTime(&this->lastJsonFileWriteTime, &lastWriteTime) == 0)
	{
		return;
	}

	while (true)
	{
		if (lastJsonFileWriteTime.dwLowDateTime == 0)
			break;

		std::cout << jsonList[nowSelectIndex] << " 파일이 수정되었습니다. 스페이스를 누르면 다시 로드 합니다." << std::endl;
		if (_getch() == ' ')
		{
			break;
		}
	}

	this->lastJsonFileWriteTime = lastWriteTime;


	std::ifstream file(fileName);
	IStreamWrapper isw(file);

	Document document;
	document.ParseStream(isw);

	if (document.HasParseError())
	{
		std::cout << "json 파싱 실패" << std::endl;
		exit(1);
	}

	for (auto iter = document.MemberBegin(); iter != document.MemberEnd(); ++iter)
	{
		std::cout << "Key : " << iter->name.GetString() << std::endl;

		std::cout << "Value : ";
		auto& value = iter->value;
		if (value.IsString())
			std::cout << value.GetString() << std::endl;
		else if (value.IsInt())
			std::cout << value.GetInt() << std::endl;
		else if (value.IsDouble())
			std::cout << value.GetDouble() << std::endl;
		else if (value.IsBool())
			std::cout << (value.GetBool() ? "true" : "false") << std::endl;
		else if (value.IsNull())
			std::cout << "null" << std::endl;
		else
			std::cout << "Unknown type" << std::endl;

		std::cout << std::endl;
	}
}
