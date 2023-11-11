//避免被重定义：
#ifndef _H_MINPACK_NON_
#define _H_MINPACK_NON_

#define   MAX_POINTS    500
#define   Ni              2
#define REL_SENSOR_TOLERANCE_ftol    1.0E-04      /* [pix] */
#define REL_PARAM_TOLERANCE_xtol     1.0E-5
#define ORTHO_TOLERANCE_gtol         0.0
#define MAXFEV                       (100*n)
#define EPSFCN                       1.0E-8     /* Do not set to zero! */
#define MODE                         1           /* variables are scalled internally */
#define FACTOR                       1000.0 

//该类的作用是用来求非线型的最优化问题：
class minpack_non
{
 public:
    minpack_non();
	~minpack_non();

    int lmdif_(void (*fcn)(int* ,int*,double*,double*), int* m, int* n, double* x, double* fvec,
		double* ftol, double* xtol, double* gtol, int* maxfev, 
		double* epsfcn, double* diag, int* mode, double* factor, int* nprint, int* info,
		int* nfev, double* fjac, int* ldfjac, int* ipvt, 
		double* qtf, double* wa1, double* wa2, double* wa3, double* wa4);
    
	double enorm_(int* n, double* x);
	
	int fdjac2_(void (*fcn)(int *,int *,double *,double *), int* m, int* n, double* x, double*fvec,
	    double* fjac, int* ldfjac, int* iflag, double* epsfcn,double* wa);

	double dpmpar_(int* i);

	int qrfac_(int* m, int* n, double* a, int* lda,
	    int* pivot, int* ipvt, int* lipvt, double* rdiag, double* acnorm, 
	    double* wa);

/* Subroutine */ int lmpar_(int *n, double* r, int *ldr, int *ipvt, double *diag,
	double *qtb, double *delta, double *par, double *x, double *sdiag, 
	double *wa1, double *wa2);

/* Subroutine */ int qrsolv_(int *n, double *r, int *ldr, int *ipvt,
	double *diag, double *qtb, double *x, double *sdiag, double *wa);
	//*****************************************************
 // void   cc_full_optimization_error_h(int *m_ptr,int *n_ptr, double *params, double *err);
};

#endif //_H_MINPACK_NON_