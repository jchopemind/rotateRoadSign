#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/**
 * The configuration of server:
 * IP address  : the address of server(Tencent cloud)
 * Socket Port : the port of communication
 */
#define SYSCFG_SERVER_IP_ADDRESS    			"118.24.76.156"
#define SYSCFG_SERVER_PORT 						1707
#define SYSCFG_HARDWARE_SERIAL_DEVICE 			"/dev/ttySAC1"
#define SYSCFG_HARDWARE_SERIAL_BRT				57600

/**
 * Error about
 */
#define NOERR				0
#define ERR_CFG     		-1
#define ERR_NOMEM   		-2
#define ERR_UNHANDLE  		-3
#define ERR_RUN				-4
#define ERR_IPADDR			-5
#define ERR_SVRCONNECT 		-6
#define ERR_DEV				-7
#define ERR_DATA			-8



/* create structure for sys */

#define BUFFER_SIZE_RECV 1024
#define BUFFER_SIZE_SEND 1024

typedef struct sys_device {
	int iSocketClient;
	char *rev_msg_buf;
	char *send_msg_buf;

	int fd_serial;
	
}Sysdev_T;


/* function  declaration */

/**
 * initialize the system : serial, led, 4G module and etc... 
 * finally, try to connect to server
 */
int sys_cfg_init(int argc, char **argv);
int try_connect_to_server(Sysdev_T *dev, char *ip_addr, int port);
void uart_close(int *fd);
int uart_recv(int fd, char *rcv_buf, int data_len);
int uart_send(int fd, char *send_buf,int data_len);
int uart_init(char *sname, int speed);



#endif
