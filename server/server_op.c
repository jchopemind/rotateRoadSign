#include <includes.h>

int try_connect_to_server(Sysdev_T *dev, char *ip_addr, int port)
{
	int errcode = NOERR;
	struct sockaddr_in tSocketServerAddr;
	
	dev->iSocketClient = socket(AF_INET, SOCK_STREAM, 0);

	tSocketServerAddr.sin_family      = AF_INET;
	tSocketServerAddr.sin_port        = htons(port);  /* host to net, short */
 	//tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
 	if(0 == inet_aton(ip_addr, &tSocketServerAddr.sin_addr)) {
		printf("try_connect_to_server ==> invalid server_ip code = %d\n", ERR_IPADDR);
		return ERR_IPADDR;
	}
	
	memset(tSocketServerAddr.sin_zero, 0, 8);

	errcode = connect(dev->iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));	
  	if (-1 == errcode) {
		printf("try_connect_to_server ==> connect error! code = %d \n", ERR_SVRCONNECT);
		return ERR_SVRCONNECT;
	}

	return NOERR;
}


