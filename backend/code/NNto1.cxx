
#include "NNto1.h"





void
NNto1::alloc() {
int t;

FF = new Float*[MAX];
FFbp = new Float*[MAX];

P_F = new Float[NF];

for (t=0;t<MAX;t++) {
	FF[t] = new Float[NF];
	FFbp[t] = new Float[NF];
	}

Y=new Float[NY]; // Note difference wrt NN here
BP=new Float[MAX*NU];

for (int f=0;f<NF;f++)
	P_F[f]=0;
}



NNto1::NNto1(int the_NU, int the_NY, int the_NH, int the_context, int the_Moore, int the_NF, int the_NH2, int the_CoF) :
NU(the_NU), NY(the_NY), NH(the_NH), context(the_context), Moore(the_Moore), NF(the_NF), NH2(the_NH2), CoF(the_CoF)
{

int NK[8196];

for (int c=0;c<2*context+1;c++) {
	NK[c]=NU;
}
NK[2*context+1]=1;
NK[2*context+2]=1;


if (Moore)
	NetOut = new NN(1, NF, NH, NY, NK);
else
	NetOut = new NN(0, NF, NH, NY, NK);


	NetF = new NNt((2*context+1)+2, 0, NH2, NF, NK, 0);

NetOut->set_input(1);
NetF->set_input(1);

NetOut->set_output(1);
NetF->set_output(0);

alloc();
resetGradient();
}





NNto1::NNto1(istream& is) {
is >> NU >> NY >> NH >> context >> Moore;
is >> NF >> NH2 >> CoF;

NetOut = new NN(is);
NetF = new NNt(is);

NetOut->set_input(1);
NetF->set_input(1);

NetOut->set_output(1);
NetF->set_output(0);

alloc();
resetGradient();
}





void
NNto1::read(istream& is) {
is >> NU >> NY >> NH >> context >> Moore;
is >> NF >> NH2 >> CoF;

NetOut->read(is);
NetF->read(is);

NetOut->set_input(1);
NetF->set_input(1);

Moore=NetOut->get_NI();
if (Moore) Moore=1;
}




void
NNto1::write(ostream& os) {
os << NU << " " << NY << " " << NH << " " << context << " " << Moore << "\n";
os <<NF<<" "<<NH2<<" "<<CoF<<"\n";

NetOut->write(os);
NetF->write(os);
}


void
NNto1::resetGradient() {
NetOut->resetGradient();
NetF->resetGradient();
}

void
NNto1::initWeights(int seed) {
NetOut->initWeights(seed++);
NetF->initWeights(seed++);
}



void
NNto1::F1_F(Float *seq, int t, int length) {

Float I[8196];
int f,s,c;

  for (c=-context; c<=context; c++) {
    if (t+c <= 0 || t+c > length) {
	for (int i=0;i<NU;i++)
		I[NU*(context+c)+i] = 0.0;
      }
    else {
	for (int i=0;i<NU;i++)
		I[NU*(context+c)+i] = seq[NU*(t+c)+i];
      }
    }
I[NU*(2*context+1)] = 0.001*double(t);
I[NU*(2*context+1)+1] = 0.001*double(length-t);

Float* X=new Float[8];

NetF->forward(I,X);
//cout << "F1_F 1 NF: " << NF << "\n"<<flush;

for (f=0; f<NF; f++) {
//cout << "F1_F 1 t: " << t << " f: " << f << "\n"<<flush;

	FF[t][f] = NetF->out()[f];
	}
	
//cout << "F1_F 2\n"<<flush;

delete[] X;
}


void
NNto1::F1_F(int *seq, int t, int length) {

int I[8196];
int f,s,c;

  for (c=-context; c<=context; c++) {
    if (t+c <= 0 || t+c > length) {
		I[context+c] = -1;
      }
    else {
		I[context+c] = seq[t+c];
      }
    }
Float* X=new Float[8];

NetF->forward(I,X);
for (f=0; f<NF; f++) {
	FF[t][f] = NetF->out()[f];
	}
delete[] X;
}





