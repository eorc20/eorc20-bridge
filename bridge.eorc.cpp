#include "bridge.eorc.hpp"
#include "src/utils.cpp"

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
    const uint64_t id = {data.begin() + 40, data.begin() + 48};
    const string calldata = {data.begin() + 48, data.end()};

    // parse reserved addresses
    const name from_account = *silkworm::extract_reserved_address(from);
    const name to_account = *silkworm::extract_reserved_address(to);

    // parse inscription
    const string inscription = {calldata.begin() + 6, calldata.end()}; // start after "data:,"
    check(inscription.size() > 0, "inscription is empty");

    const json j = json::parse(inscription);
    const string p = j["p"];
    const string op = j["op"];
    const string tick = j["tick"];
    const string amt = j["amt"];

    print(
        "\nfrom: ", from,
        "\nfrom_account: ", from_account,
        "\nto: ", to,
        "\nto_account: ", to_account,
        "\np: ", p,
        "\nop: ", op,
        "\ntick: ", tick,
        "\namt: ", amt
    );
    return {from, to, p, op, tick, amt};
}
