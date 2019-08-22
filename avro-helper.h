#ifndef _AVRO_HELPER_H_
#define _AVRO_HELPER_H_

#include "GenericWriter.hh"
#include <utility>

class GenericWriter : avro::GenericWriter
{
	using avro::GenericWriter(const ValidSchema& s, const EncoderPtr& encoder);
	//Not including static functions here - they can come later, if needed
	
	using GenericUnionDatum = std::pair<const avro::GenericUnion&, const avro::GenericDatum&>;
	void write(const GenericUnionDatum datum) const;
};

void GenericWriter::write(const GenericUnionDatum& union_datum) const {
	//datum.selectBranch(union_datum.first.encodeUnionIndex());
	encoder_.encodeUnionIndex(union_datum.first.unionBranch());	//TODO: encoder_ is not accessible here!
	return write(union_datum.second);
}

#endif  //_AVRO_HELPER_H_
