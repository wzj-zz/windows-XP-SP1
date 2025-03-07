//      TITLE("System Initialization")
//++
//
// Module Name:
//
//       start.s
//
// Abstract:
//
//       This module implements the code necessary to initially startup the
//       NT system.
//
// Author:
//
//       Bernard Lint 8-Dec-1995
//
// Environment:
//
//    Kernel mode only.
//
// Revision History:
//
//    Based on MIPS version (from David N. Cutler (davec) 5-Apr-1991)
//
//--

#include "ksia64.h"

         PublicFunction(SwapFromIdle)
         PublicFunction(KeBugCheck)
         PublicFunction(KiInitializeKernel)
         PublicFunction(KeLowerIrql)
         PublicFunction(KiNormalSystemCall)
         PublicFunction(KiRetireDpcList)
         PublicFunction(KeAcquireQueuedSpinLockAtDpcLevel)
         PublicFunction(KeReleaseQueuedSpinLockFromDpcLevel)
         PublicFunction(KeTryToAcquireQueuedSpinLockRaiseToSynch)

         .global    __imp_HalProcessorIdle
         .global    KiIvtBase
         .global    KiIA64PtaContents

#if !defined(NT_UP)
         .global    HalAllProcessorsStarted
         .global    KeNumberProcessors
         .global    KiBarrierWait
         .global    KiKernelPcrPage
#endif // !defined(NT_UP)

#if DBG
         PublicFunction(KdPollBreakIn)
         PublicFunction(DbgBreakPointWithStatus)
         .global    KdDebuggerEnabled
#endif // DBG

        SBTTL("System Initialization")
//++
// Routine:
//
//       VOID
//       KiSystemBegin (
//           PLOADER_PARAMETER_BLOCK)
//
// Routine Description:
//
//       This routine is called when the NT system begins execution.
//       Its function is to initialize system hardware state, call the
//       kernel initialization routine, and then fall into code that
//       represents the idle thread for all processors.
//
// Arguments:
//
//       a0 - Supplies a pointer to the loader parameter block.
//
// Return Value:
//
//       None.
//
// Note:
//
//       On entry the global pointer (gp) is initialized for this module (the
//       NTOS kernel) by the loader.
//
//--

         NESTED_ENTRY(KiSystemBegin)
         ALTERNATE_ENTRY(_start)
         NESTED_SETUP(1,2,6,0)                   // Also see "alloc" below

//
// Register aliases
//

         rpT1      = t0
         rpT2      = t1
         rpT3      = t2
         rT1       = t3
         rT2       = t4
         rT3       = t5
         rT4       = t6

         rpLpb     = t7
         rThread   = t8
         rPcrPfn   = t9
         rPrcb     = t10
         rPcr2Pfn  = t11
         rProcNum  = t12
         rPKR      = t13
         rPkrNum   = t14
         rRR       = t15
         rKstack   = t16
         rVa       = t17
         rPcrTr    = t18
         rpPcr     = t19

         FAST_DISABLE_INTERRUPTS                 // disable interrupts
//
// invalidate ITR used by SAL.
//

         mov       rT2 = 0x14 << 2
         movl      rT3 = 0x03f00000
         ;;

         ptr.i     rT3, rT2

         ALTERNATE_ENTRY(KiInitializeSystem)

         mov       ar.rsc = r0                   // put RSE in lazy mode
         movl      rT2 = KiIvtBase
         ;;

         rsm       1 << PSR_IC                   // PSR.ic = 0
         mov       cr.iva = rT2
#if !defined(NT_UP)
         add       rpT1 = @gprel(KiKernelPcrPage), gp
         ;;
         ld8       rPcrPfn = [rpT1]
#else
         mov       rPcrPfn = 0
#endif
         ;;

         srlz.d
         add       rpT2 = LpbPcrPage, a0         // -> PCR page number
         cmp.eq    pt0, pt1 = 0, rPcrPfn
         ;;

