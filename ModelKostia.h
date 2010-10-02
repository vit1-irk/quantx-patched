#ifndef INCLUDED_MODEL_H
#define INCLUDED_MODEL_H

struct PotData
{
	double  Uwleft; //value onleft
	double * u; //value of potential
	double * l; //length of step (width)
	int size; //??number of step
	int e2find;
};

struct Elevdata
{
	int nlevels;
	double * energies;
	int bufsize;

	double * a;
	double * b;
};

#endif /* INCLUDED_MODEL_H */
