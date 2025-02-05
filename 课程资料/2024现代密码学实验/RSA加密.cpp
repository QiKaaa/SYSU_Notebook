#include <iostream>
#include <cstring>
#include <bitset>
#include <algorithm>
#include <iomanip>
//#include <io.h>
#include <immintrin.h>
#ifdef _WIN32
#include <fcntl.h>
#endif
#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
using namespace std;
const int N = 256;
const int M = 40;
unsigned char ig1[16], ig2[256];
unsigned char n[N];
unsigned char e[N];
unsigned char len;
unsigned char s[N];
unsigned char DB[N];
unsigned char seed[M] = {
	0x5F, 0x7F, 0x00, 0x3F, 0x9F, 0x1C, 0x95, 0xF2,
	0x90, 0x33, 0x53, 0x90, 0x3F, 0xAC, 0xC8, 0x18,
	0xD2, 0xF8, 0x52, 0x97, 0xDB, 0x4B, 0xB6, 0xD1,
	0xB8, 0xAA, 0x27, 0x79, 0x42, 0xC0, 0x78, 0x59,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
unsigned char DBMask[N];
unsigned char SeedMask[M];
unsigned char MaskedDB[N];
unsigned char MaskedSeed[M];
unsigned char EM[N];
const unsigned char HashL[32] = {
	0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14, 0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
	0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C, 0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55
};

void setBit(unsigned char& x, int pos) {
	x |= (1 << pos);
}
unsigned char GetByte(unsigned int x, int pos) {
	return (unsigned char)(x >> (pos << 3)) & 255;
}
void XorByte(unsigned int& x, unsigned char y, int pos) {
	x ^= (y << (pos << 3));
}
void SetByte(unsigned int& x, unsigned char  y, int pos) {
	XorByte(x, GetByte(x, pos), pos);
	XorByte(x, y, pos);
}
unsigned int ROTR(unsigned int x, int y) {
	return (x << (32 - y)) | (x >> y);
}
//------SHA256---------
namespace SHA256 {
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
	const unsigned int Hsh[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
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
	unsigned int phi0(unsigned int x) {
		return ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3);
	}
	unsigned int phi1(unsigned int x) {
		return ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10);
	}
	unsigned int sigma0(unsigned int x) {
		return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22);
	}
	unsigned int sigma1(unsigned int x) {
		return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25);
	}
	unsigned int Ch(unsigned int x, unsigned int y, unsigned int z) {
		return (x & y) ^ ((~x) & z);
	}
	unsigned int Ma(unsigned int x, unsigned int y, unsigned int z) {
		return (x & y) ^ (x & z) ^ (y & z);
	}
	void Extend() {
		For(i, 0, 15) {
			W[i] = 0;
			For(j, 0, 3) {
				SetByte(W[i], s[(i << 2) + j], 3 - j);
			}
					//cerr<<"W"<<i<<endl;
					//Rof(j,31,0)cerr<<((W[i]>>j)&1);
					//cerr<<endl;
		}
		For(i, 16, 63) {
			W[i] = W[i - 16] + W[i - 7] + phi0(W[i - 15]) + phi1(W[i - 2]);
		}
	}
	void Mix() {
		For(i, 0, 7) lst[i] = H[i];
		For(i, 0, 63) {
			unsigned int T1 = H[7] + sigma1(H[4]) + Ch(H[4], H[5], H[6]) + K[i] + W[i];
			unsigned int T2 = sigma0(H[0]) + Ma(H[0], H[1], H[2]);
			For(j, 0, 7) tmp[j] = H[j];
			For(j, 1, 3) H[j] = tmp[j - 1];
			For(j, 5, 7) H[j] = tmp[j - 1];
			H[0] = T1 + T2;
			H[4] = tmp[3] + T1;
		}
		For(i, 0, 7) H[i] = H[i] + lst[i];
		For(i, 0, 63) s[i] = 0;
	}
	void calc_SHA256() {
		Extend();
		Mix();
	}
	void SHA256(unsigned char *a, unsigned char *b, int l) {
		int cnt = l&63;
		unsigned long long len = (unsigned long long)l<<3ull;
		int m512 = cnt*8;
		int lim = l >> 6;
		For(i, 0, lim - 1) {
			For(j, 0, 63) {
				s[j] = a[(i << 6) | j];
			}
			calc_SHA256();
		}
		For(i, 0, cnt - 1) s[i] = a[i + (lim << 6)];
		//For(i, 0 ,cnt - 1) std::cerr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(s[i]);
		//cerr << endl;
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
					For(j, 0 ,63) std::cerr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(s[j]);
					cerr << endl;					
					calc_SHA256();
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
		//cerr << len << endl;
		//For(i, 0, 63) std::cerr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(s[i]);
		//cerr << endl;
		calc_SHA256();
		For(i, 0, 7) Rof(j, 3, 0) {
			b[(i << 2) | (3 ^ j)] = GetByte(H[i], j);
		}

		For(i, 0, 63) s[i] = 0;
		For(i, 0, 7) H[i] = Hsh[i];
	}
}
//---------SHA256-------------------

