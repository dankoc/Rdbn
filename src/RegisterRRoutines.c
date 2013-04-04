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


SEXP get_genomic_data_R(SEXP chrom_r, SEXP centers_r, SEXP bigwig_plus_file_r, SEXP bigwig_minus_file_r, SEXP model_r);
SEXP train_rbm_R(SEXP rbm_r, SEXP training_data_r, SEXP batch_size_r, SEXP cdn_r);

/**************************************************************
 *
 *      Register entry points...
 *
 **************************************************************/
void R_init_GROseq(DllInfo *info) {
     R_CallMethodDef callMethods[]  = {
       {"get_genomic_data_R", (DL_FUNC)&get_genomic_data_R, 5},
       {"train_rbm_R", (DL_FUNC)&get_genomic_data_R, 4},
       {NULL, NULL, 0}
     };

        R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

