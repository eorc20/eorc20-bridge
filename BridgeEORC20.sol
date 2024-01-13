// SPDX-License-Identifier: MIT
pragma solidity ^0.8.18;

import {Ownable} from "./contracts/access/Ownable.sol";

// https://ethereum-magicians.org/t/discussion-on-erc-7583-for-inscribing-data-in-smart-contract/17661
interface IERC7583 {
    event TransferIns(address indexed from, address indexed to, uint256 indexed id);
    event Inscribe(uint256 indexed id, bytes data);
}

// data:,{"p":"eorc20","op":"transfer","tick":"eoss","amt":"10"}
// 0x646174613a2c7b2270223a22656f72633230222c226f70223a227472616e73666572222c227469636b223a22656f7373222c22616d74223a223130227d
contract BridgeEORC20 is IERC7583, Ownable {
    string  public bridgeAccount = "bridge.eorc";
    address public bridgeAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000; // reserved address for bridge.eorc
    uint256 public id = 0;

    constructor() Ownable(bridgeAddress) {}

    // when calling inscribe from a contract, the contract address is used as `from`
    // any EORC-20 tokens held by the contract can be transferred to the `to` address
    // facilitates withdrawal of EORC-20 tokens from the contract
    function inscribe(address from, address to, bytes calldata data) public onlyOwner {
        id++;
        emit Inscribe( id, data );
        emit TransferIns( from, to, id );
    }

    // This function is executed when a contract receives plain Ether (without data)
    receive() external payable {
        revert("cannot call contract without calldata");
    }

    // EVM/Native bridge
    // =================
    // handles incoming inscriptions
    fallback() external payable {
        require(tx.origin == msg.sender, "contracts not allowed");

        // append `from` sender with message calldata
        bytes memory data = abi.encodePacked(msg.sender, msg.data);

        // push calldata to EOS Native
        address evmAddress = 0xbBBBbBbbbBBBBbbbbbbBBbBB5530EA015b900000; // reserved address for eosio.evm
        (bool success, ) = evmAddress.call{value: msg.value}(
            abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", bridgeAccount, true, data )
        );
        if (!success) { revert(); }
    }
}