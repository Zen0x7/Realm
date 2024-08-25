#include <gtest/gtest.h>
#include <message.h>
#include <protocol.h>
#include <boost/uuid/random_generator.hpp>

TEST(Protocol, ContainsData) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    auto body = hello_message.body();
    EXPECT_EQ(body[0], 'E');
    EXPECT_EQ(body[1], 'H');
    EXPECT_EQ(body[2], 'L');
    EXPECT_EQ(body[3], 'O');
}

TEST(Protocol, ContainsSender) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    auto sender_serialized = message::serialize(sender_id);
    auto sender_id_as_string = hello_message.get_sender();
    EXPECT_EQ(sender_id_as_string[0], sender_serialized[0]);
    EXPECT_EQ(sender_id_as_string[1], sender_serialized[1]);
    EXPECT_EQ(sender_id_as_string[2], sender_serialized[2]);
    EXPECT_EQ(sender_id_as_string[3], sender_serialized[3]);
    EXPECT_EQ(sender_id_as_string[4], sender_serialized[4]);
    EXPECT_EQ(sender_id_as_string[5], sender_serialized[5]);
    EXPECT_EQ(sender_id_as_string[6], sender_serialized[6]);
    EXPECT_EQ(sender_id_as_string[7], sender_serialized[7]);
    EXPECT_EQ(sender_id_as_string[8], sender_serialized[8]);
    EXPECT_EQ(sender_id_as_string[9], sender_serialized[9]);
    EXPECT_EQ(sender_id_as_string[10], sender_serialized[10]);
    EXPECT_EQ(sender_id_as_string[11], sender_serialized[11]);
    EXPECT_EQ(sender_id_as_string[12], sender_serialized[12]);
    EXPECT_EQ(sender_id_as_string[13], sender_serialized[13]);
    EXPECT_EQ(sender_id_as_string[14], sender_serialized[14]);
    EXPECT_EQ(sender_id_as_string[15], sender_serialized[15]);
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
