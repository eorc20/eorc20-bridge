import { deploy } from "./abi.js"
import { bytecode } from "./bytecode.js";
import { transact } from "./transact.js";

// push transaction
const name = "EOSS eorc-20"
const tick = "eoss"
const max = 210000000000n

await transact([deploy(name, tick, max, bytecode)]);
