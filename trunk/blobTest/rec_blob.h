
namespace rec_blob
{

	void start();

	/*!
	@brief packet 을 blob 로 추가시킴.
	@param sqlite3 *db: db 핸들 포인터
	@param DWORD dwTime: 페킷이 추가될때의 상대적 시간값. 
	게임최초 시작시간을 0으로 간주한다.
	@return none
	@see 
	*/

	void reset_time();

	/*!
	@brief packet 을 blob 로 추가시킴.
	@param sqlite3 *db: db 핸들 포인터
	@param DWORD dwTime: 페킷이 추가될때의 상대적 시간값. 
	게임최초 시작시간을 0으로 간주한다.
	@return none
	@see 
	*/

	void add(const void* blob, 	size_t size_blob );

	///////////////////////////////////////////////////////////////////////////////

	/*!
	@brief last_rec_id 값이후의  blob 필드의 값을 을 가져옴.
	@param sqlite3 *db: db 핸들 포인터
	@param DWORD last_rec_id: 마지막 페킷의 레코드 아이디. 
	가져올때마다 갱신된다. 
	이값을 기준으로 다음페킷을 가져올수있기 때문에
	중요한 값이다.

	@param DWORD dwElapsedTime: 현재 페킷을 적용하기까지의 상대적 시간값.
	@return none
	@see 
	*/
	void* get(	DWORD& last_rec_id, DWORD& dwElapsedTime );


	void save( const std::string& fullPath );

}

