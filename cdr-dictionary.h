#ifndef _CDR_DICTIONARY_H_
#define _CDR_DICTIONARY_H_

#include "cdr.h"

#include "avro"

#include <map>
#include <functional>
#include <string>

namespace AVRO
{
	//template<typename CdrType>???
	using DataGetter = std::function<GenericDatum(const Cdr&)>;
	using DataGetterMap = std::map<std::string, DataGetter>;

	//Lambda DataGetter
	auto diameter_app_id_getter = [](const Cdr& cdr) -> GenericDatum {
		return cdr.type == Diameter? 
			GenericDatum{cdr.detail.diameter.app_id}:
			GenericDatum{};
	};
	//Function DataGetter
	GenericDatum isup_caller_no_getter(const Cdr& cdr) {
		return cdr.type == Isup? 
			GenericDatum{cdr.detail.isup.caller_no}: 
			GenericDatum{};
	}
	//Higher-level DataGetter for composability
	auto type_value_getter = [](CdrType type, auto callable_value_getter, const Cdr& cdr) {
		return cdr.type == type? 
			GenericDatum{callable_value_getter(cdr)}:
			GenericDatum{};
	};
	//Lower-level DataGetters
	auto hop_by_hop_id_getter = [](const Cdr& cdr) { return cdr.detail.diameter.hop_by_hop_id; };
	auto end_to_end_id_getter = [](const Cdr& cdr) { return cdr.detail.diameter.end_to_end_id; };
	auto request_code_getter = [](const Cdr& cdr) { return cdr.detail.diameter.request_code; };

	//The lookup of the DataGetter against the Avro field name
	DataGetterMap dataGetterMap = {
		{"timestamp"s, {[](const Cdr& cdr) { return GenericDatum{cdr.timestamp}; }}},
		{"diameter.application-id"s, {diameter_app_id_getter}},
		{"isup.caller-no"s, {isup_caller_no_getter}},
		{"isup.called-no"s, {[](const Cdr& cdr) { 
			return cdr.type == Isup? GenericDatum{cdr.detail.isup.called_no}: GenericDatum{}; }}},
		{"diameter.hop_by_hop_id"s, {[](const Cdr& cdr) { 
			return type_value_getter(Diameter, hop_by_hop_id_getter, cdr); }}},
		{"diameter.end_to_end_id"s, {[](const Cdr& cdr) { 
			return type_value_getter(Diameter, end_to_end_id_getter, cdr); }}},
		{"diameter.request_code"s, {[](const Cdr& cdr) { 
			return type_value_getter(Diameter, request_code_getter, cdr); }}}
	};

	//DataGetter Dictionary Abstraction for Cdr
	class CdrDictionary
	{
		CdrDictionary(const DataGetterMap& m) : dataGetterMap(m) {}
		const DataGetter& find(const std::string& field_name) const {
			if (auto it = dataGetterMap.find(field_name); it != end(dataGetterMap)) {
				return it->second;
			}
			static DataGetter nullGetter{[](const Cdr&) { return GenericDatum{}; }};
			return nullGetter;
		}
		
		private:
		DataGetterMap dataGetterMap;
	};
	
#ifndef __cplusplus
	//Fake dictionary objects for the C world
	//Need extern? 
	CdrDictionary cs_dict{dataGetterMap};
	CdrDictionary epc_dict{dataGetterMap};
#endif
}

#endif //_CDR_DICTIONARY_H_
