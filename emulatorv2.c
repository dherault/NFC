// $ gcc -o emulatorv2 emulatorv2.c -lnfc

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <termio.h>

#define STR_SIZE 256

static const char* EOL_CMD = "\n";
static const char* DEFAULT_CMD = "ls -l";

static _Bool SetEcho(int dev_tty, _Bool enable)
{
	struct termio termBuf;
	ioctl(dev_tty, TCGETA, &termBuf);
	if (enable) {
		termBuf.c_lflag |= ECHO; // Enable echo
	} else {
		termBuf.c_lflag &= ~ECHO; // Don't echo
	}
	ioctl(dev_tty, TCSETAW, &termBuf);
	return 1;
}

static _Bool SendCMDToTTY(const char* cmd)
{
	int dev_tty = open("/dev/tty", O_RDWR);
	if (!dev_tty) {
		return 0;
	}

	SetEcho(dev_tty, 0);
	size_t cmdSize = strlen(cmd);
	for (size_t i = 0; i < cmdSize; i++) {
		ioctl(dev_tty, TIOCSTI, &cmd[i]);
	}
	SetEcho(dev_tty, 1);

	close(dev_tty);
	return 1;
}

int main(int argc, char* argv[])
{
	sleep(5);
	char sys_cmd[STR_SIZE];
	memset(sys_cmd, NULL, STR_SIZE);
	for (int i = 1; i < argc; i++) {
		sprintf(sys_cmd, "%s%s%s", sys_cmd, i == 1 ? "" : " ", argv[i]);
	}
	if (argc == 1) {
		sprintf(sys_cmd, "%s", DEFAULT_CMD);
	}
	sprintf(sys_cmd, "%s%s", sys_cmd, EOL_CMD);
	sys_cmd[STR_SIZE - 1] = NULL;

	if (!SendCMDToTTY(sys_cmd)) {
		perror("Error while sending the command to TTY");
	}
	return 0;
}