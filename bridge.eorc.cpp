#include "bridge.eorc.hpp"
#include "src/utils.cpp"

[[eosio::action]]
void bridge::regtoken( const symbol_code symcode, const name contract, const string tick, const string name, const uint64_t max, const bytes address )
{
    require_auth(get_self());

    // input validation
    check(is_account(contract), "contract account does not exist");
    check(tick.size() > 0, "tick is empty");
    check(name.size() > 0, "name is empty");
    check(max > 0, "max must be greater than 0");
    check(address.size() > 0, "address is empty");
    check(address.size() == 20, "address must be 20 bytes");

    // token validation
    const asset supply = eosio::token::get_supply(contract, symcode);

    // insert token to table
    tokens_table tokens(get_self(), get_self().value);
    auto token = tokens.find(symcode.raw());
    check(token == tokens.end(), "token already exists");

    tokens.emplace(get_self(), [&](auto& row) {
        row.sym = supply.symbol;
        row.contract = contract;
        row.tick = tick;
        row.name = name;
        row.max = max;
        row.address = address;
    });
}

[[eosio::action]]
void bridge::deltoken( const symbol_code symcode )
{
    require_auth(get_self());
    tokens_table tokens(get_self(), get_self().value);
    auto token = tokens.find(symcode.raw());
    check(token != tokens.end(), "token does not exist");
    tokens.erase(token);
}

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
    const bridge_message_data message_data = parse_bridge_message_data(msg.data);
    const bridge_message_calldata inscription_data = parse_bridge_message_calldata(message_data.calldata);
}

[[eosio::action]]
void bridge::test(bytes data) {
    const bridge_message_data message_data = parse_bridge_message_data(data);
    const bridge_message_calldata inscription_data = parse_bridge_message_calldata(message_data.calldata);
}

bridge::bridge_message_data bridge::parse_bridge_message_data( const bytes data )
{
    // extract bridge message bytes data
    const bytes from = {data.begin(), data.begin() + 20};
    const bytes to = {data.begin() + 20, data.begin() + 40};
    const string calldata = {data.begin() + 40, data.end()};

    // parse reserved addresses
    const name from_account = name{*silkworm::extract_reserved_address(from)};
    const name to_account = name{*silkworm::extract_reserved_address(to)};

    print(
        "\nfrom: ", bytesToHexString(from),
        "\nfrom_account: ", from_account,
        "\nto: ", bytesToHexString(to),
        "\nto_account: ", to_account,
        "\ncalldata: ", calldata
    );
    return {from, from_account, to, to_account, calldata};
}

bridge::bridge_message_calldata bridge::parse_bridge_message_calldata(const string calldata)
{
    // parse inscription
    const string inscription = utils::split( calldata, "data:," )[0];
    check(inscription.size() > 0, "inscription is empty");

    const json j = json::parse(inscription);
    const string p = j["p"];
    const string op = j["op"];
    const string tick = j["tick"];
    const uint64_t amt = std::stoul(string{j["amt"]});

    print(
        "\ncalldata: ", calldata,
        "\ninscription: ", inscription,
        "\np: ", p,
        "\nop: ", op,
        "\ntick: ", tick,
        "\namt: ", amt
    );
    return {p, op, tick, amt};
}
