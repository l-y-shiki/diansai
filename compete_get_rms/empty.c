#include "ti/driverlib/dl_adc12.h"
#include "ti_msp_dl_config.h"
#include "arm_const_structs.h"
#include "arm_math.h"
#include "math.h"
#include "my_delay.h"
#include "oled.h"
#include "bmp.h"
#include "stdio.h"

#define ADC_SAMPLE_SIZE (2048)
#define ADC_FIFO_SAMPLES (ADC_SAMPLE_SIZE >> 1)//传输数目减半
#define FFT_LEN 2048 
#define Wave_Num 10//最大谐波数
uint16_t gADC_I[ADC_SAMPLE_SIZE];//电流采集
uint16_t gADC_U[ADC_SAMPLE_SIZE];//电压采集
float fft_data_in[FFT_LEN*2]; 
float fft_data_out[FFT_LEN/2]; 
float I_wave[Wave_Num];//电流谐波数据
int index_1;
float THD = 0;
uint8_t ADC_flag = 0;
float I_phase;
float U_phase;
float D_phase;//相位差
float I_A;//电流幅值
float U_A;
float I_eff;//电流最后有效值
float U_eff;
float I_sum;
float I_rms;
float I_dc;
float power_factor;//功率因素
float P;//有功功率
uint8_t change_flag = 0;//换页标志
char sbuff[20];

//捕获
volatile uint32_t gCaptureCnt;
volatile bool gSynced;
volatile bool gCheckCaptures;
uint32_t gLoadValue;
//捕获

