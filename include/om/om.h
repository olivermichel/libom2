
#ifndef LIBOM2_H
#define LIBOM2_H

#include <atomic>
#include <arpa/inet.h>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <poll.h>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <regex>
#include <unistd.h>
#include <vector>

namespace om {

	namespace sys {
		//! a unix file descriptor
		class file_descriptor
		{
		public:
			//! returns the file descriptor as an integer
			inline int fd() const { return _fd; }
		protected:
			int _fd = -1;
		};

		inline unsigned write_uint16(uint16_t val_, char* buf_)
		{
			buf_[0] = (val_ >> 8) & 0xff;
			buf_[1] = (val_ >> 0) & 0xff;
			return 2;
		}

		inline unsigned write_uint32(uint32_t val_, char* buf_)
		{
			buf_[0] = (val_ >> 24) & 0xff;
			buf_[1] = (val_ >> 16) & 0xff;
			buf_[2] = (val_ >>  8) & 0xff;
			buf_[3] = (val_ >>  0) & 0xff;
			return 4;
		}

		inline unsigned write_uint64(uint64_t val_, char* buf_)
		{
			buf_[0] = (val_ >> 56) & 0xff;
			buf_[1] = (val_ >> 48) & 0xff;
			buf_[2] = (val_ >> 40) & 0xff;
			buf_[3] = (val_ >> 32) & 0xff;
			buf_[4] = (val_ >> 24) & 0xff;
			buf_[5] = (val_ >> 16) & 0xff;
			buf_[6] = (val_ >>  8) & 0xff;
			buf_[7] = (val_ >>  0) & 0xff;
			return 8;
		}

		inline uint16_t read_uint16(const char* buf_)
		{
			return  ((((uint16_t) buf_[0]) << 8) & 0xff00)
				  | ((((uint16_t) buf_[1]) << 0) & 0x00ff);
		}

		inline uint32_t read_uint32(const char* buf_)
		{
			return  ((((uint32_t) buf_[0]) << 24) & 0xff000000)
			      | ((((uint32_t) buf_[1]) << 16) & 0x00ff0000)
				  | ((((uint32_t) buf_[2]) <<  8) & 0x0000ff00)
			      | ((((uint32_t) buf_[3]) <<  0) & 0x000000ff);
		}

		inline uint64_t read_uint64(const char* buf_)
		{
			return ((((uint64_t) buf_[0]) << 56) & 0xff00000000000000)
			     | ((((uint64_t) buf_[1]) << 48) & 0x00ff000000000000)
			     | ((((uint64_t) buf_[2]) << 40) & 0x0000ff0000000000)
			     | ((((uint64_t) buf_[3]) << 32) & 0x000000ff00000000)
			     | ((((uint64_t) buf_[4]) << 24) & 0x00000000ff000000)
			     | ((((uint64_t) buf_[5]) << 16) & 0x0000000000ff0000)
			     | ((((uint64_t) buf_[6]) <<  8) & 0x000000000000ff00)
			     | ((((uint64_t) buf_[7]) <<  0) & 0x00000000000000ff);
		}
	}

	namespace net {

		//! reverses the byte order of a 16 bit unsigned integer
		inline uint16_t reverse_byte_order(uint16_t a_)
		{
			return (a_ >> 8) | (a_ << 8);
		}

		//! reverses the byte order of a 32 bit unsigned integer
		inline uint32_t reverse_byte_order(uint32_t a_)
		{
			return ((a_ & 0xff000000)>>24) | ((a_ & 0x00ff0000)>>8)
				   | ((a_ & 0x0000ff00)<<8) | (a_<<24);
		}

		static std::pair<std::string, unsigned short> parse_host_port(const std::string& host_port_)
		{
			std::size_t colon_pos = host_port_.find(':');

			if (colon_pos != std::string::npos) {
				std::string host_part = host_port_.substr(0, colon_pos);
				std::string port_part = host_port_.substr(colon_pos + 1);
				return std::make_pair(host_part, (unsigned short) std::stoul(port_part));
			}

			throw std::invalid_argument("om::net::parse_host_port: invalid argument");
		}

		//! a media access control address (IEEE 802)
		class mac_addr
		{
//			friend class std::hash<mac_addr>;

		public:

			static const unsigned LEN = 6;

			//! constructs a mac_addr and sets the address 00:00:00:00:00:00
			mac_addr() = default;

			mac_addr(const mac_addr&) = default;
			mac_addr& operator=(const mac_addr&) = default;

			mac_addr(mac_addr&&) = default;
			mac_addr& operator=(mac_addr&&) = default;

