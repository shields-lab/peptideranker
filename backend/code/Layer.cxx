
// Layer ver 3.03
// 12/12/2003
// Copyright (C) Gianluca Pollastri 2003




#include "Layer.h"
#include <stdlib.h>

#define miny 1e-4

#define momentum 0.9

void 
Layer::softmax() {
  int y;

  int overflow=0;
  Float max=A[0];
  int amax=0;
  Float norm=0;
  for (y=0; y<NY; y++) {
    if (A[y]>85) {
      overflow=1;
      }
    else {
      norm += (Float)exp(A[y]);
      }
    if (A[y]>max) {
      max = A[y];
      amax = y;
      }
    }

  if (overflow) {
    for (y=0; y<NY; y++) {
	Y[y] = miny;
    }
    Y[amax]=1.0-miny*(NY-1);
  } else {
    for (y=0; y<NY; y++) {
      Y[y] = (Float)exp(A[y])/norm;
    }
  }
  for (y=0; y<NY; y++) {
    if (Y[y]<miny) {
	Y[y] = miny;
    }
  }
}



void
Layer::squash() {
for (int y=0; y<NY; y++)
	Y[y]=(Float)tanh(A[y]);
}






void
Layer::alloc(int NY, int nu, int* NK)
{
int y,u;
NUtot=0;

for (u=0; u<nu; u++)
	NUtot += NK[u];

Y=new Float[NY];
A=new Float[NY];
U=new Float[NUtot];

delta=new Float[NY];
backprop=new Float[NUtot];

W=new Float**[NY];
dW=new Float**[NY];
d2W=new Float**[NY];
B=new Float[NY];
dB=new Float[NY];
d2B=new Float[NY];
memset(d2B,0,NY*sizeof(Float));
for (y=0; y<NY; y++) {
	W[y]=new Float*[nu];
	dW[y]=new Float*[nu];
	d2W[y]=new Float*[nu];
	for (u=0; u<nu; u++) {
		W[y][u]=new Float[NK[u]];
		dW[y][u]=new Float[NK[u]];
		d2W[y][u]=new Float[NK[u]];
		memset(d2W[y][u],0,NK[u]*sizeof(Float));
		}
	}

}




Layer::Layer(istream& is)
{
int y,u,k;

is >> NY;
is >> NU;
is >> NUr;
NK=new int[NU+NUr];

for (u=0; u<NU+NUr; u++) is >> NK[u];

alloc(NY,NU+NUr,NK);

for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      is >> W[y][u][k];
      }
    }
  is >> B[y];
  }
ninput=0;
output=0;


NUplain =0;
for (u=0; u<NU; u++) {
  NUplain += NK[u];
}

}



void
Layer::read(istream& is)
{
int y,u,k;

is >> NY;
is >> NU;
is >> NUr;

for (u=0; u<NU+NUr; u++) is >> NK[u];

for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      is >> W[y][u][k];
      }
    }
  is >> B[y];
  }
ninput=0;
output=0;

NUplain =0;
for (u=0; u<NU; u++) {
  NUplain += NK[u];
}
}





void Layer::write(ostream& os)
{
int y,u,k;

os << NY << "\n";
os << NU << "\n";
os << NUr << "\n";

for (u=0; u<NU+NUr; u++)
	os << NK[u] << " ";
os << "\n";

for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      os << W[y][u][k] << " ";
      }
    }
  os << B[y] << "\n";
  }
}




void
Layer::forward(int* I)
{
int y,u,nur;
memset(U,0,NUtot*sizeof(Float));

  for (y=0; y<NY; y++) {
    Float a=B[y];
	nur=0;
    for (u=0; u<NU; u++) {
		if (I[u]>=0) {
			a += W[y][u][I[u]];
			U[nur+I[u]]=1.0;
		}
		nur += NK[u];
	}
	Y[y]=A[y]=a;
  }
}




void
Layer::forward(Float* I)
{
int y,u,k,i;
memset(U,0,NUtot*sizeof(Float));

  for (y=0; y<NY; y++) {
    i=0;
    Float a=B[y];
    for (u=0; u<NU; u++) {
      for (k=0; k<NK[u]; k++) {
        U[i]=I[i];
        a += W[y][u][k]*I[i++];
        }
      }
    Y[y]=A[y]=a;
    }
}


void
Layer::forward(int* I1, Float* I2)
{
int y,u,k,i,nur;
memset(U,0,NUtot*sizeof(Float));


  for (y=0; y<NY; y++) {
    Float a=B[y];
	nur=0;

    for (u=0; u<NU; u++) {
		if (I1[u]>=0) {
			a += W[y][u][I1[u]];
			U[nur+I1[u]]=1.0;
		}
		nur += NK[u];
	}

	i=0;
    for (u=NU; u<NU+NUr; u++) {
      for (k=0; k<NK[u]; k++) {
        U[NUplain+i]=I2[i];
        a += W[y][u][k]*I2[i++];
        }
      }
    Y[y]=A[y]=a;
    }
}

