import { Address } from "viem";
import { mint } from "./actions.js"
import { transact } from "./transact.js";

// CLI
const to = process.argv[2] as Address;
const amount = BigInt(process.argv[3] ?? 0);

if ( !to || !amount ) {
    console.error("Usage: bun scripts/mint.ts <to> <amount>");
    process.exit(1);
}

// push transaction
const action = mint(to, amount);
await transact([action]);