			//! - bytes 0 and 1 are discarded
			explicit mac_addr(uint64_t addr_)
			{
				_addr[0] = (uint8_t)(addr_ >> (5 * 8));
				_addr[1] = (uint8_t)(addr_ >> (4 * 8));
				_addr[2] = (uint8_t)(addr_ >> (3 * 8));
				_addr[3] = (uint8_t)(addr_ >> (2 * 8));
				_addr[4] = (uint8_t)(addr_ >> (1 * 8));
				_addr[5] = (uint8_t)(addr_ >> (0 * 8));
			}

			explicit mac_addr(const unsigned char* buf_)
			{
				for (unsigned i = 0; i < LEN; i++) _addr[i] = buf_[i];
			}

			explicit mac_addr(const std::string& str_)
			{
				int temp[6] = {0};


				if (std::sscanf(str_.c_str(), "%x:%x:%x:%x:%x:%x",
								&temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]) != 6)
					throw std::invalid_argument("mac_addr: invalid address format");

				for (unsigned i = 0; i < 6; i++)
					_addr[i] = (uint8_t) temp[i];
			}

			//! casts to an 8 byte unsigned integer of the address (bytes 0,1 set to 0)
			explicit operator uint64_t() const
			{
				return to_uint64();
			}

			friend inline bool operator<(const mac_addr& lhs_, const mac_addr& rhs_)
			{
				return lhs_._addr[0] < rhs_._addr[0] || lhs_._addr[1] < rhs_._addr[1]
					   || lhs_._addr[2] < rhs_._addr[2] || lhs_._addr[3] < rhs_._addr[3]
					   || lhs_._addr[4] < rhs_._addr[4] || lhs_._addr[5] < rhs_._addr[5];
			}

			friend inline bool operator==(const mac_addr& lhs_, const mac_addr& rhs_)
			{
				return lhs_._addr[0] == rhs_._addr[0] && lhs_._addr[1] == rhs_._addr[1]
					   && lhs_._addr[2] == rhs_._addr[2] && lhs_._addr[3] == rhs_._addr[3]
					   && lhs_._addr[4] == rhs_._addr[4] && lhs_._addr[5] == rhs_._addr[5];
			}

			//! returns a std::string of the mac_addr in canonical form
			std::string to_string() const
			{
				std::stringstream ss;
				ss << std::hex
				   << std::setw(2) << std::setfill('0') << (int) _addr[0] << ':'
				   << std::setw(2) << std::setfill('0') << (int) _addr[1] << ':'
				   << std::setw(2) << std::setfill('0') << (int) _addr[2] << ':'
				   << std::setw(2) << std::setfill('0') << (int) _addr[3] << ':'
				   << std::setw(2) << std::setfill('0') << (int) _addr[4] << ':'
				   << std::setw(2) << std::setfill('0') << (int) _addr[5];
				return ss.str();
			}

			//! returns an 8 byte unsigned integer of the address (bytes 0,1 set to 0)
			uint64_t to_uint64() const
			{
				return  (uint64_t) _addr[0] << (5*8) | (uint64_t) _addr[1] << (4*8)
				      | (uint64_t) _addr[2] << (3*8) | (uint64_t) _addr[3] << (2*8)
				      | (uint64_t) _addr[4] << (1*8) | (uint64_t) _addr[5] << (0*8);
			}

			//! writes a mac_addr to a std::ostream in canonical form
			friend std::ostream& operator<<(std::ostream& os_, mac_addr& a_)
			{
				return (os_ << a_.to_string());
			}

			//! writes the mac_addr into a byte buffer
			void write(uint8_t* dst_)
			{
				dst_[0] = _addr[0];
				dst_[1] = _addr[1];
				dst_[2] = _addr[2];
				dst_[3] = _addr[3];
				dst_[4] = _addr[4];
				dst_[5] = _addr[5];
			}

		private:
			uint8_t _addr[6] = { 0 };
		};


		//! An internet protocol version 4 address (RFC 791 - https://tools.ietf.org/html/rfc791)
		//!
		//! This class does not convert between host and network byte order.
		class ip4_addr
		{
//			friend class std::hash<ip4_addr>;

		public:

			static const unsigned LEN = 4;

			static ip4_addr from_bytes(unsigned char* buf_)
			{
				return ip4_addr(buf_[0] << 24 | buf_[1] << 16 | buf_[2] << 8 | buf_[3] << 0);
			}

			static ip4_addr from_string(const char* addr_)
			{
				return ip4_addr(addr_);
			}

