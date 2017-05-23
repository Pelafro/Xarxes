#pragma once
#include <cstdint>
static uint64_t inet_addr(const char* host, uint16_t family = 2, const uint64_t buffer = 0, const uint64_t retval = 0) {
	switch (*host) {
		case '.':case ':':	return inet_addr(host + 1, family, 0, (retval >> 8) | (buffer << (64 - 8)));
		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': return inet_addr(host + 1, family, buffer * 10 + (host[0] - '0'), retval);
		default: return ((buffer & 0xFF) << 8 | buffer >> 8) << 16 | family | retval;
	}
}
union sockaddr {
	char raw[16];
	struct {
		uint16_t family, port;
		uint32_t address;
	} data;
	uint64_t hash;
	sockaddr(uint16_t port = 0, uint32_t ip = 0, uint16_t family = 2) :data({ family,uint16_t(port << 8 | port >> 8),ip }) {}
	sockaddr(const char* host, uint16_t family = 2) :hash(inet_addr(host, family)) {}
};
extern "C" {
#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#define IF_WIN(w,l) w
	__declspec(dllimport) int __stdcall WSAStartup(unsigned short, void*);
#else
#define __stdcall __cdecl
#define IF_WIN(w,l) l
#endif
	int __stdcall bind(int, sockaddr*, int);
	int __stdcall socket(int, int, int);
	int __stdcall sendto(int, const void*, size_t, int, const sockaddr*, int);
	int __stdcall recvfrom(int, void*, size_t, int flags, sockaddr*, int*);
	int __stdcall IF_WIN(ioctlsocket, ioctl)(unsigned int, unsigned int, unsigned long*);
}
struct UDP {
	int sd;
	UDP(const unsigned short port = 0) {
		IF_WIN({ char info[1024]; WSAStartup(514, &info); }, );
		long unsigned opt = 1;
		IF_WIN(ioctlsocket, ioctl)(sd = socket(2, 2, 0), 0x8004667E, &opt); //TODO:Check why old FIONBIO was === 0x5421
		sockaddr address(port);
		bind(sd, &address, sizeof(sockaddr));
	};
	const int write(const void* buffer, const int len, const sockaddr* to)const { return sendto(sd, buffer, len, 0, to, sizeof(sockaddr)); }
	const int read(void* buffer, const int len, sockaddr* from = 0) { int t = from ? sizeof(sockaddr) : 0; return recvfrom(sd, buffer, len, 0, from, from ? &t : 0); }
};

#include <vector>
#include <string>
#include <iostream>
#include <type_traits>
#define UDPSIterable(type_macro) \
template<class T> UDPStream &operator<<(const type_macro<T> output) { *this << uint32_t(output.size()); return emplace_back(reinterpret_cast<const uint8_t*>(&output[0]), output.size()*sizeof(T)); }\
template<class T> UDPStream &operator>>(type_macro<T>& output){\
	uint32_t sz;\
	*this >> sz;\
	if (index + sz > data.size())\
		throw wrong();\
	output = type_macro<T>(reinterpret_cast<T*>(&data[0]+index),reinterpret_cast<T*>(&data[0]+index+(sz*sizeof(T))));\
	index += sz*sizeof(T);\
	return *this;\
}
static std::ostream dummy(0);
static size_t charplen(const char* str, size_t s = 0) { return (str[s]) ? charplen(str, s + 1) : s; }
struct UDPStream : public UDP {
	enum controller { packet, answer };
	struct empty : public std::exception {};
	struct wrong : public std::exception {};
	UDPStream(const unsigned short port = 0, const unsigned int reserve_bytes = 1024) : UDP(port) { reserve(reserve_bytes); };
	inline void reserve(unsigned int s) { data.reserve(s); }
	//Command Controller
	UDPStream &operator<<(controller cmd) {
		if (cmd == packet)
			data.clear();
		if (cmd == answer)
			write(&data[0], data.size(), &target);
		return *this;
	}
	UDPStream &operator>>(controller cmd) {
		if (cmd == packet) {
			sockaddr from;
			int t = sizeof(sockaddr);
			data.resize(data.capacity());
			int l = recvfrom(sd, &data[0], data.capacity(), 0, &from, &t);
			index = 0;
			if (l > 0) {
				data.resize(l);
				target.hash = from.hash;
			} else {
				data.clear();
				throw empty();
			}
		}
		return *this;
	}
	UDPSIterable(std::vector)
		//POD handling
		template<class T, typename = std::enable_if<!std::is_pointer<T>::value>>
	UDPStream &operator<<(T&& new_data) { return emplace_back(reinterpret_cast<const uint8_t*>(&new_data), sizeof(T)); }
	template<class T, typename = std::enable_if<!std::is_pointer<T>::value>>
	UDPStream &operator>>(T&& new_data) { return extract_from(reinterpret_cast<uint8_t*>(&new_data), sizeof(T)); }
	//std::string
	UDPStream &operator<<(const std::string output) { *this << uint32_t(output.length()); return emplace_back(reinterpret_cast<const uint8_t*>(&output[0]), output.length()); }
	UDPStream &operator>>(std::string& output) {
		uint32_t sz;
		*this >> sz;
		if (index + sz > data.size())
			throw wrong();
		output = std::string(&data[0] + index, &data[0] + index + sz);
		index += sz;
		return *this;
	}
	//Fetch and Send from/to Address
	UDPStream &operator<<(const sockaddr new_target) { write(&data[0], data.size(), &new_target); return *this; }
	UDPStream &operator>>(sockaddr& new_target) { new_target.hash = target.hash; return *this; }
	//
	UDPStream &operator<<(std::istream& input) {
		std::string msg;
		do {
			std::getline(input, msg);
		} while (msg == "");
		*this << msg;
		return *this;
	}
	friend std::ostream &operator<<(std::ostream &output, UDPStream &uo) {
		try {
			std::string msg;
			uo >> msg;
			output << msg;
			return output;
		} catch (...) {
			return dummy;
		}
	}
	//char* (send only)
	UDPStream &operator<<(const char* input) {
		size_t len = charplen(input);
		*this << len;
		return emplace_back(reinterpret_cast<const uint8_t*>(input), len);
	}
private:
	sockaddr target;
	std::vector<uint8_t> data;
	unsigned int index = 0;
	//Data emplacement and extraction features
	inline UDPStream& emplace_back(const uint8_t* new_data, size_t length) {
		for (size_t i = 0; i < length; i++)
			data.push_back(new_data[i]);
		return *this;
	}
	inline UDPStream& extract_from(uint8_t* new_data, size_t length) {
		if (index + length > data.size())
			throw wrong();
		for (size_t i = 0; i < length; i++)
			new_data[i] = data[index + i];
		index += length;
		return *this;
	}
};
#undef UDPSIterable