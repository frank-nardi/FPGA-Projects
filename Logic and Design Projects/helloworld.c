#include "stdbool.h"
#include "stdint.h"

#define portA *(unsigned volatile*)0x40000000
#define LED *(unsigned volatile*)0x40000008

#define ADC1 *(int volatile*)0x44A10258
#define ADC2 *(int volatile*)0x44A10278
#define timer_dur *(unsigned volatile*)0x44A00008
#define timer_state *(unsigned volatile*)0x44A00004

#define pmod_counter *(unsigned volatile*)0x44A20000

#define BTN *(unsigned volatile*)0x40010008
#define btnU_offset 0b1000
#define btnD_offset 0b0100
#define btnL_offset 0b0010
#define btnR_offset 0b0001

#define HEX_DATA 1
#define RAW_DATA 0


#define SW *(unsigned volatile*)0x40010000

// seven segment display (SSD) signals
    // HEX data, 0xFEEF will show FEEF on SSD
#define SSD_HEX *(unsigned volatile*)0x44A50000

    // RAW data top TWO bytes, 0xFE will show FE, 0b_ will show
    // the raw data allowing for non standard characters to be
    // printed (not 0 through F)
#define SSD_RAW_TOP *(unsigned volatile*)0x44A50004

    // bottom TWO bytes of raw segment data
#define SSD_RAW_BOT *(unsigned volatile*)0x44A50008

    // mode and display point position. set bit 4 to 1 to use hex
    // data mode, set bit 4 to 0 for raw data mode
    // set bits [3:0] to 1 to display a decimal point after
    // segment symbol i, where i is the bit being set to 1.
#define SSD_MODE_DP *(unsigned volatile*)0x44a5000C


// this is a value in mV that we define to be a logical 1 for the purposes of metal detection input into AD converter.
#define threshold_mV 200
#define threshold_duration 9

#include "xil_printf.h"

void delay_n_secs(uint8_t seconds)
{
    timer_dur = 100 * 1000 * 1000 * seconds;

    while( (timer_state & 0b1) == 0){}

    return;
}

void delay_n_msecs(uint16_t millis)
{
    timer_dur = 100 * 1000 * millis;

    while( (timer_state & 0b1) == 0){}

    return;
}

void printSSD(_Bool is_hex, uint32_t whole_vector, uint8_t dp_vector)
{

    if (is_hex)
    {
        SSD_MODE_DP = 0x10 | (dp_vector & 0xF);
        SSD_HEX = whole_vector & 0xFFFF;
    }
    else
    {
        SSD_MODE_DP = dp_vector & 0xF;
        SSD_RAW_TOP = (whole_vector >> 14) & 0x3FFF;
        SSD_RAW_BOT = whole_vector & 0x3FFF;
    }

    return;
}

void update_SSD_data(_Bool detected_left, _Bool detected_right, _Bool is_close)
{
    if (!detected_left && !detected_right)
    {
        // raw mode, vector says ndet, decimal point after n to imply two word output
        printSSD(RAW_DATA, 0b0101011010000100001100000111, 0b1000);
        return;
    }

    else if (detected_left && detected_right)
    {
        // raw mode, vector says cntr, no decimal point.
        printSSD(RAW_DATA, 0b1000110010101100001110101111, 0b0000);
        return;
    }

    if(!is_close)
    {

    if (detected_left && !detected_right)
    {// far left case
            // raw mode, vector says F.LFt
            printSSD(RAW_DATA, 0b0001110100011100011100000111, 0b1000);
            return;
    }

    else if (!detected_left && detected_right)
    {// far right case
          printSSD(RAW_DATA, 0b0001110010111100100000000111, 0b1000);
          return;
    }
    }

    else
    {
        if (detected_left && !detected_right)
            {// far left case
                    // raw mode, vector says LEFt, no decimal point.
                    printSSD(RAW_DATA, 0b1000111000011000011100000111, 0b1000);
                    return;
            }

            else if (!detected_left && detected_right)
            {// far right case
                // raw mode, vector says rght, no decimal point.
                printSSD(RAW_DATA, 0b0101111001000000010010000111, 0b0000);
                  return;
            }
    }
}

void print_title()
{
    printSSD(RAW_DATA, 0b1001000111100000001100000111, 0b0000);
    delay_n_msecs(500);
    printSSD(RAW_DATA, 0b0000110000011100010001000111, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b0000111000100010001111110111, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b0001000100011111101110100001, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b1000111111011101000010000110, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b1110111010000100001100000111, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b0100001000011000001110000110, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b0000110000011100001101000110, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b0000111000011010001100000111, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b0000110100011000001111000000, 0b0000);
    delay_n_msecs(250);
    printSSD(RAW_DATA, 0b1000110000011110000000101111, 0b0000);
    delay_n_msecs(500);

    return;
}

