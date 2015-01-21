 
//************************************************************
//************************************************************
//**														**
//**	Title:												**
//** 	Device / Size:	Raspberry Pi						**
//**	Programmed by:										**
//**	Copyright (c):	Cabe Atwell 						**
//**					All rights reserved					**
//**														**
//**														**
//************************************************************
//************************************************************


#include <stdio.h>
#include <time.h>

//#include <stdlib.h>
#include <string.h>
//#include <delays.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "main.h"
#define	MAIN_C
#include "ap-main.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>
#include <sys/time.h>

//********************************
//********************************
//********** INITIALISE **********
//********************************
//********************************
void initialise (void)
{

	//SETUP HEARTBEAT TIMER
	clock_gettime(CLOCK_REALTIME, &gettime_now);
	last_heartbeat = gettime_now.tv_nsec;

        printf("StandUp Timer set\n");
}

//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************



//***********************************
//***********************************
//********** MAIN FUNCTION **********
//***********************************
//***********************************
int main(int argc, char **argv)
{
	WORD w_temp;

	//**********************
	//**********************
	//***** INITIALISE *****
	//**********************
	//**********************

	//GENERAL INITIALISE
	initialise();

	//---------------------------------
	//----- CREATE THE GTK WINDOW -----
	//---------------------------------
	gtk_init(&argc, &argv);
	
	MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL); 		//GTK_WINDOW_TOPLEVEL = Has a titlebar and border, managed by the window manager.
	gtk_window_set_title(GTK_WINDOW(MainWindow), "Countdown");
	gtk_window_set_default_size(GTK_WINDOW(MainWindow), 1020, 738);		//Size of the the client area (excluding the additional areas provided by the window manager). Aiming for 1280x720 standard 720p resolution
	gtk_window_set_position(GTK_WINDOW(MainWindow), GTK_WIN_POS_CENTER);

	//Set background colour
	GdkColor color;
	gdk_color_parse("#000", &color);
	gtk_widget_modify_bg(MainWindow, GTK_STATE_NORMAL, &color);
	

	//Get screen size
	//int ScreenWidth;
	//int ScreenHeight;
	GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(MainWindow));
	ScreenWidth = gdk_screen_get_width(screen);			// in pixels
	ScreenHeight = gdk_screen_get_height(screen);
	
	//Create a Fixed Container
	//GtkWidget *fixed;
	fixed = gtk_fixed_new();
	gtk_widget_set_size_request(fixed, ScreenWidth, ScreenHeight);
	gtk_container_add(GTK_CONTAINER(MainWindow), fixed);
	gtk_widget_show(fixed);

	//----- ADD THE LABEL -----
	
	//Add Timer Label
	//GtkWidget *TimerLabel;
	TimerLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(TimerLabel), "");
	gtk_label_set_justify(GTK_LABEL(TimerLabel), GTK_JUSTIFY_CENTER);		//Only sets alignment of multiple lines
	
	//Centre align the label widget in the screen 
	gtk_widget_set_size_request(TimerLabel, ScreenWidth, 0);
	gtk_misc_set_alignment(GTK_MISC(TimerLabel), 0.5, 0.5);
	//gtk_fixed_put(GTK_FIXED(fixed), TimerLabel, 0, ((ScreenHeight >> 1) - FONT_SIZE));		//x, y from top left
	gtk_fixed_put(GTK_FIXED(fixed), TimerLabel, 0, 0);		//x, y from top left
	
	
	

	
	
	gtk_widget_show_all(MainWindow);
		

	//Set size
	gtk_window_maximize(GTK_WINDOW(MainWindow));
	gtk_window_set_resizable(GTK_WINDOW(MainWindow), FALSE);
	
	gtk_widget_show_all(MainWindow);
	
	//Close the application if the x button is pressed if ALT+F4 is used
	g_signal_connect(G_OBJECT(MainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	//Get keypresses
	g_signal_connect (G_OBJECT (MainWindow), "key_press_event", G_CALLBACK (on_key_press), NULL);


	//Create background timer
	g_timeout_add(1, (GSourceFunc) main_loop, (gpointer) MainWindow);  //Register the timer and set time in mS. The timer_event() function is called repeatedly until it returns FALSE.
	
	
	
	//----- ENTER THE GTK MAIN LOOP -----
	gtk_main();		//Enter the GTK+ main loop, will wait for events to happen 
	
	return 0;
}






//******************************
//******************************
//********** KEYPRESS **********
//******************************
//******************************
gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	process_countdown(TRUE, event);
	
	
	return FALSE;		//Return false so timer fires again  
}


