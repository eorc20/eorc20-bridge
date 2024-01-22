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

constexpr size_t kAddressLength{20};
constexpr size_t kHashLength{32};
constexpr uint64_t evm_gaslimit = 500000;
constexpr uint64_t evm_init_gaslimit = 10000000;

class [[eosio::contract("bridge.eorc")]] bridge : public eosio::contract {

public:
    using contract::contract;

    /**
     * ## TABLE `tokens`
     *
     * ### params
     *
     * - `{symbol} sym` - (primary key) token symbol
     * - `{name} contract` - token contract
     * - `{string} name` - token name
     * - `{string} p` - inscription protocol
     * - `{name} tick` - (secondary key) token tick
     * - `{uint64_t} max` - max amount
     * - `{bytes} address` - token address
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "0,EOSS",
     *     "contract": "token.eorc",
     *     "name": "EOSS eorc-20",
     *     "p": "eorc20",
     *     "tick": "eoss",
     *     "max": 210000000000,
     *     "address": "59c2fffb3541a8d50ae75ae3c650f029509acdbe"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol              sym;
        name                contract;
        string              name;
        string              p;
        string              tick;
        uint64_t            max;
        bytes               address;

        uint64_t primary_key() const { return sym.code().raw(); }
        checksum256 by_tick() const { return to_checksum(tick); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row,
        indexed_by<"by.tick"_n, const_mem_fun<tokens_row, checksum256, &tokens_row::by_tick>>
    > tokens_table;

    static checksum256 to_checksum( string str )
    {
        size_t start_index = (str.substr(0, 2) == "0x") ? 2 : 0;
        str = str.substr(start_index);
        return sha256(str.c_str(), str.length());
    }

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
    struct [[eosio::table("configs")]] configs_row {
        bool            paused = false;
    };
    typedef eosio::singleton< "configs"_n, configs_row > configs_table;

    /**
     * ## ACTION `regtoken`
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{symbol_code} symcode` - token symbol code
     * - `{name} contract` - token contract
     * - `{string} tick` - token tick
     * - `{string} name` - token name
     * - `{uint64_t} max` - max amount
     * - `{bytes} address` - token address
     *
     * ### example
     *
     * ```bash
     * $ cleos push action bridge.eorc regtoken '["EOSS", "token.eorc, "eoss", "EOSS eorc-20", 210000000000, "59c2fffb3541a8d50ae75ae3c650f029509acdbe"]' -p bridge.eorc
     * ```
     */
    [[eosio::action]]
    void regtoken( const symbol_code symcode, const name contract, const string tick, const string name, const uint64_t max, const bytes address );

    [[eosio::action]]
    void deltoken( const symbol_code symcode );

    [[eosio::action]]
    void pause( const bool paused );

    struct bridge_message_calldata {
        string      p;
        string      op;
        string      tick;
        string      amt;
        string      max;
        string      lim;
    };

    struct bridge_message_data {
        bytes       sender;
        bytes       from;
        name        from_account;
        bytes       to;
        name        to_account;
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
    void test(const bytes data);

    [[eosio::on_notify("*::transfer")]]
    void on_transfer_token( const name from,
                            const name to,
                            const asset quantity,
                            const string memo );

private:
    bridge_message_data parse_bridge_message_data( const bytes data );
    bridge_message_calldata parse_bridge_message_calldata(const string calldata);
    tokens_row get_tick( const string tick );
    tokens_row get_token( const symbol_code symcode, const name contract );
    void handle_erc20_transfer( const tokens_row token, const asset quantity, const string memo );
    bytes parse_address( const string memo );
    bool is_token_exists( const name contract, const symbol_code symcode );
    name get_token_issuer( const name contract, const symbol_code symcode );
};