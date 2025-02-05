#include <iostream>

#ifdef _WIN32
#include <fcntl.h>
#endif


#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
#define ll long long
#define db double

using namespace std;
unsigned char nn[4],s[24];
unsigned char type;
struct gf{
	unsigned long long a[5];
	void set(int x,unsigned long long y=1ull){
		a[x>>6]|=y<<(x&((1<<6)-1));
	}
	void _xor(int x){
		a[x>>6]^=(1ull<<(x&((1<<6)-1)));
	}
	int deg(){
		Rof(i,4,0){
			if(!a[i]) continue;
			Rof(j,63,0){
				if((a[i]>>j)&1ull){
					return (i<<6)+j;
				}
			}
		}
	}
	unsigned long long getbit(int x){
		return (a[x>>6]>>(x&63))&1ull;
	}
	gf(){
		For(i,0,4) a[i]=0; 
	}

	gf operator +(const gf &B){
		For(i,0,2){
			this->a[i]^=B.a[i]; 
		}
		return *this;
	}
	
	gf reduction(){
		//f(x)=x^131+x^13+x^2+x+1
		//260=   +130+129+24+13+12+11
		//259=130+129+128+23+12+11+10
		//..
		//249=120+119+118+13+2+1+0
		//248=130+119+118+117
		//..
		//131=13+2+1+0
		Rof(i,260,131){
			if(getbit(i)){
				_xor(i);
				_xor(i-131+13);
				_xor(i-131+2);
				_xor(i-131+1);
				_xor(i-131);
				if(getbit(i)){
					std::cerr<<i<<std::endl;
				}
			}
		}
		return *this;
	}
	
	void shl(){
		//左移1位
		Rof(i,2,0){
			a[i]<<=1ull;
			if(i){
				a[i]|=(a[i-1]>>63ull)&1ull;
			}
		}
		if(getbit(131)){
			_xor(131);
			_xor(13);
			_xor(2);
			_xor(1);
			_xor(0);
		}
	}
	gf shl(int x){
		gf C;
		For(i,0,131-x)
			C.set(i+x,this->getbit(i));
		return C;
	}
	gf operator *(const gf &B){
		gf C,D;
		For(i,0,2) D.a[i]=B.a[i];
		For(k,0,63){
			For(i,0,2){
				if((this->a[i]>>k)&1){
					For(j,0,4-i){
						C.a[j+i]^=D.a[j];
					}
				}
			}
			if(k!=63) D.shl();
		}
		return C.reduction();
	}
	
	gf pow2(){
		gf C;
		For(i,0,130){
			C.set(2*i,this->getbit(i));
		}
		return C.reduction();
	}
	bool check(){
		return a[0]==1 && !a[1] && !a[2] && !a[3] && !a[4]; 
	}
	gf inv(const gf &Z){
		//扩展欧几里得
		//Ag1+Bg2=u, Ah1+Bh2=v
		//Ah1+Bh1=v, A(g1-u/v*h1)+B(g2-u/v*h2)=u%v
		gf A=*this,B=Z,u=*this,v=Z;
		gf g1,g2,tmp1;
		g1.a[0]=1;
		while(!u.check()){
			int d=u.deg()-v.deg();
//			if(d==0){
//				cerr<<1<<endl;
//			}
			if(d<0){
				d=-d;
				swap(u,v);
				swap(g1,g2);
			}
			
			tmp1=v;
			u=u+v.shl(d);
			v=tmp1;
			
			tmp1=g2;
			g1=g1+g2.shl(d);
			g2=tmp1;
		}
		return g1;
	}
	
};
gf A,B,Ans,poly;
void gf_scan(gf &A){
	For(i,0,4) A.a[i]=0;
	unsigned char s;
	For(i,0,23){
		s=getchar();
		For(k,0,7){
			if((s>>k)&1){
				A.set((i<<3)+k);
			}
		}
	}
}
void gf_print(gf &A){
	For(i,0,23){
		unsigned char s=0;
		For(j,0,7){
			s|=A.getbit((i<<3)+j)<<j;
		}
		putchar(s);
	}
}

int main(){
	#ifdef _WIN32
	setmode(fileno(stdin), O_BINARY);
	setmode(fileno(stdout), O_BINARY);
	#endif
	
	#ifndef ONLINE_JUDGE
	freopen("in2.bin","r",stdin);
	freopen("out2.bin","w",stdout);
	#endif	
	
	unsigned int T=0;
	For(i,0,3) nn[i]=getchar();
	For(i,0,3) For(j,0,7){
		T=T+(((nn[i]>>j)&1)<<((i<<3)+j));
	}
	for(unsigned int i=1;i<=T;++i){
		type=getchar();
		gf_scan(A);
		gf_scan(B);
		switch(type){
			case 0:
				Ans=A+B;
				break;
			case 1:
				Ans=A*B;
				break;
			case 2:
				Ans=A.pow2();
				break;
			case 3:
				For(i,0,4) B.a[i]=0;
				B.set(131);
				B.set(13);
				B.set(2);
				B.set(1);
				B.set(0);
				Ans=A.inv(B);
				break;
		}
		gf_print(Ans);
	}
}
