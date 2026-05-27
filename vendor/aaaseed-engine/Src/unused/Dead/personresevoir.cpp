//=============================================================================
// Copyright © 2000 Point Grey Research, Inc. All Rights Reserved.
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


//=============================================================================
// Includes
//=============================================================================
//#include "stdafx.h"
#include "personresevoir.h"
#include "err.h"



//=============================================================================
// Class Definitions
//=============================================================================
//=============================================================================
// Construction/Destruction
//=============================================================================


c_person_reservoir::c_person_reservoir( INT32 allocated_people )
{
        p_people = new INT_TRACKED_PERSON[allocated_people];
        if( p_people == (PINT_TRACKED_PERSON)NULL )
        {
                ERR_PRINT_STRING( "Censys 3d, tracking : error allocating tracked people array." );
        }


        //   ASSERT( p_people != (PINT_TRACKED_PERSON)NULL );


        p_people_pointers = new PINT_TRACKED_PERSON[allocated_people];
        if( p_people_pointers == (PPINT_TRACKED_PERSON)NULL )
        {
                ERR_PRINT_STRING( "Censys 3d, tracking : error allocating tracked people pointers array." );
        }
        //   ASSERT( p_people_pointers != (PPINT_TRACKED_PERSON)NULL );


        // Set pointers & indecies
        for( INT32 i = 0; i < allocated_people; i++ )
        {
                p_people_pointers[i] = &p_people[i];
                p_people[i].nPeoplePointerIndex = i;
        }


        nb_total_people = allocated_people;
        next_free_person = 0;
}


c_person_reservoir::~c_person_reservoir()
{
        delete [] p_people;
        delete [] p_people_pointers;
}


// Gets a new person. Returns FALSE if there are non left, TRUE otherwise
BOOL    c_person_reservoir::get_new_person( PTRACKED_PERSON &pPerson )
{
        if( next_free_person == nb_total_people )
        {
                return FALSE;
        }
        else
        {
                pPerson = &p_people_pointers[ next_free_person ]->person;
                next_free_person++;
                return TRUE;
        }
}


// Releases a person. Returns FALSE of pPerson was not obtained
// using get_new_person(), TRUE if the person was successfully released
BOOL    c_person_reservoir::release_person( PTRACKED_PERSON pPerson )
{
        if( next_free_person <= 0 )
        {
                return FALSE;
        }
        else
        {
                pPerson->nID = -1;


                pPerson->path.clear();


                if( pPerson->path.size() == 0 )
                {
                        ERR_PRINT_STRING( "Censys 3d, tracking : person path size is zero." );
                }
                //      ASSERT( pPerson->path.size() == 0 );


                PINT_TRACKED_PERSON pIntPerson = (PINT_TRACKED_PERSON)pPerson;



                INT32 nLastNonFreePersonIndex = next_free_person - 1;
                INT32 nNewlyFreePersonIndex = pIntPerson->nPeoplePointerIndex;


                PINT_TRACKED_PERSON pLastNonFreePerson = p_people_pointers[ nLastNonFreePersonIndex ];
                PINT_TRACKED_PERSON pNewlyFreePerson = pIntPerson;


                p_people_pointers[ nLastNonFreePersonIndex ] = pNewlyFreePerson;
                pNewlyFreePerson->nPeoplePointerIndex = nLastNonFreePersonIndex;


                p_people_pointers[ nNewlyFreePersonIndex ] = pLastNonFreePerson;
                pLastNonFreePerson->nPeoplePointerIndex = nNewlyFreePersonIndex;


                // Add 1 more free person to free people heap
                next_free_person--;


                return TRUE;
        }


}


// Returns the number of non-free people
INT32   c_person_reservoir::used_people_count()
{
        return next_free_person;
}


// Retreives the used person corresponding to nIndex
// nIndex must be less than used_people_count()
BOOL    c_person_reservoir::get_used_person( INT32 nIndex, PTRACKED_PERSON &pPerson )
{
        if( nIndex >= used_people_count() || nIndex < 0 )
        {
        //      ASSERT( FALSE );
                ERR_PRINT_STRING( "Censys 3d, tracking : invalid index in get_used_person." );
                return FALSE;
        }
        else
        {
                pPerson = &p_people_pointers[ nIndex ]->person;
                return TRUE;
        }
}
