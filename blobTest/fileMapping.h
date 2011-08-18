#ifndef __FILEMAPPING__
#define __FILEMAPPING__
#include <windows.h>

class fileMapping
{
private:
	HANDLE hMap;
	HANDLE hf;
	BYTE* pkBuff ;
public:
	fileMapping():hMap(INVALID_HANDLE_VALUE), hf(INVALID_HANDLE_VALUE),pkBuff(0)
	{}

	fileMapping(const wchar_t* szFileName);
	~fileMapping()
	{
		UnmapViewOfFile( pkBuff );
		CloseHandle( hMap);
		CloseHandle( hf );
	}

	template<class T>
	T* GetBuffer()
	{
		return (T*)pkBuff ;
	}

	unsigned long GetBufferSize()
	{
		return GetFileSize(hf,NULL);
	}
};

#endif