//             GTK3
//
//  modified by Windy
#include "P6.h"
#include "Unix.h"
#include "gtk3.h"

#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#define M5WIDTH 320
#define M5HEIGHT 200


static cairo_surface_t * surface=NULL;

GdkPixbuf *offscreen=NULL; // offscreen
int        width=640;		// screen width
int        height=480;		// screen height

GtkWidget *da;            // drawing area

// ******************* init_drawing_area ****************************************
static gboolean init_drawing_area(GtkWidget *widget , GdkEventConfigure *event, gpointer data) {

	offscreen = gdk_pixbuf_new( GDK_COLORSPACE_RGB, FALSE, 8, width ,height);
	if( offscreen ==NULL)
		{
		 g_print("Error Create GdkPixbuf \n");
		 exit(1);
		}
	
	int n_ch = gdk_pixbuf_get_n_channels( offscreen);
	int row  = gdk_pixbuf_get_rowstride( offscreen);

	//surface = cairo_image_surface_create( CAIRO_FORMAT_RGB24 , width ,height);
	surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32 , width ,height);
	cairo_t *cr = cairo_create(surface);

	gdk_cairo_set_source_pixbuf( cr, offscreen ,0,0);
	cairo_paint(cr);
	cairo_destroy(cr);

	return TRUE;
}

// ****************** draw callback *************************************
static gboolean draw_cb( GtkWidget *widget, cairo_t * cr , gpointer data)
{

	cairo_set_source_surface( cr, surface,0,0);
	cairo_paint(cr);

   printf("draw_cb\n");
	return FALSE;
}

// ****************** OSD_get_pixel *************************************
// エミュレータのビットマップ作成ルーチンに、オフスクリーンのポインターを知らせる
byte *OSD_get_pixel(void)
{
  byte *p = gdk_pixbuf_get_pixels(offscreen);
  return p;
}

// ****************** timer callback *************************************
// 約1/60秒毎に呼び出される。まず、Z80 CPUを一画面分実行してから、生成されたビットマップを画面に描画する
//static gboolean timer_func(gpointer user_data)
static gboolean timer_func(GtkWidget *widget)
{
  static gint cnt = 0;

  if( offscreen ==NULL)
   {
  		return FALSE;
  }

 Z80();   // Z80 CPU 

 srand( time(NULL));

  g_print("progress:%2d\n", cnt++);

	int n_ch = gdk_pixbuf_get_n_channels( offscreen);
	int row  = gdk_pixbuf_get_rowstride( offscreen);

	//g_print("n_ch=%d  row=%d \n",n_ch , row);
	guchar *pixels;

	cairo_t *cr = cairo_create(surface);			//  offscreen -> drawing area		
	gdk_cairo_set_source_pixbuf( cr, offscreen ,0,0);
	cairo_paint(cr);
	cairo_destroy(cr);

  gtk_widget_queue_draw(da);				// fire draw event
  
  return (TRUE);
}


// ****************** close window *************************************
static void close_window(void)
{
	if(surface) 
		{
		cairo_surface_destroy( surface);
		}
	gtk_main_quit();
}





// 下記は、michinari-nukazawa さんのメニュープログラムです。
// https://blog.michinari-nukazawa.com/2015/12/gtk3-menubar-example.html




// gcc main_menu.c -Wall $(pkg-config --cflags --libs gtk+-3.0) -o main_menu && ./main_menu
/** @brief Menu example of gtk3 application.
 *
// “Mnemonics” (ex. "(F)ile > (Q)uit"), “Accelerators”(ex. "Ctrl+Q")
//
// michinari.nukazawa@gmail.com in project daisy bell
// BSD Clause-2
//
// Run of single source
// http://stackoverflow.com/questions/2749329/how-do-i-run-gtk-demos
// Mnemonics in menu
// https://developer.gnome.org/gtk3/stable/GtkMenuItem.html#gtk-menu-item-set-use-underline
// Accel
// https://developer.gnome.org/gtk3/stable/GtkAccelLabel.html#gtk-accel-label-set-accel
// https://mail.gnome.org/archives/commits-list/2015-April/msg06114.html
// https://developer.gnome.org/gtk3/stable/gtk3-Keyboard-Accelerators.html
// https://developer.gnome.org/gtk3/stable/GtkAccelLabel.html
**/


