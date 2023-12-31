// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.x;

interface IERC7583 {
    event Inscribe(bytes data);
}

// https://ethereum-magicians.org/t/discussion-on-erc-7583-for-inscribing-data-in-smart-contract/17661
// 0x646174613a2c7b2270223a22656f72633230222c226f70223a227472616e73666572222c227469636b223a22656f7373222c22616d74223a223130227d
// data:,{"p":"eorc20","op":"transfer","tick":"eoss","amt":"10"}
contract BridgeEORC20 is IERC7583 {
    address public evmAddress = 0xbBBBbBbbbBBBBbbbbbbBBbBB5530EA015b900000;
    string  public account = "bridge.eorc";
    address public owner = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000;
    event Received(address, uint256);
    event CallData(address, bytes);

    function inscribe(bytes calldata data) public {
        require(msg.sender == owner);
        emit Inscribe(data);
    }

    // This function is executed when a contract receives plain Ether (without data)
    receive() external payable {
        revert();
    }

    fallback() external payable {
        // add `from` to calldata
        bytes memory data = abi.encodePacked(msg.sender, msg.data);

        // push calldata to EOS Native
        (bool success, ) = evmAddress.call{value: msg.value}(
            abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", account, true, data )
        );
        if (!success) { revert(); }
    }
}