_Bool btn_R_deb(void)
{
    enum state {st_idle, st_is_pressed, st_is_debounced, st_reset, st_post_debounce};
    static enum state st = st_reset;
    static uint8_t times_pressed = 0;
    _Bool btn = BTN & btnR_offset;
    _Bool retval = false;

    switch (st)
    {
        default:
        case st_reset:
            // reset, we want count to go to zero
            times_pressed = 0;
            // go immediately to idle
            st = st_idle;
            // default retval to false.
            retval = false;
            break;

        case st_idle:
            // check if button has been pressed once
            if (btn)
            {
                // if so, we go to is_pressed state
                st = st_is_pressed;
            }
            break;

        case st_is_pressed:
            // if we are in this state for so long that times_pressed reaches 5, we go to is_debounced
            if (btn && ++times_pressed >= 5)
            {
                st = st_is_debounced;
            }
            break;

        case st_is_debounced:
            // we set retval to true, and leave immediately.
            st = st_post_debounce;
            retval = true;
            break;

        case st_post_debounce:
            // sit in here until button goes low, then return to idle.
            if (!btn)
            {
                st = st_idle;
            }
            break;
    }


    return retval;
}

_Bool btn_L_deb(void)
{
    enum state {st_idle, st_is_pressed, st_is_debounced, st_reset, st_post_debounce};
    static enum state st = st_reset;
    static uint8_t times_pressed = 0;
    _Bool btn = BTN & btnL_offset;
    _Bool retval = false;

    switch (st)
    {
        default:
        case st_reset:
            // reset, we want count to go to zero
            times_pressed = 0;
            // go immediately to idle
            st = st_idle;
            // default retval to false.
            retval = false;
            break;

        case st_idle:
            // check if button has been pressed once
            if (btn)
            {
                // if so, we go to is_pressed state
                st = st_is_pressed;
            }
            break;

        case st_is_pressed:
            // if we are in this state for so long that times_pressed reaches 5, we go to is_debounced
            if (btn && ++times_pressed >= 5)
            {
                st = st_is_debounced;
            }
            break;

        case st_is_debounced:
            // we set retval to true, and leave immediately.
            st = st_post_debounce;
            retval = true;
            break;

        case st_post_debounce:
            // sit in here until button goes low, then return to idle.
            if (!btn)
            {
                st = st_idle;
            }
            break;
    }


    return retval;
}

_Bool btn_U_deb(void)
{
    enum state {st_idle, st_is_pressed, st_is_debounced, st_reset, st_post_debounce};
    static enum state st = st_reset;
    static uint8_t times_pressed = 0;
    _Bool btn = BTN & btnU_offset;
    _Bool retval = false;

    switch (st)
    {
        default:
        case st_reset:
            // reset, we want count to go to zero
            times_pressed = 0;
            // go immediately to idle
            st = st_idle;
            // default retval to false.
            retval = false;
            break;

        case st_idle:
            // check if button has been pressed once
            if (btn)
            {
                // if so, we go to is_pressed state
                st = st_is_pressed;
            }
            break;

        case st_is_pressed:
            // if we are in this state for so long that times_pressed reaches 5, we go to is_debounced
            if (btn && ++times_pressed >= 5)
            {
                st = st_is_debounced;
            }
            break;

        case st_is_debounced:
            // we set retval to true, and leave immediately.
            st = st_post_debounce;
            retval = true;
            break;

        case st_post_debounce:
            // sit in here until button goes low, then return to idle.
            if (!btn)
            {
                st = st_idle;
            }
            break;
    }


    return retval;
}

_Bool btn_D_deb(void)
{
    enum state {st_idle, st_is_pressed, st_is_debounced, st_reset, st_post_debounce};
    static enum state st = st_reset;
    static uint8_t times_pressed = 0;
    _Bool btn = BTN & btnD_offset;
    _Bool retval = false;

    switch (st)
    {
        default:
        case st_reset:
            // reset, we want count to go to zero
            times_pressed = 0;
            // go immediately to idle
            st = st_idle;
            // default retval to false.
            retval = false;
            break;

        case st_idle:
            // check if button has been pressed once
            if (btn)
            {
                // if so, we go to is_pressed state
                st = st_is_pressed;
            }
            break;

        case st_is_pressed:
            // if we are in this state for so long that times_pressed reaches 5, we go to is_debounced
            if (btn && ++times_pressed >= 5)
            {
                st = st_is_debounced;
            }
            break;

        case st_is_debounced:
            // we set retval to true, and leave immediately.
            st = st_post_debounce;
            retval = true;
            break;

        case st_post_debounce:
            // sit in here until button goes low, then return to idle.
            if (!btn)
            {
                st = st_idle;
            }
            break;
    }


    return retval;
}

