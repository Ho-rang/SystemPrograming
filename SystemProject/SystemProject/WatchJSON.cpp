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
		std::cout << "���丮 �ҷ����� ����" << path << std::endl;
		return false;
	}

	// ���丮 �ð� �˾ƿ���
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
	/// �о�� ����� Directory ��� �ϳ��� �ҷ�����
	WIN32_FIND_DATAA fileData;
	HANDLE hDirectory = FindFirstFileA(directoryPath.c_str(), &fileData); // �˻��� �����մϴ�.

	if (hDirectory == INVALID_HANDLE_VALUE)
	{
		std::cout << "���丮 ��� �ҷ����� ����. path : " << directoryPath << std::endl;
		exit(1);
	}

	std::cout << std::endl << "���丮 ���" << std::endl;

	do
	{
		std::string name = fileData.cFileName;

		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			std::cout << name << std::endl; // �̰� ���丮
			continue;
		}

		std::cout << name << std::endl; // ���� �̸� ���

		// .json ���� ���� ����
		if (name.length() > 5 && name.substr(name.length() - 5) == ".json")
		{
			jsonList.push_back(name);
		}

	} while (FindNextFileA(hDirectory, &fileData) != 0); // ���� ���� �˻�

	FindClose(hDirectory);
}

void WatchJSON::SelectJsonFile()
{
	if (jsonList.empty())
	{
		return;
		std::cout << ".json ������ �����ϴ�." << std::endl;
		exit(1);
	}

	std::cout << std::endl << "===== .json ��� =====" << std::endl;
	for (auto i = 0; i < jsonList.size(); i++)
	{
		std::cout << i + 1 << ". " << jsonList[i] << std::endl;
	}

	do
	{
		std::cout << std::endl << "�ε��� ���� ���� : ";
		std::cin >> nowSelectIndex;
	} while (nowSelectIndex <= 0 || nowSelectIndex > jsonList.size());

	//index�� ���缭 1���ֱ�
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
		std::cout << "json ���� ���� ����" << fileName << std::endl;
		exit(1);
	}

	FILETIME lastWriteTime = { 0 };
	if (!GetFileTime(hFile, NULL, NULL, &lastWriteTime))
	{
		std::cout << ".json ���� �ð� �ҷ����� ���� : " << GetLastError() << std::endl;
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

		std::cout << jsonList[nowSelectIndex] << " ������ �����Ǿ����ϴ�. �����̽��� ������ �ٽ� �ε� �մϴ�." << std::endl;
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
		std::cout << "json �Ľ� ����" << std::endl;
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
