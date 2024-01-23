[[eosio::action]]
void bridge::test(string data) {
    // print(to_number(data));
    // print(current_evm_block_number());
    // const bridge_message_data message_data = parse_bridge_message_data(data);
    // const bridge_message_calldata inscription_data = parse_bridge_message_calldata(message_data.calldata);
}

// @debug
template <typename T>
void bridge::clear_table( T& table, uint64_t rows_to_clear )
{
    auto itr = table.begin();
    while ( itr != table.end() && rows_to_clear-- ) {
        itr = table.erase( itr );
    }
}

// @debug
[[eosio::action]]
void bridge::cleartable( const name table_name, const optional<name> scope, const optional<uint64_t> max_rows )
{
    require_auth( get_self() );
    const uint64_t rows_to_clear = (!max_rows || *max_rows == 0) ? -1 : *max_rows;
    const uint64_t value = scope ? scope->value : get_self().value;

    // tables
    bridge::config_table _config( get_self(), value );
    bridge::tokens_table _tokens( get_self(), value );
    bridge::deploy_table _deploy( get_self(), value );

    if (table_name == "tokens"_n) clear_table( _tokens, rows_to_clear );
    else if (table_name == "deploy"_n) clear_table( _deploy, rows_to_clear );
    else if (table_name == "config"_n) _config.remove();
    else check(false, "cleartable: [table_name] unknown table to clear" );
}
