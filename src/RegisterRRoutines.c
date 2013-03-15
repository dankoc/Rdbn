//using namespace std;
//extern "C" {

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
SEXP update_mean(SEXP tf_posterior, SEXP loci_bed, SEXP module_length, SEXP bin_width, SEXP bigwig_file);
SEXP estimate_tf_binding(SEXP loci_bed, SEXP p_z, SEXP param, SEXP bg_param, SEXP model_type, SEXP window_size, SEXP bigwig_files);
SEXP RnbinomMLE(SEXP n, SEXP weights, SEXP k);

/**************************************************************
 *
 *      Register entry points...
 *
 **************************************************************/
void R_init_GROseq(DllInfo *info) {
     R_CallMethodDef callMethods[]  = {
       {"compute_priors", (DL_FUNC)&compute_priors, 1},
       {"update_mean", (DL_FUNC)&update_mean, 5},
       {"estimate_tf_binding", (DL_FUNC)&estimate_tf_binding, 7},
	   {"RnbinomMLE", (DL_FUNC)&estimate_tf_binding, 3},
       {NULL, NULL, 0}
     };

        R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

//}
