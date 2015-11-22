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

        std::string read() {
            uint32_t size;

            stream.read(reinterpret_cast<char *>(&size), 4);

            std::string::value_type buffer[size] ;

            stream.read(buffer, size);

            return std::string(buffer, buffer+size);

        }

        std::string read(size_t size) {
            uint32_t size_;

            stream.read(reinterpret_cast<char *>(&size_), 4);

            if (size_ != size) {
                std::stringstream error;
                error << "Argument size mismatch, from stream: " << size << " expected: " << size;
                throw std::invalid_argument(error.str());
            }

            std::string::value_type buffer[size] ;

            stream.read(buffer, size);

            //std::copy(buffer, buffer + size, begin(data));

            return std::string(buffer, buffer+size);
        }
    };


    class Writer {
        std::ostream &stream;

    public:
        Writer(std::ostream &os) : stream(os) { }

        void write(std::string const &data) {
            uint32_t const size = data.size();
            stream.write(reinterpret_cast<char const *>(&size), sizeof(size));
            stream.write(data.data(), size);
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
        constexpr size_t sz = sizeof(T);

        std::string::value_type *start = reinterpret_cast<std::string::value_type *>(&f);

        auto buf = std::string(start, start + sz);

        w.write(buf);

        return w;
    }

    template<typename T>
    inline std::enable_if_t<std::is_fundamental<T>::value, Reader> operator>>(Reader r, T &f) {
        constexpr size_t sz = sizeof(T);

        std::string data = r.read(sz);

        f = *reinterpret_cast<T const *>(data.data());

        return r;
    }

    inline Writer operator<<(Writer w, std::string const &f) {
        w.write(f);

        return w;
    }

}


#endif //PACKET_PACKET_H
