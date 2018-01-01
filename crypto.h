#pragma once
#include <cstdint>
#include <string>

// Password characters are remapped to 6-bits, using the following alphabet: 0..9,A..Z,a..z,+,-.
// This maximizes the use of the 128-bit keyspace. To utilise it fully, one 22 character passwords.
char decode_char(uint8_t b);
uint8_t encode_char(char c);

// Obviously, passwords must be random, to be fully impervious to dictionary or probabilistic
// attacks. Humans are very bad at generating random data, so provide some help in generating
// strong passwords.
std::string generate_password(int n);

// ChaCha 12-rounds, using 128-bit keys (ie. up to 22 character passwords)
class ChaCha {
public:
    void init(const std::string& password);
    void cipher(char *buffer, size_t n);

private:
    void quarter_round(int a, int b, int c, int d);
    void double_round();
    void twelve_rounds();

    uint32_t x[16];
};
