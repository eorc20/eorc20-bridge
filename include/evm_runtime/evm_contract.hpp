#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

#include <string>

typedef std::vector<uint8_t> bytes;

class [[eosio::contract("eosio.evm")]] evm_runtime : public eosio::contract {
    public:
    using contract::contract;

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