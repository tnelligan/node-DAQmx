/*****************************************************************************
*
* This file defines the National Instruments System Monitor Interface.
*
* © Copyright 2012,
* National Instruments Corporation.
* All rights reserved.
*
* file:        niSysMon.h
* originated:  5/8/2007
*
*****************************************************************************/

/* Note: this header requires the use of C99 data types.                      */
/* The client of this file is required to ensure that these types are defined */
/* before including this file, generally by including stdint.h beforehand.    */

#ifndef ___niSysMon_h___
#define ___niSysMon_h___

#ifndef NISYSMON_EXPORT
   #define NISYSMON_EXPORT
#endif

#if defined (_MSC_VER) || defined (_CVI_)
   #define NISYSMON_CALL __cdecl
#elif defined (__GNUC__)
   #define NISYSMON_CALL
#endif

/*****************************************************************************
*
* NI System Monitor Status Codes
*
*****************************************************************************/

#define NISYSMON_SUCCESS                           (0)

/* Warnings */
#define NISYSMON_WARN_UNKNOWN_WARNING              (24900)  /* Unknown Warning. */
#define NISYSMON_WARN_FAN_OUT_OF_RANGE_HIGH        (24901)  /* The reading for the indicated fan is beyond the sensor's maximum range. */
#define NISYSMON_WARN_FAN_OUT_OF_RANGE_LOW         (24902)  /* The reading for the indicated fan is below the sensor's minimum range. */
#define NISYSMON_WARN_TEMP_OUT_OF_RANGE_HIGH       (24903)  /* The reading for the indicated temperature sensor is beyond the sensor's maximum range. */
#define NISYSMON_WARN_TEMP_OUT_OF_RANGE_LOW        (24904)  /* The reading for the indicated temperature sensor is below the sensor's minimum range. */
#define NISYSMON_WARN_HARDWARE_ERROR               (24905)  /* One or more hardware components could not be enumerated due to an unexpected error.  The hardware has been ignored.*/

/* Errors */
#define NISYSMON_ERROR_UNKNOWN_ERROR               (-25000) /* Unknown Error. */
#define NISYSMON_ERROR_FEATURE_NOT_SUPPORTED       (-25001) /* The requested operation is not supported for the initialized resource. */
#define NISYSMON_ERROR_RESOURCE_NOT_FOUND          (-25002) /* Invalid resource name specified. */
#define NISYSMON_ERROR_INVALID_SESSION             (-25003) /* The given session or object reference is invalid. */
#define NISYSMON_ERROR_INVALID_PARAMETER           (-25004) /* The specified parameter is of incorrect type or format, or the index specified is out of range. */
#define NISYSMON_ERROR_INVALID_BUFFER_SIZE         (-25005) /* Buffer not large enough to hold all data. */
#define NISYSMON_ERROR_OBJECT_NOT_MONITORED        (-25006) /* Object is not being monitored by the chassis or controller */
#define NISYSMON_ERROR_PXI_SYSTEM_INI              (-25007) /* The pxisys.ini or pxiesys.ini file could not be found. */
#define NISYSMON_ERROR_INVALID_PXIESYS_INI         (-25008) /* System information in pxisys.ini or pxiesys.ini appears to be improperly formatted. */
#define NISYSMON_ERROR_INSTALLATION                (-25009) /* Some file or component that should have been installed is missing. */
#define NISYSMON_ERROR_INCORRECT_CHASSIS_SLOT      (-25010) /* The Chassis Monitor Module is in the incorrect slot. */
#define NISYSMON_ERROR_OS_FAILURE                  (-25011) /* The operating system returned an unexpected error. */
#define NISYSMON_ERROR_INSUFFICIENT_PRIVILEGE      (-25012) /* Insufficient user privilege to complete the requested operation. */
#define NISYSMON_ERROR_COM_INITIALIZATION_FAILURE  (-25013) /* Failed to initialize the COM library. */
#define NISYSMON_ERROR_IWBEMLOCATOR_FAILURE        (-25014) /* Failed to create IWbemLocator object. */
#define NISYSMON_ERROR_WMI_CONNECTION_FAILURE      (-25015) /* Could not connect to WMI.*/
#define NISYSMON_ERROR_WMI_PROXY_FAILURE           (-25016) /* Error in setting proxy blanket for WMI. */
#define NISYSMON_ERROR_MEMORY_ALLOCATION_FAILURE   (-25017) /* Error allocating memory for object. */
#define NISYSMON_ERROR_FAN_RPM_NOT_SUPPORTED       (-25018) /* Reading the Rpm for the designated fan is not supported. */
#define NISYSMON_ERROR_POWER_INTERRUPTED           (-25019) /* The device can not be accessed because the machine is changing to a lower power state. */
#define NISYSMON_ERROR_FIRMWARE_VERSION_UNSUPPORT  (-25020) /* The device firmware version is incompatible with the driver version. */
#define NISYSMON_ERROR_MXI_FIRMWARE_KB_5Z8ENB62    (-25021) /* The firmware version on the MXI controller can not support monitoring. See KnowledgeBase article 5Z8ENB62 for more details.*/

