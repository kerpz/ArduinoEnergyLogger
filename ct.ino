// CT sensor part
// ADC specs for wemos d1 mini
#define ADC_BITS    10
#define ADC_COUNTS  (1<<ADC_BITS)
double offsetI = ADC_COUNTS>>1;

void extractCT() {
    // Bmon part
    //int raw = analogRead(A0);
    //device_voltage = raw / 1023.0;
    //device_voltage = device_voltage * 4.2;

    // Emon part
    int SupplyVoltage = 3200;
    unsigned int Number_of_Samples = 1480;
    double sumI, sqI, filteredI;
    for (unsigned int n = 0; n < Number_of_Samples; n++)
    {
      int sampleI = analogRead(A0);
      offsetI = (offsetI + (sampleI-offsetI)/1024);
      filteredI = sampleI - offsetI;
      sqI = filteredI * filteredI;
      sumI += sqI;
    }
    double I_RATIO = ct_calibration * ((SupplyVoltage / 1000.0) / (ADC_COUNTS));
    ct_current = I_RATIO * sqrt(sumI / Number_of_Samples);

    ct_power = (ct_current * ct_voltage * ct_pf); // watts / power apparent
}
