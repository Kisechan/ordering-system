#ifndef NETWORKCONFIG_H
#define NETWORKCONFIG_H

namespace NetworkConfig {

    // Server configuration
    constexpr const char* SERVER_IP = "127.0.0.1"; // Replace with actual server IP
    constexpr int SERVER_PORT = 8080;              // Replace with actual server port

    // TCP connection timeout in milliseconds
    constexpr int TCP_TIMEOUT_MS = 5000;          // Adjust as needed

} // namespace NetworkConfig

#endif // NETWORKCONFIG_H