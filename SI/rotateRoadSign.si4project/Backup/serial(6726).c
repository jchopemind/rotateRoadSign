#include <includes.h>


/**
 * Open the serial device according to devname
 * return file descriptor if success
 */
static int uart_open(char *devname)
{
	int retcode = NOERR;
	int fd = -1;

	fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd <= 0) {
		printf("Cannot open serial device : %s \n", devname);
		return fd;
	}

	/* Restoring the blocked status of serial port */
	retcode = fcntl(fd, F_SETFL, 0);
	if(retcode < 0) {
		printf("uart_open ==> error code = %d\n", retcode);
		return retcode;
	}else{
		printf("uart_open ==> retcode = %d\n", retcode);
	}

	/* check port whether it is a terminal device */
	if(0 == isatty(STDIN_FILENO)) {
		printf("uart_open ==> standard input is not a terminal device!\n");
		return ERR_DEV;
	} else {
		printf("uart_open ==> standard input is a terminal dev!\n");
	}

	return fd;
}

void uart_close(int *fd)
{
	close(*fd);
	*fd = -1;
}

static int uart_set(int fd, int speed, int flow_ctl, int databits, int stopbits, int parity)
{
	int i, errorcode = 0;
	int status;
	int speed_arr[] = {B115200, B19200, B9600, B4800, B2400, B1200, B300};
	int name_arr[] = {115200, 19200, 9600, 4800, 2400, 1200, 300};

	struct termios options;

	errorcode = tcgetattr(fd, &options);
	if(errorcode != 0) {
		printf("uart_set ==> setup serial error! code = %d\n", errorcode);
		return errorcode;
	}

	for(i = 0; i < sizeof(speed_arr) / sizeof(int); i++) {
		if(speed == name_arr[i]) {
			cfsetispeed(&options, speed_arr[i]);
			cfsetospeed(&options, speed_arr[i]);
		}
	}

	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;

	switch(flow_ctl) {
	case 0:
		options.c_cflag &= ~CRTSCTS;
		break;
	case 1:
		options.c_cflag |= CRTSCTS;
		break;
	case 2:
		options.c_cflag |= IXON | IXOFF | IXANY;
		break;
	}

	options.c_cflag &= ~CSIZE;

	switch (databits) {
	case 5:
		options.c_cflag |= CS5;
		break;
	case 6:
		options.c_cflag |= CS6;
		break;
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr, "Unsupported this data size\n");
		return ERR_DATA;
	}
		
	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~INPCK;
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);
		options.c_cflag |= INPCK;
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK;
		break;
	case 's':
    case 'S':
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
    default:
		fprintf(stderr, "Unsupported parity\n");
		return (ERR_DATA);     
	}

	switch (stopbits) {
	case 1:       
		options.c_cflag &= ~CSTOPB; 
		break;     
	case 2:       
		options.c_cflag |= CSTOPB; 
		break;    
	default:       
		fprintf(stderr, "Unsupported stop bits\n");     
		return (ERR_DATA);  
	}

	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    options.c_cc[VTIME] = 1; 
    options.c_cc[VMIN] = 1;

	tcflush(fd, TCIFLUSH);
	
	if (tcsetattr(fd, TCSANOW, &options) != 0) {
		printf("uart_set ==> com set error!\n");
		return ERR_DEV;
	}

	return NOERR;
}



/**
 * Function: Receive data from serial port
 * @fd		 : The file descriptor of serial device
 * @rcv_buf : The buffer of data it will be received
 * @data_len: length of the data
 * # ret	 : the size of data received
 */
int uart_recv(int fd, char *rcv_buf, int data_len)    
{    
	int len, fs_sel;    
	fd_set fs_read;    
	   
	struct timeval time;    
	   
	FD_ZERO(&fs_read);    
	FD_SET(fd, &fs_read);    
	   
	time.tv_sec = 10;    
	time.tv_usec = 0;    
	   
	fs_sel = select(fd + 1, &fs_read, NULL, NULL, &time);    
	printf("fs_sel = %d\n", fs_sel);    
	if(fs_sel) {    
	    len = read(fd,rcv_buf,data_len);    
	    printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);    
	    return len;    
	} else {    
	    printf("Sorry,I am wrong!");    
	    return FALSE;    
	}         
}    

/**
 * send data to serial device port
 */
int uart_send(int fd, char *send_buf,int data_len)    
{    
	int len = 0;
	   
	len = write(fd, send_buf, data_len);
	if (len > 0) {    
	    printf("send data is %s\n", send_buf);
	    return len;
	} else {
	    tcflush(fd, TCOFLUSH);
	    return ERR_DEV;
	}
}


int uart_init(char *sname, int speed)
{
	int err = -1, fd = -1;  

	fd = uart_open(sname);
	if(fd <= 0) {
		printf("uart_init ==> uart init error!\n");
		return fd;
	}

	err = uart_set(fd, speed, 0, 8, 1, 'N');
    if (err < 0)
        return FALSE;

	return NOERR;
}



