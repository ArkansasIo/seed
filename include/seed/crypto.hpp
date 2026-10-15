#pragma once
#include <string>
#include <string_view>
namespace seed {
std::string random_salt_hex(std::size_t bytes=16);
std::string password_hash(std::string_view password,std::string_view salt_hex,unsigned iterations);
bool constant_time_equal(std::string_view a,std::string_view b) noexcept;
}
