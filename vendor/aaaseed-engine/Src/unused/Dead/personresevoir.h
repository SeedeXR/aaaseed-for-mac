//=============================================================================
// Copyright � 2000 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================



#ifdef AAA_PERSONRESEVOIR_H
#error "PERSONRESEVOIR_H included more than once."
#endif
#define AAA_PERSONRESEVOIR_H 1


#pragma warning( disable : 4786 )


// File: personreservoir.h
// Description: Header file for class PersonReservoir. PersonReservoir
// is used to maintain a pool of people that can each be obtained, used, and 
// subsequently released, without the heap overhead.


//=============================================================================
// Includes
//=============================================================================
#ifndef __CENSYS_H__
#include "censys.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif



//#include <afxmt.h>


#include <vector>
using namespace std;


// Vector of path points
typedef vector<PATHPOINT> PathPointVector;


// Structure of person
typedef struct _TRACKED_PERSON
{
        INT32 nID; // persons ID
        PathPointVector path; // path array
} TRACKED_PERSON, *PTRACKED_PERSON, **PPTRACKED_PERSON;



// Maintains a list of allocated people, and pointers
// to them.
class c_person_reservoir
{
public:
        c_person_reservoir( INT32 allocated_people = 1000 );


        ~c_person_reservoir();


        // Gets a new person. Returns FALSE if there are non left, TRUE otherwise
        BOOL get_new_person( PTRACKED_PERSON &pPerson );


        // Releases a person. Returns FALSE of pPerson was not obtained
        // using get_new_person(), TRUE if the person was successfully released
        BOOL release_person( PTRACKED_PERSON pPerson );


        // Returns the number of non-free people
        int used_people_count();


        // Retreives the used person corresponding to nIndex
        // nIndex must be less than used_people_count()
        BOOL get_used_person( INT32 nIndex, PTRACKED_PERSON &pPerson );


private:


        typedef struct _INT_TRACKED_PERSON
        {
                TRACKED_PERSON person;
                INT32 nPeoplePointerIndex;
        } INT_TRACKED_PERSON, *PINT_TRACKED_PERSON, **PPINT_TRACKED_PERSON;


        PPINT_TRACKED_PERSON  p_people_pointers;
        PINT_TRACKED_PERSON p_people;


        INT32 nb_total_people;
        INT32 next_free_person;


};


#endif // !__PERSONRESEVOIR_H__
