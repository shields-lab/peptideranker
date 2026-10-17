
#ifndef BRNN_h
#define BRNN_h 1

#include <stdlib.h>
#include <math.h>
#include "General.h"
//#include "NNr.h"
#include "NN.h"
#include "NNt.h"

#define MAX 2500


// BRNNr ver. 3.01 (5/11/2003)
// Copyright (C) Gianluca Pollastri 2003
//
// BRNNr with linear output
// inputs real valued or categorical
//
// Version 3.0:
// -compatible with NN,NNt,NNr 3.0
// -can be used as module in larger architecture (see 'backthrough'
// and the 'backp' parameter)
// -full shortcuts now operative
// 



class BRNN {
 private:
  int NU;
  int NY;
  int NH;

  int context;
  int Moore;

  int NF;
  int NB;
  int NH2;

  int CoF;
  int CoB;
  int Step;
  int shortcut;
  int doubleo;
  int modular;


  NN* NetOut;
//  NN* NetOut2;
  NNt* NetF;
  NNt* NetB;

  Float** FF;
  Float** BB;
  Float** FFbp;
  Float** BBbp;

  Float* P_F;
  Float* P_B;

  Float* Y;
  Float* BP;

  Float error;
  Float errorF;
  Float errorB;

  double epsilon;

  void alloc();


 public:

  BRNN(int NU, int NY, int NH,  int context ,int Moore,
	  int NF, int NB, int NH2, int CoF, int CoB, int Step,
	  int shortcut, int doubleo=0);
  BRNN(istream& is);
  void read(istream& is);
  void write(ostream& os);

  void resetGradient();
  void initWeights(int seed);


  void F1_F(Float* seq, int t, int length);
  void F1_F(int* seq, int t, int length);
  void B1_B(Float* seq, int t, int length);
  void B1_B(int* seq, int t, int length);
  void propagate(Float* seq, int length);
  void propagate(int* seq, int length);

  void forward(Float* seq, int t, int length);
  void forward(int* seq, int t, int length);


  void F1_Fbp(Float* seq, int t, int length, int backp=0);
  void F1_Fbp(int* seq, int t, int length, int backp=0);
  void B1_Bbp(Float* seq, int t, int length, int backp=0);
  void B1_Bbp(int* seq, int t, int length, int backp=0);
  void back_propagate(Float* seq, int length, int backp=0);
  void back_propagate(int* seq, int length, int backp=0);


  void extimation(Float* seq, int* y, int length, int backp=0);
  void extimation(int* seq, int* y, int length, int backp=0);

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
  Float getErrorB() {
	return errorB;
	};
  void resetError() {
	error=0.0;
	errorF=0.0;
	errorB=0.0;
	};

  void setEpsilon(Float eps) { epsilon=eps; };


};


#endif // BRNN_h
