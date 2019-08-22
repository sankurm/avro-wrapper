# avro-wrapper
Generic way to use apache/avro without knowledge of the avsc with customisation points providing full control

A framework to dynamically generate **AVRO output as per a run-time decided dynamic schema**. 

This uses the AVRO encoding implementation from apache/avro library. 

The implementation is capable of picking data from disparate data sources to be encoded into the AVRO output. The calling code has full control of this. The sample implementation encodes a struct Cdr. 

## Problem addressed
AVRO encoding implementations, I have come across, assume knowledge of the AVRO schema. The observation was that the code "knows" (read hard-codes) the fields, data-type and their order. 

In reality, implementations need to use a dynamic AVRO schema due to:
- changing fields over time
- the need to provide control to the user to decide the schema/output
- supporting multiple formats simultaneously for different consumers

The avro-wrapper addresses this need for dynamism in AVRO schema without having to rebuild code for change in data-format! 

## Core Concepts
The core concepts of the solution are: 
- A `SchemaRegistry` is a utility that helps store and manage many AVRO schemas. 
- A `DataGetter` is a user-provided callable/invocable that fetches & returns the data for a particular field
- A `DataGetterDictionary` is a user-provided look-up of a DataGetter for a field name, as defined by the AVRO Schema
- `AVROEncoder` is the core logic that automates encoding data as per the AVRO Schema using the DataGetterDictionary. User implementations will only use this. 

## External Dependencies
Uses apache/avro for AVRO encoding facilities. 

## Code Elements
### `AVROEncoder`
### `DataGetter`
### `DataGetterDictionary`
### `ServiceRegistry`
