#include <iostream>
#include <cstring>
#include <algorithm>
#include <iomanip>
//#include <io.h>
#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
using namespace std;
constexpr unsigned long long Base = (1ull<<32);
constexpr int Bits = 32;
constexpr int SIZE = 100;
struct abc {
	int l;
	unsigned int num[SIZE]{};
	static int OB[SIZE],NB[SIZE],tmp[SIZE];
	abc() {
		memset(num, 0, sizeof(num));
		memset(OB, 0, sizeof(OB));
		memset(NB, 0, sizeof(NB));

		l = 0;
	}
	abc(const char* _s, int _l) {
		memset(num, 0, sizeof(num));
		memset(OB, 0, sizeof(OB));
		memset(NB, 0, sizeof(NB));

		For(i,0,_l-1) OB[i] = _s[i] - '0';
		this->l = 1;
		int pos = 0;
		while (_l) {
			int nl = 0;
			int res = 0;
			For(i,0,_l - 1) {
				res = res * 10 + OB[i];
				NB[nl++] = (res>>1);
				res = res&1;
			}
			setBit(l - 1,res&1,pos++);
			if (pos == Bits) {
				++l;
				pos = 0;
			}
			int not_0 = nl;
			For(i,0,nl-1) {
				if (NB[i]!=0) {
					not_0 = i;
					break;
				}
			}
			For(i, not_0, nl-1) OB[i - not_0] = NB[i];
			_l = nl - not_0;
		}
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

	[[nodiscard]] int len()const { return l; }

	//-----Bit-------
	[[nodiscard]] unsigned char getBit(const int x, const int y) const { return (num[x] >> y) & 1; }
	void xorBit(const int x, const int y, const int pos) { num[x] ^= static_cast<unsigned int>(y << pos); }
	void setBit(const int x, const int y, const int pos) {
		xorBit(x, getBit(x, pos), pos);
		xorBit(x, y, pos);
	}
	//----Bit-------

	//----Byte------
	 [[nodiscard]] unsigned char GetByte(const int x, const int pos) const{
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
	[[nodiscard]] unsigned int GetUint(const int pos)const { return num[pos]; }
	void SetUint(int pos, unsigned int y) {
		num[pos] = y;
	}
	void removeZero() {
		while (!GetUint(l - 1) && l > 1) --l;
	}
	//----uint------

	//-----Normalize---------
	[[nodiscard]] int needNormalize() const {
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
		if (p.l == q.l) {
			return abc(1);
		}
		abc tmp, r(0);
		r.l = p.l - q.l + 1;
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
	[[nodiscard]] unsigned int mod3() const{
		unsigned int res = 0;
		For(i, 0, len() - 1) {
			res = (res + num[i] % 3) %3;
		}
		return res;
	}
	[[nodiscard]] bool isZero() const {
		if (this->l == 1 && num[0] == 0) return true;
		return false;
	}
	[[nodiscard]] bool isOne() const {
		if (this->l == 1 && num[0] == 1) return true;
		return false;
	}
};

char p[SIZE],n[SIZE],a[SIZE],b[SIZE];
int abc::tmp[SIZE];
int abc::NB[SIZE];
int abc::OB[SIZE];
abc P,N,A,B,X;

void f(abc &x,abc &a,abc &b,const abc &P, const abc &N, const abc &A,const abc &B) {
	unsigned int r = x.mod3();
	if (r == 1) {
		x = B * x % P;
		b = ( b + 1 ) % N;
	}
	else {
		if (r == 0) {
			x = x * x % P;
			a = a * 2 % N;
			b = b * 2 % N;
		}
		else {
			x = A * x % P;
			a = ( a + 1 ) % N;
		}
	}
}
void swap(abc &x, abc &y) noexcept {
	abc p = y;
	y = x;
	x = p;
}
abc exgcd(const abc &a, const abc &N, abc &x1, abc &x2) {
	abc p(a),q(N),r(a);
	while (!r.isZero()) {
		abc m = p / q;
		r =p % q;
		p = q;
		q = r;

		abc tmp = (x1 + N - m * x2 % N) % N;
		x1 = x2;
		x2 = tmp;
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

abc Pollard_rho(const abc &P, const abc &N, const abc &A, const abc &B) {
	abc x1(1), a1(0), b1(0);
	f(x1, a1, b1, P, N, A, B);
	abc x2(x1), a2(a1), b2(b1);
	f(x2, a2, b2, P, N, A, B);
	while (x1 != x2) {
		f(x1,a1,b1,P, N, A, B);
		f(x2,a2,b2, P, N, A, B);
		f(x2,a2,b2, P, N, A, B);
	}

	abc _x1(1), _x2(0);
	abc d = exgcd((b2 + N - b1)%N, N,_x1,_x2);
	if (d.isOne()) {
		return (a1 + N - a2)%N * _x1 % N;
	}
	else {
		abc c = (a1 + N - a2) % N / d * _x1 % N;
		abc Nd = N / d;
		abc u = qpow(A, c), v=qpow(A, Nd);
		while (c < N && u!= B) {
			u = u * v % P;
			c = (c + Nd )%N;
		}
		return c;
	}
}
int _2x[SIZE], _10x[SIZE], tmp[SIZE];
int _2l, _10l;
void To2(const abc &x) {
	For(i, 0, x.len() - 1) {
		For(j, 0, 31) {
			_2x[_2l++] = static_cast<int>(x.getBit(i, j));
		}
	}
	while (!_2x[_2l - 1] && _2l > 1) --_2l;
	reverse(_2x, _2x+_2l);
}
void To10() {
	while (_2l) {
		int res = 0;
		int l =0;
		For(i, 0, _2l - 1) {
			res = res * 2 +_2x[i];
			tmp[l++] = res / 10;
			res %= 10;
		}
		_10x[_10l++] = res;
		int not_0 =l;
		For(i,0,l-1) {
			if (tmp[i]) {
				not_0 = i;
				break;
			}
		}
		For(i, not_0, l - 1) _2x[i - not_0] = tmp[i];
		_2l = l-not_0;
	}
	reverse(_10x, _10x+_10l);
}
int main() {
	scanf("%s",p);
	scanf("%s",n);
	scanf("%s",a);
	scanf("%s",b);

	P=abc(p,strlen(p));
	N=abc(n,strlen(n));
	A=abc(a,strlen(a));
	B=abc(b,strlen(b));

	X = Pollard_rho(P,N,A,B);
	To2(X);
	To10();
	For(i, 0, _10l - 1) printf("%d",_10x[i]);
    return 0;
}
//5748539351
//11567100408