.file "tanf.s"

// Copyright (c) 2000, Intel Corporation
// All rights reserved.
// 
// Contributed 2/2/2000 by John Harrison, Ted Kubaska, Bob Norin, Shane Story,
// and Ping Tak Peter Tang of the Computational Software Lab, Intel Corporation.
// 
// WARRANTY DISCLAIMER
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR ITS 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
// 
// Intel Corporation is the author of this code, and requests that all
// problem reports or change requests be submitted to it directly at 
// http://developer.intel.com/opensource.
//
// History
//==============================================================
// 2/02/00: Initial version
// 4/04/00  Unwind support added
//
// API
//==============================================================
// float tanf( float x);
//
// Overview of operation
//==============================================================
// If the input value in radians is |x| >= 1.xxxxx 2^10 call the
// older slower version.
//
// The new algorithm is used when |x| <= 1.xxxxx 2^9.
//
// Represent the input X as Nfloat * pi/2 + r
//    where r can be negative and |r| <= pi/4
//
//     tan_W  = x * 2/pi
//     Nfloat = round_int(tan_W)
//
//     tan_r  = x - Nfloat * (pi/2)_hi
//     tan_r  = tan_r - Nfloat * (pi/2)_lo
//
// We have two paths: p8, when Nfloat is even and p9. when Nfloat is odd.
// p8: tan(X) =  tan(r)
// p9: tan(X) = -cot(r)
//
// Each is evaluated as a series. The p9 path requires 1/r.
//
// The coefficients used in the series are stored in a table as
// are the pi constants.
//
// Registers used
//==============================================================
//
// predicate registers used:  
// p6, p7, p8, p9, p10
//
// floating-point registers used:  
// f32 ->  f93
// f8, input
//
// general registers used
// r32 -> r43
//
// Assembly macros
//==============================================================
tan_Inv_Pi_by_2              = f32
tan_Pi_by_2_hi               = f33
tan_Pi_by_2_lo               = f34


tan_P0                       = f35
tan_P1                       = f36
tan_P2                       = f37
tan_P3                       = f38 
tan_P4                       = f39 
tan_P5                       = f40 
tan_P6                       = f41
tan_P7                       = f42
tan_P8                       = f43 
tan_P9                       = f44 
tan_P10                      = f45 
tan_P11                      = f46
tan_P12                      = f47 
tan_P13                      = f48
tan_P14                      = f49
tan_P15                      = f50

tan_Q0                       = f51 
tan_Q1                       = f52 
tan_Q2                       = f53 
tan_Q3                       = f54 
tan_Q4                       = f55 
tan_Q5                       = f56 
tan_Q6                       = f57 
tan_Q7                       = f58 
tan_Q8                       = f59
tan_Q9                       = f60
tan_Q10                      = f61

tan_r                        = f62
tan_rsq                      = f63
tan_rcube                    = f64

tan_v18                      = f65
tan_v16                      = f66
tan_v17                      = f67
tan_v12                      = f68
tan_v13                      = f69
tan_v7                       = f70
tan_v8                       = f71
tan_v4                       = f72
tan_v5                       = f73
tan_v15                      = f74
tan_v11                      = f75
tan_v14                      = f76
tan_v3                       = f77
tan_v6                       = f78
tan_v10                      = f79
tan_v2                       = f80
tan_v9                       = f81
tan_v1                       = f82
tan_int_Nfloat               = f83 
tan_Nfloat                   = f84 

tan_NORM_f8                  = f85 
tan_W                        = f86

tan_y0                       = f87
tan_d                        = f88 
tan_y1                       = f89 
tan_dsq                      = f90 
tan_y2                       = f91 
tan_d4                       = f92 
tan_inv_r                    = f93 


/////////////////////////////////////////////////////////////

tan_AD                       = r33
tan_GR_10009                 = r34 
tan_GR_17_ones               = r35 
tan_GR_N_odd_even            = r36 
tan_GR_N                     = r37 
tan_signexp                  = r38
tan_exp                      = r39
tan_ADQ                      = r40

GR_SAVE_B0                    = r42
GR_SAVE_PFS                   = r41
GR_SAVE_GP                    = r43


.data

.align 16