void calibration ( uint16_t* p_adc1_duration, uint16_t* p_adc2_duration)
{

    uint16_t adc1_min = UINT16_MAX;
    uint16_t adc2_min = UINT16_MAX;

    uint16_t adc1_temp = 0;
    uint16_t adc2_temp = 0;

    printSSD(0, 0b1000110000100010001110000011, 0b0001);
    delay_n_secs(1);

    for (int i = 0; i < 1000; i++)
    {
        timer_dur = 100;
        adc1_temp = ( (ADC1 >> 4) * 244 / 1000);
        if (adc1_min > adc1_temp)
        {
            adc1_min = adc1_temp;
        }
        while( (timer_state & 0b1) == 0){}

        timer_dur = 100;
        adc2_temp = ( (ADC2 >> 4) * 244 / 1000);
        if (adc2_min > adc2_temp)
        {
            adc2_min = adc2_temp;
        }
        while( (timer_state & 0b1) == 0){}
    }

    printSSD(RAW_DATA, 0b0001000010000110001101111001, 0b0000);
    delay_n_secs(1);
    printSSD(HEX_DATA, adc1_min, 0b0000);
    delay_n_secs(1);

    printSSD(RAW_DATA, 0b0001000010000110001100100100, 0b0000);
    delay_n_secs(1);
    printSSD(HEX_DATA, adc2_min, 0b0000);
    delay_n_secs(1);


    *p_adc1_duration = adc1_min;
    *p_adc2_duration = adc2_min;

    return;
}

// bools needed to test if a newly seen object is detected, and whether that applies
// to the close or far versions of non-centered positions
void print_num_objects_SSD(_Bool adc1, _Bool adc2, _Bool is_close, uint8_t mode_input)
{
    static uint8_t one_second_cntr = 0;

    static uint8_t f_left_cnt = 0;
    static uint8_t left_cnt = 0;
    static uint8_t center_cnt = 0;
    static uint8_t right_cnt = 0;
    static uint8_t f_right_cnt = 0;

    enum current_display {title_fleft, value_fleft, title_left, value_left, title_center, value_center, title_right, value_right, title_fright, value_fright};

    static enum current_display state = title_left;

    enum counter_list {fleft, left, cntr, right, fright, reset};
    static enum counter_list previous_count = reset;
    // logic to update the seen_counts variable based on current and previous ADC digital signals

    _Bool is_right = !adc1 && adc2;

    _Bool is_left = adc1 && !adc2;

    _Bool is_center = adc1 && adc2;


    // if metal is detected on both sensors
    if (is_center)
    {
        // to not count the same object twice in center_cnt
        if (previous_count != cntr && (one_second_cntr % 5) == 0)
        {
            xil_printf("cntr\n\r");
            previous_count = cntr;
            center_cnt++;
        }
    }

    // if metal is detected on left sensor only
    else if (is_left)
    {
        if (is_close)
        {
            if (previous_count != left && (one_second_cntr % 5) == 0)
            {
                left_cnt++;
                previous_count = left;
            }
        }
        else
        {
            if (previous_count != fleft && (one_second_cntr % 5) == 0)
            {
                f_left_cnt++;
                previous_count = fleft;
            }

        }
    }

    // if metal is detected on right sensor only
    else if (is_right)
    {
        if (is_close)
        {
            if (previous_count != right && (one_second_cntr % 5) == 0)
            {
                right_cnt++;
                previous_count = right;
            }
        }
        else
        {
            if (previous_count != fright && (one_second_cntr % 5) == 0)
            {
                f_right_cnt++;
                previous_count = fright;
            }
        }
    }
    else
    {
        previous_count = reset;
    }

    if (mode_input == 2)
    {
        switch(state)
        {
        case title_fleft:
            // F.lft
            printSSD(RAW_DATA, 0b0001110100011100011100000111, 0b1000);
            break;

        case value_fleft:
            printSSD(HEX_DATA, f_left_cnt, 0b0000);
            break;

        case title_left:
            // LEFt
            printSSD(RAW_DATA, 0b1000111000011000011100000111, 0b0000);
            break;

        case value_left:
            printSSD(HEX_DATA, left_cnt, 0b0000);
            break;

        case title_center:
            // Cntr
            printSSD(RAW_DATA, 0b1000110010101100001110101111, 0b0000);
            break;

        case value_center:
            printSSD(HEX_DATA, center_cnt, 0b0000);
            break;

        case title_right:
            // rght
            printSSD(RAW_DATA, 0b0101111001000000010110000111, 0b0000);
            break;

        case value_right:
            printSSD(HEX_DATA, right_cnt, 0b0000);
            break;

        case title_fright:
            // f.rgt
            printSSD(RAW_DATA, 0b0001110010111100100000000111, 0b1000);
            break;

        case value_fright:
            printSSD(HEX_DATA, f_right_cnt, 0b0000);
            break;
        }
    }

    // 20 mS per main loop cycle, every 50 cycles is then 1 second, rotate state.
    if ( (one_second_cntr++) == 50)
    {
        one_second_cntr = 0;
        state = (state + 1) % 10;
    }

}

