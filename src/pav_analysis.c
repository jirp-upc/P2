#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
	double res_num = 1e-12;

     	for(unsigned int i=0;i<N;i++){
    	 	res_num += x[i]*x[i];
    	}

    return (10*log10(res_num/N));
}

float compute_am(const float *x, unsigned int N){
	float res = 0;
	for(unsigned int i=0;i<N;i++){
		res += fabs(x[i]);
	}
    return res/N; //retornar operación directamente
}

float compute_zcr(const float *x, unsigned int N, float fm) {
	int count = 0;
	for(unsigned int i=1;i<N;i++){
		if(x[i]*x[i-1]<0){
	     		count++;
		}
	}	
    return (fm/2.0)*(1.0/(N-1.0))*count;
}

float hamming_window(int n, int M){
    return (float) 0.54 - 0.46*cos((2*M_PI*n)/(M-1));
}

