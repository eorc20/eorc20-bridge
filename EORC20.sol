// SPDX-License-Identifier: MIT
pragma solidity ^0.8.18;

import {ERC20} from "./contracts/token/ERC20/ERC20.sol";

contract EORC20 is ERC20 {
    string  public bridgeAccount = "bridge.eorc";
    address public bridgeAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000; // reserved address for bridge.eorc

    constructor() ERC20("EOSS eorc-20", "EOSS") {
        _mint(bridgeAddress, 210000000000);
    }

    function decimals() public view virtual override returns (uint8) {
        return 0;
    }
}