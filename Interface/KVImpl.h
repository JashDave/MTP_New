#include <iostream>
#include <vector>

using namespace std;

class KVImplHelper{
private:
  void *dataholder;
public:
  KVImplHelper();
  ~KVImplHelper();
  bool bind(string connection,string tablename);
  shared_ptr<KVData<string>> get(string const& key);
  shared_ptr<KVData<string>> put(string const& key,string const& val);
  shared_ptr<KVData<string>> del(string const& key);
  bool clear();

  int mget(vector<string>& key, vector<string>& tablename, vector<shared_ptr<KVData<string>>>& ret);
  int mdel(vector<string>& key, vector<string>& tablename, vector<shared_ptr<KVData<string>>>& ret);
  int mput(vector<string>& key, vector<string>& val, vector<string>& tablename, vector<shared_ptr<KVData<string>>>& ret);
};


/*
	This class mearges multiple operations into one request.
*/
	class KVRequestHelper {
	private:
		// int count=0;
		void *dataholder;
		std::vector<string> operation_type;

		std::vector<string> put_tablename;
		std::vector<string> put_key;
		std::vector<string> put_value;

		std::vector<string> get_tablename;
		std::vector<string> get_key;

		std::vector<string> del_tablename;
		std::vector<string> del_key;
	public:
		KVRequestHelper();
		~KVRequestHelper();		//For distroying connection object

		bool bind(string connection);
		void get(string const& key,string tablename);
		void put(string const& key,string const& val,string tablename);
		void del(string const& key,string tablename);
		KVResultSet execute();
		void reset();

    vector<string> getOperationsList();
	};
