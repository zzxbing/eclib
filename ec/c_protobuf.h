﻿/*!
\file c_protobuf.h
\author kipway@outlook.com
\update 2018.1.29 

eclib class base_protobuf ,parse google protocol buffer

not support start_group and end_group 

eclib Copyright (c) 2017-2018, kipway
source repository : https://github.com/kipway/eclib

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <stdint.h>
#include <memory.h>
#include "c11_vector.h"
#define pb_varint  0  // Varint
#define pb_fixed64 1  // 64-bit
#define pb_length_delimited  2  // Length-delimited
#define pb_start_group  3 // deprecated not support
#define pb_end_group  4   //deprecated not support
#define pb_fixed32 5  // 32-bit
namespace ec
{	
	/*!
	\brief encode and decode 
	see https://developers.google.com/protocol-buffers/docs/encoding
	*/
	class base_protobuf //base class for encode and decode protobuf
	{
	public:
		inline uint32_t en_zigzag32(int32_t v) const
		{
			return ((v << 1) ^ (v >> 31));
		}
		inline uint64_t en_zigzag64(int64_t v) const
		{
			return ((v << 1) ^ (v >> 63));
		}
		inline int32_t de_zigzag32(uint32_t v) const
		{
			return (int)((v >> 1) ^ (-(int32_t)(v & 1)));
		}
		inline int64_t de_zigzag64(uint64_t v) const
		{
			return (int)((v >> 1) ^ (-(int64_t)(v & 1)));
		}
		template<class _Tp>
		inline bool get_varint(const uint8_t* &pd, size_t &len, _Tp &out) const  //get Varint (Base 128 Varints)
		{
			int nbit = 0;
			out = 0;
			do{
				out |= (*pd & 0x7F) << nbit;
				nbit += 7;
				pd++;
				len--;
			} while (*(pd - 1) & 0x80 && len > 0 && nbit < 8 * sizeof(_Tp));
			return nbit <= 8 * sizeof(_Tp);
		}
		template<class _Tp>
		bool out_varint(_Tp v, ec::vector<uint8_t>* pout) const //out Varint (Base 128 Varints)
		{
			int nbit = 0;
			uint8_t out = 0;
			do{
				out = (v >> nbit) & 0x7F;
				nbit += 7;
				if (v >> nbit) {
					out |= 0x80;
					pout->add(out);
				}
				else {
					pout->add(out);
					break;
				}
			} while (nbit < 8 * sizeof(_Tp));
			return nbit <= 8 * sizeof(_Tp);
		}
		template<class _Tp>
		inline bool get_fixed(const uint8_t* &pd, size_t &len, _Tp &out) const  //get 32-bit and 64-bit (fixed32,sfixed32,fixed64,sfixed64,float,double)
		{
			if (len < sizeof(_Tp) || (sizeof(_Tp) != 4 && sizeof(_Tp) != 8))
				return false;
			memcpy(&out, pd, sizeof(_Tp));
			pd += sizeof(_Tp);
			len -= sizeof(_Tp);
			return true;
		}
		template<class _Tp>
		inline bool out_fixed(_Tp v, ec::vector<uint8_t>* pout) const  //out 32-bit and 64-bit (fixed32,sfixed32,fixed64,sfixed64,float,double)
		{
			if ((sizeof(_Tp) != 4 && sizeof(_Tp) != 8))
				return false;
			pout->add((uint8_t*)&v, sizeof(_Tp));
			return true;
		}
		inline bool get_key(const uint8_t* &pd, size_t &len, uint32_t &field_number, uint32_t &wire_type) const //get field_number and  wire_type
		{
			uint32_t key;
			if (!get_varint(pd, len, key))
				return false;
			wire_type = key & 0x07;
			field_number = key >> 3;
			return true;
		}
		inline bool get_length_delimited(const uint8_t* &pd, size_t &len, ec::vector<uint8_t>* pout) const //get string, bytes
		{
			pout->clear();
			uint32_t ul = 0;
			if (!get_varint(pd, len, ul))
				return false;
			if (len < ul)
				return false;
			pout->add(pd, ul);
			pd += ul;
			len -= ul;
			return true;
		}
		inline bool get_length_delimited(const uint8_t* &pd, size_t &len, void* pout, size_t &outlen) const //get string, bytes
		{
			uint32_t ul = 0;
			if (!get_varint(pd, len, ul))
				return false;
			if (len < ul || outlen < ul)
				return false;
			memcpy(pout, pd, ul);
			pd += ul;
			len -= ul;
			outlen = ul;
			return true;
		}
		inline bool jump_over(const uint8_t* &pd, size_t &len, uint32_t wire_type) const //jump over unkown field_number
		{
			switch (wire_type)
			{
			case 0: // Varint
			{
				uint64_t v;
				return get_varint(pd, len, v);
			}
			break;
			case 1: // 64-bit
			{
				if (len < 8)
					return false;
				pd += 8;
				len -= 8;				
				break;
			}
			case 2: //Length - delimited
			{
				size_t datalen;
				if (!get_varint(pd, len, datalen))
					return false;
				if (len < datalen)
					return false;
				pd += datalen;
				len -= datalen;				
				break;
			}
			case 5: //32 bit
			{
				if (len < 4)
					return false;
				pd += 4;
				len -= 4;				
				break;
			}
			default:
				return false;// unkown wire_type
			}
			return true;
		}
		inline bool out_length_delimited(const uint8_t* pd, size_t len, ec::vector<uint8_t>* pout) const // out string, bytes
		{
			if (!out_varint(len, pout))
				return false;
			pout->add(pd, len);
			return true;
		}
		inline bool out_key(uint32_t field_number, uint32_t wire_type, ec::vector<uint8_t>* pout) const // out field_number and  wire_type
		{
			uint32_t v = (field_number << 3) | (wire_type & 0x07);
			return out_varint(v, pout);
		}
	};
}
