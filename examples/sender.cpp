#include <iostream>
#include <rawsocket.h>
#include <thread>

using namespace std;

char str[] = "Hello";

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cerr << "Interface name and destination MAC must be provided as arguments respectively"
             << endl;

        return -1;
    }

    RawSocket socket;

    auto ret = socket.open(argv[1]);

    if (ret.has_value()) {
        cout << ret.value() << endl;
        return -1;
    } else
        cout << "Socket opened" << endl;

    auto ds_mac = socket.macaddr_from_string(argv[2]);

    if (!ds_mac.has_value())
    {
        cerr << "destination MAC address is invalid" << endl;
        return -1;
    }

    while (true) {
        socket.transmit(ds_mac.value(), *str, sizeof str, 0x1122);
        cout << "Sending Hello to " << argv[2] << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