double_tan_constants:
   data8 0xA2F9836E4E44152A, 0x00003FFE // 2/pi
   data8 0xC90FDAA22168C234, 0x00003FFF // pi/2 hi

   data8 0xBEEA54580DDEA0E1 // P14 
   data8 0x3ED3021ACE749A59 // P15
   data8 0xBEF312BD91DC8DA1 // P12 
   data8 0x3EFAE9AFC14C5119 // P13
   data8 0x3F2F342BF411E769 // P8
   data8 0x3F1A60FC9F3B0227 // P9
   data8 0x3EFF246E78E5E45B // P10
   data8 0x3F01D9D2E782875C // P11
   data8 0x3F8226E34C4499B6 // P4
   data8 0x3F6D6D3F12C236AC // P5
   data8 0x3F57DA1146DCFD8B // P6
   data8 0x3F43576410FE3D75 // P7
   data8 0x3FD5555555555555 // P0
   data8 0x3FC11111111111C2 // P1
   data8 0x3FABA1BA1BA0E850 // P2
   data8 0x3F9664F4886725A7 // P3

double_Q_tan_constants:
   data8 0xC4C6628B80DC1CD1, 0x00003FBF // pi/2 lo
   data8 0x3E223A73BA576E48 // Q8
   data8 0x3DF54AD8D1F2CA43 // Q9
   data8 0x3EF66A8EE529A6AA // Q4
   data8 0x3EC2281050410EE6 // Q5
   data8 0x3E8D6BB992CC3CF5 // Q6
   data8 0x3E57F88DE34832E4 // Q7
   data8 0x3FD5555555555555 // Q0
   data8 0x3F96C16C16C16DB8 // Q1
   data8 0x3F61566ABBFFB489 // Q2
   data8 0x3F2BBD77945C1733 // Q3
   data8 0x3D927FB33E2B0E04 // Q10


   
.align 32
.global tan#

////////////////////////////////////////////////////////



.section .text
.global tanf
.proc  tanf
.align 32
tanf: 
// The initial fnorm will take any unmasked faults and
// normalize any single/double unorms

{ .mmi
      alloc          r32=ar.pfs,1,11,0,0               
(p0)  addl           tan_AD   = @ltoff(double_tan_constants), gp
      nop.i 999
}
;;

{ .mmi
      ld8 tan_AD = [tan_AD]
      nop.m 999
      nop.i 999
}
;;

{ .mfi
	nop.m 999
(p0)  fnorm     tan_NORM_f8  = f8                      
(p0)  mov       tan_GR_17_ones     = 0x1ffff             ;;
}

{ .mfi
	nop.m 999
	nop.f 999
(p0)  mov       tan_GR_10009 = 0x10009  ;;
}



{ .mmi
      adds tan_ADQ = double_Q_tan_constants - double_tan_constants, tan_AD
(p0)  ldfe      tan_Inv_Pi_by_2 = [tan_AD],16          
	nop.i 999  
}
;;


{ .mfi
(p0)  ldfe      tan_Pi_by_2_hi = [tan_AD],16 
(p0)  fclass.m.unc  p6,p0 = f8, 0x07        
}
{ .mfi
(p0)  ldfe      tan_Pi_by_2_lo = [tan_ADQ],16           
        nop.f 999 
	nop.i 999 ;;
}


{ .mmb
(p0)  ldfd      tan_P14 = [tan_AD],8                         
(p0)  ldfd      tan_Q8  = [tan_ADQ],8                        
	nop.b 999 ;;
}

{ .mmb
(p0)  ldfd      tan_P15 = [tan_AD],8                         
(p0)  ldfd      tan_Q9  = [tan_ADQ],8                        
	nop.b 999 ;;
}



{ .mmb
(p0)  ldfd      tan_P12 = [tan_AD],8                         
(p0)  ldfd      tan_Q4  = [tan_ADQ],8                        
	nop.b 999 ;;
}

{ .mmb
(p0)  ldfd      tan_P13 = [tan_AD],8                         
(p0)  ldfd      tan_Q5  = [tan_ADQ],8                        
	nop.b 999 ;;
}

{ .mmb
(p0)  ldfd      tan_P8  = [tan_AD],8                         
(p0)  getf.exp  tan_signexp    = tan_NORM_f8                 
	nop.b 999 ;;
}

{ .mmb
(p0)  ldfd      tan_P9  = [tan_AD],8                         
(p0)  ldfd      tan_Q6  = [tan_ADQ],8
(p6)  br.ret.spnt    b0    ;; 
}


{ .mmi
(p0)  ldfd      tan_P10 = [tan_AD],8                         
(p0)  ldfd      tan_Q7  = [tan_ADQ],8                         
(p0)  and       tan_exp = tan_GR_17_ones, tan_signexp         ;;
}



// p7 is true if we must call DBX TAN
// p7 is true if f8 exp is > 0x10009 (which includes all ones
//    NAN or inf)

