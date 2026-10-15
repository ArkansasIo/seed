#pragma once
#include "seed/config.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
namespace seed {
enum class Role { User, Admin };
struct User { std::uint64_t id{}; std::string username,salt_hex,password_hash_hex; Role role{Role::User}; bool enabled{true}; };
class Database {
public:
 explicit Database(const Config&); ~Database(); Database(const Database&)=delete; Database& operator=(const Database&)=delete;
 bool connect(); bool initialize_schema(std::string* error=nullptr);
 bool bootstrap_admin_from_environment(std::string* error=nullptr);
 bool register_user(const std::string&,const std::string&,std::string* error=nullptr);
 std::optional<User> authenticate(const std::string&,const std::string&);
 std::optional<User> find_user(const std::string&); std::optional<User> find_user(std::uint64_t);
 bool set_user_enabled(std::uint64_t,bool,std::string* error=nullptr);
 bool set_user_role(std::uint64_t,Role,std::string* error=nullptr);
 std::vector<User> list_users(); bool mysql_enabled() const noexcept;
private: struct Impl; std::unique_ptr<Impl> impl_;
};
std::string role_name(Role);
}
