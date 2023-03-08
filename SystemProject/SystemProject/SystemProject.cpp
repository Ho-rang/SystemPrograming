#include <iostream>
#include <Windows.h>
#include <string>
#include "rapidjson.h"

int main()
{
	const std::string filename = "./setting.ini";

	const std::string section = "JSON";
	const std::string key = "directory";

	char value[256];
	GetPrivateProfileStringA(section.c_str(), key.c_str(), "error", value, 256, filename.c_str());

	std::cout << "Value : " << value << std::endl;
	const int MAX_BUFFER = 255;
	char buffer[MAX_BUFFER];
	GetPrivateProfileSectionNamesA(buffer, MAX_BUFFER, "./setting.ini");


	char sectionNames[4096];
	DWORD bytesRead = GetPrivateProfileSectionNamesA(sectionNames, 4096, "./setting.ini");
	if (bytesRead == 0)
	{
		std::cout << "Error: Failed to read section names from INI file." << std::endl;
		return 1;
	}
	std::cout << "Section names in INI file: " << std::endl;
	char* pSectionName = sectionNames;
	while (*pSectionName)
	{
		std::cout << pSectionName << std::endl;
		pSectionName += strlen(pSectionName) + 1;
	}

	//GetPrivateProfileStringA(NULL, NULL, "None Section", buffer, 4096, filename.c_str());
	/*std::string sectionNames(buffer);
	size_t currentPos = 0;
	size_t previousPos = 0;

	while ((currentPos = sectionNames.find('\0', previousPos)) != std::string::npos)
	{
		std::string sectionName = sectionNames.substr(previousPos, currentPos - previousPos);
		previousPos = currentPos + 1;

		const int MAX_BUFFER2 = 1024;
		char buffer2[MAX_BUFFER2];
		GetPrivateProfileSectionA(sectionName.c_str(), buffer2, MAX_BUFFER2, "./setting.ini");

		std::string sectionData(buffer2);
		size_t currentPos2 = 0;
		size_t previousPos2 = 0;

		while ((currentPos2 = sectionData.find('\0', previousPos2)) != std::string::npos)
		{
			std::string data = sectionData.substr(previousPos2, currentPos2 - previousPos2);
			previousPos2 = currentPos2 + 1;

			size_t equalPos = data.find('=');
			std::string key = data.substr(0, equalPos);
			std::string value = data.substr(equalPos + 1);

			std::cout << "Section Name: " << sectionName << std::endl;
			std::cout << "Key: " << key << " Value: " << value << std::endl;
		}
	}*/

	return 0;
}