{ .mfi
(p0)  ldfd      tan_P11 = [tan_AD],8                         
(p0)  fma.s1    tan_W   = tan_NORM_f8,          tan_Inv_Pi_by_2, f0    
(p0)  cmp.ge.unc  p7,p0 = tan_exp,tan_GR_10009               
}
{ .mfi
(p0)  ldfd      tan_Q0  = [tan_ADQ],8                         
        nop.f 999
        nop.i 999 ;;
}


{ .mmb
(p0)  ldfd      tan_P4  = [tan_AD],8                         
(p0)  ldfd      tan_Q1  = [tan_ADQ],8                         
(p7) br.cond.spnt   TAN_DBX ;;                                  
}


{ .mmb
(p0)  ldfd      tan_P5  = [tan_AD],8                         
(p0)  ldfd      tan_Q2  = [tan_ADQ],8                         
	nop.b 999 ;;
}



{ .mmb
(p0)  ldfd      tan_P6  = [tan_AD],8                         
(p0)  ldfd      tan_Q3  = [tan_ADQ],8                         
	nop.b 999 ;;
}


{ .mmi
(p0)  ldfd      tan_P7  = [tan_AD],8 
(p0)  ldfd      tan_Q10 = [tan_ADQ],8                         
	nop.i 999 ;;
}


// tan_int_Nfloat = Round_Int_Nearest(tan_W)
{ .mfi
(p0)  ldfd      tan_P0  = [tan_AD],8                         
(p0)  fcvt.fx.s1  tan_int_Nfloat = tan_W                     
	nop.i 999 ;;
}


{ .mmi
(p0)  ldfd      tan_P1  = [tan_AD],8 
        nop.m 999 
	nop.i 999 ;;
}

{ .mfi
(p0)  ldfd      tan_P2  = [tan_AD],8                         
        nop.f 999
	nop.i 999 ;;
}


{ .mmi
(p0)  ldfd      tan_P3  = [tan_AD],8                         
        nop.m 999
        nop.i 999 ;;
}

{ .mfi
	nop.m 999
(p0)  fcvt.xf     tan_Nfloat     = tan_int_Nfloat            
	nop.i 999 ;;
}


{ .mmi
	nop.m 999
(p0)  getf.sig    tan_GR_N = tan_int_Nfloat                  
	nop.i 999 ;;
}


{ .mmi
	nop.m 999
	nop.m 999
(p0)  and         tan_GR_N_odd_even = 0x1, tan_GR_N ;;          
}

// p8 ==> even
// p9 ==> odd
{ .mmi
	nop.m 999
	nop.m 999
(p0)  cmp.eq.unc  p8,p9          = tan_GR_N_odd_even, r0      ;;
}



// tan_r          = -tan_Nfloat * tan_Pi_by_2_hi + x
{ .mfi
	nop.m 999
(p0)  fnma.s1  tan_r      = tan_Nfloat, tan_Pi_by_2_hi,  tan_NORM_f8         
	nop.i 999 ;;
}


// tan_r          = tan_r -tan_Nfloat * tan_Pi_by_2_lo 
{ .mfi
	nop.m 999
(p0)  fnma.s1  tan_r      = tan_Nfloat, tan_Pi_by_2_lo,  tan_r      
	nop.i 999 ;;
}


{ .mfi
	nop.m 999
(p0)  fma.s1   tan_rsq    = tan_r, tan_r,   f0                      
	nop.i 999 ;;
}


{ .mfi
	nop.m 999
(p9)  frcpa.s1   tan_y0, p10 = f1,tan_r                  
	nop.i 999  ;;
}





{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v18 = tan_rsq, tan_P15, tan_P14        
	nop.i 999
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v4  = tan_rsq, tan_P1, tan_P0          
	nop.i 999  ;;
}



{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v16 = tan_rsq, tan_P13, tan_P12        
	nop.i 999 
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v17 = tan_rsq, tan_rsq, f0             
	nop.i 999 ;;
}



{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v12 = tan_rsq, tan_P9, tan_P8          
	nop.i 999 
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v13 = tan_rsq, tan_P11, tan_P10        
	nop.i 999 ;;
}



{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v7  = tan_rsq, tan_P5, tan_P4          
	nop.i 999 
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v8  = tan_rsq, tan_P7, tan_P6          
	nop.i 999 ;;
}



{ .mfi
	nop.m 999
(p9)  fnma.s1    tan_d   = tan_r, tan_y0, f1   
	nop.i 999 
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v5  = tan_rsq, tan_P3, tan_P2          
	nop.i 999 ;;
}



