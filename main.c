#include <includes.h>

static void sig_heartbeat(int signo)
{
	printf("print heart beat...\n");

	alarm(10);
}


int main(int argc, char **argv)
{
#if 0
	if(signal(SIGALRM, sig_heartbeat) == SIG_ERR) {
		printf("signal SIGALRM error!\n");
		exit(-1);
	}

	alarm(10); /* setup MSG_STATIONLIVE */
#endif
	int fd;

	fd = uart_init(argv[1], SYSCFG_HARDWARE_SERIAL_BRT);
	if(fd <= 0) {
		printf("uart_init error! fd = %d\n", fd);
		return fd;
	}

	while(1) {
		show_string(fd, 2, RGB(255,0,255), 14, 15, 24, 2);

		sleep(8);
	}

	

	return 0;
}


