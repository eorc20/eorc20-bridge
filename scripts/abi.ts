import { parseAbi, encodeDeployData } from "viem";
import { Address, Hex, encodeFunctionData, parseUnits } from "viem"
import { CONTRACT, session } from "./config.js";
import { AnyAction } from "@wharfkit/session";
import { call } from "./eos.evm.js";

export const wagmiAbi = [
    {
        inputs: [
            { name: "_name", type: "string" },
            { name: "_tick", type: "string" },
            { name: "_max", type: "uint64" },
            { name: "_lim", type: "uint64" },
        ],
        stateMutability: "nonpayable",
        type: "constructor",
    },
] as const;

export const abi = parseAbi([
    'function transfer(address to, uint256 amount) returns (bool)',
    'function balanceOf(address account) returns (uint256)',
    'function mint(address to, uint256 value) public',
    'function burn(uint256 value) public',
    'function setFee(uint256 _egressFee) public',
])

export function mint(to: Address, value: bigint): AnyAction {
    const data = encodeFunctionData({abi, args: [ to, value ], functionName: "mint"});
    return call(session, CONTRACT, parseUnits("0.0", 18), data)
}

export function transfer(to: Address, value: bigint): AnyAction {
    const data = encodeFunctionData({abi, args: [ to, value ], functionName: "transfer"});
    return call(session, CONTRACT, parseUnits("0.0", 18), data)
}

export function deploy(name: string, tick: string, max: bigint, lim: bigint, bytecode: Hex): AnyAction {
    const data = encodeDeployData({abi: wagmiAbi, args: [ name, tick, max, lim ], bytecode});
    return call(session, null, parseUnits("0.0", 18), data)
}
