#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>

using bytes_view = std::basic_string_view<uint8_t>;

uint64_t be64toh(uint64_t big_endian_value) {
    return ((big_endian_value & 0x00000000000000FFULL) << 56) |
           ((big_endian_value & 0x000000000000FF00ULL) << 40) |
           ((big_endian_value & 0x0000000000FF0000ULL) << 24) |
           ((big_endian_value & 0x00000000FF000000ULL) << 8) |
           ((big_endian_value & 0x000000FF00000000ULL) >> 8) |
           ((big_endian_value & 0x0000FF0000000000ULL) >> 24) |
           ((big_endian_value & 0x00FF000000000000ULL) >> 40) |
           ((big_endian_value & 0xFF00000000000000ULL) >> 56);
}

namespace silkworm {
    constexpr uint8_t reserved_address_prefix[] = {0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb};

    inline std::optional<uint64_t> extract_reserved_address(const bytes& addr) {
        if (!std::equal(std::begin(reserved_address_prefix), std::end(reserved_address_prefix), addr.begin()))
            return std::nullopt;

        uint64_t reserved;
        memcpy(&reserved, addr.data() + sizeof(reserved_address_prefix), sizeof(reserved));
        return be64toh(reserved);
    }

    inline bool is_reserved_address(const bytes& addr) {
        return extract_reserved_address(addr) != std::nullopt;
    }
}

std::string bytesToHexString(const std::vector<uint8_t>& bytes) {
    std::string hexString;
    hexString.reserve(bytes.size() * 2);

    for (const auto& byte : bytes) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02x", byte);
        hexString += buffer;
    }

    return hexString;
}

string to_address(const bytes addr) {
    return bytesToHexString(addr);
}
