import { Address, encodeFunctionData, parseUnits } from "viem"
import { CONTRACT, session } from "./config.js";
import { abi } from "./abi.js";
import { AnyAction } from "@wharfkit/session";
import { call } from "./eos.evm.js";

export function mint(to: Address, value: bigint): AnyAction {
    const data = encodeFunctionData({abi, args: [ to, value ], functionName: "mint"});
    return call(session, CONTRACT, parseUnits("0.0", 18), data)
}