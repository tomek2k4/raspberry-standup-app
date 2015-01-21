



//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef MAIN_C_INIT		//Do only once the first time this file is used
#define	MAIN_C_INIT


//Outputs
//#define LED_GREEN(state)				bcm2835_gpio_write(RPI_V2_GPIO_P1_24, !state)

//Inputs
//bcm2835_gpio_lev returns uint8_t
//#define SW_A_INPUT						bcm2835_gpio_lev(RPI_V2_GPIO_P1_03)





#endif



//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef MAIN_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------
void initialise (void);
gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
static gboolean main_loop(GtkWidget *widget);
void process_countdown (bool KeyPressed, GdkEventKey *event);
void do_heartbeat (void);


//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)



#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------



#endif




//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef MAIN_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------
long int last_heartbeat;
long int heartbeat_difference;
struct timespec gettime_now;
int io_state = 0;
BYTE hb_10ms_timer = 0;
BYTE hb_100ms_timer = 0;
WORD hb_1sec_timer = 0;
int ScreenWidth;
int ScreenHeight;

GtkWidget *fixed;
GtkWidget *MainWindow;
GtkWidget *TimerLabel;





//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)
BYTE store_nv_memory_100ms_timer = 0;
WORD random_number_seed;
BYTE update_random_number_seed = 1;
BYTE do_reset = 0;



#else
//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------
extern BYTE store_nv_memory_100ms_timer;
extern WORD random_number_seed;
extern BYTE update_random_number_seed;
extern BYTE do_reset;




#endif











