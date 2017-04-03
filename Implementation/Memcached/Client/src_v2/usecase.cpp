// #include <mem_config.h>
// #include <libmemcached-1.0/memcached.h>
// #include <libmemcached/memcached_pool.h>

#include <iostream>
#include <libmemcached/memcached.h>
using namespace std;

int main(){
  const char *config_string = "--SERVER=10.129.256.154";
  memcached_st *memc= memcached(config_string, strlen(config_string));
  // memcached_return_t rc;
  char *key= "foo";
  char *value= "value";
  memcached_return_t rc= memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);
  if (rc != MEMCACHED_SUCCESS)
  {
    cerr<<"set failure"<<endl;
  }

  // if(1==0)
  // {
  //   memcached_return_t rc;
  //   char *keys[]= {"fudge", "son", "food"};
  //   size_t key_length[]= {5, 3, 4};
  //   unsigned int x;
  //   uint32_t flags;
  //
  //   char return_key[MEMCACHED_MAX_KEY];
  //   size_t return_key_length;
  //   char *return_value;
  //   size_t return_value_length;
  //
  //   rc= memcached_mget(memc, keys, key_length, 3);
  //
  //   x= 0;
  //   while ((return_value= memcached_fetch(memc, return_key, &return_key_length,
  //                                         &return_value_length, &flags, &rc)))
  //   {
  //     free(return_value);
  //     x++;
  //   }
  // }
/*
  Release the memc_ptr that was pulled from the pool
*/
// memcached_pool_push(pool, memc);

/*
  Destroy the pool.
*/
// memcached_pool_destroy(pool);
 return 0;
}