void CreateFFTData(float* data_in, uint16_t* Data_A, int len);
void Gather_Energy(float*arr, uint32_t index_1, uint8_t d, uint8_t wave_num);
float Measure_THD(float* arr, int index_1, int wave_num, float* I);//thd和谐波电流测量
void SeparateData(uint16_t* Arr, uint16_t *Data_A, uint16_t *Data_B, int len);//分离连续采集的数据
void MeasureAmplitude(float* arr, int n);
void ShowData(void);
void ShowMain(void);
void Get_Rms(void);
//采样频率2k,采样点数2k
int main(void)
{
    //捕获
    volatile static uint32_t pwmPeriod;
    __attribute__((unused)) volatile static uint32_t pwmDuty;
    //捕获

    SYSCFG_DL_init();
    DL_TimerA_stopCounter(TIMER_0_INST);
    // //adc0采集配置
    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, 0x40556280);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &gADC_I[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, ADC_SAMPLE_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    //adc1采集配置
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, 0x40558280);    
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &gADC_U[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, ADC_SAMPLE_SIZE);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);

    //捕获
     gLoadValue = DL_TimerG_getLoadValue(CAPTURE_0_INST);
    gSynced        = false;
    gCheckCaptures = false;
    DL_TimerG_setCoreHaltBehavior(
        CAPTURE_0_INST, DL_TIMER_CORE_HALT_IMMEDIATE);
    NVIC_EnableIRQ(CAPTURE_0_INST_INT_IRQN);
    DL_TimerG_startCounter(CAPTURE_0_INST);
        //捕获

    //OLED_Init();		//初始化OLED
   // ShowMain();
    while (1) {
        DL_TimerA_startCounter(TIMER_0_INST);//开始采集
        while(ADC_flag == 0);//等待采集,计算完成
        ADC_flag = 0;
//捕获
//while (false == gCheckCaptures){;}
        gCheckCaptures = false;
        pwmPeriod = gLoadValue - gCaptureCnt;
        pwmDuty   = ((gLoadValue - DL_TimerG_getCaptureCompareValue(
                                     CAPTURE_0_INST, DL_TIMER_CC_0_INDEX)) *
                      100) /
                  pwmPeriod;
//捕获

        // __BKPT();
       // ShowData();
        // __BKPT();
    }
}
void ShowMain(void)
{
    OLED_ShowString(xx[0],0,(uint8_t *)"I:",8);
    OLED_ShowString(xx[0],1,(uint8_t *)"U:",8);
    OLED_ShowString(xx[0],2,(uint8_t *)"F:",8);
    OLED_ShowString(xx[0],3,(uint8_t *)"P:",8);
    OLED_ShowString(xx[0],4,(uint8_t *)"THD:",8);

    OLED_ShowString(xx[0],5,(uint8_t *)"H1:",8);//基波频率
    OLED_ShowString(xx[0],6,(uint8_t *)"H2:",8);
    OLED_ShowString(xx[0],7,(uint8_t *)"H3:",8);

    OLED_ShowString(xx[10],0,(uint8_t *)"H4:",8);
    OLED_ShowString(xx[10],1,(uint8_t *)"H5:",8);
    OLED_ShowString(xx[10],2,(uint8_t *)"H6:",8);
    OLED_ShowString(xx[10],3,(uint8_t *)"H7:",8);
    OLED_ShowString(xx[10],4,(uint8_t *)"H8:",8);
    OLED_ShowString(xx[10],5,(uint8_t *)"H9:",8);
    OLED_ShowString(xx[10],6,(uint8_t *)"H10:",8);
    OLED_ShowString(xx[10],7,(uint8_t *)"f:",8);
}
void ShowData(void){
        sprintf(sbuff, "%6.4fA", I_eff);
        OLED_ShowString(xx[2],0,(uint8_t *)sbuff,8);
        
        sprintf(sbuff, "%5.1fV", U_eff);
        OLED_ShowString(xx[2],1,(uint8_t *)sbuff,8);
        
        sprintf(sbuff, "%.3f", power_factor);
        OLED_ShowString(xx[2],2,(uint8_t *)sbuff,8);

        sprintf(sbuff, "%6.2fW", P);
        OLED_ShowString(xx[2],3,(uint8_t *)sbuff,8);

        sprintf(sbuff, "%.3f", THD);
        OLED_ShowString(xx[4],4,(uint8_t *)sbuff,8);

        sprintf(sbuff, "%.4f", I_wave[0]);
        OLED_ShowString(xx[3],5,(uint8_t *)sbuff,8);

        sprintf(sbuff, "%.4f", I_wave[1]);
        OLED_ShowString(xx[3],6,(uint8_t *)sbuff,8);

        sprintf(sbuff, "%.4f", I_wave[2]);
        OLED_ShowString(xx[3],7,(uint8_t *)sbuff,8);
        for(int k = 4; k <= 9; k++)
        {
            sprintf(sbuff, "%.4f", I_wave[k-1]);
            OLED_ShowString(xx[14], k-4,(uint8_t *)sbuff,8);
        }
        sprintf(sbuff, "%.4f", I_wave[9]);
        OLED_ShowString(xx[14],6,(uint8_t *)sbuff,8);
        sprintf(sbuff, "%2dHz", index_1);
        OLED_ShowString(xx[14],7,(uint8_t *)sbuff,8);
}

void ADC12_0_INST_IRQHandler(void)//ADC0中断
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            DL_TimerA_stopCounter(TIMER_0_INST);
          // DL_ADC12_disableConversions(ADC12_0_INST);
            //process
            CreateFFTData(fft_data_in, gADC_I, FFT_LEN);
            arm_cfft_f32(&arm_cfft_sR_f32_len2048, fft_data_in, 0, 1);
            arm_cmplx_mag_f32(fft_data_in, fft_data_out, FFT_LEN/2);
            MeasureAmplitude(fft_data_out, FFT_LEN/2);

            arm_max_f32((float32_t*)fft_data_out, FFT_LEN, (float32_t *) &I_A, (uint32_t *) &index_1);//求基波和幅值
            if(index_1 < 100)
            {
                I_phase = atan2(fft_data_in[2*index_1+1], fft_data_in[2*index_1]);//电流相位
                Gather_Energy(fft_data_out, index_1, 5, Wave_Num);
                THD = Measure_THD(fft_data_out, index_1, Wave_Num, I_wave);//thd和谐波电流测量
                Get_Rms();
            }
           // __BKPT();
            break;
        default:
            break;
    }
}
void ADC12_1_INST_IRQHandler(void)//ADC0中断
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:
            DL_TimerA_stopCounter(TIMER_0_INST);
            //process
            CreateFFTData(fft_data_in, gADC_U, FFT_LEN);
            arm_cfft_f32(&arm_cfft_sR_f32_len2048, fft_data_in, 0, 1);
            arm_cmplx_mag_f32(fft_data_in, fft_data_out, FFT_LEN/2);
            MeasureAmplitude(fft_data_out, FFT_LEN/2);
            arm_max_f32((float32_t*)fft_data_out, FFT_LEN, (float32_t *) &U_A, (uint32_t *) &index_1);//求基波和幅值
            if(index_1 > 100)
            {
                index_1 = 50;
            }
            U_phase = atan2(fft_data_in[2*index_1+1], fft_data_in[2*index_1]);//电压相位

            D_phase = U_phase - I_phase;//计算相位差 
            power_factor = fabs(cos(D_phase));//功率因素
            // y = 0.6739x + 0.3412
            power_factor = 0.6739*power_factor + 0.3412;
            if(power_factor>1)
            {
                power_factor = 1;
            }
            // y = 98.544x + 127.26
            U_eff = 98.544*U_A+127.26;
            P = U_eff*I_eff*power_factor;
            ADC_flag = 1;//测量完成
            // __BKPT();
            break;
        default:
            break;
    }
}

