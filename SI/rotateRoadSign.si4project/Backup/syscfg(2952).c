#include <includes.h>

static Sysdev_T *main_dev;

static int main_dev_allocate(void)
{
	main_dev = malloc(sizeof(Sysdev_T));
	if(!main_dev)
		return ERR_NOMEM;
	
	main_dev->iSocketClient = -1;
	main_dev->fd_serial = -1;

	main_dev->rev_msg_buf = malloc(BUFFER_SIZE_RECV);
	if(!main_dev->rev_msg_buf) {
		goto err1;
	}
	
	main_dev->send_msg_buf = malloc(BUFFER_SIZE_SEND);
	if(!main_dev->send_msg_buf) {
		goto err2;
	}
	
	return NOERR;
err2:
	free(main_dev->rev_msg_buf);
err1:
	free(main_dev);
	return ERR_NOMEM;
}

int sys_cfg_init(int argc, char **argv)
{
	int errorcode = NOERR;
	
	if(argc != 3) {
		printf("Usage: %s  <default | IPADDR> <default | SERIAL_DEV>\n", argv[0]);
		return ERR_RUN;
	}

	/**
	 * Initialize all global variables, allocate the memory for these variables.
	 */
	errorcode = main_dev_allocate();
	if(errorcode) {
		printf("sys_cfg_init ==> main_dev_allocate error code = %d\n", errorcode);
		return errorcode;
	}

	
	errorcode = try_connect_to_server(main_dev, SYSCFG_SERVER_IP_ADDRESS, SYSCFG_SERVER_PORT);
	if(errorcode < 0) {
		printf("Cannot connect to server, please check server ip!\n");
		return errorcode;
	} else {
		printf("Server can be used!\n");
	}

	main_dev->fd_serial = uart_init(SYSCFG_HARDWARE_SERIAL_DEVICE, 
										SYSCFG_HARDWARE_SERIAL_BRT);
	if(errorcode < 0) {
		printf("Cannot initialize serial, please check the device name!\n");
		return errorcode;
	} else {
		printf("uart device initialized!\n");
	}
	
	return NOERR;
}