void
Layer::forward(Float* I1, Float* I2)
{
int y,u,k,i1,i2;

  for (y=0; y<NY; y++) {
    i1=0;
    i2=0;
    Float a=B[y];
    for (u=0; u<NU; u++) {
		for (k=0; k<NK[u]; k++) {
			U[i1]=I1[i1];
			a += W[y][u][k]*I1[i1++];
		}
      }
    for (u=NU; u<NU+NUr; u++) {
      for (k=0; k<NK[u]; k++) {
        U[NUplain+i2]=I2[i2];
        a += W[y][u][k]*I2[i2++];
        }
      }
    Y[y]=A[y]=a;
    }
}

Float
Layer::f1(int y)
{
return 1.0;
}







Float
Layer::f_cost(Float* t)
{
Float sum=0.0;
//	cout << "L"<<flush;

for (int y=0; y<NY; y++)
	sum += (t[y]-Y[y])*(t[y]-Y[y]);
return sum;
}






Float
Layer::log_cost(Float* t)
{
Float sum=0.0;

for (int y=0; y<NY; y++) {
   if ((t[y]) && (Y[y]))
	sum -= t[y]*(Float)log(Y[y]);
  }
return sum;
}




Float
Layer::sq_cost(Float* t)
{
Float sum=0.0;

for (int y=0; y<NY; y++) {
	sum += (t[y]-Y[y])*(t[y]-Y[y]);
  }
return sum;
}





Float
Layer::backward(Float* rbackprop, Float weight)
{
int y,u,k;

Float BKD[1024];
  for (y=0; y<NY; y++)
	BKD[y]=rbackprop[y];

// If isn't an output layer
// rbackprop[] is a backprop contribution
// coming from upwards.
if (!output) {
  for (y=0; y<NY; y++) {
    BKD[y] *= f1(y);
    delta[y]=weight*BKD[y];
  }
}
// If this is an output layer
// rbackprop[] is the target vector.
else {
  for (y=0; y<NY; y++) {
    delta[y]=weight*(Y[y]-BKD[y])*f1(y);
    }
  }

Float sum;
int i=0;

// If this isn't an input layer
// the backprop contribution
// must be computed, either for
// the real input part or fully.
if (ninput==1) {
//	cout << "1 " << NU << " " << NUr << flush;
  for (u=NU; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      sum=0.0;
      for (y=0; y<NY; y++) {
		  sum += W[y][u][k]*delta[y];
        }
      backprop[NUplain+i]=sum;
	  i++;
      }
    }
  }
else if (ninput==2) {
//	cout << "2 " << NU << " " << NUr << flush;
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      sum=0.0;
      for (y=0; y<NY; y++) {
		  sum += W[y][u][k]*delta[y];
        }
//	  cout << i << " " << sum << "\n" << flush;
      backprop[i]=sum;
	  i++;
      }
    }
  }
//cout << "\n" << flush;

Float err=0.0;
if (output) {
	err=f_cost(rbackprop);
	}
else {
	for (int yyy=0;yyy<NY;yyy++)
		err+= delta[yyy]*delta[yyy];
	}
return err;
}





Float
Layer_soft::backward(Float* rbackprop, Float weight)
{
int y,u,k;

Float BKD[1024];
  for (y=0; y<NY; y++)
	BKD[y]=rbackprop[y];

// If isn't an output layer
// rbackprop[] is a backprop contribution
// coming from upwards.
if (!output) {
  for (y=0; y<NY; y++) {
    BKD[y] *= f1(y);
    delta[y]=weight*BKD[y];
  }
}
// If this is an output layer
// rbackprop[] is the target vector.
else {
  for (y=0; y<NY; y++) {
    delta[y]=weight*(Y[y]-BKD[y]);
    }
  }

Float sum;
int i=0;

// If this isn't an input layer
// the backprop contribution
// must be computed, either for
// the real input part or fully.
if (ninput==1) {
//	cout << "1 " << NU << " " << NUr << flush;
  for (u=NU; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      sum=0.0;
      for (y=0; y<NY; y++) {
		  sum += W[y][u][k]*delta[y];
        }
      backprop[NUplain+i]=sum;
	  i++;
      }
    }
  }
else if (ninput==2) {
//	cout << "2 " << NU << " " << NUr << flush;
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      sum=0.0;
      for (y=0; y<NY; y++) {
		  sum += W[y][u][k]*delta[y];
        }
//	  cout << i << " " << sum << "\n" << flush;
      backprop[i]=sum;
	  i++;
      }
    }
  }
//cout << "\n" << flush;

Float err=0.0;
if (output) {
	err=f_cost(rbackprop);
	}
else {
	for (int yyy=0;yyy<NY;yyy++)
		err+= delta[yyy]*delta[yyy];
	}
return err;
}



void
Layer::gradient(int* I)
{
int y,u;

for (y=0; y<NY; y++) {
  for (u=0; u<NU; u++) {
    if (I[u]>=0) {
      dW[y][u][I[u]] += delta[y];
      }
    }
  dB[y] += delta[y];
  }
}