void cb_show_about_dialog (GtkMenuItem *menuitem, gpointer user_data)
{   
    const char *appname = "Menu example";
    GtkWindow *parent_window = NULL;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new (parent_window,
                                     flags,
                                     GTK_MESSAGE_QUESTION,
                                     GTK_BUTTONS_CLOSE,
                                     "This is :'%s'",
                                     appname);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

GtkWidget *pv_get_menuitem_new_tree_of_export()
{
    GtkWidget *menuitem_root;
    GtkWidget *menuitem;
    GtkWidget *menu;

    menuitem_root = gtk_menu_item_new_with_label ("Export");

    menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_root), menu);

    menuitem = gtk_menu_item_new_with_label ("jpeg/png");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
    menuitem = gtk_menu_item_new_with_label ("svg");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

    return menuitem_root;   
}

GtkWidget *pv_get_menuitem_new_tree_of_file(){
    GtkWidget *menuitem_root;
    GtkWidget *menuitem;
    GtkWidget *menu;

    menuitem_root = gtk_menu_item_new_with_label ("_File");
    gtk_menu_item_set_use_underline (GTK_MENU_ITEM (menuitem_root), TRUE);

    menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_root), menu);

    menuitem = gtk_menu_item_new_with_label ("Open");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
    menuitem = gtk_menu_item_new_with_label ("Save");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
    menuitem = gtk_menu_item_new_with_label ("Save As");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
    menuitem = pv_get_menuitem_new_tree_of_export();
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
    menuitem = gtk_menu_item_new_with_label ("Quit");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

    g_signal_connect(menuitem, "activate", G_CALLBACK(gtk_main_quit), NULL);

    return menuitem_root;
}

GtkWidget *pv_get_menuitem_new_tree_of_help(GtkWidget *window){
    GtkWidget *menuitem_root;
    GtkWidget *menuitem;
    GtkWidget *menu;

    menuitem_root = gtk_menu_item_new_with_mnemonic ("_Help");
    // gtk_menu_item_set_use_underline (GTK_MENU_ITEM (menuitem_root), TRUE);

    menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_root), menu);

    // ** Issue: Mnemonic not works on submenu in Ubuntu15.10(cause Unity/Ubuntu?).
    menuitem = gtk_menu_item_new_with_mnemonic ("_About");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

    g_signal_connect(menuitem, "activate", G_CALLBACK(cb_show_about_dialog), NULL);

    // ** Accel to "Help > About (Ctrl+A)"
    GtkAccelGroup *accel_group;
    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group,
                            GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    return menuitem_root;
}

// ************** cb_kicked ************************************
void cb_kicked (GtkWidget *button, GtkWidget *menubar)
{
    // Todo: Append new menu item.
    g_print("kicked.\n");
}

// ************** do_menus ************************************
GtkWidget *do_menus()
{
    GtkWidget *window = NULL;
    GtkWidget *box;
    GtkWidget *box2;
    GtkWidget *button;

    GtkWidget *menubar;
    GtkWidget *menuitem;
    GtkAccelGroup *accel_group;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Menus");
    gtk_widget_set_size_request (window, 300,200);
    gtk_container_set_border_width (GTK_CONTAINER (window), 2);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (window), box);

    menubar = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (box), menubar, FALSE, TRUE, 0);

    menuitem = pv_get_menuitem_new_tree_of_file();
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);

    menuitem = pv_get_menuitem_new_tree_of_help(window);
    gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);

 //   button = gtk_button_new_with_label ("kick");          // 一応、残しておく
 //   g_signal_connect (button, "clicked",
 //       G_CALLBACK (cb_kicked), menubar);
 //   gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 0);

	da = gtk_drawing_area_new();                            // 描画エリア
	gtk_widget_set_size_request( da, width ,height);
    gtk_box_pack_start( GTK_BOX (box), da , FALSE,TRUE , 0);


return window;
}


