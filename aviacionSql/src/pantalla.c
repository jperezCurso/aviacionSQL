/* ****************************************************************************
 * pantalla.c
 *
 *  Created on: 18 may 2024
 *      Author: juan
 *************************************************************************** */
#include "pantalla.h"
#include "basedatos.h"
#include "misutils.h"
#include "ficheros.h"

// ini variables globales ----------------------------------------------------
char texto_a_buscar[60];
// Copia de los widgets que se usan en funciones fuera de la app_activate ----
GtkWidget *lab1_copy;
GtkWidget *but_buscar_copy;
GtkWidget *but_volver_copy;
GtkWidget *scrwin_lispor_copy;
GtkWidget *scrwin_lisrut_copy;
GListStore *store_lisports_copy;
GListStore *store_lisrutas_copy;
GtkWidget *img_port_ori_copy;
GtkWidget *img_port_des_copy;
// Aeropuertos origen y destino y sus posiciones para los iconos en el mapa --
int ori_port;
int ori_x;
int ori_y;
int des_port;
int des_x;
int des_y;

// Estado aplicación:
//		0- inicio(pide aeropuerto)
//		1- muestra lista aeropuertos
//		2- muestra rutas
int estado_app = 0;

// variables globales en otros fuentes ---------------------------------------
extern st_list_aeropu *lista_aeropuertos;
extern st_list_rutas *lista_rutas;
extern st_port_tooltip *tt_port;
extern unsigned long_st_laeropu;
extern unsigned long_st_lrutas;
extern char* laerop_nombres[];
extern char* lrutas_nombres[];
// fin variables globales ----------------------------------------------------

// ini Declaración de funciones ----------------------------------------------
static void app_activate (GtkApplication *app);
static void event_aeropuerto_pulsado( GtkSelectionModel *selmodel, guint posi, gpointer user_data );
static void event_ruta_pulsada( GtkSelectionModel *selmodel, guint posi, gpointer user_data );
static void mover_lisports_a_columnview();
static void mover_lisrutas_a_columnview();
static void create_scrollwin(GtkWidget *, GtkWidget *);
static void colocar_port_origen();
static void colocar_port_destino();
static GtkWidget* obtener_imagen(const char * fichero);
GListModel * create_lisport_model();
GListModel * create_lisrutas_model();
// fin Declaración de funciones ----------------------------------------------


//ini Datos para el columnview de listado_aeropuertos -----------------------------------------------
#define LISPORT_TYPE_ITEM (lisport_item_get_type())
//G_DECLARE_FINAL_TYPE (LisportItem, lisport_item, LISPORT, ITEM, GObject)
G_DECLARE_FINAL_TYPE (LisportItem, lisport_item, LISPORT, ITEM, GtkWidget)

// Estructura listado aeropuertos a mostrar en pantalla
struct _LisportItem {
	GObject parent_instance;
	const char *nombre;
	const char *id;
};

struct _LisportItemClass { GObjectClass parent_class; };

G_DEFINE_TYPE (LisportItem, lisport_item, GTK_TYPE_WIDGET)

static void lisport_item_init(LisportItem *item) { }
static void lisport_item_class_init(LisportItemClass *class) { }

static LisportItem * lisport_item_new(const char *name, const char *id ) {
    LisportItem  *item = g_object_new(LISPORT_TYPE_ITEM, NULL);
    item->nombre = g_strdup(name);
    item->id     = g_strdup(id);
    return item;
}

GListModel * create_lisport_model(void) {
	GListStore * store_lisport = g_list_store_new(G_TYPE_OBJECT);
	store_lisports_copy = store_lisport;
    return G_LIST_MODEL(store_lisport);
}

static const char* lisport_item_get_nombre(LisportItem *item) { return item->nombre; }
static const char* lisport_item_get_id(LisportItem *item) { return item->id; }

static void setup_cb(GtkSignalListItemFactory *factory, GObject *listitem) {
    GtkWidget *label = gtk_label_new(NULL);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), label);
}

