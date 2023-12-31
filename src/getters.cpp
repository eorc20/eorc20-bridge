#include <eosio/transaction.hpp>
#include <eosio/crypto.hpp>

checksum256 bridge::get_trx_id()
{
    size_t size = transaction_size();
    char buf[size];
    size_t read = read_transaction( buf, size );
    check( size == read, "pomelo::get_trx_id: read_transaction failed");
    return sha256( buf, read );
}

checksum256 bridge::make_key(const string str)
{
    // Check if the input string has the "0x" prefix
    size_t start_index = (str.substr(0, 2) == "0x") ? 2 : 0;

    // Convert hex string to bytes
    std::vector<uint8_t> bytes;
    for (size_t i = start_index; i < str.size(); i += 2) {
        std::string byteString = str.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, 0, 16));
        bytes.push_back(byte);
    }

    // Call the existing make_key function for byte vector
    return make_key(bytes.data(), bytes.size());
}

checksum256 bridge::make_key(const bytes data)
{
    return make_key((const uint8_t *)data.data(), data.size());
}

checksum256 bridge::make_key(const uint8_t *ptr, const size_t len)
{
    uint8_t buffer[32] = {};
    check(len <= sizeof(buffer), "len provided to make_key is too small");
    memcpy(buffer, ptr, len);
    return checksum256(buffer);
}
