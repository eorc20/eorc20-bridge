import { Hex } from "viem";

import BridgeEORC20 from "../artifacts/contracts/BridgeEORC20.sol/BridgeEORC20.json";

export const bytecode = BridgeEORC20.bytecode as Hex;
