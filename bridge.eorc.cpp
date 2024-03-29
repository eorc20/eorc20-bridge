#include "bridge.eorc.hpp"
#include "src/utils.cpp"

// logging (used for backend syncing)
#include "src/logs.cpp"

// DEBUG (used to help testing)
#ifdef DEBUG
#include "src/debug.cpp"
#endif

[[eosio::on_notify("*::transfer")]]
void bridge::on_transfer_token( const name from,
                              const name to,
                              const asset quantity,
                              const string memo )
{
    // ignore outgoing transfer from self
    if ( from == get_self() || to != get_self() || from == "eosio.ram"_n ) return;

    const tokens_row token = get_token_by_contract(quantity.symbol.code(), get_first_receiver());
    const address to_address = parse_address(memo);
    const address contract = get_deploy(token.tick).address;
    check(!silkworm::is_reserved_address(to_address), "invalid EVM address, cannot be reserved address");
    handle_erc20_call(METHOD_TRANSFER, contract, to_address, quantity);
}

[[eosio::action]]
void bridge::onbridgemsg( const bridge_message_t message )
{
    const bridge_message_v0 &msg = std::get<bridge_message_v0>(message);
    check(msg.receiver == get_self(), "invalid message receiver");
    const bridge_message_data message_data = parse_bridge_message_data(msg.data);
    const bridge_message_calldata inscription_data = parse_bridge_message_calldata(message_data.calldata);

    print(
        "\nonbridgemsg",
        "\n-----------",
        "\nmessage.sender:", bytesToHexString(msg.sender),
        "\nmessage.receiver:", msg.receiver
    );

    // handle operations
    const name op = inscription_data.op;
    if ( op == "transfer"_n) handle_transfer_op(msg.sender, message_data, inscription_data);
    else if ( op == "deploy"_n) handle_deploy_op(msg.sender, message_data, inscription_data);
    else if ( op == "mint"_n) handle_mint_op(msg.sender, message_data, inscription_data);
    else check(false, "invalid inscription operation");

    // log inscription
    bridge::inscribe_action inscribe(get_self(), {get_self(), "active"_n});
    bridge::transferins_action transferins(get_self(), {get_self(), "active"_n});
    inscribe.send(message_data.id, message_data.calldata);
    transferins.send(message_data.from, message_data.to, message_data.id);
}

[[eosio::action]]
void bridge::regtoken( const name tick, const symbol_code symcode, const name contract )
{
    require_auth(get_self());

    // input validation
    check(is_account(contract), "contract account does not exist");

    // token validation
    auto deploy = get_deploy(tick);
    const asset maximum_supply = asset{deploy.max, symbol{symcode, 0}};
    const eosio::name issuer = get_self();

    // check if max supply matches
    if ( is_token_exists(contract, symcode) ) {
        const asset supply = eosio::token::get_supply(contract, symcode);
        check(supply.symbol == maximum_supply.symbol, "invalid token symbol");
        check(supply.amount == maximum_supply.amount, "invalid token max supply");
        check(get_token_issuer(contract, symcode) == issuer, "invalid token issuer");

    // create new token
    } else {
        eosio::token::create_action create(contract, {contract, "active"_n});
        create.send(issuer, maximum_supply);
    }

    // insert token to table
    tokens_table tokens(get_self(), get_self().value);
    auto token = tokens.find(symcode.raw());
    check(token == tokens.end(), "token already exists");

    tokens.emplace(get_self(), [&](auto& row) {
        row.tick = tick;
        row.maximum_supply = maximum_supply;
        row.contract = contract;
        row.issuer = issuer;
    });
}

bool bridge::is_token_exists( const name contract, const symbol_code symcode )
{
    eosio::token::stats stats(contract, symcode.raw());
    auto itr = stats.find(symcode.raw());
    return itr != stats.end();
}

name bridge::get_token_issuer( const name contract, const symbol_code symcode )
{
    eosio::token::stats stats(contract, symcode.raw());
    return stats.get(symcode.raw(), "token does not exist").issuer;
}

[[eosio::action]]
void bridge::deltoken( const name tick )
{
    require_auth(get_self());
    tokens_table tokens(get_self(), get_self().value);
    auto token = tokens.find(tick.value);
    check(token != tokens.end(), "token does not exist");
    tokens.erase(token);
}

[[eosio::action]]
void bridge::deldeploy( const name tick )
{
    require_auth(get_self());
    deploy_table _deploy(get_self(), get_self().value);
    auto deploy = _deploy.find(tick.value);
    check(deploy != _deploy.end(), "deploy does not exist");
    _deploy.erase(deploy);
}

