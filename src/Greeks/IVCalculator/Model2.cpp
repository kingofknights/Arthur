#include "Model2.h"

#define _USE_MATH_DEFINES

#include <cmath>
#include <iomanip>
#include <iostream>

using namespace std;

// WOrked perfectly
//
/// Standard normal probability density function
double norm_pdf(const double &x) { return (1.0 / (pow(2 * M_PI, 0.5))) * exp(-0.5 * x * x); }

// An approximation to the cumulative distribution function
// for the standard normal distribution
// Note: This is a recursive function

double norm_cdf(const double &x) {
	double k	 = 1.0 / (1.0 + 0.2316419 * x);
	double k_sum = k * (0.319381530 + k * (-0.356563782 + k * (1.781477937 + k * (-1.821255978 + 1.330274429 * k))));

	if (x >= 0.0) {
		return (1.0 - (1.0 / (pow(2 * M_PI, 0.5))) * exp(-0.5 * x * x) * k_sum);
	} else {
		return 1.0 - norm_cdf(-x);
	}
}

/// This calculates d_j, for j in {1,2}. This term appears in the closed
/// form solution for the European call or put price

double d_j(const int &j, const double &S, const double &K, const double &r, const double &v, const double &T) { return (log(S / K) + (r + (pow(-1, j - 1)) * 0.5 * v * v) * T) / (v * (pow(T, 0.5))); }

/// Calculate the European vanilla call Delta
double CallDelta(const double &UnderlyingPrice, const double &ExercisePrice, const double &Time, const double &Interest, const double &Volatility, const double &Dividend) {
	return norm_cdf(d_j(1, UnderlyingPrice, ExercisePrice, Interest, Volatility, Time));
}

// Calculate the European vanilla call price based on
// underlying S, strike K, risk-free rate r, volatility of
// underlying sigma and time to maturity T
double call_price(const double &S, const double &K, const double &r, const double &v, const double &T) {
	return S * norm_cdf(d_j(1, S, K, r, v, T)) - K * exp(-r * T) * norm_cdf(d_j(2, S, K, r, v, T));
}

// Calculate the European vanilla call Gamma
double call_gamma(const double S, const double K, const double r, const double v, const double T) { return norm_pdf(d_j(1, S, K, r, v, T)) / (S * v * sqrt(T)); }

// Calculate the European vanilla call Vega
double call_vega(const double S, const double K, const double r, const double v, const double T) { return S * norm_pdf(d_j(1, S, K, r, v, T)) * sqrt(T); }

// Calculate the European vanilla call Theta
double call_theta(const double S, const double K, const double r, const double v, const double T) {
	return -(S * norm_pdf(d_j(1, S, K, r, v, T)) * v) / (2 * sqrt(T)) - r * K * exp(-r * T) * norm_cdf(d_j(2, S, K, r, v, T));
}

// Calculate the European vanilla call Rho
double call_rho(const double S, const double K, const double r, const double v, const double T) { return K * T * exp(-r * T) * norm_cdf(d_j(2, S, K, r, v, T)); }

// Calculate the European vanilla put price based on
// underlying S, strike K, risk-free rate r, volatility of
// underlying sigma and time to maturity T

double put_price(const double &S, const double &K, const double &r, const double &v, const double &T) {
	return -S * norm_cdf(-d_j(1, S, K, r, v, T)) + K * exp(-r * T) * norm_cdf(-d_j(2, S, K, r, v, T));
}

/// Calculate the European vanilla put Delta
double PutDelta(const double &UnderlyingPrice, const double &ExercisePrice, const double &Time, const double &Interest, const double &Volatility, const double &Dividend) {
	return norm_cdf(d_j(1, UnderlyingPrice, ExercisePrice, Interest, Volatility, Time)) - 1;
}

// Calculate the European vanilla put Gamma
double put_gamma(const double S, const double K, const double r, const double v, const double T) {
	return call_gamma(S, K, r, v, T);  // Identical to call by put-call parity
}

// Calculate the European vanilla put Vega
double put_vega(const double S, const double K, const double r, const double v, const double T) {
	return call_vega(S, K, r, v, T);  // Identical to call by put-call parity
}

// Calculate the European vanilla put Theta
double put_theta(const double S, const double K, const double r, const double v, const double T) {
	return -(S * norm_pdf(d_j(1, S, K, r, v, T)) * v) / (2 * sqrt(T)) + r * K * exp(-r * T) * norm_cdf(-d_j(2, S, K, r, v, T));
}

// Calculate the European vanilla put Rho
double put_rho(const double S, const double K, const double r, const double v, const double T) { return -T * K * exp(-r * T) * norm_cdf(-d_j(2, S, K, r, v, T)); }

double ImpliedPutVolatility(const double UnderlyingPrice, const double ExercisePrice, const double Time, const double Interest, const double CurrentPrice, const double Dividend) {
	double High = 5;
	double Low	= 0;
	do {
		if (put_price(UnderlyingPrice, ExercisePrice, Interest, (High + Low) / 2, Time) > CurrentPrice) {
			High = (High + Low) / 2;
		} else {
			Low = (High + Low) / 2;
		}
	} while ((High - Low) > 0.0001);

	return (High + Low) / 2;
}

double ImpliedCallVolatility(const double UnderlyingPrice, const double ExercisePrice, const double Time, const double Interest, const double Target, const double Dividend) {
	double High = 5;
	double Low	= 0;
	do {
		if (call_price(UnderlyingPrice, ExercisePrice, Interest, (High + Low) / 2, Time) > Target) {
			High = (High + Low) / 2;
		} else {
			Low = (High + Low) / 2;
		}
	} while ((High - Low) > 0.0001);
	return (High + Low) / 2;
}

double option_price_implied_volatility_call_black_scholes_newton(const double &S /*Future Price*/, const double &K /*Strike Price*/, const double &r /*Rate Of Interest*/, const double &time,
																 const double &option_price, bool IsCE) {
	if (S <= 0) return 0.0;	 /// No Future No IV
	/// cout << " option_price_implied_volatility_call_black_scholes_newton S"<< std::fixed << std::setprecision(4) << S <<" K "<< K << " r "<< r << " time "<< time << " option_price "<<option_price
	/// <<endl;

	if (IsCE) {
		return ImpliedCallVolatility(S, K, time, r, option_price, 0);
	} else {
		return ImpliedPutVolatility(S, K, time, r, option_price, 0);
	}
};
