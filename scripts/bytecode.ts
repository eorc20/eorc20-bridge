import fs from "fs";
import { Hex } from "viem";

import BridgeEORC20 from "../artifacts/contracts/BridgeEORC20.sol/BridgeEORC20.json";

export const bytecode = BridgeEORC20.bytecode as Hex;

fs.writeFileSync("bytecode.txt", bytecode.replace("0x", "").toLocaleLowerCase());

const hasher = new Bun.CryptoHasher("sha256");
hasher.update(bytecode.replace("0x", "").toLocaleLowerCase());
const hash = hasher.digest("hex");
console.log({hash})