void getSeed() {
	unsigned int _seed[8];
	For(i, 0, 7) {
		_rdrand32_step(&_seed[i]);
		Rof(j, 3, 0) seed[(i << 2) | (3 ^ j)] = GetByte(_seed[i], j);
	}
	return;
}
//256-32-1=223λDB
void getDB() {
	int ZF = 256 - 1 - 1 - len - 32 * 2;
	For(i, 0, 31) DB[i] = HashL[i];
	For(i, 1, ZF) DB[i + 31] = 0;
	DB[32 + ZF] = 1;
	For(i, 0, len - 1) DB[33 + ZF + i] = s[i];
}
void MGF(unsigned char *s, int src_len, unsigned char *t, int dst_len) {
	//���lenλ��������ַ���
	int cnt = dst_len>>5;
	int tot = 0;
	For(i, 0, cnt) {
		if (tot >= dst_len) break;
		tot += 32;
		Rof(j, 3, 0) {
			s[src_len + j] = GetByte((unsigned int)i, 3 - j);
		}
		SHA256::SHA256(s, t+(i<<5), src_len+4);
		//if (i == 1) return;
	}
	For(i, 0, 3) s[src_len + i] = 0;
	For(i, dst_len, tot - 1) t[i] = 0;
}

const unsigned long long Base = (1ull << 32ull);
const int Bits = 32;
typedef bitset<4096> Bint;
struct abc {
	Bint s;
	int l;
	unsigned int num[150];
	abc() {
		memset(num, 0, sizeof(num));
		s.reset();
		l = 0;
	}
	abc(const unsigned char* _s, int _l, bool is_e) {
		memset(num, 0, sizeof(num));
		s.reset();
		for(int i = _l - 1;i > 0;i -= 4) {
			SetByte((_l - 1 - i) >> 2, _s[i], 0);
			SetByte((_l - 1 - i) >> 2, _s[i - 1], 1);
			SetByte((_l - 1 - i) >> 2, _s[i - 2], 2);
			SetByte((_l - 1 - i) >> 2, _s[i - 3], 3);
		}
		l = 64;
		removeZero();
		if(is_e) {
			Rof(i, _l - 1, 0) {
				For(j, 0, 7) {
					if ((_s[i] >> j) & 1) {
						s.set(((_l - 1 - i) << 3) | j);
					}
				}
			}
		}
	}
	abc(const abc &x) {
		memcpy(num,x.num,sizeof(num));
		l = x.l;
		s = x.s;
	}

	int len()const { return l; }
	int test(int x)const { return s.test(x); }
	void set(int x) { s.set(x); }
	void reset(int x) { s.reset(x); }
	bool any()const { return s.any(); }
	void shr() { s >>= 1; }


	//-----Bit-------
	unsigned char getBit(int x, int y) { return (num[x] >> y) & 1; }
	void xorBit(int x, int y, int pos) { num[x] ^= (y << pos); }
	void setBit(int x, int y, int pos) {
		xorBit(x, getBit(x, pos), pos);
		xorBit(x, y, pos);
	}
	//----Bit-------

	//----Byte------
	unsigned char GetByte(int x, int pos) const{
		return (unsigned char)(num[x] >> (pos << 3)) & 255;
	}
	void XorByte(int x, unsigned char y, int pos) {
		num[x] ^= (y << (pos << 3));
	}
	void SetByte(int x, unsigned char  y, int pos) {
		XorByte(x, GetByte(x, pos), pos);
		XorByte(x, y, pos);
	}
	//----Byte------
	