void
NNto1::propagate(Float* seq, int length) {
int T=length;
int t,f;

for (f=0; f<NF; f++)
	FF[0][f] = P_F[f];

for (t=1; t<=T; t++)
	F1_F(seq,t,T);
}

void
NNto1::propagate(int* seq, int length) {
int T=length;
int t,f;

for (f=0; f<NF; f++)
	FF[0][f] = P_F[f];

for (t=1; t<=T; t++)
	F1_F(seq,t,T);
}




void
NNto1::F1_Fbp(Float *seq, int t, int length, int backp) {
Float I[8196];
int f,s,c;

  for (c=-context; c<=context; c++) {
    if (t+c <= 0 || t+c > length) {
	for (int i=0;i<NU;i++)
		I[NU*(context+c)+i] = 0.0;
      }
    else {
	for (int i=0;i<NU;i++)
		I[NU*(context+c)+i] = seq[NU*(t+c)+i];
      }
    }
I[NU*(2*context+1)] = 0.001*double(t);
I[NU*(2*context+1)+1] = 0.001*double(length-t);
Float* X=new Float[8];

NetF->forward(I,X);

errorF+=NetF->backward(FFbp[t]);

if (backp) {
  for (c=-context; c<=context; c++) {
    if (t+c > 0 && t+c <= length) {
	for (int i=0;i<NU;i++)
		BP[NU*(t+c)+i]+=NetF->back_out()[NU*(context+c)+i];
      }
    }
}
NetF->gradient(I,X,FFbp[t]);

delete[] X;
}

void
NNto1::F1_Fbp(int *seq, int t, int length, int backp) {
int I[8196];
int f,s,c;

  for (c=-context; c<=context; c++) {
    if (t+c <= 0 || t+c > length) {
		I[context+c] = -1;
      }
    else {
		I[context+c] = seq[t+c];
      }
    }
Float* X=new Float[8];

NetF->forward(I,X);

errorF+=NetF->backward(FFbp[t]);

if (backp) {
  for (c=-context; c<=context; c++) {
    if (t+c > 0 && t+c <= length) {
	for (int i=0;i<NU;i++)
		BP[NU*(t+c)+i]+=NetF->back_out()[NU*(context+c)+i];
      }
    }
}
NetF->gradient(I,X,FFbp[t]);

delete[] X;
}








void
NNto1::back_propagate(Float* seq, int length, int backp) {
int T=length;
int t;

for (t=T; t>0; t--) {
	F1_Fbp(seq,t,T,backp);
	}
}

void
NNto1::back_propagate(int* seq, int length, int backp) {
int T=length;
int t;

for (t=T; t>0; t--) {
	F1_Fbp(seq,t,T,backp);
	}
}





void
NNto1::extimation(Float *seq, int y, int length, int backp) {

double norma = 30.0;
	
// If need backprop through..
if (backp) {
  NetOut->set_input(2);
  NetF->set_input(2);
  memset(BP,0,MAX*NU*sizeof(Float));
}

int t,f,v,c,o;
Float I[8192];
memset(I,0,8192*sizeof(Float));
Float* target=new Float[1024];
Float* X=new Float[NF];
memset(X,0,(NF)*sizeof(Float));

propagate(seq,length);


for (t=0;t<length+2;t++) {
	memset(FFbp[t],0,NF*sizeof(Float));
}

for (t=1; t<=length; t++) {

// Compute inputs for gradient
if (Moore) {
	for (int i=0;i<NU;i++)
		I[i] +=  seq[NU*t+i]/(double)length;
  }

// We now set the hidden inputs
    for (f=0;f<NF;f++)
	X[f] += FF[t][f]/(double)norma;
}

memset(target,0,NY*sizeof(Float));
target[y]=1.0;
NetOut -> forward(I,X);
for (o=0;o<NY;o++) {
	Y[o]=NetOut->out()[o];
}

error += NetOut->backward(target);
NetOut->gradient(I,X,target);


for (t=1; t<=length; t++) {

    for (f=0;f<NF;f++)
	FFbp[t][f] += NetOut->back_out()[Moore*NU + f]/(double)norma;

if (backp && Moore) {
	for (int i=0;i<NU;i++)
		BP[NU*t+i] += NetOut->back_out()[i]/(double)norma;
} // if backp
  
} // for t


back_propagate(seq,length,backp);
NetOut->set_input(1);
NetF->set_input(1);

delete[] target;
delete[] X;


}




