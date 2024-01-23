// eosio
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio.token/eosio.token.hpp>

// evm
#include <evm_runtime/evm_contract.hpp>
#include <nlohmann/json.hpp>
#include <intx/intx.hpp>
#include <evmc/hex.hpp>

// utils
#include <utils/utils.hpp>

using json = nlohmann::json;

using namespace eosio;
using namespace std;

typedef std::vector<uint8_t> bytes;
typedef bytes address;

constexpr size_t kAddressLength{20};
constexpr size_t kHashLength{32};
constexpr uint64_t EVM_GASLIMIT = 500000;
constexpr uint64_t EVM_INIT_GAS_LIMIT = 10000000;
// constexpr uint32_t EVM_LOCK_GENESIS_TIME = 1680661089; // "2023-04-05T02:18:09Z"

class [[eosio::contract("bridge.eorc")]] bridge : public eosio::contract {

public:
    using contract::contract;

    /**
     * ## TABLE `tokens`
     *
     * ### params
     *
     * - `{name} tick` - (primary key) inscription token ticker
     * - `{bytes} address` - EVM token address
     * - `{asset} maximum_supply` - (secondary key) EOS token maximum supply
     * - `{name} contract` - EOS token contract
     * - `{name} issuer` - EOS token issuer
     *
     * ### example
     *
     * ```json
     * {
     *     "tick": "eoss",
     *     "maximum_supply": "210000000000 EOSS",
     *     "contract": "token.eorc",
     *     "issuer": "bridge.eorc"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        name                tick;
        asset               maximum_supply;
        name                contract;
        name                issuer;

        uint64_t primary_key() const { return tick.value; }
        uint64_t by_supply() const { return maximum_supply.symbol.code().raw(); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row,
        indexed_by<"by.supply"_n, const_mem_fun<tokens_row, uint64_t, &tokens_row::by_supply>>
    > tokens_table;

    /**
     * ## TABLE `deploy`
     *
     * ### params
     *
     * - `{name} tick` - (primary key) inscription token ticker
     * - `{string} p` - inscription protocol
     * - `{int64_t} max` - max amount
     * - `{int64_t} lim` - limit
     * - `{bytes} address` - token address
     * - `{checksum256} trx_id` - transaction id
     * - `{uint32_t} block_num` - block number
     * - `{uint32_t} evm_block_num` - evm block number
     * - `{time_point_sec} timestamp` - timestamp
     *
     * ### example
     *
     * ```json
     * {
     *     "tick": "eoss",
     *     "p": "eorc-20",
     *     "max": 210000000000,
     *     "lim": 10000,
     *     "address": "59c2fffb3541a8d50ae75ae3c650f029509acdbe",
     *     "trx_id": "84ebd4daf667feb3faf5abb2685c0543781d35848b98bbb3b05bbb8d9e875a69",
     *     "block_num": 12345678,
     *     "timestamp": "2024-01-22T00:00:00"
     * }
     * ```
     */
    struct [[eosio::table("deploy")]] deploy_row {
        name                tick;
        string              p;
        int64_t             max;
        int64_t             lim;
        bytes               address;
        checksum256         trx_id;
        uint32_t            block_num;
        time_point_sec      timestamp;

        uint64_t primary_key() const { return tick.value; }
        checksum256 by_address() const { return evm_runtime::make_key(address); }
    };
    typedef eosio::multi_index< "deploy"_n, deploy_row,
        indexed_by<"by.address"_n, const_mem_fun<deploy_row, checksum256, &deploy_row::by_address>>
    > deploy_table;

    /**
     * ## TABLE `mints`
     *
     * > Record the minting of EVM tokens.
     *
     * - scope: `{name} tick`
     *
     * ### params
     *
     * - `{uint64_t} id` - (primary key) incremental ID
     * - `{bytes} address` - address
     * - `{int64_t} balance` - token balance
     *
     * ### example
     *
     * ```json
     * {
     *     "id": 0,
     *     "address": "3495F427Fcfe765d9C1174efe5C14014D2020Ed1",
     *     "balance": 30000
     * }
     * ```
     */
    struct [[eosio::table("mints")]] mints_row {
        uint64_t            id;
        bytes               address;
        int64_t             balance;

