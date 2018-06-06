
#ifndef LIBOM2_H
#define LIBOM2_H

#include <arpa/inet.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace om {

	namespace sys {
		class file_descriptor
		{
		public:
			inline int fd() const { return _fd; }
		protected:
			int _fd = -1;
		};
	}

	namespace net {
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