	//----uint------
	unsigned int GetUint(int pos)const { return num[pos]; }
	void SetUint(int pos, unsigned int y) {
		num[pos] = y;
	}
	void removeZero() {
		while (!GetUint(l - 1) && l > 1) --l;
	}
	//----uint------
	
	//-----Normalize---------
	int needNormalize() {
		if (num[len() - 1] >= (Base>>1)) return 0;
		int pos = 0;
		Rof(i, Bits - 1, 0) if ((num[len() - 1] >> i) & 1) {
			pos = i;
			break;
		}
		return Bits - pos - 1;
	}
	void normalize(int step) {
		if (!step) return;
		num[l] = num[l - 1] >> (Bits - step);
		Rof(i, len() - 1, 0) {
			num[i] = (num[i] << step) | (num[i - 1] >> (Bits - step));
		}
		while (GetUint(l)) ++l;
		removeZero();
	}
	void invNormalize(int step) {
		if (!step) return;
		num[l] = 0;
		For(i, 0, l - 1) {
			num[i] = (num[i] >> step) | ((num[i + 1] & (((unsigned int)1 << step) - 1)) << (Bits - step));
		}
		removeZero();
	}
	//-----Normalize---------

	abc operator +(const abc& a) {
		abc res;
		res.l = max(this->l, a.l);
		unsigned long long o = 0;
		For(i, 0, res.l - 1) {
			unsigned long long x = (unsigned long long)this->GetUint(i) + a.GetUint(i) + o;
			o = x >> Bits;
			res.SetUint(i, x & (Base - 1));
		}

		while (o) {
			res.SetUint(res.l++, o & (Base-1));
			o >>= Bits;
		}
		return res;
	}

