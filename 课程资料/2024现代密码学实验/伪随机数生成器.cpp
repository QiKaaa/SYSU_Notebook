#include <iostream>
#include <cstring>
#include <bitset>
#include <algorithm>
#include <iomanip>
// #include <io.h>
#ifdef _WIN32
#include <fcntl.h>
#endif
#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
using namespace std;
constexpr unsigned long long Base = (1ull<<32);
constexpr int Bits = 32;
constexpr int SIZE = 100;
constexpr int SIZE2 = 200;
struct abc {
	int l;
	unsigned int num[SIZE]{};
	abc() {
		memset(num, 0, sizeof(num));
		l = 0;
	}
	abc(const unsigned char* _s, int _l) {
		memset(num, 0, sizeof(num));
		for (int i = 0; i < _l; i+=4) {
			SetByte(i>>2, _s[i], 0);
			SetByte(i>>2, _s[i + 1], 1);
			SetByte(i>>2, _s[i + 2], 2);
			SetByte(i>>2, _s[i + 3], 3);
		}
		l = _l>>2;
		removeZero();
	}
	abc(const abc &x) {
		memcpy(num,x.num,sizeof(num));
		l = x.l;
	}
	explicit abc(const unsigned int &x) {
		memset(num,0,sizeof(num));
		num[0] = x;
		l = 1;
	}

	int len()const { return l; }

	//-----Bit-------
	unsigned char getBit(const int x, const int y) const { return (num[x] >> y) & 1; }
	void xorBit(const int x, const int y, const int pos) { num[x] ^= static_cast<unsigned int>(y << pos); }
	void setBit(const int x, const int y, const int pos) {
		xorBit(x, getBit(x, pos), pos);
		xorBit(x, y, pos);
	}
	//----Bit-------

	//----Byte------
	 unsigned char GetByte(const int x, const int pos) const{
		return static_cast<unsigned char>(num[x] >> (pos << 3)) & 255;
	}
	void XorByte(const int x, const unsigned char y, int pos) {
		num[x] ^= (y << (pos << 3));
	}
	void SetByte(const int x, const unsigned char  y, const int pos) {
		XorByte(x, GetByte(x, pos), pos);
		XorByte(x, y, pos);
	}
	//----Byte------

	//----uint------
	unsigned int GetUint(const int pos)const { return num[pos]; }
	void SetUint(int pos, unsigned int y) {
		num[pos] = y;
	}
	void removeZero() {
		while (!GetUint(l - 1) && l > 1) --l;
	}
	//----uint------

	//-----Normalize---------
	int needNormalize() const {
		if (num[len() - 1] >= (Base>>1)) return 0;
		int pos = 0;
		Rof(i, Bits - 1, 0) if ((num[len() - 1] >> i) & 1) {
			pos = i;
			break;
		}
		return Bits - pos - 1;
	}
	void normalize(const int step) {
		if (!step) return;
		num[l] = num[l - 1] >> (Bits - step);
		Rof(i, len() - 1, 0) {
			num[i] = (num[i] << step);
			if (i!=0) num[i] |= (num[i - 1] >> (Bits - step));
		}
		while (GetUint(l)) ++l;
		removeZero();
	}
	void invNormalize(const int step) {
		if (!step) return;
		num[l] = 0;
		For(i, 0, l - 1) {
			num[i] = (num[i] >> step) | ((num[i + 1] & ((static_cast<unsigned int>(1) << step) - 1)) << (Bits - step));
		}
		removeZero();
	}
	//-----Normalize---------

	// abc& operator = (const abc &x) {
	// 	memcpy(num,x.num,sizeof(num));
	// 	l = x.l;
	// 	return *this;
	// }
	abc operator +(const unsigned int a) const {
		abc res;
		res.l = this -> l;
		unsigned long long o = 0;
		For(i, 0, res.l - 1) {
			unsigned long long x = static_cast<unsigned long long>(this->GetUint(i)) + ( i == 0? a : 0) + o;
			o = x >> Bits;
			res.SetUint(i, x & (Base - 1));
		}

		while (o) {
			res.SetUint(res.l++, o & (Base-1));
			o >>= Bits;
		}
		return res;
	}
	abc operator +(const abc& a) const {
		abc res;
		res.l = max(this->l, a.l);
		unsigned long long o = 0;
		For(i, 0, res.l - 1) {
			unsigned long long x = static_cast<unsigned long long>(this->GetUint(i)) + a.GetUint(i) + o;
			o = x >> Bits;
			res.SetUint(i, x & (Base - 1));
		}

		while (o) {
			res.SetUint(res.l++, o & (Base-1));
			o >>= Bits;
		}
		return res;
	}
	//plus

