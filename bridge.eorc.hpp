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

    struct bridge_message_v0 {
        eosio::name receiver;
        bytes sender;
        eosio::time_point timestamp;
        bytes value;
        bytes data;
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
    void handle_message(const bytes data);
};