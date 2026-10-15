#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace seed {
class SplitMix64 { public: explicit SplitMix64(std::uint64_t s):state_(s){} std::uint64_t next() noexcept; double unit_interval() noexcept; private: std::uint64_t state_; };
struct Planet { std::string id; double radius_km{},temperature_c{},gravity_g{}; std::string type; };
struct GalaxySummary { std::uint64_t seed{}; std::vector<Planet> planets; };
class MathEngine { public: static double clamp(double,double,double); static double lerp(double,double,double); static double effective_hull(double,double,double); static double armor_damage(double,double,double=1.0); static GalaxySummary generate_demo_world(std::uint64_t,std::size_t); };
}
