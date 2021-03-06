/*

   BLIS    
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas at Austin

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name of The University of Texas at Austin nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"

extern gemm_t* gemm_cntl;

void bli_gemm( obj_t*  alpha,
               obj_t*  a,
               obj_t*  b,
               obj_t*  beta,
               obj_t*  c )
{
        printf("Inicio aca");
	num_t dt = bli_obj_datatype( *c );

	// If an induced method is available (ie: implemented and enabled),
	// call it instead.
	if ( bli_gemmind_has_avail( dt ) )
	{
		gemm_fp_t func = bli_gemmind_get_avail( dt );

		func( alpha, a, b, beta, c );
                printf("Fin por aca");
                return;

	}

	bli_gemm_front( alpha, a, b, beta, c,
	                gemm_cntl );
}


#undef  GENTFUNC
#define GENTFUNC( ctype, ch, opname ) \
\
void PASTEMAC(ch,opname)( \
                          trans_t transa, \
                          trans_t transb, \
                          dim_t   m, \
                          dim_t   n, \
                          dim_t   k, \
                          ctype*  alpha, \
                          ctype*  a, inc_t rs_a, inc_t cs_a, \
                          ctype*  b, inc_t rs_b, inc_t cs_b, \
                          ctype*  beta, \
                          ctype*  c, inc_t rs_c, inc_t cs_c  \
                        ) \
{ \
	const num_t dt = PASTEMAC(ch,type); \
\
	obj_t       alphao, ao, bo, betao, co; \
\
	dim_t       m_a, n_a; \
	dim_t       m_b, n_b; \
\
	bli_set_dims_with_trans( transa, m, k, m_a, n_a ); \
	bli_set_dims_with_trans( transb, k, n, m_b, n_b ); \
\
	bli_obj_create_1x1_with_attached_buffer( dt, alpha, &alphao ); \
	bli_obj_create_1x1_with_attached_buffer( dt, beta,  &betao  ); \
\
	bli_obj_create_with_attached_buffer( dt, m_a, n_a, a, rs_a, cs_a, &ao ); \
	bli_obj_create_with_attached_buffer( dt, m_b, n_b, b, rs_b, cs_b, &bo ); \
	bli_obj_create_with_attached_buffer( dt, m,   n,   c, rs_c, cs_c, &co ); \
\
	bli_obj_set_conjtrans( transa, ao ); \
	bli_obj_set_conjtrans( transb, bo ); \
\
	PASTEMAC0(opname)( &alphao, \
	                   &ao, \
	                   &bo, \
	                   &betao, \
	                   &co ); \
}

INSERT_GENTFUNC_BASIC0( gemm )

