

#ifndef Sequence_h
#define Sequence_h 1
//changed by cm Wednesday 17 June 2009 
#include <iostream>
#include <fstream>
using namespace std;
using std::cout;

//#include <iostream.h>
#include <math.h>
#include "General.h"

//#define MAX_T 8196
#define MAX_T 30000 //cm





static int translateU[26] = {
  0,	// A
  -1,	// B
  1,	// C
  2,	// D
  3,	// E
  4,	// F
  5,	// G
  6,	// H
  7,	// I
  -1,	// J
  8,	// K
  9,	// L
  10,	// M
  11,	// N
  -1,	// O
  12,	// P
  13,	// Q
  14,	// R
  15,	// S
  16,	// T
  -1,	// U
  17,	// V
  18,	// W
//  -1,	// X
  20,	// X cm Wednesday 24 March 2010/Tuesday 06 July 2010  
  19,	// Y
  -1	// Z
};


static char Utranslate[26] = "ACDEFGHIKLMNPQRSTVWYX";
//static char Utranslate[26] = "ACDEFGHIKLMNPQRSTVWY";









class Sequence {
public:


char name[256];

int* u;
Float* y_pred;
int yc;
int y;

int length;

  int alignments_loaded;
  int** ALIGNMENTS;

  Float* HeP;
  int HePl;



Sequence(istream& is) {

  int i;
  char c;
  HePl=0;

 
  is >> name;
  is >> length;

//cout << name << " " << length << "\n" << flush;

  u = new int[length+1];
  y_pred = new Float[16];
  memset(y_pred,0,16*sizeof(Float));

  for (i=1;i<length+1;i++) {
    is >> c;
    u[i] = c-'A';
//	if (u[i]<0 || u[i]>26) u[i]=-1;
	u[i] = translateU[u[i]];
  }
  is >> y;
  y--;
}





int load_alignments(char* alidir, int dir=0)
{
	filebuf inbuf;
	char temp[MAX_T];
	char fname[1024];

	//cout << name << " " << alidir << "\n" << flush;

	if (dir==0) {
		strcpy(fname, alidir);
		strcat(fname, name);
		strcat(fname, ".flatblast");
	} else {
		strcpy(fname, alidir);
	}


	if (inbuf.open(fname, ios::in) != 0) {
		istream is(&inbuf);
		is >> alignments_loaded;

		//cout << fname << " " << alignments_loaded << "\n" << flush;

		if (alignments_loaded) {
			//cout << fname << " ALIGNMENT\n" << flush;
			ALIGNMENTS=new int*[alignments_loaded];
			//cout << "2" << "\n" << flush;
		}
		else{
			//cout << fname << " NO ALIGNMENT\n" << flush;
		}
		for (int quanti=0;quanti<alignments_loaded;quanti++){

			//cout << quanti << " ";
			is>>temp;
			int ali_length = strlen(temp);
			//cout << ali_length << " " << flush;
			int maxa = length;
			
			if (ali_length>maxa) maxa=ali_length;
			//cout << maxa << " \n" << flush;
			ALIGNMENTS[quanti]=new int[maxa+1];
			ALIGNMENTS[quanti][0]=ali_length;
			for (int i=1;i<=ali_length;i++)
			{
				if (temp[i-1]=='.' || temp[i-1]=='-') ALIGNMENTS[quanti][i]=20;
				else ALIGNMENTS[quanti][i]=translateU[temp[i-1]-'A'];

				if (ALIGNMENTS[quanti][i]<0 || ALIGNMENTS[quanti][i]>19) {
					ALIGNMENTS[quanti][i] = 20;
				}
			}

		}
	} else {
		alignments_loaded=0; 
		//cout << fname << " NO ALIGNMENT\n" << flush;
	}
	inbuf.close();
	return alignments_loaded;
}

int unload_alignments()
	{
	if (alignments_loaded) {
	  for (int quanti=0;quanti<alignments_loaded;quanti++)
		delete[] ALIGNMENTS[quanti];
	  delete[] ALIGNMENTS;
	  alignments_loaded = 0;
	  return 1;
	  }
	else return 0;
	}




Float profile_entropy() {
Float sum=0;
for (int t=1;t<=length;t++) {
	for (int aa=0;aa<21;aa++) {
		if (HeP[21*t+aa])
			sum -= HeP[21*t+aa]*(Float)log(HeP[21*t+aa]);
		if (HeP[21*t+aa]<0 || HeP[21*t+aa]>1) {
			cout << HeP[21*t+aa] << " " << aa << " " << t <<" "<< length<<"\n" << flush;
			}
		}
	}
return sum;
}


int generate_profile(char* alidir, int dir=0) {

    if (HePl) return 1;

	Float* Pres= new Float[length+1];
	HeP = new Float[21*(length+1)];


	memset(HeP,0,21*(length+1)*sizeof(Float));
	memset(Pres,0,(length+1)*sizeof(Float));
	int t, a;

// Add the main sequence to HeP
	for (t=1;t<=length;t++) {
		int aa = u[t];
		if (aa >=0 && aa <21) {
			HeP[21*t+aa]++;
			Pres[t]++;
			}
	   	}

      load_alignments(alidir,dir);
		int offset=0;
      int AL= alignments_loaded;

  if (alignments_loaded>0) {
	//cout << "ALIGNMENT loaded\n" << flush;
	
	for (a=0;a<AL;a++) {
	  for (t=1;t<=length;t++) {
		int aa = ALIGNMENTS[a][t];
		if (aa >=0 && aa <21) {
			HeP[21*t+aa]++;
			Pres[t]++;
	   	}
	  }
	}
	for (t=1;t<=length;t++) {
		Float total=0.0;
		Float totalg=0.0;
		int aa;
		for (aa=0;aa<20;aa++) {
//			HeP[21*t+aa] += belief*frequencies[aa];
			total += HeP[21*t+aa];
			totalg += HeP[21*t+aa];
		}
		totalg += HeP[21*t+20];
		for (aa=0;aa<21;aa++) {
			if (total)
				HeP[21*t+aa] /= total;
//			cout << HeP[21*t+aa] << " ";
			}
//		HeP[21*t+20] = 0;
		HeP[21*t+20] /= totalg;
//		cout << "\n" << flush;
		}

Float* weights=new Float[AL];

//cout << profile_entropy() << "\n" << flush;
//cout << "\n" << flush;

for (int iter=0;iter<1;iter++) {

// We can now think to weight each sequence based on its information content

memset(weights, 0, AL*sizeof(Float));

for (a=0;a<AL;a++) {

	Float sum=0;
    for (t=1;t<=length;t++) {
		int aa = ALIGNMENTS[a][t];
		if (aa >=0 && aa <20) {
			sum -= (Float)log(HeP[21*t+aa]);
		}
    }
	sum /= (Float)length;
	weights[a]=sum;
}



// Now we recompute the profile matrix

memset(HeP,0,21*(length+1)*sizeof(Float));

for (t=1;t<=length;t++) {
	int aa = u[t];
	if (aa >=0 && aa <21) {
		HeP[21*t+aa]++;
		}
   	}

for (a=0;a<AL;a++) {
	if (1) {
		for (t=1;t<=length;t++) {
			int aa = ALIGNMENTS[a][t];
				if (aa >=0 && aa <21) {
					HeP[21*t+aa] += weights[a];
				}
	   		}
		}
	}

	for (t=1;t<=length;t++) {
		Float total=0.0;
		Float totalg=0.0;
		int aa;
		for (aa=0;aa<20;aa++) {
//			HeP[21*t+aa] += belief*frequencies[aa];
			total += HeP[21*t+aa];
			totalg += HeP[21*t+aa];
			}
		totalg += HeP[21*t+20];
		for (aa=0;aa<21;aa++) {
			if (total)
				HeP[21*t+aa] /= total;
//			cout << HeP[21*t+aa] << " ";
			}
//		HeP[21*t+20] = 0;
		HeP[21*t+20] /= totalg;
//		cout << "\n" << flush;
		}

//cout << profile_entropy() << "\n" << flush;
}

//cout << "\n"<<flush;

delete[] weights;

}
else{
//cout << " NO ALIGNMENT\n" << flush;
}
unload_alignments();
HePl=1;

delete[] Pres;


return 0;

}













void unload_profile() {
	HePl = 0;
	delete[] HeP;
	}

















void write(ostream& os) {
  int i;

  os<<name<<"\n";
  os<<length<<"\n";


  for (i=1;i<length+1;i++) {
    os << (char)(Utranslate[u[i]]);
  }
  os << "\n";

  os << y+1 << " " << yc+1;
  os << "\n";



}

void write_predictions(ostream& os, int classes=0) {
  int i;

//  os<<name<<"\n";
  os<<length<<"\n";


  for (i=1;i<length+1;i++) {
    os << (char)(Utranslate[u[i]]);
  }
  os << "\n";

//  os << y+1 << " " << yc+1;
  os << yc+1;
  os << "\n";

 for (i=0;i<classes;i++) {
        os << y_pred[i] << " ";
 }
os << "\n";

}

void write(ostream& os, int classes) {
  int i;

  os<<name<<"\n";
  os<<length<<"\n";


  for (i=1;i<length+1;i++) {
    os << (char)(Utranslate[u[i]]);
  }
  os << "\n";

  os << y+1 << " " << yc+1;
//  os << yc+1;
  os << "\n";

 for (i=0;i<classes;i++) {
  	os << y_pred[i] << " ";
 }
os << "\n";
}





};
















