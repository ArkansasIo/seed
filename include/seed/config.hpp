#pragma once
#include <cstdint>
#include <string>
namespace seed {
struct Config {
 std::string host{"127.0.0.1"}; std::uint16_t port{43123}; int max_clients{64};
 std::string db_host{"127.0.0.1"}; std::uint16_t db_port{3306};
 std::string db_name{"seed_engine"}, db_user{"seed_app"}, db_password;
 unsigned connect_timeout_seconds{5}, session_ttl_minutes{60}, pbkdf2_iterations{310000};
 static Config load(const std::string& path);
};
}
