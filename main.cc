#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

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
{
    std::string s, fileName, password;
    std::cin >> fileName >> password;

    for (auto& key: keyring)
        s += key.first + "\t" + key.second + "\n";

    ChaCha c;
    c.init(password);
    c.cipher(&s[0], s.length());

    std::ofstream os(fileName, std::ios::binary);
    os.write(s.c_str(), s.length());
}

void load(std::map<std::string, std::string>& keyring)
{
    std::string fileName, password;
    std::cin >> fileName >> password;

    std::ifstream is(fileName, std::ios::binary);
    is.seekg(0, is.end);
    const auto n = is.tellg();
    is.seekg(0, is.beg);

    std::string s;
    s.resize(n);
    is.read(&s[0], n);

    ChaCha c;
    c.init(password);
    c.cipher(&s[0], s.length());

    std::istringstream ss(s);
    std::string line;
    std::size_t pos;

    while (std::getline(ss, line) && (pos = line.find('\t')) != std::string::npos)
        keyring[line.substr(0, pos)] = line.substr(pos + 1, line.length() - pos - 1);
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
        else if (s == "load")
            load(keyring);
        else if (s == "quit")
            break;
        else
            std::cout << "unrecognized command '" << s << "'\n";
    }
}
