
#include <math.h>

#include "Sequence.h"
#include "MModel.h"





void
evaluate(MModel* M, DataSet& D, char* which, char* alidir)
{
	int y;

  //cout << "\n counting_" << which << "_errors" << flush;
  M->resetNErrors();
  int p;
  for (p=0; p<D.length; p++) {
if (strncmp(which,"test",4)==0)
    M->predict(D.seq[p],alidir);
else
    M->predict(D.seq[p],alidir);
    //if (p%20==0) cout << "." << flush;
  }

double a[128];
double all=0;

for (y=0;y<M->getClasses();y++) {
	a[y]=M->getCounted()[y];
	all += a[y];
}

  //cout << "\n\n" << which <<"_NErrors= " << M->getNErrors() << "/" << all;
  //cout << " " << (double)M->getNErrors()/(double)(all)*100.0;
  //cout << " " << 100-(double)M->getNErrors()/(double)(all)*100.0;

for (y=0;y<M->getClasses();y++) {
  //cout << "\nClass" << y << "= " << M->getNErrors_(y) << "/" << a[y];
  //cout << "\t" << (double)M->getNErrors_(y)/(double)a[y]*100.0;
  //cout << "\t" << 100-(double)M->getNErrors_(y)/(double)a[y]*100.0;
}

  //cout<<y_pred[0]<<"\t"<<y_pred[1]\n";

}







int
main(int argc, char** argv)
{

// This is for predicting


  if (argc<4) {
    cerr << "Usage: " << argv[0] << " model_file protein_file alignment_folder\n";
    exit(1);
  }

    char model[256];
    char prot[256];
    char alidir[256];
	char alig[256];
	strcpy(model,argv[1]);
	strcpy(prot,argv[2]);
	strcpy(alidir,argv[3]);


    MModel* M;
    char tmp[1024];
    sprintf(tmp, model);
    ifstream mstream(tmp);
    M = new MModel(mstream);

  //cout << "Reading " << prot << " .. ";
  ifstream tstream(prot);
  DataSet T(tstream);

//cout << "read\n" << flush;

evaluate(M, T, prot, alidir);
strcat(prot,".predictions");
//T.write(prot);
T.write(prot,M->getClasses());



	return 0;
}