//
// Initialize fixed TLB entries that map the PCR into system and user space.
// N.B. These pages are per *processor* so we need a fixed mapping.
//

 (pt0)   ld8       rPcrPfn = [rpT2]              // set PCR page number
         movl      rVa = KiPcr                   // set virtual address of PCR
         ;;

         mov       cr.ifa = rVa                  // setup IFA for insert
         mov       rT1 = PAGE_SHIFT << IDTR_PS
         shl       rPcrPfn = rPcrPfn, PAGE_SHIFT // shift to PPN field
         ;;

         mov       cr.itir = rT1                 // setup IDTR
         movl      rPcrTr = KIPCR_TR_INITIAL     // PCR translation except for PPN

         mov       rT2 = DTR_KIPCR_INDEX
         ;;
         or        rPcrTr = rPcrPfn, rPcrTr      // form full TR
         ;;

         itr.d     dtr[rT2] = rPcrTr             // insert PCR TR to DTR
         ssm       1 << PSR_IC                   // PSR.ic = 1
         ;;

         srlz.i                                  // I serialize
         add       rpT1 = LpbKernelStack, a0     // -> idle thread stack
         mov       rpLpb = a0                    // save a0
         ;;

         LDPTR     (rKstack, rpT1)               // rKstack = idle thread stack
                                                 // N.B. This is also the base
                                                 //      of backing store
         ;;
         mov       ar.bspstore = rKstack         // setup BSP
         .fframe   STACK_SCRATCH_AREA
         add       sp = -STACK_SCRATCH_AREA, rKstack  // set sp
         ;;

         loadrs                                  // invalidate RSE
         invala
         ;;

         mov       ar.rsc = RSC_KERNEL           // put RSE in eager mode
         ;;

         alloc     rT4 = ar.pfs,1,2,6,0          // keep in sync with entry point alloc

         mov       savedbrp = zero               // setup bogus brp and pfs to
         mov       savedpfs = zero               // stop the debugger

         PROLOGUE_END

//
// Get page frame numbers for the PCR and PDR pages that were allocated by
// the OS loader. Page directory is 4 physically contiguous pages.
//

         add       rpT1 = LpbPrcb, rpLpb         // -> PRCB
         dep.z     rRR = UREGION_INDEX, RR_INDEX, RR_INDEX_LEN
         ;;

         LDPTR     (rPrcb, rpT1)                 // get processor block address
         movl      rT2 = (START_PROCESS_RID << RR_RID)| RR_PS_VE
         ;;

//
// set RR[0], Region ID (RID) = 1, Page Size (PS) = 8K, VHPT enabled (VE) = 1
//

         mov       rr[rRR] = rT2                 // initialize rr[0]
         movl      rT1 = (START_SESSION_RID << RR_RID) | RR_PS_VE
         ;;

//
// set RR[1]
//
         movl      rRR = SADDRESS_BASE           // initialize
         ;;
         mov       rr[rRR] = rT1                 //   hydra region rr[1]
         ;;

//
// set RR[4], Region ID (RID) = 0, Page Size (PS) = 64K, VHPT enabled (VE) = 0
//

//
// set RR[2] to RR[7], RID = KSEG3_RID, Page Size = 8K, VHPT disabled 
//

         movl      rRR = 2 << RR_INDEX
         movl      rT2 = (KSEG3_RID << RR_RID) | (PAGE_SHIFT << RR_PS)
         ;;

         mov       rr[rRR] = rT2                 // initialize rr[2]
         movl      rRR = 3 << RR_INDEX
         ;;
         
         mov       rr[rRR] = rT2                 // initialize rr[3]
         movl      rRR = 4 << RR_INDEX
         ;;

         mov       rr[rRR] = rT2                 // initialize rr[4]
         movl      rRR = 5 << RR_INDEX
         ;;

         mov       rr[rRR] = rT2                 // initialize rr[5]
         movl      rRR = 6 << RR_INDEX
         ;;

         mov       rr[rRR] = rT2                 // initialize rr[6]

//
// Protection Key Registers
//

         mov       rPKR = zero                   // pkr index = 0
         mov       rT1 = PKR_VALID               // rr[0].v=1, pkr[0].key=0
         mov       rT2 = START_GLOBAL_RID
         ;;

         mov       pkr[rPKR] = rT1
         add       rPKR = 1, rPKR                // increment PKR number
         shl       rT2 = rT2, RR_RID
         ;;

         or        rT2 = rT1, rT2
         ;;
         mov       pkr[rPKR] = rT2               // pkr[1].v=1, pkr[1].key=START_GLOBAL_RID
         add       rPKR = 1, rPKR                // increment PKR number
         movl      rT2 = START_PROCESS_RID
         ;;

         shl       rT2 = rT2, RR_RID
         ;;
         or        rT2 = rT1, rT2
         ;;
         mov       pkr[rPKR] = rT2               // pkr[1].v=1, pkr[1].key=START_GLOBAL_RID

         add       rPKR = 1, rPKR                // increment PKR number
         mov       rT2 = zero
         ;;

Ksb_PKRLoop:

         mov       pkr[rPKR] = rT2               // set PKR invalid
         add       rPKR = 1, rPKR                // increment PKR number
         ;;

         cmp.gtu   pt0 = PKRNUM, rPKR            // at end?
(pt0)    br.cond.sptk.few.clr Ksb_PKRLoop
         ;;

//
// Initialize debug registers
//

         mov       rT1 = 0
         ;;

Ksb_DbrLoop:

         mov       ibr[rT1] = r0
         mov       dbr[rT1] = r0
         ;;

#ifndef  NO_12241
         srlz.d
