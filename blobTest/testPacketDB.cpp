#include <stdafx.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rec_blob.h"
///////////////////////////////////////////////////////////////////////////////
// struct 넣고 빼고.
///////////////////////////////////////////////////////////////////////////////
struct tPacket
{
	int type;
	WCHAR desc[64];
};

///////////////////////////////////////////////////////////////////////////////

struct tPacket_move : public tPacket
{
	int x,y,z;
	tPacket_move() 
	{
		type = 118;
		x=10;
		y=20;
		z=30;
		wcscpy_s( desc, 64, L"move" );
	}

};

///////////////////////////////////////////////////////////////////////////////

struct tPacket_Attk : public tPacket
{
	int targetId;
	tPacket_Attk()
	{
		type = 119;
		targetId = 18;
		wcscpy_s( desc,64, L"張어택" );
		
	}

};

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

int testPacketDb()
{
	//DWORD start = timeGetTime();
	//if( sqlite3_exec(db, "drop TABLE tb_packet", NULL, NULL, NULL) != SQLITE_OK )
	//{
	//	printf("[X] delete FROM fileChunkTable, %s \n", sqlite3_errmsg(db));
	//	//sqlite3_close(db);
	//	//return -1;
	//}
	
	tPacket_Attk attk;
	tPacket_move move;

	rec_blob::start();

	Sleep( 1000 );
	rec_blob::add( (const void*)&attk, sizeof( attk ) );//timeGetTime()-start
	Sleep( 1000 );
	rec_blob::add( (const void*)&move, sizeof( move ) );
	Sleep( 1000 );
	rec_blob::add( (const void*)&attk, sizeof( attk ) );//timeGetTime()-start
	Sleep( 1000 );
	rec_blob::add( (const void*)&move, sizeof( move ) );
	Sleep( 1000 );
	rec_blob::add( (const void*)&attk, sizeof( attk ) );//timeGetTime()-start
	Sleep( 1000 );
	rec_blob::add( (const void*)&move, sizeof( move ) );


	DWORD last_rec_id=0,dwElapsedTime = 0; // 마지막 rec_id 값. 계속 마지막 값으로 갱신됨.
	float timeScale = 2.0f;
	while( true )
	{
		tPacket* pkt = (tPacket*)rec_blob::get( last_rec_id, dwElapsedTime );
		if( pkt == 0 )
			break;

		float fElapsed = dwElapsedTime / timeScale;

		Sleep( (DWORD)fElapsed );

		printf( "%d\n", pkt->type );
	}


	rec_blob::save( "blobtest.rec" );

	return 0;
}