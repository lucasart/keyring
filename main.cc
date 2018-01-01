#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "crypto.h"

void generate()
{
    int n;
    std::cin >> n;
    std::cout << generate_password(n) << '\n';
}

void view(const std::map<std::string, std::string>& keyring)
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

void save(const std::map<std::string, std::string>& keyring)
// FIXME: only works on Little Endian machines
{
    std::string s, fileName, password;
    std::cin >> fileName >> password;

    for (auto& key: keyring)
        s += key.first + "\t" + key.second + "\t";

    ChaCha c;
    c.init(password);
    c.cipher(&s[0], s.length());

    std::ofstream os(fileName, std::ios::binary);
    os.write(s.c_str(), s.length());
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
