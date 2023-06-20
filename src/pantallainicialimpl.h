/***************************************************************************
                          pantallainicialimpl.h  -  description
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

#ifndef PANTALLAINICIALIMPL_H
#define PANTALLAINICIALIMPL_H

#include <qwidget.h>
#include <pantallainicial.h>
#include <qmenubar.h>
#include <qpopupmenu.h>


/**
  *@author César Hernández Bañó
  */

class pantallainicialimpl : public pantallainicial  {
   Q_OBJECT
public: 
	pantallainicialimpl(QWidget *parent=0, const char *name=0);
	~pantallainicialimpl();
	void pon_nombre_fichero_avi(void);
	void pon_argumentos_idioma(char **arg,int *i);
	void pon_bitrate_audio(void);

	char fichero_avi[300];
	int bitrate_audio;
	char slang[10],alang[10];

	//Menus
	QMenuBar *menu;
	QPopupMenu *popup_file;
	QPopupMenu *popup_ver;
	QPopupMenu *popup_ayuda;


public slots:

	void evento_tiempo();

	virtual void slot_elegir_directorio();

	virtual void slot_desactiva_divx4();
	virtual void slot_desactiva_lavc();

	virtual void slot_pon_duracion_90();
	virtual void slot_pon_duracion_100();
	virtual void slot_pon_duracion_110();
	virtual void slot_pon_duracion_120();

	virtual void slot_pon_alto();
	virtual void slot_pon_bitrate_y_ancho();

	virtual void slot_ver_dvd();
	virtual void slot_crear_divx();
	virtual void slot_previsualizar_divx();
	virtual void slot_detectar_recorte();
	virtual void slot_acerca_de();
	
    virtual void slot_pon_alang_en();
    virtual void slot_pon_alang_es();
    virtual void slot_pon_slang_en();
    virtual void slot_pon_slang_es();
    virtual void slot_pon_slang_ninguno();

    virtual void slot_pon_bitrate_audio_96();
    virtual void slot_pon_bitrate_audio_128();
	
	//virtual void slot_tamanyo_archivo();


};

#endif
