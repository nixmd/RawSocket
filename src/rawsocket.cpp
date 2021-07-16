#include "rawsocket.h"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <cerrno>

RawSocket::RawSocket()
: m_opened(false)
{
}

RawSocket::~RawSocket() { close(m_socket); }

std::optional<std::string> RawSocket::open(std::string interface)
{
    /* Opening RAW socket */
    if ((m_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
        return std::string("Could not create socket. ") + strerror(errno);

    struct sockaddr_ll sockaddr;

    /* Binding Network Device to the opened socket */
    memset(&sockaddr, 0x0, sizeof(sockaddr));
    sockaddr.sll_family   = AF_PACKET;
    sockaddr.sll_protocol = htons(ETH_P_ALL);
    sockaddr.sll_ifindex  = if_nametoindex(interface.c_str());

    if (bind(m_socket, (struct sockaddr*) &sockaddr, sizeof(sockaddr)) < 0)
        return std::string("Could not bind interface to socket. ") + strerror(errno);

    m_opened = true;

    /* Storing name of Network Device */
    m_interface = interface;

    /* If return is empty, it means everything was alright */
    return std::nullopt;
}

void RawSocket::close_socket()
{
    close(m_socket);
    m_opened = false;
}

std::optional<std::string> RawSocket::transmit(
    mac_address destination_mac, char& payload, unsigned payload_size, unsigned short protocol)
{
    return transmit(
        destination_mac, macaddr_from_ifname(m_interface).value(), payload, payload_size, protocol);
}

std::optional<std::string> RawSocket::transmit(
    mac_address    destination_mac,
    mac_address    source_mac,
    char&          payload,
    unsigned       payload_size,
    unsigned short protocol)
{
    if (!m_opened)
        return "Socket not open.";

    struct eth_frame packet;

    /* Setting the possible payload size */
    payload_size = MAX_PAYLOAD_SIZE > payload_size ? payload_size : MAX_PAYLOAD_SIZE;

    /* Calculating packet overall size */
    unsigned packet_size = ETH_FRAME_HDR_SIZE + payload_size;

    packet.h_proto = protocol;
    memcpy(&packet.h_dest, &destination_mac, sizeof packet.h_dest);
    memcpy(&packet.h_source, &source_mac, sizeof packet.h_source);
    memcpy(packet.payload, &payload, payload_size);

    /* Returns empty optional only if determined size of bytes sent */
    if (send(m_socket, &packet, packet_size, 0) == packet_size)
        return std::nullopt;

    return strerror(errno);
}

std::tuple<int, mac_address, mac_address, unsigned short, std::string> RawSocket::receive(
    char& payload_buffer, unsigned payload_buffer_size)
{
    if (!m_opened)
        return std::make_tuple(-1, 0, 0, 0, "Socket is not open");

    ssize_t        recv_size;
    ssize_t        payload_size = -1;
    unsigned short proto;

    /* Wait for incoming data */
    recv_size = recv(m_socket, &m_recv_buffer, sizeof m_recv_buffer, 0);

    /* Determine valid size to prevent invalid address access for the next operation */
    recv_size = recv_size > payload_buffer_size ? payload_buffer_size : recv_size;

    /* Calculate incoming payload_size */
    payload_size = recv_size - ETH_FRAME_HDR_SIZE;
    proto        = m_recv_buffer.h_proto;

    /* Copy incoming payload into the payload_buffer only if the payload_size is higher than 0 */
    if (payload_size > 0)
        memcpy(&payload_buffer, &m_recv_buffer.payload, payload_size);

    /* Return the results as a tuple */
    return std::make_tuple(
        payload_size,
        *(mac_address*) m_recv_buffer.h_source,
        *(mac_address*) m_recv_buffer.h_dest,
        proto,
        payload_size < 0 ? strerror(errno) : "");
}

std::optional<mac_address> RawSocket::macaddr_from_string(std::string str)
{
    std::string lower_str;
    mac_address mac;

    /* Check for string length */
    if (str.length() < 17)
        return std::nullopt;

    for (char& c : str)
        lower_str.push_back(std::tolower(c));

    /* Check for characters validity */
    for (unsigned i = 0; i < 17; i++)
    {
        char c = lower_str.c_str()[i];

        if (!(c == ':' || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return std::nullopt;
    }

    sscanf(
        lower_str.c_str(),
        "%02x:%02x:%02x:%02x:%02x:%02x",
        (int*) &reinterpret_cast<char*>(&mac)[0],
        (int*) &reinterpret_cast<char*>(&mac)[1],
        (int*) &reinterpret_cast<char*>(&mac)[2],
        (int*) &reinterpret_cast<char*>(&mac)[3],
        (int*) &reinterpret_cast<char*>(&mac)[4],
        (int*) &reinterpret_cast<char*>(&mac)[5]);

    return mac;
}

std::optional<mac_address> RawSocket::macaddr_from_ifname(std::string interface)
{
    int          fd;
    struct ifreq ifr;

    /* Don't go further if it could not open socket for defined interface name */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0) < 0))
        return std::nullopt;

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    close(fd);

    return *reinterpret_cast<mac_address*>(ifr.ifr_hwaddr.sa_data) & 0xFFFFFFFFFFFFUL;
}

std::string RawSocket::macaddr_to_string(mac_address mac)
{
    char           buf[18];
    unsigned char* ptr = reinterpret_cast<unsigned char*>(&mac);

    for (unsigned i = 0; i < 5; i++)
        sprintf(&buf[i * 3], "%02x:", ptr[i]);

    sprintf(&buf[15], "%02x", ptr[5]);

    return std::string(buf, 18);
}
