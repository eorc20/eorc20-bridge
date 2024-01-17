#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <nlohmann/json.hpp>
#include <utils/utils.hpp>
// #include <endian.h>
// #include <silkworm/core/common/utils.hpp>
// #include <evm_runtime/utils.cpp>

// to_address

using json = nlohmann::json;

using namespace eosio;
using namespace std;

typedef std::vector<uint8_t> bytes;

class [[eosio::contract("bridge.eorc")]] bridge : public eosio::contract {

public:
    using contract::contract;

    struct inscription_data {
        bytes       from;
        bytes       to;
        string      p;
        string      op;
        string      tick;
        string      amt;
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