#include <filesystem>
#include <large_flock/core/model_manager/tiktoken.hpp>

namespace large_flock {

namespace core {

int Tiktoken::GetNumTokens(const std::string &str) {

    int length = 0;
    std::string word;
    for (char c : str) {
        if (c == ' ') {
            if (!word.empty()) {
                length++;
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        length++;
    }

    return length;
}

} // namespace core

} // namespace large_flock