#include <stdio.h>
#include "winscard.h"

/*
 * !!!!! PROTOTYPES !!!!!
 */
int checkSCardResult(char* method_name, LONG result);


int main(void)
{

  LONG rv;                      // For the Result of SC-Methods
  SCARDCONTEXT scard_context;   // Context for Methods

  // Establish Context ~> Has to be the first thing to do!
  rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &scard_context);
  checkSCardResult("SCardEstablishContext", rv);
}


/*
 * Checks the Result of and SCard-Method
 *
 * param[IN]   method_name    Name of the called Method (for printing purpose)
 * param[IN]   result         Result of the SCard-Method
 *
 * return      ret_val        Returns -1 if an error occured, otherwise 0
 */
int checkSCardResult(char* method_name, LONG result)
{
  int ret_val;

  if ( result != SCARD_S_SUCCESS )
  {
    // something went wrong
    printf("%s: %s\n", method_name, pcsc_stringify_error(result));
    ret_val = -1;
  }
  else
  {
    // all Good
    printf("%s: Result OK\n", method_name);
    ret_val = 0;
  }

  return ret_val;
}