#endif
         cmp.ne    pt0 = 7, rT1

         add       rT1 = 1, rT1
 (pt0)   br.cond.sptk.few Ksb_DbrLoop
         ;;

//
// Initialize control registers:
//       DCR
//       PSR pk, it, dt enabled
//

         ssm       (1 << PSR_DI) | (1 << PSR_PP) | (1 << PSR_IC) | (1 << PSR_DFH) | (1 << PSR_AC)
         ;;

         rsm       (1 << PSR_PK) | (1 << PSR_MFH) // must clear the mfh bit
         ;;

         srlz.i
         movl      rT1 = DCR_INITIAL
         ;;

         mov       cr.dcr = rT1

//
// Clear TC
//
// Set up ITR entry 2 with EPC page for system call
//

         movl      rT4 = KiNormalSystemCall
         ptc.e     zero
         add       rpT1 = LpbKernelVirtualBase, rpLpb
         add       rpT2 = LpbKernelPhysicalBase, rpLpb
         ;;

         LDPTR     (rT2, rpT1)                   // virtual load point
         LDPTR     (rT3, rpT2)                   // physical load point
         shr       rT4 = rT4, PAGE_SHIFT         // page number for syscall page
         ;;

         shr       rT2 = rT2, PAGE_SHIFT         // page number
         shr       rT3 = rT3, PAGE_SHIFT         // page number
         ;;

         sub       rT2 = rT4, rT2                // page offset
         movl      rVa = MM_EPC_VA
         ;;

         add       rT3 = rT3, rT2                // compute pfn of syscall page

         movl      rT2 = TR_VALUE(1,0,7,0,1,1,0,1)
         ;;
         shl       rT3 = rT3, PAGE_SHIFT         // set page number in TR
         ;;
         or        rT2 = rT2, rT3
         mov       rT3 = ITR_EPC_INDEX
         ;;

//
// Clear PSR.IC bit
//

         add       rpT1 = LpbPcrPage2, a0
         ;;
         ld8       rPcr2Pfn = [rpT1]             // set second PCR page
         mov       rT1 = PAGE_SHIFT << IDTR_PS

         rsm       1 << PSR_IC                   // PSR.ic = 0
         ;;
         srlz.d

         mov       cr.ifa = rVa                  // virtual address of epc page
         mov       cr.itir = rT1
         ;;
         itr.i     itr[rT3] = rT2

         movl      rVa = KiPcr2                  // set virtual address of PCR
         ;;

         mov       cr.ifa = rVa                  // setup IFA for insert
         movl      rPcrTr = KIPCR_TR_INITIAL     // PCR translation except for PPN

         mov       cr.itir = rT1                 // setup IDTR
         mov       rT2 = DTR_KIPCR2_INDEX
         shl       rPcr2Pfn = rPcr2Pfn, PAGE_SHIFT // shift to PPN field
         ;;

         or        rPcrTr = rPcr2Pfn, rPcrTr     // form full TR
         ;;
         itr.d     dtr[rT2] = rPcrTr             // insert PCR TR to DTR

         ssm       1 << PSR_IC                   // PSR.ic = 1
         ;;
         srlz.i                                  // I serialize

//
// Set the cache policy for cached memory.
// **** TBD ****

//
// Set the first level data and instruction cache fill size and size.
//
// **** TBD

//
// Set the second level data and instruction cache fill size and size.
//
// ***** TBD

//
// Set the data cache fill size and alignment values.
//
// **** TBD

//
// Set the instruction cache fill size and alignment values.
//
// **** TBD

//
// Sweep the data and instruction caches.
//
// **** TBD

//
// Initialize the fixed entries that map the PDR pages.
// Setup page directory, pte's for page dir
// **** TBD ****
//

//
// Initialize the Processor Control Registers (PCR).
//

//
// Initialize the minor and major version numbers.
//

         mov       rT1 = PCR_MINOR_VERSION       // set minor version number
         movl      rpPcr = KiPcr                 // get PCR address
         ;;

         add       rpT1 = PcMinorVersion, rpPcr
         add       rpT2 = PcMajorVersion, rpPcr
         mov       rT2 = PCR_MAJOR_VERSION       // set major version number
         ;;

         st2       [rpT1] = rT1                  // store minor
         st2       [rpT2] = rT2                  // store major
         add       rpT3 = PcPrcb, rpPcr          // -> PCR.Prcb
         ;;

//
// Set address of processor block.
//

         st8       [rpT3] = rPrcb                // store Prcb address