void
NNto1::extimation(int *seq, int y, int length, int backp) {

double norma = 30.0;
	
// If need backprop through..
if (backp) {
  NetOut->set_input(2);
  NetF->set_input(2);
  memset(BP,0,MAX*NU*sizeof(Float));
}

int t,f,v,c,o;
Float I[8192];
memset(I,0,8192*sizeof(Float));
Float* target=new Float[1024];
Float* X=new Float[NF];
memset(X,0,(NF)*sizeof(Float));

propagate(seq,length);

for (t=0;t<length+2;t++) {
	memset(FFbp[t],0,NF*sizeof(Float));
}

for (t=1; t<=length; t++) {

// Compute inputs for gradient
if (Moore) {
		I[seq[t]] +=  1.0/(double)length;
}

// We now set the hidden inputs
    for (f=0;f<NF;f++)
	X[f] += FF[t][f]/(double)norma;
}

memset(target,0,NY*sizeof(Float));
target[y]=1.0;
NetOut -> forward(I,X);
for (o=0;o<NY;o++) {
	Y[o]=NetOut->out()[o];
}

error += NetOut->backward(target);
NetOut->gradient(I,X,target);


for (t=1; t<=length; t++) {

    for (f=0;f<NF;f++)
	FFbp[t][f] += NetOut->back_out()[Moore*NU + f]/(double)norma;

if (backp && Moore) {
	for (int i=0;i<NU;i++)
		BP[NU*t+i] += NetOut->back_out()[i]/(double)norma;
} // if backp
  
} // for t

back_propagate(seq,length,backp);
NetOut->set_input(1);
NetF->set_input(1);

delete[] target;
delete[] X;
} 







void
NNto1::extimation(Float *seq, Float* y, int length, int backp) {

double norma = 30.0;
	
// If need backprop through..
if (backp) {
  NetOut->set_input(2);
  NetF->set_input(2);
  memset(BP,0,MAX*NU*sizeof(Float));
}

int t,f,v,c,o;
Float I[8192];
memset(I,0,8192*sizeof(Float));

Float* X=new Float[NF];
memset(X,0,(NF)*sizeof(Float));

propagate(seq,length);

for (t=0;t<length+2;t++) {
	memset(FFbp[t],0,NF*sizeof(Float));
}

for (t=1; t<=length; t++) {

// Compute inputs for gradient
if (Moore) {
	for (int i=0;i<NU;i++)
		I[i] +=  seq[NU*t+i]/(double)length;
  }

// We now set the hidden inputs
    for (f=0;f<NF;f++)
	X[f] += FF[t][f]/(double)norma;
}

NetOut -> forward(I,X);
for (o=0;o<NY;o++) {
	Y[o]=NetOut->out()[o];
}

error += NetOut->backward(y);
NetOut->gradient(I,X,y);


for (t=1; t<=length; t++) {

    for (f=0;f<NF;f++)
	FFbp[t][f] += NetOut->back_out()[Moore*NU + f]/(double)norma;

if (backp && Moore) {
	for (int i=0;i<NU;i++)
		BP[NU*t + i] += NetOut->back_out()[i]/(double)norma;
} // if backp
  
} // for t

back_propagate(seq,length,backp);
NetOut->set_input(1);
NetF->set_input(1);

delete[] X;
}




