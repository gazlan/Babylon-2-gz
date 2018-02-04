/* ******************************************************************** **
** @@ BGZ Source File
** @  Copyrt : 
** @  Author : 
** @  Update :
** @  Update :
** @  Notes  :
** ******************************************************************** */

/* ******************************************************************** **
**                uses pre-compiled headers
** ******************************************************************** */

#include "stdafx.h"

#include "..\shared\file_find.h"
#include "..\shared\mmf.h"
#include "..\shared\file.h"

/* ******************************************************************** **
** @@                   internal defines
** ******************************************************************** */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#endif 

/* ******************************************************************** **
** @@                   internal prototypes
** ******************************************************************** */

/* ******************************************************************** **
** @@                   external global variables
** ******************************************************************** */

extern DWORD   dwKeepError = 0;

/* ******************************************************************** **
** @@                   static global variables
** ******************************************************************** */

/* ******************************************************************** **
** @@                   real code
** ******************************************************************** */

/* ******************************************************************** **
** @@ Proceed()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 31 Jul 2007
** @  Notes  :
** ******************************************************************** */

static bool Proceed
(
   const char * const   pszFileName
)
{
   char     pszGZName[_MAX_PATH];
   char     pszDrive [_MAX_DRIVE];
   char     pszDir   [_MAX_DIR];
   char     pszFName [_MAX_FNAME];
   char     pszExt   [_MAX_EXT];

   _splitpath(pszFileName,pszDrive,pszDir,pszFName,pszExt);
   _makepath( pszGZName,pszDrive,pszDir,pszFName,"gz");

   MMF*     pMF = new MMF;

   ASSERT(pMF);

   if (!pMF)
   {
      // Error !
      return false;
   }

   if (!pMF->OpenReadOnly(pszFileName))
   {
      // Error !
      pMF->Close();
      delete pMF;
      pMF = NULL;
      return false;
   }

   BYTE*    pBuf   = pMF->Buffer();
   DWORD    dwSize = pMF->Size();

   // Reading and testing first 4 byte.
   // BGL file signature must be : 0x12340001 or 0x12340002
   DWORD    dwSignature = *(DWORD*)pBuf;

   if ((dwSignature != 0x01003412) && (dwSignature != 0x02003412))
   {
      // Is NOT .BGL file
      pMF->Close();
      delete pMF;
      pMF = NULL;
      return false;
   }

   WORD     wGZStart = (WORD)((pBuf[4] << 8) + pBuf[5]);

   HANDLE   hGZ = CreateFile(pszGZName);

   VERIFY(hGZ != INVALID_HANDLE_VALUE);

   if (hGZ == INVALID_HANDLE_VALUE)
   {
      // Error
      pMF->Close();
      delete pMF;
      pMF = NULL;
      return false;
   }
   
   WriteBuffer(hGZ,pBuf + wGZStart,dwSize - wGZStart);

   CloseHandle(hGZ);
   hGZ = INVALID_HANDLE_VALUE;

   pMF->Close();

   delete pMF;
   pMF = NULL;

   return true;
}

/* ******************************************************************** **
** @@ ShowHelp()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update : 31 Jul 2007
** @  Notes  :
** ******************************************************************** */

void ShowHelp()
{
   const char  pszCopyright[] = "-*-   bgz 1.0   *   (c) Gazlan, 2007   -*-";
   const char  pszDescript [] = "Babylon packed DB to .gz archive converter";
   const char  pszE_Mail   [] = "complains_n_suggestions direct to gazlan@yandex.ru";

   printf("%s\n\n",pszCopyright);
   printf("%s\n\n",pszDescript);
   printf("Usage: bgz.com wildcards\n\n");
   printf("%s\n",pszE_Mail);
}

/* ******************************************************************** **
** @@ main()
** @  Copyrt : 
** @  Author : 
** @  Modify :
** @  Update :
** @  Notes  :
** ******************************************************************** */

int main(int argc,char** argv)
{
   if ((argc < 2) || (argc > 4))
   {
      ShowHelp();
      return 0;
   }

   if (argc == 2 && ((!strcmp(argv[1],"?")) || (!strcmp(argv[1],"/?")) || (!strcmp(argv[1],"-?")) || (!stricmp(argv[1],"/h")) || (!stricmp(argv[1],"-h"))))
   {
      ShowHelp();
      return 0;
   }

   FindFile   FF;

   FF.SetMask(argv[1]);

   int      iTotal = 0;

   while (FF.Fetch())
   {
      if ((FF._w32fd.dwFileAttributes | FILE_ATTRIBUTE_NORMAL) && !(FF._w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      {
         if (Proceed(FF._w32fd.cFileName))
         {
            ++iTotal;
         }
      }
   }

   printf("\n[i]: Processed %d file(s) total.\n",iTotal);

   return 0;
}

/* ******************************************************************** **
** @@                   The End
** ******************************************************************** */