			static ip4_addr from_string(std::string addr_)
			{
				return ip4_addr(addr_);
			}

			static ip4_addr from_host(uint32_t addr_)
			{
				return ip4_addr(htonl(addr_));
			}

			static ip4_addr from_net(uint32_t addr_)
			{
				return ip4_addr(addr_);
			}

			//! constructs an ip4_addr and sets the address 0.0.0.0
			ip4_addr() = default;

			ip4_addr(const ip4_addr&) = default;
			ip4_addr& operator=(const ip4_addr&) = default;

			inline bool operator<(const ip4_addr& rhs_) const
			{
				return _addr < rhs_._addr;
			}

			inline bool operator==(const ip4_addr& rhs_) const
			{
				return _addr == rhs_._addr;
			}

			//! returns a std::string in dotted decimal notation
			std::string to_string() const
			{
				std::stringstream ss;
				ss << (_addr >> 0  & 0x000000ff) << '.';
				ss << (_addr >> 8  & 0x000000ff) << '.';
				ss << (_addr >> 16 & 0x000000ff) << '.';
				ss << (_addr >> 24 & 0x000000ff);
				return ss.str();
			}

			//! returns a 4 byte unsigned integer of the ip address
			uint32_t to_uint32() const
			{
				return _addr;
			}

			//! writes an ip4_addr to a std::ostream in dotted decimal notation
			friend std::ostream& operator<<(std::ostream& os_, const ip4_addr& addr_)
			{
				return (os_ << addr_.to_string());
			}

			static uint32_t parse(const char* str_)
			{
				uint32_t addr = 0;

				if((addr = inet_addr(str_)) == -1)
					throw std::invalid_argument("error");

				return addr;
			}

			static ip4_addr reverse_byte_order(const ip4_addr& addr_)
			{
				return ip4_addr::from_net(htonl(addr_._addr));
			}

		private:
			uint32_t _addr = 0;

			explicit ip4_addr(uint32_t addr_) : _addr(addr_) { }
			explicit ip4_addr(std::string addr_) : _addr(parse(addr_.c_str())) { }
			explicit ip4_addr(const char* addr_) : _addr(parse(addr_)) { }
		};

		//! base class for network packet headers
		class packet_header
		{
		public:
			packet_header() = delete;
			explicit packet_header(std::size_t len_)
				: _alloc(true), _buf(new unsigned char[len_]{0}), _len(len_) { }
			explicit packet_header(const unsigned char* buf_)
				: _buf(buf_) { }
			packet_header(const packet_header&) = delete;
			packet_header& operator=(const packet_header&) = delete;
			packet_header(packet_header&&) = default;
			packet_header& operator=(packet_header&&) = default;

			//! returns the length of the packet after initial parsing, including optional fields
			std::size_t len() const
			{
				return _len;
			}

			//! writes the packet to a byte buffer
			inline void write(unsigned char* buf_)
			{
				std::memcpy(buf_, _buf, this->len());
			}

			virtual ~packet_header()
			{
				if (_alloc)
					delete[] _buf;
			}

		protected:
			bool _alloc               = false;
			const unsigned char* _buf = nullptr;
			std::size_t _len          = 0;
		};

		//! a standard Ethernet header
		class ethernet_header : public packet_header
		{
		public:

			//! constructs an ethernet header with all fields set to 0
			ethernet_header()
				: packet_header(14), _eth((ether_header*) _buf)  { }

			//! constructs an ethernet header from a byte buffer
			explicit ethernet_header(const unsigned char* buf_)
				: packet_header(buf_), _eth((ether_header*) _buf)
			{
				_len = 14;
			}

			//! returns the frame's destination address
			mac_addr dest_addr() const
			{
				return mac_addr(_eth->ether_dhost);
			}

			//! sets the frame's destination address
			void set_dest_addr(mac_addr dest_addr_)
			{
				dest_addr_.write(_eth->ether_dhost);
			}

			//! returns the frame's source address
			mac_addr src_addr() const
			{
				return mac_addr(_eth->ether_shost);
			}

			//! sets the frame's source address
			void set_src_addr(mac_addr src_addr_)
			{
				src_addr_.write(_eth->ether_shost);
			}

			//! returns the frame's ether type
			uint16_t ether_type() const
			{
				return reverse_byte_order(_eth->ether_type);
			}

			//! sets the frame's ether type
			void set_ether_type(uint16_t ether_type_)
			{
				_eth->ether_type = reverse_byte_order(ether_type_);
			}

		private:
			ether_header* _eth = nullptr;
		};

