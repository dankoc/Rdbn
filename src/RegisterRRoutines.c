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

#include "rbm.h"
#include "dbn.h"

/**************************************************************
 *
 *      Register entry points...
 *
 **************************************************************/
void R_init_Rdbn(DllInfo *info) {
     R_CallMethodDef callMethods[]  = {
       {"train_rbm_R", (DL_FUNC)&train_rbm_R, 5},
       {"train_dbn_R", (DL_FUNC)&train_dbn_R, 5},
       {"backpropagation_dbn_R", (DL_FUNC)&backpropagation_dbn_R, 6},
       {"predict_dbn_R", (DL_FUNC)&predict_dbn_R, 4},
       {"convert_to_max_R", (DL_FUNC)&convert_to_max_R, 3},
       {"daydream_dbn_R", (DL_FUNC)&daydream_dbn_R, 5},
       {"receptivefields_dbn_R", (DL_FUNC)&recptivefields_dbn_R, 4},
       {NULL, NULL, 0}
     };

        R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

