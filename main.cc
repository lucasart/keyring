#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

char decode_char(uint8_t b)
{
    assert(b < 64);
    return b < 10 ? b + '0'
        : b < 36 ? b - 10 + 'A'
        : b < 62 ? b - 36 + 'a'
        : b == 62 ? '+'
        : '-';
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
        else if (s == "quit")
            break;
        else
            std::cout << "unrecognized command '" << s << "'\n";
    }
}
