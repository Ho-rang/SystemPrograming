#include <iostream>
#include <Windows.h>
#include <string>

int main()
{
	const std::string filename = "setting.ini";

	const std::string section = "JSON";
	const std::string key = "derectory";

	char value[256];
	GetPrivateProfileStringA(section.c_str(), key.c_str(), "", value, 256, filename.c_str());

	std::cout << "Value : " << value << std::endl;


    return 0;
}