#include <fstream>
#include "crypto.h"

namespace {

uint64_t rotl(uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}

uint64_t xoroshiro128p(uint64_t s[2])
{
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = s0 + s1;

    s1 ^= s0;
    s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
    s[1] = rotl(s1, 36);

    return result;
}

void password_to_key(const std::string& password, uint64_t key[2])
{
    int bit = 0;
    key[0] = key[1] = 0;

    // Spread the password across 128-bit, using 6-bit per char
    for (char c: password) {
        const uint8_t b = encode_char(c);

        if (bit < 64) {
            key[0] |= b << bit;

            if (bit == 60)  // overlapping
                key[1] |= b >> 4;
        } else
            key[1] |= b << (bit - 64);

        bit += 6;

        if (bit >= 128)  // guard against oversized shift (undefined)
            break;
    }

    // Password will often not occupy the full 128 bits (that would take 22 character password).
    // Also, human chosen passwords can have weak entropy. So we need to run a few rounds of 128-bit
    // PRNG, to maximize entropy.
    for (int i = 0; i < 16; i++)
        xoroshiro128p(key);
}

}  // namespace

char decode_char(uint8_t b)
{
    return b < 10 ? b + '0'
        : b < 36 ? b - 10 + 'A'
        : b < 62 ? b - 36 + 'a'
        : b == 62 ? '+'
        : '-';
}

uint8_t encode_char(char c)
{
    return '0' <= c && c <= '9' ? c
        : 'A' <= c && c <= 'Z' ? c - 'A' + 10
        : 'a' <= c && c <= 'z' ? c - 'a' + 36
        : c == '+' ? 62
        : 63;
}

std::string generate_password(int n)
{
    std::string password;
    std::ifstream rng("/dev/urandom", std::ios::binary);

    for (int i = 0; i < n; i++) {
        uint8_t b;
        rng >> b;
        password += decode_char(b >> 2);
    }

    return password;
}

void ChaCha::init(const std::string& password)
{
    // Make a strong 128-bit key from password
    uint64_t key[2];
    password_to_key(password, key);

    // Initialize ChaCha with key
    for (int i = 0; i < 16; i++)
        x[i] = xoroshiro128p(key) >> 32;
}

void ChaCha::cipher(const char *src, char *dst, size_t n)
{
    int j = 0;

    for (size_t i = 0; i < n / 4; i++) {
        if (j == 0)
            twelve_rounds();

        *reinterpret_cast<uint32_t *>(dst) = *reinterpret_cast<const uint32_t *>(src) ^ x[j];

        src += 4, dst += 4;
        j = (j + 1) % 16;
    }

    for (size_t i = 0; i < n % 4; i++) {
        if (j == 0)
            twelve_rounds();

        *dst = *src ^ char(x[j] >> (8 * i));
        src++, dst++;
    }
}

void ChaCha::quarter_round(int a, int b, int c, int d)
{
    x[a] += x[b]; x[d] = rotl(x[d] ^ x[a], 16);
    x[c] += x[d]; x[b] = rotl(x[b] ^ x[c], 12);
    x[a] += x[b]; x[d] = rotl(x[d] ^ x[a], 8);
    x[c] += x[d]; x[b] = rotl(x[b] ^ x[c], 7);
}

void ChaCha::double_round()
{
    quarter_round(0, 4, 8, 12);
    quarter_round(1, 5, 9, 13);
    quarter_round(2, 6, 10, 14);
    quarter_round(3, 7, 11, 15);
    quarter_round(0, 5, 10, 15);
    quarter_round(1, 6, 11, 12);
    quarter_round(2, 7, 8, 13);
    quarter_round(3, 4, 9, 14);
}

void ChaCha::twelve_rounds()
{
    for (int i = 0; i < 6; i++)
        double_round();
}
