
#include <iostream>
#include <sstream>

#include "gtest/gtest.h"

#include "Packet.h"

using namespace std;

class PacketTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    PacketTest() {
        // You can do set-up work for each test here.
    }

    virtual ~PacketTest() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }

    istringstream i;
    ostringstream o;

};

TEST_F(PacketTest, TestBasicStream1) {
    auto w = Packet::Writer(o);

    w.write("hello");

    ASSERT_EQ(5+4, o.tellp());

    i.str(o.str());

    auto r = Packet::Reader(i);

    auto res = r.read(5);

    EXPECT_EQ("hello", res);
}

TEST_F(PacketTest, TestStreamOneParameterFloat) {
    float value = 5.0f;
    o << Packet::packet << value;
    ASSERT_EQ(4+4, o.tellp());
    i.str(o.str());

    float f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

// 0101...
TEST_F(PacketTest, TestStreamOneParameterUInt32odd) {
    uint32_t value = 0x55555555u;
    o << Packet::packet << value;
    ASSERT_EQ(4+4, o.tellp());
    i.str(o.str());

    uint32_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

// 1010...
TEST_F(PacketTest, TestStreamOneParameterUInt32even) {
    uint32_t value = 0xAAAAAAAAu;
    o << Packet::packet << value;
    ASSERT_EQ(4+4, o.tellp());
    i.str(o.str());

    uint32_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

TEST_F(PacketTest, TestStreamOneParameterUInt64even) {
    uint64_t value = 0xAAAAAAAAAAAAAAAAul;
    o << Packet::packet << value;
    ASSERT_EQ(8+4, o.tellp());
    i.str(o.str());

    uint64_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

TEST_F(PacketTest, TestStreamOneParameterUInt64odd) {
    uint64_t value = 0x5555555555555555ul;
    o << Packet::packet << value;
    ASSERT_EQ(8+4, o.tellp());
    i.str(o.str());

    uint64_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

TEST_F(PacketTest, TestStreamTwoParameters) {
    float value1 = -46.4930f;
    uint64_t value2 = 48385038;

    o << Packet::packet << value1 << value2;
    ASSERT_EQ(4+8+4+4, o.tellp());

    i.str(o.str());

    float value1out;
    uint64_t value2out;

    i >> Packet::packet >> value1out >> value2out;

    ASSERT_EQ(value1, value1out);
    ASSERT_EQ(value2, value2out);
}

struct test {
    float v1;
    int v2;
    long v3;
};

Packet::Reader operator>>(Packet::Reader r, test &t) {
    // 4+4+8+3*4 = 28 + 4 for the outer packet
    auto s = r.read(28);
    auto inp = istringstream(s);
    inp >> Packet::packet >> t.v1 >> t.v2 >> t.v3;
    return r;
}

Packet::Writer operator<<(Packet::Writer w, test const t) {
    auto outp = ostringstream();
    outp << Packet::packet << t.v1 << t.v2 << t.v3;
    w.write(outp.str());

    return w;
}


TEST_F(PacketTest, TestStreamAggregate) {
    test values { 3.14159f, -3023943, 0xDEADBEEFDEADBEF };
    o << Packet::packet << values;
    ASSERT_EQ(32, o.tellp());
    i.str(o.str());

    test values2;
    i >> Packet::packet >> values2;

    ASSERT_EQ(values.v1, values2.v1);
    ASSERT_EQ(values.v2, values2.v2);
    ASSERT_EQ(values.v3, values2.v3);


}
