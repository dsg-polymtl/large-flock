#ifndef TIKTOKEN_HPP
#define TIKTOKEN_HPP
#include "large_flock/common.hpp"

namespace large_flock {

namespace core {

class Tiktoken {
public:
    static int GetNumTokens(const std::string &str);
};

} // namespace core

} // namespace large_flock

#endif // TIKTOKEN_HPP
