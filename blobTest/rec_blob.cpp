#include <stdafx.h>
#include <string>
//-----------------------------------------------------------------------------

static DWORD lastTime = 0; // 가장최근 페킷이 추가될때의 시간.
                           // 이전페킷과의 시간간격 = 현재시간 - lastTime 

static	sqlite3 *db = NULL;
static	sqlite3_stmt *stmt = NULL;

//-----------------------------------------------------------------------------

/*
** This function is used to load the contents of a database file on disk 
** into the "main" database of open database connection pInMemory, or
** to save the current contents of the database opened by pInMemory into
** a database file on disk. pInMemory is probably an in-memory database, 
** but this function will also work fine if it is not.
**
** Parameter zFilename points to a nul-terminated string containing the
** name of the database file on disk to load from or save to. If parameter
** isSave is non-zero, then the contents of the file zFilename are 
** overwritten with the contents of the database opened by pInMemory. If
** parameter isSave is zero, then the contents of the database opened by
** pInMemory are replaced by data loaded from the file zFilename.
**
** If the operation is successful, SQLITE_OK is returned. Otherwise, if
** an error occurs, an SQLite error code is returned.
*/
	static int loadOrSaveDb(sqlite3 *pInMemory, const char *zFilename, int isSave)
	{
		int rc;                   /* Function return code */
		sqlite3 *pFile;           /* Database connection opened on zFilename */
		sqlite3_backup *pBackup;  /* Backup object used to copy data */
		sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
		sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */

		/* Open the database file identified by zFilename. Exit early if this fails
		** for any reason. */
		rc = sqlite3_open(zFilename, &pFile);
		if( rc==SQLITE_OK ){

			/* If this is a 'load' operation (isSave==0), then data is copied
			** from the database file just opened to database pInMemory. 
			** Otherwise, if this is a 'save' operation (isSave==1), then data
			** is copied from pInMemory to pFile.  Set the variables pFrom and
			** pTo accordingly. */
			pFrom = (isSave ? pInMemory : pFile);
			pTo   = (isSave ? pFile     : pInMemory);

			/* Set up the backup procedure to copy from the "main" database of 
			** connection pFile to the main database of connection pInMemory.
			** If something goes wrong, pBackup will be set to NULL and an error
			** code and  message left in connection pTo.
			**
			** If the backup object is successfully created, call backup_step()
			** to copy data from pFile to pInMemory. Then call backup_finish()
			** to release resources associated with the pBackup object.  If an
			** error occurred, then  an error code and message will be left in
			** connection pTo. If no error occurred, then the error code belonging
			** to pTo is set to SQLITE_OK.
			*/
			pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
			if( pBackup ){
				(void)sqlite3_backup_step(pBackup, -1);
				(void)sqlite3_backup_finish(pBackup);
			}
			rc = sqlite3_errcode(pTo);
		}

		/* Close the database connection opened on database file zFilename
		** and return the result of this function. */
		(void)sqlite3_close(pFile);
		return rc;
	}


namespace rec_blob
{

	void reset_time()
	{
		lastTime = timeGetTime();
	}

	//-----------------------------------------------------------------------------

	void start()
	{
		reset_time();

		int nRet = 0;

		if ( sqlite3_open( ":memory:", &db) != SQLITE_OK )//:memory:
		{
			printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(db));
			return ;
		}

		char szQuery[0xFF];
		//	int n = sprintf_s(szQuery, 255, "CREATE TABLE Table_Packet(id INTEGER PRIMARY KEY AUTOINCREMENT,type int, desc varchar(32),  pkt BLOB );");
		int n = sprintf_s(szQuery, 0xFF, "CREATE TABLE tb_packet(rec_id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"timeStamp INTEGER,  pkt BLOB );");
		if ( sqlite3_exec(db, szQuery, NULL, NULL, NULL) != SQLITE_OK )
		{

			printf("fail to create table, %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}

	}

	//-----------------------------------------------------------------------------

	void add(	const void* blob, size_t size_blob )
	{
		DWORD currTime = timeGetTime();
		DWORD elapsedTime = currTime - lastTime ;
		lastTime = currTime;
		sqlite3_stmt *stmt = NULL;
		const char* szQuery = "INSERT INTO tb_packet (timeStamp, pkt) VALUES (?,?);";

		if ( sqlite3_prepare(db, szQuery, strlen(szQuery), &stmt, NULL) != SQLITE_OK )
		{
			printf("fail to sqlite3_prepare, %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
		int ret = 0;
		ret = sqlite3_bind_int( stmt, 1, elapsedTime );
		if( SQLITE_OK != ret  )
		{
			printf("fail to sqlite3_bind_blob, %s\n", sqlite3_errmsg(db));
		}

		ret = sqlite3_bind_blob( stmt, 2, (const void *)blob, size_blob, NULL);
		if( SQLITE_OK == ret  )
			sqlite3_step(stmt);
		else
		{
			printf("fail to sqlite3_bind_blob, %s\n", sqlite3_errmsg(db));
		}
		sqlite3_finalize( stmt );
	}

	//-----------------------------------------------------------------------------

	void* get( DWORD& rec_id, DWORD& dwElapsedTime )
	{
		char szQr[256]={0,};

		int n = sprintf_s(szQr, 255, "select * from tb_packet where rec_id > %d ", rec_id );
		sqlite3_stmt *stmt = NULL;
		if ( sqlite3_prepare(db, szQr, strlen(szQr), &stmt, NULL) != SQLITE_OK )
		{
			printf("fail to select, %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return 0;
		}

		int nRet = 0;
		do
		{
			nRet = sqlite3_step(stmt);
			switch ( nRet )
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
					rec_id = sqlite3_column_int(stmt,0) ;
					dwElapsedTime = sqlite3_column_int(stmt,1) ;
					void* tmp = (void*)sqlite3_column_blob(stmt, 2) ;
					return tmp;
				}
				break;
			default:
				printf("sqlite3_step return '%d'\n", nRet);
				break;
			}

		} while ( nRet == SQLITE_ROW );
		return 0;
	}

	void save( const std::string& fullPath )
	{
		//dump file
		loadOrSaveDb( db, fullPath.c_str(), true );

		sqlite3_close( db );
	}
}
