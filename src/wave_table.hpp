#ifndef _WAVE_TABLE_HPP_
#define _WAVE_TABLE_HPP_

#include <complex>
#include <vector>

constexpr double PI = 3.1415926535897;

typedef enum {
  WT_SINE,
} wave_type;

template <typename sample_type>
class wave_table {
private:
  double power_dbfs;
  std::vector<std::complex<sample_type>> buffer;
public:
  wave_table(wave_type wt, size_t len, const sample_type ampl)
    : buffer(len, {0.0, 0.0}) {
    if (wt == wave_type::WT_SINE) {
      static sample_type tau = 2 * PI;
      static const std::complex<sample_type> J(0, 1);

      for (size_t I = 0; I < len; ++I) {
	std::complex<sample_type> p =
	  ampl * std::exp(J * static_cast<sample_type>(tau * I / len));
	buffer[I] = std::real(p);
      }

      power_dbfs = static_cast<double>(20 * std::log10(ampl));
    }
  }

  size_t size() const {
    return buffer.size();
  }

  const std::complex<sample_type>& operator[](size_t index) const {
    return buffer[index];
  }

  std::complex<sample_type>& operator[](size_t index) {
    return buffer[index];
  }
};

#endif
