#include <iostream>
#include <rawsocket.h>
#include <thread>

using namespace std;

char buffer[1500];

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Interface name must be passed as argument" << endl;
        return -1;
    }
    
    RawSocket socket;
    auto ret = socket.open(argv[1]);

    if (ret.has_value()) {
        cout << ret.value() << endl;
        return -1;
    } else
        cout << "Socket opened" << endl;

    while (true) {
        auto [size, src_mac, dst_mac, proto, str] = socket.receive(*buffer, sizeof buffer);

        if (size < 0)
            cout << str;
        else if (proto == 0x1122) {
            cout << "Received [" << buffer << "] from " << RawSocket::macaddr_to_string(src_mac)
                 << " send to " << RawSocket::macaddr_to_string(dst_mac) << endl;
        }
    }

    return 0;
}
