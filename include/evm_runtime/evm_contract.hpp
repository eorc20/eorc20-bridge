#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

#include <string>
using namespace eosio;

typedef std::vector<uint8_t> bytes;

class [[eosio::contract("eosio.evm")]] evm_runtime : public eosio::contract {
    public:
    using contract::contract;

    // TO-DO: fix this, can't query config
    struct [[eosio::table("config")]] config_row {
        uint32_t            version; // placeholder for future variant index
        uint64_t            chainid = 0;
        time_point_sec      genesis_time;
        asset               ingress_bridge_fee;
        uint64_t            gas_price = 0;
        uint32_t            miner_cut = 0;
        uint32_t            status = 0; // <- bit mask values from status_flags
    };
    typedef eosio::singleton< "config"_n, config_row > config_table;

    [[eosio::action]]
    void call(eosio::name from, const bytes& to, const bytes& value, const bytes& data, uint64_t gas_limit);

    static eosio::checksum256 make_key(const uint8_t* ptr, size_t len) {
        uint8_t buffer[32]={0};
        eosio::check(len <= sizeof(buffer), "invalida size");
        memcpy(buffer, ptr, len);
        return eosio::checksum256(buffer);
    }

    static eosio::checksum256 make_key(bytes data){
        return make_key((const uint8_t*)data.data(), data.size());
    }

    using call_action = eosio::action_wrapper<"call"_n, &evm_runtime::call>;
};