		class arp_header : public packet_header
		{
		public:
			//! constructs an ethernet header with all fields set to 0
			arp_header()
				: packet_header(8 + 20), _ether_arp((ether_arp*) _buf)
			{ }

			//! constructs an ethernet header from a byte buffer
			explicit arp_header(const unsigned char* buf_)
				: packet_header(buf_), _ether_arp((ether_arp*) _buf)
			{
				_len = 28;
			}

			uint16_t hardware_type() const
			{
				return reverse_byte_order(_ether_arp->ea_hdr.ar_hrd);
			}

			uint16_t protocol_type() const
			{
				return reverse_byte_order(_ether_arp->ea_hdr.ar_pro);
			}

			uint16_t operation() const
			{
				return reverse_byte_order(_ether_arp->ea_hdr.ar_op);
			}

			mac_addr sender_hardware_addr() const
			{
				return mac_addr(_ether_arp->arp_sha);
			}

			ip4_addr sender_protocol_addr() const
			{
				return ip4_addr::from_bytes(_ether_arp->arp_spa);
			}

			mac_addr target_hardware_addr() const
			{
				return mac_addr(_ether_arp->arp_tha);
			}

			ip4_addr target_protocol_addr() const
			{
				return ip4_addr::from_bytes(_ether_arp->arp_tpa);
			}

		private:
			ether_arp* _ether_arp = nullptr;
		};

		//! an ip version 4 header
		class ip4_header : public packet_header
		{
		public:
			//! constructs an ip v4 header with all fields set to 0
			ip4_header()
				: packet_header(20), _ip((ip*) _buf)
			{
				_ip->ip_v  = 4;
				_ip->ip_hl = 5;
			}

			//! constructs an ip v4 header from a byte buffer
			explicit ip4_header(const unsigned char* buf_)
				: packet_header(buf_), _ip((ip*) _buf)
			{
				if (_ip->ip_hl != 5)
					_ip->ip_hl = 5;

				if (_ip->ip_v != 4)
					_ip->ip_v = 4;

				_len = _ip->ip_hl * 4;
			}

			void set_ip_v(unsigned ip_v_= 4)
            {
			    _ip->ip_v  = ip_v_;
            }

            void set_ip_hl(unsigned ip_hl_ = 5)
            {
			    _ip->ip_hl = ip_hl_;
            }

			uint16_t total_len() const
			{
				return reverse_byte_order(_ip->ip_len);
			}

			void set_total_len(uint16_t total_len_)
			{
				_ip->ip_len = reverse_byte_order(total_len_);
			}

			uint16_t id() const
			{
				return reverse_byte_order(_ip->ip_id);
			}

			void set_id(uint16_t id_)
			{
				_ip->ip_id = reverse_byte_order(id_);
			}

			uint8_t ttl() const
			{
				return _ip->ip_ttl;
			}

			void set_ttl(uint8_t ttl_)
			{
				_ip->ip_ttl = ttl_;
			}

			uint8_t proto() const
			{
				return _ip->ip_p;
			}

			void set_proto(uint8_t proto_)
			{
				_ip->ip_p = proto_;
			}

			ip4_addr src_addr() const
			{
				return ip4_addr::from_net(_ip->ip_src.s_addr);
			}

			void set_src_addr(const ip4_addr& src_addr_)
			{
				_ip->ip_src.s_addr = src_addr_.to_uint32();
			}

			ip4_addr dest_addr() const
			{
				return ip4_addr::from_net(_ip->ip_dst.s_addr);
			}

			void set_dest_addr(const ip4_addr& dest_addr_)
			{
				_ip->ip_dst.s_addr = dest_addr_.to_uint32();
			}

		private:
			ip* _ip = nullptr;
		};

		//! an icmp packet header
		class icmp_header : public packet_header
		{
		public:
			//! constructs an icmp header with all fields set to 0
			icmp_header()
				: packet_header(8), _icmp((icmp*) _buf)  { }

			//! constructs an icmp header from a byte buffer
			explicit icmp_header(const unsigned char* buf_)
				: packet_header(buf_), _icmp((icmp*) _buf)
			{
				_len = 8;
			}

			uint8_t type() const
			{
				return _icmp->icmp_type;
			}

			uint8_t code() const
			{
				return _icmp->icmp_code;
			}

		private:
			icmp* _icmp = nullptr;
		};

		//! a tcp packet header
		class tcp_header : public packet_header
		{
		public:
			//! constructs a tcp header

			//! all fields except internet header length and protocol version are set to 0
			tcp_header()
				: packet_header(20), _tcp((tcphdr*) _buf)  { }

