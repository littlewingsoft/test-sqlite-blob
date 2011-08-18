
#include <stdafx.h>
#include <windows.h>
#include <string>

using namespace std;

wstring TargetDir( _T("C:\\Documents and Settings\\a\\My Documents\\Downloads\\TorrentSeedFiles") );
extern int File2DB(const TCHAR* szTargetDir, const char* szDBPathName );
extern int DB2File(const char* szDBPathName, const TCHAR* szTargetDir );
extern int testPacketDb();
// blobTest.exe aaaa.db rootDir
int main(int argc, char **argv)
{
	
	if( argc <= 1 ) 
	{
		printf( ":: Usage ::\n" 
				"blobTest dbName TargetDir\n\n"
				"ex)\n"
				"blobTest c:\\dbFolder\\test.db c:\\TargetDir \n"
			);
		return -1;
	}
	//char* ld_locale = setlocale (LC_ALL, NULL);
	//setlocale (LC_ALL, ""	);//ks_c_5601-1987
	//ld_locale = setlocale (LC_ALL, NULL);

	printf( "argv[1]= %s \nargv[2]= %s\n", argv[1], argv[2] );
	
	wchar_t wTargetDir[256];
	int wlen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[2], strlen(argv[2]), wTargetDir, 256);
	wTargetDir[ wlen ] ='\0';
	TargetDir = wTargetDir;
	const char* szdbName = argv[1]; //"test.db"; //":memory:";


//#define FILE2DB
//#define DB2FILE
//#define DB2FILE

#ifdef FILE2DB
	 //"C:/Documents and Settings/a/My Documents/Downloads/Game"
	return File2DB( wTargetDir, szdbName );
#elif DB2FILE
	//"C:\\Documents and Settings\\a\\My Documents\\Downloads\\Game"
	return DB2File( szdbName, wTargetDir );
#else
	testPacketDb();
#endif

}