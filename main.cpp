#include <iostream>
#include "tcpclient.h"
using namespace std;
using namespace tinyredis;

void test1();
void test2();
void test3();
void test4();

int main()
{
    test4();
    return 0;
}

void test4(){
    RedisClient client;
    client.connectServer("127.0.0.1", 23333);
    client.sadd("a", "a");
    client.sadd("a", "b");
    client.sadd("a", "c");
        client.sadd("a", "d");
    client.sadd("b", "a");
    client.sadd("b", "b");
    client.sadd("b", "f");

    if(client.sismember("a", "a") == RESULT_1){
        cout<<"is member"<<endl;
    }

    client.srem("a", "d");
    vector<string> vec;
    client.smembers("a", vec);
    for(int i = 0; i < vec.size(); i++){
        cout<<vec[i]<<endl;
    }
    vec.clear();
    cout<<"-----------------"<<endl;

    client.sdiff("a","b", vec);
    for(int i = 0; i < vec.size(); i++){
        cout<<vec[i]<<endl;
    }
}

void test3(){
    RedisClient client;
    client.connectServer("127.0.0.1", 23333);
    client.lpush("a", "a");
  client.rpush("a", "b");
    client.rpush("a", "c");
    client.rpush("a", "d");
    client.rpush("a", "e");
    client.rpush("a", "f");
    client.rpush("a", "g");
    string ret;
    client.lindex("a", 0, ret);
    cout<<ret<<endl;
    client.rpop("a", ret);
    cout<<ret<<endl;
    client.lpop("a", ret);
    cout<<ret<<endl;
    client.linsertAfter("a", "c", "gggg");
    client.linsertBefore("a", "c", "ffffff");
    client.lrem("a", 1, "c");
}

void test1(){
    RedisClient client;
    client.connectServer("127.0.0.1", 23333);
    client.set("test1", "1234");
    if(RESULT_1 == client.exists("test1")){
        cout<<"exists"<<endl;
    }
    string ret;
    client.get("test1", ret);
    cout<<"test1:"<<ret<<endl;
    client.incrby("test1", 2);
    client.get("test1", ret);
    cout<<"test1:"<<ret<<endl;
    client.del("test1");
    if(client.get("test1", ret) != RESULT_OK){
        cout<<"buok"<<endl;
    }
}

void test2(){
    RedisClient client;
    client.connectServer("127.0.0.1", 23333);
    client.hset("a", "b", "c");
    client.hset("a", "d", "e");
    client.hset("a", "f", "g");
    client.hset("a", "h", "i");
    client.hdel("a", "b");

    if(client.hexists("a", "f") == RESULT_1){
        cout<<"1"<<endl;
    }else{
        cout<<"0"<<endl;
    }
    if(client.hexists("a", "b") == RESULT_1){
        cout<<"1"<<endl;
    }else{
        cout<<"0"<<endl;
    }
    vector<RedisPair> vec;
    client.hgetall("a", vec);
    for(int i = 0; i < vec.size(); i++){
        cout<<vec[i].first<<","<<vec[i].second<<endl;
    }
}
