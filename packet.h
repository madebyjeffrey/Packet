//
// Created by Jeffrey Drake on 2015-11-18.
//

#ifndef PACKET_PACKET_H
#define PACKET_PACKET_H

#include <vector>
#include <array>
#include <ostream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <type_traits>

namespace Packet {


    class Reader {
        std::istream &stream;

    public:
        Reader(std::istream &is) : stream(is) { }


        std::string read_bytes(size_t size) {
            //uint32_t size_;

            //stream.read(reinterpret_cast<char *>(&size_), 4);

            /*if (size_ != size) {
                std::stringstream error;
                error << "Argument size mismatch, from stream: " << size << " expected: " << size;
                throw std::invalid_argument(error.str());
            }*/

            std::string::value_type buffer[size] ;

            stream.read(buffer, size);

            //std::copy(buffer, buffer + size, begin(data));

            return std::string(buffer, buffer+size);
        }

        template<typename T>
        std::enable_if_t<std::is_fundamental<T>::value, void> read(T &f) {
            constexpr size_t sz = sizeof(T);

            std::string data = read_bytes(sz);

            f = *reinterpret_cast<T const *>(data.data());
        }

        void read(std::string &f) {
            uint16_t size;

            read(size);

            if (size & 0x8000) {
                auto bytes = read_bytes(size & 0x7FFF);
                uint16_t sz = *reinterpret_cast<uint16_t const *>(bytes.data());
                if (sz > (size & 0x7FFF) - 2) {
                    std::stringstream error;
                    error << "Argument size mismatch, string max is " << (size & 0x7FFF) << " and actual stated size is " << sz;
                    throw std::invalid_argument(error.str());
                } else {
                    f = std::string(begin(bytes)+2, begin(bytes)+sz+2);
                }
            } else {
                f = read_bytes(size);
            }
        }
    };


    class Writer {
        std::ostream &stream;

    public:
        Writer(std::ostream &os) : stream(os) { }

        void write_bytes(std::string const &data) {
            stream.write(data.data(), data.size());
        }

        template<typename T>
        std::enable_if_t<std::is_fundamental<T>::value, void> write(T const f) {
            constexpr size_t sz = sizeof(T);

            std::string::value_type const *start = reinterpret_cast<std::string::value_type const *>(&f);

            auto buf = std::string(start, start + sz);

            write_bytes(buf);
        }



        void write(std::string const &data) {
            uint32_t const size = data.size();

            if (size < 0x7FFF) {
                uint16_t sz = static_cast<uint16_t>(size);
                write(sz);
                write_bytes(data);
            } else {
                std::stringstream error;
                error << "Argument size mismatch, string max is " << 0x7FFF << " and actual size is " << size;
                throw std::invalid_argument(error.str());
            }
        }

        /* Writes a string of fixed size with the actual size with the first word after */
        void write_fixed_string(std::string const &data, size_t fixed_size) {
            if (data.size() == fixed_size && fixed_size <= 0x7FFF) {
                write(data);
            } else if (data.size() + 2 <= fixed_size ) {
                uint16_t sz = (fixed_size & 0x7FFF) | 0x8000;
                write(sz);
                sz = data.size() & 0x7FFF;
                write(sz);
                write_bytes(data);
                auto remainder = fixed_size - 2 - data.size(); // this value seems to be wrong
                if (remainder > 0 && remainder < fixed_size)
                    write(std::string(remainder, '\0'));
            } else {
                std::stringstream error;
                error << "Argument size mismatch, fixed size is "
                    << fixed_size << " and string size is " << data.size() << " but it must be less than fixed size by 2.";
                throw std::invalid_argument(error.str());
            }
        }


    };

    class Packet {
    } packet;

    Reader operator>>(std::istream &is, Packet const) {
        return Reader(is);
    }

    Writer operator<<(std::ostream &os, Packet const) {
        return Writer(os);
    }


    template<typename T>
    inline std::enable_if_t<std::is_fundamental<T>::value, Writer> operator<<(Writer w, T f) {
        w.write(std::forward<T>(f));

        return w;
    }

    template<typename T>
    inline std::enable_if_t<std::is_fundamental<T>::value, Reader> operator>>(Reader r, T &f) {
        r.read(f);

        return r;
    }

    /*inline Writer operator<<(Writer w, std::string const &f) {
        w.write(f);

        return w;
    }*/

}


#endif //PACKET_PACKET_H
