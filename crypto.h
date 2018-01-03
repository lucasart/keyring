#pragma once
#include <cstdint>
#include <string>

// Obviously, passwords must be random, to be fully impervious to dictionary or probabilistic
// attacks. Humans are very bad at generating random data, so provide some help in generating
// strong passwords.
std::string generate_password(int n);

// ChaCha 12-rounds, using 128-bit keys (ie. up to 22 character passwords)
class ChaCha {
public:
    ChaCha(const std::string& password, uint64_t nonce[4]);
    void cipher(char *buffer, size_t n);

private:
    void quarter_round(int a, int b, int c, int d);
    void double_round();
    void twenty_rounds();

    union {
        uint32_t x[16];
        uint64_t qw[8];
    };
};