static void bind_nombre_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    GObject *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    const char *string = lisport_item_get_nombre(LISPORT_ITEM(item));
    gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_id_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    GObject *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    const char *string = lisport_item_get_id(LISPORT_ITEM(item));
    gtk_label_set_text(GTK_LABEL (label), string);
}
//fin Datos para el columnview de listado_aeropuertos -------------------------------------------

//ini Datos para el columnview de listado_rutas -------------------------------------------------
#define LISRUTAS_TYPE_ITEM (lisrutas_item_get_type())
G_DECLARE_FINAL_TYPE (LisrutasItem, lisrutas_item, LISRUTAS, ITEM, GObject)

// Estructura listado rutas a mostrar en pantalla
struct _LisrutasItem {
	GObject parent_instance;
	const char *iataOri;	// 0
	const char *iataDes;	// 1
	const char *aerolin;	// 2
	const char *distanc;	// 9
	const char *notas;	  	// 4 codeshare
	const char *vehicul; 	// 3 equipment
};

struct _LisrutasItemClass { GObjectClass parent_class; };

G_DEFINE_TYPE (LisrutasItem, lisrutas_item, G_TYPE_OBJECT)

static void lisrutas_item_init(LisrutasItem *item) { }
static void lisrutas_item_class_init(LisrutasItemClass *class) { }

static LisrutasItem * lisrutas_item_new(const char *iataOri, const char *iataDes, const char *aerolin,
										const char *distanc, const char *notas, const char *vehicul) {
    LisrutasItem  *item = g_object_new(LISRUTAS_TYPE_ITEM, NULL);
    item->iataOri = g_strdup(iataOri);
    item->iataDes = g_strdup(iataDes);
    item->aerolin = g_strdup(aerolin);
    item->distanc = g_strdup(distanc);
    item->notas   = g_strdup(notas);
    item->vehicul = g_strdup(vehicul);
    return item;
}

GListModel * create_lisrutas_model(void) {
	GListStore * store_lisrutas = g_list_store_new(G_TYPE_OBJECT);
	store_lisrutas_copy = store_lisrutas;
    return G_LIST_MODEL(store_lisrutas);
}

static const char* lisrutas_item_get_iataOri(LisrutasItem *item) { return item->iataOri; }
static const char* lisrutas_item_get_iataDes(LisrutasItem *item) { return item->iataDes; }
static const char* lisrutas_item_get_aerolin(LisrutasItem *item) { return item->aerolin; }
static const char* lisrutas_item_get_distanc(LisrutasItem *item) { return item->distanc; }
static const char* lisrutas_item_get_notas  (LisrutasItem *item) { return item->notas; }
static const char* lisrutas_item_get_vehicul(LisrutasItem *item) { return item->vehicul; }

static void bind_iataOri_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child( listitem );
    GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( listitem ) );
    const char *string = lisrutas_item_get_iataOri( LISRUTAS_ITEM( item ) );
    gtk_label_set_text( GTK_LABEL( label ), string );
}

static void bind_iataDes_cb( GtkSignalListItemFactory *factory, GtkListItem *listitem ) {
    GtkWidget *label = gtk_list_item_get_child( listitem );
    GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( listitem ) );
    const char *string = lisrutas_item_get_iataDes( LISRUTAS_ITEM( item ) );
    gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_aerolin_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child( listitem );
    GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( listitem ) );
    const char *string = lisrutas_item_get_aerolin( LISRUTAS_ITEM( item ) );
    gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_distanc_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child( listitem );
    GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( listitem ) );
    const char *string = lisrutas_item_get_distanc( LISRUTAS_ITEM( item ) );
    gtk_label_set_text( GTK_LABEL( label ), string );
}

static void bind_notas_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child( listitem );
    GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( listitem ) );
    const char *string = lisrutas_item_get_notas( LISRUTAS_ITEM( item ) );
    gtk_label_set_text( GTK_LABEL( label ), string );
}

static void bind_vehicul_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child( listitem );
    GObject *item = gtk_list_item_get_item( GTK_LIST_ITEM( listitem ) );
    const char *string = lisrutas_item_get_vehicul( LISRUTAS_ITEM( item ) );
    gtk_label_set_text( GTK_LABEL( label ), string );
}
//fin Datos para el columnview de listado_aeropuertos



// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Función inicial llamada desde main.
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int pantalla_inicial(int argc, char *argv[]) {
	GtkApplication *app;
	int status;

	app = gtk_application_new( "org.gtk.example", G_APPLICATION_FLAGS_NONE );
	g_signal_connect( app, "activate", G_CALLBACK (app_activate), NULL );
	status = g_application_run( G_APPLICATION (app), argc, argv );
	g_object_unref( app );

	return status;
}


// ----------------------------------------------------------------------------
// Botón de búsqueda de aeropuertos.
// Debe teclear al menos INPUT_UMBRAL caracteres.
// Recoge el valor del campo indicado por ptr, lo convierte en minúsculas para
//   buscarlo más tarde en los nombres de aeropuertos de la base de datos que se
//   obtendrán en minúsculas también.
// Crea el Widget para almacenar la lista de aeropuertos
// en *btn puede venir el botón Buscar o el entry de búsqueda.
// ----------------------------------------------------------------------------
static void click_buscar_cb (GtkButton *btn, gpointer ptr) {
	const char* s = gtk_editable_get_text( GTK_EDITABLE( ptr ) );

	if (strlen( s ) >= INPUT_UMBRAL) {
		strcpy( texto_a_buscar, s );
		for (int i=0; texto_a_buscar[i]; i++) {
			// Si se pulsó Enter, ese carácter se elimina de la búsqueda
			if (texto_a_buscar[i] == 0x0d || texto_a_buscar[i] == 0x0a) {
				texto_a_buscar[i] = '\0';
				break;
			}
			texto_a_buscar[i] = tolower( texto_a_buscar[i] );
		}

		sql_buscar_aeropuertos( texto_a_buscar );
		mover_lisports_a_columnview();

		gtk_widget_set_sensitive( GTK_WIDGET( ptr ), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET( but_buscar_copy ), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET( scrwin_lispor_copy ), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET( but_volver_copy ), TRUE );
		estado_app = 1;
	}
}

// ----------------------------------------------------------------------------
// Vuele al estado anterior de la app.
// En ptr viene el campo t_buscar
// 0 - inicial, pide aeropuerto a buscar
// 1 - muestra aeropuertos con los datos introducidos
// 2 - muestra las rutas de ese aeropuerto
// ----------------------------------------------------------------------------
static void click_volver_cb (GtkButton *btn, gpointer ptr) {
	if (estado_app == 1) {
		gtk_widget_set_sensitive( GTK_WIDGET( ptr ), TRUE );
		gtk_editable_set_editable( GTK_EDITABLE( ptr ), TRUE );
		gtk_editable_set_text( GTK_EDITABLE( ptr ), "" );
		gtk_widget_grab_focus( GTK_WIDGET( ptr ) );

		gtk_widget_set_sensitive( GTK_WIDGET( scrwin_lispor_copy ), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET( but_buscar_copy ), FALSE );
		gtk_widget_set_sensitive( GTK_WIDGET( btn ), FALSE );
		gtk_widget_set_name( lab1_copy, "red" );
		estado_app = 0;
	}
	else if (estado_app == 2) {
		gtk_widget_set_sensitive( GTK_WIDGET( scrwin_lispor_copy ), TRUE );
		gtk_widget_set_sensitive( GTK_WIDGET( scrwin_lisrut_copy ), FALSE );
		gtk_widget_set_visible( img_port_ori_copy, FALSE );
		gtk_widget_set_visible( img_port_des_copy, FALSE );
		estado_app = 1;

	}
}

// ----------------------------------------------------------------------------
// Botón de finalización de aplicación.
// ----------------------------------------------------------------------------
static void click_salir_cb (GtkWindow *window) {
	estado_app = 0;
	sql_liberar_memoria( FREE_TODO );
	gtk_window_close( window );
}

