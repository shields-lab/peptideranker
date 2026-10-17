



#include "MModel.h"




void
MModel::alloc() {

	counted = new int[NY[0]];
	nerrors_ = new int[NY[0]];
	accuracy = new int[NY[0]];
	pred_y = new int[NY[0]];
	
	Conf=new int*[NY[0]];
	for (int y=0;y<NY[0];y++)
		Conf[y]=new int[NY[0]];

	Conf_Matrix=new int*[NY[0]];
	for (int y=0;y<NY[0];y++)
		Conf_Matrix[y]=new int[NY[0]];


}



MModel::MModel(istream& is) {
char fname[256];
filebuf inbuf;
	
is >> nModels;
Net=new NNto1*[nModels];

  NU=new int[nModels];
  NY=new int[nModels];
  NH=new int[nModels];

  context=new int[nModels];
  Moore=new int[nModels];

  NF=new int[nModels];
  NH2=new int[nModels];

  CoF=new int[nModels];

  Thresholds=new Float*[nModels];


for (int n=0;n<nModels; n++) {
  is>>fname;
  inbuf.open(fname, ios::in);
  istream is2(&inbuf);

  is2 >> NU[n]>>NY[n]>>NH[n]>>context[n];
  is2>>NF[n]>>NH2[n]>>CoF[n]>>Moore[n];
  Thresholds[n] = new Float[NY[n]];
  for (int y=0;y<NY[n]-1;y++) {
	is2 >> Thresholds[n][y];
  }

  Net[n] = new NNto1(is2);
  Net[n]->resetGradient();

  inbuf.close();
}

alloc();
}





void MModel::predict(Sequence* seq, char* alidir) {
	int n,c;
	Float* O=new Float[NY[0]];
	memset(O,0,NY[0]*sizeof(Float));

	for (n=0;n<nModels;n++) {

		if (alidir == NULL) {
			Net[n]->predict(seq->u,seq->length);

		} else {
			seq->generate_profile(alidir);
			Net[n]->predict(seq->HeP,seq->length);
		}

		for (c=0; c<NY[0]; c++) {
			O[c] += Net[n]->out()[c]/(double)nModels;
//			cout << Net[n]->out()[c]/(double)nModels << " ";
		}
//		cout << "\n";
	}


	Float pred=0.0;
	int argp=-1;

	for (c=0; c<NY[0]; c++) {
//				cout << O[c] << " ";
		seq->y_pred[c] = O[c];
		if (O[c]>pred) {
			pred = O[c];
			argp = c;
		}
	}
	seq->yc = argp;
//	cout << argp << "\n";

	if (seq->yc != seq->y) {
		nerrors++;
		nerrors_[seq->y]++;
		Conf[seq->y][seq->yc]++;
	}

	if (seq->yc == seq->y) {
		accuracy[seq->y]++;
	}


	counted[seq->y]++;
	pred_y[seq->yc]++;
	Conf_Matrix[seq->y][seq->yc]++;

	delete[] O;
}



void
MModel::predict_all(Sequence* seq, char* alidir) {
        int n,c;
        Float* O=new Float[NY[0]];
        memset(O,0,NY[0]*sizeof(Float));

        for (n=0;n<nModels;n++) {

                if (alidir == NULL) {
                        Net[n]->predict(seq->u,seq->length);

                } else {
                        seq->generate_profile(alidir,1);
                        Net[n]->predict(seq->HeP,seq->length);
                }

                for (c=0; c<NY[0]; c++) {
                        O[c] += Net[n]->out()[c]/(double)nModels;
//                      cout << Net[n]->out()[c]/(double)nModels << " ";
                }
//              cout << "\n";
        }


        Float pred=0.0;
        int argp=-1;

        for (c=0; c<NY[0]; c++) {
//                              cout << O[c] << " ";
                seq->y_pred[c] = O[c];
                if (O[c]>pred) {
                        pred = O[c];
                        argp = c;
                }
        }
        seq->yc = argp;
//      cout << argp << "\n" <<flush;

        delete[] O;

}