void CAPTURE_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(CAPTURE_0_INST)) {
        case DL_TIMERG_IIDX_CC1_DN:
            if (gSynced == true) {
                gCaptureCnt = DL_TimerG_getCaptureCompareValue(
                    CAPTURE_0_INST, DL_TIMER_CC_1_INDEX);
                gCheckCaptures = true;
            } else {
                gSynced = true;
            }
            /* Manual reload is needed to workaround timer capture limitation */
            DL_TimerG_setTimerCount(CAPTURE_0_INST, gLoadValue);
            break;
        case DL_TIMERG_IIDX_ZERO:
            /* If Timer reaches zero then no PWM signal is detected and it
             * requires re-synchronization
             */
            gSynced = false;
            break;
        default:
            break;
    }
}


void Get_Rms(void)
{
    I_sum = 0;
    float temp;
    int k =100;
    for(int i = 0; i<40; i++)
    {
        temp = (float)gADC_I[i+k]*3.3/4095;
        I_sum += temp*temp;
    }
    I_sum = I_sum/40;
    I_rms = sqrt(I_sum-I_dc*I_dc);
}
void CreateFFTData(float* data_in, uint16_t* Data_A, int len)
{
    for(int i = 0; i < len; i++)
    {
        data_in[2 * i] = Data_A[i];
        data_in[2*i + 1] = 0;
    }
}
void Gather_Energy(float*arr, uint32_t index_1, uint8_t d, uint8_t wave_num)
{
    for(int i = 1; i <= wave_num; i++)
    {
        float energy = 0;
        energy += arr[i*index_1]*arr[i*index_1];
        for(int j = 1; j < d; j++)
        {
            energy += arr[i*index_1 - j]*arr[i*index_1 - j];
            energy += arr[i*index_1 + j]*arr[i*index_1 + j];
        }
        arr[i*index_1] = sqrt(energy);
    }
}

float Measure_THD(float* arr, int index_1, int wave_num, float* I)
{
    float sum = 0;
    for(int i = 1; i <= wave_num; i++)
    {
        //记录谐波电流(电压值)
        // y = 4.3503*I_rms-0.0585
        I_wave[i-1] = 4.3503*arr[index_1*i]-0.0585;//1-10次
        if(I_wave[i-1] < 0)
        {
            I_wave[i-1] = 0;
        }
        sum += arr[index_1*i]*arr[index_1*i];
    }
    return sqrt(sum)/arr[index_1];
}
void SeparateData(uint16_t* Arr, uint16_t *Data_A, uint16_t *Data_B, int len)//分离连续采集的数据
{
    for(int i = 0; i < len; i++)
    {
        if(i%2 == 0)
        {
            Data_A[i/2] = Arr[i];
        }
        else{
            Data_B[i/2+1] = Arr[i];
        }
    } 
}
void MeasureAmplitude(float* arr, int n)
{
    I_dc = arr[0]/(2*n)*3.3/4095;
    arr[0] = 0;
    for (int i = 1; i < n; i++)
    {
        arr[i] /= n;
        arr[i] = arr[i]*3.3/4095;//计算幅值
    }
}