[[eosio::action]]
void bridge::pause( const bool paused )
{
    require_auth(get_self());
    config_table _config(get_self(), get_self().value);
    auto config = _config.get_or_default();
    check(config.paused != paused, "paused is unchanged");
    config.paused = paused;
    _config.set(config, get_self());
}

void bridge::check_deploy_inscription( const bytes address, const bridge_message_calldata inscription_data )
{
    // validate transfer
    const name tick = inscription_data.tick;
    const int64_t amount = inscription_data.amt;
    check(amount > 0, "inscription amount must be greater than 0");
    const deploy_row deploy = get_deploy(tick);
    check(deploy.address == address, "invalid deploy address");
}

void bridge::handle_transfer_op( const bytes address, const bridge_message_data message_data, const bridge_message_calldata inscription_data )
{
    // validate transfer
    const name tick = inscription_data.tick;
    const int64_t amount = inscription_data.amt;
    check(amount > 0, "inscription amount must be greater than 0");
    check_deploy_inscription(address, inscription_data);

    const bytes contract = get_deploy(tick).address;
    const name to = message_data.to_account;

    // ignore bridge recipient
    if ( message_data.to == contract ) return;
    if ( to == get_self() ) return;

    // only handle EVM=>Native reserved address transfers
    if ( to ) {
        print("\ntransfer to reserved address: ", to);
        const tokens_row token = get_token(tick);
        const symbol sym = token.maximum_supply.symbol;
        const string memo = bytesToHexString(message_data.from); // use from address as memo

        eosio::token::transfer_action transfer(token.contract, {get_self(), "active"_n});
        transfer.send(get_self(), to, asset{amount, sym}, memo);
    }
}

void bridge::handle_mint_op( const bytes address, const bridge_message_data message_data, const bridge_message_calldata inscription_data )
{
    // validate transfer
    const name tick = inscription_data.tick;
    const int64_t amount = inscription_data.amt;
    check(amount > 0, "inscription amount must be greater than 0");
    check_deploy_inscription(address, inscription_data);

    // only listen if bridge is recipient
    const bytes contract = get_deploy(tick).address;
    if ( message_data.to != contract ) return;

    // mint tokens on Native to bridge
    const tokens_row token = get_token(tick);
    const symbol sym = token.maximum_supply.symbol;
    const bytes from = message_data.from;
    const string memo = bytesToHexString(from); // use from address as memo
    const asset quantity = asset{amount, sym};
    eosio::token::issue_action issue(token.contract, {get_self(), "active"_n});
    issue.send(get_self(), quantity, memo);

    // mint on EVM
    const deploy_row deploy = get_deploy(tick);
    handle_erc20_call(METHOD_MINT, deploy.address, from, quantity);

    // // add minted newly tokens to address
    // mints_table mints(get_self(), tick.value);
    // auto index = mints.get_index<"by.address"_n>();
    // auto itr = index.find(evm_runtime::make_key(from));
    // if ( itr == index.end() ) {
    //     mints.emplace(get_self(), [&](auto& row) {
    //         row.id = mints.available_primary_key();
    //         row.address = from;
    //         row.balance = amount;
    //     });
    // } else {
    //     index.modify(itr, get_self(), [&](auto& row) {
    //         row.balance += amount;
    //     });
    // }
}

void bridge::handle_deploy_op( const bytes address, const bridge_message_data message_data, const bridge_message_calldata inscription_data )
{
    // only handle deploy with [to=null] address
    if ( message_data.to != bytes(kAddressLength, 0) ) return;

    const name tick = inscription_data.tick;
    const string p = inscription_data.p;
    const int64_t max = inscription_data.max;
    const int64_t lim = inscription_data.lim;

    // validate deploy
    check(max > 0, "inscription max must be greater than 0");

    // insert deploy to table
    deploy_table deploy(get_self(), get_self().value);
    auto _deploy = deploy.find(tick.value);
    check(_deploy == deploy.end(), tick.to_string() + " tick is already deployed");

    const checksum256 trx_id = get_trx_id();
    deploy.emplace(get_self(), [&](auto& row) {
        row.tick = tick;
        row.p = p;
        row.max = max;
        row.lim = lim;
        row.address = address;
        row.trx_id = trx_id;
        row.block_num = current_block_number();
        row.timestamp = current_time_point();
    });
}

