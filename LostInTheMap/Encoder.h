#pragma once
#include <string>

class Encoder
{
public:
	static std::string decode_base64(std::string encoded);
	static std::string encode_base64(std::string normal);
	Encoder();
	~Encoder();
};