//*********************
//*********************
//***** MAIN LOOP *****
//*********************
//*********************
//CALLED FROM A TIMER WITHIN gtk_main()
static gboolean main_loop(GtkWidget *widget)
{
	static bool in_progress = 0;
	
	//Protect against threaded calls on top of each other
	if (in_progress)
		return TRUE;
	in_progress = 1;

		
	
	//----- PROCESS USER MODE -----
	process_countdown(FALSE, NULL);
	


	//----- DO HEARTBEAT -----
	do_heartbeat();


	in_progress = 0;
	return TRUE;
}


//***************************************
//***************************************
//********** PROCESS COUNTDOWN **********
//***************************************
//***************************************
void process_countdown (bool KeyPressed, GdkEventKey *event)
{
	static int seconds_last = 99;
	static int auto_countdown_hold_at_end_secs = 0;
	static int auto_countdown_secs_to_go = 0;
	char TempString[256];
	char TimeString[256];
	char ValueString[32];
	bool auto_countdown_reset_leds = 0;

	

	//----- CHECK FOR SPECIAL KEYPRESSES -----
	if (KeyPressed)
	{
		if ((event->state & GDK_CONTROL_MASK) && (event->keyval == 't'))
		{
			//CTRL+T to trigger auto countdown

			if (auto_countdown_secs_to_go == 0)
				auto_countdown_secs_to_go = 3615;
			else if (auto_countdown_secs_to_go > 75)
				auto_countdown_secs_to_go = 75;
			else
				auto_countdown_secs_to_go = 15;
			
			auto_countdown_hold_at_end_secs = 10;

			//Cause to be shown
			seconds_last = 99;
		}
	}
	
	//----- GET CURRENT TIME -----
	time_t now;
	struct tm newyear;
	double seconds;
	int display_days = 0;
	int display_hours = 0;
	int display_mins = 0;
	int display_secs;
	int font_size;
	
	time(&now);  //Get current time
	newyear = *localtime(&now);
	
	//Only update if seconds have changed
	if (seconds_last == newyear.tm_sec)
		return;
	seconds_last = newyear.tm_sec;
	
	
	//----- SET TARGET DATE AND TIME -----
	newyear.tm_hour = 10;
	newyear.tm_min =15 ;
	newyear.tm_sec = 0;
	newyear.tm_mday++;


	//----- GET TIME UNTIL TARGET -----	
	//Halt at zero on 1st Jan until midday 
	if ( (newyear.tm_mon == 0) && (newyear.tm_mday == 1) && (newyear.tm_hour > 12))
		seconds = 0;
	else
		seconds = difftime(mktime(&newyear), now);
	
	if (seconds < 0)		//Needed if the target is changed in the future to be a specific date and time rather than an auto next 1st Jan
		seconds = 0;

	
	//Override in test mode
	if (auto_countdown_secs_to_go)
	{
		seconds = auto_countdown_secs_to_go;
		auto_countdown_secs_to_go--;
	}
	else if (auto_countdown_hold_at_end_secs)
	{
		auto_countdown_hold_at_end_secs--;
		seconds = 0;
		
		if (auto_countdown_hold_at_end_secs == 0)
			auto_countdown_reset_leds = 1;
	}
	
	while (seconds >= (24 * 60 * 60))
	{
		seconds -= (24 * 60 * 60);
		display_days++;
	}
	while (seconds >= (60 * 60))
	{
		seconds -= (60 * 60);
		display_hours++;
	}
	while (seconds >= 60)
	{
		seconds -= 60;
		display_mins++;
	}
	display_secs = (int)seconds;
	
	
	//----- DISPLAY IT -----
	if (display_days > 0)
	{

		snprintf(TimeString, sizeof(TimeString), "StandUp in: %d:%02d:%02d:%02d", display_days, display_hours, display_mins, display_secs);
		font_size = (int)ScreenWidth / 20;
	}
	else if (display_hours > 0)
	{
		snprintf(TimeString, sizeof(TimeString), "StandUp in: %d:%02d:%02d", display_hours, display_mins, display_secs);
		font_size = (int)ScreenWidth / 20;
	}
	else if (display_mins > 0)
	{
		snprintf(TimeString, sizeof(TimeString), "StandUp in: %d:%02d", display_mins, display_secs);
		font_size = (int)ScreenWidth / 20;
	}
	else
	{
		snprintf(TimeString, sizeof(TimeString), "StandUp in: %d", display_secs);
		font_size = (int)ScreenWidth / 20;
	}

	
	//Set the vertical position of the label.  The font height value isn't actually the exact height of the characters in pixels, but using it as it works well enough. 
	gtk_fixed_move(GTK_FIXED(fixed), TimerLabel, 0, ((ScreenHeight >> 1) - font_size));
	

	//Display it
	strcpy(TempString, "<span foreground=\"white\"  font_desc=\"");
	snprintf(ValueString, sizeof(ValueString), "%d", font_size);		//Add value
	strcat(TempString, ValueString);
	strcat(TempString, ".0\">");
	
	
	strcat(TempString, TimeString);
	strcat(TempString, "</span>");
	gtk_label_set_markup(GTK_LABEL(TimerLabel), TempString);
	
	

	//----- DO COUNTDOWN OUTPUTS -----
	if ((display_days == 0) && (display_hours == 0) && (display_mins == 0) && (display_secs == 0))
        {
           //Do reminder stuff

        }
	//Reset the outptus
	if (
		(auto_countdown_reset_leds) ||
		( (newyear.tm_mon == 0) && (newyear.tm_mday == 1) && (newyear.tm_hour == 0) && (newyear.tm_min == 10))
	)
	{
	}

}




