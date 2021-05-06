#include "../header/test.hpp"
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iostream>
//#include "../header/hash.hpp"

void makeTest(GOSTHash &hash) {
    using std::endl;
    using std::cout;
    unsigned int cr;
    GOSTHash testHash(hash.getHashSize());
    uint8_t *newHash;
    uint8_t *generatedString;
    int strLen;

    strLen = 0;
    cr = 0;
    generatedString = new uint8_t[4096];
    for (int i = 0; i < 1000; i++) {
        bool flag = true;
        int j = 0;
        while (flag) {
            strLen = generateString(generatedString);
            if (generatedString == nullptr)
                break;
            testHash.hashUpdate(generatedString, strLen);
            testHash.hashFinal();
            newHash = testHash.getHash();

            /*
            if (hash.compareHash(newHash, 24) == true) {
                cout << "24 bits equal in iteration number " << j << endl;
                cr += j;
                flag = false;
            } 
            if (hash.compareHash(newHash, 20) == true) {
                cout << "20 bits equal in iteration number " << j << endl;
                flag = false;
            }
            if (hash.compareHash(newHash, 16) == true) {
                cout << "16 bits equal in iteration number " << j << endl;
                flag = false;
            }
            if (hash.compareHash(newHash, 12) == true) {
                cout << "12 bits equal in iteration number " << j << endl;
                flag = false;
            }
            */
            if (hash.compareHash(newHash, 12) == true) {
//                cout << "8 bits equal in iteration number " << j << endl;
                flag = false;
                cr += j;
            }
            j++;
        }
    }
    cout << cr / 1000 << endl;
    delete [] newHash;
    delete [] generatedString;
}

int generateString(uint8_t *s) {
    int len;

    srand(time(0));
    //4096 max value of buffer for hash
    len = rand() % 4096 ;
    for (int i = 0; i < len; ++i)
        s[i] = rand() % 256;
    return len;
}
