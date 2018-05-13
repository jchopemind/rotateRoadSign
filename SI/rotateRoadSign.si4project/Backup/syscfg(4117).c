#include <includes.h>

int sys_cfg_init(int argc, char **argv)
{
	if(argc != 3) {
		printf("Usage: %s  <default | IPADDR> <default | SERIAL_DEV>\n");
		return ERR_RUN;
	}
	
	try_connect_to_server();
	
	return NOERR;
}


