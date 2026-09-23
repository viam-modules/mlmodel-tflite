include(default)

# viam-cpp-sdk 0.20.1 leaves xtensor uncapped; 0.27 needs C++20 and this build is C++17.
[replace_requires]
xtensor/*: xtensor/0.26.0
