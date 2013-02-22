#ifndef USB_INCLUDED
#define USB_INCLUDED

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

/** Endpoint address of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 1)

/** Endpoint address of the CDC device-to-host data IN endpoint. */
#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN  | 2)

/** Endpoint address of the CDC host-to-device data OUT endpoint. */
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 3)

/** Endpoint address of the MIDI streaming data IN endpoint, for device-to-host data transfers. */
#define MIDI_STREAM_IN_EPADDR          (ENDPOINT_DIR_IN  | 4)

/** Endpoint address of the MIDI streaming data OUT endpoint, for host-to-device data transfers. */
#define MIDI_STREAM_OUT_EPADDR         (ENDPOINT_DIR_OUT | 5)

/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define CDC_NOTIFICATION_EPSIZE        8

/** Size in bytes of the CDC data IN and OUT endpoints. */
#define CDC_TXRX_EPSIZE                16

/** Endpoint size in bytes of the Audio isochronous streaming data IN and OUT endpoints. */
#define MIDI_STREAM_EPSIZE             64


/* Type Defines: */
/** Type define for the device configuration descriptor structure. This must be defined in the
 *  application code, as the configuration descriptor contains several sub-descriptors which
 *  vary between devices, and which describe the device's usage to the host.
 */
typedef struct
{
    USB_Descriptor_Configuration_Header_t    Config;

    // CDC Control Interface
    USB_Descriptor_Interface_Association_t    CDC_IAD;
    USB_Descriptor_Interface_t                CDC_CCI_Interface;
    USB_CDC_Descriptor_FunctionalHeader_t     CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t        CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t      CDC_Functional_Union;
    USB_Descriptor_Endpoint_t                 CDC_ManagementEndpoint;

    // CDC Data Interface
    USB_Descriptor_Interface_t                CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t                 CDC_DataOutEndpoint;
    USB_Descriptor_Endpoint_t                 CDC_DataInEndpoint;

    // MIDI Audio Control Interface
    USB_Descriptor_Interface_t                Audio_ControlInterface;
    USB_Audio_Descriptor_Interface_AC_t       Audio_ControlInterface_SPC;

    // MIDI Audio Streaming Interface
    USB_Descriptor_Interface_t                Audio_StreamInterface;
    USB_MIDI_Descriptor_AudioInterface_AS_t   Audio_StreamInterface_SPC;
    USB_MIDI_Descriptor_InputJack_t           MIDI_In_Jack_Emb;
    USB_MIDI_Descriptor_InputJack_t           MIDI_In_Jack_Ext;
    USB_MIDI_Descriptor_OutputJack_t          MIDI_Out_Jack_Emb;
    USB_MIDI_Descriptor_OutputJack_t          MIDI_Out_Jack_Ext;
    USB_Audio_Descriptor_StreamEndpoint_Std_t MIDI_In_Jack_Endpoint;
    USB_MIDI_Descriptor_Jack_Endpoint_t       MIDI_In_Jack_Endpoint_SPC;
    USB_Audio_Descriptor_StreamEndpoint_Std_t MIDI_Out_Jack_Endpoint;
    USB_MIDI_Descriptor_Jack_Endpoint_t       MIDI_Out_Jack_Endpoint_SPC;

} USB_Descriptor_Configuration_t;

/* Function Prototypes: */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
        const uint8_t wIndex,
        const void** const DescriptorAddress)
ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif // USB_INCLUDED
