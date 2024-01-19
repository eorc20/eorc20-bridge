import { Session, Chains } from "@wharfkit/session";
import { WalletPluginPrivateKey } from "@wharfkit/wallet-plugin-privatekey";
import { Chain, createPublicClient, http, parseGwei } from "viem";
import { eosTestnet } from "viem/chains";

// Wharfkit
if ( !process.env.ACTOR) throw new Error("ACTOR is required");
if ( !process.env.PRIVATE_KEY) throw new Error("PRIVATE_KEY is required");
export const ACTOR = process.env.ACTOR
export const PERMISSION = process.env.PERMISSION ?? "active";
export const PRIVATE_KEY = process.env.PRIVATE_KEY;
export const CONTRACT = "0x59C2ffFB3541A8d50AE75AE3C650F029509aCDBE";

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
