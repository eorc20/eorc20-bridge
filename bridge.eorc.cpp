#include "bridge.eorc.hpp"

#include "src/utils.cpp"

[[eosio::action]]
void bridge::onbridgemsg(const bridge_message_t message)
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    check(msg.receiver == get_self(), "invalid message receiver");

    // const checksum256 trx_id = get_trx_id();
    // const bytes data = {msg.data.begin(), msg.data.end()};
    // const string from = bytesToHexString(bytes{data.begin(), data.begin() + 20});
    // const string to = bytesToHexString(msg.sender);
    // const string value = bytesToHexString(msg.value);
    // const string calldata = {data.begin() + 20, data.end()};
    // // const auto[ token0, token1 ] = utils::split( calldata, "data:," );
    // const string token1 = utils::split( calldata, "data:," )[1];

    // print("onbridgemsg:",
    //     "\nfrom: ", from,
    //     "\nto: ", to,
    //     "\nreceiver: ", msg.receiver,
    //     "\ntrx_id: ", trx_id,
    //     "\ncalldata: ", calldata,
    //     "\nvalue: ", value,
    //     "\ntoken1: ", token1
    // );

    // const json j = json::parse(token1);
    // const string p = j["p"];
    // const string op = j["op"];
    // const string tick = j["tick"];
    // const string amt = j["amt"];
    // print("\njson:",
    //     "\np: ", p,
    //     "\nop: ", op,
    //     "\ntick: ", tick,
    //     "\namt: ", amt
    // );
}