{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v11 = tan_rsq, tan_Q9, tan_Q8         
	nop.i 999
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v12 = tan_rsq, tan_rsq, f0            
	nop.i 999 ;;
}


{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v15 = tan_v17, tan_v18, tan_v16        
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v7 = tan_rsq, tan_Q5, tan_Q4          
	nop.i 999 ;;
}


{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v11 = tan_v17, tan_v13, tan_v12        
	nop.i 999
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v14 = tan_v17, tan_v17, f0             
	nop.i 999 ;;
}



{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v8 = tan_rsq, tan_Q7, tan_Q6          
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v3 = tan_rsq, tan_Q1, tan_Q0          
	nop.i 999 ;; 
}




{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v3 = tan_v17, tan_v5, tan_v4           
	nop.i 999
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v6 = tan_v17, tan_v8, tan_v7           
	nop.i 999 ;;
}



{ .mfi
	nop.m 999
(p9)  fma.s1     tan_y1  = tan_y0, tan_d, tan_y0    
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v10 = tan_v12, tan_Q10, tan_v11       
	nop.i 999 ;; 
}


{ .mfi
	nop.m 999
(p9)  fma.s1     tan_dsq = tan_d, tan_d, f0        
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v9  = tan_v12, tan_v12,f0             
	nop.i 999 ;;
}


{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v4 = tan_rsq, tan_Q3, tan_Q2          
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v6  = tan_v12, tan_v8, tan_v7         
	nop.i 999 ;; 
}



{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v10 = tan_v14, tan_v15, tan_v11        
	nop.i 999 ;; 
}



{ .mfi
	nop.m 999
(p9)  fma.s1     tan_y2  = tan_y1, tan_d, tan_y0         
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1     tan_d4  = tan_dsq, tan_dsq, tan_d       
	nop.i 999  ;;
}


{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v2 = tan_v14, tan_v6, tan_v3           
	nop.i 999
}
{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v9 = tan_v14, tan_v14, f0              
	nop.i 999 ;;
}


{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v2  = tan_v12, tan_v4, tan_v3         
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v5  = tan_v9, tan_v10, tan_v6         
	nop.i 999  ;;
}


{ .mfi
	nop.m 999
(p9)  fma.s1     tan_inv_r = tan_d4, tan_y2, tan_y0      
	nop.i 999 
}
{ .mfi
        nop.m 999
(p8)  fma.s1   tan_rcube  = tan_rsq, tan_r,   f0
        nop.i 999  ;;
}



{ .mfi
	nop.m 999
(p8)  fma.s1  tan_v1 = tan_v9, tan_v10, tan_v2           
	nop.i 999 
}
{ .mfi
	nop.m 999
(p9)  fma.s1  tan_v1  = tan_v9, tan_v5, tan_v2          
	nop.i 999   ;;
}



{ .mfb
	nop.m 999
(p8)  fma.s   f8  = tan_v1, tan_rcube, tan_r             
(p0)  nop.b 999  
}
{ .mfb
	nop.m 999
(p9)  fms.s.s0  f8      = tan_r, tan_v1, tan_inv_r        
(p0)  br.ret.sptk    b0 ;;    
}
.endp tanf


.proc TAN_DBX
TAN_DBX: 
.prologue
{ .mfi
        nop.m 0
        nop.f 0
.save   ar.pfs,GR_SAVE_PFS
        mov  GR_SAVE_PFS=ar.pfs                 // Save ar.pfs
};;
{ .mfi
        mov GR_SAVE_GP=gp                       // Save gp
        nop.f 0
.save   b0, GR_SAVE_B0
        mov GR_SAVE_B0=b0                       // Save b0
}
.body
{ .mfi
	nop.m 999
(p0) fmerge.s f9 = f1,f1 
	nop.i 999   ;;
}

{ .mib
	nop.m 999
	nop.i 999                      
(p0)  br.call.sptk.many   b0=__libm_tan# ;;            
}
{ .mmi
    mov   gp = GR_SAVE_GP                  // Restore gp
	nop.m 999
    mov   b0 = GR_SAVE_B0;;                // Restore return address
}

{ .mfi
      nop.m 999
(p0)  fnorm.s f8 = f8
      mov   ar.pfs = GR_SAVE_PFS             // Restore ar.pfs
};;

{ .mib
         nop.m 999
         nop.i 999
(p0)  br.ret.sptk     b0
;;
}
.endp TAN_DBX


.type __libm_tan#,@function
.global __libm_tan#
