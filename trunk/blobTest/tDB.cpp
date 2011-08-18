#include <stdafx.h>
#include "tDB.h"

	void tDB::opendb(const char* szDBPathName )
	{
		if ( sqlite3_open( szDBPathName, &db) != SQLITE_OK )
		{
			//printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(db));
			throw "fail to sqlite3_open()";
		}	
	}

	void tDB::closedb()
	{
		sqlite3_close( db );
	}

	tDB::tDB(const char* szDBPathName)
	{
		opendb(szDBPathName);
	}
	tDB::~tDB()
	{
		closedb();
	}
