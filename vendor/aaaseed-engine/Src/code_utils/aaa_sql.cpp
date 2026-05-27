#include "aaa_sql.h"
#include "mysql.h"
#include "err.h"
#include "stdio.h"

MYSQL mysql;
//typedef unsigned __int64 INT64;

void sql_query( CHAR* query_string)
{
MYSQL_RES *result;
unsigned int num_fields;
unsigned int num_rows;

	if( mysql_query( &mysql, query_string) )
		{
		ERR_PRINT_STRING( "AAASQL: MYSQL: Failed to query: Error: %s", mysql_error(&mysql) );
		}
	else // query succeeded, process any data returned by it
		{
		result = mysql_store_result( &mysql);
		if (result)	// there are rows
			{
			num_fields = mysql_num_fields(result);
				{
				MYSQL_FIELD *field;
				while((field = mysql_fetch_field(result)))
					{
					GOOD_PRINT_STRING("field name %s", field->name);
					}
				}
			// retrieve rows, then call mysql_free_result(result
			num_rows = (unsigned long) mysql_num_rows(result);
			GOOD_PRINT_STRING( "Number of rows: %lu", num_rows );
			
/*
			for( i=0; i<num_rows; i++)
				{
				mysql_data_seek( result, i);
				}
*/

MYSQL_ROW row;

			while( (row = mysql_fetch_row(result)) )
				{
				unsigned long *lengths;
				lengths = mysql_fetch_lengths(result);
				for( INT64 i = 0; i < num_fields; i++ )
					{
					PRINT_STRING("[%.*s] ", (int) lengths[i], row[i] ? row[i] : "NULL");
					}
				PRINT_STRING("\n");
				}

			if( !mysql_eof(result) )	// mysql_fetch_row() failed due to an error
				{
				fprintf( stderr, "Error: %s\n", mysql_error(&mysql));
				}

			mysql_free_result(result);
			}
		else	// mysql_store_result() returned nothing; should it have?
			{
			if(mysql_field_count(&mysql) == 0)
				{
				// query does not return data
				// (it was not a SELECT)
				num_rows = mysql_affected_rows(&mysql);
				}
			else // mysql_store_result() should have returned data
				{
				fprintf(stderr, "Error: %s\n", mysql_error(&mysql));
				}
			}
		}

}


void sql_connect()
{
	mysql_init( &mysql);
	mysql_options( &mysql, MYSQL_READ_DEFAULT_GROUP, "AAASeed");
	if( !mysql_real_connect( &mysql, "", "Maa", "", "menagerie", 0, NULL, 0 ) )
		{
		ERR_PRINT_STRING( "AAASQL: MYSQL: Failed to connect to database: Error: %s", mysql_error(&mysql) );
		}

	sql_query( "select * from pet");
}

