/* File:   algorithm.c
 * Author: SuperKing
 * Created on 2016?5?9?, ??7:25*/

#include "algorithm.h"

int FXUM,FYUM,FZUM,TUM;
int HX,HY,HZ;
float T;
char DATAX_LSB,DATAX_MSB,DATAY_LSB,DATAY_MSB,DATAZ_LSB,DATAZ_MSB;

void Para_adjust()
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        Filter_ParaX[i]--;
        Filter_ParaY[i]--;
        Filter_ParaZ[i]--;
    }
}



int Son_filter(int sense[],int sense_filter[],uint16_t filter_para[])
{
    int i,temp;
    int N = Cur_I/2;
    int32_t filter_val = 0;
    
    int temp_para = filter_para[7];
    if(filter_para[7] >= N)
    {
        temp = filter_para[7] - N;
        filter_para[7] = N - 1;
        if(filter_para[0]<=temp)
            temp = filter_para[0] - 1;
        
         for(i=0;i<=temp;i++)
        {
            sense_filter[i] = 512-sense[i];
            filter_val += sense_filter[i];
        }
    }
    for(i=filter_para[0];i<=filter_para[1];i++)
    {
        sense_filter[i] = sense[i]-512;
        filter_val += sense_filter[i];
    }
    for(i=filter_para[2];i<=filter_para[3];i++)
    {
        sense_filter[i] = 512-sense[i];
        filter_val += sense_filter[i];
    }
    for(i=filter_para[4];i<=filter_para[5];i++)
    {
        sense_filter[i] = sense[i]-512;
        filter_val += sense_filter[i];
    }
    for(i=filter_para[6];i<=temp_para;i++)
    {
        sense_filter[i] = 512-sense[i];
        filter_val += sense_filter[i];
    }
    for(i=N+filter_para[0];i<N+filter_para[1];i++)
    {
        sense_filter[i] = sense[i]-512;
        filter_val += sense_filter[i];
    }
    for(i=N+filter_para[2];i<N+filter_para[3];i++)
    {
        sense_filter[i] = 512-sense[i];
        filter_val += sense_filter[i];
    }
    for(i=N+filter_para[4];i<N+filter_para[5];i++)
    {
        sense_filter[i] = sense[i]-512;
        filter_val += sense_filter[i];
    }
    for(i=N+filter_para[6];i<N+filter_para[7];i++)
    {
        sense_filter[i] = 512 -sense[i];
        filter_val += sense_filter[i];
    }
    return __builtin_divsd(filter_val,Cur_I);
}

void Filter()
{
    SenseX_Val = Son_filter(SenseX,SenseX_Filter,Filter_ParaX);
    SenseY_Val = Son_filter(SenseY,SenseY_Filter,Filter_ParaY);
    SenseZ_Val = Son_filter(SenseZ,SenseZ_Filter,Filter_ParaZ);
}
