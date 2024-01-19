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

// checksum256 bridge::get_trx_id()
// {
//     size_t size = transaction_size();
//     char buf[size];
//     size_t read = read_transaction( buf, size );
//     check( size == read, "get_trx_id: read_transaction failed");
//     return sha256( buf, read );
// }

// checksum256 make_key(const string str)
// {
//     // Check if the input string has the "0x" prefix
//     size_t start_index = (str.substr(0, 2) == "0x") ? 2 : 0;

//     // Convert hex string to bytes
//     std::vector<uint8_t> bytes;
//     for (size_t i = start_index; i < str.size(); i += 2) {
//         std::string byteString = str.substr(i, 2);
//         uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, 0, 16));
//         bytes.push_back(byte);
//     }

//     // Call the existing make_key function for byte vector
//     return make_key(bytes.data(), bytes.size());
// }

// checksum256 make_key(const bytes data)
// {
//     return make_key((const uint8_t *)data.data(), data.size());
// }

// checksum256 make_key(const uint8_t *ptr, const size_t len)
// {
//     uint8_t buffer[32] = {};
//     check(len <= sizeof(buffer), "len provided to make_key is too small");
//     memcpy(buffer, ptr, len);
//     return checksum256(buffer);
// }

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
