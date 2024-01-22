# `EORC-20` **Bridge** ![License](https://img.shields.io/github/license/eorc20/eorc20-bridge)

> `EORC-20` to `ERC-20` Inscriptions [EOS EVM](https://eosnetwork.com/eos-evm/) Bridge.

![image](https://github.com/eorc20/eorc20-bridge/assets/550895/2d5ff02d-a235-4cd7-9eae-4ec1b64dba54)

## Token Contracts

### EOS EVM

**ERC-20**

| Symbol | Name          | Decimals  | Address |
| ----- | -------------- | ----------| ------- |
|`eoss` | `EOSS eorc-20` | 0         | [0x000.... (TBD)](https://explorer.evm.eosnetwork.com/address/0x000)
|`rose` | `ROSE eorc-20` | 0         | [0x000.... (TBD)](https://explorer.evm.eosnetwork.com/address/0x000)

**ERC-1155**

| Name   | Description  | ID    | Address | URI      |
| ------ | -------------|------ | ------- | ---------|
|`eoss`  | EOSS eorc-20 | 0     | [0x000.... (TBD)](https://explorer.evm.eosnetwork.com/address/0x000) | [bafybeig4vyfgpbydni2kgt7yzevvqcy2vsmluk4um6w2kvbmbscxbqswwy](https://bafybeig4vyfgpbydni2kgt7yzevvqcy2vsmluk4um6w2kvbmbscxbqswwy.ipfs.nftstorage.link/)
|`rose`  | ROSE eorc-20 | 0     | [0x000.... (TBD)](https://explorer.evm.eosnetwork.com/address/0x000) | [bafybeig4vyfgpbydni2kgt7yzevvqcy2vsmluk4um6w2kvbmbscxbqswwy](https://bafybeig4vyfgpbydni2kgt7yzevvqcy2vsmluk4um6w2kvbmbscxbqswwy.ipfs.nftstorage.link/)

### EOS Native

**eosio.token**
| Symbol | Precision | Account |
| ------ | ----------| ------- |
| `EOSS` | 0         | [token.eorc](https://bloks.io/account/token.eorc)
| `ROSE` | 0         | [token.eorc](https://bloks.io/account/token.eorc)

**AtomicAssets**
| Name   | Collection Name | Template ID | Schema |
| -------|---------------- | ------------| -------|
| EOSS eorc-20 | [`eorc`](https://atomichub.io/explorer/collection/eos-mainnet/eorc)          | [1234]((https://atomichub.io/explorer/template/eos-mainnet/eorc/1234))        | [eorc2o](https://atomichub.io/explorer/schema/eos-mainnet/eorc/eorc2o)
| ROSE eorc-20 | [`eorc`](https://atomichub.io/explorer/collection/eos-mainnet/eorc)          | [1234]((https://atomichub.io/explorer/template/eos-mainnet/eorc/1234))        | [eorc2o](https://atomichub.io/explorer/schema/eos-mainnet/eorc/eorc2o)

## Requirements

- [Setup Metamask](https://docs.eosnetwork.com/evm/quick-start/setup-metamask) for EOS EVM
- [Setup Anchor](https://www.greymass.com/anchor) for EOS Native

## Reference
- [IPFS Car](https://car.ipfs.io/)
- [NFT Storage](https://nft.storage/)