void set_detected_value(uint16_t *p_adc1_threshold, uint16_t *p_adc2_threshold)
{
    // this function can set the threshold value for adc1 and adc2 individually.
    // The threshold value is what the adc compares to against the default calibrated
    // adc value for a metal detection.

    // a detection is seen when the sampled adc value is less than the calibrated value
    // minus its threshold, so the threshold determines the drop in mV necessary to detect metal.

    // set
    printSSD(RAW_DATA, 0b0010010000011000001111111111, 0b0000);
    delay_n_secs(1);

    //adc
    printSSD(RAW_DATA, 0b001000010000110001101111111, 0b0000);
    delay_n_secs(1);

    //trsh (threshold)
    printSSD(RAW_DATA, 0b0000111010111100100100001011, 0b0001);
    delay_n_secs(1);

    do
    {
        // 20 mS loop to update SSD display
        timer_dur = 100 * 1000 * 20;



        // if SW[0] is 0, set adc1 threshold value
        if ( (SW & 0b1) == 0)
        {
            // first segment indicates ADC chosen, last three indicate current threshold value.
            printSSD(HEX_DATA, (0b0001 << 12) | (pmod_counter & 0xFFF), 0b1000);
        }

        // otherwise we set adc2 threshold value
        else
        {
            // first segment indicates ADC chosen, last three indicate current threshold value.
            printSSD(HEX_DATA, (0b0010 << 12) | (pmod_counter & 0xFFF), 0b1000);
        }

        while ((timer_state & 0b1) == 0) {}
    }
    while (!btn_U_deb());

    // if SW[0] = 0 set adc1
    if ( (SW & 0b1) == 0)
    {
        *p_adc1_threshold = pmod_counter;
        printSSD(RAW_DATA, 0b0001000010000110001101001111, 0b0000);
        delay_n_secs(1);
        printSSD(RAW_DATA, 0b0010010000011000001111111111, 0b0000);
        delay_n_secs(1);
    }

    // else set adc2
    else
    {
        *p_adc2_threshold = pmod_counter;
        printSSD(RAW_DATA, 0b0001000010000110001100100100, 0b0000);
        delay_n_secs(1);
        printSSD(RAW_DATA, 0b0010010000011000001111111111, 0b0000);
        delay_n_secs(1);
    }
    // reset timer_dur to not get caught in main loop waiting for timer_state
    timer_dur = 1000;

    return;
}

