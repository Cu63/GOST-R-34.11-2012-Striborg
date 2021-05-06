#include "../header/hash.hpp"
#include "../header/test.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    const int kFileBufferSize = 4096;

    if (argc < 2) {
        std::cout << argv[0] << " <file name\n";
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    int len;
    uint8_t *buffer; 
    GOSTHash hashFunction(512);

    buffer = new uint8_t[kFileBufferSize];
    while ((len = fread(buffer, 1, kFileBufferSize, fp)) > 0)
        hashFunction.hashUpdate(buffer, len);
    hashFunction.hashFinal();
    hashFunction.hashPrint();

    std::cout << std::dec << hashFunction.getHashSize() << std::endl;
    makeTest(hashFunction);
    fclose(fp);
    delete [] buffer;
}
