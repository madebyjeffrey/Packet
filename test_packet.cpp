
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

/*TEST_F(PacketTest, TestStrings) {
    auto w = Packet::Writer(o);

    w.write("hello");

    ASSERT_EQ(5+4, o.tellp());

    i.str(o.str());

    auto r = Packet::Reader(i);

    auto res = r.read_bytes(5);

    EXPECT_EQ("hello", res);
}*/

TEST_F(PacketTest, TestStreamOneParameterFloat) {
    float value = 5.0f;
    o << Packet::packet << value;
    ASSERT_EQ(4, o.tellp());
    i.str(o.str());

    float f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

// 0101...
TEST_F(PacketTest, TestStreamOneParameterUInt32odd) {
    uint32_t value = 0x55555555u;
    o << Packet::packet << value;
    ASSERT_EQ(4, o.tellp());
    i.str(o.str());

    uint32_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

// 1010...
TEST_F(PacketTest, TestStreamOneParameterUInt32even) {
    uint32_t value = 0xAAAAAAAAu;
    o << Packet::packet << value;
    ASSERT_EQ(4, o.tellp());
    i.str(o.str());

    uint32_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

TEST_F(PacketTest, TestStreamOneParameterUInt64even) {
    uint64_t value = 0xAAAAAAAAAAAAAAAAul;
    o << Packet::packet << value;
    ASSERT_EQ(8, o.tellp());
    i.str(o.str());

    uint64_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

TEST_F(PacketTest, TestStreamOneParameterUInt64odd) {
    uint64_t value = 0x5555555555555555ul;
    o << Packet::packet << value;
    ASSERT_EQ(8, o.tellp());
    i.str(o.str());

    uint64_t f;
    i >> Packet::packet >> f;

    EXPECT_EQ(value, f);
}

TEST_F(PacketTest, TestStreamTwoParameters) {
    float value1 = -46.4930f;
    uint64_t value2 = 48385038;

    o << Packet::packet << value1 << value2;
    ASSERT_EQ(4+8, o.tellp());

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
    //auto s = r.read_bytes(16);
    //auto inp = istringstream(s);
    //inp >> Packet::packet >> t.v1 >> t.v2 >> t.v3;
    r >> t.v1 >> t.v2 >> t.v3;
    return r;
}

Packet::Writer operator<<(Packet::Writer w, test const t) {
    //auto outp = ostringstream();
    //outp << Packet::packet << t.v1 << t.v2 << t.v3;
    //w.write(outp.str());
    w << t.v1 << t.v2 << t.v3;

    return w;
}


TEST_F(PacketTest, TestStreamAggregate) {
    test values { 3.14159f, -3023943, 0xDEADBEEFDEADBEF };
    o << Packet::packet << values;
    ASSERT_EQ(16, o.tellp());
    i.str(o.str());

    test values2;
    i >> Packet::packet >> values2;

    ASSERT_EQ(values.v1, values2.v1);
    ASSERT_EQ(values.v2, values2.v2);
    ASSERT_EQ(values.v3, values2.v3);
}

TEST_F(PacketTest, TestStrings1) {
    auto w = Packet::Writer(o);

    std::string test_in = "this is a test";
    w.write(test_in);
    ASSERT_EQ(test_in.size() + 2, o.tellp());

    i.str(o.str());
    std::string test_out;
    auto r = Packet::Reader(i);
    r.read(test_out);

    ASSERT_EQ(test_in, test_out);
}

TEST_F(PacketTest, TestStrings2) {
    auto w = Packet::Writer(o);

    std::string test_in = "this is a test";
    w.write_fixed_string(test_in, test_in.size());
    ASSERT_EQ(test_in.size() + 2, o.tellp());

    i.str(o.str());
    std::string test_out;
    auto r = Packet::Reader(i);
    r.read(test_out);

    ASSERT_EQ(test_in, test_out);
}

TEST_F(PacketTest, TestStrings3) {
    auto w = Packet::Writer(o);

    std::string test_in = "this is a test";
    w.write_fixed_string(test_in, test_in.size()+2);

    ASSERT_EQ(test_in.size() + 4, o.tellp());

    i.str(o.str());
    std::string test_out;
    auto r = Packet::Reader(i);
    r.read(test_out);

    ASSERT_EQ(test_in, test_out);
}
