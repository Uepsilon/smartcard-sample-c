#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "winscard.h"
#include "wintypes.h"

/**************************/
/******* PROTOTYPES *******/
/**************************/
int     checkSCardResult  (char*              method_name,
                           LONG               result);

void    runCmd            (SCARDHANDLE        card,
                           SCARD_IO_REQUEST   *protStruct,
                           BYTE               *cmd,
                           DWORD              cmdLen,
                           BYTE               *receiveBuffer,
                           DWORD              receiveBufferLen);

int     checkCardResult   (BYTE               *receiveBuffer,
                           DWORD              receiveBufferLen);

/**************************/
/******** DEFINES** *******/
/**************************/
#define MAX_READERS                 1
#define MAX_RECEIVE_BUFFER_LENGTH   125


int main(void)
{

  LONG          rv;             // For the Result of SC-Methods

  SCARDCONTEXT  cardContext;    // Context for Methods
  SCARDHANDLE   card;           // The SmartCard-Handle

  BYTE          cardAtr[MAX_ATR_SIZE];
  DWORD         cardAtrLen;     // Length of card attributes
  DWORD         cardStatus;     // Status of card (inserted, etc.)
  DWORD         usedProtocol;   // Information about the Protocol used

  LPSTR         foundReaders;
  DWORD         readersLen;                 // Length of readers
  char          *readersList[MAX_READERS];  // Pointerlist of all Readers
  BYTE          readerName[MAX_READERNAME];

  int           i;
  int           numOfReaders = 0;
  char*         ptr;

  BYTE          receiveBuffer[MAX_RECEIVE_BUFFER_LENGTH];
  DWORD         receiveBufferLen;
  SCARD_IO_REQUEST  protStruct;


  /* cmds */
  BYTE cmdReadUID[] = {0xFF, 0xCA, 0x00, 0x00, 0x00};

  // Establish Context ~> Has to be the first thing to do!
  rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &cardContext);
  if ( rv != SCARD_S_SUCCESS)
  {
    printf("Cannot connect to PCSC: %lX\n", rv);
    return 1;
  }

  // Get List of all available Readersprintf("CMD: ");
  rv = SCardListReaders(cardContext, NULL, NULL, &readersLen);
  if (rv != SCARD_S_SUCCESS)
  {
    printf("SCardListReader: %lX\n", rv);
  }

  foundReaders = malloc(sizeof(char)*readersLen);
  if (foundReaders == NULL)
  {
    printf("malloc: not enough memory\n");
    return 1;
  }

  rv = SCardListReaders(cardContext, NULL, foundReaders, &readersLen);
  if (rv != SCARD_S_SUCCESS)
  {
    printf("SCardListReader: %lX\n", rv);
  }

  ptr = foundReaders;
  while ((*ptr != '\0') && (numOfReaders < MAX_READERS))
  {
    printf("Reader #%d: %s\n", (numOfReaders+1), ptr);
    readersList[numOfReaders] = ptr;
    ptr += strlen(ptr)+1;
    numOfReaders++;
  }

  if (numOfReaders == 0)
  {
    printf("No reader found\n");
    return 1;
  }

  while ( SCardConnect(cardContext, readersList[0], SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T0, &card, &usedProtocol) != SCARD_S_SUCCESS )
  {
    /* Wait for Card to be inserted */
  }

  /* set prot type */
  switch(usedProtocol)
  {
    case SCARD_PROTOCOL_T0:
      protStruct = *SCARD_PCI_T0;
      break;

    case SCARD_PROTOCOL_T1:
      protStruct = *SCARD_PCI_T1;
      break;
  }

  /* Read Card Status */
  cardAtrLen = sizeof(cardAtr);
  rv = SCardStatus(card, readerName, &readersLen, &cardStatus, &usedProtocol, cardAtr, &cardAtrLen);
  if ( rv == SCARD_S_SUCCESS )
  {
    printf(" Reader: %s (length %ld bytes)\n", readerName, readersLen);
    printf(" State: 0x%lX\n", cardStatus);
    printf(" Prot: %ld\n", usedProtocol);
    printf(" ATR (length %ld bytes):", cardAtrLen);
    for (i=0; i<cardAtrLen; i++)
      printf(" %02X", cardAtr[i]);
    printf("\n");
  }

  /* Read UID from Card */
  runCmd(card, &protStruct, cmdReadUID, sizeof(cmdReadUID), receiveBuffer, sizeof(receiveBuffer));



  /* Disconnect Card */
  rv = SCardDisconnect(card, SCARD_UNPOWER_CARD);
  checkSCardResult("SCardDisconnect", rv);

  /* Release Context */
  rv = SCardReleaseContext(cardContext);
  checkSCardResult("SCardReleaseContext", rv);

  // Release allocated Memory for the Readers
  free(foundReaders);

  return 0;
}


/*
 * Transmits CMD to Card and returns Result
 *
 * param[IN]      card                Handle for the Card
 * param[IN]      protStruct          Pointer to the current protocole Structure
 * param[IN]      cmd                 Pointer to the current CMD
 * param[IN]      cmdLen              Length of the current CMD
 * param[OUT]     receiveBuffer       Pointer to the current ReceiveBuffer
 * param[IN/OUT]  receiveBufferLen    Max length of the ReceiveBuffer
 *
 * return         None
 */
void runCmd(SCARDHANDLE card, SCARD_IO_REQUEST *protStruct, BYTE *cmd, DWORD cmdLen, BYTE *receiveBuffer, DWORD receiveBufferLen)
{
  LONG          rv;             // For the Result of SC-Methods
  int i;

  rv = SCardTransmit(card, protStruct, cmd, cmdLen, protStruct, receiveBuffer, &receiveBufferLen);

  printf("\nSending CMD: ");
  for(i=0; i < cmdLen; i++)
    printf(" %02X", cmd[i]);
  printf("\n");

  if (rv == SCARD_S_SUCCESS )
  {
    if ( checkCardResult(receiveBuffer, receiveBufferLen) )
    {
      receiveBufferLen -= 2;

      printf("Result (HEX): ");
      for(i=0; i < receiveBufferLen; i++)
        printf(" %02X", receiveBuffer[i]);
      printf("\n");
      printf("Result (DEC): ");
      for(i=0; i<receiveBufferLen; i++)
        printf("%d", receiveBuffer[i]);
      printf("\n");
    }
    else
    {
      printf("Result from Card != SUCCESS\n");
    }


  }
  else
  {
    checkSCardResult("SCardTransmit", rv);
  }
}

int checkCardResult(BYTE *receiveBuffer, DWORD receiveBufferLen)
{
  int resultValid = 0;

  if ( ( receiveBuffer[receiveBufferLen-2] == 0x90 ) &&
       ( receiveBuffer[receiveBufferLen-1] == 0x00 ) )
  {
    resultValid = 1;
  }

  return resultValid;
}

/*
 * Checks the Result of and SCard-Method
 *
 * param[IN]   method_name    Name of the called Method (for printing purpose)
 * param[IN]   result         Result of the SCard-Method
 *
 * return      ret_val        Returns -1 if an error occurred, otherwise 0
 */
int checkSCardResult(char* method_name, LONG result)
{
  int ret_val;

  if ( result != SCARD_S_SUCCESS )
  {
    // something went wrong
    printf("%s: %s\n", method_name, pcsc_stringify_error(result));
    ret_val = 0;
  }
  else
  {
    // all Good
    printf("%s: Result OK\n", method_name);
    ret_val = 1;
  }

  return ret_val;
}