// ************** gtkInit ************************************
// GTKの初期化、終了時のシグナル、再描画のシグナル、タイマーイベントの設定などをして、全てのウィジットを表示する。
// この関数を抜けたあと、エミュレータの初期化をしてから、次の関数でGTKのメインループを実行する
//
int gtkInit(int argc, char *argv[])
{
  GtkWidget *window;

  gtk_init(&argc, &argv);
  window = do_menus();
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // create frame
  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_container_add( GTK_CONTAINER(window) ,frame);


  // event
  g_signal_connect(da, "draw",G_CALLBACK(draw_cb), NULL);			// re-draw event
  g_signal_connect(da, "configure-event",G_CALLBACK(init_drawing_area),NULL); // change size event
   
  g_timeout_add(1000/60, (GSourceFunc)timer_func, NULL);			// timer event （Z80 の実行や画面の描画など）
   	
  gtk_widget_show_all(window);

  return 0;
}

//***************** GTKのメインループ *********************************
void OSD_main_loop(void)
{
	  gtk_main();
}

// ======================================================================================

ColTyp BPal[16],BPal11[4],BPal12[8],BPal13[8],BPal14[4],BPal15[8],BPal53[32];

/** MIT Shared Memory Extension for X ************************/
#ifdef MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
XShmSegmentInfo SHMInfo;
int UseSHM=1;
/** The following is used for MITSHM-auto-detection: **/
int ShmOpcode;       /* major opcode for MIT-SHM */
int (*OldHandler)();
#endif


/** Sound-related definitions ********************************/
#ifdef SOUND
int InitSound(void);
void TrashSound(void);
void FlushSound(void);
void StopSound(void);
void ResumeSound(void);
extern int UseSound;
#endif

/****************************************************************/
/*** These dummy functions are called when writes to sound    ***/
/*** registers occur. Replace them with the actual functions  ***/
/*** generating sound.                                        ***/
/****************************************************************/
#ifndef SOUND
void PSGOut(register byte R,register byte V)  { }
void SCCOut(register byte R,register byte V)  { }
#endif

char *Title="iP6 Unix/X 0.6.4";
static char TitleBuf[256];

/** Variables to pass options to the X-drivers ***************/
int SaveCPU=1;
//int do_install, do_sync;
int scale = 2;


/** Various X-related variables ******************************/
//char *Dispname=NULL;
//byte *XBuf;

int bitpix;
Bool lsbfirst;
byte Xtab[4];

int Mapped; /* flags */

void InitColor(int);
/*void TrashColor(void);*/

/* size of window */
Bool wide,high;
int Width,Height; /* this is ximage size. window size is +20 */

/** Various variables and short functions ********************/

byte *Keys;
byte Keys1[256][2],Keys2[256][2],Keys3[256][2],Keys4[256][2],
  Keys5[256][2],Keys6[256][2],Keys7[256][2];
int scr4col = 0;
static long psec=0, pusec=0;


void LockSurface(void)
{
  //  SDL_LockSurface( surface);
}

void UnlockSurface(void)
{
  //  SDL_UnlockSurface( surface);
}


void OnBreak(int Arg) { CPURunning=0; }

void unixIdle(void)
{
}

/** Screen Mode Handlers [N60/N66][SCREEN MODE] **************/
void (*SCR[2][4])() = 
{
  { RefreshScr10, RefreshScr10, RefreshScr10, RefreshScr10 },
  { RefreshScr51, RefreshScr51, RefreshScr53, RefreshScr54 },
};

/** Keyboard bindings ****************************************/
#include "Keydef.h"

void TrashMachine(void)
{
}

/** InitMachine **********************************************/
/** Allocate resources needed by Unix/X-dependent code.     **/
/*************************************************************/
int InitMachine(void)
{
  int K,L;
  int depth=24 , screen_num;
  //Window root;
  
  SetValidLine( 192);
  SetClock(4*1000000);
  
#if 0
  signal(SIGINT,OnBreak);
  signal(SIGHUP,OnBreak);
  signal(SIGQUIT,OnBreak);
  signal(SIGTERM,OnBreak);
#endif

#ifdef SOUND
  if(UseSound) InitSound();
#endif // SOUND
  Width=M5WIDTH*scale;
  Height=M5HEIGHT*scale;
  
  lsbfirst =0;
  bitpix= 24;
  choosefuncs(lsbfirst,bitpix);
  /*setwidth(wide);*/
  setwidth(scale-1);

  InitColor(screen_num); 		// 色の初期化
#if 0
    SDL_LockSurface( surface);
    memset( surface->pixels , 0 , surface->h *surface->pitch);
    SDL_UnlockSurface( surface);
#endif
  if(Verbose & 1) printf("OK\n");

  return(1);
}






