#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <nlohmann/json.hpp>
#include <utils/utils.hpp>
#include <eosio.token/eosio.token.hpp>

using json = nlohmann::json;

using namespace eosio;
using namespace std;

typedef std::vector<uint8_t> bytes;

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
     * - `{name} tick` - (secondary key) token tick
     * - `{string} name` - token name
     * - `{uint64_t} max` - max amount
     * - `{bytes} address` - token address
     *
     * ### example
     *
     * ```json
     * {
     *     "symcode": "0,EOSS",
     *     "contract": "token.eorc",
     *     "tick": "eoss",
     *     "name": "EOSS eorc-20",
     *     "max": 210000000000,
     *     "lim": 10000,
     *     "address": "78a68C9200f720267918d22A102E03241A4fE946"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol              sym;
        name                contract;
        string              tick;
        string              name;
        uint64_t            max;
        bytes               address;

        uint64_t primary_key() const { return sym.code().raw(); }
        checksum256 by_tick() const { return to_checksum(tick); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row,
        indexed_by<"by.tick"_n, const_mem_fun<tokens_row, checksum256, &tokens_row::by_tick>>
    > tokens_table;

    static checksum256 to_checksum( string address )
    {
        if ( address.length() > 40 ) address = address.substr(2);
        return sha256(address.c_str(), address.length());
    }

    /**
     * ## TABLE `configs`
     *
     * ### params
     *
     * - `{string} contract` - Solidity contract name
     * - `{checksum256} hash` - Solidity compiled bytecode hash
     * - `{bytes} bytecode` - Solidity compiled bytecode for contract
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "BridgeEORC",
     *     "hash": "77bc64e6bfe1907f1...",
     *     "bytecode": "600680546001600160..."
     * }
     * ```
     */
    struct [[eosio::table("configs")]] configs_row {
        string          contract;
        checksum256     hash;
        bytes           bytecode;
    };
    typedef eosio::singleton< "configs"_n, configs_row > configs_table;

    /**
     * ## ACTION `setconfig`
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{string} contract` - Solidity contract name
     * - `{bytes} bytecode` - Solidity compiled bytecode for contract
     *
     * ### example
     *
     * ```bash
     * $ cleos push action bridge.eorc setconfig '["BridgeEORC", "600680546001600160..."]' -p bridge.eorc
     * ```
     */
    [[eosio::action]]
    void setconfig( const optional<string> contract, const optional<bytes> bytecode );

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
     * $ cleos push action bridge.eorc regtoken '["EOSS", "token.eorc, "eoss", "BridgeEORC", 210000000000, "59C2ffFB3541A8d50AE75AE3C650F029509aCDBE"]' -p bridge.eorc
     * ```
     */
    [[eosio::action]]
    void regtoken( const symbol_code symcode, const name contract, const string tick, const string name, const uint64_t max, const bytes address );

    [[eosio::action]]
    void deltoken( const symbol_code symcode );

    struct bridge_message_calldata {
        string      p;
        string      op;
        string      tick;
        uint64_t    amt;
    };

    struct bridge_message_data {
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

private:
    // checksum256 make_key(const string str);
    // checksum256 make_key(const bytes data);
    // checksum256 make_key(const uint8_t *ptr, const size_t len);

    // checksum256 get_trx_id();
    bridge_message_data parse_bridge_message_data( const bytes data );
    bridge_message_calldata parse_bridge_message_calldata(const string calldata);
};