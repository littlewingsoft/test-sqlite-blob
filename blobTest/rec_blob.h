
namespace rec_blob
{

	void start();

	/*!
	@brief packet �� blob �� �߰���Ŵ.
	@param sqlite3 *db: db �ڵ� ������
	@param DWORD dwTime: ��Ŷ�� �߰��ɶ��� ����� �ð���. 
	�������� ���۽ð��� 0���� �����Ѵ�.
	@return none
	@see 
	*/

	void reset_time();

	/*!
	@brief packet �� blob �� �߰���Ŵ.
	@param sqlite3 *db: db �ڵ� ������
	@param DWORD dwTime: ��Ŷ�� �߰��ɶ��� ����� �ð���. 
	�������� ���۽ð��� 0���� �����Ѵ�.
	@return none
	@see 
	*/

	void add(const void* blob, 	size_t size_blob );

	///////////////////////////////////////////////////////////////////////////////

	/*!
	@brief last_rec_id ��������  blob �ʵ��� ���� �� ������.
	@param sqlite3 *db: db �ڵ� ������
	@param DWORD last_rec_id: ������ ��Ŷ�� ���ڵ� ���̵�. 
	�����ö����� ���ŵȴ�. 
	�̰��� �������� ������Ŷ�� �����ü��ֱ� ������
	�߿��� ���̴�.

	@param DWORD dwElapsedTime: ���� ��Ŷ�� �����ϱ������ ����� �ð���.
	@return none
	@see 
	*/
	void* get(	DWORD& last_rec_id, DWORD& dwElapsedTime );


	void save( const std::string& fullPath );

}

