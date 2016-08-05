//**********************************************************************
//Tinyredis client 是为我做的Tinyredis小程序 所配套的c语言api,并不是redis的client
//小弟才疏学浅,如果代码能给你一些帮助,这将会是我的荣幸.
//代码只能算一般般,如果我哪里写的不好还请斧正,谢谢
//**********************************************************************
#ifndef TCPCLIENT
#define TCPCLIENT
#include <string>
#include <utility>
#include <vector>
using std::vector;
using std::string;

namespace tinyredis{

enum ResultCode{
    ERROR_SYNAX,
    ERROR_TYPE,
    ERROR_UNKNOW,
    ERROR_NETWORK,
    RESULT_1 ,
    RESULT_0,
    RESULT_OK,
    RESULT_NULL
};

enum RedisDataType{
    TYPE_STRING,
    TYPE_INT,
    TYPE_LIST,
    TYPE_HASH,
    TYPE_SET,
    TYPE_OTHER
};

typedef std::pair<string, string> RedisPair;

class RedisClient{
public:
    RedisClient();
    ~RedisClient();
    int connectServer(const string& ip, size_t port);
    //函数格式说明:
    // key value 都是输入的参数, 懂redis的应该明白
    //这里返回数据采用的是传入vector引用的方式.
    //然后根据返回值,判断是否执行成功

    //每一行函数声明,后面的注释是返回值可能返回的类型.
    //redis的返回值设计得很分裂,一会儿OK,一会儿1,让人疑惑
    //ERROR_TYPE 和 ERROR_SYNAX每个函数都可能返回,但有的返回1,有的返回OK,有的可以返回NULL或者0
    ResultCode set(const string& key, const string& value);         // OK
    ResultCode get(const string&key, string& ret);                            // 1
    ResultCode exists(const string&key);                                // 1 / 0
    ResultCode incrby(const string&key, int incr);                // value(1)
    ResultCode del(const string&key);                                          //  1 / 0

    ResultCode hget(const string&field,const string&key, string& ret);                // value(1) / RESULT_NULL
    ResultCode hset(const string&field,const string&key, const string& value);              // 1
    ResultCode hexists(const string&field,const string&key);                                         // 1 / 0
    ResultCode hgetall(const string&field, vector<RedisPair>& ret);                            // 1 / NULL
    ResultCode hdel(const string&field,const string&key);                                          // 1 / 0
    ResultCode hincrby(const string&field,const string&key, int incr);                            // 1 / 0 /NULL

    ResultCode lpush(const string& key, const string& value);                                    //value (1) / NULL
    ResultCode rpush(const string& key, const string& value);                                   //value (1) / NULL
    ResultCode lrange(const string& key, int left, int right, vector<string>& vec);    // 1 / NULL
    ResultCode lrem(const string& key, int count, const string& value);                   // value(1)/ 0
    ResultCode lindex(const string& key, int index, string& ret);                                //1
    ResultCode lpop(const string& key, string& ret);                                                   // value(1)
    ResultCode rpop(const string& key, string& ret);                                                  // value(1)
    ResultCode linsertBefore(const string& key, const string& target, const string& insert);  // value(1) / NULL /
    ResultCode linsertAfter(const string& key, const string& target, const string& insert);

    ResultCode type(const string& key, RedisDataType& type);                                    // 1  /  NULL
    ResultCode srem(const string& key, const string& value);                                      // 1 / 0
    ResultCode sadd(const string& key, const string& value);
    ResultCode sismember(const string& key, const string& value);                           // 1 / 0
    ResultCode smembers(const string& key, vector<string>& retVector);                //1/ NULL
    ResultCode sdiff(const string& key1, const string& key2,  vector<string>& retVector);     // 1 / NULL
    ResultCode sinter(const string& key1, const string& key2,  vector<string>& retVector);     // 1 / NULL
    ResultCode sunion(const string& key1, const string& key2,  vector<string>& retVector);     // 1 / NULL


private:
    string sendCommomd(const string& cmd);
    int connfd_;
};

}

#endif // TCPCLIENT

