#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>

using namespace eosio;
using namespace std;

class [[eosio::contract("bridge.eorc")]] bridge : public eosio::contract {
public:
    using contract::contract;

    [[eosio::on_notify("atomicassets::transfer")]]
    void on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo );

private:

};