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

checksum256 bridge::make_key(bytes data) {
    return make_key((const uint8_t *)data.data(), data.size());
}

checksum256 bridge::make_key(const uint8_t *ptr, size_t len) {
    uint8_t buffer[32] = {};
    check(len <= sizeof(buffer), "len provided to make_key is too small");
    memcpy(buffer, ptr, len);
    return checksum256(buffer);
}

