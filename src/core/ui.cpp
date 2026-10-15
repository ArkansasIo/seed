#include "seed/ui.hpp"
#include <chrono>
#include <iostream>
#include <thread>
namespace seed {
void print_banner(){std::cout<<"\n====================================\n SEED ENGINE — C++23\n Game Mathematics / Procedural Worlds\n====================================\n";}
void boot_loader(){print_banner();std::cout<<"Boot loader: checking runtime";for(int i=0;i<4;++i){std::cout<<'.'<<std::flush;std::this_thread::sleep_for(std::chrono::milliseconds(60));}std::cout<<" OK\n";}
void loading_screen(const std::string& label){std::cout<<label<<" [";for(int i=0;i<20;++i){std::cout<<'='<<std::flush;std::this_thread::sleep_for(std::chrono::milliseconds(10));}std::cout<<"] 100%\n";}
}
