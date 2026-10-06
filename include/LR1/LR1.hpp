#ifndef LIBLR1_CONV_LR1_HPP
#define LIBLR1_CONV_LR1_HPP

#include <memory>

#include <LR1/decoder.hpp>

namespace LR1 {
    std::unique_ptr<Converter> getConverter(ResourceType type);
}

#endif //LIBLR1_CONV_LR1_HPP
