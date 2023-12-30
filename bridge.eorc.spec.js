import { TimePointSec, Name } from "@greymass/eosio";
import { Blockchain } from "@proton/vert"
import { it, describe, beforeEach } from "node:test";
import assert from 'node:assert';

// Vert EOS VM
const blockchain = new Blockchain()

// contracts
const contract = blockchain.createContract('bridge.eorc', 'bridge.eorc', true);
const atomic = blockchain.createContract('atomicassets', './include/atomicassets/atomicassets', true);

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
  it("atomicassets", async () => {
    await atomic.actions.init([]).send();
    await atomic.actions.createcol([collection_name, collection_name, true, [collection_name], [], 0, []]).send(collection_name);
    await atomic.actions.createschema([collection_name, collection_name, "inscriptions", [{"name": "name", "type": "string"}]]).send(collection_name);
    await atomic.actions.createtempl([collection_name, collection_name, "inscriptions", true, true, 0, [{"key": "name", "value": ["string", "astronaut"]}]]).send(collection_name);
    await atomic.actions.mintasset([collection_name, collection_name, "inscriptions", 1, "myaccount", [], [], []]).send(collection_name);
    await atomic.actions.mintasset([collection_name, collection_name, "inscriptions", 1, "myaccount", [], [], []]).send(collection_name);
    await atomic.actions.mintasset([collection_name, collection_name, "inscriptions", 1, "myaccount", [], [], []]).send(collection_name);
    await atomic.actions.mintasset([collection_name, collection_name, "inscriptions", 1, "myaccount", [], [], []]).send(collection_name);

    assert.ok(true)
  });

  it("transfer", async () => {
    await atomic.actions.transfer(["myaccount", "bridge.eorc", [1099511627776, 1099511627777], "0x856ed111f878a6CC5c0E7784E5988f2DD9388181"]).send("myaccount");
  });
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