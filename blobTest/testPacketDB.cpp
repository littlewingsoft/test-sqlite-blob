#include <stdafx.h>
#include <stdlib.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// struct ³Ö°í »©°í.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct tPacket
{
	int type;
	char desc[64];
};

struct tPacket_move : public tPacket
{
	int x,y,z;
	tPacket_move() 
	{
		type = 0x18;
		x=10;
		y=20;
		z=30;
		strcpy_s( desc, 64,"move" );
	}

};

struct tPacket_Attk : public tPacket
{
	int targetId;
	tPacket_Attk()
	{

	}

};

int testPacketDb()
{
	sqlite3 *db = NULL;
	sqlite3_stmt *stmt = NULL;
	int nRet = 0;

	if ( sqlite3_open( "packet.db", &db) != SQLITE_OK )//:memory:
	{
        printf("fail to sqlite3_open(), %s\n", sqlite3_errmsg(db));
        return -1;
	}

	if( sqlite3_exec(db, "drop TABLE Table_Packet", NULL, NULL, NULL) != SQLITE_OK )
	{
		printf("[X] delete FROM fileChunkTable, %s \n", sqlite3_errmsg(db));
		//sqlite3_close(db);
		//return -1;
	}

	char szQuery[0xFF];
	int n = sprintf_s(szQuery, 255, "CREATE TABLE Table_Packet(id INTEGER PRIMARY KEY AUTOINCREMENT,type int, desc varchar(32),  pkt BLOB );");
    if ( sqlite3_exec(db, szQuery, NULL, NULL, NULL) != SQLITE_OK )
    {
		
        printf("fail to create table, %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    n = sprintf_s(szQuery,255, "INSERT OR IGNORE INTO Table_Packet (type,desc,pkt) VALUES (?,?,?);"); //
    if ( sqlite3_prepare(db, szQuery, n, &stmt, NULL) != SQLITE_OK )
    {
        printf("fail to sqlite3_prepare, %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
	sqlite3_reset(stmt);

	tPacket_Attk tt;
	tt.type = 0x119;
	tt.targetId =  0x09 ;
	strcpy_s( tt.desc,64, "íå¾îÅÃ" );
	// type bind
	int ret = sqlite3_bind_int( stmt, 1, tt.type );

	sqlite3_reset(stmt);
	// desc bind
	ret = sqlite3_bind_text( stmt, 2, tt.desc, sizeof( tt.desc ),0 );

	// pkt bind
	sqlite3_reset(stmt);
	const void* szblob = (const void*)&tt;
	ret = sqlite3_bind_blob( stmt, 3, (const void *)szblob , sizeof( tt ), NULL);
	if( SQLITE_OK == ret  )
		sqlite3_step(stmt);

	{
		int n = sprintf_s(szQuery, 255, "select * from Table_Packet;");
	
		if ( sqlite3_prepare(db, szQuery, n, &stmt, NULL) != SQLITE_OK )
		{
			printf("fail to select, %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return -1;
		}

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
					tPacket_Attk* tmp = (tPacket_Attk*)sqlite3_column_blob(stmt, 3) ;
					printf("type(int) column value  : %d\n", tmp->type);
					printf("desc(TEXT) column value  : %s\n", tmp->desc);
				}
				break;
			default:
				printf("sqlite3_step return '%d'\n", nRet);
				break;
			}

		} while ( nRet == SQLITE_ROW );
    
	}

	sqlite3_close( db );
	return 0;
}