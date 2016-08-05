#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include <iostream>
#include <sstream>
#include "tcpclient.h"
#include "utils.h"
using std::cout;
using std::endl;
namespace tinyredis{

const string STRING_NULL = "NULL\n";

RedisClient::RedisClient(){

}

RedisClient::~RedisClient(){
    ::close(connfd_);
}

int RedisClient::connectServer(const std::string& ip, size_t port){
    int sockfd, status, save_errno;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = inet_aton(ip.c_str(), &server_addr.sin_addr);
    //the server_ip is not valid value
    if( status == 0 ) {
        errno = EINVAL;
        return -1;
    }

    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
    if( sockfd == -1 ){
        return sockfd;
    }

    status = ::connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr) );
    if( status == -1 ){
        save_errno = errno;
        ::close(sockfd);
        errno = save_errno; //the close may be error
        return -1;
    }
    connfd_ = sockfd;
    return sockfd;
}

string RedisClient::sendCommomd(const string &cmd){
    int n;
    n = ::write(connfd_, cmd.c_str(), cmd.length());
    char buf[1024];

    n = ::read(connfd_, buf, 1023);
    buf[n] = '\0';

    return buf;
}

/*
const char * ERROR_SYNAX = "Error: Synax wrong.\n";
const char * ERROR_TYPE = "Error: Type wrong.\n";
const char * RESULT_1 = "(interger)1\n";
const char * RESULT_0 = "(interger)0\n";
const char * RESULT_OK = "OK\n";
const char * RESULT_NULL = "NULL\n";
*/
ResultCode resultCode(const string& ret){
    if(ret.length() == 0){
        return ERROR_NETWORK;
    }else if(ret == "Error: Synax wrong."){
        return ERROR_SYNAX;
    }else if(ret == "Error: Type wrong."){
        return ERROR_TYPE;
    }else if(ret == "Error: Unknow\n"){
        return ERROR_UNKNOW;
    }else if(ret == "1"){
        return RESULT_1;
    }else if(ret == "0"){
        return RESULT_0;
    }else if(ret == "OK"){
        return RESULT_OK;
    }else if(ret == "NULL"){
        return RESULT_NULL;
    }else if(isDigit(ret) || ret.length() > 0){
        return RESULT_1;
    }else{
        return ERROR_UNKNOW;
    }
}

void toResult(string &ret){
    if(ret.length() < 0){
        return;
    }
    //去掉回车
    ret.erase(ret.length() - 1, 1);
    //去掉包围的双引号
    if(ret[0] == '\"'){
        ret.erase(0,1);
        ret.erase(ret.length() - 1, 1);
    }else if(beginWith(ret, "(interger)")){
        ret.erase(0, 10);
    }
}



void removeQuo(string &ret){
    if(ret[0] == '\"'){
        ret.erase(0,1);
        ret.erase(ret.length() - 1, 1);
    }
}

void processVecData(const std::string& str, vector<string>& retVector){
    std::stringstream ss(str);
    std::string line;
    while(std::getline(ss, line)){
        removeQuo(line);
        retVector.push_back(line);
    }
}

ResultCode RedisClient::set(const string &key, const string &value){
    string cmd = "set "+ key+ " " + value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return resultCode(ret);
}

