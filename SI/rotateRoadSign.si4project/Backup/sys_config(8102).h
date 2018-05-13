#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/**
 * The configuration of server:
 * IP address  : the address of server(Tencent cloud)
 * Socket Port : the port of communication
 */
#define SYSCFG_SERVER_IP_ADDRESS    118.24.76.156
#define SYSCFG_SERVER_PORT 			888



/**
 * Error about
 */
#define NOERR			0
#define ERR_CFG     	-1
#define ERR_NOMEM   	-2
#define ERR_UNHANDLE  	-3
#define ERR_RUN			-4
#endif