/******************************************************************************
*
* Type Definitions
*
******************************************************************************/

typedef uint32_t NiSysMonSessionId;
typedef int32_t NiSysMonStatus;

/******************************************************************************
* Operation Identifiers
*
* These codes define the attributes that can be read.
* The names adhere to the following format:
*
* NISMOPCODE_<datatype>_[Indexed?]_ATTR_NAME
*
* The combination of the datatype and the presence of "NDXD" can be
* used to determine if a index needs to be provided to the method.
* For functions that do not require an index, the input parameter should be set to 0.
* For example:
*
* NISMOPCODE_U32_ATTR_PROCESSOR_COUNT      -> The value of a_Index will not be used and can be set to 0.
* NISMOPCODE_D64_NDXD_ATTR_VOLTAGE_READING -> Set a_Index to the index of the voltage sensor to read.
*
* Note that the operations that are supported will be dependent upon what
* resource string is used and which device is being communicated with.
* Making a call that is unsupported for the current device will return
* error NISYSMON_ERROR_FEATURE_NOT_SUPPORTED.
*
******************************************************************************/

/* Device Information */
#define NISMOPCODE_STR_ATTR_DEVICE_MODEL_NAME    0x1000
#define NISMOPCODE_U32_ATTR_DEVICE_TYPE          0x1001

/* Possible responses to NISMOPCODE_U32_ATTR_DEVICE_TYPE */
enum kNiSmDeviceType
{
   kNiSmDevTypePxiChassis     = 0,
   kNiSmDevTypePxiController  = 1,
   kNiSmDevTypePxiEChassis    = 2,
   kNiSmDevTypePxiEController = 3,

   kNiSmDevTypeCount          = 4
};

/* Cpu/Core Attributes */
#define NISMOPCODE_U32_ATTR_PROCESSOR_COUNT      0x2000
#define NISMOPCODE_D64_NDXD_ATTR_PROC_UTILIZ     0x2004 /* Supported on Windows only - returns Percentage (100.0 = 100%) */
#define NISMOPCODE_D64_ATTR_TOTAL_CPU_UTILIZ     0x2005 /* Supported on Windows only - returns Percentage (100.0 = 100%) */
#define NISMOPCODE_U64_NDXD_ATTR_PROC_CLK_SPEED  0x2007 /* Supported on Windows only - returns Hertz */
#define NISMOPCODE_STR_NDXD_ATTR_PROC_NAME       0x2008 /* Supported on Windows only */
#define NISMOPCODE_STR_NDXD_ATTR_PROC_MANUFACT   0x2009 /* Supported on Windows only */
#define NISMOPCODE_D64_NDXD_ATTR_PROC_TEMP       0x200A /* Returns degrees Celsius */

/* Memory Attributes - Supported on Windows only*/
#define NISMOPCODE_U64_ATTR_TOTAL_PHYS_MEMORY    0x3000 /* returns Bytes */
#define NISMOPCODE_U64_ATTR_FREE_PHYS_MEMORY     0x3001 /* returns Bytes */
#define NISMOPCODE_U64_ATTR_TOTAL_VIRTUAL_MEMORY 0x3002 /* returns Bytes */
#define NISMOPCODE_U64_ATTR_FREE_VIRTUAL_MEMORY  0x3003 /* returns Bytes */

