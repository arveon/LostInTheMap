#pragma once
#include <string>
#include <vector>

/*
CODE USED IN THESE METHODS IS NOT WRITTEN BY ME
CODE IS COPIED FROM: https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
WRITTEN BY USER: polfosol
*/

class Encoder
{
public:
	static std::string decode_base64(const void* data, const size_t len);
	static std::string decode_base64(std::string in);
	static std::string encode_base64(std::string normal);
	Encoder();
	~Encoder();
};

