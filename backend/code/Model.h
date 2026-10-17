
#ifndef Model_h
#define Model_h 1

#include <stdlib.h>
#include <math.h>
#include "Sequence.h"
#include "NNto1.h"


class Model {
 private:
  int NU;
  int NY;
  int NH;

  int context;
  int Moore; 

  int NF;
  int NH2;

  int CoF;

  Float* Thresholds;


  int modular;


  NNto1* Net;

  int** Conf;

//  Float temp_error;
  int temp_aas;
  
  int* counted;
  double squared_error;
  double error;
  int nerrors;
  int* nerrors_;

  Float epsilon;

  void alloc();


 public:

  Model(int NY, int NH,  int context ,int Moore, int NF, int NH2, int CoF, 
        Float* Thresholds);
  Model(istream& is);
  void read(istream& is);
  void write(ostream& os);

  void randomize(int seed);


  void extimation(Sequence* seq, char* alidir=NULL);
  void maximization();
  void maximizationL1();

  void predict(Sequence* seq, char* alidir=NULL);

  Float* out() {return Net->out();}
  int** getConf() {return Conf;}

  int getNErrors() { return nerrors;};

  int getNErrors_(int i) { return nerrors_[i];};
  int getClasses() { return NY;};

  int* getCounted() {return counted;}


   void resetNErrors() { 
	error=0;
	nerrors=0;
	memset(nerrors_,0,NY*sizeof(int));
	memset(counted,0,NY*sizeof(int));
	for (int p=0;p<NY;p++)
	  for (int y=0;y<NY;y++)
		Conf[p][y]=0;
	Net->resetError();
	};

  Float get_error() { 
	return error;
	};
  Float get_squared_error() { 
	return Net->getError();
	};
  Float get_squared_errorF() { 
	return Net->getErrorF();
	};
  void reset_squared_error() { 
	Net->resetError();
	};

  void setEpsilon(Float eps) { 
	  Net->setEpsilon(eps);
  };


};


#endif // Model_h
