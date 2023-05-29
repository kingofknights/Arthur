#ifndef __BLACK_SCHOLES_CPP
#define __BLACK_SCHOLES_CPP

#include "black_scholes.h"

#include "bs_prices.h"

BlackScholesCall::BlackScholesCall(double _S, double _K, double _r, double _T) : S(_S), K(_K), r(_r), T(_T) {}

double BlackScholesCall::operator()(double sigma) const { return call_price(S, K, r, sigma, T); }

#endif
