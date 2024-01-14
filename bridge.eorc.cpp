#include "bridge.eorc.hpp"

#include "src/utils.cpp"

[[eosio::action]]
void bridge::onbridgemsg(const bridge_message_t message)
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    check(msg.receiver == get_self(), "invalid message receiver");

    const bytes data = {msg.data.begin(), msg.data.end()};
    handle_message(data);
}

[[eosio::action]]
void bridge::test(const bytes data) {
    handle_message(data);
}

void bridge::handle_message(const bytes data)
{
    const checksum256 trx_id = get_trx_id();
    const string from = bytesToHexString(bytes{data.begin(), data.begin() + 20});
    const string to = {data.begin() + 20, data.begin() + 40};
    const string calldata = {data.begin() + 40, data.end()};
    const string token1 = utils::split( calldata, "data:," )[0];
    check(token1.size() > 0, "token1 is empty");

    print("onbridgemsg:",
        "\ntrx_id: ", trx_id,
        "\ncalldata: ", calldata,
        "\ntoken1: ", token1
    );

    const json j = json::parse(token1);
    const string p = j["p"];
    const string op = j["op"];
    const string tick = j["tick"];
    const string amt = j["amt"];
    print("\njson:",
        "\np: ", p,
        "\nop: ", op,
        "\ntick: ", tick,
        "\namt: ", amt
    );
}