//
// Initialize the addresses of various data structures that are referenced
// from the exception and interrupt handling code.
//
// N.B. The panic stack is a separate stack that is used when the current
//      kernel stack overlfows.
//

         add       rpT1 = PcInitialStack,rpPcr   // -> initial kernel stack address in PCR
         add       rpT2 = LpbPanicStack, rpLpb   // -> lpb panic stack address

         add       rpT3 = PcPanicStack, rpPcr    // -> panic stack address in PCR
         ;;

         LDPTRINC  (rT2, rpT2, LpbThread-LpbPanicStack)   // panic stack address

         st8       [rpT1] = rKstack, PcKernelGP-PcInitialStack
         ;;

         LDPTR     (rThread, rpT2)               // -> current (idle) thread
         st8       [rpT3] = rT2, PcCurrentThread-PcPanicStack  // set PCR panic stack address
         ;;

         st8       [rpT3] = rThread              // set PCR current thread
         st8       [rpT1] = gp                   // save GP
         mov       rT1 = HIGH_LEVEL
         ;;

         SET_IRQL(rT1)                           // Set to highest IRQL

//
// Clear floating status and zero the count and compare registers.
//

         mov       ar.ccv = zero
         movl      rT1 = FPSR_FOR_KERNEL
         ;;

         mov       ar.fpsr = rT1
         mov       ar.ec = zero
         mov       ar.lc = zero

//
// Set system dispatch address limits used by get and set context.
// ***** TBD ******

//
// Set the default cache error routine address.
// ****** TBD *******

//
// Sweep the data and instruction caches.
// **** TBD *******

//
// Setup arguments and call kernel initialization routine.
//

         add       rpT1 = LpbProcess, rpLpb      // -> idle process address
         add       rpT2 = PbNumber, rPrcb
         mov       out3 = rPrcb                  // arg 4 is current PRCB
         ;;

         LDPTR     (out0, rpT1)                  // arg 1 is process
         mov       out1 = rThread                // arg 2 is thread
         mov       out2 = rKstack                // arg 3 is kernel stack

         ld1       out4 = [rpT2]                 // get processor number
         mov       out5 = rpLpb                  // arg 6 is pointer to LPB
         br.call.sptk.many.clr brp = KiInitializeKernel
         ;;                                                // (C code)

         alloc     t0 = ar.pfs,0,0,0,0
         mov       brp = zero     // setup a bogus brp to stop debugger
         br        KiIdleLoop     // branch to KIIdleLoop()
         ;;
//
// Never get to this point!
//
         NESTED_EXIT(KiSystemBegin)
//
//++
//
// VOID
// KiIdleLoop (
//    VOID
//    )
//
// Routine Description:
//
//    This is the idle loop for NT.  This code runs in a thread for
//    each processor in the system.  The idle thread runs at IRQL
//    DISPATCH_LEVEL and polls for work.
//
// Arguments:
//
//    None.
//
// Return Value:
//
//    None.  (This routine never returns).
//
//  When another thread is selected to run, SwapContext is called.
//  Normally, SwapContext saves and restores the non-volatile context.
//  The idle loop never returns so the preserved registers of the caller
//  do not need to be saved.
//  In other architectures, the idle loop pre-initializes the storage area
//  (switch frame) where SwapContext would normally save the preserved
//  registers with values that the idle loop needs
//  in those registers upon return from SwapContext and skips over the
//  register save on the way into SwapContext (alternate entry point
//  SwapFromIdle).
//  In this design the idle loop registers are preserved in
//  the register stack which is saved in the kernel backing store for the
//  idle thread. This allows us to skip the restore in SwapContext (not yet
//  implemented).
//
//  All callers to SwapContext pass the following arguments:
//
//  -- s0 = Prcb
//  -- s1 = current (previous) thread
//  -- s2 = new (next) thread
//
//  Idle also enters SwapContext at SwapFromIdle with sp pointing to the switch
//  frame for preserved state.
//
//  Since all registers used by the idle are saved in the register stack,
//  no registers need be recomputed on return from SwapContext.
//
//--


        NESTED_ENTRY(KiIdleLoop)
        NESTED_SETUP(0,6,2,0)

