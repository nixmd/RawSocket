#ifndef RAWSOCKET_H
#define RAWSOCKET_H

#define ETH_FRAME_HDR_SIZE 14
#define MAX_PAYLOAD_SIZE 1500
#define MAX_MTU_SIZE (MAX_PAYLOAD_SIZE + ETH_FRAME_HDR_SIZE)

#include <cstdint>
#include <string>
#include <optional>
#include <tuple>

using mac_address = uint64_t;

class RawSocket
{
public:
    RawSocket();

    ~RawSocket();

    /**
     * @brief open Opens a raw socket at interface name. Opening raw socket requires root
     * permissions
     * @param interface Interface name in strings
     * @return Returns an optional object. If the operation had any errors, it will report the error
     *  string as the optional object value, otherwise, it will be empty.
     */
    std::optional<std::string> open(std::string interface);

    /**
     * @brief close_socket Close raw socket
     */
    void close_socket();

    /**
     * @brief transmit Transmit an Ethernet frame over raw socket. Payload size can't be larger than
     * 1500 bytes in case Maximum MTR has been set for the interface.
     * @param destination_mac Destination MAC address as 64bit unsigned int
     * @param payload Payload packet
     * @param payload_size Payload size in bytes, can't be bigger than 1500 bytes
     * @param protocol Ethernet frame protocol number
     * @return Returns true if frame was sent successfully, otherwise false is returned
     */
    std::optional<std::string> transmit(
        mac_address destination_mac, char& payload, unsigned payload_size, unsigned short protocol);

    /**
     * @brief transmit Transmit an Ethernet frame over raw socket with custom set source MAC
     * address. Payload size can't be larger than 1500 bytes in case Maximum MTR has been set for
     * the interface.
     * @param destination_mac Destination MAC address as 64bit unsigned int
     * @param source_mac Custom set source MAC address as 64bit unsigned int
     * @param payload Payload packet
     * @param payload_size Payload size in bytes, can't be bigger than 1500 bytes
     * @param protocol Ethernet frame protocol number
     * @return Returns true if frame was sent successfully, otherwise false is returned
     */
    std::optional<std::string> transmit(
        mac_address    destination_mac,
        mac_address    source_mac,
        char&          payload,
        unsigned       payload_size,
        unsigned short protocol);

    /**
     * @brief receive Blocks thread to receive an Ethernet frame.
     * @param payload_buffer Buffer to store incoming Ethernet frame payload.
     * @param payload_buffer_size payload buffer size. Sizes bigger than 1500 bytes is waist of
     * memory.
     * @return Returns a tuple of values. First parameter determines size of incoming packet. If it
     * was a negative value, an error has occurred then other parameters except the last one are
     * invalid. The last parameter reports the error string.
     * If the first parameter was positive, it means the size of incoming payload. And therefore
     * second parameter will be source MAC address, third parameter will be destination MAC address,
     * and fourth parameter will be Ethernet frame protocol.
     */
    std::tuple<int, mac_address, mac_address, unsigned short, std::string> receive(
        char& payload_buffer, unsigned payload_buffer_size);

    /**
     * @brief macaddr_from_string Calculates mac_address value from MAC address represented as
     * string
     * @param str MAC address representing string
     * @return mac_address as 64bit unsigned int or nothing if the format of input string was
     * invalid.
     */
    static std::optional<mac_address> macaddr_from_string(std::string str);

    /**
     * @brief macaddr_to_string Format MAC Address to string
     * @param mac mac_address as 64bit unsigned int
     * @return MAC address as string
     */
    static std::string macaddr_to_string(mac_address mac);

private:
    struct eth_frame
    {
        unsigned char h_dest[6];
        unsigned char h_source[6];
        uint16_t      h_proto;
        char          payload[MAX_PAYLOAD_SIZE];
    };

    /**
     * @brief macaddr_from_ifname Get MAC address of target interface name
     * @param interface Interface name as string
     * @return Returns mac_address or nothing if interface not found
     */
    std::optional<mac_address> macaddr_from_ifname(std::string interface);

    int         m_socket;
    eth_frame   m_recv_buffer;
    std::string m_interface;
    bool        m_opened;
};

#endif  // RAWSOCKET_H