class DataSet {
public:
  int length;
  Sequence** seq;
  int totSize;

  DataSet() {};

  DataSet(int the_length) {
	totSize=0;
	length=the_length;
	seq = new Sequence*[length];
  }

  DataSet(istream& is, int quot=0)
    {
	  totSize=0;
      is >> length;
      seq = new Sequence*[length];
      for (int p=0; p<length; p++) {
		seq[p] = new Sequence(is);
		totSize += seq[p]->length;
      }
    };

  void write(ostream& os)
    {
      os << length << "\n";
      for (int p=0; p<length; p++) {
	seq[p]->write(os);
      }
    };
  void write(ostream& os, int classes)
    {
      os << length << "\n";
      for (int p=0; p<length; p++) {
	seq[p]->write(os,classes);
      }
    };
  void write(char* fname)
    {
      filebuf outbuf;
      if (outbuf.open(fname, ios::out) != 0) {
	ostream os(&outbuf);
	this->write(os);
      } else {
//	FAULT("Failed to write to file " << fname);
      }
      outbuf.close();
    };

  void write(char* fname, int classes)
    {
      filebuf outbuf;
      if (outbuf.open(fname, ios::out) != 0) {
	ostream os(&outbuf);
	this->write(os,classes);
      } else {
//	FAULT("Failed to write to file " << fname);
      }
      outbuf.close();
    };



  void write_predictions(ostream& os, int classes=0)
    {
//      os << length << "\n";
      for (int p=0; p<length; p++) {
        seq[p]->write_predictions(os,classes);
      }
    };

  void write_predictions(char* fname, int classes=0)
    {
      filebuf outbuf;
      if (outbuf.open(fname, ios::out) != 0) {
        ostream os(&outbuf);
        this->write_predictions(os,classes);
      } else {
//      FAULT("Failed to write to file " << fname);
      }
      outbuf.close();
    };

};



#endif // Sequence_h
