// To compile:
// $ gcc -o keyboard_emulator_reader keyboard_emulator_reader.c -lnfc

#include <stdlib.h>
#include <string.h>
#include <nfc/nfc.h> 

static void
print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;
  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

long int parse_dex(const uint8_t *pbtData, const size_t szBytes)
{
  char smallpart[100];
  char bigpart[100] = "";
  char* end;
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    sprintf(smallpart, "%02x", pbtData[szPos]);
    strcat(bigpart, smallpart);
  }

  long int out = strtol(bigpart, &end, 16);

  if (!*end) {
    //printf("Converted successfully : %ld\n", out);
    return out;
  }
  else{
    //printf("Conversion error, non-convertible part: %s", end);
    return 0;
  }
}
char *before_x(const long int uid)
{
  char str[100];
  char *charout = malloc(100 * sizeof(char));
  bzero(charout, 100 * sizeof(char));
  sprintf(str, "%ld", uid);
  int len = strlen(str);
  int i;
  for (i = 0; i < 2*len; i+=2)
  {
      charout[i] = str[i/2];
      if (i+1 < 2*len-1)
      {
        charout[i+1] = *" ";
      }
  }
  return charout;
}
void  do_x(char *type)
{
  char cmd[100] = "xdotool key --clearmodifiers --delay 0 ";
  strcat(cmd, type);
  printf("%s\n", cmd);
  system(cmd);
  system("xdotool key --clearmodifiers --delay 0 Return");
  free(type);
}

int
main(int argc, const char *argv[])
{
  nfc_device *pnd;
  nfc_target nt;
  const nfc_target ntbis;

  // Allocate only a pointer to nfc_context
  nfc_context *context;

  // Initialize libnfc and set the nfc_context
  nfc_init(&context);
  if (context == NULL) {
    printf("Unable to init libnfc (malloc)\n");
    exit(EXIT_FAILURE);
  }

  // Display libnfc version
  const char *acLibnfcVersion = nfc_version();
  (void)argc;
  printf("%s utilise libnfc v%s\n", argv[0], acLibnfcVersion);

  // Open, using the first available NFC device which can be in order of selection:
  //   - default device specified using environment variable or
  //   - first specified device in libnfc.conf (/etc/nfc) or
  //   - first specified device in device-configuration directory (/etc/nfc/devices.d) or
  //   - first auto-detected (if feature is not disabled in libnfc.conf) device
  pnd = nfc_open(context, NULL);

  if (pnd == NULL) {
    printf("ERROR: %s\n", "Unable to open NFC device.");
    exit(EXIT_FAILURE);
  }
  // Set opened NFC device to initiator mode
  if (nfc_initiator_init(pnd) < 0) {
    nfc_perror(pnd, "nfc_initiator_init");
    exit(EXIT_FAILURE);
  }

  printf("Lecteur NFC: %s \n", nfc_device_get_name(pnd));

  // Poll for a ISO14443A (MIFARE) tag
  const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
  };

  long int uid = 0;
  int i = 0;
  int check = 1;
  while (1) {
    if (nfc_initiator_list_passive_targets(pnd, nmMifare, &nt, 1) > 0) {
      if (uid != parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen)) {
        //printf("%s\n", strtol(*nt.nti.nai.abtUid, 16));
        i++;
        printf("Uid : ");
        print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
        printf("Parse dex : ");
        printf("%ld\n",parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen));
        uid = parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
        printf("Before x : ");
        printf("%s\n", before_x(uid));
        printf("Do x : ");
        do_x(before_x(uid));
      } else {
        //printf("i: %d uid:%ld nt:%ld\n",i, uid, parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen));
        //i++;
      }
    } else {
      check = 0;
      uid = 0;
    }
  }
  // Close NFC device
  nfc_close(pnd);
  // Release the context
  nfc_exit(context);
  exit(EXIT_SUCCESS);
}