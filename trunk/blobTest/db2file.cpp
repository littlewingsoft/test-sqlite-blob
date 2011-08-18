#include <stdafx.h>
#include <windows.h>
#include <vector>
#include <string>

using namespace std;

static sqlite3 *db ;
static sqlite3_stmt *stmt ;
static int n = 0, nRet = 0;
static char szQuery[0xFF];


vector<wstring> tokenize(const wstring& str,const wstring& delimiters)
{
	vector<wstring> tokens;
    	
	// skip delimiters at beginning.
   	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    	
	// find first "non-delimiter".
    	string::size_type pos = str.find_first_of(delimiters, lastPos);

    	while (string::npos != pos || string::npos != lastPos)
    	{
        	// found a token, add it to the vector.
        	tokens.push_back(str.substr(lastPos, pos - lastPos));
		
        	// skip delimiters.  Note the "not_of"
        	lastPos = str.find_first_not_of(delimiters, pos);
		
        	// find next "non-delimiter"
        	pos = str.find_first_of(delimiters, lastPos);
    	}

	return tokens;
}
//path 쪼개서 경로만 빼낸후 모두 분리후에 존재하는지 여부 검색.
//있다면 건너띄고 없다면 folder 생성.
void checkUsableFolder(const wchar_t* szFullPath)
{
	wchar_t drive[16];
	wchar_t dir[256];
	wchar_t filename[256];
	wchar_t ext[32];

	_wsplitpath_s( szFullPath,drive,16, dir,256, filename,256,  ext,32 );

	vector<wstring> tok = tokenize( dir, L"\\" );

	wstring tmp = wstring(drive);
	for( size_t n= 0 ; n< tok.size(); n++)
	{
		tmp = tmp + L"\\" + tok[n];
		
		CreateDirectory( tmp.c_str(), NULL );
	}
}



bool deflateChunk(const wchar_t* szFullPath, const void* ChunkData, size_t ChunkSize )
{
	checkUsableFolder(szFullPath );
	FILE* fp=NULL;
	errno_t ret = _wfopen_s( &fp, szFullPath, L"wb+" );
	if( ret != 0 )
	{
		printf( "[X] fopen_s( %s )", szFullPath );
		return false;
	}

	fwrite( ChunkData, ChunkSize, 1, fp ); 

	fclose( fp );
	return true;
}

int setData(void* data,int cols, char** value, char** name) 
{

    for (int i = 0; i < cols; i++) 
	{
        printf( "%s, %s", value[i], name[i]);
    }
    return 0;
}

//db에서 szTargetDir 밑에다가 모조리 풀기. 풀기속도 측정.
int DB2File( const char* szDBPathName, const TCHAR* szTargetDir )
{
	FILE* fp=0;
	fopen_s( &fp, "db2file.log", "wt+");
	time_t start = time( 0 );

    if ( sqlite3_open( szDBPathName, &db) != SQLITE_OK )
    {
        printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(db));
        return -1;
    }
	time_t elapsed = time( 0 ) - start;
	fprintf( fp, "openTime: %d\n", elapsed );
	start = time( 0 );

	char* errMsg;
	sqlite3_exec( db,"SELECT count(*) FROM fileChunkTable;" ,setData, 0,&errMsg );


    // step 3. select dummy blob value
    n = sprintf_s(szQuery, 255,"SELECT * FROM fileChunkTable;");
    if ( sqlite3_prepare(db, szQuery, n, &stmt, NULL) != SQLITE_OK )
    {
        printf("fail to select, %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    do
    {
        switch ( (nRet = sqlite3_step(stmt)) )
        {
        case SQLITE_DONE:
            break;
        case SQLITE_BUSY:
            printf("database is busy.\n");
            break;
        case SQLITE_LOCKED:
            printf("database is locked.\n");
            break;
        case SQLITE_ROW:
			{
			//printf("id column value  : %d\n", sqlite3_column_int(stmt, 0));
            //printf("id column length : %d\n", sqlite3_column_bytes(stmt, 0));

            //printf("filePath column length : %d\n", sqlite3_column_bytes(stmt, 1));
			//printf("fileChunk(BLOB) column length : %d\n", sqlite3_column_bytes(stmt, 1));
            //printf("filePath(TEXT) column value  : %s\n", sqlite3_column_text(stmt, 2));

            
            //printf("fileChunk(BLOB) column value  : %s\n", sqlite3_column_blob(stmt, 2) );

			wchar_t uni[256];
			int wlen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt, 2) , uni, 256);
			uni[ wlen ] ='\0';
			
			wchar_t szBuff[256];
			swprintf_s( szBuff,256, L"%s\\%s", szTargetDir, uni );
			//deflateChunk( szBuff, (const void*)sqlite3_column_blob(stmt, 1), sqlite3_column_bytes(stmt, 1) );
			}
            break;
        default:
            printf("sqlite3_step return '%d'\n", nRet);
            break;
        }

    } while ( nRet == SQLITE_ROW );
    
	elapsed = time( 0 ) - start;
	fprintf( fp, "queryTime: %d\n" , elapsed );

    if ( stmt )
        sqlite3_finalize(stmt);

    sqlite3_close(db);



	fclose(fp );

	return 0;
}
