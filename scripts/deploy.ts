import { deploy } from "./abi.js"
import { bytecode } from "./bytecode.js";
import { transact } from "./transact.js";

// push transaction
const name = "EOSS eorc-20"
const symbol = "eoss"
const tick = symbol;
const max = 210000000000n
const lim = 10000n

await transact([deploy(name, symbol, tick, max, lim, bytecode)]);
