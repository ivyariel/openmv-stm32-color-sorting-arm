#include "control.h"

void arm_go_to_mid(void)
{
	bus_servo_control(254, 2048, 1000);
}

void arm_reset(void)
{
    /*bus_servo_control(1, 2048, 2000); // 1000: open; 2048: closed
		bus_servo_control(2, 2260, 2000);
    bus_servo_control(3, 2048, 2000);	
    bus_servo_control(4, 2100, 2000); //302  
    bus_servo_control(5, 2048, 2000);*/
    bus_servo_control(6, 2048, 2000);
}
// Move the arm to the color block pickup position.
void arm_A_detect_pos(void)
{
    bus_servo_control(6, 3250, 1500);
    bus_servo_control(4, 3000, 1500);
    bus_servo_control(1, 1000, 1500);
    delay_ms(1000);  
	bus_servo_control(3, 2600, 1500);
	bus_servo_control(4, 3000, 1500);
    bus_servo_control(5, 2600, 1500);
    delay_ms(500);
    bus_servo_control(3,1500 , 1500);
 	bus_servo_control(4, 2800, 1500);
    bus_servo_control(5, 3000, 1500); 
    delay_ms(1000);    
} 
// Place the green block.
void put_green(void) 
{
    bus_servo_control(1, 2048, 1500);
    delay_ms(1500);  
    bus_servo_control(3,3100 , 1500);
    bus_servo_control(4, 1600, 2000);
    bus_servo_control(5, 2500, 2000);   

    delay_ms(1000);
    bus_servo_control(1, 2048, 1000);
    delay_ms(1000); 
    /* Release the color block. */
    bus_servo_control(6,1025 , 3000);
    delay_ms(1000);
     bus_servo_control(3,1500 , 2000);
 	bus_servo_control(4, 2800, 2000);
    bus_servo_control(5, 3000, 2000); 
    delay_ms(1500);
    bus_servo_control(1, 1000, 1000);
    delay_ms(1000);  
   bus_servo_control(3,1300 , 1000);    
    bus_servo_control(4, 2900, 1000);
    bus_servo_control(5, 2048, 1000);
    delay_ms(200);  
}
// Place the blue block.
void put_blue(void)
{
    bus_servo_control(1, 2048, 1500);
    delay_ms(1500);  
    bus_servo_control(3,3100 , 1500);
    bus_servo_control(4, 1600, 2000);
    bus_servo_control(5, 2500, 2000);   

    delay_ms(1000);
    bus_servo_control(1, 2048, 1000);
    delay_ms(1000); 
	/* Release the color block. */
    bus_servo_control(6,1380 , 3000);
    delay_ms(1500);
    bus_servo_control(3,1500 , 2000);
 	bus_servo_control(4, 2800, 2000);
    bus_servo_control(5, 3000, 2000); 
    delay_ms(1500);
    bus_servo_control(1, 1000, 1000);
    delay_ms(1000);  
    bus_servo_control(3,1300 , 1000);    
    bus_servo_control(4, 2900, 1000);
    bus_servo_control(5, 2048, 1000);
    delay_ms(200); 
}
// Place the red block.
void put_red(void)
{
    bus_servo_control(1, 2048, 1500);
    delay_ms(1500);  
    bus_servo_control(3,3100 , 1500);
    bus_servo_control(4, 1600, 2000);
    bus_servo_control(5, 2500, 2000);   

    delay_ms(1000);
    bus_servo_control(1, 2048, 1000);
    delay_ms(1000); 
	/* Release the color block. */
    bus_servo_control(6,640 , 3000);
    delay_ms(1500);
    bus_servo_control(3,1500 , 2000);
 	bus_servo_control(4, 2800, 2000);
    bus_servo_control(5, 3000, 2000); 
    delay_ms(1500);
    bus_servo_control(1, 1000, 1000);
    delay_ms(1000);  
   bus_servo_control(3,1300 , 1000);    
    bus_servo_control(4, 2900, 1000);
    bus_servo_control(5, 2048, 1000);
    delay_ms(200);   
}

void arm_put_action(uint8_t color)
{
	if (color == 1)// Red
	{
        put_red();
	}
	else if (color == 2)
	{
        put_blue();
	}
	else if (color == 4)
	{
		put_green();
	}
}

/* Run three pick-and-place cycles using the color reported by OpenMV.
 * The pickup and drop-off positions are the existing fixed poses above.
 */
void servo_control(void)
{
    static uint8_t phase = 0;
    static uint8_t completed = 0;
    uint8_t color;

    if (completed >= 3)
    {
        delay_ms(100);
        return;
    }

    if (phase == 0)
    {
        arm_reset();
        delay_ms(2000);
        arm_A_detect_pos();
        Openmv.m = 0;  // Discard detections made while the arm was moving.
        phase = 1;
        return;
    }

    color = (uint8_t)Openmv.m;
    if (color != 1 && color != 2 && color != 4)
    {
        delay_ms(50);
        return;
    }

    Openmv.m = 0;
    arm_put_action(color);
    delay_ms(1000);
    completed++;
    phase = 0;
}
