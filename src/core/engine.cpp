#include "seed/engine.hpp"
#include <algorithm>
#include <stdexcept>
namespace seed {
std::uint64_t SplitMix64::next()noexcept{std::uint64_t z=(state_+=0x9E3779B97F4A7C15ULL);z=(z^(z>>30))*0xBF58476D1CE4E5B9ULL;z=(z^(z>>27))*0x94D049BB133111EBULL;return z^(z>>31);}
double SplitMix64::unit_interval()noexcept{return double(next()>>11)/double(std::uint64_t{1}<<53);}
double MathEngine::clamp(double v,double a,double b){if(a>b)throw std::invalid_argument("invalid bounds");return std::clamp(v,a,b);}
double MathEngine::lerp(double a,double b,double t){return a+(b-a)*t;}
double MathEngine::effective_hull(double b,double e,double m){return std::max(0.0,b*(1.0+e*.01)*m);}
double MathEngine::armor_damage(double d,double a,double min){if(d<0||a<0||min<0)throw std::invalid_argument("negative combat value");return std::max(min,d*100.0/(100.0+a));}
GalaxySummary MathEngine::generate_demo_world(std::uint64_t s,size_t n){SplitMix64 r(s);GalaxySummary g{s,{}};g.planets.reserve(n);const char*t[]={"Rocky","Ocean","Desert","Ice","GasGiant"};for(size_t i=0;i<n;++i)g.planets.push_back({"planet-"+std::to_string(i+1),lerp(800,70000,r.unit_interval()),lerp(-180,420,r.unit_interval()),lerp(.1,3.5,r.unit_interval()),t[r.next()%5]});return g;}
}
