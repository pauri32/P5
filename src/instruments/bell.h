#ifndef BELL
#define BELL

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class bell: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
	  float A;
    float I, N1, N2;
    float phase;
    float step, mod_c, phase_m, alpha_m;
    std::vector<float> tbl;
  public:
    bell(const std::string &param = "");
    void command(long cmd, long note, long velocity=1);
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;}
  };
}

#endif
