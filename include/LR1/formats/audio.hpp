#ifndef LIBLR1_CONV_AUDIO_HPP
#define LIBLR1_CONV_AUDIO_HPP

#include <vector>

#include <LR1/decoder.hpp>

struct AVFormatContext;
struct AVIOContext;

namespace LR1 {
    struct PcmData {
        int sampleRate;
        int channels;
        std::vector<int16_t> data;
    };

    class AudioDecoder : public virtual Decoder<ResourceType::Audio, PcmData> {
        public:
            AudioDecoder() : ctx(nullptr) {}
            ~AudioDecoder() override;

            std::optional<PcmData> decode(const std::filesystem::path &path) override;
            std::optional<PcmData> decode(const std::vector<uint8_t> &data) override;

            bool save(const std::filesystem::path &path, const PcmData &decoded) override;

        private:
            AVFormatContext* ctx;

            std::optional<PcmData> decode();
    };

    class MemoryIO {
        public:
            MemoryIO(const std::vector<uint8_t>& data);
            ~MemoryIO();

            AVIOContext* ctx;

            static int readBuf(void* userdata, uint8_t* buf, int len);

        private:
            static constexpr size_t bufSize = 4096;

            uint8_t* buf;
            const std::vector<uint8_t>& data;
            size_t offset;
    };
}

#endif //LIBLR1_CONV_AUDIO_HPP
