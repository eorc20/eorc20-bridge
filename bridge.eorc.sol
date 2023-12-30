// SPDX-License-Identifier: GPL-3.0
pragma solidity >=0.4.16 <0.9.0;

// 0x646174613a2c7b2270223a22656f72633230222c226f70223a227472616e73666572222c227469636b223a22656f7373222c22616d74223a223130227d
// data:,{"p":"eorc20","op":"transfer","tick":"eoss","amt":"10"}
contract BridgeEORC20 {
    address public evmAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000;
    string  public account = "bridge.eorc";
    event Received(address, uint256);
    event Data(address, bytes);

    receive() external payable {
        emit Received(msg.sender, msg.value);
    }

    fallback() external payable {
        emit Data(msg.sender, msg.data);
        (bool success, ) = evmAddress.call{value: msg.value}(abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", account, true, msg.data ));
        if (!success) { revert(); }
    }
}