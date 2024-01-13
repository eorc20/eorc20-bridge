// SPDX-License-Identifier: MIT
pragma solidity ^0.8.18;

import {ERC20} from "./contracts/token/ERC20/ERC20.sol";
import {ERC20Pausable} from "./contracts/token/ERC20/extensions/ERC20Pausable.sol";
import {Ownable} from "./contracts/access/Ownable.sol";

contract EORC20 is ERC20Pausable, Ownable {
    string  public bridgeAccount = "bridge.eorc";
    address public bridgeAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000; // reserved address for bridge.eorc

    constructor() ERC20("EOSS eorc-20", "EOSS") Ownable(bridgeAddress) {
        _mint(bridgeAddress, 210000000000);
    }

    function decimals() public view virtual override returns (uint8) {
        return 0;
    }

    function pause() public onlyOwner {
        _pause();
    }

    function unpause() public onlyOwner {
        _unpause();
    }
}