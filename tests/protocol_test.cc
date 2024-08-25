#include <gtest/gtest.h>
#include <message.h>
#include <protocol.h>
#include <boost/uuid/random_generator.hpp>

TEST(Protocol, ContainsData) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    EXPECT_EQ(hello_message.body()[0], 'E');
    EXPECT_EQ(hello_message.body()[1], 'H');
    EXPECT_EQ(hello_message.body()[2], 'L');
    EXPECT_EQ(hello_message.body()[3], 'O');
}

TEST(Protocol, HasCRCIntegrityChecks) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    EXPECT_TRUE(protocol::has_integrity(hello_message));
    hello_message.body()[5] = 0x00;
    hello_message.body()[6] = 0x00;
    hello_message.body()[7] = 0x00;
    hello_message.body()[8] = 0x00;
    EXPECT_FALSE(protocol::has_integrity(hello_message));
}