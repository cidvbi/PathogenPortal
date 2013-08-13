/*
 
 Contains code from Numerical recipes in C
 
*/

#include "nuc_code.h"
#include "interface.h"
#include "io.h"
#include "samstat.h"
#include "misc.h"



char* shorten_pathname(char* p)
{
	int i;
	char* tmp = p;
	for(i = 0; i< strlen(p);i++){
		if(p[i] == '/'){
			tmp = p+i +1;
		}
	}
	return tmp;
}

unsigned char* reverse_complement2(unsigned char* p,int len)
{
	unsigned char* tmp = malloc(sizeof(unsigned char)*MAX_LINE);
	int i,c;
	c = 0;
	for(i =len-1; i >= 0;i--){
		tmp[c] = rev_nuc_code5[(int)p[i]];
		c++;
	}
	tmp[c] = 0;
	for(i= 0; i < len;i++){
		p[i] = tmp[i];
	}
	free(tmp);
	return p;
}

unsigned char* reverse_without_complement(unsigned char* p,int len)
{
	unsigned char* tmp = malloc(sizeof(unsigned char)*MAX_LINE);
	int i,c;
	c = 0;
	for(i =len-1; i >= 0;i--){
		tmp[c] = (int)p[i];
		c++;
	}
	tmp[c] = 0;
	for(i= 0; i < len;i++){
		p[i] = tmp[i];
	}
	free(tmp);
	return p;
}

int* reverse_without_complement_int(int* p,int len)
{
	int* tmp = malloc(sizeof(int)*MAX_LINE);
	int i,c;
	c = 0;
	for(i =len-1; i >= 0;i--){
		tmp[c] = p[i];
		c++;
	}
	tmp[c] = 0;
	for(i= 0; i < len;i++){
		p[i] = tmp[i];
	}
	free(tmp);
	return p;
}




float betai(float a, float b, float x)
{

	//float betacf(float a, float b, float x);
	///float gammln(float xx);
	//void nrerror(char error_text[]);
	float bt;
	if (x < 0.0 || x > 1.0){
		fprintf(stderr,"Bad x in routine betai\n");
	}
	if (x == 0.0 || x == 1.0){
		bt = 0.0;
	}else{	//Factors in front of the continued fraction.
		bt = exp(gammln(a+b)-gammln(a)-gammln(b)+a*log(x)+b*log(1.0-x));
	}
	if (x < (a+1.0)/(a+b+2.0)){	//Use continued fraction directly.
		return bt*betacf(a,b,x)/a;
	}else{//	Use continued fraction after making the sym-
		return 1.0-bt*betacf(b,a,1.0-x)/b;//	metry transformation.
	}
	
}

float betacf(float a, float b, float x)
{

	int m,m2;
	float aa,c,d,del,h,qab,qam,qap;
	qab=a+b;
	qap=a+1.0;
	qam=a-1.0;
	c=1.0;
	d=1.0-qab*x/qap;
	if (fabs(d) < FPMIN){
		d=FPMIN; 
	}
	d=1.0/d;
	h=d;
	for (m=1;m<=MAXIT;m++) {
		m2=2*m;
		aa=m*(b-m)*x/((qam+m2)*(a+m2));
		d=1.0+aa*d;
		
		if (fabs(d) < FPMIN){
			d=FPMIN;
		}
		c=1.0+aa/c;
		if (fabs(c) < FPMIN){
			c=FPMIN;
		}
		d=1.0/d;
		h *= d*c;
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
		
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS){
			break;
		}
		
	}
	if (m > MAXIT){
		//fprintf(stderr,"%f	%f	%f\n",a,b,x);
	//	fprintf(stderr,"a or b too big, or MAXIT too small in betacf\n");
	}
	return h;

}

double binomial_distribution(double p , int n, int k)
{
	double fac = gammln((double)n + 1.0);
	if(k < 0){
	//fprintf(stderr,"Bad k in binomialdist\n");
	}
	if(k > n){
		return 0;
	}
	return exp(k*log(p) +(n-k)*log(1.0 - p) + fac -gammln(k + 1.0) - gammln(n-k+1.0)); 
	
}



double gammln(const double xx)
{
	int j;
	double x,tmp,y,ser;
	static const double cof[14] = {57.1562356658629235,-59.5979603554754912,14.1360979747417471, -0.491913816097620199,0.339946499848118887e-4,0.465236289270485756e-4, -0.983744753048795646e-4, 0.0158088703224912494e-3,-0.210264441724104883e-3, 0.217439618115212643e-3, -0.164318106536763890e-3,0.888182239838527433e-4, -0.261908384015814087e-4, 0.368991826595316234e-5};
	if(xx <= 0.0){
		//fprintf(stderr,"bar arg in gammln");
	}
	y = xx;
	x = xx;
	tmp = x+5.24218750000000000;
	tmp = (x + 0.5) * log(tmp) - tmp;
	ser = 0.999999999999997092;
	for(j = 0; j < 14;j++){
		ser += cof[j] / ++y;
	}
	return tmp + log(2.5066282746310005*ser/x);
}



int cmpr(const void *a, const void *b)
{ 
	struct mer_info* const *one = a;
	struct mer_info* const *two = b;
		
	if((*one)->p_value >  (*two)->p_value){
		return 1;
	}else if((*one)->p_value <  (*two)->p_value ){
		return -1;
	}else{
		if((*one)->count <  (*two)->count ){
			return 1;
		}else if((*one)->count >  (*two)->count){
			return -1;
		}else{
			
		
			return 0;
		}
	}
}



int byg_end(const char* pattern,const char*text)
{
	const char* tmp = 0;
	int Tc;
	int i  = 0;
	int s = 0;
	int T[256];
	for (i = 0;i < 256;i++){ 
		T[i] = 0; 
	}
	
	int m = (int)strlen(pattern);
	int n = (int)strlen(text);
	if (m > n){
		i = m;
		m = n;
		n = i;
		tmp = text;
		text = pattern;
		pattern = tmp;
	}
	
	int mb = (1 << (m-1));
	
	for (i= 0;i < m;i++){
		T[(int)pattern[i]] |= (1 << i);
	}
	
	for (i = 0;i < n;i++){
		s <<= 1;
		s |= 1;
		if(!text[i]){
			return -1;
		}
		Tc = T[(int)text[i]];
		s &= Tc;
		if(s & mb){
			return i+1;
		}
	}
	return 0;
}



