/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptors for Custom HID Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] = {
	0x12,			/*bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,	/*bDescriptorType */
	0x00,			/*bcdUSB */
	0x02,
	0x00,			/*bDeviceClass */
	0x00,			/*bDeviceSubClass */
	0x00,			/*bDeviceProtocol */
	0x40,			/*bMaxPacketSize40 */
	0x83,			/*idVendor (0x0483) */
	0x04,
	0x50,			/*idProduct = 0x5750 */
	0x57,
	0x00,			/*bcdDevice rel. 2.00 */
	0x02,
	1,			/*Index of string descriptor describing
				   manufacturer */
	2,			/*Index of string descriptor describing
				   product */
	3,			/*Index of string descriptor describing the
				   device serial number */
	0x01			/*bNumConfigurations */
}

;				/* CustomHID_DeviceDescriptor */

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] = {
	0x09,			/* bLength: Configuration Descriptor size */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType: Configuration */
	CUSTOMHID_SIZ_CONFIG_DESC,
	/* wTotalLength: Bytes returned */
	0x00,
	0x01,			/* bNumInterfaces: 1 interface */
	0x01,			/* bConfigurationValue: Configuration value */
	0x00,			/* iConfiguration: Index of string descriptor describing
				   the configuration */
	0xC0,			/* bmAttributes: Self powered */
	0x32,			/* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Custom HID interface ****************/
	/* 09 */
	0x09,			/* bLength: Interface Descriptor size */
	USB_INTERFACE_DESCRIPTOR_TYPE,	/* bDescriptorType: Interface descriptor type */
	0x00,			/* bInterfaceNumber: Number of Interface */
	0x00,			/* bAlternateSetting: Alternate setting */
	0x02,			/* bNumEndpoints */
	0x03,			/* bInterfaceClass: HID */
	0x00,			/* bInterfaceSubClass : 1=BOOT, 0=no boot */
	0x00,			/* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
	0,			/* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID HID ********************/
	/* 18 */
	0x09,			/* bLength: HID Descriptor size */
	HID_DESCRIPTOR_TYPE,	/* bDescriptorType: HID */
	0x10,			/* bcdHID: HID Class Spec release number */
	0x01,
	0x00,			/* bCountryCode: Hardware target country */
	0x01,			/* bNumDescriptors: Number of HID class descriptors to follow */
	0x22,			/* bDescriptorType */
	CUSTOMHID_SIZ_REPORT_DESC,	/* wItemLength: Total length of Report descriptor */
	0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
	/* 27 */
	0x07,			/* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */

	0x81,			/* bEndpointAddress: Endpoint Address (IN) */
	0x03,			/* bmAttributes: Interrupt endpoint */
	0x08,			/* wMaxPacketSize: 8 Bytes max */
	0x00,
	0x10,			/* bInterval: Polling Interval (32 ms) */
	/* 34 */

	0x07,			/* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: */
	/*      Endpoint descriptor type */
	0x01,			/* bEndpointAddress: */
	/*      Endpoint Address (OUT) */
	0x03,			/* bmAttributes: Interrupt endpoint */
	0x08,			/* wMaxPacketSize: 8 Bytes max  */
	0x00,
	0x20,			/* bInterval: Polling Interval (20 ms) */
	/* 41 */
};

/* CustomHID_ConfigDescriptor */
const uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] = {
	0x05, 0x01, 		/*Usage Page (1: Generic Desktop) */
	0x09, 0x06, 		/*Usage (6: Keyboard) 表示报表定义的是HID键盘*/
	0xA1, 0x01, 		/*Collection (1: Application) ====================集合开始*/
						/* 	以下定义了键盘的修饰键输入报表，共有8个键，组成一个字节*/
						/*	用法见HID Usage Table中的第10节中的键盘用法定义*/
	0x05, 0x07, 		/* 	Usage page (7: Key Codes) */
	0x19, 0xe0, 		/* 	Usage Minimum (224)*/
	0x29, 0xe7, 		/* 	Usage Maximum (231)*/
	0x15, 0x00, 		/* 	Logical Minimum (0)*/
	0x25, 0x01, 		/* 	Logical Maximum (1)*/
	0x75, 0x01, 		/* 	Report Size (1)*/
	0x95, 0x08, 		/* 	Report Count (8)*/
	0x81, 0x02,			/* 	Input (Data,Variable,Absolute) */
						/*	以下定义了一个保留字节的输入报表*/
	0x95, 0x01,			/*	Report Count (1)*/
	0x75, 0x08,			/*	Report Size (8),*/
	0x81, 0x03, 		/* 	Input (Constant) = Reserved Byte*/
						/*	以下定义了键盘的LED指示灯输出报表项目，共有5个指示灯*/
						/*	用法见HID Usage Table中的第11节中的LED用法定义*/
	0x95, 0x05,			/*	Report Count (5)*/
	0x75, 0x01,			/*	Report Size (1)*/
	0x05, 0x08,			/*	Usage Page (Page# for LEDs)*/
	0x19, 0x01,			/*	Usage Minimum (1)*/
	0x29, 0x05,			/*	Usage Maximum (5)*/
	0x91, 0x02,			/*	Output (Data, Variable, Absolute)*/
						/*	以下定义了3个填充位，与前面的5个LED指示灯数据组成一个完整的字节*/
	0x95, 0x01,			/*	Report Count (1)*/
	0x75, 0x03,			/*	Report Size (3)*/
	0x91, 0x03,			/*	Output (Constant)*/
						/*	以下定义了键盘的按键值输入报表项目，共6个字节，存放键编号（0~101）*/
						/*	用法见HID Usage Table中的第10节中的键盘用法定义*/
						/*	这样的设计可以允许一次输入6个按键的键值*/
	0x95, 0x06,			/*	Report Count (6)*/
	0x75, 0x08,			/*	Report Size (8)*/
	0x15, 0x00,			/*	Logical Minimum (0)*/
	0x25, 0x65,			/*	Logical Maximum (101)*/
	0x05, 0x07,			/*	Usage Page (7: Key Codes)*/
	0x19, 0x00,			/*	Usage Minimum (0)*/
	0x29, 0x65,			/*	Usage Maximum (101)*/
	0x81, 0x00,			/*	Input (Data, Array)*/
	0xC0 				/* End_Collection*/
};		/* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] = {
	CUSTOMHID_SIZ_STRING_LANGID,
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
	0x04
}

;				/* LangID = 0x0409: U.S. English */

const uint8_t CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] = {
	CUSTOMHID_SIZ_STRING_VENDOR,	/* Size of Vendor string */
	USB_STRING_DESCRIPTOR_TYPE,	/* bDescriptorType */
	/* Manufacturer: "STMicroelectronics" */
	'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
	'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
	'c', 0, 's', 0
};

const uint8_t CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] = {
	CUSTOMHID_SIZ_STRING_PRODUCT,	/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,	/* bDescriptorType */
	'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
	'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
	'D', 0
};

uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] = {
	CUSTOMHID_SIZ_STRING_SERIAL,	/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,	/* bDescriptorType */
	'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0
};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
