// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import {ERC20} from "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import {Ownable} from "@openzeppelin/contracts/access/Ownable.sol";
import {Strings} from "@openzeppelin/contracts/utils/Strings.sol";

// https://ethereum-magicians.org/t/discussion-on-erc-7583-for-inscribing-data-in-smart-contract/17661
interface IERC7583 {
    event TransferIns(address indexed from, address indexed to, uint64 indexed id);
    event Inscribe(uint64 indexed id, bytes data);
}

contract BridgeEORC20 is ERC20, IERC7583, Ownable {
    address private evmAddress = 0xbBBBbBbbbBBBBbbbbbbBBbBB5530EA015b900000; // Reserved address for eosio.evm
    address public bridgeAddress = 0xbBbbBBbBbbBBbbbbbbbbBbBB3Ddc96280Aa5D000; // Bridge contract address
    string public bridgeAccount = "bridge.eorc";
    uint64 public id = 0; // Inscription ID
    string public p = "eorc-20"; // Inscription Protocol
    string public tick; // The token ticker
    uint64 public max; // The maximum supply of the token
    uint64 public lim; // Maximum mint per transaction (0 = snapshot)

    constructor(
        string memory name_,
        string memory symbol_,
        string memory tick_,
        uint64 max_,
        uint64 lim_
    ) ERC20(name_, symbol_) Ownable(bridgeAddress) {
        require(max_ > 0, "max supply must be greater than 0");
        require(lim_ <= max_, "max mint per transaction must be less than max supply");
        tick = tick_;
        max = max_;
        lim = lim_;
        _inscribe(_msgSender(), address(0), deployOp());
    }

    function decimals() public view virtual override returns (uint8) {
        return 0;
    }

    // mint is used for snapshot distribution
    function mint(address to, uint256 value) public onlyOwner returns (bool) {
        if ( lim > 0 ) {
            require(value <= lim, "mint amount exceeds limit");
        }
        _mint(to, value);
        require(totalSupply() <= max, "max supply reached");
        return true;
    }

    function _isReservedAddress(address addr) internal pure returns (bool) {
        return ((uint160(addr) & uint160(0xFffFfFffffFfFFffffFFFffF0000000000000000)) == uint160(0xBBbbBbBbbBbbBbbbBbbbBBbb0000000000000000));
    }

    function _update(address from, address to, uint256 value) override internal {
        super._update(from, to, value);

        // inscribe mint or transfer
        if ( from == address(0) ) {
            _inscribe(to, address(0), mintOp(value));
        } else {
            _inscribe(from, to, transferOp(value));
        }
        _afterTokenTransfer(to, value);
    }

    function _afterTokenTransfer(address to, uint256 value) internal {
        // all transfers to reserved address will be escrowed bridge address
        if (_isReservedAddress(to)) {
            _transfer(to, bridgeAddress, value);
        }
    }

    // Inscription events on `deploy`, `mint` & `transfer`
    function _inscribe(address from, address to, bytes memory data) internal {
        emit Inscribe( id, data );
        emit TransferIns( from, to, id );
        _notifyBridge( from, to, data );
        id++;
    }

    function _notifyBridge(address from, address to, bytes memory data) internal {
        bytes memory receiver_msg = abi.encodePacked(_msgSender(), from, to, id, data);
        (bool success, ) = evmAddress.call{value: msg.value}(
            abi.encodeWithSignature("bridgeMsgV0(string,bool,bytes)", bridgeAccount, true, receiver_msg )
        );
        if (!success) { revert("error with bridge message"); }
    }

    function mintOp(uint256 value) internal view returns (bytes memory) {
        return abi.encodePacked('data:,{"p":"', p,'","op":"mint","tick":"', tick, '","amt":"', Strings.toString(value), '"}');
    }

    function transferOp(uint256 value) internal view returns (bytes memory) {
        return abi.encodePacked('data:,{"p":"', p,'","op":"transfer","tick":"', tick, '","amt":"', Strings.toString(value), '"}');
    }

    function deployOp() internal view returns (bytes memory) {
        return abi.encodePacked('data:,{"p":"', p,'","op":"deploy","tick":"', tick, '","max":"', Strings.toString(max), '","lim":"', Strings.toString(lim), '"}');
    }

    // This function is executed when a contract receives plain Ether (without data)
    receive() external payable {
        revert("cannot call contract without calldata");
    }

    // fallback is a special function that is executed either when
    // shall only be used for user submitted inscription `mint` operation
    // payable fee for bridge message may be required to cover gas fees
    // invalid mint operations will be reverted by bridge
    fallback() external payable {
        _notifyBridge(_msgSender(), address(this), msg.data);
    }
}