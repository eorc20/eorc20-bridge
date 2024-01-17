#include "bridge.eorc.hpp"
#include "src/utils.cpp"

[[eosio::action]]
void bridge::setconfig( const optional<string> contract, const optional<bytes> bytecode )
{
    require_auth(get_self());
    configs_table configs(get_self(), get_self().value);
    auto config = configs.get_or_default();
    if ( contract ) config.contract = *contract;
    if ( bytecode ) {
        config.bytecode = *bytecode;
        const auto hash = eosio::sha256( (const char*) &config.bytecode, sizeof( config.bytecode ));
        config.hash = hash;
    }
    check(config.contract.size() > 0, "contract name is empty");
    check(config.bytecode.size() > 0, "bytecode is empty");
    configs.set(config, get_self());
}

[[eosio::action]]
void bridge::onbridgemsg(const bridge_message_t message)
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    check(msg.receiver == get_self(), "invalid message receiver");
    parse_inscription_data(msg.data);
}

[[eosio::action]]
void bridge::test(bytes data) {
    parse_inscription_data(data);
}

bridge::inscription_data bridge::parse_inscription_data(const bytes data)
{
    // extract bridge message bytes data
    const bytes from = {data.begin(), data.begin() + 20};
    const bytes to = {data.begin() + 20, data.begin() + 40};
    const string calldata = {data.begin() + 40, data.end()};

    // parse reserved addresses
    const name from_account = name{*silkworm::extract_reserved_address(from)};
    const name to_account = name{*silkworm::extract_reserved_address(to)};

    // parse inscription
    const string inscription = utils::split( calldata, "data:," )[0];
    check(inscription.size() > 0, "inscription is empty");

    const json j = json::parse(inscription);
    const string p = j["p"];
    const string op = j["op"];
    const string tick = j["tick"];
    const uint64_t amt = std::stoull(string{j["amt"]});

    print(
        "\nfrom: ", bytesToHexString(from),
        "\nfrom_account: ", from_account,
        "\nto: ", bytesToHexString(to),
        "\nto_account: ", to_account,
        "\ncalldata: ", calldata,
        "\ninscription: ", inscription,
        "\np: ", p,
        "\nop: ", op,
        "\ntick: ", tick,
        "\namt: ", amt
    );
    return {from, from_account, to, to_account, p, op, tick, amt};
}
