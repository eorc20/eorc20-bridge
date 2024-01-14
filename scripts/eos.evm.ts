import { AnyAction, Session } from "@wharfkit/session";
import { Address, PrivateKeyAccount, bytesToHex, keccak256, numberToBytes } from "viem";
import { chain, gas, session, gasPrice } from "./config.js";

export function toTransactionId(rlptx: Address) {
    const value = Buffer.from(rlptx.replace(/^0x/,""), "hex");
    return keccak256(value);
}

export function pushtx(serializedTransaction: Address): AnyAction {
    console.info("pushtx", {serializedTransaction});
    return {
        account: "eosio.evm",
        name: "pushtx",
        authorization: [session.permissionLevel],
        data: {
            miner: session.actor,
            rlptx: serializedTransaction.replace(/^0x/,""),
        }
    }
}

export function call(session: Session, to: Address, value: bigint, data?: Address): AnyAction {
    // console.info("call", {from: session.actor.toString(), to, value})
    const valueHex = bytesToHex(numberToBytes(value, {size: 32}), {size: 32}).replace(/^0x/,"");
    return {
        account: "eosio.evm",
        name: "call",
        authorization: [session.permissionLevel],
        data: {
            from: session.actor,
            to: to.replace(/^0x/,""),
            value: valueHex,
            data: data?.replace(/^0x/,"") ?? "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            gas_limit: gas,
        }
    }
}

export async function sign(nonce: number, account: PrivateKeyAccount, to: Address, value: bigint, data?: Address) {
    return account.signTransaction({
        chainId: chain.id,
        gas,
        gasPrice,
        nonce,
        to,
        value,
        data,
    })
}
