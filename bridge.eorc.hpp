#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <nlohmann/json.hpp>
#include <utils/utils.hpp>

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
     * - `{name} tick` - token tick
     *
     * ### example
     *
     * ```json
     * {
     *     "sym": "0,EOSS",
     *     "contract": "token.eorc",
     *     "tick": "eoss"
     * }
     * ```
     */
    struct [[eosio::table("tokens")]] tokens_row {
        symbol              sym;
        name                contract;
        name                tick;

        uint64_t primary_key() const { return sym.code().raw(); }
    };
    typedef eosio::multi_index< "tokens"_n, tokens_row> tokens_table;

    /**
     * ## TABLE `tickers`
     *
     * ### params
     *
     * - `{name} tick` - (primary key) token tick
     * - `{symbol} symcode` - token symbol code
     * - `{string} name` - token name
     * - `{uint64_t} max` - max amount
     * - `{uint64_t} lim` - limit amount
     * - `{bytes} address` - token address
     *
     * ### example
     *
     * ```json
     * {
     *     "tick": "eoss",
     *     "symcode": "EOSS"
     *     "name": "EOSS eorc-20",
     *     "max": 210000000000,
     *     "lim": 10000,
     *     "address": "0x78a68C9200f720267918d22A102E03241A4fE946"
     * }
     * ```
     */
    struct [[eosio::table("tickers")]] tickers_row {
        name                tick;
        symbol_code         symcode;
        string              name;
        uint64_t            max;
        uint64_t            lim;
        bytes               address;

        uint64_t primary_key() const { return tick.value; }
    };
    typedef eosio::multi_index< "tickers"_n, tickers_row> tickers_table;

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

    struct inscription_data {
        bytes       from;
        name        from_account;
        bytes       to;
        name        to_account;
        string      p;
        string      op;
        string      tick;
        uint64_t    amt;
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
    checksum256 make_key(const string str);
    checksum256 make_key(const bytes data);
    checksum256 make_key(const uint8_t *ptr, const size_t len);
    checksum256 get_trx_id();
    inscription_data parse_inscription_data(const bytes data);
};