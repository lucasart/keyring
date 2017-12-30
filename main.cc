#include <fstream>
#include <iostream>
#include <map>
#include <string>

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

void generate()
{
    int n;
    std::cin >> n;
    std::ifstream rng("/dev/urandom", std::ios::binary);

    for (int i = 0; i < n; i++) {
        uint8_t b;
        rng >> b;
        std::cout << decode_char(b >> 2);
    }

    std::cout << '\n';
}

void view(std::map<std::string, std::string>& keyring)
{
    for (auto& key: keyring)
        std::cout << key.first << '\t' << key.second << '\n';
}

void add(std::map<std::string, std::string>& keyring)
{
    std::string key, value;
    std::cin >> key >> value;

    if (keyring.find(key) != keyring.end())
        std::cout << "error: '" << key << "' already exists\n";
    else
        keyring[key] = value;
}

void modify(std::map<std::string, std::string>& keyring)
{
    std::string key, value;
    std::cin >> key >> value;

    if (keyring.find(key) == keyring.end())
        std::cout << "error: '" << key << "' doesn't exist\n";
    else
        keyring[key] = value;
}

void remove(std::map<std::string, std::string>& keyring)
{
    std::string key;
    std::cin >> key;

    if (keyring.find(key) == keyring.end())
        std::cout << "error: '" << key << "' doesn't exist\n";
    else
        keyring.erase(key);
}

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

void password_digest(std::string& password, uint64_t digest[2])
{
    int bit = 0;
    digest[0] = digest[1] = 0;

    // Spread the password across 128-bit, using 6-bit per char
    for (char c: password) {
        const uint8_t b = encode_char(c);

        if (bit < 64) {
            digest[0] |= b << bit;

            if (bit == 60)  // overlapping
                digest[1] |= b >> 4;
        } else
            digest[1] |= b << (bit - 64);

        bit += 6;

        if (bit >= 128)  // guard against oversized shift (undefined)
            break;
    }

    // Password will often not occupy the full 128 bits (that would take 22 character password).
    // Also, human chosen passwords can have weak entropy. So we need to run a few rounds of 128-bit
    // PRNG, to maximize entropy.
    for (int i = 0; i < 16; i++)
        xoroshiro128p(digest);
}

void cipher(const char *src, char *dst, int size, uint64_t digest[2])
{
    uint64_t key;

    for (int i = 0; i < size; i++) {
        if (i % 8 == 0)
            key = xoroshiro128p(digest);

        dst[i] = src[i] ^ (key >> (8 * (i % 8)));
    }
}

void save(std::map<std::string, std::string>& keyring)
{
    std::string s, fileName, password;
    std::cin >> fileName >> password;

    for (auto& key: keyring)
        s += key.first + "\t" + key.second + "\t";

    uint64_t digest[2];
    password_digest(password, digest);

    char *buffer = new char[s.length()];

    cipher(s.c_str(), buffer, s.length(), digest);

    std::ofstream os(fileName, std::ios::binary);
    os.write(buffer, s.length());

    delete[] buffer;
}

int main()
{
    std::string s;
    std::map<std::string, std::string> keyring;

    while (std::cin >> s) {
        if (s == "generate")
            generate();
        else if (s == "view")
            view(keyring);
        else if (s == "add")
            add(keyring);
        else if (s == "modify")
            modify(keyring);
        else if (s == "remove")
            remove(keyring);
        else if (s == "save")
            save(keyring);
        else if (s == "quit")
            break;
        else
            std::cout << "unrecognized command '" << s << "'\n";
    }
}
