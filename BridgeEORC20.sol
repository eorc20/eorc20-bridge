// SPDX-License-Identifier: MIT
pragma solidity ^0.8.18;

import {ERC20} from "./contracts/token/ERC20/ERC20.sol";
import {Ownable} from "./contracts/access/Ownable.sol";
import {Strings} from "./contracts/utils/Strings.sol";

// https://ethereum-magicians.org/t/discussion-on-erc-7583-for-inscribing-data-in-smart-contract/17661
interface IERC7583 {
    event TransferIns(address indexed from, address indexed to, uint64 indexed id);
    event Inscribe(uint64 indexed id, bytes data);
}

contract BridgeEORC20 is ERC20, Ownable, IERC7583 {
    address public evmAddress = 0xbBBBbBbbbBBBBbbbbbbBBbBB5530EA015b900000; // reserved address for eosio.evm
    address public bridgeAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000; // reserved address for bridge.eorc
    string public bridgeAccount = "bridge.eorc";
    uint64 public id = 0; // Inscription ID
    string public p = "eorc20"; // Protocol
    uint64 public max; // The maximum supply of the token
    string public tick; // The token ticker

    constructor(
        string memory _name,
        string memory _tick,
        uint64 _max
    ) ERC20(_name, _tick) Ownable(bridgeAddress) {
        tick = _tick;
        max = _max;
        _inscribe(_msgSender(), address(this), deployOp());
    }

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
        _beforeTokenTransfer(from, to, value);
        super._update(from, to, value);
        _afterTokenTransfer(from, to, value);
        require(totalSupply() <= max, "max supply reached");
        _inscribe(from, to, transferOp(value));
    }

    function _isReservedAddress(address addr) internal pure returns (bool) {
        return ((uint160(addr) & uint160(0xFffFfFffffFfFFffffFFFffF0000000000000000)) == uint160(0xBBbbBbBbbBbbBbbbBbbbBBbb0000000000000000));
    }

    function _beforeTokenTransfer(address from, address to, uint256 value) internal {
        // ignore mint and burn
        if (from == address(0) || to == address(0)) return;

        if (from == bridgeAddress) {
            require(_msgSender() == bridgeAddress, "unauthorized bridge sender");
            _mint(from, value);
        }
    }

    function _afterTokenTransfer(address from, address to, uint256 value) internal {
        // ignore mint and burn
        if (from == address(0) || to == address(0)) return;

        if (_isReservedAddress(to)) {
            _burn(to, value);
        }
    }

    // Inscription events on `deploy`, `mint` & `transfer`
    function _inscribe(address from, address to, bytes memory data) internal {
        emit Inscribe( id, data );
        emit TransferIns( from, to, id );
        _notifyBridge(from, to, data);
        id++;
    }

    function _notifyBridge(address from, address to, bytes memory data) internal {
        bytes memory receiver_msg = abi.encodePacked(id, from, to, data);
        (bool success, ) = evmAddress.call{value: msg.value}(
            abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", bridgeAccount, true, receiver_msg )
        );
        if (!success) { revert("error with bridge message"); }
    }

    function transferOp(uint256 value) internal view returns (bytes memory) {
        return abi.encodePacked('data:,{"p":"', p,'","op":"transfer","tick":"', tick, '","amt":"', Strings.toString(value), '"}');
    }

    function deployOp() internal view returns (bytes memory) {
        return abi.encodePacked('data:,{"p":"', p,'","op":"deploy","tick":"', tick, '","max":"', max, '"}');
    }
}