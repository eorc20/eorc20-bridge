import { AnyAction, Session } from "@wharfkit/session";
import { toTransactionId } from "./eos.evm.js";
import { chain, session } from "./config.js";

export async function transact(actions: AnyAction[]) {
    const response = await session.transact({actions});
    if ( response.response ) {
        const trx_id = response.response.transaction_id;
        console.info(`EOS\t${new Date().toISOString()}\thttps://eos.eosq.eosnation.io/tx/${trx_id}`);
        const { action_traces } = response.response.processed;
        for ( const action_trace of action_traces ) {
            const rlptx = action_trace.act.data.rlptx;
            if ( rlptx ) {
                console.info(`EVM\t${new Date().toISOString()}\t${chain.blockExplorers.default.url}/tx/${toTransactionId(rlptx)}`);
            }
        }
    }
}