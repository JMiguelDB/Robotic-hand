#include "mbed.h"

Serial pc(SERIAL_TX, SERIAL_RX); //Pines que usaremos para la transmisión puerto serie con el PC
Serial XBEE(D8,D2); //Pines a los que está conectado el XBee

AnalogIn FSRmen(A0);
AnalogIn FSRanu(A1);
AnalogIn FSRmed(A2);
AnalogIn FSRind(A3);
AnalogIn FSRpul(A4);

PwmOut men(D3);
PwmOut anu(D5);
PwmOut med(D6);
PwmOut ind(D9);
PwmOut pul(D10);
PwmOut mun(D11);

void leeDatos();
void calibracion();
float map(int x, int in_min, int in_max, float out_min, float out_max);
void menique(int pos);
void anular(int pos);
void medio(int pos);
void indice(int pos);
void pulgar(int pos);
void movimiento(int pos);

int calibraReposo[5];
int calibraTension[5];
int valDedo[5];
int eje[2];
int giro[2];
float presion[5];
int posicion[5];

int main()
{
    men.period(0.020);          // servo requires a 20ms period
    anu.period(0.020);
    med.period(0.020);
    ind.period(0.020);
    pul.period(0.020);

    calibracion();

    while(1) {

        if(pc.readable()){
            if(pc.getc() == 'c') {
                calibracion();
            }
        }
        
        leeDatos();
        menique(valDedo[0]);
        anular(valDedo[1]);
        medio(valDedo[2]);
        indice(valDedo[3]);
        pulgar(valDedo[4]);
        movimiento(eje[0]);

    }
}

void leeDatos()
{
    int i = 0;
    char lectura[30];

    if(XBEE.getc() == '[') {
        valDedo[0] = 0;
        valDedo[1] = 0;
        valDedo[2] = 0;
        valDedo[3] = 0;
        valDedo[4] = 0;
        eje[0] = 0;
        eje[1] = 0;
        i = 0;

        XBEE.gets(lectura,30);

        while(lectura[i] != ':') {
            valDedo[0] = (lectura[i] - 48) + valDedo[0]*10;
            i++;
        }
        i++;
        while(lectura[i] != ':') {
            valDedo[1] = (lectura[i] - 48) + valDedo[1]*10;
            i++;
        }
        i++;
        while(lectura[i] != ':') {
            valDedo[2] = (lectura[i] - 48) + valDedo[2]*10;
            i++;
        }
        i++;
        while(lectura[i] != ':') {
            valDedo[3] = (lectura[i] - 48) + valDedo[3]*10;

            i++;
        }
        i++;
        while(lectura[i] != ':') {
            valDedo[4] = (lectura[i] - 48) + valDedo[4]*10;
            i++;
        }
        i++;
        while(lectura[i] != ':') {
            eje[0] = (lectura[i] - 48) + eje[0]*10;
            i++;
        }
        i++;
        while(lectura[i] != ']') {
            eje[1] = (lectura[i] - 48) + eje[1]*10;
            i++;
        }
    }
    //pc.printf("M: %d, A: %d, M: %d, I: %d, P: %d \n", valDedo[0],valDedo[1],valDedo[2],valDedo[3],valDedo[4]);
    //pc.printf("X: %d, Y: %d \n", eje[0],eje[1]);
}

void calibracion()
{
    int letra = 0;
    for(int i = 0; i < 5; i++) {
        calibraReposo[i] = 0;
        calibraTension[i] = 0;
    }
    pc.printf("Calibracion\n");
    pc.printf("1 - Extienda la mano, mantenga la mano horizontal y pulse 'e' para realizar la lectura\n");
    while(letra != 'e') {
        letra = pc.getc();
    }
    int t = 0;
    while(t < 10) {
        leeDatos();
        if((valDedo[0] > 60 || valDedo[0] < 0) || (valDedo[3] > 60 || valDedo[3] < 0)) {
            calibraReposo[0] = calibraReposo[0];
            calibraReposo[1] = calibraReposo[1];
            calibraReposo[2] = calibraReposo[2];
            calibraReposo[3] = calibraReposo[3];
            calibraReposo[4] = calibraReposo[4];
            giro[0] = giro[0];
        } else {
            calibraReposo[0] = calibraReposo[0] + valDedo[0];
            calibraReposo[1] = calibraReposo[1] + valDedo[1];
            calibraReposo[2] = calibraReposo[2] + valDedo[2];
            calibraReposo[3] = calibraReposo[3] + valDedo[3];
            calibraReposo[4] = calibraReposo[4] + valDedo[4];
            giro[0] = giro[0] + eje[0];
            t++;
        }
    }
    for(int i = 0; i < 100; i++) {
        presion[0] = presion[0] + (FSRmen.read() * 1000);
        presion[1] = presion[1] + (FSRanu.read() * 1000);
        presion[2] = presion[2] + (FSRmed.read() * 1000);
        presion[3] = presion[3] + (FSRind.read() * 1000);
        presion[4] = presion[4] + (FSRpul.read() * 1000);

    }
    presion[0] = presion[0] / 100;
    presion[1] = presion[1] / 100;
    presion[2] = presion[2] / 100;
    presion[3] = presion[3] / 100;
    presion[4] = presion[4] / 100;
    calibraReposo[0] = calibraReposo[0]/10;
    calibraReposo[1] = calibraReposo[1]/10;
    calibraReposo[2] = calibraReposo[2]/10;
    calibraReposo[3] = calibraReposo[3]/10;
    calibraReposo[4] = calibraReposo[4]/10;
    giro[0] = giro[0]/10;
    pc.printf("Calibracion de la mano extendida completada\n");
    letra = 0;
    pc.printf("2 - Apriete la mano, gire la mano y pulse 't' para realizar la lectura\n");
    while(letra != 't') {
        letra = pc.getc();
    }
    t = 0;
    while(t < 10) {
        leeDatos();
        if((valDedo[0] > 60 || valDedo[0] < 0) || valDedo[3] < 0) {
            calibraTension[0] = calibraTension[0];
            calibraTension[1] = calibraTension[1];
            calibraTension[2] = calibraTension[2];
            calibraTension[3] = calibraTension[3];
            calibraTension[4] = calibraTension[4];
            giro[1] = giro[1];
        } else {
            calibraTension[0] = calibraTension[0] + valDedo[0];
            calibraTension[1] = calibraTension[1] + valDedo[1];
            calibraTension[2] = calibraTension[2] + valDedo[2];
            calibraTension[3] = calibraTension[3] + valDedo[3];
            calibraTension[4] = calibraTension[4] + valDedo[4];
            giro[1] = giro[1] + eje[0];
            t++;
        }
    }
    calibraTension[0] = calibraTension[0]/10;
    calibraTension[1] = calibraTension[1]/10;
    calibraTension[2] = calibraTension[2]/10;
    calibraTension[3] = calibraTension[3]/10;
    calibraTension[4] = calibraTension[4]/10;
    giro[1] = giro[1]/10;

    pc.printf("Calibracion de la mano apretada completada\n");

    pc.printf("Calibracion finalizada\n");

    pc.printf("Menique extendido %d  y apretado %d \n", calibraReposo[0], calibraTension[0]);
    pc.printf("Anular extendido %d  y apretado %d \n", calibraReposo[1], calibraTension[1]);
    pc.printf("Medio extendido %d  y apretado %d \n", calibraReposo[2], calibraTension[2]);
    pc.printf("Indice extendido %d  y apretado %d \n", calibraReposo[3], calibraTension[3]);
    pc.printf("Pulgar extendido %d  y apretado %d \n", calibraReposo[4], calibraTension[4]);
    pc.printf("Mano extendida %d  y girada %d \n", giro[0], giro[1]);

    pc.printf("Pulse ENTER para acceder a la simulacion \n");
    letra = 0;
    while(letra != '\r') {
        letra = pc.getc();
    }
}

