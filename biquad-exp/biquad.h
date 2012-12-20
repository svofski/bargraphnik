/*
 * With thanks to Nigel Redmon http://www.earlevel.com/main/2011/01/02/biquad-formulas/
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

class Filter {
public:
	virtual float ffilter(float x) = 0;
	virtual float ifilter(int32_t x) = 0;
	virtual int32_t scale() const = 0;
	virtual float freq() const = 0;
	virtual float Q() const = 0;
	virtual int sampleRate() const = 0;
	virtual float A0() const = 0;
	virtual float A1() const = 0;
	virtual float A2() const = 0;
	virtual float B1() const = 0;
	virtual float B2() const = 0;
};

template <size_t S> class Biquad: public Filter {
public:
	Biquad(int sampleRate, float freq, float Q);

	float A0() const { return m_a0; };
	float A1() const { return m_a1; };
	float A2() const { return m_a2; };
	float B1() const { return m_b1; };
	float B2() const { return m_b2; };

	int32_t IA0() const { return m_ia0; };
	int32_t IA1() const { return m_ia1; };
	int32_t IA2() const { return m_ia2; };
	int32_t IB1() const { return m_ib1; };
	int32_t IB2() const { return m_ib2; };


	float ffilter(float x) {
		float result = m_a0*x + m_a1*m_x_1 + m_a2*m_x_2 - m_b1*m_y_1 - m_b2*m_y_2;
		m_x_2 = m_x_1;
		m_x_1 = x;
		m_y_2 = m_y_1;
		m_y_1 = result;
		return result;
	}

	float ifilter(int32_t x) {
		int32_t result = (m_ia0*x + m_ia1*m_ix_1 + m_ia2*m_ix_2 - m_ib1*m_iy_1 - m_ib2*m_iy_2)/S;
		m_ix_2 = m_ix_1;
		m_ix_1 = x;
		m_iy_2 = m_iy_1;
		m_iy_1 = result;
		return result;
	}

	int32_t scale() const { return (int32_t) S; }
	float freq() const { return m_Freq; };
	float Q() const { return m_Q; };
	int32_t sampleRate() const { return m_SampleRate; };


private:
	void calcBandpass(int sampleRate, float freq, float Q);
	void calcInteger();

	float m_Freq;
	float m_SampleRate;
	float m_Q;

	float m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_x_1, m_x_2, m_y_1, m_y_2;

	int32_t m_ia0, m_ia1, m_ia2, m_ib1, m_ib2;
	int32_t m_ix_1, m_ix_2, m_iy_1, m_iy_2;
};


template <size_t S> Biquad<S>::Biquad(int sampleRate, float freq, float Q) {
	m_Freq = freq;
	m_SampleRate = sampleRate;
	m_Q = Q;
	calcBandpass(sampleRate, freq, Q);
	calcInteger();
	m_x_1 = m_x_2 = m_y_1 = m_y_2 = 0;
	m_ix_1 = m_ix_2 = m_iy_1 = m_iy_2 = 0;
}

template <size_t S> void Biquad<S>::calcBandpass(int sampleRate, float freq, float Q) {
	float norm;
	float V, K;

	K = tan(M_PI * freq/sampleRate);
	norm = 1 / (1 + K / Q + K * K);
	m_a0 = K / Q * norm;
	m_a1 = 0;
	m_a2 = -m_a0;
	m_b1 = 2 * (K * K - 1) * norm;
	m_b2 = (1 - K / Q + K * K) * norm;
}

template <size_t S> void Biquad<S>::calcInteger() {
	m_ia0 = (int) round(m_a0 * S);
	m_ia1 = (int) round(m_a1 * S);
	m_ia2 = (int) round(m_a2 * S);
	m_ib1 = (int) round(m_b1 * S);
	m_ib2 = (int) round(m_b2 * S);
}
