import { parseAbi, encodeDeployData } from "viem";
import { Address, Hex, encodeFunctionData, parseUnits } from "viem"
import { CONTRACT, session } from "./config.js";
import { AnyAction, Asset, Bytes, Name, UInt64 } from "@wharfkit/session";
import { call } from "./eos.evm.js";

export const wagmiAbi = [
    {
        inputs: [
            { name: "name_", type: "string" },
            { name: "symbol_", type: "string" },
            { name: "tick_", type: "string" },
            { name: "max_", type: "uint64" },
            { name: "lim_", type: "uint64" },
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
    'function deploy(uint64 _max, uint64 _lim) public',
])

export function mint(to: Address, value: bigint): AnyAction {
    const data = encodeFunctionData({abi, args: [ to, value ], functionName: "mint"});
    return call(session, CONTRACT, parseUnits("0.0", 18), data)
}

export function transferData(to: Address, value: bigint): Hex {
    return encodeFunctionData({abi, args: [ to, value ], functionName: "transfer"});
}

export function transfer(to: Address, value: bigint): AnyAction {
    return call(session, CONTRACT, parseUnits("0.0", 18), transferData(to, value))
}

export function deploy(name: string, symbol: string, tick: string, max: bigint, lim: bigint, bytecode: Hex): AnyAction {
    const data = encodeDeployData({abi: wagmiAbi, args: [ name, symbol, tick, max, lim ], bytecode});
    return call(session, null, parseUnits("0.0", 18), data)
}

export function regtoken(symcode: Asset.SymbolCode, contract: Name, tick: string, name: string, max: UInt64, address: Bytes): AnyAction {
    return {
        account: "bridge.eorc",
        name: "regtoken",
        authorization: [session.permissionLevel],
        data: {
            symcode,
            contract,
            tick,
            name,
            max,
            address
        },
    }
}
