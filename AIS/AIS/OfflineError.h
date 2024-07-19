/************************************************************************/
/* OASError.h                                                          */
/************************************************************************/

#ifndef _OASERROR_H
#define _OASERROR_H

//#define ERROR_BUFFER_ALL_ZEROS  0x754L

//
// MessageId: ERROR_UNDEFINED_FILE_TYPE
//
// MessageText: Undefined File Type
//
#define ERROR_UNDEFINED_FILE_TYPE	0xE0000001


//
// MessageId: ERROR_FILE_FORMAT_ABNORMAL
//
// MessageText: Error File Format
//
#define ERROR_FILE_FORMAT_ABNORMAL	0xE0000002	


//
// MessageId: ERROR_FILE_SIZE_ZERO
//
// MessageText: File is empty
//
#define ERROR_FILE_SIZE_ZERO			0xE0000003


//
// MessageId: ERROR_INPUT_FILE_OFFSET
//
// MessageText: 
//
#define ERROR_INPUT_FILE_OFFSET				0xE0000004


//
// MessageId: ERROR_UNDEFINED_SMT
//
// MessageText: Undefined Signal Modulation Type
//
#define ERROR_UNDEFINED_MODULATION_TYPE		0xE0000005

//
// MessageId: ERROR_UNDEFINED_DECODER_TYPE
//
// MessageText: Undefined Decoder Type
//
#define ERROR_UNDEFINED_DECODER_TYPE		0xE0000009

//
// MessageId: ERROR_INCORRECT_DDC_PARAMETER
//
// MessageText: 
//
#define ERROR_UD_INCORRECT_DDC_PARAMETER 0xE000000A


//
// MessageId: ERROR_UNCREATED_DATA_CHANNEL
//
// MessageText: 
//
#define ERROR_UNCREATED_DATA_CHANNEL	0xE000000C

//
// MessageId: ERROR_UNCREATED_SIGNAL_CHANNEL
//
// MessageText: 
//
#define ERROR_UNCREATED_SIGNAL_CHANNEL	0xE000000D

//
// MessageId: ERROR_PARAM_PROBE_LACK_DATA
//
// MessageText: 
//
#define ERROR_PARAM_PROBE_LACK_DATA		0xE000000E

//
// MessageId: ERROR_PARAM_FH_INIT
//
// MessageText: 
//
#define ERROR_FH_PARAM_INIT				0xE000000F


//
// MessageId: ERROR_FH_NO_SORT_RESULT
//
// MessageText: 
//
#define ERROR_FH_NO_SORT_RESULT			0xE0000010


//
// MessageId: ERROR_FH_NO_DEHOP_RESULT
//
// MessageText: 
//
#define ERROR_FH_NO_DEHOP_RESULT		0xE0000011

//
// MessageId: ERROR_DEMOD_PARAM
//
// MessageText: 
//
#define ERROR_DEMOD_PARAM				0xE0000012

//
// MessageId: ERROR_UD_FILE_NOT_LOAD
//
// MessageText: 
//
#define ERROR_UD_FILE_NOT_LOAD				0xE0000013

//
// MessageId: ERROR_UD_DATA_LACK
//
// MessageText: 
//
#define ERROR_UD_DATA_LACK					0xE0000014

//
// MessageId: ERROR_UD_DATA_LACK
//
// MessageText: 
//
#define ERROR_UD_DATA_RANGE_INVALID			0xE0000015


//
// MessageId: ERROR_UD_DATA_LACK
//
// MessageText: 
//
#define ERROR_UD_MULTI_CHANNEL_INVALID		0xE0000016


//
// MessageId: ERROR_UD_CENTER_FREQ_INVALID
//
// MessageText: 
//
#define ERROR_UD_CENTER_FREQ_INVALID		0xE0000017

//
// MessageId: ERROR_UD_ADD_SIGNALCHANNEL_FAILD
//
// MessageText: 
//
#define ERROR_UD_ADD_SIGNALCHANNEL_FAILD	0xE0000018

//
// MessageId: ERROR_UD_ADD_SPECTRUM_FAILD
//
// MessageText: 
//
#define ERROR_UD_ADD_SPECTRUM_FAILD			0xE0000019


//
// MessageId: ERROR_UD_ADD_DEMOD_FAILD
//
// MessageText: 
//
#define ERROR_UD_ADD_DEMODULATOR_FAILD		0xE0000020


//
// MessageId: ERROR_UD_ADD_PARAM_ESTIMATE_FAILD
//
// MessageText: 
//
#define ERROR_UD_ADD_PARAM_ESTIMATE_FAILD	0xE0000021


#endif  //#ifndef SPDERROR_H