#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>

#define FALSE -1
#define TRUE 0



/*打开串口并返回串口设备描述符*/
int UART0_Open(int fd, char* port)
{
	fd = open(port, O_RDWR|O_NOCTTY|O_NDELAY);
	if(FALSE == fd)
	{
		perror("Can't Open Serial Port");
		return(FALSE);
	}

	if(fcntl(fd, F_SETFL, 0) < 0)
	{
		printf("fcntl failed!\n");
		return(FALSE);
	}
	else 
	{
		printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
	}
	
	if(0 == isatty(STDIN_FILENO))
	{
		printf("standard input is not a terminal device\n");
		return(FALSE);
	}
	else
	{
		printf("isatty success!\n");
	}

	printf("fd->open=%d\n", fd);
	return fd;

}

/*关闭串口*/
void UART0_Close(int fd)
{
	close(fd);
}

/*设置串口数据位，停止位和校验位*/
int UART0_Set(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
	int i;
	int status;
	int speed_arr[] = {B115200, B19200, B9600, B4800, B2400, B1200, B300};
	int name_arr[] = {115200, 19200, 9600, 4800, 2400, 1200, 300};

	struct termios options;
	
	if(tcgetattr(fd, &options) != 0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}
	
	/*设置串口输入波特率和输出波特率*/
	for(i=0; i<sizeof(speed_arr)/sizeof(int); i++)
	{
		if(speed == name_arr[i])
		{
			cfsetispeed(&options, speed_arr[i]);
			cfsetospeed(&options, speed_arr[i]);
		}
	}

	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;

	switch(flow_ctrl)
	{
		/*不使用数据流控*/
		case 0:
			options.c_cflag &= ~CRTSCTS;
			break;
		/*使用硬件流控*/
		case 1:
			options.c_cflag |= CRTSCTS;
			break;
		/*使用软件流控*/
		case 2:
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
	}

	/*设置数据位*/
	options.c_cflag &= ~CSIZE;
	switch(databits)
	{
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
			fprintf(stderr, "Unsupported data size \n");
			return (FALSE);
	}

	/*设置校验位*/
	switch(parity)
	{
		case 'n':
		case 'N':/*无奇偶校验*/
			options.c_cflag &= ~PARENB;
			options.c_iflag &= ~INPCK;
			break;
		case 'o':
		case 'O':/*设置为奇校验*/
			options.c_cflag |= (PARODD | PARENB);
			options.c_iflag |= INPCK;
			break;
		case 'e':
		case 'E':/*设置为偶校验*/
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_iflag |= INPCK;
		case 's':
		case 'S':/*设置为空格*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
		default:
			fprintf(stderr, "Unsupported parity\n");
			return(FALSE);
	}

	/*设置停止位*/
	switch(stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;break;
		case 2:
			options.c_cflag |= CSTOPB; break;
		default:
			fprintf(stderr, "Unsupported stop bits\n");
			return(FALSE);
	}

	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	/*设置等待时间和最小接收字符*/
	options.c_cc[VTIME] = 0;/*读取一个字符等待1*（1/10）s*/
	options.c_cc[VMIN] = 1;/*读取字符的最少个数为1*/

	/*如果发生溢出，接收数据，但是不再读取 刷新收到的数据但是不读*/
	tcflush(fd, TCIFLUSH);

	if(tcsetattr(fd, TCSANOW, &options) != 0)
	{
		perror("com set error!\n");
		return(FALSE);
	}
	return(TRUE);
}

/*串口初始化*/
int UART0_Init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
	int err;
	
	if(UART0_Set(fd, speed, flow_ctrl, databits, stopbits, 'N') == FALSE)
	{
		return(FALSE);
	}
	else
	{
		return(TRUE);
	}
}

/*接收串口数据*/
int UART0_Recv(int fd, char *rcv_buf, int data_len)
{
	int len,fs_sel;
	fd_set fs_read;

	struct timeval time;
	
	FD_ZERO(&fs_read);
	FD_SET(fd, &fs_read);

	time.tv_sec = 10;
	time.tv_usec = 0;

//	fs_sel = select(fd+1, &fs_read, NULL, NULL, &time);
//	printf("fs_sel = %d\n", fs_sel);
	if(1)
	{
		len = read(fd, rcv_buf, data_len);
		printf("I am right!(version1.2) len = %d fs_sel = %d\n", len, fs_sel);
		return len;
	}
	else 
	{
		printf("Sorry, I am wrong!");
		return FALSE;
	}
}

/*发送数据*/
int UART0_Send(int fd, char *send_buf, int data_len)
{
	int len = 0;

	len = write(fd, send_buf, data_len);
	if(len == data_len)
	{
		printf("send data is %s\n", send_buf);
		return len;
	}
	else
	{
		tcflush(fd, TCOFLUSH);
		return FALSE;
	}
}
int main(int argc, char **argv)
{
	int fd;
	int err;
	int len;
	int i;
	char command9[5] = {0xee, 0xee, 0x0a, 0xfc, 0xfc};
 	char command12[22] = {0xee, 0xee, 0x0c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xfc, 0xfc};
	char rcv_buf[100];
	char send_buf[20] = "Hello World!";

	if(argc != 3)
	{
		printf("Usage: %s /dev/ttySn 0(send data)/1(receive data \n)", argv[0]);
		return FALSE;
	}
	/*打开串口*/
	fd = UART0_Open(fd, argv[1]);
	do
	{
		err = UART0_Init(fd, 115200, 0, 8, 1, 'N');
		printf("Set Port Exactly!\n");
	}while(FALSE == err || FALSE == fd);

	if(0 == strcmp(argv[2], "0"))
	{
	/*	for(i=0; i<10; i++)
		{
			len = UART0_Send(fd, send_buf, 10);
			if(len > 0)
				printf("%d time send %d data successfl\n", i, len);
			else
				printf("send data failed!\n");

			sleep(2);
		}*/
		while(1)
		{
			UART0_Send(fd, command12, 22);
			//usleep(100000);
		}
		UART0_Close(fd);
	}
	else
	{
		while(1)
		{		
			len = UART0_Recv(fd, rcv_buf, 99);
			if(len > 0)
			{
				rcv_buf[len] = '\0';
				printf("receive data is %s \n", rcv_buf);
				printf("len = %d\n", len);
			}
			else
			{
				printf("cannot receive data\n");
			}
//			sleep(3);
		}
		UART0_Close(fd);
	}
}
