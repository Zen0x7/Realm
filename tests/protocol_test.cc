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

TEST(Protocol, SenderIsIdentifiable) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    const auto sender_serialized = message::serialize(sender_id);
    const auto sender_id_as_string = hello_message.get_sender();
    for (std::size_t i = 0; i < 16; i++) {
        EXPECT_EQ(sender_id_as_string[i], sender_serialized[i]);
    }
}

TEST(Protocol, MessageIsIdentifiable) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    const auto id_serialized = message::serialize(hello_message.id_);
    const auto id_as_string = hello_message.get_identifier();
    for (std::size_t i = 0; i < 16; i++) {
        EXPECT_EQ(id_as_string[i], id_serialized[i]);
    }
}

TEST(Protocol, MessageHasCRCIntegrityChecks) {
    const auto sender_id = boost::uuids::random_generator()();
    message hello_message = message::from_string("EHLO", sender_id);
    EXPECT_TRUE(protocol::has_integrity(hello_message));
    for (std::size_t i = 5; i < 9; i++) {
        hello_message.body()[i] = 0x00;
    }
    EXPECT_FALSE(protocol::has_integrity(hello_message));
}
