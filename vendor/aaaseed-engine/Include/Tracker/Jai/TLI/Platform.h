/****************************************************************************
 (c) 2004 by STEMMER IMAGING

 License: This file is published under the license of the EMVA GenICam  Standard Group. 
 A text file describing the legal terms is included in  your installation as 'license.txt'. 
 If for some reason you are missing  this file please contact the EMVA or visit the website
 (http://www.genicam.org) for a full copy.

 THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP 
 OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, 
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO, 
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS; 
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) 
 ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

/// \file     $Source: /cvs/genicam/genicam/library/CPP/include/TLI/Platform.h,v $
/// \brief    GenICam TL Plattform helper
/// \version  $Revision: 1.5 $
/// \author   rst ( SI )
/// \date     $Date: 2006/07/18 13:31:53 $


#ifndef GC_PLATFORM_H_
#define GC_PLATFORM_H_

#if 0

#include <Base/GCBase.h>

#ifdef WIN32
  #include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
  #ifdef _GCTLIDLL
    #define GC_IMPORT_EXPORT __declspec(dllexport) 
    //#define GC_IMPORT_EXPORT 
  #else
    #define GC_IMPORT_EXPORT 
  #endif
  #define GC_CALLTYPE __stdcall
  
  #if ! defined EXTERN_C
    #define EXTERN_C extern "C"
  #endif
#else

  #define GC_IMPORT_EXPORT 
  #define GC_CALLTYPE 

  #define GC_IMPORT_EXPORT 
  
  #if ! defined EXTERN_C
    #define EXTERN_C extern "C"
  #endif

#endif



#ifdef __cplusplus
}
#endif

#endif //#if 0

#endif //GC_PLATFORM_H_