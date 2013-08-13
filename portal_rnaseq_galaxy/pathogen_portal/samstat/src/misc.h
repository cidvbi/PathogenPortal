/*
 
Contains code from Numerical recipes in C
 
 */

#define PI 3.14159265
#define MAXIT 100 
#define EPS 3.0e-7 
#define FPMIN 1.0e-30

unsigned char* reverse_complement2(unsigned char* p,int len);
unsigned char* reverse_without_complement(unsigned char* p,int len);
int* reverse_without_complement_int(int* p,int len);

double binomial_distribution(double p , int n, int k);
double gammln(const double xx);
float betai(float a, float b, float x);
float betacf(float a, float b, float x);

int cmpr(const void *a, const void *b);

char* shorten_pathname(char* p);
int byg_end(const char* pattern,const char*text);



