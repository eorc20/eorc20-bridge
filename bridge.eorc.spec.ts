import { TimePointSec, Name, Asset } from "@greymass/eosio";
import { Blockchain } from "@proton/vert"
import { describe, expect, test, beforeEach } from "bun:test";
import { bytecode } from "./scripts/bytecode.js";

// Vert EOS VM
const blockchain = new Blockchain()

// contracts
const contract = blockchain.createContract('bridge.eorc', 'bridge.eorc', true);

// one-time setup
beforeEach(async () => {
  blockchain.setTime(TimePointSec.from("2024-01-17T00:00:00.000"));
});

function getToken(symcode: string) {
  const scope = Name.from('bridge.eorc').value.value;
  const primary_key = Asset.Symbol.from(symcode).value.value;
  return contract.tables.tokens(scope).getTableRow(primary_key);
}

function getTicker(tick: string) {
  const scope = Name.from('bridge.eorc').value.value;
  const primary_key = Name.from(tick).value.value;
  return contract.tables.tokens(scope).getTableRow(primary_key);
}

interface Configs {
  contract: string;
  hash: string;
  bytecode: string;
}

function getConfigs() {
  const scope = Name.from('bridge.eorc').value.value;
  return contract.tables.configs(scope).getTableRows()[0] as Configs
}

describe('bridge.eorc', () => {
  test('setconfig', async () => {
    const name = "BridgeEORC";
    await contract.actions.setconfig([name, bytecode.replace("0x", "")]).send();
    const config = getConfigs();
    expect(config.hash).toBe("825a207581dbdc4273697876ac567f0209248cd0d59f7e1630dd57ac7bbdd489");
    expect(config.contract).toBe(name);
  });
});

// /**
//  * Expect a promise to throw an error with a specific message.
//  * @param promise - The promise to await.
//  * @param {string} errorMsg - The error message that we expect to see.
//  */
// const expectToThrow = async (promise, errorMsg) => {
//   try {
//     await promise
//     assert.fail('Expected promise to throw an error');
//   } catch (e) {
//     if ( errorMsg ) assert.match(e.message, errorMsg);
//     else assert.fail('Expected promise to throw an error');
//   }
// }

function timeout(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}