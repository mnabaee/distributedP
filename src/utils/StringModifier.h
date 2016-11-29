/*
 * StringModifier.h
 *
 *  Created on: Jul 29, 2016
 *      Author: mahdy
 */

#ifndef UTILS_STRINGMODIFIER_H_
#define UTILS_STRINGMODIFIER_H_
#include <string>
#include <zlib.h>

using namespace std;

class StringModifier {
	static void hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2);

	static const std::string base64_chars;
	static inline bool is_base64(unsigned char c) {
	  return (isalnum(c) || (c == '+') || (c == '/'));
	}
	static std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
	static std::string base64_decode(std::string const& encoded_string);

public:
	StringModifier();
	virtual ~StringModifier();

	static void replaceTemplateWith(string& str, const string& key, const string& value);
	static void removePhraseAfter(string& str, const string& key);

	static string UriEncode(const string& input);

	static string zCompress(const string& input, int compressionlevel = Z_BEST_COMPRESSION);
	static string zDecompress(const string& input);

	static unsigned char* getMd(const string& val, int* mdLen);
	static unsigned int calculateHash(const string& val);

	static string base64Encode(const string& str);
	static string base64Decode(const string& str);

};

#endif /* UTILS_STRINGMODIFIER_H_ */
