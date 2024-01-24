import { Address } from "viem";
import { CONTRACT, walletClient } from "./config.js";
import { eosTestnet } from "viem/chains";

// CLI
// const to = process.argv[2] as Address;
const amount = BigInt(process.argv[2] ?? 0);

if ( !amount ) {
    console.error("Usage: bun scripts/mint.ts <to> <amount>");
    process.exit(1);
}

const hash = await walletClient.sendTransaction({
    account: walletClient.account,
    data: '0x646174613a2c7b2270223a22656f72632d3230222c226f70223a226d696e74222c227469636b223a22656f7373222c22616d74223a22313030227d',
    to: CONTRACT,
    value: 0n,
    chain: eosTestnet,
})

console.log(hash);