			//! constructs a tcp header from a byte buffer
			explicit tcp_header(const unsigned char* buf_)
				: packet_header(buf_), _tcp((tcphdr*) _buf)
			{
				_len = 20;
			}

			uint16_t src_port() const
			{
				return reverse_byte_order(_tcp->th_sport);
			}

			uint16_t dest_port() const
			{
				return reverse_byte_order(_tcp->th_dport);
			}

			uint32_t seq_no() const
			{
				return (reverse_byte_order(_tcp->th_seq));
			}

			uint32_t ack_no() const
			{
				return (reverse_byte_order(_tcp->th_ack));
			}

			uint8_t flags() const
			{
				return _tcp->th_flags;
			}

			uint16_t window_size() const
			{
				return reverse_byte_order(_tcp->th_win);
			}

		private:
			tcphdr* _tcp = nullptr;
		};

		//! a udp datagram header
		class udp_header : public packet_header
		{
		public:
			//! constructs a tcp header with all fields set to 0
			udp_header()
				: packet_header(8), _udp((udphdr*) _buf)  { }

			//! constructs a tcp header from a byte buffer
			explicit udp_header(const unsigned char* buf_)
				: packet_header(buf_), _udp((udphdr*) _buf)
			{
				_len = 8;
			}

			uint16_t src_port() const
			{
				return reverse_byte_order(_udp->uh_sport);
			}

			void set_src_port(uint16_t src_port_)
			{
				_udp->uh_sport = reverse_byte_order(src_port_);
			}

			uint16_t dest_port() const
			{
				return reverse_byte_order(_udp->uh_dport);
			}

			void set_dest_port(uint16_t dest_port_)
			{
				_udp->uh_dport = reverse_byte_order(dest_port_);
			}

			uint16_t payload_length() const
			{
				return reverse_byte_order(_udp->uh_ulen);
			}

			void set_payload_length(uint16_t payload_length_)
			{
				_udp->uh_ulen = reverse_byte_order(payload_length_);
			}

		private:
			udphdr* _udp = nullptr;
		};

		class ip4_flow_key
		{
		public:

			ip4_flow_key()                               = default;
			ip4_flow_key(const ip4_flow_key&)            = default;
			ip4_flow_key& operator=(const ip4_flow_key&) = default;

			explicit ip4_flow_key(const ip4_addr& ip_src_, const ip4_addr& ip_dst_,
				uint16_t tp_src_, uint16_t tp_dst_, uint8_t ip_proto_)
				: _ip_src(ip_src_),
				  _ip_dst(ip_dst_),
				  _tp_src(tp_src_),
				  _tp_dst(tp_dst_),
				  _ip_proto(ip_proto_) { }

			bool operator==(const struct ip4_flow_key& other_) const
			{
				return _ip_src   == other_._ip_src
					&& _ip_dst   == other_._ip_dst
					&& _ip_proto == other_._ip_proto
					&& _tp_src   == other_._tp_src
					&& _tp_dst   == other_._tp_dst;
			}

			bool operator<(const struct ip4_flow_key& other_) const
			{
				return _ip_src   < other_._ip_src
					|| _ip_dst   < other_._ip_dst
					|| _ip_proto < other_._ip_proto
					|| _tp_src   < other_._tp_src
					|| _tp_dst   < other_._tp_dst;
			}

			bool operator!=(const struct ip4_flow_key& other_) const
			{
				return !(*this == other_);
			}

			static ip4_flow_key from_ip4_bytes(const unsigned char* buf_)
			{
				ip4_flow_key flow_key;
				ip4_header ip(buf_);

				flow_key._ip_src   = ip.src_addr();
				flow_key._ip_dst   = ip.src_addr();
				flow_key._ip_proto = ip.proto();

				if (ip.proto() == 6) {
					om::net::tcp_header tcp(buf_ + 20);
					flow_key._tp_src = tcp.src_port();
					flow_key._tp_dst = tcp.dest_port();
				} else if (ip.proto() == 17) {
					om::net::udp_header udp(buf_ + 20);
					flow_key._tp_src = udp.src_port();
					flow_key._tp_dst = udp.dest_port();
				}

				return flow_key;
			}

			ip4_addr ip_src() const
			{
				return _ip_src;
			}

			ip4_addr ip_dst() const
			{
				return _ip_dst;
			}

			uint16_t tp_src() const
			{
				return _tp_src;
			}

			uint16_t tp_dst() const
			{
				return _tp_dst;
			}

			uint8_t ip_proto() const
			{
				return _ip_proto;
			}