ResultCode RedisClient::get(const string &key,  string &ret){
    string cmd = "get "+ key;
    ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::exists(const string &key){
    string cmd = "exists "+ key;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::incrby(const string &key, int incr){
    string cmd = "incrby "+ key+ " " + int2string(incr);
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::del(const string &key){
    string cmd = "del "+ key;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::hget(const string &field, const string &key,  string &ret){
    string cmd = "hget "+ field +" " + key;
    ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::hset(const string &field, const string &key, const string &value){
    string cmd = "hset "+ field +" " + key + " " +value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::hexists(const string &field, const string &key){
    string cmd = "hexists "+ field +" " + key;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::hgetall(const string &field, vector<RedisPair> &retVector){
    string cmd = "hgetall "+ field;
    string ret = sendCommomd(cmd);
    if(STRING_NULL == ret){
        return RESULT_0;
    }
    std::stringstream ss(ret);
    std::string line;
    int i = 0;
    RedisPair rPair;
    while(std::getline(ss, line)){
        if( i % 2 == 0 ){
            removeQuo(line);
            rPair.first = line;
        }else{
            removeQuo(line);
            rPair.second = line;
            retVector.push_back(rPair);
        }
        i++;
    }
    return RESULT_1;
}

ResultCode RedisClient::hdel(const string &field, const string &key){
    string cmd = "hdel "+ field +" " + key;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::hincrby(const string &field, const string &key, int incr){
    string cmd = "hincrby "+ field+" "+key+ " " + int2string(incr);
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::lpush(const string &key, const string &value){
    string cmd = "lpush "+ key+" "+value;
    string ret = sendCommomd(cmd);
    toResult(ret);

    return  resultCode(ret);
}

ResultCode RedisClient::rpush(const string &key, const string &value){
    string cmd = "rpush "+ key+" "+value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::lrange(const string &key, int left, int right, vector<string>& retVector){
    string cmd = "lrange "+ key+" "+int2string(left) + " " + int2string(right);
    string ret = sendCommomd(cmd);
    if(ret == STRING_NULL){
        return RESULT_NULL;
    }
   processVecData(ret, retVector);
    return RESULT_1;
}

ResultCode RedisClient::lrem(const string &key, int count,const string &value){
    string cmd = "lrem "+ key+ " " + int2string(count) + " "+value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::lindex(const string &key, int index, string& ret){
    string cmd = "lindex "+ key+ " " + int2string(index);
    ret = sendCommomd(cmd);

    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::lpop(const string &key, string &ret){
    string cmd = "lpop "+ key;
    ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::rpop(const string &key, string &ret){
    string cmd = "rpop "+ key;
    ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::linsertBefore(const string &key, const string &target, const string &insert){
    string cmd = "linsert "+ key +" before "+ target + " " + insert;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::linsertAfter(const string &key, const string &target, const string &insert){
    string cmd = "linsert "+ key +" after "+ target + " " + insert;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::type(const string &key, RedisDataType &type){
    string cmd = "type "+ key;
    string ret = sendCommomd(cmd);
    if(ret ==STRING_NULL){
        return RESULT_NULL;
    }else{
        if(ret == "string"){
            type = TYPE_STRING;
        }else if(ret == "hash"){
            type = TYPE_HASH;
        }else if(ret == "list"){
            type = TYPE_LIST;
        }else if(ret == "set"){
            type = TYPE_SET;
        }else if(ret == "int"){
            type = TYPE_INT;
        }else{
            type = TYPE_OTHER;
        }
        return RESULT_1;
    }
}

ResultCode RedisClient::srem(const string &key, const string &value){
    string cmd = "srem "+ key + " " + value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::sadd(const string &key, const string &value){
    string cmd = "sadd "+ key + " " + value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::sismember(const string &key, const string &value){
    string cmd = "sismember "+ key + " " + value;
    string ret = sendCommomd(cmd);
    toResult(ret);
    return  resultCode(ret);
}

ResultCode RedisClient::smembers(const string &key, vector<string> &retVector){
     string cmd = "smembers "+ key;
     string ret = sendCommomd(cmd);
     if(ret == STRING_NULL){
         return RESULT_NULL;
     }
    processVecData(ret, retVector);
    return RESULT_1;
}

ResultCode RedisClient::sdiff(const string &key1, const string &key2,  vector<string>& retVector){
    string cmd = "sdiff "+ key1 + " " + key2;
    string ret = sendCommomd(cmd);
    if(ret == STRING_NULL){
        return RESULT_NULL;
    }
   processVecData(ret, retVector);
   return RESULT_1;
}

ResultCode RedisClient::sinter(const string &key1, const string &key2,  vector<string>& retVector){
    string cmd = "sinter "+ key1 + " " + key2;
    string ret = sendCommomd(cmd);
    if(ret == STRING_NULL){
        return RESULT_NULL;
    }
   processVecData(ret, retVector);
   return RESULT_1;
}

ResultCode RedisClient::sunion(const string &key1, const string &key2 ,  vector<string>& retVector){
    string cmd = "sinter "+ key1 + " " + key2;
    string ret = sendCommomd(cmd);
    if(ret == STRING_NULL){
        return RESULT_NULL;
    }
   processVecData(ret, retVector);
   return RESULT_1;
}

}
