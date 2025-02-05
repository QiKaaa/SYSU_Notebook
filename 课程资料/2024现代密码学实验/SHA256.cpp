#include <iostream>
#include <algorithm>
#include <iomanip>
#ifdef _WIN32
#include <fcntl.h>
#endif
#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
using namespace std;
const int N = 64;

const unsigned int K[64] = {
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
unsigned int H[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};
unsigned int lst[8];
unsigned int tmp[8];
unsigned int W[N];
unsigned char s[N];
unsigned char t[32];

#define setBit(x, pos) x |= (1 << (pos))
#define GetByte(x, pos) ((x >> ((pos) << 3)) & 255)
#define XorByte(x, y, pos) x ^= (y << ((pos) << 3))
#define ROTR(x,y) ((x << (32 - y)) | (x >> y))
#define phi0(i) (ROTR(W[i - 15], 7) ^ ROTR(W[i - 15], 18) ^ (W[i - 15] >> 3))
#define phi1(i) (ROTR(W[i - 2], 17) ^ ROTR(W[i - 2], 19) ^ (W[i - 2] >> 10))
#define sigma0 (ROTR(H[0], 2) ^ ROTR(H[0], 13) ^ ROTR(H[0], 22))
#define sigma1 (ROTR(H[4], 6) ^ ROTR(H[4], 11) ^ ROTR(H[4], 25))
#define Ch ((H[4] & H[5]) ^ ((~H[4]) & H[6]))
#define Ma ((H[0] & H[1]) ^ (H[0] & H[2]) ^ (H[1] & H[2]))

void Extend() {
	For(i, 0, 15) {
		W[i] = 0;
		For(j, 0, 3) {
			XorByte(W[i], s[(i << 2) | j], 3 ^ j);
		}
	}
	For(i, 16, 63) {
		W[i] = W[i - 16] + W[i - 7] + phi0(i) + phi1(i);
	}
}
void Mix() {
	For(i, 0, 7) lst[i] = H[i];
	For(i, 0, 63) {
		unsigned int T1 = H[7] + sigma1 + Ch + K[i] + W[i];
		unsigned int T2 = sigma0 + Ma;
		Rof(j, 7, 1) H[j] = H[j - 1];
		H[0] = T1 + T2;
		H[4] += T1;
	}
	For(i, 0, 7) H[i] = H[i] + lst[i];
	For(i, 0, 63) s[i] = 0;
}
void SHA256(){
	Extend();
	Mix();
}
int main() {
#ifdef _WIN32
	_setmode(_fileno(stdin), O_BINARY);
	_setmode(_fileno(stdout), O_BINARY);
#endif
	FILE* stream1 = stdin;
	FILE* stream2 = stdout;
#ifndef ONLINE_JUDGE
	stream1 = fopen("input4.bin","rb");
	stream2 = fopen("output4.bin","wb");
#endif 
	int cnt = 0;
	unsigned long long len = 0;
	int m512 = 0;
	while (!feof(stream1)) {
		size_t ret_code = fread(s, sizeof(unsigned char), N, stream1);
		if(ret_code == N){
			len += 512;
			SHA256();
		} else{
			cnt = ret_code;
			m512 = ret_code * 8;
			len += ret_code * 8;
			break;
		}
	}

	For(i, cnt, 63) {
		s[i] = (unsigned char)0;
	}
	setBit(s[cnt], 7);
	int num0 = (448 - m512 - 1 + 512) & 511;
	if (num0 == 0) num0 = 512;
	int pos = 6;
	//0
	For(i, 1, num0) {
		--pos;
		if (pos == -1) {
			if (cnt == 63) {
				SHA256();
				cnt = 0;
			}
			else {
				++cnt;
			}
			pos = 7;
		}
	}
	Rof(i, 63, 0) {
		if ((len >> i) & 1ull) {
			setBit(s[cnt], pos);
		}
		--pos;
		if (pos == -1) {
			++cnt;
			pos = 7;
		}
	}
	SHA256();
	For(i, 0, 7) Rof(j, 3, 0){
		t[(i<<2) | (3 ^ j)] = GetByte(H[i],j);
	}
	fwrite(t, sizeof(unsigned char), sizeof(t), stream2);
	fclose(stream1);
	fclose(stream2);
}