		private:
			ip4_addr _ip_src;
			ip4_addr _ip_dst;
			uint16_t _tp_src   = 0;
			uint16_t _tp_dst   = 0;
			uint8_t  _ip_proto = 0;
		};

		//! an unix internet socket
		class socket : public sys::file_descriptor
		{
		public:

			enum class type { stream = SOCK_STREAM, dgram = SOCK_DGRAM };

			explicit socket(type type_ = type::stream)
			{
				if ((_fd = ::socket(AF_INET, (int) type_, 0)) == -1)
					throw std::runtime_error("socket: could not open: errno: "
							  + std::to_string(errno));
			}

			void bind(const std::string& ip_addr_, unsigned short port_ = 0)
			{
				struct sockaddr_in in_addr {};
				in_addr.sin_family = AF_INET;
				in_addr.sin_addr.s_addr = htonl(inet_addr(ip_addr_.c_str()));
				in_addr.sin_port = htons(port_);

				if (::bind(_fd, (struct sockaddr*) &in_addr, sizeof(in_addr)))
					throw std::runtime_error("socket: could not bind: errno: "
							  + std::to_string(errno));
			}

			unsigned send_to(const std::string& ip_dst_, unsigned short tp_dst_,
							 const unsigned char* buf_, unsigned len_, int flags_ = 0)
			{
				struct sockaddr_in dest {};
				dest.sin_family = AF_INET;
				dest.sin_port = htons(tp_dst_);
				dest.sin_addr.s_addr = inet_addr(ip_dst_.c_str());

				ssize_t len = ::sendto(_fd, buf_, len_, flags_, (const sockaddr*) &dest,
					sizeof(dest));

				if (len == -1)
					throw std::runtime_error("socket: could not send: errno: "
							  + std::to_string(errno));

				return (unsigned) len;
			}


			unsigned receive(unsigned char* buffer_, unsigned len_, int flags_ = 0)
			{
				ssize_t len = ::recv(_fd, buffer_, len_, flags_);

				if (len == -1)
					throw std::runtime_error("socket: could not receive: errno: "
											 + std::to_string(errno));

				return (unsigned) len;
			}

			bool is_open()
			{
				return _fd != -1;
			}

			void close()
			{
				::close(_fd);
				_fd = -1;
			}