//
// allocate and build switch frame
//

        .fframe   SwitchFrameLength
        add       sp = -SwitchFrameLength, sp
        ;;
        add       t0 = ExFltS3+SwExFrame+STACK_SCRATCH_AREA, sp
        add       t1 = ExFltS2+SwExFrame+STACK_SCRATCH_AREA, sp
        ;;

        .save.f   0xC
        stf.spill [t0] = fs3, ExFltS1-ExFltS3         // save fs3
        stf.spill [t1] = fs2, ExFltS0-ExFltS2         // save fs2
        mov       t10 = bs4
        ;;

        .save.f   0x3
        stf.spill [t0] = fs1, ExIntS3-ExFltS1         // save fs1
        stf.spill [t1] = fs0, ExIntS2-ExFltS0         // save fs0
        mov       t11 = bs3
        ;;

        .save.g   0xC
        .mem.offset 0,0
        st8.spill [t0] = s3, ExIntS1-ExIntS3          // save s3
        .mem.offset 8,0
        st8.spill [t1] = s2, ExIntS0-ExIntS2          // save s2
        mov       t12 = bs2
        ;;

        .save.g   0x3
        .mem.offset 0,0
        st8.spill [t0] = s1, ExBrS4-ExIntS1           // save s1
        .mem.offset 8,0
        st8.spill [t1] = s0, ExBrS3-ExIntS0           // save s0
        mov       t13 = bs1
        ;;

        .save.b   0x18
        st8       [t0] = t10, ExBrS2-ExBrS4           // save bs4
        st8       [t1] = t11, ExBrS1-ExBrS3           // save bs3
        mov       t4 = ar.unat                        // captured Nats of s0-s3
        ;;

        .save.b   0x6
        st8       [t0] = t12, ExBrS0-ExBrS2           // save bs2
        st8       [t1] = t13, ExApEC-ExBrS1           // save bs1
        mov       t14 = bs0
        ;;

        .save.b   0x1
        st8       [t0] = t14, ExApLC-ExBrS0           // save bs0
        .savepsp  ar.pfs, ExceptionFrameLength-ExApEC-STACK_SCRATCH_AREA
        st8       [t1] = t16, ExIntNats-ExApEC
        mov       t15 = ar.lc
        ;;

        .savepsp  ar.lc, ExceptionFrameLength-ExApLC-STACK_SCRATCH_AREA
        st8       [t0] = t15
        .savepsp  @priunat, ExceptionFrameLength-ExIntNats-STACK_SCRATCH_AREA
        st8       [t1] = t4                           // save Nats of s0-s3
        nop.i     0
        ;;

        PROLOGUE_END

//
// Register aliases
//

        rpT1      = t0
        rpT2      = t1
        rpT3      = t2
        rT1       = t3
        rT2       = t4
        rT3       = t5

#if !defined(NT_UP)
        rpNumProc = t7
        rpBWait   = t8
        rpDispLock = t9
#endif // !defined(NT_UP)

        rpNextTh  = t10

#if DBG
        rDbgCount = t11
        rKdEnable = t12
#endif //DBG

        pEmty     = pt1
        pIdle     = pt2

#if !defined(NT_UP)
        pLoop     = pt6
#endif // !defined(NT_UP)

#if DBG
        pBrk      = ps2
        pNotZero  = ps3
#endif // DBG


        rpHPI     = loc2
        rHalGP    = loc3
        rKerGP    = loc4

#if !defined(NT_UP)
        rProcNum  = loc5
#endif // !defined(NT_UP)

        //
        // Initialize SwitchFPSR, SwitchPFS, SwitchRp in the switch frame
        //

        mov       rT3 = ar.bsp
        movl      rT2 = KiIdleReturn

        add       rpT1 = SwPFS+STACK_SCRATCH_AREA, sp
        mov       rKerGP = gp                // save the kernel's gp
        mov       rT1 = 0x308                // must match the values specified
                                             // by the alloc instruction at
                                             // the entry of the KiIdleLoop
        ;;

        st8.nta   [rpT1] = rT1, SwRp-SwPFS   // set pfs in the switch frame
        add       rT3 = 0x30, rT3            // 6 local registers
        ;;

        st8.nta   [rpT1] = rT2, SwFPSR-SwRp  // set brp in the switch frame
        movl      rT1 = FPSR_FOR_KERNEL
        ;;
        extr.u    rpT3 = rT3, 0, 9
        st8.nta   [rpT1] = rT1, SwRnat-SwFPSR
        ;;
        cmp.gtu   pt1 = 0x30, rpT3           // Is there a slot for NAT bits?
        st8.nta   [rpT1] = zero, SwBsp-SwRnat
        ;;
(pt1)   add       rT3 = 8, rT3               // Adjust for NAT bits.        
        ;;
        st8.nta   [rpT1] = rT3
        movl      rpT1 = KiPcr + PcPrcb
        ;;

//
// In a multiprocessor system the boot processor proceeds directly into
// the idle loop. As other processors start executing, however, they do
// not directly enter the idle loop and spin until all processors have
// been started and the boot master allows them to proceed.
//

//
//  Setup initial idle loop register values
//

        LDPTRINC  (s0, rpT1, PcCurrentThread-PcPrcb)     // address of PRCB
        add       rpT2 = @gprel(__imp_HalProcessorIdle), gp
        ;;

        LDPTR     (s2, rpT1)                             // idle thread
        LDPTR     (rpT2, rpT2)                           // -> function pointer
        add       rpT3 = PbNumber, s0
        ;;

        ld1       rProcNum = [rpT3]
        ld8       rpHPI = [rpT2], PlGlobalPointer-PlEntryPoint  // entry point
        ;;

