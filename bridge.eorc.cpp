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
    const string from = bytesToHexString(bytes{data.begin(), data.begin() + 20});
    const string to = bytesToHexString(bytes{data.begin() + 20, data.begin() + 40});

    // std::optional<uint64_t> dest_acc = silkworm::is_reserved_address(bytes{data.begin() + 20, data.begin() + 40});
    std::optional<uint64_t> to_reserved = silkworm::extract_reserved_address(bytes{data.begin() + 20, data.begin() + 40});
    eosio::name to_account(*to_reserved);

    const string calldata = {data.begin() + 40, data.end()};
    const string inscription = utils::split( calldata, "data:," )[0];
    check(inscription.size() > 0, "inscription is empty");

    const json j = json::parse(inscription);
    const string p = j["p"];
    const string op = j["op"];
    const string tick = j["tick"];
    const string amt = j["amt"];

    print(
        "\nfrom: ", from,
        "\nto: ", to,
        "\nto_account: ", to_account,
        "\np: ", p,
        "\nop: ", op,
        "\ntick: ", tick,
        "\namt: ", amt
    );
    return {from, to, p, op, tick, amt};
}
