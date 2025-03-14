//------------------------------------------------------------------------------
// GB_jit__AxB_dot2__2c1f046bbb0bbbcd.c
//------------------------------------------------------------------------------
// SuiteSparse:GraphBLAS v8.0.1, Timothy A. Davis, (c) 2017-2023,
// All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
// The above copyright and license do not apply to any
// user-defined types and operators defined below.
//------------------------------------------------------------------------------

#include "GB_jit_kernel.h"

// semiring: (plus, rdiv, double)

// monoid:
#define GB_Z_TYPE double
#define GB_ADD(z,x,y) z = (x) + (y)
#define GB_UPDATE(z,y) z += y
#define GB_DECLARE_IDENTITY(z) double z = 0
#define GB_DECLARE_IDENTITY_CONST(z) const double z = 0
#define GB_HAS_IDENTITY_BYTE 1
#define GB_IDENTITY_BYTE 0x00
#define GB_PRAGMA_SIMD_REDUCTION_MONOID(z) GB_PRAGMA_SIMD_REDUCTION (+,z)
#define GB_Z_IGNORE_OVERFLOW 1
#define GB_Z_NBITS 64
#define GB_Z_ATOMIC_BITS 64
#define GB_Z_HAS_ATOMIC_UPDATE 1
#define GB_Z_HAS_OMP_ATOMIC_UPDATE 1
#define GB_Z_HAS_CUDA_ATOMIC_BUILTIN 1
#define GB_Z_CUDA_ATOMIC GB_cuda_atomic_add
#define GB_Z_CUDA_ATOMIC_TYPE double

// multiplicative operator:
#define GB_MULT(z,x,y,i,k,j) z = (y) / (x)

// multiply-add operator:
#define GB_MULTADD(z,x,y,i,k,j) z += (y) / (x)

// special cases:

// C matrix: full
#define GB_C_IS_HYPER  0
#define GB_C_IS_SPARSE 0
#define GB_C_IS_BITMAP 0
#define GB_C_IS_FULL   1
#define GBP_C(Cp,k,vlen) ((k) * (vlen))
#define GBH_C(Ch,k)      (k)
#define GBI_C(Ci,p,vlen) ((p) % (vlen))
#define GBB_C(Cb,p)      1
#define GB_C_NVALS(e) int64_t e = (C->vlen * C->vdim)
#define GB_C_NHELD(e) GB_C_NVALS(e)
#define GB_C_ISO 0
#define GB_C_IN_ISO 0
#define GB_C_TYPE double
#define GB_PUTC(c,Cx,p) Cx [p] = c

// M matrix: none
#define GB_M_TYPE void
#define GB_MCAST(Mx,p,msize) 1
#define GB_MASK_STRUCT 1
#define GB_MASK_COMP   0
#define GB_NO_MASK     1

// A matrix: full
#define GB_A_IS_HYPER  0
#define GB_A_IS_SPARSE 0
#define GB_A_IS_BITMAP 0
#define GB_A_IS_FULL   1
#define GBP_A(Ap,k,vlen) ((k) * (vlen))
#define GBH_A(Ah,k)      (k)
#define GBI_A(Ai,p,vlen) ((p) % (vlen))
#define GBB_A(Ab,p)      1
#define GB_A_NVALS(e) int64_t e = (A->vlen * A->vdim)
#define GB_A_NHELD(e) GB_A_NVALS(e)
#define GB_A_ISO 0
#define GB_A_TYPE double
#define GB_A2TYPE double
#define GB_DECLAREA(a) double a
#define GB_GETA(a,Ax,p,iso) a = Ax [p]

// B matrix: sparse
#define GB_B_IS_HYPER  0
#define GB_B_IS_SPARSE 1
#define GB_B_IS_BITMAP 0
#define GB_B_IS_FULL   0
#define GBP_B(Bp,k,vlen) Bp [k]
#define GBH_B(Bh,k)      (k)
#define GBI_B(Bi,p,vlen) Bi [p]
#define GBB_B(Bb,p)      1
#define GB_B_NVALS(e) int64_t e = B->nvals
#define GB_B_NHELD(e) GB_B_NVALS(e)
#define GB_B_ISO 0
#define GB_B_TYPE double
#define GB_B2TYPE double
#define GB_DECLAREB(b) double b
#define GB_GETB(b,Bx,p,iso) b = Bx [p]

#include "GB_mxm_shared_definitions.h"
#ifndef GB_JIT_RUNTIME
#define GB_jit_kernel GB_jit__AxB_dot2__2c1f046bbb0bbbcd
#define GB_jit_query  GB_jit__AxB_dot2__2c1f046bbb0bbbcd_query
#endif
#include "GB_jit_kernel_AxB_dot2.c"
GB_JIT_GLOBAL GB_JIT_QUERY_PROTO (GB_jit_query) ;
GB_JIT_GLOBAL GB_JIT_QUERY_PROTO (GB_jit_query)
{
    (*hash) = 0xdf8cbb0c0ac7ce22 ;
    v [0] = 8 ; v [1] = 0 ; v [2] = 1 ;
    defn [0] = NULL ;
    defn [1] = NULL ;
    defn [2] = NULL ;
    defn [3] = NULL ;
    defn [4] = NULL ;
    return (true) ;
}
