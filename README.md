# `EORC-20` **Bridge**
![License](https://img.shields.io/github/license/pinax-network/eorc20-bridge)
[![Antelope CDT](https://github.com/pinax-network/eorc20-bridge/actions/workflows/release.yml/badge.svg)](https://github.com/pinax-network/eorc20-bridge/actions/workflows/release.yml)
[![Blanc++ Vert](https://github.com/pinax-network/eorc20-bridge/actions/workflows/ci.yml/badge.svg)](https://github.com/pinax-network/eorc20-bridge/actions/workflows/ci.yml)

> Send [EORC-20 NFT Inscriptions](https://eos.atomichub.io/market?collection_name=eorc&order=asc&schema_name=inscriptions&sort=price&state=1&symbol=EOS) to [EOS EVM](https://eosnetwork.com/eos-evm/).

## Quickstart

1. Send NFT to `bridge.eorc`

```bash
# Send NFT to contract using Bitcoin Bech32 address as memo
$ cleos push action atomicassets transfer '["myaccount", "brdige.eorc", [2199024546311], "0x856ed111f878a6CC5c0E7784E5988f2DD9388181"]' -p myaccount
```

## Requirements

- [Setup Metamask](https://docs.eosnetwork.com/evm/quick-start/setup-metamask) for EOS EVM
- [AtomicAssets](https://atomichub.io/) account