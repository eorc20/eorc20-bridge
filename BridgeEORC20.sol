// SPDX-License-Identifier: MIT
pragma solidity ^0.8.18;

import {ERC20} from "./contracts/token/ERC20/ERC20.sol";
import {Ownable} from "./contracts/access/Ownable.sol";
import {Strings} from "./contracts/utils/Strings.sol";

// https://ethereum-magicians.org/t/discussion-on-erc-7583-for-inscribing-data-in-smart-contract/17661
interface IERC7583 {
    event TransferIns(address indexed from, address indexed to, uint256 indexed id);
    event Inscribe(uint256 indexed id, bytes data);
}

contract BridgeEORC20 is ERC20, Ownable, IERC7583 {
    address public evmAddress = 0xbBBBbBbbbBBBBbbbbbbBBbBB5530EA015b900000; // reserved address for eosio.evm
    address public bridgeAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000; // reserved address for bridge.eorc
    string  public bridgeAccount = "bridge.eorc";
    uint256 public id = 0;

    constructor(
        string memory _name,
        string memory _symbol
    ) ERC20(_name, _symbol) Ownable(bridgeAddress) {}

    function decimals() public view virtual override returns (uint8) {
        return 0;
    }

    function mint(address to, uint256 value) public onlyOwner {
        _mint(to, value);
    }

    function burn(uint256 value) public {
        _burn(_msgSender(), value);
    }

    function _update(address from, address to, uint256 value) override internal {
        super._update(from, to, value);
        _afterTokenTransfer(from, to, value);
        _inscribe(from, to, value);
    }

    function _isReservedAddress(address addr) internal pure returns (bool) {
        return ((uint160(addr) & uint160(0xFffFfFffffFfFFffffFFFffF0000000000000000)) == uint160(0xBBbbBbBbbBbbBbbbBbbbBBbb0000000000000000));
    }

    function _afterTokenTransfer(address from, address to, uint256 value) internal {
        // ignore mint and burn
        if (from == address(0) || to == address(0)) return;
        if (_isReservedAddress(to)) {
            _notifyBridge(from, to, value);
            _burn(to, value);
        }
    }

    function _notifyBridge(address from, address to, uint256 value) internal {
        bytes memory data = transferOp(symbol(), value);
        bytes memory receiver_msg = abi.encodePacked(from, to, data);
        (bool success, ) = evmAddress.call{value: msg.value}(abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", bridgeAccount, true, receiver_msg ));
        if (!success) { revert("error sending bridge message"); }
    }

    // Inscription events on Transfer
    function _inscribe(address from, address to, uint256 value) internal {
        bytes memory data = transferOp(symbol(), value);
        emit Inscribe( id, data );
        emit TransferIns( from, to, id );
        id++;
    }

    function transferOp(string memory tick, uint256 value) internal pure returns (bytes memory) {
        return abi.encodePacked('data:,{"p":"eorc20","op":"transfer","tick":"', tick, '","amt":"', Strings.toString(value), '"}');
    }

    // This function is executed when a contract receives plain Ether (without data)
    receive() external payable {
        revert("cannot call contract without calldata");
    }

    // Handles incoming inscriptions using calldata
    //
    // data:,{"p":"eorc20","op":"transfer","tick":"eoss","amt":"10"}
    // 0x646174613a2c7b2270223a22656f72633230222c226f70223a227472616e73666572222c227469636b223a22656f7373222c22616d74223a223130227d
    fallback() external payable {
        address from = _msgSender();
        address to = address(this);
        require(tx.origin == from, "contracts not allowed");

        // append `from` sender with message calldata
        bytes memory receiver_msg = abi.encodePacked(from, to, _msgData());

        // push calldata to EOS Native
        (bool success, ) = evmAddress.call{value: msg.value}(abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", bridgeAccount, true, receiver_msg ));
        if (!success) { revert(); }
    }
}