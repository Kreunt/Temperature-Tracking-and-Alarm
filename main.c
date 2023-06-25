#include <wiringPi.h> #include <stdio.h> #include <stdlib.h> #include <stdint.h> #include <string.h> #include <curl/curl.h>
#define MAX_ZEIT 85 #define DHT11PIN 7 #define VERSUCHE 5 #define BUZZERPIN 0
int dht11_val[5] = {0, 0, 0, 0, 0};
int dht11_lese_werte()
{
    // uint8_t ist gleich wie byte (unsigned integer 8bit) uint8_t lststate=HIGH; uint8_t zaehler=0; uint8_t j=0,i; //Werte von dht11_val array soll jedes Mal auf Null zurückgesetzt werden, wenn die Funktion lese_werte angeruft
    wird.for (i = 0; i < 5; i++) dht11_val[i] = 0;
    pinMode(DHT11PIN, OUTPUT); // definiert den 7ten Pin als Output bit digitalWrite(DHT11PIN,LOW);//setzt es LOW(0) delay(18);//wartet auf die Antwort vom Pi digitalWrite(DHT11PIN,HIGH);//setzt es HIGH(1) delayMicroseconds(40);//wartet auf die Antwort vom DHT11 Sensor pinMode(DHT11PIN,INPUT);//setzt den 7ten Pin als Input
    for (i = 0; i < MAX_ZEIT; i++)
    {
        zaehler = 0;
        while (digitalRead(DHT11PIN) == lststate)
        {
            zaehler++;
            delayMicroseconds(1);
            if (zaehler == 255)
                break;
        } // diese Schleife bestimmt ob das kommende Bit 0 oder 1 ist. 26-28 microSec ist 0 und 70 microSec ist 1. lststate=digitalRead(DHT11PIN); if(zaehler==255) break;
        // die ersten 3 Übergänge werden ignoriert. if((i>=4)&&(i%2==0)){ dht11_val[j/8]<<=1; if(zaehler>50) dht11_val[j/8]|=1;
        j++;
    }
}
// Checksumme prüfen und die geprüften Daten drucken if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))) {
printf("Relative Luftfeuchtigkeit: %d.%d \nTemperatur:
%d.%d\n",dht11_val[0],dht11_val[1],dht11_val[2],dht11_val[3]); return 1;
}
else printf("Daten nicht gültig, übergehe\n");
return 0;
}
int mailAnfrage()
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    printf("Curl wird gemacht.\n");
    if (curl)
    {
        char mainUrl[] = "https://doktoradanis.net/deneme.php?temperatur=";
        char numberStr[2];
        sprintf(numberStr, "%d", dht11_val[2]);
        strcat(mainUrl, numberStr);
        curl_easy_setopt(curl, CURLOPT_URL, mainUrl);
        res = curl_easy_perform(curl);
        printf("\nCurl wurde gemacht.\n");
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform wurde gescheitert: %s\n", curl_easy_strerror(res));
        }
        else
        {
            printf("Resultat: %s \n", res);
        }
        printf("Curl wurde zurückgegeben.");
        curl_easy_cleanup(curl);
        return 0;
    }
}
void buzz()
{
    digitalWrite(BUZZERPIN, HIGH);
    delay(500);
    digitalWrite(BUZZERPIN, LOW);
}
int main(void)
{
    int versuche = VERSUCHE;
    if (wiringPiSetup() == -1)
        exit(1);
    pinMode(BUZZERPIN, OUTPUT);
    while (versuche)
    {
        int erfolg = dht11_lese_werte();
        if (erfolg)
        {
            if (dht11_val[2] >= 30)
            {
                buzz();
            }
            int httpsReq = mailAnfrage();
            break;
        }
        versuche--;
        delay(5000);
    }
    return 0;
}