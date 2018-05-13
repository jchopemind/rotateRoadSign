#include <includes.h>

static void sig_heartbeat(int signo)
{
	printf("print heart beat...\n");

	alarm(10);
}


int main(int argc, char **argv)
{
	if(signal(SIGALRM, sig_heartbeat) == SIG_ERR) {
		printf("signal SIGALRM error!\n");
		exit(-1);
	}

	alarm(10); /* setup MSG_STATIONLIVE */


	sys_cfg_init(argc, argv);

	return 0;
}