	abc operator <<(int x) const {
		abc res;
		Rof(i, l - 1, 0) res.SetUint(i + x, this->GetUint(i));
		For(i, 0, x - 1) res.SetUint(i, 0);
		res.l = this->l + x;
		return res;
	}

	abc operator *(unsigned int a) const {
		abc res; res.l = 1;
		if (!a) return res;
		res.l = this->l;
		unsigned long long o = 0;
		For(i, 0, this->l - 1) {
			unsigned long long x = static_cast<unsigned long long>(this->GetUint(i)) * a + o;
			res.SetUint(i, x & (Base - 1));
			o = x >> Bits;
		}
		while (o) {
			res.SetUint(res.l++, o& (Base -1));
			o >>= Bits;
		}
		return res;
	}
	abc operator *(const abc& a) const {
		if (a.isZero() || this->isZero()) return abc(0);
		abc res; res.l = 1;
		For(i, 0, a.l - 1) {
			res = res + ((*this * a.GetUint(i)) << i);
		}
		return res;
	}

	abc operator -(const abc& a) const {
		abc res;
		res.l = this->l;
		unsigned int o = 0;
		For(i, 0, l - 1) {
			unsigned long long x = this->GetUint(i);
			unsigned long long y = static_cast<unsigned long long>(a.GetUint(i)) + o;
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
		if (this->l < a.l) return true;
		if (this->l > a.l) return false;
		Rof(i, this->l - 1, 0) {
			if (this->GetUint(i) < a.GetUint(i)) return true;
			if (this->GetUint(i) > a.GetUint(i)) return false;
		}
		return false;
	}
	bool operator <=(const abc& a)const {
		if (this->l < a.l) return true;
		if (this->l > a.l) return false;
		Rof(i, this->l - 1, 0) {
			if (this->GetUint(i) < a.GetUint(i)) return true;
			if (this->GetUint(i) > a.GetUint(i)) return false;
		}
		return true;
	}
	bool operator !=(const abc &a)const {
		if (this->l != a.l) return true;
		Rof(i, this->l - 1, 0) {
			if (this->GetUint(i) != a.GetUint(i)) return true;
		}
		return false;
	}

	void getLR(const abc &X, const int pos, const int l) {
		memset(num, 0, sizeof(num));
		For(i, pos, pos + l - 1) {
			num[i - pos] = X.num[i];
		}
		this->l = l;
	}
	abc operator %(const abc& a) const {
		abc p = *this, q = a;
		int step = q.needNormalize();
		p.normalize(step);
		q.normalize(step);

		if (p < q) {
			//do nothing
		}
		else {
			if (p.l == q.l) {
				p = p - q;
			}
			else {
				abc tmp;
				Rof(i, p.l - q.l, 0) {
					unsigned long long x = min((static_cast<unsigned long long>(p.GetUint(i + q.l)) * Base + static_cast<unsigned long long>(p.GetUint(i + q.l - 1)))/q.GetUint(q.l - 1), Base - 1);
					tmp.getLR(p, i, 1 + q.l);
					tmp.removeZero();
					//[x-2, x]
					if (x >= 2ull) x = x - 2ull;
					else x = 0;
					abc tmp2 = q * (x & (Base - 1));
					tmp = tmp - tmp2;
					while (q <= tmp)
						tmp = tmp - q;
					For(j, 0, q.l) p.num[j + i] = tmp.num[j];
				}
			}
		}
		p.removeZero();
		p.invNormalize(step);
		return p;
	}
	abc operator /(const abc& a) const {
		abc p = *this, q = a;
		int step = q.needNormalize();
		p.normalize(step);
		q.normalize(step);

		if (p < q) {
			return abc(0);
		}
		abc tmp, r(0);
		r.l = p.l - q.l ;
		Rof(i, p.l - q.l, 0) {
			unsigned long long x = min((static_cast<unsigned long long>(p.GetUint(i + q.l)) * Base + static_cast<unsigned long long>(p.GetUint(i + q.l - 1)))/q.GetUint(q.l - 1), Base - 1);
			tmp.getLR(p, i, 1 + q.l);
			tmp.removeZero();
			//[x-2, x]
			if (x >= 2ull) x = x - 2ull;
			else x = 0;
			abc tmp2 = q * (x & (Base - 1));
			tmp = tmp - tmp2;
			while (q <= tmp){
				tmp = tmp - q;
				++x;
			}
			For(j, 0, q.l) p.num[j + i] = tmp.num[j];
			r.num[i] = (x & (Base - 1));
			r.num[i + 1] +=(x >> Bits);
		}
		while (r.GetUint(r.len())) ++r.l;
		r.removeZero();
		return r;
	}
	unsigned int mod3() const{
		unsigned int res = 0;
		For(i, 0, len() - 1) {
			res = (res + num[i] % 3) %3;
		}
		return res;
	}
	bool isZero() const {
		if (this->l == 0) return true;
		if (this->l == 1 && num[0] == 0) return true;
		return false;
	}
	bool isOne() const {
		if (this->l == 1 && num[0] == 1) return true;
		return false;
	}
};

abc P,G,S,SIG;

void swap(abc &x, abc &y) noexcept {
	abc p = y;
	y = x;
	x = p;
}
abc exgcd(const abc &a, const abc &N, abc &x1, abc &x2) {
	abc p(a),q(N),r(a);
	abc m, tmp;
	int cnt = 0;
	while (!r.isZero()) {
		// if (cnt == 15) {
		// 	cnt+=0;
		// }
		m = p / q;
		r =p % q;
		p = q;
		q = r;

		tmp = (x1 + N - m * x2 % N) % N;
		x1 = x2;
		x2 = tmp;
		// printf("%d\n",++cnt);
		// Print(p);
		// Print(q);
		// Print(x1);
		// Print(x2);
		// puts("------");
	}
	return p;
}
abc qpow(const abc &x, const abc &y) {
	abc res(1);
	abc tmp(x);
	For(i, 0, y.len() - 1) {
		For(j, 0, 31) {
			if (y.getBit(i,j)) {
				res =res * tmp %P;
			}
			tmp = tmp * tmp %P;
		}
	}
	return res;
}
abc inv(const abc &a) {
	abc x1(1),x2(0);
	exgcd(a,P,x1,x2);
	return x1;
	// return qpow(a,P-abc(2));
}

unsigned char GetByte(unsigned int x, int pos) {
	return (unsigned char)(x >> (pos << 3)) & 255;
}

unsigned char p[16];
unsigned char g[16];
unsigned char s0[16];
unsigned char n0[4];
unsigned int n;
unsigned char Ans;
unsigned long long ans0= 0;
unsigned long long ans1 = 0;
int main() {
#ifdef _WIN32
	_setmode(_fileno(stdin), O_BINARY);
	_setmode(_fileno(stdout), O_BINARY);
#endif
	FILE* stream1 = stdin;
	FILE* stream2 = stdout;
#ifndef ONLINE_JUDGE
	stream1 = fopen("input8.bin", "rb");
	stream2 = fopen("output8.bin", "wb");
#endif
	size_t lp = fread(p, sizeof(unsigned char), 16, stream1);
	size_t lg = fread(g, sizeof(unsigned char), 16, stream1);
	size_t ls = fread(s0, sizeof(unsigned char), 16, stream1);
	size_t _n = fread(n0, sizeof(unsigned char), 4, stream1);
	for (size_t i = 0; i < _n; ++i) {
		n = (n<<8)+ n0[3 - i];
	}

	P = abc(p, lp);
	G = abc(g, lg);
	S = abc(s0, ls);

	SIG = P/abc(2);
	while (n > 0) {
		Ans = 0;
		For(i, 0, 7) {
			S = qpow(G,S);
			if (S < SIG) {
				++ans0;
			} else {
				++ans1;
				Ans |=(1 << i);
			}
		}
		fputc(Ans, stream2);
		--n;
	}
	for (size_t i = 0; i < 4; ++i) fputc(GetByte(ans0, i), stream2);
	for (size_t i = 0; i < 4; ++i) fputc(GetByte(ans1, i), stream2);
	fclose(stream1);
	fclose(stream2);
}