/** Keyboard *************************************************/
/** Check for keyboard events, parse them, and modify P6    **/
/** keyboard matrix.                                        **/
/*************************************************************/
void Keyboard(void)
{
#if 0             // キーボードを使えるようにする時のために、残しておく
  SDL_Event e;
   
  //printf("keyboard ");
  	
#if 1
  int J;

#ifdef SOUND
  //FlushSound();  /* Flush sound stream on each interrupt */
#endif
//  if(XCheckWindowEvent(Dsp,Wnd,KeyPressMask|KeyReleaseMask,&E))
   //printf("keyup %c ", e.key.keysym.sym);

  int ret= SDL_PollEvent( &e);
  if( ret==0) return;
  //printf("keyboard %04X \n" , e.type );
  
 // if( e.type == SDL_QUIT) exit(0);
  
  if( e.type == SDL_KEYDOWN ||  e.type == SDL_KEYUP)
  {
    //J=XLookupKeysym((XKeyEvent *)&E,0);
    J= e.key.keysym.sym;
   // printf("%02X \n",J);
    /* for stick,strig */
    {
      byte tmp;
 /*     switch(J) {
        case XK_space  : tmp = STICK0_SPACE; break;
	case XK_Left   : tmp = STICK0_LEFT; break;
	case XK_Right  : tmp = STICK0_RIGHT; break;
	case XK_Down   : tmp = STICK0_DOWN; break;
	case XK_Up     : tmp = STICK0_UP; break;
	case XK_Pause  : tmp = STICK0_STOP; break;
	case XK_Shift_L: 
	case XK_Shift_R: tmp = STICK0_SHIFT; break;
	default: tmp = 0;
      }
      if(E.type==KeyPress) stick0 |= tmp;
      else	           stick0 &= ~tmp;
    } */
    /* end of for stick,strig */
    //if(E.type==KeyPress)
    if( e.type == SDL_KEYDOWN)
    {
/*      switch(J)
      {
        case XK_F10:
#ifdef SOUND
	  StopSound(); 
#endif
	  run_conf();
	  ClearScr();
#ifdef SOUND
	  ResumeSound();
#endif
	  break;
#ifdef DEBUG
        case XK_F11: Trace=!Trace;break;
#endif
        case XK_F12: CPURunning=0;break;

        case XK_Control_L: kbFlagCtrl=1;break;
        case XK_Alt_L: kbFlagGraph=1;break;

        case XK_Insert: J=XK_F13;break; // Ins -> F13 
      }
      if((P6Version==0)&&(J==0xFFC6)) J=0; // MODE key when 60 
    */
      J&=0xFF;
      if (kbFlagGraph)
	Keys = Keys7[J];
      else if (kanaMode)
	if (katakana)
	  if (stick0 & STICK0_SHIFT) Keys = Keys6[J];
	  else Keys = Keys5[J];
	else
	  if (stick0 & STICK0_SHIFT) Keys = Keys4[J];
	  else Keys = Keys3[J];
      else
	if (stick0 & STICK0_SHIFT) Keys = Keys2[J];
	else Keys = Keys1[J];
      keyGFlag = Keys[0]; p6key = Keys[1];

      /* control key + alphabet key */
    //  if ((kbFlagCtrl == 1) && (J >= XK_a) && (J <= XK_z))
//	{keyGFlag = 0; p6key = J - XK_a + 1;}
      /*if (p6key != 0x00) IFlag = 1;*/
      if (Keys[1] != 0x00) KeyIntFlag = INTFLAG_REQ;
    } else {
  //    if (J==XK_Alt_L) kbFlagGraph=0;
  //    if (J==XK_Control_L) kbFlagCtrl=0;
    }
  }

/*  for(J=0;XCheckWindowEvent(Dsp,Wnd,FocusChangeMask,&E);)
    J=(E.type==FocusOut); 
  if(SaveCPU&&J)
  {
#ifdef SOUND
    StopSound();          // Silence the sound  
#endif

    while(!XCheckWindowEvent(Dsp,Wnd,FocusChangeMask,&E)&&CPURunning)
    {
      if(XCheckWindowEvent(Dsp,Wnd,ExposureMask,&E)) PutImage();
      XPeekEvent(Dsp,&E);
    }

#ifdef SOUND
    ResumeSound();        // Switch the sound back on
#endif
*/
  }
#endif
#endif
}


