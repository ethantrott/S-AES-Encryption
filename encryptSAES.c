/*
*  COS430 Assignment 2
*  Ethan Trott
*  2/28/2021
*/

#include <stdio.h>

long int key = 0b1100101100101011;
int plaintext = 0b0110111101101011;

int sbox[] = {0b1001, 0b0100, 0b1010, 0b1011, 0b1101, 0b0001, 0b1000, 0b0101, 0b0110, 0b0010, 0b0000, 0b0011, 0b1100, 0b1110, 0b1111, 0b0111};

// prints integer in binary
void printBinary(int number)
{
    if (number >> 1) {
        printBinary(number >> 1);
    }
    putc((number & 1) ? '1' : '0', stdout);
}

// gets bit at specific position of a nibble
int nthBit(int val, int index){
    return (val >> (3-index)) & 1;
}

// formats output for printing binary values
void outputBinary (char* s, int val){
    printf("%s: ", s);
    printBinary(val);
    printf("\n");
}

// swaps first and second nibbles
int rotNib (int val){
    int nib1 = val >> 4;
    int nib2 = val & 0xF;

    int rotated = (nib2 << 4) | nib1;

    return rotated;
}

// applies sbox to each nibble in a byte
int sub2Nib (int val){
    int nib1 = val >> 4;
    int nib2 = val & 0xF;

    nib1 = sbox[nib1];
    nib2 = sbox[nib2];

    return (nib1 << 4) | nib2;
}

// applies sbox to each nibble in 2 bytes
int sub4Nib (int val){
    int nib1 = val >> 12;
    int nib2 = (val >> 8) & 0xF;
    int nib3 = (val >> 4) & 0xF;
    int nib4 = val & 0xF;

    nib1 = sbox[nib1];
    nib2 = sbox[nib2];
    nib3 = sbox[nib3];
    nib4 = sbox[nib4];

    return (nib1 << 12) | (nib2 << 8) | (nib3 << 4) | nib4;
}

// swaps second and fourth nibble
int shiftRow (int val){
    int nib1 = val >> 12;
    int nib2 = (val >> 8) & 0xF;
    int nib3 = (val >> 4) & 0xF;
    int nib4 = val & 0xF;

    return (nib1 << 12) | (nib4 << 8) | (nib3 << 4) | nib2;
}

// apply matrix multiplication with constant matrix
int mixColumns (int val){
    int result = 0;

    int nib1 = val >> 12;
    int nib2 = (val >> 8) & 0xF;
    int nib3 = (val >> 4) & 0xF;
    int nib4 = val & 0xF;

    int resultBits [16];

    // mix columns math from this source: https://www.nku.edu/~christensen/2015%20JMM%20SAES.pdf
    resultBits[0] = nthBit(nib1, 0) ^ nthBit(nib2, 2);
    resultBits[1] = nthBit(nib1, 1) ^ nthBit(nib2, 0) ^ nthBit(nib2, 3);
    resultBits[2] = nthBit(nib1, 2) ^ nthBit(nib2, 0) ^ nthBit(nib2, 1);
    resultBits[3] = nthBit(nib1, 3) ^ nthBit(nib2, 1);

    resultBits[4] = nthBit(nib1, 2) ^ nthBit(nib2, 0);
    resultBits[5] = nthBit(nib1, 0) ^ nthBit(nib1, 3) ^ nthBit(nib2, 1);
    resultBits[6] = nthBit(nib1, 0) ^ nthBit(nib1, 1) ^ nthBit(nib2, 2);
    resultBits[7] = nthBit(nib1, 1) ^ nthBit(nib2, 3);

    resultBits[8] = nthBit(nib3, 0) ^ nthBit(nib4, 2);
    resultBits[9] = nthBit(nib3, 1) ^ nthBit(nib4, 0) ^ nthBit(nib4, 3);
    resultBits[10] = nthBit(nib3, 2) ^ nthBit(nib4, 0) ^ nthBit(nib4, 1);
    resultBits[11] = nthBit(nib3, 3) ^ nthBit(nib4, 1);

    resultBits[12] = nthBit(nib3, 2) ^ nthBit(nib4, 0);
    resultBits[13] = nthBit(nib3, 0) ^ nthBit(nib3, 3) ^ nthBit(nib4, 1);
    resultBits[14] = nthBit(nib3, 0) ^ nthBit(nib3, 1) ^ nthBit(nib4, 2);
    resultBits[15] = nthBit(nib3, 1) ^ nthBit(nib4, 3);
    
    for (int i=0; i<16; i++) result = result | (resultBits[i]<<(15-i));
    return result;
}


int main(){
    printf("SAES Encryption by Ethan Trott\n\n");
    outputBinary("Key", key);
    outputBinary("Plaintext", plaintext);
    printf("\n");

    // -- Key Generation --
    int w0 = key >> 8;
    int w1 = key & 0xFF;
    int w2 = w0 ^ 0b10000000 ^ sub2Nib(rotNib(w1));
    int w3 = w2 ^ w1;
    int w4 = w2 ^ 0b00110000 ^ sub2Nib(rotNib(w3));
    int w5 = w4 ^ w3;

    outputBinary("W0", w0);
    outputBinary("W1", w1);
    outputBinary("W2", w2);
    outputBinary("W3", w3);
    outputBinary("W4", w4);
    outputBinary("W5", w5);

    int key0 = (w0 << 8) | w1;
    int key1 = (w2 << 8) | w3;
    int key2 = (w4 << 8) | w5;

    outputBinary("Key0", key0);
    outputBinary("Key1", key1);
    outputBinary("Key2", key2);


    // -- Encryption --

    // add round 0 key
    int ciphertext = plaintext ^ key0;

    outputBinary("\nafter round 0", ciphertext);

    // round 1
    ciphertext = sub4Nib(ciphertext);
    ciphertext = shiftRow(ciphertext);
    ciphertext = mixColumns(ciphertext);

    // add round 1 key
    ciphertext = ciphertext ^ key1;

    outputBinary("after round 1", ciphertext);

    // round 2
    ciphertext = sub4Nib(ciphertext);
    ciphertext = shiftRow(ciphertext);

    // add round 2 key
    ciphertext = ciphertext ^ key2;

    outputBinary("after round 2", ciphertext);

    printf("\n");
    outputBinary("Final Ciphertext", ciphertext);

    return 0;
}