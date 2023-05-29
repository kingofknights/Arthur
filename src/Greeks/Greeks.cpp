#include "Greeks.hpp"

#include <ctime>

#include "IVCalculator/Model2.h"

double Greeks::GetDelta(double s, double k, double v, double r, double t, bool IsCall) {
	if (IsCall) {
		return CallDelta(s, k, t, r, v, 0);
	} else {
		return PutDelta(s, k, t, r, v, 0);
	}
}

double Greeks::GetGamma(double s, double k, double v, double r, double t, bool iscall) {
	if (iscall) {
		return call_gamma(s, k, r, v, t) * 100.0;
	} else {
		return put_gamma(s, k, r, v, t) * 100.0;
	}
}

double Greeks::GetVega(double s, double k, double v, double r, double t, bool iscall) {
	if (iscall) {
		return call_vega(s, k, r, v, t) / 10000.0;
	} else {
		return put_vega(s, k, r, v, t) / 10000.0;
	}
}

double Greeks::GetRho(double s, double k, double v, double r, double t, bool IsCall) {
	if (IsCall) {
		return call_rho(s, k, r, v, t);
	} else {
		return put_rho(s, k, r, v, t);
	}
}

double Greeks::GetTheta(double s, double k, double v, double r, double t, bool IsCall) {
	if (IsCall) {
		return call_theta(s, k, r, v, t) / 36500.0;
	} else {
		return put_theta(s, k, r, v, t) / 36500.0;
	}
}

double Greeks::GetIV(double S, double K, double r, double T, double P, bool IsCE) { return option_price_implied_volatility_call_black_scholes_newton(S, K, r, T, P, IsCE); }

double Greeks::GetOptionPrice(double s, double k, double v, double r, double t, bool IsCall) {
	if (IsCall) {
		return call_price(s, k, r, v, t);
	} else {
		return put_price(s, k, r, v, t);
	}
}

double Greeks::GetExpiryGap(uint32_t Time) {
	const auto	 Cur_time = time(nullptr);
	const double expiry	  = Time + 3600;
	const double diff	  = (expiry + 315513000L - Cur_time);
	auto		 gap	  = diff / (252.0 * 24.0 * 60.0 * 60.0);
	return gap;
}