//*******************************
//*******************************
//********** HEARTBEAT **********
//*******************************
//*******************************
void do_heartbeat (void)
{
	static WORD speed_fade_prescaller = 1;


	clock_gettime(CLOCK_REALTIME, &gettime_now);
	heartbeat_difference = gettime_now.tv_nsec - last_heartbeat;

	if (heartbeat_difference < 0)
		heartbeat_difference += 1000000000;				//(Rolls over every 1 second)

	if (heartbeat_difference > 1000000)					//<<< Heartbeat every 1mS
	{
		//-------------------------
		//----- 1mS HEARTBEAT -----
		//-------------------------
		last_heartbeat += 1000000;						//<<< Heartbeat every 1mS
		if (last_heartbeat > 1000000000)				//(Rolls over every 1 second)
			last_heartbeat -= 1000000000;



		//Toggle a pin so we can verify the heartbeat is working using an oscilloscope
		/*
		io_state ^= 1;									//Toggle the pins state
		bcm2835_gpio_write(RPI_GPIO_P1_24, io_state);
		*/




		//*******************
		//***** 10 mSec *****
		//*******************
		hb_10ms_timer++;
		if (hb_10ms_timer == 10)
		{
			hb_10ms_timer = 0;





		} //if (hb_10ms_timer == 10)


		//********************
		//***** 100 mSec *****
		//********************
		hb_100ms_timer++;
		if (hb_100ms_timer == 100)
		{
			hb_100ms_timer = 0;




		}


		//*****************
		//***** 1 Sec *****
		//*****************
		hb_1sec_timer++;
		if (hb_1sec_timer == 1000)
		{
			hb_1sec_timer = 0;


		}


	} //if (heartbeat_difference > 1000000)
}