//
//  Control reaches here with IRQL at HIGH_LEVEL.  Lower IRQL to
//  DISPATCH_LEVEL and set wait IRQL of idle thread.
//

        ld8       rHalGP = [rpT2]                        // Hal GP
        add       rpT1 = ThWaitIrql, s2         // -> thread WaitIrql
        mov       out0 = DISPATCH_LEVEL         // get dispatch level IRQL
        ;;

        st1       [rpT1] = out0                 // set wait IRQL of idle thread
        br.call.sptk.few.clr brp = KeLowerIrql  // call KeLowerIrql(IRQL)
        ;;

        FAST_ENABLE_INTERRUPTS
        mov       gp = rKerGP
        ;;

//
//  In a multiprocessor system the boot processor proceeds directly into
//  the idle loop. As other processors start executing, however, they do
//  not directly enter the idle loop and spin until all processors have
//  been started and the boot master allows them to proceed.
//

#if !defined(NT_UP)

        add       rpBWait = @gprel(KiBarrierWait), gp
        ;;

Kil_WaitLoop:
        ld4       rT1 = [rpBWait]               // get barrier wait value
        ;;
        cmp.ne    pLoop = zero, rT1             // loop if not zero
(pLoop) br.dpnt.few.clr Kil_WaitLoop            // spin until allowed to proceed

#endif // !defined(NT_UP)


KiIdleReturn:

        mov       s1 = s2                       // s1 <- IdleThread

#if DBG

        mov       rDbgCount = 20 * 1000         // set breakin loop counter

#endif // DBG

        ;;

KiIdleSwitchBlocked:
        mov       rT1 = DISPATCH_LEVEL
        ;;
        SET_IRQL(rT1)

//
//  The following loop is executed for the life of the system.
//

Kil_TopOfIdleLoop::

        mov       gp = rKerGP                             // restore gp

//
// If processor 0, check for debugger breakin, otherwise just check for
// DPCs again.
//

#if DBG

#if !defined(NT_UP)

        cmp4.ne   pNotZero = zero, rProcNum     // pNotZero = not processor zero
(pNotZero) br.cond.dptk.few.clr Kil_CheckDpcList // br if not processor zero

#endif // !defined(NT_UP)

//
// Check if the debugger is enabled,  and whether it is time to poll
// for a debugger breakin.  (This check is only performed on cpu 0).
//

        sub       rDbgCount = rDbgCount,zero,1  // decrement poll counter
        ;;
        add       rKdEnable = @gprel(KdDebuggerEnabled), gp
        cmp.eq    pBrk = rDbgCount, zero        // zero yet?
        ;;

(pBrk)  ld1       rKdEnable = [rKdEnable]       // check if debugger enabled
(pBrk)  mov       rDbgCount = 20 * 1000         // set breakin loop counter
        ;;
(pBrk)  cmp.ne    pBrk = rKdEnable, zero        // pBrk = 1 if Kd enabled
(pBrk)  br.call.dpnt.few.clr brp=KdPollBreakIn  // check if breakin requested
        ;;
        mov       gp = rKerGP                             // restore gp
(pBrk)  cmp.ne    pBrk = v0, zero               // ne => break in requested
        mov       out0 = DBG_STATUS_CONTROL_C
(pBrk)  br.call.sptk brp = DbgBreakPointWithStatus
        ;;
        mov       gp = rKerGP                             // restore gp
#endif // DBG

//
// Disable interrupts and check if there is any work in the DPC list.
//

Kil_CheckDpcList:

//
// Process the deferred procedure call list for the current processor.
//

        FAST_ENABLE_INTERRUPTS                  // give interrupts a chance
        ;;
        srlz.d
        add       rpT1 = PbDpcQueueDepth, s0
        add       rpNextTh = PbNextThread, s0   // -> next thread
        ;;

        FAST_DISABLE_INTERRUPTS                 // disable interrupts
        ;;
        ld4.nta   rT1 = [rpT1]
        mov       out0 = s0                     // PRCB
        ;;

        cmp4.eq   pt0, pt1 = rT1, zero          // if eq, queue empty
        movl      rpT1 = KiPcr+PcDispatchInterrupt
        ;;

 (pt1)  st1       [rpT1] = zero                 // clear dispatch interrupt req
 (pt0)  br.cond.dpnt Kil_CheckNextThread
 (pt1)  br.call.sptk.few.clr brp = KiRetireDpcList
        ;;

        add       rpNextTh = PbNextThread, s0   // -> next thread
        mov       gp = rKerGP                   // restore gp
#if DBG
        mov       rDbgCount = 20 * 1000         // set breakin loop counter
#endif // DBG
        ;;

//
//  Check if a thread has been selected to run on this processor
//

Kil_CheckNextThread:

        ld8       s2 = [rpNextTh]               // next thread
        mov       rT1 = SYNCH_LEVEL
        ;;

        cmp.eq    pIdle = s2, zero              // if no thread to run
