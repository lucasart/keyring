## Keyring

Keyring is a password manager. It allows you to load/save all your logins and
passwords from/to encrypted file(s).

Uses state of the art [ChaCha](https://en.wikipedia.org/wiki/Salsa20#ChaCha_variant)
by Daniel Bernstein, allowing practically unlimited encryption strength (up to 256-bits).

Of course, encryption strength can only be as good as [password strength](https://en.wikipedia.org/wiki/Password_strength).
So don't be a dumb ass: choose a strong pasword, don't write it anywhere, and don't tell anyone (not even your mum).

### How to compile ?

Linux only for now. Should compile the same way on other POSIX systems (including MacOSX), but I haven't tested.
- GCC: `g++ -o ./keyring -std=c++11 -DNDEBUG -O3 -flto -s *.cc`
- Clang: `clang++ -o ./keyring -std=c++11 -DNDEBUG -O3 -flto -s *.cc`

### How to use ?

Here is the list of commands:
- `load file password`: load keyring from `file`, using `password` for deciphering. For passwords,
  only the following characters should be used: `0..9,A..Z,a..z,+,-`.
- `save file password`: saves keyring to `file`, using `password` for ciphering.
- `view`: display the key ring.
- `add key value`: adds a `(key,value)` pair to the keyring. keys must be
  unique, so you will get an error if an entry of the form `(key,...)` already
  exists in the keyring (in which case you want to use the `modify` command).
- `modify key value`: modifies a `(key,value)` pair in the keyring. Of course,
  an entry `(key,...)` must exist first, or you will get an error (in which case
  you want to use the `add` command).
- `remove key`: removes the `(key,...)` entry. Of course, such an entry must
  exist, or you will get an error.
- `generate n`: generates a random password with n characters.
- `quit`: exit the program.

### Example sessions

Let's create 3 entries, and save them to a keyring file:
```
$ ./keyring
add gmail email=john.doe@gmail.com,pwd=v5lYT1UzDL
add hsbc pwd1=ezVAyyOO1U,pwd2=QMezpaXrG5,phone_pin=654827
add yahoo email=johnDoe@yahoo.com,pwd=dTVl4Q0l+g
view
> gmail	email=john.doe@gmail.com,pwd=v5lYT1UzDL
> hsbc	pwd1=ezVAyyOO1U,pwd2=QMezpaXrG5,phone_pin=654827
> yahoo	email=johnDoe@yahoo.com,pwd=dTVl4Q0l+g
generate 10
> ajKVew5NFI
save ./secret ajKVew5NFI
quit
```
Now let's decrypt this keyring file, make some modifications, and save changes:
```
$ ./keyring
load ./secret ajKVew5NFI
view
> gmail	email=john.doe@gmail.com,pwd=v5lYT1UzDL
> hsbc	pwd1=ezVAyyOO1U,pwd2=QMezpaXrG5,phone_pin=654827
> yahoo	email=johnDoe@yahoo.com,pwd=dTVl4Q0l+g
modify yahoo email=johnDoe@yahoo.com,pwd=sk3x9(8*             
remove hsbc
view
> gmail	email=john.doe@gmail.com,pwd=v5lYT1UzDL
> yahoo	email=johnDoe@yahoo.com,pwd=sk3x9(8*
save ./secret ajKVew5NFI
quit
```
Let's re-open the file and verify:
```
$ ./keyring 
load ./secret ajKVew5NFI
view
gmail	email=john.doe@gmail.com,pwd=v5lYT1UzDL
yahoo	email=johnDoe@yahoo.com,pwd=sk3x9(8*
quit
```
