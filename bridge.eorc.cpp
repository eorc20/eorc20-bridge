#include <gems/atomic.gems.hpp>

#include "bridge.eorc.hpp"

#include "src/getters.cpp"

[[eosio::on_notify("atomicassets::transfer")]]
void bridge::on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo )
{
    // ignore self transfers
    if ( from == get_self() ) return;

    check(false, "not implemented");
}

[[eosio::action]]
void bridge::onbridgemsg(const bridge_message_t message)
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    const bytes data = {msg.data.begin(), msg.data.end()};

    check(msg.receiver == get_self(), "invalid message receiver");
    const checksum256 from = make_key(bytes{data.begin(), data.begin() + 20});
    const checksum256 sender = make_key(msg.sender);
    const checksum256 value = make_key(msg.value);
    const checksum256 trx_id = get_trx_id();
    const string calldata = {data.begin() + 20, data.end()};
    // const std::string data(msg.data.begin(), msg.data.end());

    print("from: ", from,
        "\nreceiver: ", msg.receiver,
        "\nsender: ", sender,
        "\ntrx_id: ", trx_id,
        "\ncalldata: ", calldata,
        "\nvalue: ", value
    );
}