/* Below are the routines related to allocating and deallocating
   colors */


/* set up coltable, alind8? etc. */
void InitColor(int screen_num)
{
  OSD_Color colors[32];
  XID col;
//  XColor Color;
  register byte i,j;
  register word R,G,B,H;
  int param[16][3] = /* {R,G,B} */
    { {0,0,0},{4,3,0},{0,4,3},{3,4,0},{3,0,4},{4,0,3},{0,3,4},{3,3,3},
      {0,0,0},{4,0,0},{0,4,0},{4,4,0},{0,0,4},{4,0,4},{0,4,4},{4,4,4} };

  unsigned short trans[] = { 0x0000, 0x3fff, 0x7fff, 0xafff, 0xffff };
  //unsigned short trans[] = { 0x00, 0x3f, 0x7f, 0xaf, 0xff };
  int Pal11[ 4] = { 15, 8,10, 8 };
  int Pal12[ 8] = { 10,11,12, 9,15,14,13, 1 };
  int Pal13[ 8] = { 10,11,12, 9,15,14,13, 1 };
  int Pal14[ 4] = {  8,10, 8,15 };
  int Pal15[ 8] = {  8,13,11,10, 8,13,10,15 };
  int Pal53[32] = {  0, 4, 1, 5, 2, 6, 3, 7, 8,12, 9,13,10,14,11,15,
		    10,11,12, 9,15,14,13, 1,10,11,12, 9,15,14,13, 1 };

 for(int i=0; i<4; i++)
    Xtab[i] = i;

  for(H=0;H<2;H++)
    for(B=0;B<2;B++)
      for(G=0;G<2;G++)
        for(R=0;R<2;R++)
        {
          i=R|(G<<1)|(B<<2)|(H<<3);
          
          colors[i].r    = trans[param[i][0]]/256;
          colors[i].g   = trans[param[i][1]]/256;
          colors[i].b   = trans[param[i][2]]/256;
          //col=XAllocColor(Dsp,CMap,&Color)? Color.pixel:black;
	  if (bitpix <= 8) 
	    {
	    //BPal[i].ct_byte[0]=col;
	    BPal[i].ct_byte[0]= i;
	    }
	  else 
	    {
	     BPal[i].ct_byte[0] = colors[i].b;
	     BPal[i].ct_byte[1] = colors[i].g;   
	     BPal[i].ct_byte[2] = colors[i].r;   
/*	    for(j=0; j<4; j++) {
	        BPal[i].ct_byte[j]=((ColTyp)col).ct_byte[Xtab[j]];
	        }*/
        }
    }
  // SDL_SetPaletteColors( &palet ,colors ,0,16);		// 8ビットカラーの場合は、パレット設定が必要？
   
  /* setting color list for each screen mode */
  for(i=0;i<4;i++) BPal11[i] = BPal[ Pal11[i] ];
  for(i=0;i<8;i++) BPal12[i] = BPal[ Pal12[i] ];
  for(i=0;i<8;i++) BPal13[i] = BPal[ Pal13[i] ];
  for(i=0;i<4;i++) BPal14[i] = BPal[ Pal14[i] ];
  for(i=0;i<8;i++) BPal15[i] = BPal[ Pal15[i] ];
  for(i=0;i<32;i++) BPal53[i] = BPal[ Pal53[i]];
}

/** ClearScr: clear a window *********************************/
void ClearScr()
{
 #if 0
 memset(XBuf,(byte)black,Width*Height*bitpix/8);
 #endif
}

void SetTitle(fpos_t pos)
{
#if 0
  sprintf(TitleBuf, "%s  [%ldcnt.]", Title, pos);
  SetWMHints();
#endif
}

/** PUTIMAGE: macro copying image buffer into a window ********/
void PutImage(void)
{
}


