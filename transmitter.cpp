#include "mbed.h"

Serial pc(SERIAL_TX, SERIAL_RX);
Serial XBEE(D8,D2);

AnalogIn menique(A0);
AnalogIn anular(A1);
AnalogIn medio(A2);
AnalogIn indice(A3);
AnalogIn pulgar(A4);

AnalogIn inputx(D11);
AnalogIn inputy(D12);
AnalogIn inputz(D13);

int main()
{
    float dedo[5];
    int valDedo[5];
    float cal_x=0,cal_y=0,cal_z=0;
    int x=0,y=0,z=0;
    int i=100;
    char valor[23];


    while(i--) {
        dedo[0] = menique*100;
        dedo[1] = anular*100;
        dedo[2] = medio*100;
        dedo[3] = indice*100;
        dedo[4] = pulgar*100;

        cal_x=inputx*100;
        cal_y=inputy*100;
        cal_z=inputz*100 - 10;
    }
    while(1) {
        for(int j = 0; j < 100; j++) {
            valDedo[0] = valDedo[0] + (4*(dedo[0] - menique*100)) + 50;
            valDedo[1] = valDedo[1] + (4*(dedo[1] - anular*100)) + 50;
            valDedo[2] = valDedo[2] + (4*(dedo[2] - medio*100)) + 50;
            valDedo[3] = valDedo[3] + (4*(dedo[3] - indice*100)) + 50;
            valDedo[4] = valDedo[4] + (4*(dedo[4] - pulgar*100)) + 50;
        }

        valDedo[0] = valDedo[0]/100;
        valDedo[1] = valDedo[1]/100;
        valDedo[2] = valDedo[2]/100;
        valDedo[3] = valDedo[3]/100;
        valDedo[4] = valDedo[4]/100;

        for(int t = 0; t < 5; t++) {
            if(valDedo[t] < 0) {
                valDedo[t] = valDedo[t] * -1;
            }
        }

        x=(4*(cal_x - inputx*100)) + 50;
        y=(4*(cal_y - inputy*100)) + 50;
        z=(4*(inputz*100 - cal_z)) + 50;

        sprintf(valor, "[%d:%d:%d:%d:%d:%d:%d]", valDedo[0],valDedo[1],valDedo[2],valDedo[3],valDedo[4],x,y);

        XBEE.printf("%s",valor);
        pc.printf("M: %d, A: %d, M: %d, I: %d, P: %d \n", valDedo[0],valDedo[1],valDedo[2],valDedo[3],valDedo[4]);
        pc.printf("X: %d, Y: %d, Z: %d \n", x,y,z);

        wait(0.1);
    }
}
