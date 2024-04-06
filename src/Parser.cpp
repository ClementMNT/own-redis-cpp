#include "./Parser.hpp"


std::vector<std::string> RedisParser::parse() {
    return parseRedisRequest();
}

std::vector<std::string> RedisParser::parseRedisRequest() {
    if (request[pos] == '$') {
        pos++; // Move past "$"
        return parseBulkString();
    } else if (request[pos] == '*') {
        pos++; // Move past "*"
        return parseArray();
    }
    // Return an empty vector if the input does not start with a recognized character.
    return {};
}

std::vector<std::string> RedisParser::parseBulkString() {
    int length = readLength();
    std::vector<std::string> result;
    if (length == -1) { // Handle NULL bulk string
        result.push_back("NULL");
        return result;
    }
    if (length == 0) {
        result.push_back("");
        return result;
    }
    result.push_back(request.substr(pos, length));
    pos += length + 2; // Move past the bulk string and the trailing \r\n
    return result;
}

std::vector<std::string> RedisParser::parseArray() {
    int length = readLength();
    std::vector<std::string> result;
    if (length == -1) { // Handle NULL array
        result.push_back("NULL");
        return result;
    }
    for (int i = 0; i < length; i++) {
        std::vector<std::string> element = parseRedisRequest();
        result.insert(result.end(), element.begin(), element.end());
    }
    return result;
}

int RedisParser::readLength() {
    int length = 0;
    while (request[pos] != '\r') {
        length = (length * 10) + (request[pos] - '0');
        pos++;
    }
    pos += 2; // Move past the trailing \r\n
    return length;
}
