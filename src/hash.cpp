#include "../header/hash.hpp"
#include <iostream>
#include <cstring>

GOSTHash::GOSTHash(unsigned int hashSize) {
//    std::cout << "GOSTHas\n";
    
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        buffer[i] = hash[i] = h[i] = N[i] = Sigma[i] = v0[i] = v512[i] = 0;
    }
    bufSize = 0;
    this->hashSize = hashSize;
    if (hashSize == 256)
        memset(&h[0], 0x01, BLOCK_SIZE);
    else
        memset(&h[0], 0x00, BLOCK_SIZE);
    v512[1] = 0x02;
}

void GOSTHash::hashX(const uint8_t *a, const uint8_t *b, uint8_t *c) {
    for (int i = 0; i < BLOCK_SIZE; ++i)
        c[i] = a[i] ^ b[i];
}

void GOSTHash::hash512(const uint8_t *a, const uint8_t *b, uint8_t *c) {
    unsigned int intern;

    intern = 0;
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        intern = a[i] + b[i] + (intern >> 8);
        c[i] = intern & 0xff;
    }
}

void GOSTHash::hashS(uint8_t *state) {
 //   std::cout << "GOSTHashS\n";
    vect intern;

    for (int i = 63; i >= 0; --i)
        intern[i] = Pi[state[i]];
    memcpy(state, intern, BLOCK_SIZE);
}

void GOSTHash::hashP(uint8_t *state) {
  //  std::cout << "GOSTHashP\n";
    vect intern;
    for (int i = 63; i >= 0; --i)
        intern[i] = state[Tau[i]];
    memcpy(state, intern, BLOCK_SIZE);
}

void GOSTHash::hashL(uint8_t *state) {
//    std::cout << "GOSTHashL\n";
    uint64_t *internal_in = (uint64_t *) state;
    uint64_t internal_out[8] = {0};

    memset(internal_out, 0x00, BLOCK_SIZE);
    for (int i = 7; i >= 0; --i) {
        for (int j = 63; j >= 0; --j)
            if ((internal_in[i] >> j) & 1)
                internal_out[i] ^= A[63 - j];
    }
    memcpy(state, internal_out, 64);
}

void GOSTHash::hashGetKey(uint8_t *k, int i) {
 //   std::cout << "GOSTHashGetKey\n";
    hashX(k, C[i], k);
    hashS(k);
    hashP(k);
    hashL(k);
}

void GOSTHash::hashE(uint8_t *k, const uint8_t* m, uint8_t *state) {
   // std::cout << "GOSTHashE\n";
    memcpy(k, k, BLOCK_SIZE);
    hashX(m, k, state);
    for (int i = 0; i < 12; ++i) {
        hashS(state);
        hashP(state);
        hashL(state);
        hashGetKey(k, i);
        hashX(state, k, state);
    }
}

void GOSTHash::hashG(uint8_t *h, uint8_t *N, const uint8_t *m) {
 //   std::cout << "GOSTHashG\n";
    vect K;
    vect intern;
    hashX(N, h, K);

    hashS(K);
    hashP(K);
    hashL(K);

    hashE(K, m, intern);

    hashX(intern, h, intern);
    hashX(intern, m, h);
}

void GOSTHash::hashPadding() {
   // std::cout << "GOSTHashPadding\n";
    vect intern;
    if (bufSize < BLOCK_SIZE) {
        memset(intern, 0x00, BLOCK_SIZE);
        memcpy(intern, buffer, bufSize); 
        intern[bufSize] = 0x01;
        memcpy(buffer, intern, BLOCK_SIZE);
    }
}

void GOSTHash::hashStage2(const uint8_t *data) {
  //  std::cout << "GOSTHashStage2\n";
    hashG(h, N, data);
    hash512(N, v512, N);
    hash512(Sigma, data, Sigma);
}

void GOSTHash::hashStage3() {
 //   std::cout << "GOSTHashStage3\n";
    vect intern;
    memset(intern, 0x00, BLOCK_SIZE);

    intern[1] = ((bufSize * 8) >> 8) & 0xff;
    intern[0] = (bufSize * 8) & 0xff;

    hashPadding();
    hashG(h, N, buffer);

    hash512(N, intern, N);
    hash512(Sigma, buffer, Sigma);

    hashG(h, v0, N);
    hashG(h, v0, Sigma);

    memcpy(hash, h, BLOCK_SIZE);
}

void GOSTHash::hashUpdate(const uint8_t *data, size_t len) {
 //   std::cout << "GOSTHahUpdate\n";
    while ((len > 63) && bufSize == 0) {
        hashStage2(data);
        data += 64;
        len -= 64;
    }
    size_t check_size;
    while (len) {
        check_size = 64 - bufSize;
        if (check_size > len)
            check_size = len;

        memcpy(&buffer[bufSize], data, check_size);
        bufSize += check_size;
        len -= check_size;
        data += check_size;
        if (bufSize == 64) {
            hashStage2(buffer);
            bufSize = 0;
        }
    }
}

void GOSTHash::hashFinal() {
  //  std::cout << "GOSTHashFinal\n";
    hashStage3();
    bufSize = 0;
}

void GOSTHash::hashPrint() {
    size_t i;
    if (hashSize == 256)
        i = 32;
    else
        i = 0;
    for (; i < BLOCK_SIZE; i++)
        std::cout << std::hex << (int)hash[i];
    std::cout << std::endl;
}

uint8_t *GOSTHash::getHash() {
    uint8_t *tmp;

    if (256 == hashSize) {
        tmp = new uint8_t[32];
        for (int i = 0; i < 32; i++)
            tmp[i] = hash[i + 32];
    } else {
        tmp = new uint8_t[64];
        for (int i = 0; i < 64; i++)
            tmp[i] = hash[i];
    }
    return tmp;
}

bool GOSTHash::compareHash(const uint8_t *h, int n) {
    int i;

    for (i = 0; i < n; i++) {
        if (((h[i / 8]) & (1 << (i % 8))) != ((hash[i / 8]) & (1 << (i % 8))))
            break;
    }
    if (i == n)
        return true;
    else
        return false;
}
