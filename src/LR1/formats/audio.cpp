#include <iostream>
#include <fstream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include <LR1/formats/audio.hpp>

using namespace std;
using namespace std::filesystem;

namespace LR1 {
    AudioDecoder::~AudioDecoder() {
        if (ctx) {
            avformat_free_context(ctx);
            ctx = nullptr;
        }
    }

    optional<PcmData> AudioDecoder::decode(const path &path) {
        ctx = avformat_alloc_context();
        if (avformat_open_input(&ctx, path.string().c_str(), nullptr, nullptr) < 0) {
            cerr << "Couldn't open file: " << path << endl;
            return nullopt;
        }
        const optional<PcmData> pcm = decode();
        avformat_close_input(&ctx);
        avformat_free_context(ctx);
        ctx = nullptr;

        return pcm;
    }

    std::optional<PcmData> AudioDecoder::decode(const std::vector<uint8_t> &data) {
        ctx = avformat_alloc_context();
        const MemoryIO io(data);
        if (!io.ctx) {
            cerr << "Couldn't allocate memory for audio decoder" << endl;
            return nullopt;
        }

        ctx->pb = io.ctx;
        if (avformat_open_input(&ctx, nullptr, nullptr, nullptr) < 0) {
            cerr << "Couldn't read from memory" << endl;
            return nullopt;
        }
        const optional<PcmData> pcm = decode();
        avformat_close_input(&ctx);
        avformat_free_context(ctx);
        ctx = nullptr;

        return pcm;
    }

    std::optional<PcmData> AudioDecoder::decode() {
        //Find streams
        if (avformat_find_stream_info(ctx, nullptr) < 0) {
            cerr << "Couldn't find stream information" << endl;
            return nullopt;
        }

        int streamIndex = -1;
        for (int i = 0; i < ctx->nb_streams; i++) {
            if (ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                streamIndex = i;
                break;
            }
        }

        if (streamIndex == -1) {
            cerr << "No audio stream found" << endl;
            return nullopt;
        }

        // Get Codec parameters and codec context
        const AVCodecParameters* codecParams = ctx->streams[streamIndex]->codecpar;
        const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
        AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecCtx, codecParams);

        //Open Codec
        if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
            cerr << "Couldn't open codec" << endl;
            return nullopt;
        }

        cout << "Opened codec " << codec->long_name << endl;
        cout << "Sample Rate: " << codecCtx->sample_rate << endl;
        cout << "Channels: " << codecCtx->ch_layout.nb_channels << endl;
        cout << "Bits: " << (av_get_bytes_per_sample(codecCtx->sample_fmt) * 8) << endl;

        const int sampleRate = codecCtx->sample_rate;
        const int channels = codecCtx->ch_layout.nb_channels;
        vector<int16_t> data;

        //Alloc frame and packet
        AVFrame* frame = av_frame_alloc();
        AVPacket* packet = av_packet_alloc();

        //Read frames from the audio stream
        while (av_read_frame(ctx, packet) >= 0) {
            if (packet->stream_index == streamIndex) {
                int ret = avcodec_send_packet(codecCtx, packet);
                if (ret < 0) {
                    cerr << "Couldn't send packet for decoding" << endl;
                    break;
                }

                while (true) {
                    ret = avcodec_receive_frame(codecCtx, frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                        break;
                    }
                    if (ret < 0) {
                        cerr << "Error during decoding" << endl;
                        break;
                    }

                    const size_t currentBufSize = data.size();
                    const int bytesPerSample = av_get_bytes_per_sample(codecCtx->sample_fmt);
                    const size_t totalSamples = frame->nb_samples * channels;
                    data.resize(currentBufSize + totalSamples);
                    memcpy(data.data() + currentBufSize, frame->data[0], totalSamples * bytesPerSample);
                }
            }

            av_packet_unref(packet);
        }

        av_frame_free(&frame);
        av_packet_free(&packet);
        avcodec_free_context(&codecCtx);

        return {{sampleRate, channels, data}};
    }

    bool AudioDecoder::save(const path& path, const PcmData& decoded) {
        ofstream file(path, ios::binary);
        if (!file) return false;

        //Header
        constexpr uint32_t headerSize = 44;
        const uint32_t dataSize = decoded.data.size() * sizeof(int16_t);
        const uint32_t totalSize = headerSize + dataSize - 8;

        file << "RIFF";
        file.write(reinterpret_cast<const char*>(&totalSize), 4);
        file << "WAVE";

        //Chunk
        constexpr uint32_t chunkSize = 16;
        constexpr uint16_t audioFormat = 1;
        const uint16_t numChannels = decoded.channels;
        const uint32_t sampleRate = decoded.sampleRate;
        constexpr uint16_t bitsPerSample = 16;
        const uint16_t bytesPerBlock = numChannels * bitsPerSample / 8;
        const uint32_t bytesPerSecond = sampleRate;

        file << "fmt ";
        file.write(reinterpret_cast<const char*>(&chunkSize), 4);
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        file.write(reinterpret_cast<const char*>(&numChannels), 2);
        file.write(reinterpret_cast<const char*>(&sampleRate), 4);
        file.write(reinterpret_cast<const char*>(&bytesPerSecond), 4);
        file.write(reinterpret_cast<const char*>(&bytesPerBlock), 2);
        file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

        //Data
        file << "data";
        file.write(reinterpret_cast<const char*>(&dataSize), 4);
        file.write(reinterpret_cast<const char*>(decoded.data.data()), dataSize);

        file.close();

        return true;
    }

    MemoryIO::MemoryIO(const std::vector<uint8_t> &data) : ctx(nullptr), buf(nullptr), data(data), offset(0) {
        buf = static_cast<uint8_t*>(av_malloc(bufSize));
        ctx = avio_alloc_context(buf, bufSize, 0, this, &MemoryIO::readBuf, nullptr, nullptr);
    }

    MemoryIO::~MemoryIO() {
        if (ctx) {
            av_freep(&ctx->buffer);
            avio_context_free(&ctx);
        }
    }

    int MemoryIO::readBuf(void* userdata, uint8_t* buf, const int len) {
        auto* io = static_cast<MemoryIO*>(userdata);
        const size_t bytesToCopy = min(static_cast<size_t>(len), io->data.size() - io->offset);
        if (!bytesToCopy) return AVERROR_EOF;

        memcpy(buf, io->data.data() + io->offset, bytesToCopy);
        io->offset += bytesToCopy;

        return static_cast<int>(bytesToCopy);
    }
}
