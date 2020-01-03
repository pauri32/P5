#include <iostream>
#include <math.h>
#include "sintesisFM.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

sintesisFM::sintesisFM(const std::string &param)
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h
  */
  KeyValue kv(param);
  int N1,N2;

  if (!kv.to_int("I",I))
    I = 40; //default value

  if (!kv.to_int("N1",N1))
    N1 = 40; //default value

  if (!kv.to_int("N2",N1))
    N2 = 40; //default value

  //Create a tbl with one period of a sinusoidal wave
  tbl.resize(N);
  float phase_c = 0, step_c = 2 * M_PI /(float) N1;
  float phase_m = 0, step_m = 2 * M_PI /(float) N2;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase_c+I*sin(phase_m));
    phase_c += step_c;
    phase_m += step_m;
  }
}


void sintesisFM::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    phase_c = 0;
    phase_m = 0;
	  A = vel / 127.;
    float F0 = (440.00*pow(2,((float)note-69.00)/12.00))/SamplingRate;
    step_c = 2*M_PI*F0*N1;
    step_m = 2*M_PI*F0*N2;
  }

  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & sintesisFM::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i=0; i<x.size(); ++i) {
    x[i] = sin(phase_c+I*sin(phase_m));
    phase_c += step_c;
    phase_m += step_m;
    if (index == tbl.size())
      index = 0;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
