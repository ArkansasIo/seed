#include "seed/database.hpp"
#include "seed/crypto.hpp"
#include <cstdlib>
#include <map>
#include <mutex>
#ifdef SEED_WITH_MYSQL
#include <mysql.h>
#endif
namespace seed {
struct Database::Impl{Config c;std::mutex m;
#ifdef SEED_WITH_MYSQL
MYSQL* db{};
#else
std::map<uint64_t,User> users;uint64_t next{1};
#endif
explicit Impl(const Config&x):c(x){}};
std::string role_name(Role r){return r==Role::Admin?"admin":"user";}
Database::Database(const Config&c):impl_(std::make_unique<Impl>(c)){}
Database::~Database(){
#ifdef SEED_WITH_MYSQL
if(impl_->db)mysql_close(impl_->db);
#endif
}
bool Database::connect(){
#ifdef SEED_WITH_MYSQL
impl_->db=mysql_init(nullptr);if(!impl_->db)return false;unsigned t=impl_->c.connect_timeout_seconds;mysql_options(impl_->db,MYSQL_OPT_CONNECT_TIMEOUT,&t);return mysql_real_connect(impl_->db,impl_->c.db_host.c_str(),impl_->c.db_user.c_str(),impl_->c.db_password.c_str(),impl_->c.db_name.c_str(),impl_->c.db_port,nullptr,0)!=nullptr;
#else
return true;
#endif
}
bool Database::mysql_enabled()const noexcept{
#ifdef SEED_WITH_MYSQL
return true;
#else
return false;
#endif
}
bool Database::initialize_schema(std::string*e){
#ifdef SEED_WITH_MYSQL
const char*q[]={"CREATE TABLE IF NOT EXISTS users(id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,username VARCHAR(64) NOT NULL UNIQUE,salt_hex CHAR(32) NOT NULL,password_hash_hex VARCHAR(128) NOT NULL,role ENUM('user','admin') NOT NULL DEFAULT 'user',enabled BOOLEAN NOT NULL DEFAULT TRUE) ENGINE=InnoDB","CREATE TABLE IF NOT EXISTS sessions(token CHAR(64) PRIMARY KEY,user_id BIGINT UNSIGNED NOT NULL,expires_at TIMESTAMP NOT NULL,FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE) ENGINE=InnoDB"};for(auto s:q)if(mysql_query(impl_->db,s)){if(e)*e=mysql_error(impl_->db);return false;}return true;
#else
if(e)*e="MySQL disabled: in-memory development mode";return true;
#endif
}
bool Database::bootstrap_admin_from_environment(std::string*e){auto u=getenv("SEED_BOOTSTRAP_ADMIN_USER"),p=getenv("SEED_BOOTSTRAP_ADMIN_PASSWORD");if(!u||!p||!*u||!*p)return true;if(find_user(u))return true;if(std::string(p).size()<12){if(e)*e="Admin password must be at least 12 characters";return false;}if(!register_user(u,p,e))return false;auto a=find_user(u);return a&&set_user_role(a->id,Role::Admin,e);}
bool Database::register_user(const std::string&name,const std::string&pass,std::string*e){if(name.size()<3||name.size()>64||pass.size()<12){if(e)*e="Username must be 3-64 chars and password at least 12 chars";return false;}auto salt=random_salt_hex();auto hash=password_hash(pass,salt,impl_->c.pbkdf2_iterations);std::lock_guard l(impl_->m);
#ifdef SEED_WITH_MYSQL
MYSQL_STMT*s=mysql_stmt_init(impl_->db);const char*q="INSERT INTO users(username,salt_hex,password_hash_hex) VALUES(?,?,?)";if(!s||mysql_stmt_prepare(s,q,strlen(q))){if(e)*e="Could not prepare registration";if(s)mysql_stmt_close(s);return false;}MYSQL_BIND b[3]{};unsigned long lens[3]={static_cast<unsigned long>(name.size()),static_cast<unsigned long>(salt.size()),static_cast<unsigned long>(hash.size())};const char*v[3]={name.c_str(),salt.c_str(),hash.c_str()};for(int i=0;i<3;++i){b[i].buffer_type=MYSQL_TYPE_STRING;b[i].buffer=(void*)v[i];b[i].buffer_length=lens[i];b[i].length=&lens[i];}mysql_stmt_bind_param(s,b);bool ok=mysql_stmt_execute(s)==0;if(!ok&&e)*e="Registration failed; username may exist";mysql_stmt_close(s);return ok;
#else
for(auto&[id,x]:impl_->users)if(x.username==name){if(e)*e="Username exists";return false;}User x;x.id=impl_->next++;x.username=name;x.salt_hex=salt;x.password_hash_hex=hash;impl_->users.emplace(x.id,x);return true;
#endif
}
std::optional<User> Database::find_user(const std::string&n){
#ifdef SEED_WITH_MYSQL
MYSQL_STMT*s=mysql_stmt_init(impl_->db);const char*q="SELECT id,username,salt_hex,password_hash_hex,role,enabled FROM users WHERE username=?";if(!s||mysql_stmt_prepare(s,q,strlen(q))){if(s)mysql_stmt_close(s);return {};}MYSQL_BIND in{};unsigned long len=n.size();in.buffer_type=MYSQL_TYPE_STRING;in.buffer=(void*)n.c_str();in.buffer_length=len;in.length=&len;mysql_stmt_bind_param(s,&in);if(mysql_stmt_execute(s)){mysql_stmt_close(s);return {};}MYSQL_BIND b[6]{};unsigned long lens[6]{};unsigned long long id{};char name[65]{},salt[33]{},hash[129]{},role[8]{};my_bool enabled{};b[0].buffer_type=MYSQL_TYPE_LONGLONG;b[0].buffer=&id;b[0].is_unsigned=1;char*buf[]={name,salt,hash,role};unsigned long cap[]={64,32,128,7};for(int i=0;i<4;++i){b[i+1].buffer_type=MYSQL_TYPE_STRING;b[i+1].buffer=buf[i];b[i+1].buffer_length=cap[i];b[i+1].length=&lens[i+1];}b[5].buffer_type=MYSQL_TYPE_TINY;b[5].buffer=&enabled;mysql_stmt_bind_result(s,b);int rc=mysql_stmt_fetch(s);mysql_stmt_close(s);if(rc)return {};return User{id,name,salt,hash,std::string(role)=="admin"?Role::Admin:Role::User,enabled!=0};
#else
std::lock_guard l(impl_->m);for(auto&[id,u]:impl_->users)if(u.username==n)return u;return {};
#endif
}
std::optional<User> Database::find_user(uint64_t id){
#ifdef SEED_WITH_MYSQL
MYSQL_STMT*s=mysql_stmt_init(impl_->db);const char*q="SELECT username FROM users WHERE id=?";if(!s||mysql_stmt_prepare(s,q,strlen(q))){if(s)mysql_stmt_close(s);return {};}unsigned long long key=id;MYSQL_BIND in{};in.buffer_type=MYSQL_TYPE_LONGLONG;in.buffer=&key;in.is_unsigned=1;mysql_stmt_bind_param(s,&in);if(mysql_stmt_execute(s)){mysql_stmt_close(s);return {};}char name[65]{};MYSQL_BIND b{};b.buffer_type=MYSQL_TYPE_STRING;b.buffer=name;b.buffer_length=64;mysql_stmt_bind_result(s,&b);int rc=mysql_stmt_fetch(s);mysql_stmt_close(s);if(rc)return {};return find_user(name);
#else
std::lock_guard l(impl_->m);auto i=impl_->users.find(id);if(i==impl_->users.end())return {};return i->second;
#endif
}
std::optional<User> Database::authenticate(const std::string&n,const std::string&p){auto u=find_user(n);if(!u||!u->enabled)return {};if(!constant_time_equal(password_hash(p,u->salt_hex,impl_->c.pbkdf2_iterations),u->password_hash_hex))return {};return u;}
bool Database::set_user_enabled(uint64_t id,bool on,std::string*e){auto u=find_user(id);if(!u){if(e)*e="User not found";return false;}
#ifdef SEED_WITH_MYSQL
MYSQL_STMT*s=mysql_stmt_init(impl_->db);const char*q="UPDATE users SET enabled=? WHERE id=?";if(!s||mysql_stmt_prepare(s,q,strlen(q))){if(s)mysql_stmt_close(s);return false;}my_bool v=on;unsigned long long k=id;MYSQL_BIND b[2]{};b[0].buffer_type=MYSQL_TYPE_TINY;b[0].buffer=&v;b[1].buffer_type=MYSQL_TYPE_LONGLONG;b[1].buffer=&k;b[1].is_unsigned=1;mysql_stmt_bind_param(s,b);bool ok=mysql_stmt_execute(s)==0;mysql_stmt_close(s);return ok;
#else
std::lock_guard l(impl_->m);impl_->users[id].enabled=on;return true;
#endif
}
bool Database::set_user_role(uint64_t id,Role r,std::string*e){auto u=find_user(id);if(!u){if(e)*e="User not found";return false;}
#ifdef SEED_WITH_MYSQL
MYSQL_STMT*s=mysql_stmt_init(impl_->db);const char*q="UPDATE users SET role=? WHERE id=?";if(!s||mysql_stmt_prepare(s,q,strlen(q))){if(s)mysql_stmt_close(s);return false;}std::string v=role_name(r);unsigned long n=v.size();unsigned long long k=id;MYSQL_BIND b[2]{};b[0].buffer_type=MYSQL_TYPE_STRING;b[0].buffer=(void*)v.c_str();b[0].buffer_length=n;b[0].length=&n;b[1].buffer_type=MYSQL_TYPE_LONGLONG;b[1].buffer=&k;b[1].is_unsigned=1;mysql_stmt_bind_param(s,b);bool ok=mysql_stmt_execute(s)==0;mysql_stmt_close(s);return ok;
#else
std::lock_guard l(impl_->m);impl_->users[id].role=r;return true;
#endif
}
std::vector<User> Database::list_users(){std::vector<User>o;
#ifdef SEED_WITH_MYSQL
if(mysql_query(impl_->db,"SELECT id,username,salt_hex,password_hash_hex,role,enabled FROM users ORDER BY id"))return o;MYSQL_RES*r=mysql_store_result(impl_->db);if(!r)return o;MYSQL_ROW row;while((row=mysql_fetch_row(r))){User u;u.id=strtoull(row[0],nullptr,10);u.username=row[1]?row[1]:"";u.salt_hex=row[2]?row[2]:"";u.password_hash_hex=row[3]?row[3]:"";u.role=row[4]&&std::string(row[4])=="admin"?Role::Admin:Role::User;u.enabled=row[5]&&std::string(row[5])=="1";o.push_back(u);}mysql_free_result(r);
#else
std::lock_guard l(impl_->m);for(auto&[id,u]:impl_->users)o.push_back(u);
#endif
return o;}
}
