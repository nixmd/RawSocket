# Ethernet Layer II RawSocket library

This library is used to send and receive Ethernet frames which are defined in Layer II.

To open raw sockets, root permission is required. This library only works under Linux and BSD.

send and receive examples are provided. Can be used over direct ethernet connections, over network
switches or routers.

### How to compile

```
   git clone https://github.com/nixmd/RawSocket.git
   cd RawSocket
   mkdir build
   cd build
   cmake ..
   make
```

