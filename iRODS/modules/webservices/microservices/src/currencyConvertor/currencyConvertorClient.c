/* currencyConvertorClient.c
   Generated by gSOAP 2.7.9l from currencyConvertorMS_wsdl.h
   Copyright(C) 2000-2007, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "currencyConvertorH.h"
#ifdef __cplusplus
extern "C" {
#endif

SOAP_SOURCE_STAMP("@(#) currencyConvertorClient.c ver 2.7.9l 2007-10-12 16:03:36 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call___ns1__ConversionRate(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct _ns1__ConversionRate *ns1__ConversionRate, struct _ns1__ConversionRateResponse *ns1__ConversionRateResponse)
{	struct __ns1__ConversionRate soap_tmp___ns1__ConversionRate;
	if (!soap_endpoint)
		soap_endpoint = "http://www.webservicex.net/CurrencyConvertor.asmx";
	if (!soap_action)
		soap_action = "http://www.webserviceX.NET/ConversionRate";
	soap->encodingStyle = NULL;
	soap_tmp___ns1__ConversionRate.ns1__ConversionRate = ns1__ConversionRate;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize___ns1__ConversionRate(soap, &soap_tmp___ns1__ConversionRate);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___ns1__ConversionRate(soap, &soap_tmp___ns1__ConversionRate, "-ns1:ConversionRate", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___ns1__ConversionRate(soap, &soap_tmp___ns1__ConversionRate, "-ns1:ConversionRate", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default__ns1__ConversionRateResponse(soap, ns1__ConversionRateResponse);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get__ns1__ConversionRateResponse(soap, ns1__ConversionRateResponse, "ns1:ConversionRateResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
	    return soap_recv_fault(soap,1);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

/* End of currencyConvertorClient.c */
