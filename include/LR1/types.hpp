#ifndef LIBLR1_CONV_TYPES_HPP
#define LIBLR1_CONV_TYPES_HPP

namespace LR1 {
    /**
     * The resource types currently supported with a decoder
     */
    enum class ResourceType: uint8_t {
        Image,
        Audio,
        Text,
        Model,


        /**
         * The catch type for everything without a decoder (yet).
         * It also indicates the number of supported resource types,
         * therefore it has to be placed at the end
         */
        Unsupported
    };
}

#endif //LIBLR1_CONV_TYPES_HPP
