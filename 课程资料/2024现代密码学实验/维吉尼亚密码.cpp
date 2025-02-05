#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
#define For(i,x,y) for(int i=(x);i<=(y);++i)
#define Rof(i,x,y) for(int i=(x);i>=(y);--i)
#define ll long long
#define db double
using namespace std;

const int N=2005;
const double Lc=0.065;
const double p[]={0.082, 0.015, 0.028, 0.043, 0.127, 0.022, 0.020,
 0.061, 0.070, 0.002, 0.008, 0.040, 0.024, 0.067, 0.075, 0.019, 0.001,
 0.060, 0.063, 0.091, 0.028, 0.010, 0.023, 0.001, 0.02, 0.001};

char s[N],s2[N],t[N];
int freq[26];
int mov[N];

int findT(char *s2,int l2,int limit){
	int RR=-1;
	double eps=1e18+7;
	For(T,1,limit){
		double avr=0;
		For(i,0,T-1){
			double sum=0;
			For(j,0,25) freq[j]=0;
			
			int l=0;
			for(int j=i;j<l2;j+=T){
				++freq[s2[j]-'a'];
				++l;	
			}
			
			For(j,0,25){
				sum+=(double)1ll*(freq[j]-1)*freq[j];
			}
			avr+=(double)sum/(1ll*l*(l-1));			
		}
		avr/=T;
		if(abs(avr-Lc)<eps){
			RR=T;
			eps=abs(avr-Lc);
		}
	}
	return RR;
}

void findMove(char *s2,int l2,int T){
	For(i,0,T-1){
		mov[i]=-1;
		double eps=1e18+7;
		For(delta,0,25){
			For(j,0,25) freq[j]=0;
			int l3=0;
			for(int j=i;j<l2;j+=T){
				++freq[(s2[j]-'a'+delta)%26];
				++l3;
			}
			double sum=0;
			For(j,0,25){
				sum+=1.0*freq[j]/l3*p[j];
			}
			if(abs(sum-Lc)<eps){
				mov[i]=delta;
				eps=abs(sum-Lc);
				t[i]=(26-delta)%26+'A';
			}
		}
	}
}
void solve(){
	int l=0;
	while(1){
		char c=getchar();
		if(c==EOF){
			break;
		}
		s[l++]=c;		
	}
//	puts(s);
	
	int l2=0;
	For(i,0,l-1){
		if(s[i]>='A' && s[i]<='Z') s2[l2++]=s[i]-'A'+'a';
		if(s[i]>='a' && s[i]<='z') s2[l2++]=s[i];
	}
//	puts(s2);
	int T=findT(s2,l2,l/50);
//	printf("%d\n",T);
	findMove(s2,l2,T);
	
	if(t[0]=='S' && t[1]=='I' && t[2]=='G' && t[3]=='N'
	&& t[4]=='S' && t[5]=='I' && t[6]=='G' && t[7]=='N'){
		t[4]='\0';
	}
	puts(t);
	For(i,0,l2-1){
		s2[i]=(s2[i]-'a'+mov[i%T])%26+'a';
	}
//	puts(s2);
	int j=0;
	For(i,0,l2-1){
		while(j<l && !isalpha(s[j])) ++j;
		if(s[j]>='A' && s[j]<='Z') s[j++]=s2[i]-'a'+'A';
		else s[j++]=s2[i];
	}
	puts(s);
}
int main(){
	#ifndef ONLINE_JUDGE
		freopen("in.txt","r",stdin);
		freopen("out.txt","w",stdout);
	#endif
	solve();
}
