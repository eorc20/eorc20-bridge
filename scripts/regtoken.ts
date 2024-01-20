import { Asset, Bytes, Name, UInt64 } from "@wharfkit/session";
import { regtoken } from "./abi.js"
import { transact } from "./transact.js";

// CLI
const symcode = Asset.SymbolCode.from(process.argv[2]);
const contract = Name.from(process.argv[3]);
const tick = process.argv[4];
const name = process.argv[5];
const max = UInt64.from(process.argv[6]);
const address = Bytes.fromString(process.argv[7]);

if ( process.argv.length != 8 ) {
    console.error("Usage: bun scripts/regtoken.ts <symcode> <token_contract> <tick> <name> <max> <address>");
    process.exit(1);
}

// push transaction
await transact([regtoken(symcode, contract, tick, name, max, address)]);