bridge::bridge_message_data bridge::parse_bridge_message_data( const bytes data )
{
    // extract bridge message bytes data
    const bytes msg_sender = {data.begin(), data.begin() + 20};
    const bytes from = {data.begin() + 20, data.begin() + 40};
    const bytes to = {data.begin() + 40, data.begin() + 60};
    const uint64_t id = bytesToUint64({data.begin() + 60, data.begin() + 68});
    const string calldata = {data.begin() + 68, data.end()};

    // parse reserved addresses
    const name from_account = name{*silkworm::extract_reserved_address(from)};
    const name to_account = name{*silkworm::extract_reserved_address(to)};

    print(
        "\nparse_bridge_message_data",
        "\n-------------------------",
        "\ndata: ", bytesToHexString(data),
        "\nmsg_sender: ", bytesToHexString(msg_sender),
        "\nfrom: ", bytesToHexString(from),
        "\nfrom_account: ", from_account,
        "\nto: ", bytesToHexString(to),
        "\nto_account: ", to_account,
        "\nid: ", id,
        "\ncalldata: ", calldata
    );
    return {msg_sender, from, from_account, to, to_account, id, calldata};
}

bridge::bridge_message_calldata bridge::parse_bridge_message_calldata(const string calldata)
{
    // parse inscription
    const string inscription = utils::split( calldata, "data:," )[0];
    check(inscription.size() > 0, "inscription is empty");

    const json j = json::parse(inscription);
    const string p = j["p"];
    const name op = utils::parse_name(j["op"]);
    const name tick = utils::parse_name(j["tick"]);

    // validate inscription
    check(tick.value, "invalid inscription tick");
    check(p == "eorc-20", "invalid inscription protocol");
    check(op == "mint"_n || op == "transfer"_n || op == "deploy"_n, "invalid inscription operation");

    // parse inscription data
    int64_t amt = 0;
    int64_t max = 0;
    int64_t lim = 0;
    if ( op == "mint"_n || op == "transfer"_n ) {
        check(j.contains("amt"), "invalid inscription amount");
        amt = to_number(string{j["amt"]});
    }
    if ( op == "deploy"_n ) {
        check(j.contains("max"), "invalid inscription max");
        check(j.contains("lim"), "invalid inscription lim");
        max = to_number(string{j["max"]});
        lim = to_number(string{j["lim"]});
    }

    print(
        "\nparse_bridge_message_calldata",
        "\n------------------------------",
        "\ncalldata: ", calldata,
        "\ninscription: ", inscription,
        "\np: ", p,
        "\nop: ", op,
        "\ntick: ", tick,
        "\namt: ", amt,
        "\nmax: ", max,
        "\nlim: ", lim
    );
    return {p, op, tick, amt, max, lim};
}

bridge::deploy_row bridge::get_deploy( const string tick )
{
    return get_deploy(utils::parse_name(tick));
}

bridge::deploy_row bridge::get_deploy( const name tick )
{
    deploy_table deploy(get_self(), get_self().value);
    return deploy.get(tick.value, "EORC-20 is not deployed");
}

bridge::tokens_row bridge::get_token_by_contract( const symbol_code symcode, const name contract )
{
    tokens_table tokens(get_self(), get_self().value);
    auto index = tokens.get_index<"by.supply"_n>();
    const auto token = index.get(symcode.raw(), "EORC-20 token not registerred");
    check(token.contract == contract, "invalid token contract");
    return token;
}

bridge::tokens_row bridge::get_token( const name tick )
{
    tokens_table tokens(get_self(), get_self().value);
    return tokens.get(tick.value, "EORC-20 token not registerred");
}

void bridge::handle_erc20_call( const char method[4], const bytes contract, const address to, const asset quantity )
{
    // convert quantity to uint256
    intx::uint256 value((uint64_t)quantity.amount);
    uint8_t value_buffer[32] = {};
    intx::be::store(value_buffer, value);

    // create call data
    bytes call_data;
    call_data.reserve(4 + 64);
    call_data.insert(call_data.end(), method, method + 4);
    call_data.insert(call_data.end(), 32 - kAddressLength, 0);  // padding for address
    call_data.insert(call_data.end(), to.begin(), to.end());
    call_data.insert(call_data.end(), value_buffer, value_buffer + 32);

    // send 0 EOS value
    bytes value_zero;
    value_zero.resize(32, 0);

    // send call action
    evm_runtime::call_action call_act("eosio.evm"_n, {{get_self(), "active"_n}});
    call_act.send(get_self(), contract, value_zero, call_data, EVM_INIT_GAS_LIMIT);
}

bytes bridge::parse_address( const string memo )
{
    auto address_bytes = evmc::from_hex(memo);
    eosio::check(!!address_bytes, "memo must be valid 0x EVM address");
    eosio::check(address_bytes->size() == kAddressLength, "memo must be valid 0x EVM address");
    return bytes{address_bytes->begin(), address_bytes->end()};
}