// ----------------------------------------------------------------------------
// Callback para manejar el evento de pulsación de teclas
// Recibe txt_buscar en user_data y comprueba cuantos caracteres se han escrito.
// Cuando pasa del umbral establecido permite buscar aeropuertos con ese texto.
// ----------------------------------------------------------------------------
static gboolean on_key_released(GtkEventControllerKey *controller, guint keyval,
							    guint keycode, GdkModifierType state, gpointer user_data) {
	const char* s = gtk_editable_get_text( GTK_EDITABLE( user_data ) );

	if (strlen( s ) >= INPUT_UMBRAL) {
		gtk_widget_set_name( lab1_copy, "green" );
		gtk_widget_set_sensitive( GTK_WIDGET( but_buscar_copy ), TRUE );
	}
	else {
		gtk_widget_set_name( lab1_copy, "red" );
		gtk_widget_set_sensitive( GTK_WIDGET( but_buscar_copy ), FALSE );
	}

	if (keyval == GDK_KEY_Return || keyval == GDK_KEY_KP_Enter) {
        return TRUE; // Indica que hemos manejado el evento
    }

    return FALSE; // Permite que otros manejadores manejen el evento
}

// ----------------------------------------------------------------------------
// Evento pulsación en una columna de la lista de aeropuertos
// ----------------------------------------------------------------------------
static void event_aeropuerto_pulsado( GtkSelectionModel *selmodel, guint posi, gpointer user_data ) {
	ori_port = lista_aeropuertos[posi].id;
	sql_buscar_rutas( ori_port );
	mover_lisrutas_a_columnview();
	gtk_widget_set_sensitive( GTK_WIDGET( scrwin_lispor_copy ), FALSE );
	gtk_widget_set_sensitive( GTK_WIDGET( scrwin_lisrut_copy ), TRUE );
	estado_app = 2;
}

// ----------------------------------------------------------------------------
// Evento pulsación en una columna de la lista de rutas
// ----------------------------------------------------------------------------
static void event_ruta_pulsada( GtkSelectionModel *selmodel, guint posi, gpointer user_data ) {
	des_port = lista_rutas[posi].port_des_id;
	util_gps_a_2d( lista_rutas[posi].latDes, lista_rutas[posi].lonDes, MAPA_ANCHO, MAPA_ALTO, &des_x, &des_y);
	util_gps_a_2d( lista_rutas[posi].latOri, lista_rutas[posi].lonOri, MAPA_ANCHO, MAPA_ALTO, &ori_x, &ori_y);

	// Hay que sumar este desplazamiento al resultado de la función util_gps_a_2d()
	ori_x -=  42;
	ori_y += 149;
	des_x -=  42;
	des_y += 149;

	colocar_port_origen();
	colocar_port_destino();

	files_generar_fichero( posi );
	sql_insertar_ruta_elegida( lista_rutas[posi] );
}

// ----------------------------------------------------------------------------
// Se genera la pantalla principal a mostrar y se asignan las funciones a los botones.
// ----------------------------------------------------------------------------
static gboolean app_close (GtkWindow *window, gpointer data) {
	sql_liberar_memoria(FREE_TODO);
	g_list_store_remove_all( store_lisports_copy );
	g_list_store_remove_all( store_lisrutas_copy );
	return FALSE;
}

// ----------------------------------------------------------------------------
// Pone la lista de aeropuertos en el columnView
// ----------------------------------------------------------------------------
static void mover_lisports_a_columnview() {
	char resid[10];

	g_list_store_remove_all( store_lisports_copy );

	for (int ind=0; ind < long_st_laeropu; ind++ ) {
		sprintf( resid, "%d", lista_aeropuertos[ind].id );
	    g_list_store_append( store_lisports_copy, lisport_item_new( lista_aeropuertos[ind].nombre, resid ) );
	}
}

