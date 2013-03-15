/**************************************************************
 *
 *      Associates a vector of genomic featuers (e.g. genes, CpG islands, etc.) with a table of sequence reads.
 *
 **************************************************************/
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>


SEXP compute_priors(SEXP all_posteriors);

/**************************************************************
 *
 *      Register entry points...
 *
 **************************************************************/
void R_init_GROseq(DllInfo *info) {
     R_CallMethodDef callMethods[]  = {
       {"compute_priors", (DL_FUNC)&compute_priors, 1},
       {NULL, NULL, 0}
     };

        R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

