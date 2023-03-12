#include "WatchJSON.h"
#include <stdlib.h>

int main()
{
	WatchJSON watchJosn;

	watchJosn.LoadIni();
	
	while(true)
	{
		system("cls");
		watchJosn.LoadDirectory();

		watchJosn.SelectJsonFile();
		while(watchJosn.CheckUpdateDirectory())
		{
			watchJosn.LoadJsonFile();
			Sleep(1);
		}
		Sleep(1);
	}

	return 0;
}