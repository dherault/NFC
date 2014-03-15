
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

/* Setup the uinput device */
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
printf("simulate key error\n");
} else {
printf("simuate key %d, %d\n", keycode, keyvalue);
}

event.type = EV_SYN;
event.code = SYN_REPORT;
event.value = 0;
write(fd, &event, sizeof(event));
if (write(fd, &event, sizeof(event)) < 0) {
printf("simulate key error\n");
} else {
printf("simuate key %d, %d\n", keycode, keyvalue);
}
}

int main(int argc, char *argv[]) {
unsigned int key_code = 30;
if (argc >= 2) {
key_code = atoi(argv[1]);
} 
int uinp_fd;
if ((uinp_fd = setup_uinput_device()) < 0) {
printf("Unable to find uinput device\n");
return -1;
}
sleep(1);
send_key_event(uinp_fd, key_code, 1);
send_key_event(uinp_fd, key_code, 0);

/* Destroy the input device */
ioctl(uinp_fd, UI_DEV_DESTROY);
/* Close the UINPUT device */
close(uinp_fd);
} 