# Realm Data Protocol

## Generics

### Message

Realm protocol uses the following `message` struct:

```php
struct {
    char[16] sender_id;
    char[16] message_id;
    char[512] data;
    char[4] crc;
}
```

A serialized version can be seen as the following example:

```
[2 bytes: data_size]
[16 bytes: sender_id]
[16 bytes: message_id]
[{data_size} bytes: data]
[4 bytes: crc]
```

> Note: Serialization put all the content in a single line.

Basically we have 5 parts:

1. `data_size` serializes `uint16_t` encoded in 2 bytes.
2. `sender_id` serializes `boost::uuids::uuid` encoded in 16 bytes.
3. `message_id` serializes `boost::uuids::uuid` encoded in 16 bytes.
4. `data` encoded in `data_size` bytes.
5. `crc` serializes `boost::crc_32_type` encoded in 4 bytes.

In order to confirm the message, the counterpart should reply with an ack message, which uses the same shape but doesn't include data. Of course, `message_id` should be the same `message_id` of the original one.

```
[2 bytes: data_size = 0]
[16 bytes: sender_id]
[16 bytes: message_id]
[4 bytes: crc]
```

So basically, if we illustrate this situation we are going to see something like:

```
[worker] establishes a connection with [state] so
[state] sent [8 77777777 55555555 33333333 0000] to [worker] as WELCOME_MESSAGE
[worker] sent [0 33333333 55555555 0000] to [state] as ACK
```

> Note: Of course, this is an example and data parts sizes aren't exact.