void menique(int pos)
{
    float val;
    pos = pos - 4300;
    if(pos > calibraReposo[0]) {
        pos = calibraReposo[0];
    } else if(pos < calibraTension[0] && pos > 20) {
        pos = calibraTension[0];
    }
    if((FSRmen.read() * 1000) < 100 || pos > posicion[0]){    
        posicion[0] = pos;    
    } else{
        pos = posicion[0];
    }
    val = map(pos, calibraTension[0], calibraReposo[0], 0.0005, 0.0025);
    men.pulsewidth(val);
    
    //pc.printf("FSR menique = %f \n", FSRmen.read() * 1000);
        
}

void anular(int pos)
{
    float val1;
    if(pos > calibraReposo[1]) {
        pos = calibraReposo[1];
    } else if(pos < calibraTension[1] && pos > 25) {
        pos = calibraTension[1];
    }
    if((FSRanu.read() * 1000) < 100 || pos > posicion[1]){    
        posicion[1] = pos;    
    } else{
        pos = posicion[1];
    }
    val1 = map(pos, calibraTension[1], calibraReposo[1], 0.0025, 0.0005); 
    anu.pulsewidth(val1);
    //pc.printf("FSR anular = %f \n", FSRanu.read() * 1000);
}

void medio(int pos)
{
    if(pos > calibraReposo[2]) {
        pos = calibraReposo[2];
    } else if(pos < calibraTension[2] && pos > 20) {
        pos = calibraTension[2];
    }
    if((FSRmed.read() * 1000) < 800 || pos > posicion[2]){    
        posicion[2] = pos;    
    } else{
        pos = posicion[2];
    }
    float val2 = map(pos, calibraTension[2], calibraReposo[2], 0.0025, 0.0005);
    med.pulsewidth(val2);
    //pc.printf("FSR medio = %f \n", FSRmed.read() * 1000);
}

void indice(int pos)
{
    float val3;
    if(pos < calibraReposo[3] && pos > 31) {
        pos = calibraReposo[3];
    } else if(pos > calibraTension[3]) {
        pos = calibraTension[3];
    }
    if((FSRind.read() * 1000) < 200 || pos < posicion[3]){    
        posicion[3] = pos;    
    } else{
        pos = posicion[3];
    }
    val3 = map(pos, calibraTension[3], calibraReposo[3], 0.0025, 0.0005);
    ind.pulsewidth(val3);
    //pc.printf("FSR indice = %f \n", FSRind.read() * 1000);
}

void pulgar(int pos)
{
    float val4;
    if(pos > calibraReposo[4]) {
        pos = calibraReposo[4];
    } else if(pos < calibraTension[4] && pos > 20) {
        pos = calibraTension[4];
    }
    if((FSRpul.read() * 1000) < 100 || pos > posicion[4]){    
        posicion[4] = pos;    
    } else{
        pos = posicion[4];
    }
    val4 = map(pos, calibraTension[4], calibraReposo[4], 0.0025, 0.0005);
    pul.pulsewidth(val4);
    //pc.printf("FSR pulgar = %f \n", FSRpul.read() * 1000);
}

void movimiento(int pos)
{
    float val5 = map(pos, giro[0], giro[1], 0.0005, 0.0025);
    mun.pulsewidth(val5);
}
float map(int x, int in_min, int in_max, float out_min, float out_max)
{
    return ((float)x - (float)in_min) * (out_max - out_min) / ((float)in_max - (float)in_min) + out_min;
}
