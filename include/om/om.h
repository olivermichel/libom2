
#ifndef LIBOM2_H
#define LIBOM2_H

#include <arpa/inet.h>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

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

		//! a media access control address (IEEE 802)
		class mac_addr
		{
		public:

			static const unsigned LEN = 6;

			//! constructs a mac_addr and sets the address 00:00:00:00:00:00
			mac_addr() = default;

			mac_addr(const mac_addr&) = default;
			mac_addr& operator=(const mac_addr&) = default;

			mac_addr(mac_addr&&) = default;
			mac_addr& operator=(mac_addr&&) = default;

			//! - bytes 0 and 1 are discarded
			mac_addr(uint64_t addr_)
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
			std::string str() const
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

			//! writes a mac_addr to a std::ostream in canonical form
			friend inline std::ostream& operator<<(std::ostream& os_, mac_addr& a_)
			{
				return os_ << (std::to_string((int) a_._addr[0])
							   + ":" + std::to_string((int) a_._addr[1])
							   + ":" + std::to_string((int) a_._addr[2])
							   + ":" + std::to_string((int) a_._addr[3])
							   + ":" + std::to_string((int) a_._addr[4])
							   + ":" + std::to_string((int) a_._addr[5]));
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


		//! an internet protocol version 4 address (RFC 791 - https://tools.ietf.org/html/rfc791)
		class ip4_addr
		{
		public:

			static const unsigned LEN = 4;

			//! constructs an ip4_addr and sets the address 0.0.0.0
			ip4_addr() = default;

			ip4_addr(const ip4_addr& copy_from_) = default;
			ip4_addr& operator=(const ip4_addr& copy_from_) = default;

			//! constructs an ip4_addr
			//! \param addr_ address as 4 byte unsigned integer
			ip4_addr(uint32_t addr_) : _addr(htonl(addr_)) { }

			//! constructs an ip4_addr
			//! \param addr_ address as char array in dotted decimal notation
			ip4_addr(const char* addr_)
			{
				if ((_addr = inet_addr(addr_)) == -1)
					throw std::invalid_argument("ip4_addr: invalid argument " + std::string(addr_));
			}

			//! constructs an ip4_addr
			//! \param addr_ pointer to a byte buffer
			ip4_addr(const unsigned char* buf_)
				: _addr(buf_[0] << 24 | buf_[1] << 16 | buf_[2] << 8 | buf_[3]) { }

			//! constructs an ip4_addr
			//! \param addr_ address as std::string in dotted decimal notation
			ip4_addr(const std::string& addr_)
			{
				if ((_addr = inet_addr(addr_.c_str())) == -1)
					throw std::invalid_argument("ip4_addr: invalid argument " + addr_);
			}

			//! casts an ip4_addr object to a 4 byte unsigned integer in network byte order
			inline operator uint32_t() const
			{
				return _addr;
			}

			inline bool operator<(const ip4_addr& rhs_)
			{
				return _addr < rhs_._addr;
			}

			inline bool operator==(const ip4_addr& rhs_)
			{
				return _addr == rhs_._addr;
			}

			//! returns a std::string in dotted decimal notation
			std::string str_desc() const
			{
				std::stringstream ss;
				ss << (int)(_addr >> 0 & 0xff) << "." << (int)(_addr >> 8 & 0xff)
				   << '.' << (int)(_addr >> 16 & 0xff) << '.' <<  (int)(_addr >> 24 & 0xff);
				return ss.str();
			}

			//! writes an ip4_addr to a std::ostream in dotted decimal notation
			friend std::ostream& operator<<(std::ostream& os_, const ip4_addr& addr)
			{
				return (os_ << (int)(addr._addr >> 0 & 0xff)  << "."
							<< (int)(addr._addr >> 8 & 0xff)  << '.'
							<< (int)(addr._addr >> 16 & 0xff) << '.'
							<< (int)(addr._addr >> 24 & 0xff));
			}

		private:
			uint32_t _addr = 0;
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
				: packet_header(8 + 20), _arp_hdr((arphdr*) _buf), _ether_arp((ether_arp*) _buf + 8)
			{ }

			//! constructs an ethernet header from a byte buffer
			explicit arp_header(const unsigned char* buf_)
				: packet_header(buf_), _arp_hdr((arphdr*) _buf), _ether_arp((ether_arp*) _buf + 8)
			{
				_len = 28;
			}

		private:
			arphdr* _arp_hdr = nullptr;
			ether_arp* _ether_arp = nullptr;
		};

		//! an ip version 4 header
		class ip4_header : public packet_header
		{
		public:
			//! constructs an ip v4 header with all fields set to 0
			ip4_header()
				: packet_header(20), _ip((ip*) _buf)  { }

			//! constructs an ip v4 header from a byte buffer
			explicit ip4_header(const unsigned char* buf_)
				: packet_header(buf_), _ip((ip*) _buf)
			{
				_len = _ip->ip_hl * 4;
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
				return ip4_addr(reverse_byte_order(_ip->ip_src.s_addr));
			}

			void set_src_addr(const ip4_addr& src_addr_)
			{
				_ip->ip_src.s_addr = src_addr_;
			}

			ip4_addr dest_addr() const
			{
				return ip4_addr(reverse_byte_order(_ip->ip_dst.s_addr));
			}

			void set_dest_addr(const ip4_addr& dest_addr_)
			{
				_ip->ip_dst.s_addr = dest_addr_;
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

		private:
			icmp* _icmp = nullptr;
		};

		//! a tcp packet header
		class tcp_header : public packet_header
		{
		public:
			//! constructs a tcp header with all fields set to 0
			tcp_header()
				: packet_header(20), _tcp((tcphdr*) _buf)  { }

			//! constructs a tcp header from a byte buffer
			explicit tcp_header(const unsigned char* buf_)
				: packet_header(buf_), _tcp((tcphdr*) _buf)
			{
				_len = 20;
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

		private:
			udphdr* _udp = nullptr;
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

			void bind(std::string ip_addr_, unsigned short port_ = 0)
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
			enum class event : short { in = POLL_IN, out = POLLOUT };

			using callback_t = std::function<void (int, event)>;

			poll() = default;

			poll(sys::file_descriptor fd_, callback_t cb_, event events_ = event::in)
			{
				add_fd(fd_, cb_, events_);
			}

			void add_fd(sys::file_descriptor fd_, callback_t cb_, event events_ = event::in)
			{
				_fds[_count_fds++] = {
					.fd = fd_.fd(), .events = static_cast<short>((short) events_), .revents = 0
				};

				_callbacks[fd_.fd()] = std::move(cb_);
			}

			void block()
			{
				for (int r = 0; r >= 0; r = ::poll(_fds, _count_fds, 1000)) {
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

		private:
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
}

#endif
