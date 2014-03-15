// To compile:
// $ gcc -o keyboard_emulator_reader keyboard_emulator_reader.c -lnfc

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
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
  static char charout[100];
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
int setup_uinput_device() {
int uinp_fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
if (uinp_fd == 0) {
printf("Unable to open /dev/uinput\n");
return -1;
}

struct uinput_user_dev uinp;
memset(&uinp, 0, sizeof(uinp)); // Intialize the uInput device to NULL
strncpy(uinp.name, "Virtual Keyboard Device", strlen(
"Virtual Keyboard Device"));
uinp.id.version = 4;
uinp.id.bustype = BUS_USB;
uinp.id.product = 1;
uinp.id.vendor = 1;

// Setup the uinput device
ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
int i = 0;
for (i = 0; i < 256; i++) {
ioctl(uinp_fd, UI_SET_KEYBIT, i);
}

/* Create input device into input sub-system */
write(uinp_fd, &uinp, sizeof(uinp));
if (ioctl(uinp_fd, UI_DEV_CREATE)) {
printf("Unable to create UINPUT device.");
return -1;
}

return uinp_fd;
}

void send_key_event(int fd, unsigned int keycode, int keyvalue) {
struct input_event event;
gettimeofday(&event.time, NULL);

event.type = EV_KEY;
event.code = keycode;
event.value = keyvalue;

if (write(fd, &event, sizeof(event)) < 0) {
//printf("simulate key error\n");
} else {
//printf("simuate key %d, %d\n", keycode, keyvalue);
}

event.type = EV_SYN;
event.code = SYN_REPORT;
event.value = 0;
write(fd, &event, sizeof(event));
if (write(fd, &event, sizeof(event)) < 0) {
//printf("simulate key error\n");
} else {
//printf("simuate key %d, %d\n", keycode, keyvalue);
}
}
do_x(int uinp_fd, long int uid)
{
  unsigned char str[100];
  sprintf(str, "%ld", uid);
  int len = strlen(str);
  int i;
  int key_code;

  for (i = 0; i < len; i++)
  {
    key_code = (int) str[i] - 48;
      printf("%d\n", key_code);
    switch(key_code) {
      case 0:
        send_key_event(uinp_fd, KEY_KP0, 1);
        send_key_event(uinp_fd, KEY_KP0, 0);
        break;
      case 1:
        send_key_event(uinp_fd, KEY_KP1, 1);
        send_key_event(uinp_fd, KEY_KP1, 0);
        break;
      case 2:
        send_key_event(uinp_fd, KEY_KP2, 1);
        send_key_event(uinp_fd, KEY_KP2, 0);
        break;
      case 3:
        send_key_event(uinp_fd, KEY_KP3, 1);
        send_key_event(uinp_fd, KEY_KP3, 0);
        break;
      case 4:
        send_key_event(uinp_fd, KEY_KP4, 1);
        send_key_event(uinp_fd, KEY_KP4, 0);
        break;
      case 5:
        send_key_event(uinp_fd, KEY_KP5, 1);
        send_key_event(uinp_fd, KEY_KP5, 0);
        break;
      case 6:
        send_key_event(uinp_fd, KEY_KP6, 1);
        send_key_event(uinp_fd, KEY_KP6, 0);
        break;
      case 7:
        send_key_event(uinp_fd, KEY_KP7, 1);
        send_key_event(uinp_fd, KEY_KP7, 0);
        break;
      case 8:
        send_key_event(uinp_fd, KEY_KP8, 1);
        send_key_event(uinp_fd, KEY_KP8, 0);
        break;
      case 9:
        send_key_event(uinp_fd, KEY_KP9, 1);
        send_key_event(uinp_fd, KEY_KP9, 0);
        break;
      default:
        printf("%s %d\n", "noooo", key_code);
        break;

    }
  }
      send_key_event(uinp_fd, KEY_ENTER, 1);
      send_key_event(uinp_fd, KEY_ENTER, 0);
}

int main(int argc, const char *argv[])
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

  int uinp_fd;
  if ((uinp_fd = setup_uinput_device()) < 0) {
  printf("Unable to find uinput device\n");
  return -1;
  }
  sleep(1);
  



  long int uid = 0;
  int i = 0;
  int check = 1;
  while (1) {
    if (nfc_initiator_list_passive_targets(pnd, nmMifare, &nt, 1) > 0) {
      if (uid != parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen)) {
        //printf("%s\n", strtol(*nt.nti.nai.abtUid, 16));
        i++;
        printf("\nmotherfucker %d\n", i);
        printf("Uid : ");
        print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
        printf("Parse dex : ");
        printf("%ld\n",parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen));
        uid = parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
        //printf("Before x : ");
        //printf("%s\n", before_x(uid));
        printf("Do x : ");
        do_x(uinp_fd, uid);
      } else {
        //printf("i: %d uid:%ld nt:%ld\n",i, uid, parse_dex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen));
        //i++;
      }
    } else {
      check = 0;
      uid = 0;
    }
  }
    /* Destroy the input device */
  ioctl(uinp_fd, UI_DEV_DESTROY);
  /* Close the UINPUT device */
  close(uinp_fd);
  // Close NFC device
  nfc_close(pnd);
  // Release the context
  nfc_exit(context);
  exit(EXIT_SUCCESS);
}