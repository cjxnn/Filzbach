#include <math.h>

#include "repressilator.h"
#include "filzbach.h"

double getm(const double gamma, const double m, const double alpha, const double n, const double p, const double theta) {
	return m + delta_t * (alpha / (1 + pow(p, n)) - gamma * m + theta);
}

double getp(const double beta, const double m, const double mu, const double p){
	return p + delta_t * (beta * m - mu * p);
}

double getError(const double value) {
	return value + normal_draw(0, variance);
}