#ifndef _AVRO_WRAPPER_H_
#define _AVRO_WRAPPER_H_

#include "CdrDictionary.h"
#include "Cdr.h"

#include <memory>
#include <utility>

#include <avro> //apache avro - check the right include

#ifndef __cplusplus
#define EXTERN_C 
#else
#define EXTERN_C extern "C"
#endif

#ifdef __cplusplus
namespace AVRO
{
	using owning_buffer = std::pair<std::unique_ptr<uint8_t[]>, int>;
	using SchemaType = CdrType;
	using SchemaRegister = std::map<SchemaType, Schema>;
	
	class SchemaRegistry
	{
		SchemaRegistry(const SchemaRegister& _registry);
		SchemaRegistry(SchemaRegister&& _registry);
		SchemaRegistry(const SchemaRegistry& other);
		SchemaRegistry(SchemaRegistry&& other);
		
		void reg(SchemaType type, const Schema& schema);
		void reg(SchemaType type, Schema&& schema);
		const Schema& find(SchemaType type) const;
		void unreg(SchemaType type);
		
		private:
		//TODO: Should this be ValidSchema instead? 
		SchemaRegistry registry;
	};
	
	class AvroEncoder
	{
		AvroEncoder(const Schema& schema);
		
		template<typename T>
		owning_buffer encode(const T& data, const dictionary& dict) const;
		
		private:
			template<typename T>
			void encodeHeader(GenericWriter& writer, const T& data, const dictionary& dict) const;
			template<typename T>
			void encodeFields(GenericWriter& writer, const T& data, const dictionary& dict) const;
		
			Schema schema;
	};
  
	//Free function wrapper for Avro encoding
	template<typename T>
	owning_buffer avro_encode(const T& data, const dictionary& dict, const Schema& schema) {
		return AvroEncoder{schema}.encode(data, dict);
	}
	template<typename T>
	owning_buffer avro_encode(const T& data, const dictionary& dict, SchemaType type) {
		const Schema& schema = schemaRegistry.find(type);
		return avro_encode(data, dict, schema);
	}
}

template<typename T>
inline AVRO::owning_buffer AVRO::AvroEncoder::encode(const T& data, const dictionary& dict) const {
	EncoderPtr encoder = binaryEncoder();
	auto out = memoryOutputStream(8096);
	//GenericWriter writer{validSchema, encoderPtr};
	StreamWriter writer{out};	//encoder not used!!!???
	encodeHeader(writer, data, dict);
	encodeFields(writer, data, dict);
	return writer.getBuffer();
}
template<typename T>
inline void AVRO::AvroEncoder::encodeHeader(GenericWriter& writer, const T& data, const dictionary& dict) const {
}
template<typename T>
inline void AVRO::AvroEncoder::encodeFields(GenericWriter& writer, const T& data, const dictionary& dict) const {
	const int no_fields = schema.root().names();
	for (int i = 0; i < no_names; i++) {
		const std::string& fieldName = schema.root().nameAt(i);
		const DataGetter& getter = dict.find(fieldName);
		GenericDatum value = getter(data);
		writer.write(value);
	}
	writer.flush();
}

inline AVRO::SchemaRegistry::SchemaRegistry(const SchemaRegister& _registry) : 
registry(_registry) {
}
inline AVRO::SchemaRegistry::SchemaRegistry(SchemaRegister&& _registry) : 
registry(_registry) {
}
inline AVRO::SchemaRegistry::SchemaRegistry(const SchemaRegistry& other) : 
registry(other.registry) {
}
inline AVRO::SchemaRegistry::SchemaRegistry(SchemaRegistry&& other) : 
registry(other.registry) {
}

inline void AVRO::SchemaRegistry::reg(SchemaType type, const Schema& schema) { 
	registry[type] = schema; 
}
inline void AVRO::SchemaRegistry::reg(SchemaType type, Schema&& schema) { 
	registry[type] = schema; 
}
inline const Schema& AVRO::SchemaRegistry::find(SchemaType type) const {
	if (auto it = registry.find(type); it != end(registry)) {
		return it->second;
	}
	return avro::NullSchema{};
}
inline void AVRO::SchemaRegistry::unreg(SchemaType type) { 
	registry.erase(type); 
}

#endif

#ifndef __cplusplus
	struct c_owning_buffer
	{
		uint8_t* data;
		int len;
	};
	//Caller to be owner of returned buffer - _MUST_ free
	EXTERN_C c_owning_buffer avro_encode_cs(void* data, SchemaType type) {
		auto buff = avro_encode(reinterpret_cast<const T_CS_CDR_MSG&>(*data), cs_dict, type);
		return {buff.first.release(), buff.second};
	}
	//Caller to be owner of returned buffer - _MUST_ free
	EXTERN_C c_owning_buffer avro_encode_epc(void* data, SchemaType type) {
		auto buff = avro_encode(reinterpret_cast<const T_EPC_CDR_MSG&>(*data), epc_dict, type);
		return {buff.first.release(), buff.second};
	}
#endif
  
#endif  //_AVRO_WRAPPER_H_