	abc operator <<(int x) {
		abc res;
		Rof(i, l - 1, 0) res.SetUint(i + x, this->GetUint(i));
		For(i, 0, x - 1) res.SetUint(i, 0);
		res.l = this->l + x;
		return res;
	}//2193510331
	abc operator *(unsigned int a) {
		abc res; res.l = 1;
		if (!a) return res;
		res.l = this->l;
		unsigned long long o = 0;
		For(i, 0, this->l - 1) {
			unsigned long long x = (unsigned long long)this->GetUint(i) * a + o;
			res.SetUint(i, x & (Base - 1));
			o = x >> Bits;
		}
		while (o) {
			res.SetUint(res.l++, o& (Base -1));
			o >>= Bits;
		}
		return res;
	}
	abc operator *(const abc& a) {
		abc res; res.l = 1;
		For(i, 0, a.l - 1) {
			res = res + ((*this * a.GetUint(i)) << i);
		}
		return res;
	}
	abc operator -(const abc& a) {
		abc res;
		res.l = this->l;
		unsigned int o = 0;
		For(i, 0, l - 1) {
			unsigned long long x = (unsigned long long)this->GetUint(i);
			unsigned long long y = (unsigned long long)a.GetUint(i) + o;
			if (x < y) {
				x += Base;
				o = 1;
			}
			else {
				o = 0;
			}
			res.SetUint(i, (x - y) & (Base - 1));
		}
		res.num[res.len() - 1] -= o;
		res.removeZero();
		return res;
	}
	bool operator <(const abc& a)const {
		if (this->l < a.l) return 1;
		if (this->l > a.l) return 0;
		Rof(i, this->l - 1, 0) {
			if (this->GetUint(i) < a.GetUint(i)) return 1;
			if (this->GetUint(i) > a.GetUint(i)) return 0;
		}
		return 0;
	}
	bool operator <=(const abc& a)const {
		if (this->l < a.l) return 1;
		if (this->l > a.l) return 0;
		Rof(i, this->l - 1, 0) {
			if (this->GetUint(i) < a.GetUint(i)) return 1;
			if (this->GetUint(i) > a.GetUint(i)) return 0;
		}
		return 1;
	}
	void getLR(const abc &X, int pos, int l) {
		memset(num, 0, sizeof(num));
		For(i, pos, pos + l - 1) {
			num[i - pos] = X.num[i];
		}
		this->l = l;
	}
	abc operator %(const abc& a) {
		abc p = *this, q = a;
		abc tmp, tmp2;
		if (p < q) return p;
		if (p.l == q.l) {
			return p - q;
		}
		Rof(i, p.l - q.l, 0) {
			unsigned long long x = min((unsigned long long)((unsigned long long)p.GetUint(i + q.l) * Base + (unsigned long long)p.GetUint(i + q.l - 1))/q.GetUint(q.l - 1), Base - 1);
			tmp.getLR(p, i, 1 + q.l);
			tmp.removeZero();
			//[x-2, x]
			if (x >= 2ull) x = x - 2ull;
			else x = 0;
			tmp2 = q * (x & (Base - 1));
			tmp = tmp - tmp2;
			while (q <= tmp)
				tmp = tmp - q;
			For(j, 0, q.l) p.num[j + i] = tmp.num[j];
		}
		p.removeZero();
		return p;
	}
};
abc X, Y, Mod, Res;
unsigned char ans[512];
void Print(abc X, int len) {
	for(int i = len - 1; i >= 0; --i) {
		int id = (len - i - 1) << 2;
		ans[id] = X.GetByte(i, 3);
		ans[id | 1] = X.GetByte(i, 2);
		ans[id | 2] = X.GetByte(i, 1);
		ans[id | 3] = X.GetByte(i, 0);
	}
}
void Debug(abc X,int len) {
	Print(X, len);
	For(i, 0, (len<<2)-1)
			std::cerr << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ans[i]);
	cerr << endl;
}
int main() {
#ifdef _WIN32
	_setmode(_fileno(stdin), O_BINARY);
	_setmode(_fileno(stdout), O_BINARY);
#endif
	FILE* stream1 = stdin;
	FILE* stream2 = stdout;
#ifndef ONLINE_JUDGE
	stream1 = fopen("input2.bin", "rb");
	stream2 = fopen("seed2.bin", "wb");
#endif 
	size_t _a = fread(ig1, sizeof(unsigned char), 16, stream1);
	size_t _b = fread(n, sizeof(unsigned char), 256, stream1);
	size_t _c = fread(e, sizeof(unsigned char), 256, stream1);
	size_t _d = fread(ig2, sizeof(unsigned char), 256, stream1);
	len = fgetc(stream1);
	size_t _e = fread(s, sizeof(unsigned char), len, stream1);


	getSeed();
	getDB();
	//fwrite(DB, sizeof(unsigned char), 223, stream2);
	MGF(seed, 32, DBMask, 223);
	//fwrite(seed, sizeof(unsigned char), 36, stream2);
	//fwrite(DBMask, sizeof(unsigned char), 223, stream2);
	For(i, 0, 222)  MaskedDB[i] = DBMask[i] ^ DB[i];
	//fwrite(MaskedDB, sizeof(unsigned char), 223, stream2);
	MGF(MaskedDB, 223, SeedMask, 32);
	For(i, 0, 31) MaskedSeed[i] = SeedMask[i] ^ seed[i];
	//fwrite(MaskedSeed, sizeof(unsigned char), 32, stream2);
	EM[0] = 0;
	For(i, 0, 31) EM[i + 1] = MaskedSeed[i];
	For(i, 0, 222) EM[i + 33] = MaskedDB[i];
	//fwrite(EM, sizeof(unsigned char), 256, stream2);


	X = abc(EM, 256, 0);
	Y = abc(e, 256, 1);
	Mod = abc(n, 256, 0);
	Res.l = 1;Res.num[0] = 1;
	int step = Mod.needNormalize();
	//Debug(X, 64);
	if (step) {
		Mod.normalize(step);
	}
	int cnt = 0;
	while(Y.any()) {
		if (Y.test(0)) {
			Res = Res * X;
			Res.normalize(step);
			Res = Res % Mod;
			Res.invNormalize(step);
		}
		++cnt;
		X = X * X;
		//if (cnt == 1) {
		//	Debug(X, 128);
		//	X.normalize(step);
		//	X.invNormalize(step);
		//	Debug(X, 128);
		//}
		X.normalize(step);
		X = X % Mod;
		X.invNormalize(step);
		//if (cnt == 12) {
		//	/*Debug(X, 128);*/
		//	//Debug(X, 64);
		//}
		Y.shr();
	}
	//cerr << cnt << endl;
	Print(Res, 64);
	fwrite(ans, sizeof(unsigned char), 256, stream2);
	fclose(stream1);
	fclose(stream2);
}