#ifndef __TDB__
#define __TDB__

struct sqlite3;

struct tDB
{
	sqlite3 *db ;
	void opendb(const char* szDBPathName );
	void closedb();
	tDB(const char* szDBPathName);
	~tDB();
};

#endif