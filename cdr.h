//Dummy Cdr type that needs to be sent over in Avro format

#ifndef _CDR_H_
#define _CDR_H_

enum CdrType
{
	Diameter, Isup, Map, S1, S5, S11, Gx, Gy, Gm, Sip, X2
};

struct DiameterCdr
{
	int app_id;
	int hop_by_hop_id;
};
struct IsupCdr
{
	int caller_no;
	int called_no;
};

struct Cdr
{
	long timestamp;
	CdrType type;
	union {
		DiameterCdr diameter;
		IsupCdr isup;
	} detail;
};

#endif //_CDR_H_
