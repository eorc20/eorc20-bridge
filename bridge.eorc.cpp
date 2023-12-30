#include <gems/atomic.gems.hpp>

#include "bridge.eorc.hpp"

[[eosio::on_notify("atomicassets::transfer")]]
void bridge::on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo )
{
    // ignore self transfers
    if ( from == get_self() ) return;

    check(false, "not implemented");
}