void
Layer::gradient(Float* I)
{
int y,u,k;
int i;

for (y=0; y<NY; y++) {
  i=0;
  for (u=0; u<NU; u++) {
    for (k=0; k<NK[u]; k++) {
      	dW[y][u][k] += delta[y]*I[i++];
	}
      }
  dB[y] += delta[y];
  }
}


void
Layer::gradient(int* I1,Float* I2)
{
int y,u,k;
int i;

for (y=0; y<NY; y++) {
  for (u=0; u<NU; u++) {
    if (I1[u]>=0) {
      dW[y][u][I1[u]] += delta[y];
      }
    }
  i=0;
  for (u=NU; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      	dW[y][u][k] += delta[y]*I2[i++];
	}
      }
  dB[y] += delta[y];
  }
}


void
Layer::gradient(Float* I1,Float* I2)
{
int y,u,k;
int i1,i2;

for (y=0; y<NY; y++) {
  i1=0;
  for (u=0; u<NU; u++) {
    for (k=0; k<NK[u]; k++) {
      dW[y][u][k] += delta[y]*I1[i1++];
      }
    }
  i2=0;
  for (u=NU; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      	dW[y][u][k] += delta[y]*I2[i2++];
	}
      }
  dB[y] += delta[y];
  }
}



void
Layer::gradient()
{
gradient(U,&(U[NUplain]));
}


Float sign(Float a) {
if (a>0) return 1.0;
if (a<0) return -1.0;
return 0.0;
}

Float clipped(Float a) {
Float b=sign(a)*a;
if (b>1) return sign(a)*1.0;
if (b<0.1) return sign(a)*0.1;
return a;
}


void
Layer::updateWeights(Float epsilon)
{
int y,u,k;
for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      d2W[y][u][k] = momentum*d2W[y][u][k] + dW[y][u][k];
      W[y][u][k] -= epsilon*d2W[y][u][k];
      }
    }
  d2B[y] = momentum*d2B[y] + dB[y];
  B[y] -= epsilon*d2B[y];
  }

}

void
Layer::updateWeightsL1(Float epsilon)
{
int y,u,k;
double sum=0;

for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      sum += dW[y][u][k]*dW[y][u][k];
      }
    }
  sum += dB[y]*dB[y];
  }
sum = sqrt(sum);
for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      W[y][u][k] -= epsilon*dW[y][u][k]/sum;
      }
    }
  B[y] -= epsilon*dB[y]/sum;
  }
}


void
Layer::updateWeightsClipped(Float epsilon)
{
int y,u,k;
for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
//      W[y][u][k] -= epsilon*sign(dW[y][u][k]);
      W[y][u][k] -= epsilon*clipped(dW[y][u][k]);
//      W[y][u][k] -= epsilon*dW[y][u][k];
//	  cout << y << " " << u << " " << k << " " <<dW[y][u][k] << "\n";
      }
    }
//  B[y] -= epsilon*sign(dB[y]);
  B[y] -= epsilon*clipped(dB[y]);
//  cout << dB[y] << "\n\n\n";
//  B[y] -= epsilon*dB[y];
  }

}




void
Layer::resetGradient()
{
int y,u;
for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    memset(dW[y][u], 0, NK[u]*sizeof(Float));
    }
  dB[y] = 0.0;
  }
}



void
Layer::initWeights(int seed)
{
int y,u,k;
Float D=(Float)(NU+NUr);
//if (D>20) D=20.0;
//D=sqrt(D);

//srand48(seed);
srand(seed);
for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
      W[y][u][k] = (Float)(0.5-(Float)rand()/(double(RAND_MAX)))/D;
      }
    }
  B[y] = (Float)(0.5-(Float)rand()/(double(RAND_MAX)))/D;
  }
}



void
Layer::set_dW(Float*** newdW) {
for (int y=0; y<NY; y++) {
  for (int u=0; u<NU+NUr; u++) {
    for (int k=0; k<NK[u]; k++) {
	dW[y][u][k]=newdW[y][u][k];
      }
    }
  }
}

Float
Layer::dlength()
{
int y,u,k;
Float sum=0.0;

for (y=0; y<NY; y++) {
  for (u=0; u<NU+NUr; u++) {
    for (k=0; k<NK[u]; k++) {
       sum += dW[y][u][k]*dW[y][u][k];
//	   cout << sum << " " << flush;
      }
    }
  sum += dB[y]*dB[y];
  }
return sqrt(sum);
}






// Layer_soft


Float
Layer_soft::f1(int y)
{
return (Y[y] - Y[y]*Y[y]);
}


Float
Layer_soft::f_cost(Float* t)
{
//	cout << "S"<<flush;
return Layer::log_cost(t);
}



// Layer_tanh


Float
Layer_tanh::f1(int y)
{
return 1.0-(Y[y]*Y[y]);
}




Float
Layer_tanh::f_cost(Float* t)
{
//	cout << "T"<<flush;
return Layer::sq_cost(t);
}