(pIdle) br.dpnt.few.clr Kil_Idle                // br to Idle

//
// A thread has been selected for execution on this processor. Acquire
// the context swap lock, get the thread address again (it may have
// changed), clear the address of the next thread in the processor block,
// and call swap context to start execution of the selected thread.
//

        SET_IRQL(rT1)

        FAST_ENABLE_INTERRUPTS

#if !defined(NT_UP)

        add       out0 = PbLockQueue + (LockQueueContextSwapLock * 2 * 8), s0
        add       s2 = PbNextThread, s0       // -> next thread
        br.call.sptk brp = KeAcquireQueuedSpinLockAtDpcLevel
        ;;

//
// Reread the next thread because it may be changed in a MP system.
//
// The selected thread may be blocked from switching at this time
// if it is being scheduled on this processor because it is being
// removed from another processor.   The switch is blocked until
// the thread is completely removed from the other processor.  In
// this case, drop the context swap lock and continue execution as
// if resuming from a context switch.  (The release below returns
// to KiIdleSwitchBlocked above).
//

        ld8       s2 = [s2]                   // reread next thread
        movl      rpT2 = KiIdleSwitchBlocked
        ;;

        add       rpT1 = ThIdleSwapBlock, s2  // &NewThread->IdleSwapBlock
        mov       brp = rpT2
        cmp.eq    pt0 = s2, s1                // check for swap idle to idle
        ;;

        ld1       rT1 = [rpT1], ThState-ThIdleSwapBlock
        add       out0 = PbLockQueue + (LockQueueContextSwapLock * 2 * 8), s0
(pt0)   br.dpnt.few.clr Kil_Idle2Idle         // rare case, idle to idle
        ;;

        cmp.ne    pIdle = rT1, zero
        add       rpT2 = PbCurrentThread, s0  // -> address of current thread
(pIdle) br.call.spnt bt0 = KeReleaseQueuedSpinLockFromDpcLevel
        ;;

#else

        add       rpT2 = PbCurrentThread, s0  // -> address of current thread
        add       rpT1 = ThState, s2
        ;;

#endif // !defined(NT_UP)

                                              // set new thread as current
        st8       [rpT2] = s2, PbNextThread-PbCurrentThread
        mov       rT1 = Running
        ;;

        st1       [rpT1] = rT1                // set new thread state to running
        st8.rel   [rpT2] = zero               // clear address of next thread

//
// Swap context to new thread
// (returns to KiIdleReturn above).
//

        br.call.sptk.few.clr brp = SwapFromIdle
        ;;

//
// There are no entries in the DPC list and a thread has not been selected
// for execution on this processor. Call the HAL so power managment can be
// performed.
//
// N.B. The HAL is called with interrupts disabled. The HAL will return
//      with interrupts enabled.
//

Kil_Idle:
        mov       bt0 = rpHPI
        movl      rT1 = Kil_TopOfIdleLoop
        ;;

        mov       gp = rHalGP                   // set Hal GP
        mov       brp = rT1
        br.call.dpnt.few.clr   bt1 = bt0        // call HalProcessorIdle
        ;;                                      // does not return here.


//
// In the event that a thread was scheduled to run on this processor
// but before the idle loop picked it up, it was made inelligible for
// this processor and there is no other thread to run, the idle thread
// will have been selected as the NextThread for this processor and
// this processor will have been marked idle in KiIdleSummary.
//
// It is then possible for another thread to be selected for this processor
// between the time the idle loop picks up the NextThread field and clears
// it.
//
// To avoid this rare case, if the switch is from the idle thread to the
// idle thread, we need to drop the context swap lock, acquire the 
// dispatcher lock and if the next thread field is still the idle thread,
// clear it and continue to idle.   If it is no longer the idle thread,
// switch to that thread.
//

Kil_Idle2Idle:
        add       s2 = PbNextThread, s0       // get &Prcb->NextThread
        br.call.sptk brp = KeReleaseQueuedSpinLockFromDpcLevel
        ;;

        add       out0 = PbLockQueue + (LockQueueDispatcherLock * 2 * 8), s0
        br.call.sptk brp = KeAcquireQueuedSpinLockAtDpcLevel
        ;;

        add       out0 = PbLockQueue + (LockQueueDispatcherLock * 2 * 8), s0
        ld8       rT1 = [s2]                 // get Prcb->NextThread
        ;;

        cmp.eq    pt0 = rT1, s2              // check if still Idle Thread
        ;;

(pt0)   st8       [s2] = zero                // clear Prcb->NextThread
        br.call.sptk brp = KeReleaseQueuedSpinLockFromDpcLevel
        ;;

        br          KiIdleSwitchBlocked      // loop.

        NESTED_EXIT(KiIdleLoop)

