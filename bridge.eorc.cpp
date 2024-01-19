#include "bridge.eorc.hpp"
#include "src/utils.cpp"

[[eosio::on_notify("*::transfer")]]
void bridge::on_transfer_token( const name from,
                              const name to,
                              const asset quantity,
                              const string memo )
{
    // ignore outgoing transfer from self
    if ( from == get_self() || to != get_self() || from == "eosio.ram"_n ) return;

    const tokens_row token = get_token(quantity.symbol.code(), get_first_receiver());
    handle_erc20_transfer(token, quantity, memo);
}

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
void bridge::pause( const bool paused )
{
    require_auth(get_self());
    configs_table configs(get_self(), get_self().value);
    auto config = configs.get_or_default();
    check(config.paused != paused, "paused is unchanged");
    config.paused = paused;
    configs.set(config, get_self());
}

[[eosio::action]]
void bridge::onbridgemsg( const bridge_message_t message )
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    check(msg.receiver == get_self(), "invalid message receiver");
    const bridge_message_data message_data = parse_bridge_message_data(msg.data);
    const bridge_message_calldata inscription_data = parse_bridge_message_calldata(message_data.calldata);

    // validate inscription
    const tokens_row token = get_tick(inscription_data.tick);
    check(token.address == msg.sender, "invalid registered sender");

    // only handle EVM=>Native reserved address transfers
    if ( message_data.to_account ) {
        print("\ntransfer to reserved address: ", message_data.to_account);
        eosio::token::transfer_action transfer(token.contract, {get_self(), "active"_n});
        const int64_t amount = inscription_data.amt;
        check(amount == inscription_data.amt, "amount overflow");
        transfer.send(get_self(), message_data.to_account, asset{amount, token.sym}, bytesToHexString(message_data.from));
    }
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

    // validate inscription
    check(p == "eorc20", "invalid inscription protocol");
    check(op == "transfer", "only transfer operation is supported");
    check(amt > 0, "inscription amount must be positive");

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

bridge::tokens_row bridge::get_tick( const string tick )
{
    tokens_table tokens(get_self(), get_self().value);
    auto index = tokens.get_index<"by.tick"_n>();
    return index.get(to_checksum(tick), "EORC-20 token not registerred");
}

bridge::tokens_row bridge::get_token( const symbol_code symcode, const name contract )
{
    tokens_table tokens(get_self(), get_self().value);
    const auto token = tokens.get(symcode.raw(), "EORC-20 token not registerred");
    check(token.contract == contract, "invalid token contract");
    return token;
}

void bridge::handle_erc20_transfer( const tokens_row token, const asset quantity, const string memo )
{
    // const char method[4] = {'\xa9', '\x05', '\x9c', '\xbb'};  // sha3(transfer(address,uint256))[:4]
    const char method[4] = {'\x40', '\xc1', '\x0f', '\x19'};  // sha3(mint(address,uint256))[:4]

    intx::uint256 value((uint64_t)quantity.amount);

    uint8_t value_buffer[32] = {};
    intx::be::store(value_buffer, value);

    const bytes address_bytes = parse_address( memo );
    check( isRe )

    bytes call_data;
    call_data.reserve(4 + 64);
    call_data.insert(call_data.end(), method, method + 4);
    call_data.insert(call_data.end(), 32 - kAddressLength, 0);  // padding for address
    call_data.insert(call_data.end(), address_bytes.begin(), address_bytes.end());
    call_data.insert(call_data.end(), value_buffer, value_buffer + 32);

    bytes value_zero; // value of EVM native token (aka EOS)
    value_zero.resize(32, 0);

    evm_runtime::call_action call_act("eosio.evm"_n, {{get_self(), "active"_n}});
    call_act.send(get_self(), token.address, value_zero, call_data, evm_init_gaslimit);
}

bytes bridge::parse_address( const string memo )
{
    auto address_bytes = evmc::from_hex(memo);
    eosio::check(!!address_bytes, "memo must be valid 0x EVM address");
    eosio::check(address_bytes->size() == kAddressLength, "memo must be valid 0x EVM address");
    return bytes{address_bytes->begin(), address_bytes->end()};
}