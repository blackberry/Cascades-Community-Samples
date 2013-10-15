#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>
#include "applicationui.hpp"

#include <Qt/qdeclarativedebug.h>

#include "PeripheralDetail.hpp"

#include <pthread.h>
#include <QDebug>
#include <iostream>

using namespace bb::cascades;


void msgHandler(QtMsgType type, const char * message) {
	std::cerr << message << std::endl;
}

Q_DECL_EXPORT int main(int argc, char **argv)
{
	qInstallMsgHandler(msgHandler);
	qRegisterMetaType<PeripheralDetail>("PeripheralDetail");
    Application app(argc, argv);

    // Create the Application UI object, this is where the main.qml file
    // is loaded and the application scene is set.
    new ApplicationUI(&app);

    // Enter the application main event loop.
    return Application::exec();
}


//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <errno.h>
//#include <sys/neutrino.h>
//#include <sys/siginfo.h>
//
//#include <peripheral_discovery.h>
//
//static void print_peripheral_properties( pd_peripheral_t *peripheral ) {
//  pd_peripheral_properties_t *properties;
//  pd_peripheral_property_t *property;
//  pd_property_type_t type;
//  const char *name;
//  const char *strvalue;
//  int intval;
//
//  if( (properties = pd_alloc_property_list()) == NULL ) {
//    printf( "Couldn't allocate properties\n" );
//    return;
//  }
//
//  if( pd_get_peripheral_properties( peripheral, properties ) == EOK ) {
//    while( pd_get_next_property( properties, &property ) == EOK ) {
//      type = pd_get_property_type( property );
//      switch( type ) {
//        case PD_STRING_TYPE:
//          pd_get_property_as_string( property, &name, &strvalue );
//          fprintf(stderr,"  property: %s, value: %s\n", name, strvalue);
//          break;
//        case PD_INTEGER_TYPE:
//          pd_get_property_as_integer( property, &name, &intval );
//          fprintf(stderr,"  property: %s, value: %d\n", name, intval);
//          break;
//      }
//    }
//  } else {
//    printf( "Couldn't get properties\n" );
//  }
//
//  pd_free_property_list( &properties );
//}
//
//#define MY_PD_PULSE_CODE 1
//
//void list() {
//	int rc;
//	rc = pd_initialize( 0 );
//	if (rc!=EOK) {
//		fprintf(stderr, "Couldn't connect to peripheral discovery API\n" );
//		return;
//	}
//
//	bool displaySupported(false);
//	pd_is_class_supported(PD_CLASS_VENDOR_DEFINED,&displaySupported);
//
//	fprintf(stderr, "Vendor Supported %d\n", displaySupported);
//
//	pd_peripheral_list_t * list = pd_alloc_peripheral_list();
//
//	rc = pd_get_peripherals(PD_CLASS_VENDOR_DEFINED,list);
//	if (rc!=EOK) {
//		fprintf(stderr,"Failed get periphs\n");
//	} else {
//		int id;
//		pd_peripheral_t * peripheral;
//
//		fprintf(stderr,"Get peripherals worked\n");
//
//		while(EOK==pd_get_next_peripheral(list,&id,&peripheral)) {
//			fprintf(stderr,"Found one\n");
//		}
//
//		fprintf(stderr,"Done");
//	}
//
//	pd_free_peripheral_list(&list);
//}
//
//int main( int argc, char *argv[] ) {
//  bool usb_host_mode_supported;
//  bool serial_is_supported, vendor_defined_is_supported;
//  struct sigevent ev1, ev2, ev3;
//  pd_peripheral_t *peripheral;
//  int chid, coid;
//
//  list();
//
//  if( pd_is_bus_supported( PD_BUS_USB_HOST_MODE,
//                           &usb_host_mode_supported ) != EOK ) {
//    printf( "Error determining if usb host mode is supported\n" );
//    pd_uninitialize();
//    return 1;
//  }
//  if( pd_is_class_supported( PD_CLASS_SERIAL,
//                             &serial_is_supported ) != EOK ) {
//    fprintf(stderr,"Error determining if serial class is supported\n");
//    pd_uninitialize();
//    return 1;
//  }
//
//  if( pd_is_class_supported( PD_CLASS_VENDOR_DEFINED,
//                             &vendor_defined_is_supported ) != EOK ) {
//    fprintf(stderr,"Error determining if vendor defined class is supported\n");
//    pd_uninitialize();
//    return 1;
//  }
//
//  if( !usb_host_mode_supported ) {
//    printf( "USB host mode is not supported.\n" \
//             "No sense trying to find peripherals\n" );
//    pd_uninitialize();
//    return 1;
//  }
//
//  if( !serial_is_supported && !vendor_defined_is_supported ) {
//    printf( "None of the classes I'm interested in are supported.\n");
//    printf( "No sense trying to find peripherals\n" );
//    pd_uninitialize();
//    return 1;
//  }
//
//  chid = ChannelCreate( 0 );
//  coid = ConnectAttach( 0, 0, chid, 0, 0 );
//
//  // Initialize pulse sigevent.
//  // You can add the class id to help identify events later,
//  // if you're registering for multiple classes.
//  if( serial_is_supported ) {
//    SIGEV_PULSE_INIT( &ev1, coid, SIGEV_PULSE_PRIO_INHERIT, MY_PD_PULSE_CODE,
//                    PD_CLASS_SERIAL );
//  }
//  if( vendor_defined_is_supported ) {
//    SIGEV_PULSE_INIT( &ev2, coid, SIGEV_PULSE_PRIO_INHERIT, MY_PD_PULSE_CODE,
//                    PD_CLASS_VENDOR_DEFINED );
//  }
//
//  SIGEV_PULSE_INIT( &ev3, coid, SIGEV_PULSE_PRIO_INHERIT, MY_PD_PULSE_CODE, PD_CLASS_DISPLAY);
//
//  if( serial_is_supported ) {
//    fprintf(stderr,"register %d\n",pd_register_event( PD_CLASS_SERIAL, &ev1 ));
//  }
//  if( vendor_defined_is_supported ) {
//    fprintf(stderr,"register %d\n",pd_register_event( PD_CLASS_VENDOR_DEFINED, &ev2 ));
//  }
//
//  fprintf(stderr,"register %d\n",pd_register_event( PD_CLASS_DISPLAY, &ev3));
//
//  // You need to allocate a pd_peripheral_t to get events,
//  // but you only need to do it once.
//  peripheral = pd_alloc_peripheral();
//
//  // A message receive thread.
//  while (1) {
//	  fprintf(stderr,"While\n");
//    struct _pulse pulse;
//    pd_event_type_t type;
//    int peripheral_id;
//    pd_class_t event_class;
//
//    fprintf(stderr,"pre received\n");
//    MsgReceive(chid, &pulse, sizeof(pulse), NULL);
//    fprintf(stderr,"post receive\n");
//
//    if( pulse.code == MY_PD_PULSE_CODE ) {
//      // If you populated the sigevent value pointer, retrieve it now.
//      event_class = (pd_class_t)(int) pulse.value.sival_ptr;
//
//      // Get the event that woke this thread up.
//      if( pd_get_event( &type, &peripheral_id, peripheral ) == EOK ) {
//        switch( type ) {
//          case PD_EVENT_INSERTION:
//            printf( "Peripheral Insertion. id=0x%X, class=%d\n",
//                    peripheral_id, event_class );
//            print_peripheral_properties( peripheral );
//            break;
//          case PD_EVENT_REMOVAL:
//            printf( "Peripheral Removal. id=0x%X, class=%d\n",
//                    peripheral_id, event_class );
//            break;
//        }
//      }
//    }
//  }
//
//  pd_free_peripheral( &peripheral );
//
//  pd_uninitialize();
//
//  return 0;
//}