#if !defined(NT_UP)

//++
// Routine:
//
//      VOID
//      KiOSRendezvous (
//           )
//
// Routine Description:
//
//      OS rendezvous entry point called from SAL for MP boot.
//      Establish kernel mapping and rfi to KiInitializeSystem with
//      translation turned on.
//
// Arguments:
//
// Return Value:
//
//       None.
//
//--

        .global MmSystemParentTablePage
        .global MiNtoskrnlPhysicalBase
        .global MiNtoskrnlVirtualBase
        .global MiNtoskrnlPageShift

        LEAF_ENTRY(KiOSRendezvous)

        mov         t5 = ip                     // get the physical addr for KiOSRendezvous

        mov         psr.l = zero                // initialize psr.l
        movl        t0 = KSEG0_BASE
        ;;

        mov         ar.rsc = zero               // invalidate register stack
        mov         t1 = (START_GLOBAL_RID << RR_RID) | (PAGE_SHIFT << RR_PS) | (1 << RR_VE)
        ;;

//
// Initialize Region Register for kernel
//

        loadrs
        mov         rr[t0] = t1

//
// Setup translation for kernel/hal binary.
//
        movl    t0 = KSEG0_BASE;
        ;;
        movl    t2 = MiNtoskrnlPhysicalBase
        movl    t4 = MiNtoskrnlPageShift
        movl    t0 = MiNtoskrnlVirtualBase
        movl    t3 = KiOSRendezvous
        ;;

        sub     t6 = t2, t3
        sub     t7 = t4, t3
        sub     t0 = t0, t3
        ;;
        add     t6 = t5, t6     // get a physical addr for MiNtoskrnlPhysicalBase
        add     t7 = t5, t7     // get a physical addr for MiNtoskrnlPageShift
        add     t0 = t5, t0     // get a physical addr for MiNtoskrnlVirtualBase
        ;;
        ld8     t6 = [t6]
        ld4     t7 = [t7]
        ld8     t0 = [t0]
        movl    t8 = VALID_KERNEL_EXECUTE_PTE
        ;;
        mov     cr.ifa = t0
        or      t2 = t6, t8
        shl     t1 = t7, PS_SHIFT
        ;;
        mov     cr.itir = t1
        mov     t3 = DTR_KERNEL_INDEX
        ;;

        itr.d   dtr[t3] = t2
        ;;
        itr.i   itr[t3] = t2


//
// Setup VHPT
//

        movl        t0 = KiIA64PtaContents
        movl        t3 = KiOSRendezvous
        ;;
        sub         t6 = t0, t3
        ;;
        add         t6 = t5, t6
        ;;
        ld8         t6 = [t6]
        ;;
        mov         cr.pta = t6

//
// Turn on address translation
//
        movl        t1 = (1 << PSR_BN) | (1 << PSR_IT) | (1 << PSR_DA) | (1 << PSR_RT) | (1 << PSR_DT) | (1 << PSR_IC)
        ;;

        mov         cr.ipsr = t1

//
// Branch to KiInitializeSystem
//
//      Need to do a "rfi" in order set "it" bits in the PSR.
//      This is the only way to set them.
//
        movl        t0 = KiOSRendezvousStub
        ;;
        mov         cr.iip = t0
        ;;
        rfi
        ;;

        .global     KeLoaderBlock

KiOSRendezvousStub:

        alloc       t0 = 0,0,1,0
        movl        gp = _gp
        mov         t9 = PAGE_SHIFT << PS_SHIFT
        movl        t8 = KADDRESS_BASE
        ;;

//
// Set up the VHPT table
//

        thash       t8 = t8
        add         t7 = @gprel(MmSystemParentTablePage), gp
        ;;

        ld8         t7 = [t7]
        rsm         1 << PSR_IC                   // PSR.ic = 0
        ;;
        srlz.d

        thash       t8 = t8
        movl        t6 = PDR_TR_INITIAL
        ;;
//
// Install DTR for the kernel parent page table
//

        thash       t8 = t8
        mov         t10 = DTR_KTBASE_INDEX
        shl         t7 = t7, PAGE_SHIFT
        ;;

        mov         cr.ifa = t8
        or          t7 = t7, t6
        mov         cr.itir = t9
        ;;

        itr.d       dtr[t10] = t7

        ssm         1 << PSR_IC                  // PSR.ic = 1
        ;;
        srlz.i                                   // I serialize
        add         out0 = @gprel(KeLoaderBlock), gp
        ;;
        ld8         out0 = [out0]               // dereference pointer
        movl        t0 = KiInitializeSystem
        ;;

        mov         bt0 = t0
        br.call.sptk brp = bt0                  // branch to entry point

        LEAF_EXIT(KiOSRendezvous)

#endif // !defined(NT_UP)