// ----------------------------------------------------------------------------
// Pone la lista de rutas en el columnView
// ----------------------------------------------------------------------------
static void mover_lisrutas_a_columnview() {
	char resid[12];

	g_list_store_remove_all( store_lisrutas_copy );

	for (int ind=0; ind < long_st_lrutas; ind++ ) {
		sprintf(resid, "%10.0lf", lista_rutas[ind].distancia);
	    g_list_store_append( store_lisrutas_copy, lisrutas_item_new(
	    		lista_rutas[ind].iataOri,
	    		lista_rutas[ind].iataDes,
				lista_rutas[ind].aerolin,
				resid,
				lista_rutas[ind].notas,
				lista_rutas[ind].vehicul)
	    );
	}
}

// ----------------------------------------------------------------------------
// Crea el objeto scrollwind que contendrá las listas de aeropuertos y rutas
// ----------------------------------------------------------------------------
static void create_scrollwin(GtkWidget *scrwin, GtkWidget *cv_lis) {
	gtk_scrolled_window_set_min_content_width( GTK_SCROLLED_WINDOW( scrwin ), 300 );
	gtk_scrolled_window_set_min_content_height( GTK_SCROLLED_WINDOW( scrwin ), 100 );
	gtk_scrolled_window_set_max_content_height( GTK_SCROLLED_WINDOW( scrwin ), 100 );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrwin ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_scrolled_window_set_child( (GtkScrolledWindow *)scrwin, cv_lis );
	gtk_scrolled_window_set_propagate_natural_width( (GtkScrolledWindow *)scrwin, TRUE) ;
	gtk_scrolled_window_set_propagate_natural_height( (GtkScrolledWindow *)scrwin, TRUE );
	gtk_scrolled_window_set_has_frame( (GtkScrolledWindow *)scrwin, TRUE );
	gtk_widget_set_halign( scrwin, GTK_ALIGN_CENTER );
	gtk_widget_set_sensitive( GTK_WIDGET( scrwin ), FALSE );
}

// ----------------------------------------------------------------------------
// Se genera la pantalla principal a mostrar y se asignan las funciones a los botones.
// --------------------------------------------------------------BOTON_ANCHO, BOTON_ALTO----------
static void app_activate (GtkApplication *app) {
	GtkWidget *win;
	GtkWidget *box;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *overlay;
	GtkWidget *lab1;
	GtkWidget *but_buscar;
	GtkWidget *but_volver;
	GtkWidget *but_salir;
	GtkWidget *ent_buscar;
	GtkWidget *img_logo;
	GtkWidget *img_mapa;
	GtkWidget *img_port_ori;
	GtkWidget *img_port_des;
	GtkCssProvider *provLab;
	GtkWidget *cv_lisports;
	GtkColumnViewColumn *col_lisports;
	GtkSingleSelection *sel_lisports;
	GListModel *mod_lisports;
	GtkListItemFactory *factory;
	GtkWidget *scrwin_lispor;
	GtkWidget *cv_lisrutas;
	GtkColumnViewColumn *col_lisrutas;
	GtkSingleSelection *sel_lisrutas;
	GListModel *mod_lisrutas;
	GtkWidget *scrwin_lisrut;
// Cada campo del columnview tiene una función bind asignada. Este es para cv_lisports
#define PTR_LP_SIZE 2
	void (*bind_lisports[ PTR_LP_SIZE ])( GtkSignalListItemFactory *, GtkListItem * ) = {
			bind_nombre_cb, bind_id_cb
	};
// Cada campo del columnview tiene una función bind asignada. Este es para cv_lisrutas
#define PTR_LR_SIZE  6
	void (*bind_lisrutas[ PTR_LR_SIZE ])( GtkSignalListItemFactory *, GtkListItem * ) = {
		bind_iataOri_cb, bind_iataDes_cb, bind_aerolin_cb, bind_distanc_cb, bind_notas_cb, bind_vehicul_cb
	};

	win = gtk_application_window_new( app );
	gtk_window_set_title( GTK_WINDOW( win ), "Window" );
	gtk_window_set_default_size( GTK_WINDOW( win ), WIN_ANCHO, WIN_ALTO );
	gtk_window_set_resizable( GTK_WINDOW( win ), FALSE );

	box = gtk_box_new( GTK_ORIENTATION_VERTICAL, BOX_SPACING );
	gtk_widget_set_halign( box, GTK_ALIGN_CENTER );
	gtk_widget_set_valign( box, GTK_ALIGN_START );
	gtk_box_set_homogeneous( GTK_BOX( box ), FALSE );
	gtk_window_set_child( GTK_WINDOW( win ), box );

	box1 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, BOX_SPACING );
	gtk_widget_set_halign( box1, GTK_ALIGN_CENTER );
	gtk_widget_set_valign( box1, GTK_ALIGN_CENTER );
	gtk_box_set_homogeneous( GTK_BOX( box1 ), FALSE );

	box2 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, BOX_SPACING );
	gtk_widget_set_halign( box2, GTK_ALIGN_CENTER );
	gtk_widget_set_valign( box2, GTK_ALIGN_START );
	gtk_box_set_homogeneous( GTK_BOX( box2 ), TRUE );

	overlay = gtk_overlay_new();

	lab1 = gtk_label_new( "Aeropuerto" );
	gtk_widget_set_valign( lab1, GTK_ALIGN_CENTER );
	gtk_widget_set_vexpand( lab1, FALSE );
	gtk_widget_set_size_request( lab1, BOTON_ANCHO, BOTON_ALTO );
	lab1_copy = lab1;

	ent_buscar = gtk_entry_new();
	gtk_widget_set_valign( ent_buscar, GTK_ALIGN_CENTER );
	gtk_widget_set_hexpand( ent_buscar, TRUE );
	gtk_widget_set_size_request( ent_buscar, INPUT_ANCHO, INPUT_ALTO );
	gtk_widget_set_tooltip_text( ent_buscar, "nombre aeropuerto");
	gtk_entry_set_placeholder_text( GTK_ENTRY( ent_buscar ), "nombre aeropuerto" );
