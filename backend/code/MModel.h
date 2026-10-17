
#ifndef MModel_h
#define MModel_h 1
//changed by cm Wednesday 17 June 2009 
#include <iostream>
#include <fstream>
using namespace std;
using std::cout;

#include <stdlib.h>
//#include <iostream.h>
//#include <fstream.h>
#include <math.h>
#include "Sequence.h"
#include "NNto1.h"


class MModel {
 private:
  int nModels;
  int* NU;
  int* NY;
  int* NH;

  int* context;
  int* Moore; 

  int* NF;
  int* NH2;

  int* CoF;

  Float** Thresholds;


  int modular;


  NNto1** Net;

  int** Conf;
  int** Conf_Matrix;

  Float temp_error;
  int temp_aas;
  
  int* counted;
  int* pred_y;
  double squared_error;
  int nerrors;
  int* nerrors_;
  int* accuracy;

  	
  Float epsilon;

  void alloc();


 public:

  MModel(istream& is);

  void predict(Sequence* seq, char* alidir=NULL);
  void predict_all(Sequence* seq, char* alidir=NULL);


  int** getConf() {return Conf;}
  int** getConf_Matrix() {return Conf_Matrix;}

  int getNErrors() { return nerrors;};

  int getNErrors_(int i) { return nerrors_[i];};
  int getAccuracy(int i) { return accuracy[i];};
  int getClasses() { return NY[0];};

  int* getCounted() {return counted;}
  int getPred_y(int i) {return pred_y[i];}
  int getCounted(int i) {return counted[i];}


	void resetNErrors() { 
		nerrors=0;
		memset(nerrors_,0,NY[0]*sizeof(int));
		memset(counted,0,NY[0]*sizeof(int));
		memset(accuracy,0,NY[0]*sizeof(int));
		memset(pred_y,0,NY[0]*sizeof(int));
		
		for (int p=0;p<NY[0];p++){
			for (int y=0;y<NY[0];y++){
				Conf[p][y]=0;
				Conf_Matrix[p][y]=0;
			}
		}
	};


};


#endif // MModel_h
