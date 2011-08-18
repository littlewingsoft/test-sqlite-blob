#include "fileMapping.h"
#include <locale.h>
	fileMapping::fileMapping(const wchar_t* szFileName)
	{
		hf = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if( hf == INVALID_HANDLE_VALUE)
			throw "file handle INVALID_HANDLE_VALUE";

		hMap = CreateFileMapping( hf, NULL, PAGE_READONLY, 0, 0, NULL);
		if( hMap == 0 )
		{
			throw "CreateFileMapping INVALID_HANDLE_VALUE";
		}

		pkBuff = (BYTE*)MapViewOfFile( hMap, FILE_MAP_READ, 0, 0, 0);//FILE_MAP_ALL_ACCESS

	}
