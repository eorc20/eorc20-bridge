#include <gems/atomic.gems.hpp>
#include "bridge.eorc.hpp"

#include "src/utils.cpp"

[[eosio::action]]
void bridge::onbridgemsg(const bridge_message_t message)
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    check(msg.receiver == get_self(), "invalid message receiver");

    const checksum256 trx_id = get_trx_id();
    const bytes data = {msg.data.begin(), msg.data.end()};
    const string from = bytesToHexString(bytes{data.begin(), data.begin() + 20});
    const string to = bytesToHexString(msg.sender);
    const string value = bytesToHexString(msg.value);
    const string calldata = {data.begin() + 20, data.end()};

    print("onbridgemsg:",
        "\nfrom: ", from,
        "\nto: ", to,
        "\nreceiver: ", msg.receiver,
        "\ntrx_id: ", trx_id,
        "\ncalldata: ", calldata,
        "\nvalue: ", value
    );
}
