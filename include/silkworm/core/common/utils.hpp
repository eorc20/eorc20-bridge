/*
   Copyright 2022 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <optional>

namespace silkworm {

inline std::optional<uint64_t> extract_reserved_address(const evmc::address& addr) {
    constexpr uint8_t reserved_address_prefix[] = {0xbb, 0xbb, 0xbb, 0xbb,
                                                   0xbb, 0xbb, 0xbb, 0xbb,
                                                   0xbb, 0xbb, 0xbb, 0xbb};

    if(!std::equal(std::begin(reserved_address_prefix), std::end(reserved_address_prefix), static_cast<evmc::bytes_view>(addr).begin()))
        return std::nullopt;
    uint64_t reserved;
    memcpy(&reserved, static_cast<evmc::bytes_view>(addr).data()+sizeof(reserved_address_prefix), sizeof(reserved));
    return be64toh(reserved);
}

inline bool is_reserved_address(const evmc::address& addr) {
    return extract_reserved_address(addr) != std::nullopt;
}

} // namespace silkworm