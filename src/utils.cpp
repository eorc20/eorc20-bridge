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

checksum256 get_trx_id()
{
    size_t size = transaction_size();
    char buf[size];
    size_t read = read_transaction( buf, size );
    check( size == read, "get_trx_id: read_transaction failed");
    return sha256( buf, read );
}

int64_t to_number(const std::string& str) {
    if (str.empty()) return 0;

    char* end;
    errno = 0; // Reset errno before the call
    uint64_t num = std::strtoull(str.c_str(), &end, 10);

    // Check if conversion was successful
    check(*end == '\0' && errno != ERANGE, "invalid number format or overflow");

    // Check for underflow
    check(num <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()), "number underflow");

    return static_cast<int64_t>(num);
}

uint64_t bytesToUint64(const bytes& b) {
    check(b.size() == 8, "bytesToUint64: invalid bytes size");

    uint64_t result = 0;
    for (int i = 0; i < 8; ++i) {
        result = (result << 8) | b[i];
    }
    return result;
}

uint32_t current_evm_block_number() {
    const uint32_t current = current_time_point().sec_since_epoch();
    return current - EVM_LOCK_GENESIS_TIME + 1;
}