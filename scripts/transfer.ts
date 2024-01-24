import { Address } from "viem";
import { CONTRACT, walletClient } from "./config.js";
import { eosTestnet } from "viem/chains";
import { transferData } from "./abi.js";

// CLI
const to = process.argv[2] as Address;
const amount = BigInt(process.argv[3] ?? 0);

if ( !to || !amount ) {
    console.error("Usage: bun scripts/transfer.ts <to> <amount>");
    process.exit(1);
}

const hash = await walletClient.sendTransaction({
    account: walletClient.account,
    data: transferData(to, amount),
    to: CONTRACT,
    value: 0n,
    chain: eosTestnet,
})

console.log({contract: CONTRACT, from: walletClient.account.address, to, amount, hash});