        uint64_t primary_key() const { return id; }
        checksum256 by_address() const { return evm_runtime::make_key(address); }
    };
    typedef eosio::multi_index< "mints"_n, mints_row,
        indexed_by<"by.address"_n, const_mem_fun<mints_row, checksum256, &mints_row::by_address>>
    > mints_table;

    /**
     * ## TABLE `configs`
     *
     * ### params
     *
     * - `{bool} paused` - disables all actions if true
     *
     * ### example
     *
     * ```json
     * {
     *     "paused": false,
     * }
     * ```
     */
    struct [[eosio::table("config")]] config_row {
        bool            paused = false;
    };
    typedef eosio::singleton< "config"_n, config_row > config_table;

    /**
     * ## ACTION `regtoken`
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} tick` - token tick
     * - `{symbol_code} symcode` - token symbol code
     * - `{name} contract` - token contract
     *
     * ### example
     *
     * ```bash
     * $ cleos push action bridge.eorc regtoken '["eoss", "EOSS", "token.eorc"]' -p bridge.eorc
     * ```
     */
    [[eosio::action]]
    void regtoken( const name tick, const symbol_code symcode, const name contract );

    [[eosio::action]]
    void deltoken( const name tick );

    [[eosio::action]]
    void deldeploy( const name tick );

    [[eosio::action]]
    void pause( const bool paused );

    struct bridge_message_calldata {
        string      p;
        name        op;
        name        tick;
        int64_t     amt;
        int64_t     max;
        int64_t     lim;
    };

    struct bridge_message_data {
        bytes       msg_sender;
        bytes       from;
        name        from_account;
        bytes       to;
        name        to_account;
        uint64_t    id;
        string      calldata;
    };

    struct bridge_message_v0 {
        name        receiver;
        bytes       sender;
        time_point  timestamp;
        bytes       value;
        bytes       data;
    };
    using bridge_message_t = std::variant<bridge_message_v0>;

    [[eosio::action]]
    void onbridgemsg(const bridge_message_t message);

    [[eosio::action]]
    void inscribe( const uint64_t id, const string data );
    using inscribe_action = eosio::action_wrapper<"inscribe"_n, &bridge::inscribe>;

    void transferins( const address from, const address to, const uint64_t id );
    using transferins_action = eosio::action_wrapper<"transferins"_n, &bridge::transferins>;

    [[eosio::on_notify("*::transfer")]]
    void on_transfer_token( const name from,
                            const name to,
                            const asset quantity,
                            const string memo );

    // DEBUG (used to help testing)
    #ifdef DEBUG
    [[eosio::action]]
    void test(const string data);

    // @debug
    [[eosio::action]]
    void cleartable( const name table_name, const optional<name> scope, const optional<uint64_t> max_rows );
    #endif

private:
    bridge_message_data parse_bridge_message_data( const bytes data );
    bridge_message_calldata parse_bridge_message_calldata(const string calldata);
    deploy_row get_deploy( const string tick );
    deploy_row get_deploy( const name tick );
    tokens_row get_token_by_contract( const symbol_code symcode, const name contract );
    tokens_row get_token( const name tick );
    void handle_erc20_transfer( const tokens_row token, const asset quantity, const string memo );
    void handle_transfer_op( const bytes address, const bridge_message_data message_data, const bridge_message_calldata inscription_data );
    void handle_deploy_op( const bytes address, const bridge_message_data message_data, const bridge_message_calldata inscription_data );
    void handle_mint_op( const bytes address, const bridge_message_data message_data, const bridge_message_calldata inscription_data );

    void check_deploy_inscription( const bytes address, const bridge_message_calldata inscription_data );

    bytes parse_address( const string memo );
    bool is_token_exists( const name contract, const symbol_code symcode );
    name get_token_issuer( const name contract, const symbol_code symcode );

    // DEBUG (used to help testing)
    #ifdef DEBUG
    template <typename T>
    void clear_table( T& table, uint64_t rows_to_clear );
    #endif
};