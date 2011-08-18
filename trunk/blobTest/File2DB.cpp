#include <stdafx.h>
#include <string>
#include <windows.h>
#include "fileMapping.h"

static sqlite3 *db ;

static int n = 0, nRet = 0;
static char szQuery[0xFF];

extern std::wstring TargetDir;
typedef std::basic_string<TCHAR> tstring;
typedef void (*CALLBACK_ARG)(const tstring& szFullPath, const tstring& keyfileName);
void FindFiles(const TCHAR* path, CALLBACK_ARG pkCallBackFunc )
{
	setlocale(LC_ALL, ".OCP");
	char* ld_locale = setlocale (LC_ALL, NULL);
	//setlocale (LC_ALL, "");
	//ld_locale = setlocale (LC_ALL, NULL);

    HANDLE hSearch = NULL;
    WIN32_FIND_DATA wfd = {0};
    BOOL bResult = TRUE;
    wchar_t spath[MAX_PATH] = {0};
    wchar_t File[MAX_PATH] = {0};
    wchar_t Drive[_MAX_DRIVE] = {0};
    wchar_t Dir[MAX_PATH] = {0};

    hSearch = FindFirstFile( path, &wfd);

    if(hSearch == INVALID_HANDLE_VALUE)    return;

    _tsplitpath_s( path,Drive,_MAX_DRIVE,Dir,MAX_PATH,NULL,NULL,NULL,NULL);

    while(bResult)
    {
        if( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
			if( std::wstring(wfd.cFileName)!= L"." && std::wstring(wfd.cFileName) != L".." )
            {
                swprintf_s(spath,MAX_PATH, L"%s%s%s\\*.*",Drive,Dir,wfd.cFileName);
                FindFiles(spath , pkCallBackFunc);
            }
        }
        else
        {
			swprintf_s(spath,MAX_PATH, L"%s%s%s",Drive,Dir,wfd.cFileName); //wide 형식의 문자로. 일단 mbcs로 바꾸진 말자.

			std::wstring ws = spath;
			ws = ws.substr( TargetDir.length() );

			//char mbcs[256];
			//WideCharToMultiByte(CP_ACP, 0, spath, -1, mbcs, 256, NULL, NULL);

			//target 경로를 제외한 경로.

			pkCallBackFunc( spath , ws );//wfd.cFileName,
        //            wsprintfA(File,wfd.cFileName);
            //SendMessageA(hListBox,LB_ADDSTRING,NULL,LPARAM(wfd.cFileName));
        }

        bResult = FindNextFile(hSearch,&wfd);
    }


    FindClose(hSearch);
}

/**
    test table (named 'foo') scheme is follows	
    +----------+-----------------+----------------+
    | id (int) | fileName (text) |  fileChunk (BLOB)  |
    +----------+-----------------+----------------+
    |        0 |            aaaa |   010101010    |
    +----------+-----------------+----------------+
    |        1 |            bbbb |   010101010    |
    +----------+-----------------+----------------+
*/

void insertQry( const tstring& szFullPath, const tstring& Key_subDirFileName )//const char* szFileName,
{
	sqlite3_stmt *stmt ;
   // step 2. insert dummy blob value
	//printf( "INSERT OR IGNORE INTO fileChunkTable (fileChunk) VALUES (?);\n" );
    n = sprintf_s(szQuery,255, "INSERT OR IGNORE INTO fileChunkTable (fileName,fileChunk) VALUES (?,?);"); //
    if ( sqlite3_prepare(db, szQuery, n, &stmt, NULL) != SQLITE_OK )
    {
        printf("fail to sqlite3_prepare, %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }

	try
	{

/*		wchar_t uni[256];
		int wlen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFullPath, strlen(szFullPath), uni, 256);
		uni[ wlen ] ='\0';*/
		fileMapping fm( szFullPath.c_str() );// uni

		//path insert
		sqlite3_reset(stmt);
		char mbcs[256];
		WideCharToMultiByte(CP_ACP, 0, Key_subDirFileName.c_str(), -1, mbcs, 256, NULL, NULL);
		sqlite3_bind_text( stmt,1, (const char*)mbcs, -1, NULL ) ;

		// chunk insert
		sqlite3_reset(stmt);
		const void* szblob = fm.GetBuffer<const void*>();
		int ret = sqlite3_bind_blob( stmt, 2, (const void *)szblob , fm.GetBufferSize(), NULL);
		if( SQLITE_OK == ret  )
			sqlite3_step(stmt);
		
		//printf( "[complete] %s  \n", mbcs );
	}
	catch( const char* szComment )
	{
		printf( "[X] %s %s \n", szComment, szFullPath );
	}


    if ( sqlite3_finalize(stmt) != SQLITE_OK )
    {
		printf("[X] sqlite3_finalize, %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return ;
    }	
}



//TargetDir 안의 모든 file과 서브폴더를 재귀호출하여 db 로 삽입
int File2DB(const TCHAR* szTargetDir, const char* szDBPathName )
{
	time_t start = time( 0 );

    if ( sqlite3_open( szDBPathName, &db) != SQLITE_OK )
    {
        printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // step 1. create dummy table
    n = sprintf_s(szQuery, 255, "CREATE TABLE fileChunkTable (id INTEGER PRIMARY KEY AUTOINCREMENT, fileChunk BLOB, fileName TEXT);");
    if ( sqlite3_exec(db, szQuery, NULL, NULL, NULL) != SQLITE_OK )
    {
		if( sqlite3_exec(db, "delete FROM fileChunkTable", NULL, NULL, NULL) != SQLITE_OK )
		{
			printf("[X] delete FROM fileChunkTable, %s \n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return -1;
		}
        //printf("fail to create table, %s\n", sqlite3_errmsg(db));
        //sqlite3_close(db);
        //return -1;
    }

	FindFiles( szTargetDir, insertQry );

    sqlite3_close(db);

	time_t elapsed = time( 0 ) - start;
	FILE* fp=0;
	fopen_s( &fp, "db2file.log", "wt+");
	fprintf( fp, "elapsed: %d\n", elapsed );
	fclose(fp );

	return 0;
}
