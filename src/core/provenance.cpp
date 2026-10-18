#include "core/provenance.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <array>
#include <cstdint>

namespace etwin::core {
namespace {

class Sha256 {
public:
    Sha256() { reset(); }

    void reset() {
        digest_[0] = 0x6a09e667u;
        digest_[1] = 0xbb67ae85u;
        digest_[2] = 0x3c6ef372u;
        digest_[3] = 0xa54ff53au;
        digest_[4] = 0x510e527fu;
        digest_[5] = 0x9b05688cu;
        digest_[6] = 0x1f83d9abu;
        digest_[7] = 0x5be0cd19u;
        buffer_size_ = 0;
        total_size_ = 0;
    }

    void update(const unsigned char* data, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            buffer_[buffer_size_++] = data[i];
            if (buffer_size_ == 64) {
                process_block(buffer_.data());
                total_size_ += 512;
                buffer_size_ = 0;
            }
        }
    }

    std::string hex() {
        uint64_t total_bits = total_size_ + buffer_size_ * 8;

        unsigned char pad = 0x80;
        update(&pad, 1);
        while (buffer_size_ != 56) {
            unsigned char zero = 0x00;
            update(&zero, 1);
        }

        unsigned char length_bytes[8];
        for (int i = 0; i < 8; ++i) {
            length_bytes[i] = static_cast<unsigned char>((total_bits >> (56 - i * 8)) & 0xff);
        }
        update(length_bytes, 8);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (uint32_t d : digest_) {
            for (int i = 0; i < 4; ++i) {
                oss << std::setw(2) << static_cast<int>((d >> (24 - i * 8)) & 0xff);
            }
        }
        return oss.str();
    }

private:
    static constexpr std::array<uint32_t, 64> K = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
        0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
        0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
        0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
        0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
        0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };

    static constexpr uint32_t rotr(uint32_t x, unsigned int n) {
        return (x >> n) | (x << (32 - n));
    }

    void process_block(const unsigned char* p) {
        std::array<uint32_t, 64> w{};
        for (int i = 0; i < 16; ++i) {
            w[static_cast<size_t>(i)] = (static_cast<uint32_t>(p[i * 4]) << 24) |
                (static_cast<uint32_t>(p[i * 4 + 1]) << 16) |
                (static_cast<uint32_t>(p[i * 4 + 2]) << 8) |
                static_cast<uint32_t>(p[i * 4 + 3]);
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[static_cast<size_t>(i - 15)], 7) ^
                rotr(w[static_cast<size_t>(i - 15)], 18) ^
                (w[static_cast<size_t>(i - 15)] >> 3);
            uint32_t s1 = rotr(w[static_cast<size_t>(i - 2)], 17) ^
                rotr(w[static_cast<size_t>(i - 2)], 19) ^
                (w[static_cast<size_t>(i - 2)] >> 10);
            w[static_cast<size_t>(i)] = w[static_cast<size_t>(i - 16)] + s0 +
                w[static_cast<size_t>(i - 7)] + s1;
        }

        uint32_t a = digest_[0], b = digest_[1], c = digest_[2], d = digest_[3];
        uint32_t e = digest_[4], f = digest_[5], g = digest_[6], h = digest_[7];

        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t temp1 = h + S1 + ch + K[static_cast<size_t>(i)] + w[static_cast<size_t>(i)];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            h = g; g = f; f = e; e = d + temp1;
            d = c; c = b; b = a; a = temp1 + temp2;
        }

        digest_[0] += a; digest_[1] += b; digest_[2] += c; digest_[3] += d;
        digest_[4] += e; digest_[5] += f; digest_[6] += g; digest_[7] += h;
    }

    std::array<uint32_t, 8> digest_{};
    std::array<unsigned char, 64> buffer_{};
    size_t buffer_size_ = 0;
    uint64_t total_size_ = 0;
};

std::string sha256_string(const unsigned char* data, size_t size) {
    Sha256 hasher;
    hasher.update(data, size);
    return hasher.hex();
}

} // namespace

std::string sha256(const std::string& data) {
    return sha256_string(reinterpret_cast<const unsigned char*>(data.data()), data.size());
}

std::string sha256_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open file for hashing: " + path);

    Sha256 hasher;
    char buf[8192];
    while (f.read(buf, sizeof(buf)) || f.gcount() > 0) {
        hasher.update(reinterpret_cast<const unsigned char*>(buf),
                      static_cast<size_t>(f.gcount()));
    }
    return hasher.hex();
}

Hash hash_combine(const Hash& a, const Hash& b) {
    return sha256(a + "|" + b);
}

Provenance::Provenance(DataSource source) : source_(std::move(source)) {}

void Provenance::add_transform(const TransformRecord& transform) {
    transforms_.push_back(transform);
}

Hash Provenance::compute_hash() const {
    Hash h = source_.content_hash.value_or(sha256(source_.dataset_id + source_.version));
    for (const auto& t : transforms_) {
        h = hash_combine(h, t.output_hash);
    }
    return h;
}

std::string Provenance::to_json() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"dataset_id\":\"" << source_.dataset_id << "\",";
    oss << "\"version\":\"" << source_.version << "\",";
    oss << "\"source_url\":\"" << (source_.source_url) << "\",";
    oss << "\"license\":\"" << (source_.license) << "\",";
    if (source_.retrieval_time) {
        oss << "\"retrieval_time\":\"" << source_.retrieval_time->to_iso8601() << "\",";
    }
    if (source_.content_hash) {
        oss << "\"content_hash\":\"" << *source_.content_hash << "\",";
    }
    oss << "\"hash\":\"" << compute_hash() << "\",";
    oss << "\"transforms\":[";
    for (size_t i = 0; i < transforms_.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& t = transforms_[i];
        oss << "{\"algorithm\":\"" << t.algorithm << "\",\"version\":\"" << t.version
            << "\",\"input_hash\":\"" << t.input_hash << "\",\"output_hash\":\"" << t.output_hash << "\"}";
    }
    oss << "]}";
    return oss.str();
}

Provenance Provenance::from_json(const std::string& json) {
    (void)json;
    return Provenance(DataSource{});
}

bool Provenance::is_reproducible() const {
    if (!source_.content_hash.has_value()) return false;
    if (!source_.retrieval_time.has_value()) return false;
    if (quality_.rejected || quality_.suspicious) return false;
    return true;
}

bool Provenance::has_known_issues() const {
    return quality_.gap_filled || quality_.interpolated ||
           quality_.extrapolated || quality_.suspicious;
}

} // namespace etwin::core
