#include <iostream>
#include <cstring>
#include <algorithm>
#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
#define ll long long
#define db double
using namespace std;

const int N=2005;
const int M=105;
const int mod=26;
const int PHI=12;
char s[N],t[N];
int a[M][M],tmp[M][M],sol[M],L[M][M],b[M];
int inv[26];

int gcd(int x,int y){
	return y?gcd(y,x%y):x;
}
int lcm(int x,int y){
	return x/gcd(x,y)*y;
}
void Gauss(int n){
	int f=1;
	
	//Gauss
	For(i,1,n-1){ //找主元
		bool find=0;
		For(j,i,n){
			if(inv[a[j][i]]){ 
			//有和26互质的数，直接当主元
				int v=inv[a[j][i]];
				f*=a[j][i];
				if(j!=i){
					f*=-1;
					For(k,1,n+1) swap(a[i][k],a[j][k]);					
				}
				For(k,i,n+1)
					a[i][k]=a[i][k]*v%mod; //主元系数化1
				f=f*v%26;
				For(j,i+1,n){
					Rof(k,n+1,i){
						a[j][k]=(a[j][k]-a[i][k]*a[j][i]%mod+mod)%mod;
					}
				}
				find=1;				
			}
		}
//		if(!find){
//			//没有和26互质的数，说明这个元的因数全是2的倍数或者13
//			//分类讨论
//			For(j,i,n){
//				if(a[j][i]!=0){
//					if(j!=i){
//						f*=-1;
//						For(k,1,n+1) swap(a[i][k],a[j][k]);
//						break;
//					}
//				}
//				For(j,i+1,n){
//					int _l=lcm(a[i][i],a[j][i]);
//					int x=_l/a[j][i],v=inv[x],y=_l/a[i][i];
//					
//					f=f*v%mod;
//					For(k,i,n+1) a[j][k]=a[j][k]*x%mod;
//					
//					For(k,i,n+1) a[j][k]=(a[j][k]-a[i][k]*y%mod+mod)%mod;
//				}
//			}
//		}
//		For(i,1,n+1){
//			For(j,1,n+2) printf("%d ",a[i][j]);
//		puts("");
//		}
//		puts("");
	}
	
	For(i,1,n) sol[i]=0;
	Rof(i,n,1){
		Rof(j,n,i+1){
			a[i][n+1]=(a[i][n+1]-a[i][j]*sol[j]%mod+mod)%mod;
		}
		if(inv[a[i][i]]) sol[i]=a[i][n+1]*inv[a[i][i]]%mod;
		else sol[i]=a[i][n+1]/a[i][i]%mod;
	}
}

void solve(){
	int n;
	scanf("%d",&n);
	scanf("%s",s+1);
	scanf("%s",t+1);
	int len=strlen(s+1);
	For(i,1,n){
		For(j,1,n+1) For(k,1,n+2) a[i][j]=0;
		For(j,1,n+1){
			int begin=(j-1)*n+1;
			For(k,1,n){
				a[j][k]=s[begin+k-1]-'A';
			}
			a[j][n+1]=1;
			a[j][n+2]=t[begin+i-1]-'A';
		}
		
//		For(i,1,n+1){
//			For(j,1,n+2) printf("%d ",a[i][j]);
//		puts("");
//		}
//		
//		puts("");
		Gauss(n+1);
		For(j,1,n) L[j][i]=sol[j];
		b[i]=sol[n+1];
	}
	For(i,1,n){
		For(j,1,n) printf("%d ",L[i][j]);
		puts("");
	}
	For(i,1,n) printf("%d ",b[i]);
}
void init(){
	inv[0]=0;
	For(i,1,25){
		if(gcd(i,26)!=1) continue;
		
		inv[i]=1;
		For(j,1,PHI-1){
			inv[i]=inv[i]*i%mod;
		}
	}
//	For(i,1,25){
//		printf("%d %d\n",i,inv[i]);
//	}
}
int main(){
	init();
	solve();
}
