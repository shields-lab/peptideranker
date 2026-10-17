
#ifndef NNto1_h
#define NNto1_h 1

#include <stdlib.h>
#include <math.h>
#include "General.h"
#include "NN.h"
#include "NNt.h"

//#define MAX 8000
#define MAX 14000 //cm


// NNto1 ver. 1.0 (18/7/2008)
// Copyright (C) Gianluca Pollastri 2008
//
// BRNN with single softmax output (sequence to class mapping)
//


class NNto1 {
 private:
  int NU;
  int NY;
  int NH;

  int context;
  int Moore;

  int NF;
  int NH2;

  int CoF;

  NN* NetOut;

  NNt* NetF;

  Float** FF;
  Float** FFbp;

  Float* P_F;

  Float* Y;
  Float* BP;

  Float error;
  Float errorF;

  double epsilon;

  void alloc();


 public:

  NNto1(int NU, int NY, int NH,  int context ,int Moore,
	  int NF, int NH2, int CoF);
  NNto1(istream& is);
  void read(istream& is);
  void write(ostream& os);

  void resetGradient();
  void initWeights(int seed);


  void F1_F(Float* seq, int t, int length);
  void F1_F(int* seq, int t, int length);
  void propagate(Float* seq, int length);
  void propagate(int* seq, int length);


  void F1_Fbp(Float* seq, int t, int length, int backp=0);
  void F1_Fbp(int* seq, int t, int length, int backp=0);
  void back_propagate(Float* seq, int length, int backp=0);
  void back_propagate(int* seq, int length, int backp=0);


  void extimation(Float* seq, int y, int length, int backp=0);
  void extimation(int* seq, int y, int length, int backp=0);

  void extimation(Float* seq, Float* y, int length, int backp=0);
  void extimation(int* seq, Float* y, int length, int backp=0);

  void backthrough(Float* seq, Float* y, int length, int backp=0) {
    NetOut->set_output(0);
    extimation(seq,y,length,backp);
    NetOut->set_output(1);
  }
  void backthrough(int* seq, Float* y, int length, int backp=0) {
    NetOut->set_output(0);
    extimation(seq,y,length,backp);
    NetOut->set_output(1);
  }


  void resetBP(int length);


  void maximization();
  void maximizationL1();

  void Feed(Float* seq, int length);
  void Feed(int* seq, int length);
  void predict(Float* seq, int length);
  void predict(int* seq, int length);


  Float* out() {return Y;}
  Float* back_out() {return BP;}




  Float getError() {
	return error;
	};
  Float getErrorF() {
	return errorF;
	};
  void resetError() {
	error=0.0;
	errorF=0.0;
	};

  void setEpsilon(Float eps) { epsilon=eps; };


};


#endif // NNto1_h
