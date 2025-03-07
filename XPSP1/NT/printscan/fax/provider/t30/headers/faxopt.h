/* Copyright Microsoft Corp 1993
 *
 *      faxopt.c
 *
 * Include file for faxopt.dll
 *
 *
 *      Created:        1/10/93
 *      Author:         mikegins
 */

#ifndef __FAXOPT_H__
#define __FAXOPT_H__ 1

#ifndef __FAXSEC_H__

#define SEC_SUCCESS         0   /* Operation succeeded          */
#define SEC_BADFILE         1   /* File cannot be opened        */
#define SEC_KEYTOOSMALL     2   /* Keybuffer too small          */
#define SEC_NOMATCH         3   /* No key of requested type found for
                  specified keyholder, or too many matches
                  were found (ie: joe@+123 and mart@+123 both
                  match @+123 with equal weight */
#define SEC_FILECORRUPT     4   /* File integrity check failed  */
#define SEC_NOMEMORY        5   /* Aborted due to low memory conditions */
#define SEC_BADPTR          6   /* An invalid pointer was passed in */
#define SEC_USERABORT       7   /* User aborted operation       */
#define SEC_BADPASS         8   /* Invalid password             */
#define SEC_ABORT           9   /* Callback caused abort        */
#define SEC_BUSY           10   /* Concurrent job already running */
#define SEC_WRONGKEY       11   /* Wrong key used in decryption */
#define SEC_NEWERVERSION   12  /* File generated by newer version of software,
                  and we cannot decrypt it */
   

#define SIG_SUCCESS     0   /* Operation succeeded */
#define SIG_BADINFILE   1   /* Bad input file */
#define SIG_BADOUTFILE  2   /* Bad output file */
#define SIG_FORGED      3   /* Signature does not match keyset */
#define SIG_AUTHENTIC   4   /* Signature matches keyset */
#define SIG_NOTVERIFIED 5   /* No key found, so can not verify signature */
#define SIG_BADPASS     6   /* Bad password or incorrect session handle */
#define SIG_NOMEM       7   /* Out of memory error */
#define SIG_ABORT       8   /* Callback caused abort */
#define SIG_BUSY        9   /* Concurrent job already running */

#endif

#define FAXOPTBASEATT   0x6000
#define iATTlogostr     (FAXOPTBASEATT+ 0)
#define iATTlogo1       (FAXOPTBASEATT+ 1)
#define iATTlogo2       (FAXOPTBASEATT+ 2)
#define iATTlogo3       (FAXOPTBASEATT+ 3)
#define iATTlogo4       (FAXOPTBASEATT+ 4)
#define iATTlogo5       (FAXOPTBASEATT+ 5)
#define iATTinccover    (FAXOPTBASEATT+ 6)
#define iATTtranshour   (FAXOPTBASEATT+ 7)
#define iATTtransquart  (FAXOPTBASEATT+ 8)
#define iATTdelivform   (FAXOPTBASEATT+ 9)
#define iATTimagequal   (FAXOPTBASEATT+10)
#define iATTprintorient (FAXOPTBASEATT+11)
#define iATTcheapbeghr  (FAXOPTBASEATT+13)
#define iATTcheapendhr  (FAXOPTBASEATT+14)
#define iATTcheapbegq   (FAXOPTBASEATT+15)
#define iATTcheapendq   (FAXOPTBASEATT+16)
#define iATTnumret      (FAXOPTBASEATT+17)
#define iATTminbetret   (FAXOPTBASEATT+18)
#define iATTpapersize   (FAXOPTBASEATT+19)
#define iATTtransprio   (FAXOPTBASEATT+20)
#define iATTencrypt     (FAXOPTBASEATT+21)
#define iATTsign        (FAXOPTBASEATT+22)
#define iATTIncludeBody (FAXOPTBASEATT+23)
#define iATTSimplePwd   (FAXOPTBASEATT+24)

#define SelectUser Func20
WORD FAR PASCAL SelectUser(LPSTR userid, DWORD userinst, LPSTR olduser,
              LPSTR selecteduser);
#define GetSimplePwd Func21
WORD FAR PASCAL GetSimplePwd(LPSTR pwd);
#define ImportKeysDlg Func22
WORD FAR PASCAL ImportKeysDlg(LPSTR filename, HWND hDlg);

/* 0 = success, 1 = End of Job, 2 = failure */
typedef WORD FAR PASCAL ReadCB(DWORD hf, LPBUFFER FAR *buf);
typedef WORD FAR PASCAL WriteCB(DWORD hf, LPBUFFER buf);
typedef void FAR PASCAL LogCB(LPSTR userid, DWORD userinst);

/* 0 = continue, 1 = abort */
typedef WORD FAR PASCAL SigCB(DWORD hf, LPSTR username, WORD result);

#if FALSE
#define MailStarting Func00
void FAR PASCAL MailStarting(LogCB *LoginCB, LogCB *LogoutCB, LPSTR userid);
#define MailShutdown Func01
void FAR PASCAL MailShutdown(void);
#define EncryptInit Func02
WORD FAR PASCAL EncryptInit(LPSTR userid, DWORD userinst, LPSTR destuser,
               WORD FAR *instance, LPBUFFER buf);
#define EncryptData Func03
WORD FAR PASCAL EncryptData(DWORD userinst, WORD inst, LPBUFFER buf);
#define DecryptInit Func04
WORD FAR PASCAL DecryptInit(LPSTR userid, DWORD userinst, LPSTR srcuser,
               WORD FAR *instance, LPBUFFER buf);
#define DecryptData Func05
WORD FAR PASCAL DecryptData(DWORD userinst, WORD inst, LPBUFFER buf);
#define SimpleEncryptInit Func06
WORD FAR PASCAL SimpleEncryptInit(LPSTR pwd, WORD FAR *instance, LPBUFFER buf);
#define SimpleEncryptData Func07
WORD FAR PASCAL SimpleEncryptData(WORD inst, LPBUFFER buf);
#define SimpleDecryptInit Func08
WORD FAR PASCAL SimpleDecryptInit(LPSTR pwd, WORD FAR *instance, LPBUFFER buf);
#define SimpleDecryptData Func09
WORD FAR PASCAL SimpleDecryptData(WORD inst, LPBUFFER buf);
#ifndef DELRINA
#define SignFile Func10
#endif
WORD FAR PASCAL SignFile(ReadCB *readCB, DWORD readHand, WriteCB *writeCB,
            DWORD writeHand, LPSTR userid, DWORD userinst,
            LPSTR filename, WORD FAR *HeaderSize);
#define UnSignFile Func11
WORD FAR PASCAL UnSignFile(ReadCB FAR *readCB, DWORD readHand,
              WriteCB FAR *writeCB,DWORD writeHand);
#ifndef DELRINA
#define ExistsPubKey Func12
#endif
BOOL FAR PASCAL ExistsPubKey(LPSTR name);
#define IsUserLoggedIn Func13
WORD FAR PASCAL IsUserLoggedIn(void);
#define SignFileHeaderSize Func14
WORD FAR PASCAL SignFileHeaderSize(ReadCB FAR *readCB, DWORD readHand,
                  WORD FAR *headersize, LPSTR filename);
#define GetSignatures Func15
WORD FAR PASCAL GetSignatures(ReadCB FAR *readCB, DWORD readHand,
                 SigCB FAR *sigCB, DWORD sigHand, LPSTR filename);

/* Call to start hashing bft header.  Returns 0 for failure, or non-zero
   context for passing to SendBFTHash and DoneBFTHash */
#define StartBFTHash Func23
WORD FAR PASCAL StartBFTHash(void);

/* Pass in context from StartBFTHash, and buffer with length */
#define SendBFTHash Func24
void FAR PASCAL SendBFTHash(WORD Context, BYTE FAR *buf, WORD len);

/* Pass in context after done calling SendBFTHash.  If salt points to 3 bytes
   which are 0, a salt is generated and returned, else the salt is used.
   Key must be 5 bytes, from which the actual key is derived.  Res must be 16
   bytes long to receive hashed encrypted result. */

#define DoneBFTHash Func25
void FAR PASCAL DoneBFTHash(WORD Context, BYTE FAR *salt, BYTE FAR *key,
               BYTE FAR *res);
#endif
#endif
