import { parseAbi } from "viem";

export const abi = parseAbi([
    'function transfer(address to, uint256 amount) returns (bool)',
    'function balanceOf(address account) returns (uint256)',
    'function mint(address to, uint256 value) public',
    'function burn(uint256 value) public',
    'function setFee(uint256 _egressFee) public'
])