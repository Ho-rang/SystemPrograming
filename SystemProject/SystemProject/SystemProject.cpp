#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>

#include "rapidjson.h"

int main()
{
	/// ini 파일에서 Json directory 위치 읽어오기
	char value[256];
	//						section		key											path
	GetPrivateProfileStringA("JSON", "directory", "error", value, sizeof(value), "./setting.ini");
	std::string path = value;

	std::cout << "Value : " << path << std::endl;
	path += "/*.*";

	/// 읽어온 경로의 directory 정보 불러오기
	WIN32_FIND_DATAA fileData;
 	HANDLE hFind = FindFirstFileA(path.c_str(), &fileData); // 검색을 시작합니다.

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				std::cout << fileData.cFileName << std::endl; // 이건 디렉토리
			}
			else
			{
				std::cout << fileData.cFileName << std::endl; // 파일 이름을 출력합니다.
			}
		} while (FindNextFileA(hFind, &fileData) != 0); // 다음 파일을 검색합니다.

		FindClose(hFind); // 검색 핸들을 닫습니다.
	}
	else 
	{
		std::cout << "디렉토리 불러오기 실패" << std::endl;
	}

	


	return 0;
}