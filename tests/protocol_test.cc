#include <gtest/gtest.h>
#include <message.h>
#include <protocol.h>

TEST(Protocol, HasCRCIntegrityChecks) {
    message hello_message = message::from_string("EHLO");
    EXPECT_TRUE(protocol::has_integrity(hello_message));

    hello_message.body()[5] = 0x00;
    hello_message.body()[6] = 0x00;
    hello_message.body()[7] = 0x00;
    hello_message.body()[8] = 0x00;
    EXPECT_FALSE(protocol::has_integrity(hello_message));
}