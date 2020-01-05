#include <iostream>
#include <math.h>
#include "senovibrato.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

senovibrato::senovibrato(const std::string &param)
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

  N1 = 3;
  N2 = 1;
  I = 0.000328;

  //Create a tbl with one period of a sinusoidal wave
  tbl.resize(N);
  phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}


void senovibrato::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	  A = vel / 127.;

    float F0 = (440.00*pow(2,((float)note-69.00)/12.00))/SamplingRate;
    float FC = N1*F0;
    float FM = N2*F0;

    step_c = tbl.size()*FC;
    step_m = tbl.size()*FM;
  }

  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & senovibrato::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i=0; i<x.size(); ++i) {
    if (round(step_c*index + I*tbl[step_m*index]) == tbl.size()){
      index = 0;
    }
    x[i] = A*tbl[round(step_c*index + I*tbl[round(step_m*index)])];
    index++;
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}