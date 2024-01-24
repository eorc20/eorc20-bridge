import { Session, Chains } from "@wharfkit/session";
import { WalletPluginPrivateKey } from "@wharfkit/wallet-plugin-privatekey";
import { Chain, Hex, createPublicClient, http, parseGwei } from "viem";
import { eosTestnet } from "viem/chains";
import { privateKeyToAccount } from 'viem/accounts'
import { createWalletClient } from 'viem'


// Wharfkit
if ( !process.env.ACTOR) throw new Error("ACTOR is required");
if ( !process.env.PRIVATE_KEY) throw new Error("PRIVATE_KEY is required");
export const ACTOR = process.env.ACTOR
export const PERMISSION = process.env.PERMISSION ?? "active";
export const PRIVATE_KEY = process.env.PRIVATE_KEY;
export const CONTRACT = "0xBcb1C286ABC2ad777227802414d6Ef53d6E3362A";

export const walletPlugin = new WalletPluginPrivateKey(process.env.PRIVATE_KEY);

export const session = new Session({
    chain: Chains.Jungle4,
    actor: ACTOR,
    permission: PERMISSION,
    walletPlugin,
})

// Chain specific
export const chain: Chain = eosTestnet;
export const client = createPublicClient({chain, transport: http() })
export const gas = 1500000n; // Gas Limit
export const gasPrice = parseGwei('900'); // Gas Price (Fixed to 150 Gwei)

if ( !process.env.EVM_PRIVATE_KEY) throw new Error("EVM_PRIVATE_KEY is required");

export const walletClient = createWalletClient({
    account: privateKeyToAccount(process.env.EVM_PRIVATE_KEY as Hex),
    chain,
    transport: http(),
})