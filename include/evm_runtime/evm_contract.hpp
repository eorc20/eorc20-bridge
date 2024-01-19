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

    using call_action = eosio::action_wrapper<"call"_n, &evm_runtime::call>;
};