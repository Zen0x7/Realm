# Realm Data Protocol

## Generics

### Wrapper

There are a wrapper structure which should be respected in order to make transactions in a bidirectional way:

One message will be similar like:

```
  38,f3d2a257-44f2-4a75-b689-e15d1687e419,9a16fbe1-f1b8-464c-9d2d-c4ae515768e9,{"event":"user_action:form_completed"},01010101
```

Basically there are 5 identifiable parts:

1. `38` is the number of bytes after the third comma.
2. `f3d2a257-44f2-4a75-b689-e15d1687e419` is the `sender identifier` which can represents both, server or worker.
3. `9a16fbe1-f1b8-464c-9d2d-c4ae515768e9` is the `message identifier` which is used for ACK purposes.
4. `{"event":"user_action:form_completed"}` is a custom data example sent via the protocol.
5. `01010101` is the CRC-32 checksum of the content between the 1st comma and the 4th comma.

By the way, the previous isn't a real example as is a `human-readable` message. 

In order to have a real representation we need to consider the following concerns:

- The first element MUST BE a 2 bytes number.
- The second element and third element are Universal Unique Identifier version 4 serialized in 16 bytes.
- The fourth element MUST BE a valid JSON expression.
- The last element MUST BE a binary representation of 4 bytes.
- And commas MUST BE not included in the message body as SHOULD BE considered as logic separator.

## From Server to Workers

### 
