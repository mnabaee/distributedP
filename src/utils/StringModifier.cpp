/*
 * StringModifier.cpp
 *
 *  Created on: Jul 29, 2016
 *      Author: mahdy
 */

#include "StringModifier.h"
#include <vector>
#include <utils/Logging.h>
#include <functional>
#include<openssl/evp.h>
#include <b64/cencode.h>
#include <b64/cdecode.h>


StringModifier::StringModifier() {
	// TODO Auto-generated constructor stub

}

StringModifier::~StringModifier() {
	// TODO Auto-generated destructor stub
}

void StringModifier::replaceTemplateWith(string& str, const string&key, const string& value){
	size_t index2 = 0;
	while(true){
		size_t index = str.find(key, index2);
		if( index == string::npos ) return;

		str.replace( index, key.length(), value);
		index2 = index + value.length();
	}
}

void StringModifier::removePhraseAfter(string& str, const string& key){
	//This removes the characters right after a special key until a space is met
	size_t index2 = 0;
	while(true){
		size_t index = str.find(key, index2);
		if( index == string::npos ) return;
		size_t indexEnd = str.find(" ", index);
		if( indexEnd == string::npos ){
			str.replace( index, str.length() - indexEnd, ""  );
			return;
		}else{
			str.replace( index, indexEnd - index, "" );
			index2 = indexEnd;
		}
	}
}

void StringModifier::hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2)
{
    hex1 = c / 16;
    hex2 = c % 16;
    hex1 += hex1 <= 9 ? '0' : 'a' - 10;
    hex2 += hex2 <= 9 ? '0' : 'a' - 10;
}

string StringModifier::UriEncode(const string& s){

	   const char *str = s.c_str();
	    vector<char> v(s.size());
	    v.clear();
	    for (size_t i = 0, l = s.size(); i < l; i++)
	    {
	        char c = str[i];
	        if ((c >= '0' && c <= '9') ||
	            (c >= 'a' && c <= 'z') ||
	            (c >= 'A' && c <= 'Z') ||
	            c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
	            c == '*' || c == '\'' || c == '(' || c == ')')
	        {
	            v.push_back(c);
	        }
	        else if (c == ' ')
	        {
	            v.push_back('+');
	        }
	        else
	        {
	            v.push_back('%');
	            unsigned char d1, d2;
	            hexchar(c, d1, d2);
	            v.push_back(d1);
	            v.push_back(d2);
	        }
	    }

	    return string(v.cbegin(), v.cend());
}

string StringModifier::zCompress(const string& str, int compressionlevel){
    z_stream zs;                        // z_stream is zlib's control structure
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionlevel) != Z_OK){
    	LogError("deflateInit failed while compressing.");
    	return "";
    }

    zs.next_in = (Bytef*)str.data();
    zs.avail_in = str.size();           // set the z_stream's input

    int ret;
    char outbuffer[str.length() + 20];
    std::string outstring;

    // retrieve the compressed bytes blockwise
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = deflate(&zs, Z_FINISH);

        if (outstring.size() < zs.total_out) {
            // append the block to the output string
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
    	LogError("Exception during zlib compression: %s", zs.msg);
    	return "";
    }

    return outstring;

}

string StringModifier::zDecompress(const string& str){
	   z_stream zs;                        // z_stream is zlib's control structure
	    memset(&zs, 0, sizeof(zs));

	    if (inflateInit(&zs) != Z_OK){
	    	LogError("inflateInit failed while decompressing.");
	    	return "";
	    }

	    zs.next_in = (Bytef*)str.data();
	    zs.avail_in = str.size();

	    int ret;
	    char outbuffer[32768];
	    std::string outstring;

	    // get the decompressed bytes blockwise using repeated calls to inflate
	    do {
	        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
	        zs.avail_out = sizeof(outbuffer);

	        ret = inflate(&zs, 0);

	        if (outstring.size() < zs.total_out) {
	            outstring.append(outbuffer,
	                             zs.total_out - outstring.size());
	        }

	    } while (ret == Z_OK);

	    inflateEnd(&zs);

	    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
	    	LogError("Exception during zlib decompression:", zs.msg);
	    	return "";
	    }

	    return outstring;
}

unsigned char* StringModifier::getMd(const string& val, int* mdLen2){
    const char * data = val.c_str();
    int dataLen = val.length();

    unsigned char *md = NULL;
    EVP_MD_CTX *ctx = NULL;
    const EVP_MD *mdType = EVP_md5();

    int mdLen = EVP_MD_size(mdType);

    md = (unsigned char *) malloc(mdLen);
    ctx = EVP_MD_CTX_create();

    EVP_MD_CTX_init(ctx);
    EVP_DigestInit_ex(ctx, mdType, NULL);
    EVP_DigestUpdate(ctx, data, dataLen);
    EVP_DigestFinal_ex(ctx, md, NULL);
    EVP_MD_CTX_cleanup(ctx);
    EVP_MD_CTX_destroy(ctx);
    *mdLen2 = mdLen;
    return md;
}

unsigned int StringModifier::calculateHash(const string& val){

    int hashSize;
    unsigned char* hexchars = getMd(val, &hashSize);

    unsigned int final = 0;
    /*
    final |= ( hexchars[0] << 24 );
    final |= ( hexchars[1] << 16 );
    final |= ( hexchars[2] <<  8 );
    final |= ( hexchars[3]       );
    */
    final |= ( hexchars[0] );
    free(hexchars);
    return final;
}

string StringModifier::base64Encode(const string& str){
	//const char* chars = str.c_str();

	return base64_encode( (unsigned char *) str.c_str(), str.length() );
}

string StringModifier::base64Decode(const string& str){
	return base64_decode(str);
}

const std::string StringModifier::base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


std::string StringModifier::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string StringModifier::base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}



