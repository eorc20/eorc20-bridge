import { TimePointSec, Name } from "@greymass/eosio";
import { Blockchain } from "@proton/vert"
import { it, describe, beforeEach } from "node:test";
import assert from 'node:assert';

// Vert EOS VM
const blockchain = new Blockchain()

// contracts
const contract = blockchain.createContract('bridge.eorc', 'bridge.eorc', true);

const collection_name = "eorc";
blockchain.createAccounts(collection_name, 'myaccount`');

// one-time setup
beforeEach(async () => {
  blockchain.setTime(TimePointSec.from("2023-03-10T00:00:00.000"));
});

function getRow(asset_id) {
  const scope = Name.from('bridge.eorc').value.value;
  return contract.tables.ords(scope).getTableRow(BigInt(asset_id));
}

function getConfig() {
  const scope = Name.from('bridge.eorc').value.value;
  return contract.tables.config(scope).getTableRows()[0];
}

describe('bridge.eorc', () => {

});

/**
 * Expect a promise to throw an error with a specific message.
 * @param promise - The promise to await.
 * @param {string} errorMsg - The error message that we expect to see.
 */
const expectToThrow = async (promise, errorMsg) => {
  try {
    await promise
    assert.fail('Expected promise to throw an error');
  } catch (e) {
    if ( errorMsg ) assert.match(e.message, errorMsg);
    else assert.fail('Expected promise to throw an error');
  }
}

function timeout(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}