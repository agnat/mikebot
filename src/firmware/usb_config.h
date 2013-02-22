#ifndef USB_CONFIG_INCLUDED
#define USB_CONFIG_INCLUDED


//#define USB_RS232_ONLY
//#define USB_SENSORS_ONLY

#if defined(USB_RS232_ONLY) || defined(USB_SENSORS_ONLY)
# define USB_ONE_TTY_ONLY
#endif

#if defined(USB_RS232_ONLY)
# define RS232_CDC cdc1
#elif defined(USB_SENSORS_ONLY)
# define SENSOR_CDC cdc1
#else
# define RS232_CDC cdc1
# define SENSOR_CDC cdc2
#endif

#endif // USB_CONFIG_INCLUDED
