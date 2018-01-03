#include <algorithm>
#include <fstream>
#include "crypto.h"

namespace {

uint64_t rotl(uint64_t x, int k)
{
    return (x << k) | (x >> (64 - k));
}

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

}  // namespace

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
    std::fill_n(&x[0], 16, 0);
    int bit = 0;

    for (char c: password) {
        const uint32_t b = encode_char(c);

        x[bit / 32] |= b << (bit % 32);

        if ((bit + 6) % 32 < bit % 32)
            x[(bit + 6) / 32] |= b >> (32 - (bit % 32));

        bit += 6;
    }
}

void ChaCha::cipher(char *buffer, size_t n)
{
    int j = 0;

    for (size_t i = 0; i < n / 4; i++) {
        if (j == 0)
            twenty_rounds();

        *reinterpret_cast<uint32_t *>(buffer) ^= x[j];

        buffer += 4;
        j = (j + 1) % 16;
    }

    for (size_t i = 0; i < n % 4; i++) {
        if (j == 0)
            twenty_rounds();

        *buffer++ ^= x[j] >> (8 * i);
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

void ChaCha::twenty_rounds()
{
    for (int i = 0; i < 10; i++)
        double_round();
}
