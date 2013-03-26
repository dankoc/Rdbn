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


SEXP get_genomic_data_R(SEXP centers_r, SEXP plus_counts_r, SEXP minus_counts_r, SEXP model_r);

/**************************************************************
 *
 *      Register entry points...
 *
 **************************************************************/
void R_init_GROseq(DllInfo *info) {
     R_CallMethodDef callMethods[]  = {
       {"get_genomic_data_R", (DL_FUNC)&get_genomic_data_R, 4},
       {NULL, NULL, 0}
     };

        R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

