#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <curl/curl.h>
#include <string.h>


void postAPI(double temp) {
    CURL *curl;
    CURLcode res;

    //Formating the string
    char temperature_str[100];
    sprintf(temperature_str,"{\"temperature\":\"%.2f\"}", temp);
    printf("%s\n", temperature_str);

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    /* get a curl handle */
    curl = curl_easy_init();

    if(curl) {
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");
        /* First set the URL that is about to receive our POST. This URL can just as well be a https:// URL if that is what should receive the data. */
        curl_easy_setopt(curl, CURLOPT_URL,
                "http://0.0.0.0:8000/api/v1/temperature/");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, temperature_str);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}


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
//	    printf("Ambient = %5.2lfC   Object = %5.2lfC    \r",
//                    nrt[0] - 273.15, nrt[1] - 273.15);

	    postAPI((nrt[1]-273.15));

	    //output = fopen("temp.json","w+");
            //
	    //if(output == NULL) {
	        //freopen("temp.json","w+",output);
		//printf("erro");
	    //}
            //
            //fprintf(output,"oioi");
	    //fprintf(output,"{\"t\":%5.2lf}\n", nrt[1]-273.15);
	    //fclose(output);
	    //fflush(stdout);
	    //delay(1, 0);
	    //break;
	}
    }

    //loop & do noise reduction with moving exponential average
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
