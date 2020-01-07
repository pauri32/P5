#include <iostream>
#include <math.h>
#include "bell.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

bell::bell(const std::string &param)
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h
  */
  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value

  //N1 = 10;
  //N2 = 1;
  //I = 0.000328;

  //Create a tbl with one period of a sinusoidal wave
  tbl.resize(N);
  phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}


void bell::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	  A = vel / 127.;

    float F0 = (440.00*pow(2,((float)note-69.00)/12.00))/SamplingRate;
    float FC = N1*F0;
    float FM = N2*F0;

    step = tbl.size()*F0;
    mod_c = tbl.size()*FC;
    phase_m = 0;
    alpha_m = 2*M_PI*FM;
  }

  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & bell::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i=0; i<x.size(); ++i) {
    if (round(step*index) == tbl.size()){
      index = 0;
    }
    x[i] = A*tbl[round(step*index)];
    index += (1 + I*sin(phase_m));
    phase_m += alpha_m;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
