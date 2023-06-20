# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project

#dvd_a_divx.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/dvd_a_divx.pot

KDE_ICON = dvd_a_divx

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = dvd_a_divx

# the application source, library search path, and link libraries
dvd_a_divx_SOURCES = main.cpp pantallainicial.ui pantallainicialimpl.cpp
dvd_a_divx_LDFLAGS = $(KDE_RPATH) $(all_libraries)
dvd_a_divx_LDADD = $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = dvd_a_divx.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/dvd_a_divx
shellrc_DATA = dvd_a_divxui.rc