void
NNto1::extimation(int *seq, Float* y, int length, int backp) {

double norma = 30.0;
	
// If need backprop through..
if (backp) {
  NetOut->set_input(2);
  NetF->set_input(2);
  memset(BP,0,MAX*NU*sizeof(Float));
}

int t,f,v,c,o;
Float I[8192];
memset(I,0,8192*sizeof(Float));

Float* X=new Float[NF];
memset(X,0,(NF)*sizeof(Float));

propagate(seq,length);

for (t=0;t<length+2;t++) {
	memset(FFbp[t],0,NF*sizeof(Float));
}

for (t=1; t<=length; t++) {

// Compute inputs for gradient
if (Moore) {
		I[seq[t]] +=  1.0/(double)length;
}

// We now set the hidden inputs
    for (f=0;f<NF;f++)
	X[f] += FF[t][f]/(double)norma;
}

NetOut -> forward(I,X);
for (o=0;o<NY;o++) {
	Y[o]=NetOut->out()[o];
}
error += NetOut->backward(y);
NetOut->gradient(I,X,y);

for (t=1; t<=length; t++) {

    for (f=0;f<NF;f++)
	FFbp[t][f] += NetOut->back_out()[Moore*NU + f]/(double)norma;

if (backp && Moore) {
	for (int i=0;i<NU;i++)
		BP[NU*t + i]+=NetOut->back_out()[i]/(double)norma;
} // if backp
  
} // for t

back_propagate(seq,length,backp);
NetOut->set_input(1);
NetF->set_input(1);

delete[] X;
}
































void
NNto1::resetBP(int length) {
for (int t=0;t<length+2;t++) {
	memset(FFbp[t],0,NF*sizeof(Float));
	}
}






















void
NNto1::maximization() {

NetOut->updateWeights(epsilon);
NetOut->resetGradient();

NetF->updateWeights(epsilon);
NetF->resetGradient();
}


void
NNto1::maximizationL1() {

NetOut->updateWeightsL1(epsilon);
NetOut->resetGradient();

NetF->updateWeightsL1(epsilon);
NetF->resetGradient();
}



void
NNto1::Feed(Float* seq, int length) {

double norma = 30.0;

int t,f,b,v,c,o;
Float I[8192];
memset(I,0,8192*sizeof(Float));
Float* X=new Float[NF];
memset(X,0,(NF)*sizeof(Float));


//cout << "Feed\n";

propagate(seq,length);

for (t=1; t<=length; t++) {


// Compute inputs for gradient
if (Moore) {
	for (int i=0;i<NU;i++) {
		I[i] +=  seq[NU*t+i]/(double)length;
	}
  }

// We now set the hidden inputs
    for (f=0;f<NF;f++)
	X[f] += FF[t][f]/(double)norma;
}

//if (Moore) {
//	for (int i=0;i<NU;i++) cout << I[i] << " ";
//	cout << "\n";
//}

NetOut -> forward(I,X);
for (o=0;o<NY;o++) {
	Y[o]=NetOut->out()[o];
}

delete[] X;
}






void
NNto1::Feed(int* seq, int length) {

double norma = 30.0;
int t,f,b,v,c,o;
Float I[8192];
memset(I,0,8192*sizeof(Float));
Float* X=new Float[NF];
memset(X,0,(NF)*sizeof(Float));



propagate(seq,length);

for (t=1; t<=length; t++) {


// Compute inputs for gradient
if (Moore) {
  for (c=-context; c<=context; c++) {
    if (t+c <= 0 || t+c > length) {
      }
    else {
		I[NU*(context+c)+seq[t+c]] +=  1.0/(double)length;
    }
  }
}

// We now set the hidden inputs
    for (f=0;f<NF;f++)
	X[f] += FF[t][f]/(double)norma;
}

NetOut -> forward(I,X);
for (o=0;o<NY;o++) {
	Y[o]=NetOut->out()[o];
}
delete[] X;
}





void
NNto1::predict(Float* seq, int length) {

Feed(seq,length);
}

void
NNto1::predict(int* seq, int length) {

Feed(seq,length);
}