/* Voltage readings */
#define NISMOPCODE_U32_ATTR_VOLTAGE_COUNT        0x4000
#define NISMOPCODE_D64_NDXD_ATTR_VOLTAGE_READING 0x4001 /* returns Volts */
#define NISMOPCODE_D64_NDXD_ATTR_NOMINAL_VOLTAGE 0x4002 /* returns Volts */
#define NISMOPCODE_D64_NDXD_ATTR_VOLTAGE_MAX     0x4003 /* returns Volts */
#define NISMOPCODE_D64_NDXD_ATTR_VOLTAGE_MIN     0x4004 /* returns Volts */

/* Temperature Readings */
#define NISMOPCODE_U32_ATTR_TEMP_COUNT           0x5000
#define NISMOPCODE_D64_NDXD_ATTR_MAX_TEMP        0x5001 /* returns degrees Celsius */
#define NISMOPCODE_D64_NDXD_ATTR_MIN_TEMP        0x5002 /* returns degrees Celsius */
#define NISMOPCODE_D64_NDXD_ATTR_TEMP_READING    0x5003 /* returns degrees Celsius */
#define NISMOPCODE_U32_NDXD_ATTR_TEMP_TYPE       0x5004

/* Possible responses to NISMOPCODE_U32_NDXD_ATTR_TEMP_TYPE */
enum kNiSmTempType
{
   kNiSmTempTypeIntake    = 0,
   kNiSmTempTypeExhaust   = 1,
   kNiSmTempTypeInvalid   = 2,

   kNiSmTempTypeCount     = 3
};

/* Fan Readings */
#define NISMOPCODE_U32_ATTR_FAN_COUNT            0x6000
#define NISMOPCODE_U32_NDXD_ATTR_FAN_READING     0x6001 /* return RPM */
#define NISMOPCODE_U32_NDXD_ATTR_FAN_HEALTH      0x6002

/* Possible response to NISMOPCODE_U32_NDXD_ATTR_FAN_HEALTH */
enum
{
   kNiSmFanNotHealthy  = 0,
   kNiSmFanHealthy     = 1,

   kNiSmFanHealthCount = 2
};

/*****************************************************************************
* Methods
*
* OpenSession must be called before any other method is called, and the
* session Id returned should be passed in to all other calls.  Calling OpenSession
* multiple times with the same resource string will return the same session id,
* but CloseSession must be called the same number of times as OpenSession in order
* for memory to be properly freed and connections to devices to be closed.
*
******************************************************************************/

#if defined(__cplusplus) || defined(__cplusplus__)

   extern "C"
   {

#endif

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_OpenSession (
   const char         * a_pResourceString,
   NiSysMonSessionId  * a_pSessionId
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_CloseSession (
   NiSysMonSessionId    a_SessionId
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetAttributeD64 (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   double             * a_pValue
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetIndexedAttributeD64 (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   const uint32_t       a_Index,
   double             * a_pValue
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetAttributeU32 (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   uint32_t           * a_pValue
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetIndexedAttributeU32 (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   const uint32_t       a_Index,
   uint32_t           * a_pValue
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetAttributeU64 (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   uint64_t           * a_pValue
   );

NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetIndexedAttributeU64 (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   const uint32_t       a_Index,
   uint64_t           * a_pValue
   );

/* If the buffer length is insufficient, the required
 * size will be returned in a_pBufferLength. */
NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetAttributeSTR (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   char               * a_pBuffer,
   uint32_t           * a_pBufferLength
   );

/* If the buffer length is insufficient, the required
 * size will be returned in a_pBufferLength. */
NISYSMON_EXPORT NiSysMonStatus NISYSMON_CALL NiSysMon_GetIndexedAttributeSTR (
   NiSysMonSessionId    a_SessionId,
   const uint32_t       a_OperationId,
   const uint32_t       a_Index,
   char               * a_pBuffer,
   uint32_t           * a_pBufferLength
   );

#if defined(__cplusplus) || defined(__cplusplus__)

   };

#endif

#endif /* ___niSysMon_h___ */
