#pragma once
#include <string>
#include <vector>

#include "istreamwrapper.h"
#include "document.h"
#include <Windows.h>

class WatchJSON
{
private:
	std::string path;
	std::vector<std::string> jsonList;
	int nowSelectIndex;

	FILETIME lastDirectoryWriteTime;

	FILETIME lastJsonFileWriteTime;
public:
	WatchJSON();
	~WatchJSON();
	void LoadIni();
	void LoadDirectory();
	bool CheckUpdateDirectory();

	void SelectJsonFile();

	void LoadJsonFile();
};

