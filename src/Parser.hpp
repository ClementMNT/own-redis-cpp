#pragma once
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <algorithm>


class RedisParser {
public:
    RedisParser(const std::string& input) : request(input), pos(0) {}
    std::vector<std::string> parse();
private:
    std::string request;
    size_t pos;
    std::vector<std::string> parseRedisRequest();
    std::vector<std::string> parseBulkString();
    std::vector<std::string> parseArray();
    int readLength();

};