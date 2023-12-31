#include <gems/atomic.gems.hpp>

#include "bridge.eorc.hpp"

[[eosio::on_notify("atomicassets::transfer")]]
void bridge::on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo )
{
    // ignore self transfers
    if ( from == get_self() ) return;

    check(false, "not implemented");
}

[[eosio::action]]
void bridge::onbridgemsg(const bridge_message_t message)
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);

    check(msg.receiver == get_self(), "invalid message receiver");
    checksum256 addr_key = make_key(msg.sender);
    // print(msg.receiver, addr_key);
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