#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>

volatile int end;
FILE *output;

void delay(int s, int n) {
	struct timespec period = { s, n };
	while (nanosleep(&period, &period)) { }
}

int MLX90614_read(int smbusfd, int num, double *temps) {
	union i2c_smbus_data msg;
	struct i2c_smbus_ioctl_data sdat = {
		.read_write = I2C_SMBUS_READ,
		.command = 6,  // read register 6, ambient
		.size = I2C_SMBUS_WORD_DATA,
		.data = &msg
	};
	int res;
	assert((num > 0) && (num < 4));
	while (num--) {
		if ((res = ioctl(smbusfd, I2C_SMBUS, &sdat)) < 0) {
			return res;  // maybe not the best value
		}
		*temps = 0.02 * (double)msg.word;
		temps++;
		sdat.command++;
	}
	return 0;
}

void *sampler(void *fd) {
	int smbusfd, res;
	double temps[2], nrt[2];
	smbusfd = *((int*)fd);

	// get first sample
	while (!end) {
		if (MLX90614_read(smbusfd, 2, nrt)) {
			printf("\nRead failure.\n");
			delay(5, 0);
		}
		else {
			printf("Ambient = %5.2lfC   Object = %5.2lfC    \r",
				nrt[0] - 273.15, nrt[1] - 273.15);
			output = fopen("temp.json","w+");
			if(output == NULL) {
				freopen("temp.json","w+",output);
				//printf("erro");
			}
			//fprintf(output,"oioi");
			fprintf(output,"{\"t\":%5.2lf}\n", nrt[1]-273.15);
			fclose(output);
			fflush(stdout);
			delay(1, 0);
			break;
		}
	}
	// loop & do noise reduction with moving exponential average
	while (!end) {
		if (MLX90614_read(smbusfd, 2, temps)) {
			printf("\nRead failure.\n");
			delay(5, 0);
		}
		else {
			nrt[0] = nrt[0] * 0.8 + temps[0] * 0.2;
			nrt[1] = nrt[1] * 0.8 + temps[1] * 0.2;
			printf("Ambient = %5.2lfC   Object = %5.2lfC    \r",
				nrt[0] - 273.15, nrt[1] - 273.15);
			fflush(stdout);
			delay(1, 0);
		}
	}
}

int main() {
	pthread_t tid;
	int i2cfd;
	
	i2cfd = open("/dev/i2c-1", O_RDWR);
	if (i2cfd < 0) {
		perror("open");
		return __LINE__;
	}
	if (ioctl(i2cfd, I2C_SLAVE, 0x5a) < 0) {
		perror("address");
		close(i2cfd);
		return __LINE__;
	}
	if (ioctl(i2cfd, I2C_PEC, 1) < 0) {
		perror("PEC");
		close(i2cfd);
		return __LINE__;
	}
	end = 0;
	pthread_create(&tid, NULL, sampler, &i2cfd);
	
	// wait for input; needs CR/enter
	getchar();
	end = 1;
	pthread_join(tid, NULL);
	close(i2cfd);
}