#ifndef __IV_CALCULATOR__
#define __IV_CALCULATOR__

double CallDelta(const double &UnderlyingPrice, const double &ExercisePrice, const double &Time, const double &Interest, const double &Volatility, const double &Dividend);

double call_price(const double &S, const double &K, const double &r, const double &v, const double &T);

double call_gamma(const double S, const double K, const double r, const double v, const double T);

double call_vega(const double S, const double K, const double r, const double v, const double T);

double call_theta(const double S, const double K, const double r, const double v, const double T);

double call_rho(const double S, const double K, const double r, const double v, const double T);

double PutDelta(const double &UnderlyingPrice, const double &ExercisePrice, const double &Time, const double &Interest, const double &Volatility, const double &Dividend);

double put_price(const double &S, const double &K, const double &r, const double &v, const double &T);

double put_gamma(const double S, const double K, const double r, const double v, const double T);

double put_vega(const double S, const double K, const double r, const double v, const double T);

double put_theta(const double S, const double K, const double r, const double v, const double T);

double put_rho(const double S, const double K, const double r, const double v, const double T);

double option_price_implied_volatility_call_black_scholes_newton(const double &S, const double &K, const double &r, const double &time, const double &option_price, bool IsCE);

#endif
