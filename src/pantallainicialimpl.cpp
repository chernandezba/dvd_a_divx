/***************************************************************************
                          pantallainicialimpl.cpp  -  description
                             -------------------
    begin                : Wed Jul 10 2002
    copyright            : (C) 2002 by César Hernández Bañó
    email                : chernandezba@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qradiobutton.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qlcdnumber.h>
#include <qstring.h>
#include <qprogressbar.h>
#include <qspinbox.h>
#include <qfiledialog.h>
#include <qcheckbox.h>


#include <stdio.h>  //sprintf
#include <unistd.h> //fork,execv
#include <sys/wait.h> //wait
#include <sys/stat.h> //stat



#include "pantallainicialimpl.h"

//#define PROGRAMA_REPRODUCTOR "/usr/bin/xterm_divxplayer"
#define PROGRAMA_REPRODUCTOR "xterm_divxplayer"
//#define PROGRAMA_COMPRESOR "/usr/bin/xterm_divxencoder"
#define PROGRAMA_COMPRESOR "xterm_divxencoder"

#define MPLAYER_VERSION 1

#ifdef MPLAYER_VERSION
	#define VIDEO_FILTER "-vf"
#else
//For versions prior to 1
	#define VIDEO_FILTER "-vop"
#endif

//For versions from 1
//#define VIDEO_FILTER "-vf"

pantallainicialimpl::pantallainicialimpl(QWidget *parent, const char *name ) : pantallainicial(parent,name) {

	QTimer *internalTimer = new QTimer( this ); // create internal timer
	connect( internalTimer, SIGNAL(timeout()), SLOT(evento_tiempo()) );
	internalTimer->start( 5000 ); //Cada 5 segundos
	evento_tiempo(); //Poner contadores iniciales
	slot_pon_bitrate_y_ancho();
	
	menu=new QMenuBar(this,"menu");
	
	popup_file=new QPopupMenu(this,"popup_file");
	popup_file->insertItem(tr("DIVX &Folder..."),this,SLOT(slot_elegir_directorio()),CTRL+Key_I);
	popup_file->insertSeparator();
	popup_file->insertItem( tr("&Exit"),  this, SLOT(reject()), CTRL+Key_S );
	menu->insertItem( tr("&File"), popup_file );

	popup_ver=new QPopupMenu(this,"popup_ver");
	popup_ver->insertItem( "&DVD",  this, SLOT(slot_ver_dvd()), CTRL+Key_D );
	popup_ver->insertItem( tr("Preview DIV&X"),  this, SLOT(slot_previsualizar_divx()), CTRL+Key_X );	
	menu->insertItem( tr("&View"), popup_ver );
	
	popup_ayuda=new QPopupMenu(this,"popup_ayuda");
	popup_ayuda->insertItem( tr("&About ..."),  this, SLOT(slot_acerca_de()), CTRL+Key_A );	
	menu->insertItem( tr("&Help"), popup_ayuda );
	
		
	menu->show();


}

void pantallainicialimpl::slot_elegir_directorio()
{    QString f;
	
	f=QFileDialog::getExistingDirectory(LineEdit_ruta->text(),this,"Elegir","Choose Folder",FALSE);

	if ( !f.isEmpty() ) {
	// the user selected a valid existing file
	     LineEdit_ruta->setText(f);
    } else {
        // the user cancelled the dialog
    }


}

//Pone la variable bitrate_audio con el valor leido
//de la ventana
void pantallainicialimpl::pon_bitrate_audio(void)
{

	int br_audio;
	bool ok;

	br_audio=(LineEdit_bitrate_audio->text()).toInt(&ok,10);

	if (ok==true && br_audio>0) {
		bitrate_audio=br_audio;
   }
   else
   {
   	bitrate_audio=96;
   }

//   printf ("\nbitrate_audio: %d\n",bitrate_audio);


}

//Pone argumentos de idioma de audio i subtitulos
//Entrada: array de argumentos y indice actual al mismo
//Salida: array modificado y indice apuntando a siguiente elemento
void pantallainicialimpl::pon_argumentos_idioma(char **arg,int *n)
{

	int i;
	
	i=*n;	

	arg[i++]="-alang";
	
	strcpy( alang,(LineEdit_alang->text() ).latin1());
	
	arg[i++]=alang;
	
	if (!RadioButton_slang_ninguno->isChecked() ) {
	
	
		arg[i++]="-slang";
	
		strcpy( slang,(LineEdit_slang->text() ).latin1());
	
		arg[i++]=slang;
	}
	
	
	*n=i;


}

void pantallainicialimpl::slot_pon_alang_en()
{
	LineEdit_alang->setText("en");
}
void pantallainicialimpl::slot_pon_alang_es()
{
	LineEdit_alang->setText("es");
}
void pantallainicialimpl::slot_pon_slang_en()
{
   LineEdit_slang->setEnabled(true);	
	LineEdit_slang->setText("en");
}
void pantallainicialimpl::slot_pon_slang_es()
{
   LineEdit_slang->setEnabled(true);	
	LineEdit_slang->setText("es");
}
void pantallainicialimpl::slot_pon_slang_ninguno()
{
   LineEdit_slang->setEnabled(false);
}

void pantallainicialimpl::slot_pon_bitrate_audio_96()
{
	LineEdit_bitrate_audio->setText("96");
}
void pantallainicialimpl::slot_pon_bitrate_audio_128()
{
	LineEdit_bitrate_audio->setText("128");
}



void pantallainicialimpl::pon_nombre_fichero_avi(void)
{

	char buff[200],buff2[100];

	strcpy( buff,(LineEdit_ruta->text() ).latin1());
	strcpy (buff2,(LineEdit_pelicula->text() ).latin1());
	sprintf (fichero_avi,"%s%s_by_CHB.avi",buff,buff2);

}


void pantallainicialimpl::evento_tiempo()
{

	struct stat buf;
	char cadena_tamanyo[80];

	int retorno;
	int tamanyo;
	int tamanyo_total;
	bool ok;
	int progreso;

	retorno=stat(fichero_avi,&buf);
	if (retorno)
		tamanyo=-1;
	else
		tamanyo=buf.st_size>>20;

   if (tamanyo>=0) {
   	LCDNumber1->display(tamanyo);
 		tamanyo_total=(LineEdit_tamanyo_archivo->text()).toInt(&ok,10);

		if (ok==true && tamanyo_total) {
         progreso=(tamanyo*100)/tamanyo_total;
         if (progreso>100) progreso=100;
         //printf ("%d\n",progreso);
	   	ProgressBar1->setProgress(progreso);
	   }
	}
   else {
   	LCDNumber1->display(QString("---"));
   	ProgressBar1->setProgress(0);  	
   }
}


pantallainicialimpl::~pantallainicialimpl(){
}


void pantallainicialimpl::slot_desactiva_divx4()
{
	RadioButton_divx4->setChecked(false);
}

void pantallainicialimpl::slot_desactiva_lavc()
{
	RadioButton_msmpeg4->setChecked(false);
	RadioButton_mpeg4->setChecked(false);
}

void pantallainicialimpl::slot_pon_duracion_90()
{
	LineEdit_duracion->setText("90");
}

void pantallainicialimpl::slot_pon_duracion_100()
{
	LineEdit_duracion->setText("100");
}

void pantallainicialimpl::slot_pon_duracion_110()
{
	LineEdit_duracion->setText("110");
}

void pantallainicialimpl::slot_pon_duracion_120()
{
	LineEdit_duracion->setText("120");
}

void pantallainicialimpl::slot_pon_alto()
{
	float aspecto;
	int ancho;
	int alto;
	char char_alto[8];

	bool ok;

	aspecto=(LineEdit_aspecto->text()).toFloat(&ok);
	if (ok==true && aspecto>0) {
		ancho=(LineEdit_ancho->text()).toInt(&ok,10);
      if (ok==true && ancho>0) {
			alto=ancho/aspecto;
			sprintf(char_alto,"%d",alto);

			LineEdit_alto->setText(char_alto);
		}
	}

	//como se ha variado el alto (debido a un cambio en el ancho
	//o en el aspecto) desactivar recorte

	CheckBox_recorte->setChecked(false);

}

//Actualiza tambien el valor de bitrate_audio
//Llamando a la funcion correspondiente
//A su vez, el campo de cambio de bitrate de audio
//genera una señal a pon_bitrate_video_y_ancho,
//para que vuelva a leer el bitrate de audio y
//calcular el bitrate de video
void pantallainicialimpl::slot_pon_bitrate_y_ancho()

{
	int duracion;
	int bitrate_video;
	char char_bitrate_video[8];
	int tamanyo_sonido;
	long long tamanyo_archivo;

	int ancho;
	char char_ancho[80];

	bool ok;

	duracion=(LineEdit_duracion->text()).toInt(&ok,10);
	if (ok==true && duracion>0) {

		//Se calcula el bitrate de tal manera que ocupe
		//el video y el sonido el tamaño indicado

		//El bitrate indicado por mencoder es kilobits por segundo

 		tamanyo_archivo=(LineEdit_tamanyo_archivo->text()).toInt(&ok,10);
		tamanyo_archivo<<=20;

		if (ok==true && tamanyo_archivo>0) {
			pon_bitrate_audio();

			tamanyo_sonido=(bitrate_audio/8)*duracion*60;
			tamanyo_sonido <<=10;

        	//Si el video ocupa menos que el audio (caso remoto):
			if (tamanyo_sonido>tamanyo_archivo) bitrate_video=100; //100???
			else {

				bitrate_video=(duracion*60); //segundos
				bitrate_video=((tamanyo_archivo-tamanyo_sonido)/bitrate_video)*8;
				bitrate_video >>=10;
			}

		

	      if (bitrate_video>0) {
				sprintf(char_bitrate_video,"%d",bitrate_video);

				LineEdit_bitrate_video->setText(char_bitrate_video);
			}



		}

	}

}

void pantallainicialimpl::slot_ver_dvd()
{

	char *argumentos_ver_dvd[20];
	char *programa=PROGRAMA_REPRODUCTOR;
	int pid,status;
	int titulo;
	char char_titulo[10];
	bool ok;

	int arg=0;

	argumentos_ver_dvd[arg++]=PROGRAMA_REPRODUCTOR;

//	argumentos_ver_dvd[arg++]="-dvd";

	titulo=(LineEdit_titulo->text()).toInt(&ok,10);
	//controlar_ok

#ifdef MPLAYER_VERSION
sprintf (char_titulo,"dvd://%d",titulo);
argumentos_ver_dvd[arg++]=char_titulo;
#else
argumentos_ver_dvd[arg++]="-dvd";
sprintf (char_titulo,"%d",titulo);
argumentos_ver_dvd[arg++]=char_titulo;
#endif

	
	pon_argumentos_idioma(argumentos_ver_dvd,&arg);
	
	argumentos_ver_dvd[arg]=NULL;

	switch (pid=fork()) {
		case -1:
			printf ("Error en fork!");
		break;

		case 0:

			execvp(programa,argumentos_ver_dvd);
			printf ("Error al ejecutar %s",programa);

		break;

		default:

			//wait(&status);
			//printf ("\nFinal programa %s\n",programa);

		break;

	}



}

void pantallainicialimpl::slot_detectar_recorte()
{

	char *argumentos_ver_dvd[20];
	char *programa=PROGRAMA_REPRODUCTOR;
	int pid,status;
	int titulo;
	char char_titulo[10];
	char char_video_filter[30];
	bool ok;

	int arg=0;
	int ancho,alto;

	argumentos_ver_dvd[arg++]=PROGRAMA_REPRODUCTOR;

	argumentos_ver_dvd[arg++]=VIDEO_FILTER; //"-vf";

	ancho=(LineEdit_ancho->text()).toInt(&ok,10);
	alto=(LineEdit_alto->text()).toInt(&ok,10);

	sprintf(char_video_filter,"scale=%d:%d,cropdetect",ancho,alto);
	argumentos_ver_dvd[arg++]=char_video_filter;


	titulo=(LineEdit_titulo->text()).toInt(&ok,10);
	//controlar_ok

#ifdef MPLAYER_VERSION
sprintf (char_titulo,"dvd://%d",titulo);
argumentos_ver_dvd[arg++]=char_titulo;
#else
argumentos_ver_dvd[arg++]="-dvd";
sprintf (char_titulo,"%d",titulo);
argumentos_ver_dvd[arg++]=char_titulo;
#endif

	pon_argumentos_idioma(argumentos_ver_dvd,&arg);

	argumentos_ver_dvd[arg]=NULL;

	switch (pid=fork()) {
		case -1:
			printf ("Error en fork!");
		break;

		case 0:

			execvp(programa,argumentos_ver_dvd);
			printf ("Error al ejecutar %s",programa);

		break;

		default:

			//wait(&status);
			//printf ("\nFinal programa %s\n",programa);

		break;

	}



}


void pantallainicialimpl::slot_crear_divx()
{

	char *argumentos_crear_divx[20];
	char *programa=PROGRAMA_COMPRESOR;
	int pid,status;
	int titulo;
	char char_titulo[10];
	bool ok;
	int ancho,alto;
	//char char_scale[20];
	char char_crop[20];
	int bitrate_video;

	char char_divx4opts[30];
	char char_lavcopts[30];
	char char_lameopts[30];
	char char_video_filter[50];
	float volumen;
	

	int arg=0;

	argumentos_crear_divx[arg++]=PROGRAMA_REPRODUCTOR;

//	argumentos_crear_divx[arg++]="-dvd";

	titulo=(LineEdit_titulo->text()).toInt(&ok,10);
	//controlar_ok

#ifdef MPLAYER_VERSION
sprintf (char_titulo,"dvd://%d",titulo);
argumentos_crear_divx[arg++]=char_titulo;
#else
argumentos_crear_divx[arg++]="-dvd";
sprintf (char_titulo,"%d",titulo);
argumentos_crear_divx[arg++]=char_titulo;
#endif

	pon_argumentos_idioma(argumentos_crear_divx,&arg);

//	argumentos_crear_divx[arg++]="-alang";
//	argumentos_crear_divx[arg++]="es";

	argumentos_crear_divx[arg++]="-oac";
	argumentos_crear_divx[arg++]="mp3lame";

	

	argumentos_crear_divx[arg++]="-lameopts";
	
	volumen=SpinBox_volumen->value();
	volumen /=100;	
	
	sprintf(char_lameopts,"cbr:br=%d:vol=%1.02f",bitrate_audio,volumen);
	
	argumentos_crear_divx[arg++]=char_lameopts;


	bitrate_video=(LineEdit_bitrate_video->text()).toInt(&ok,10);
	
	if (RadioButton_divx4->isChecked()==true) {
		argumentos_crear_divx[arg++]="-ovc";		
		argumentos_crear_divx[arg++]="divx4";
		argumentos_crear_divx[arg++]="-divx4opts";
		sprintf(char_divx4opts,"br=%d",bitrate_video);
		argumentos_crear_divx[arg++]=char_divx4opts;
	}
	else
	if (RadioButton_msmpeg4->isChecked()==true) {
		argumentos_crear_divx[arg++]="-ovc";		
		argumentos_crear_divx[arg++]="lavc";
		argumentos_crear_divx[arg++]="-lavcopts";
		sprintf(char_lavcopts,"vcodec=msmpeg4:vbitrate=%d",bitrate_video);
		argumentos_crear_divx[arg++]=char_lavcopts;
	}
	else {
	//if (RadioButton_divx4->isChecked==true) {
		argumentos_crear_divx[arg++]="-ovc";		
		argumentos_crear_divx[arg++]="lavc";
		argumentos_crear_divx[arg++]="-lavcopts";
		sprintf(char_lavcopts,"vcodec=mpeg4:vbitrate=%d",bitrate_video);
		argumentos_crear_divx[arg++]=char_lavcopts;
	}

	argumentos_crear_divx[arg++]=VIDEO_FILTER; //"-vf";
	if (CheckBox_recorte->isChecked()==true) {
		
		sprintf (char_crop,",crop=%s",(LineEdit_recorte->text() ).latin1());
		//argumentos_crear_divx[arg++]=char_crop;
	}

	else char_crop[0]=0;

	//argumentos_crear_divx[arg++]="-vop";

	ancho=(LineEdit_ancho->text()).toInt(&ok,10);
	alto=(LineEdit_alto->text()).toInt(&ok,10);

	sprintf(char_video_filter,"scale=%d:%d%s",ancho,alto,char_crop);
	argumentos_crear_divx[arg++]=char_video_filter;


	argumentos_crear_divx[arg++]="-o";
	pon_nombre_fichero_avi();
	argumentos_crear_divx[arg++]=fichero_avi;
	argumentos_crear_divx[arg]=NULL;

	switch (pid=fork()) {
		case -1:
			printf ("Error en fork!");
		break;

		case 0:

			execvp(programa,argumentos_crear_divx);
			printf ("Error al ejecutar %s",programa);

		break;

		default:

			//wait(&status);
			//printf ("\nFinal programa %s\n",programa);

		break;

	}



}

void pantallainicialimpl::slot_previsualizar_divx()
{

	char *argumentos_ver_divx[20];
	char *programa=PROGRAMA_REPRODUCTOR;
	int pid,status;
	int titulo;
	char char_titulo[10];
	bool ok;

	int arg=0;

	argumentos_ver_divx[arg++]=PROGRAMA_REPRODUCTOR;


	argumentos_ver_divx[arg++]="-idx";
	pon_nombre_fichero_avi();
	argumentos_ver_divx[arg++]=fichero_avi;

	argumentos_ver_divx[arg]=NULL;

	switch (pid=fork()) {
		case -1:
			printf ("Error in fork!");
		break;

		case 0:

			execvp(programa,argumentos_ver_divx);
			
			printf ("Error running viewer program\n");
			//El mensaje no se ve, dado que es un proceso hijo
			//QMessageBox::warning( this, tr("Error"),
			//	tr("Error running viewer program"),QMessageBox::Ok, QMessageBox::NoButton );


		break;

		default:

			//wait(&status);
			//printf ("\nFinal programa %s\n",programa);

		break;

	}



}



void pantallainicialimpl::slot_acerca_de()
{

   QMessageBox::information( this, tr("About..."),
		tr("DVD_A_DIVX V.0.5\n"
		"(c) Cesar Hernandez (08/01/2005)\nchernandezba@gmail.com"
		"\n\n"),QMessageBox::Ok );

}

