#ifndef __BLACK_SCHOLES_H
#define __BLACK_SCHOLES_H

class BlackScholesCall {
private:
	double S;

	double K;

	double r;

	double T;

public:
	BlackScholesCall(double _S, double _K, double _r, double _T);

	double operator()(double sigma) const;
};

#endif
