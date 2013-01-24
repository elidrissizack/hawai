#include <stdio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

void error(const char* msg) 
{
	printf("%s\n", msg);
	fflush(0);
	_exit(0);
}

void make_raw (struct termios* options)
{
    options->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
            | INLCR | IGNCR | ICRNL | IXON);
    options->c_oflag &= ~OPOST;
    options->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options->c_cflag &= ~(CSIZE | PARENB);
    options->c_cflag |= CS8;
}

int main(int argc, char* argv[])
{
	if (argc < 2) 
	{
		char buffer[250];
		sprintf(buffer, "Usage: %s todo *grins", argv[0]);

		error(buffer);
	}

    printf("%s", "Opening file...!\n");

    int handle = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (handle == -1)
        error("Error opening serial port");

    if (fcntl(handle, F_SETFL, 0) == -1)
        error("Error setting non-block");

    struct termios options;
    
    if(tcgetattr(handle, &options) == -1)
        error("Error getting options");

    cfsetispeed(&options, B0);
    cfsetospeed(&options, B9600);
    make_raw(&options);
    tcsetattr(handle, TCSANOW, &options);

    // AT+CGMI aka get the device producent
    const char* test = "AT+CGMI\r";

    write(handle, test, strlen(test)+1);

    int readed = 0;
    char buffer[256];

    do
    {
        readed = read(handle, &buffer, 255);
        if (readed > 0)
            printf("Result is: %s\n", buffer);
    }
    while (0);

    close(handle);
}
