[[eosio::action]]
void bridge::inscribe( const uint64_t id, const string data )
{
    require_auth(get_self());
}

[[eosio::action]]
void bridge::transferins( const address from, const address to, const uint64_t id )
{
    require_auth(get_self());
}