void set_close_value(uint16_t* p_adc1_close_value, uint16_t* p_adc2_close_value)
{
    // this defines the value that an adc needs to see to be considered a close
    // this decides whether to display "far left" or "left" for example.

    // set
    printSSD(RAW_DATA, 0b0010010000011000001111111111, 0b0000);
    delay_n_secs(1);

    //adc
    printSSD(RAW_DATA, 0b001000010000110001101111111, 0b0000);
    delay_n_secs(1);

    // clse. (close value)
    printSSD(RAW_DATA, 0b1000110100011100100100000110, 0b0001);
    delay_n_secs(1);

    do
    {
            // 20 mS loop to update SSD display
        timer_dur = 100 * 1000 * 20;


        // if SW[0] is 0, set adc1 close value
        if ( (SW & 0b1) == 0)
        {
            // first segment indicates ADC chosen, last three indicate current threshold value.
            printSSD(HEX_DATA, (0b0001 << 12) | (pmod_counter & 0xFFF), 0b1000);
        }

        // otherwise we set adc2 close value
        else
        {
            // first segment indicates ADC chosen, last three indicate current threshold value.
            printSSD(HEX_DATA, (0b0010 << 12) | (pmod_counter & 0xFFF), 0b1000);
        }

        while ((timer_state & 0b1) == 0) {}
    }
    while (!btn_D_deb());

    // update adc1 close value
    if ( (SW & 0b1) == 0)
    {
        *p_adc1_close_value = pmod_counter;

        // adc1
        printSSD(RAW_DATA, 0b0001000010000110001101001111, 0b0000);
        delay_n_secs(1);

        // set
        printSSD(RAW_DATA, 0b0010010000011000001111111111, 0b0000);
        delay_n_secs(1);
    }

    // else update adc2 close value
    else
    {
        *p_adc2_close_value = pmod_counter;

        // adc2
        printSSD(RAW_DATA, 0b0001000010000110001100100100, 0b0000);
        delay_n_secs(1);

        // set
        printSSD(RAW_DATA, 0b0010010000011000001111111111, 0b0000);
        delay_n_secs(1);
    }

    timer_dur = 1000;

    return;
}

int main()
{

    enum mode {position, strength, num_objects};

    enum mode current_mode = position;

    _Bool adc1_digital = false;
    _Bool adc2_digital = false;

    _Bool is_close = false;

    uint16_t adc1_cal = 0;
    uint16_t adc2_cal = 0;

    uint16_t adc1_val = 0;
    uint16_t adc2_val = 0;

    uint16_t threshold_voltage_adc1 = 100;
    uint16_t threshold_voltage_adc2 = 100;

    uint16_t is_close_threshold_adc1 = 200;
    uint16_t is_close_threshold_adc2 = 200;

    uint16_t default_total = 0;
    uint16_t LED_unit = 0;

    print_title();

    calibration(&adc1_cal, &adc2_cal);

    default_total = adc1_cal + adc2_cal;

    LED_unit = default_total / 16;

    while(1)
    {
        // 20 mS
        timer_dur = 100 * 1000 * 20;

        adc1_val = ((ADC1 >> 4) * 244 ) / 1000;
        adc2_val = ((ADC2 >> 4) * 244 ) / 1000;

        if (btn_L_deb())
        {
            if (current_mode) // mode >= 0 (can go left)
            {
                current_mode -= 1;
            }
        }

        if (btn_R_deb())
        {
            // cap mode to 2, since mode enum is size 3.
            current_mode = (current_mode + 1) % 3;
        }

        if (btn_U_deb())
        {
            set_detected_value(&threshold_voltage_adc1, &threshold_voltage_adc2);
        }

        if (btn_D_deb())
        {
            set_close_value(&is_close_threshold_adc1, &is_close_threshold_adc2);
        }

        // determine a close value if the sampled value is less than half the calibrated value
        is_close = adc1_val < (adc1_cal - is_close_threshold_adc1) || adc2_val < (adc2_cal - is_close_threshold_adc2);

        adc1_digital = adc1_val < adc1_cal - threshold_voltage_adc1;
        adc2_digital = adc2_val < adc2_cal - threshold_voltage_adc2;

        if (default_total - adc1_val - adc2_val > 0)
            LED = 0xFFFF & ~(0xFFFF >> ((default_total - adc1_val - adc2_val) / LED_unit));
        else
            LED = 0x0000;


        switch (current_mode)
        {
            case position:
                // position mode, prints F.LFt, LEFt, Cntr, rght, F.rgt
                // depending on digitalized adc value readings, and is_close bool.
                update_SSD_data(adc1_digital, adc2_digital, is_close);
            break;

            case strength:
                // strength mode, prints a numeric value in HEX of the ADC calibrated values
                // minus their current values (metallic object decreases observed voltage on coils)
                if (default_total - (adc1_val + adc2_val) > 0)
                printSSD(HEX_DATA, (default_total - (adc1_val + adc2_val)), 0b0000);
                else
                // clips lower bound to 0 in case of overflow (will show 0xF... on SSD)
                printSSD(HEX_DATA, 0b0000, 0b0000);
            break;
            default:
                // we dont care what this does in current_mode = num_objects
            break;

        }
        // this counts number of objects seen, it must be calculated every cycle.
        // whether or not this actually outputs depends on the current_mode input
        print_num_objects_SSD(adc1_digital, adc2_digital, is_close, current_mode);

        while((timer_state & 0b1) == 0){}
    }

    return 0;
}
