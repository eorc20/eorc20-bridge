import { TimePointSec, Name, Asset } from "@greymass/eosio";
import { Blockchain } from "@proton/vert"
import { describe, expect, test, beforeEach } from "bun:test";
import { bytecode } from "./scripts/bytecode.js";

// Vert EOS VM
const blockchain = new Blockchain()

const sym = "0,EOSS"
const symcode = "EOSS"
const bridge_contract = "bridge.eorc"
const token_contract = "token.eorc"
const tick = "eoss"
const name = "EOSS eorc-20"
const max = 210000000000
const address = "59c2fffb3541a8d50ae75ae3c650f029509acdbe"
blockchain.createAccount(token_contract);

// one-time setup
beforeEach(async () => {
  blockchain.setTime(TimePointSec.from("2024-01-17T00:00:00.000"));
});

const contracts = {
  bridge: blockchain.createContract(bridge_contract, bridge_contract, true),
  token: blockchain.createContract(token_contract, 'include/eosio.token/eosio.token', true),
}

function getToken(symcode: string) {
  const scope = Name.from(bridge_contract).value.value;
  const primary_key = Asset.SymbolCode.from(symcode).value.value;
  return contracts.bridge.tables.tokens(scope).getTableRow(primary_key);
}

interface Configs {
  contract: string;
  hash: string;
  bytecode: string;
}

function getConfigs() {
  const scope = Name.from(bridge_contract).value.value;
  return contracts.bridge.tables.configs(scope).getTableRows()[0] as Configs
}

describe(bridge_contract, () => {
  test('token::create', async () => {
    const supply = `${max} ${symcode}`;
    await contracts.token.actions.create([token_contract, supply]).send();
  });

  test('setconfig', async () => {
    const name = "BridgeEORC";
    await contracts.bridge.actions.setconfig([name, bytecode.replace("0x", "")]).send();
    const config = getConfigs();
    expect(config.hash).toBe("24ffd55248685e1781db1cb9393a162b9bd1150981063cc2b946f0d159d130e1");
    expect(config.contract).toBe(name);
  });

  test('regtoken', async () => {
    await contracts.bridge.actions.regtoken([symcode, token_contract, tick, name, max, address]).send();
    const token = getToken(symcode);
    expect(token.sym).toBe(sym);
    expect(token.contract).toBe(token_contract);
    expect(token.name).toBe(name);
    expect(token.tick).toBe(tick);
    expect(Number(token.max)).toBe(max);
    expect(token.address).toBe(address);
  });

  test('deltoken', async () => {
    await contracts.token.actions.create([token_contract, "1.0000 DELETE"]).send();
    await contracts.bridge.actions.regtoken(["DELETE", token_contract, "delete", name, max, address]).send();
    expect(getToken("DELETE")).toBeDefined();
    await contracts.bridge.actions.deltoken(["DELETE"]).send();
    expect(getToken("DELETE")).toBeUndefined();
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