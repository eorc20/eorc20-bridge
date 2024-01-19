import { keccak256, toBytes } from 'viem'

const method = 'transfer(address,uint256)';
const hash = keccak256(toBytes(method));
const firstFourBytes = hash.substring(0, 10); // '0x' + first 8 characters of the hash

console.log(firstFourBytes);