#ifndef LIBLR1_CONV_SRF_HPP
#define LIBLR1_CONV_SRF_HPP

#include <vector>

#include <LR1/decoder.hpp>

namespace LR1 {
    class SrfDecoder : public virtual SimpleBinaryDecoder<ResourceType::Text, std::vector<std::string>> {
        public:

            bool save(const std::filesystem::path &path, const std::vector<std::string> &decoded) override;

        protected:
            std::optional<std::vector<std::string>> decode() override;
    };
}

#endif //LIBLR1_CONV_SRF_HPP
