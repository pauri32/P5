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
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value

  //VE: Hay que definir la envolvente temporal (En función de ADSR) 
  //Clarinete (A = 0.1, D = 0, S = 0.4, R = 0.1) (mirar paper fig.12)
  //Campana (A = 0, D = 0, S=0, R= 15) (mirar paper fig.14)
  E.resize(x.size());
  adsr(E);

  //Create a tbl with one period of a sinusoidal wave
   tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;

  //VE: No se qué período básico utilizar 
  /*==========================================================
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(alfa + I*sin(beta));
    alfa += step;
    beta += step;
  }
  //==========================================================*/  

  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}


void sintesisFM::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;

    int N1 = 3;
    int N2 = 1;
    	
    float f0 = (440.00*pow(2,((float)note-69.00)/12.00));
    //VE:Definir A 
    A = vel / 127.;
    //VE:Definir fc
    float fc;
    fc = N1 * f0;
    //VE: Definir fm
    float fm = N2 * f0;
    //VE: Definir alfa
    alfa = 2 * M_PI * fc/SamplingRate;
    //VE: Definir beta
    beta = 2 * M_PI * fm/SamplingRate;
    //VE:Definir Step
    step = 2*M_PI*f0/SamplingRate;
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
    x[i] = E[i]*A*sin(alfa+I*sin(beta));
    alfa += step;
    beta += step;
    if (index == tbl.size())
      index = 0;
  }

  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}
