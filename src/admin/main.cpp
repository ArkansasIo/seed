#include "seed/config.hpp"
#include "seed/database.hpp"
#include "seed/ui.hpp"
#include <iostream>
#include <string>
int main(int argc,char**argv){
 try{
  seed::boot_loader();auto c=seed::Config::load(argc>1?argv[1]:"config/seed.ini");seed::Database db(c);
  if(!db.connect()){std::cerr<<"Database connection failed\\n";return 2;}
  std::string e;if(!db.initialize_schema(&e)){std::cerr<<e<<"\\n";return 3;}
  if(!db.bootstrap_admin_from_environment(&e)){std::cerr<<e<<"\\n";return 4;}
  std::string name,password;std::cout<<"Admin username: ";std::getline(std::cin,name);std::cout<<"Admin password (visible in this starter): ";std::getline(std::cin,password);
  auto actor=db.authenticate(name,password);if(!actor||actor->role!=seed::Role::Admin){std::cerr<<"Admin authentication failed\\n";return 5;}
  std::cout<<"LOCAL ADMIN CONTROL PANEL — authenticated as "<<actor->username<<"\\n";
  for(;;){std::cout<<"1 List users 2 Enable 3 Disable 4 Promote admin 5 Demote user 0 Exit\\n> ";std::string s;if(!std::getline(std::cin,s)||s=="0")break;
   if(s=="1"){for(auto&u:db.list_users())std::cout<<u.id<<" | "<<u.username<<" | "<<seed::role_name(u.role)<<" | "<<(u.enabled?"enabled":"disabled")<<"\\n";continue;}
   if(s<"2"||s>"5")continue;std::string n;std::cout<<"User ID: ";std::getline(std::cin,n);
   try{auto id=std::stoull(n);bool ok=s=="2"?db.set_user_enabled(id,true,&e):s=="3"?db.set_user_enabled(id,false,&e):s=="4"?db.set_user_role(id,seed::Role::Admin,&e):db.set_user_role(id,seed::Role::User,&e);std::cout<<(ok?"Updated":e)<<"\\n";}catch(...){std::cout<<"Invalid ID\\n";}
  }return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<"\\n";return 1;}
}