			~socket()
			{
				if (_fd > 0) close();
			}
		};
	}

	namespace async {

		//! a wrapper around the poll() system call
		class poll
		{
		public:
			enum class event : short { in = POLLIN, out = POLLOUT };

			using callback_t = std::function<void (int, event)>;

			poll() = default;

			explicit poll(sys::file_descriptor fd_, callback_t cb_, event events_ = event::in)
			{
				add_fd(fd_, std::move(cb_), events_);
			}

			explicit poll(int fd_, callback_t cb_, event events_ = event::in)
			{
				add_fd(fd_, std::move(cb_), events_);
			}

			void add_fd(sys::file_descriptor fd_, callback_t cb_, event events_ = event::in)
			{
				_fds[_count_fds++] = {
					.fd = fd_.fd(), .events = static_cast<short>((short) events_), .revents = 0
				};

				_callbacks[fd_.fd()] = std::move(cb_);
			}

			void add_fd(int fd_, callback_t cb_, event events_ = event::in)
			{
				_fds[_count_fds++] = {
					.fd = fd_, .events = static_cast<short>((short) events_), .revents = 0
				};

				_callbacks[fd_] = std::move(cb_);
			}

			void block()
			{
				for (int r = 0; !_stop && r >= 0; r = ::poll(_fds, _count_fds, 1000)) {
					if (r > 0) {
						for (unsigned i = 0; i < _count_fds; i++) {

							if (_fds[i].revents & POLLOUT)
								_callbacks[_fds->fd](_fds[i].fd, event::out);

							if (_fds[i].revents & POLLIN)
								_callbacks[_fds->fd](_fds[i].fd, event::in);
						}
					} else { continue; }
				}
			}

			void stop()
			{
				_stop = true;
			}

		private:
			bool _stop = false;
			static const unsigned _FDS_PREALLOC = 8;
			std::map<int, std::function<void (int, event)>> _callbacks = {};
			unsigned _count_fds = 0;
			struct pollfd _fds[_FDS_PREALLOC] = {};
		};

		inline short operator&(const poll::event& lhs_, const poll::event& rhs_)
		{
			return ((short) lhs_ & (short) rhs_);
		}
	}

	namespace file {

		inline unsigned long size(const std::string& file_name_)
		{
			struct stat st {};

			if (stat(file_name_.c_str(), &st) < 0)
				throw std::runtime_error("size: could not determine file size: errno: "
										 + std::to_string(errno));

			return (unsigned long)(st.st_size);
		}

		class _file_stream
		{
		public:
			//! opens a file in the specified openmode, throws std::runtime_error upon error
			explicit _file_stream(const std::string& file_name_, std::ios::openmode open_mode_)
				: _stream(file_name_, open_mode_)
			{
				if (!_stream.is_open())
					throw std::runtime_error("om::file::_file_stream: could not open " +file_name_);
			}

			//! closes the underlying file
			virtual void close()
			{
				_stream.close();
			}

			//! closes the underlying file if it is open
			virtual ~_file_stream()
			{
				if (_stream.is_open())
					_stream.close();
			}

		protected:

			//! returns true if the file pointer was advanced beyond the end of the file
			bool _eof()
			{
				return _stream.tellg() == std::istream::pos_type(-1);
			}

			//! sets the read cursor to the beginning of the file
			void _reset()
			{
				_stream.clear();
				_stream.seekg(0, std::ios::beg);
			}

			std::fstream _stream;
		};

		template <typename T>
		class simple_binary_reader : public _file_stream
		{
		public:
			explicit simple_binary_reader(const std::string& file_name_, bool use_buffer_ = false)
				: _file_stream(file_name_, std::ios::binary | std::ios::in),
				  _use_buffer(use_buffer_),
				  _data()

			{
				T t;

				if (_use_buffer) {
					while (!_eof()) {
						_stream.read((char*) &t, sizeof(T));
						_data.push_back(t);
					}
					_iter = std::begin(_data);
				}
			}

			bool next(T& t_)
			{
				if (_use_buffer)
					t_ = *(_iter++);
				else
					_stream.read((char*) &t_, sizeof(T));

				return !done();
			}

			void reset()
			{
				if (_use_buffer)
					_iter = std::begin(_data);
				else
					_reset();
			}

			bool done()
			{
				return _use_buffer ? _iter == _data.end() : _eof();
			}

			virtual ~simple_binary_reader() = default;

		private:
			bool _use_buffer;
			std::vector<T> _data;
			typename std::vector<T>::const_iterator _iter;
		};

		template <typename T>
		class simple_binary_writer : public _file_stream
		{
		public:
			explicit simple_binary_writer(const std::string& file_name_)
				: _file_stream(file_name_, std::ios::binary | std::ios::out) { }

			//! writes t_ to the file
			void write(const T& t_)
			{
				_stream.write((char*)&t_, sizeof(T));
			}
		};
	}

	namespace concurrency {

		//! a simple thread-safe wrapper around std::queue
		template<typename T>
		class queue
		{
		public:
			queue() = default;

			//! enqueues an element
			void enqueue(T item_)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_queue.push(std::move(item_));
				_condition.notify_one();
			}

			//! dequeues an element, blocks if the queue is empty
			void dequeue_wait(T& item_)
			{
				std::unique_lock<std::mutex> lock(_mutex);
				_condition.wait(lock, [this]() { return !_queue.empty(); });
				item_ = std::move(_queue.front());
				_queue.pop();
			}

			//! tries to dequeue an element, returns false if queue is empty
			bool dequeue(T& item_)
			{
				std::lock_guard<std::mutex> lock(_mutex);

				if(_queue.empty())
					return false;

				item_ = std::move(_queue.front());
				_queue.pop();
				return true;
			}

			//! checks if the queue is empty
			bool empty() const
			{
				std::lock_guard<std::mutex> lock(_mutex);
				return _queue.empty();
			}

		private:
			mutable std::mutex _mutex;
			std::queue<T> _queue;
			std::condition_variable _condition;
		};

		class thread_joiner
		{
		public:
			explicit thread_joiner(std::vector<std::thread>& threads_) : _threads(threads_) { }

			~thread_joiner()
			{
				for (auto& t : _threads)
					if (t.joinable()) t.join();
			}

		private:
			std::vector<std::thread>& _threads;
		};

		class thread_pool
		{
		public:
			explicit thread_pool(unsigned thread_count_ = std::thread::hardware_concurrency())
				: _done(false), _joiner(_threads)
			{
				try {
					for (unsigned i = 0; i < thread_count_; i++)
						_threads.emplace_back(&thread_pool::_worker_thread, this);
				} catch(...) {
					_done = true;
					throw;
				}
			}

			~thread_pool()
			{
				_done = true;
			}

			template<typename Fx>
			void submit(Fx f_)
			{
				_task_queue.enqueue(std::function<void()>(f_));
			}

		private:
			std::atomic_bool _done;
			queue<std::function<void()>> _task_queue;
			std::vector<std::thread> _threads;
			thread_joiner _joiner;

			void _worker_thread()
			{
				while (!_done) {
					std::function<void()> task;

					if (_task_queue.dequeue(task)) {
						task();
					} else {
						std::this_thread::yield();
					}
				}
			}
		};
	}

	namespace etc {

		static std::string format_ptr(void* ptr_)
		{
			std::stringstream ss;
			ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << (uint64_t) ptr_;
			return ss.str();
		}

		static void print_bytes(const char* buf_, unsigned len_, bool format_ = true,
						 std::ostream& os_ = std::cout) // handling of ios flags not thread-safe
		{
			if (format_) os_ << "    0  ";

			for (auto i = 0; i < len_; i++) {
				if (format_ && i > 0 && i % 8 == 0)
					os_ << std::endl << std::setw(5) << std::setfill(' ') << std::dec << i << "  ";
				os_ << " " << std::hex << std::setfill('0') << std::setw(2)
					<< ((int) buf_[i] & 0xff);
			}

			os_ << std::dec << std::endl;
		}

		static std::string format_bytes(const char* buf_, unsigned len_, bool format_ = true)
		{
			std::stringstream ss;
			print_bytes(buf_, len_, format_, ss);
			return ss.str();
		}

		static std::vector<std::string> tokenize(const std::string& str_,
		    const std::string& del_ = ",")
        {
			std::regex  reg("\\" + del_ +"+");
			std::sregex_token_iterator iter(str_.begin(), str_.end(), reg, -1), end;
			return std::vector<std::string>(iter, end);
        }

		static std::chrono::time_point<std::chrono::high_resolution_clock> now()
		{
			return std::chrono::high_resolution_clock::now();
		}

		template <typename Rep = std::chrono::seconds>
		static unsigned long long now_since_epoch()
        {
			return std::chrono::duration_cast<Rep>(now().time_since_epoch()).count();
        }

        template <typename Rep = std::chrono::seconds>
        static unsigned long long since(
            std::chrono::time_point<std::chrono::high_resolution_clock> start_)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto dur = std::chrono::duration_cast<Rep>(end - start_);
            return (unsigned) dur.count();
        }

		static unsigned long long microseconds_since(
			std::chrono::time_point<std::chrono::high_resolution_clock> start_)
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
			return (unsigned) dur.count();
		}

		static double seconds_since(std::chrono::time_point<std::chrono::high_resolution_clock> start_)
		{
			auto end = std::chrono::high_resolution_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
			return (double) dur.count() / 1000000;
		}

		//! formats a unix timestamps (in seconds) using a strftime format (default: %F %T)
		static std::string format_timestamp(unsigned timestamp_,
			const std::string& format_ = "%F %T")
        {
		    auto time = (std::time_t) timestamp_;
		    std::stringstream ss;
		    ss << std::put_time(std::gmtime(&time), format_.c_str());
		    return ss.str();
        }

		template <typename F>
		static double runtime(F f_)
		{
			auto start = std::chrono::high_resolution_clock::now();
			f_();
			auto end = std::chrono::high_resolution_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			return (double) dur.count() / 1000000;
		}

		static void file_by_line(const std::string& file_name_,
			const std::function<void (const std::string&)>&& handler_)
		{
			std::ifstream file(file_name_);
			std::string line;

			while (std::getline(file, line))
				handler_(line);

			file.close();
		}
	}
}

namespace std {
	template<> struct hash<om::net::mac_addr>
	{
		std::size_t operator()(om::net::mac_addr const& a) const
		{
			return a.to_uint64();
		}
	};

	template<> struct hash<om::net::ip4_addr>
	{
		std::size_t operator()(om::net::ip4_addr const& a) const
		{
			return a.to_uint32();
		}
	};

	template<> struct hash<om::net::ip4_flow_key>
	{
		inline std::size_t operator()(const om::net::ip4_flow_key& d_) const noexcept
		{
			// pack into two different long unsigned integers
			uint64_t a = 0, b = 0;
			a |= (uint64_t) d_.ip_src().to_uint32() << 32;
			a |= (uint64_t) d_.ip_dst().to_uint32() <<  0;
			b |= (uint64_t) d_.tp_src()             << 24;
			b |= (uint64_t) d_.tp_dst()             <<  8;
			b |= (uint64_t) d_.ip_proto()           <<  0;
			return ((size_t) a xor ((size_t) b + 0x9e3779b9 + (a << 6) + (b >> 2)));
		}
	};
}

#endif