//	g_signal_connect( ent_buscar, "activate", G_CALLBACK( click_buscar_cb ), ent_buscar );

	GtkEventController *controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-released", G_CALLBACK( on_key_released ), ent_buscar );
    gtk_widget_add_controller( ent_buscar, controller );

    but_buscar = gtk_button_new_with_label( "Buscar" );
	gtk_widget_set_valign( but_buscar, GTK_ALIGN_CENTER );
	gtk_widget_set_vexpand( but_buscar, FALSE );
	gtk_widget_set_size_request( but_buscar, BOTON_ANCHO, BOTON_ALTO );
	gtk_widget_set_sensitive( GTK_WIDGET( but_buscar ), FALSE );
	g_signal_connect( but_buscar, "clicked", G_CALLBACK( click_buscar_cb ), ent_buscar );
	but_buscar_copy = but_buscar;

	but_volver = gtk_button_new_with_label( "Volver" );
	gtk_widget_set_margin_start( but_volver, BOTON_MARG_START );
	gtk_widget_set_valign( but_volver, GTK_ALIGN_CENTER );
	gtk_widget_set_vexpand( but_volver, FALSE );
	gtk_widget_set_size_request( but_volver, BOTON_ANCHO, BOTON_ALTO );
	gtk_widget_set_sensitive( GTK_WIDGET( but_volver ), FALSE );
	g_signal_connect( but_volver, "clicked", G_CALLBACK( click_volver_cb ), ent_buscar );
	but_volver_copy = but_volver;

	but_salir = gtk_button_new_with_label( "Salir" );
	gtk_widget_set_margin_start( but_salir, BOTON_MARG_START );
	gtk_widget_set_valign( but_salir, GTK_ALIGN_CENTER );
	gtk_widget_set_vexpand( but_salir, FALSE );
	gtk_widget_set_size_request( but_salir, BOTON_ANCHO, BOTON_ALTO );
	g_signal_connect_swapped( but_salir, "clicked", G_CALLBACK( click_salir_cb ), win );

	provLab = gtk_css_provider_new();
	gtk_css_provider_load_from_path( provLab, "./data/label.css" );
	gtk_style_context_add_provider( gtk_widget_get_style_context( lab1 ),
			GTK_STYLE_PROVIDER( provLab ), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	gtk_widget_set_name( lab1, "red" );
	g_object_unref( provLab );

	// obtenemos las imágenes y comprobamos que no se produzca ningún error; en ese
	// caso no se incluyen en la pantalla.
	img_logo = obtener_imagen( "logotipo.png" );
	img_mapa = obtener_imagen( "mapamundi.png" );
	img_port_ori = obtener_imagen( "avionPosOri.png" );
	img_port_des = obtener_imagen( "avionPosDes.png" );
	img_port_ori_copy = img_port_ori;
	img_port_des_copy = img_port_des;

	gtk_widget_set_size_request( img_logo, LOGO_ANCHO, LOGO_ALTO );
	if (img_logo)
		gtk_box_append( GTK_BOX( box1 ), img_logo );

	gtk_box_append( GTK_BOX( box1 ), lab1 );
	gtk_box_append( GTK_BOX( box1 ), ent_buscar );
	gtk_box_append( GTK_BOX( box1 ), but_buscar );
	gtk_box_append( GTK_BOX( box1 ), but_volver );
	gtk_box_append( GTK_BOX( box1 ), but_salir );
	gtk_box_append( GTK_BOX( box ), box1 );

	if (img_mapa) {
		gtk_box_append( GTK_BOX( box ), overlay );
		gtk_overlay_set_child( GTK_OVERLAY( overlay ), img_mapa );
		gtk_widget_set_size_request( img_mapa, MAPA_ANCHO, MAPA_ALTO );
		gtk_widget_set_visible(img_port_ori, FALSE);
		gtk_widget_set_visible(img_port_des, FALSE);
		gtk_overlay_add_overlay( GTK_OVERLAY( overlay ), img_port_ori);
		gtk_overlay_add_overlay( GTK_OVERLAY( overlay ), img_port_des);

	}

	// Crear el listado de aeropuertos dentro de un scrollwindow
	mod_lisports = create_lisport_model();
	sel_lisports = gtk_single_selection_new( G_LIST_MODEL( mod_lisports ) );
	gtk_single_selection_set_autoselect( sel_lisports, TRUE );
	cv_lisports = gtk_column_view_new( GTK_SELECTION_MODEL( sel_lisports ) );
	gtk_column_view_set_single_click_activate( GTK_COLUMN_VIEW( cv_lisports ), TRUE);
	gtk_column_view_set_show_column_separators( GTK_COLUMN_VIEW( cv_lisports ), TRUE );
	g_signal_connect( cv_lisports, "activate", G_CALLBACK( event_aeropuerto_pulsado ), NULL);

	for (int ind=0; ind < PTR_LP_SIZE; ind++) {
	    factory = gtk_signal_list_item_factory_new();
	    g_signal_connect(factory, "setup", G_CALLBACK( setup_cb ), NULL);
	    g_signal_connect(factory, "bind", G_CALLBACK( bind_lisports[ind] ), NULL);
	    col_lisports = gtk_column_view_column_new( laerop_nombres[ind], factory );
	    gtk_column_view_append_column( GTK_COLUMN_VIEW( cv_lisports ), col_lisports );
	    g_object_unref( col_lisports );
	}

	scrwin_lispor = gtk_scrolled_window_new();
	create_scrollwin( scrwin_lispor, cv_lisports );
	scrwin_lispor_copy = scrwin_lispor;
	gtk_box_append( GTK_BOX( box2 ), scrwin_lispor );

	// Crear el listado de rutas dentro de un scrollwindow
	mod_lisrutas = create_lisrutas_model();
	sel_lisrutas = gtk_single_selection_new( G_LIST_MODEL( mod_lisrutas ) );
	gtk_single_selection_set_autoselect( sel_lisrutas, TRUE );
	cv_lisrutas = gtk_column_view_new( GTK_SELECTION_MODEL( sel_lisrutas ) );
	gtk_column_view_set_single_click_activate( GTK_COLUMN_VIEW( cv_lisrutas ), TRUE);
	gtk_column_view_set_show_column_separators( GTK_COLUMN_VIEW( cv_lisrutas ), TRUE );
	g_signal_connect( cv_lisrutas, "activate", G_CALLBACK( event_ruta_pulsada ), NULL);

	for (int ind=0; ind < PTR_LR_SIZE; ind++) {
		factory = gtk_signal_list_item_factory_new();
		g_signal_connect( factory, "setup", G_CALLBACK( setup_cb ),NULL );
		g_signal_connect( factory, "bind", G_CALLBACK( bind_lisrutas[ind] ),NULL );
		col_lisrutas = gtk_column_view_column_new( lrutas_nombres[ind], factory );
		gtk_column_view_append_column( GTK_COLUMN_VIEW( cv_lisrutas ), col_lisrutas );
		g_object_unref( col_lisrutas );
	}

	scrwin_lisrut = gtk_scrolled_window_new();
	create_scrollwin( scrwin_lisrut, cv_lisrutas );
	scrwin_lisrut_copy = scrwin_lisrut;
	gtk_box_append( GTK_BOX( box2 ), scrwin_lisrut );
	gtk_box_append( GTK_BOX( box ), box2 );

	gtk_window_present( GTK_WINDOW (win) );

	g_signal_connect( win, "close-request", G_CALLBACK( app_close ), NULL );

}

