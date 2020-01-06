#ifndef SINTESISFM
#define SINTESISFM

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"


namespace upc {
  class sintesisFM: public upc::Instrument {
    EnvelopeADSR adsr;
    unsigned int index;
    //VE: Variables del Instrumento FM
    float A;
    float step; // Saltos de frecuencia
    float alfa; // Frecuencia de carry (rad/s) || Notación paper (pag 1)
    float beta; // Frecuencia modulacion (rad/s) || Notación paper (pag 1)
    float I;
    std::vector<float> E; //Envolvente temporal
    std::vector<float> tbl;
  public:
    sintesisFM(const std::string &param = "");
    void command(long cmd, long note, long velocity=1);
    const std::vector<float> & synthesize();
    bool is_active() const {return bActive;}
  };
}

#endif
