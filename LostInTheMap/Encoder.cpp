#include "Encoder.h"

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Encoder::decode_base64(std::string encoded)
{
	std::string result;
	//result = encoded;

	//FOLLOWING DECODING CODE WAS TAKEN FROM
	//https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
	//code written by 
	size_t in_len = encoded.size();
	size_t i = 0;
	size_t j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];

	while (in_len-- && (encoded[in_] != '=') && is_base64(encoded[in_])) {
		char_array_4[i++] = encoded[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = static_cast<unsigned char>(encoded.find(char_array_4[i]));

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				result += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = static_cast<unsigned char>(encoded.find(char_array_4[j]));

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) result += char_array_3[j];
	}

	return result;
}

std::string Encoder::encode_base64(std::string normal)
{
	std::string result;
	result = normal;

	return result;
}

Encoder::Encoder()
{
}


Encoder::~Encoder()
{
}