// ----------------------------------------------------------------------------
// Coloca el icono para el aeropuerto origen.
// Utiliza las variables globales ori_x y ori_y
// ----------------------------------------------------------------------------
static void colocar_port_origen() {
	sql_buscar_aeropuerto_tooltip( ori_port );
	char tt[300];
	strcpy( tt, tt_port->nombre );
	strcat( tt, " (" );
	strcat( tt, tt_port->iata );
	strcat( tt, ")\n" );
	strcat( tt, tt_port->ciudad );
	strcat( tt, "," );
	strcat( tt, tt_port->pais );

	gtk_widget_set_visible( img_port_ori_copy, TRUE );
	g_object_set( img_port_ori_copy, "halign", GTK_ALIGN_START, "margin-start", ori_x, NULL );
	g_object_set( img_port_ori_copy, "valign", GTK_ALIGN_START, "margin-top",   ori_y, NULL );
	gtk_widget_set_tooltip_text( img_port_ori_copy, tt );
}

// ----------------------------------------------------------------------------
// Coloca el icono para el aeropuerto destino
// Utiliza las variables globales des_x y des_y
// ----------------------------------------------------------------------------
static void colocar_port_destino() {
	sql_buscar_aeropuerto_tooltip( des_port );
	char tt[300];
	strcpy( tt, tt_port->nombre );
	strcat( tt, " (" );
	strcat( tt, tt_port->iata );
	strcat( tt, ")\n" );
	strcat( tt, tt_port->ciudad );
	strcat( tt, "," );
	strcat( tt, tt_port->pais );

	gtk_widget_set_visible( img_port_des_copy, TRUE );
	g_object_set( img_port_des_copy, "halign", GTK_ALIGN_START, "margin-start", des_x, NULL );
	g_object_set( img_port_des_copy, "valign", GTK_ALIGN_START, "margin-top",   des_y, NULL );
	gtk_widget_set_tooltip_text( img_port_des_copy, tt );
}

// ----------------------------------------------------------------------------
// Devuelve la imagen del fichero recibido comprobando los posibles errores.
// ----------------------------------------------------------------------------
static GtkWidget* obtener_imagen(const char * fichero) {
	GFile     *file = NULL;
	GError    *error = NULL;
	GtkWidget *img = NULL;
	GdkTexture *text = NULL;

	file = g_file_new_build_filename( "./data", fichero, NULL );
	text = gdk_texture_new_from_file( file, &error );
	if (!text) {
		g_print( "%s failed", "gdk_texture_new_from_file" );
	    if (error) {
	    	g_print( ": %s", error->message );
	        g_clear_error( &error );
	    }
	    g_print( "\n" );
	} else {
		img = gtk_picture_new_for_paintable( GDK_PAINTABLE( text ) );
	}
	return img;
}

