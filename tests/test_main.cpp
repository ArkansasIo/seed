#include "seed/engine.hpp"
#include "seed/crypto.hpp"
#include "seed/protocol.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#define CHECK(x) do{if(!(x))throw std::runtime_error("check failed: " #x);}while(0)
int main(){try{CHECK(seed::MathEngine::clamp(5,0,10)==5);CHECK(seed::MathEngine::clamp(-1,0,10)==0);CHECK(std::abs(seed::MathEngine::lerp(10,20,.5)-15)<1e-9);CHECK(seed::MathEngine::effective_hull(1000,10,1.5)==1650);auto a=seed::MathEngine::generate_demo_world(42,8),b=seed::MathEngine::generate_demo_world(42,8);CHECK(a.planets.size()==8);CHECK(a.planets[0].radius_km==b.planets[0].radius_km);CHECK(seed::split_tabs("LOGIN\talice\tsecret").size()==3);auto s=seed::random_salt_hex();auto h=seed::password_hash("example-only",s,10000);CHECK(seed::constant_time_equal(h,seed::password_hash("example-only",s,10000)));CHECK(!seed::constant_time_equal(h,seed::password_hash("wrong",s,10000)));std::cout<<"All tests passed\n";return 0;}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
