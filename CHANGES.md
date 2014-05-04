protobuf2json-c releases
========================

v0.1.1 - 04 May 2014
-------------------

 * Fixes:

   - protobuf2json: remove unnecessary json_decref
   - json2protobuf: fix boolean values converting
   - test: allocate test file path in heap

 * Other:

   - build: sanitizing integer errors
   - test: free json_string and protobuf_message
   - test: use TEST_JSON_FLAGS in all positive tests


v0.1.0 - 02 May 2014
--------------------

 * New features:

   - json2protobuf: support for all numeric types and boolean

 * Other:

   - api: change error constants values


v0.0.5 - 01 May 2014
--------------------

 * New features:

   - protobuf2json: add missed error string setting for unsupported field type

 * Other:

   - api: change error constants values
   - test: cover numeric types GPB -> JSON converting


v0.0.4 - 29 Apr 2014
--------------------

 * New features:

   - protobuf2json: errors reporting, cover all memory and jansson errors

 * Other:

   - build: fix external build (#1) (Antony Dovgal)


v0.0.3 - 15 Apr 2014
--------------------

 * New features:

   - json2protobuf: JSON parsing errors reporting
   - json2protobuf: calloc(3) errors reporting
   - json2protobuf: validate JSON structure while process message
   - json2protobuf: pass errors from nested messages processing

 * Other:

   - api: change error constants values
   - build: rename libprotobuf2json to libprotobuf2json-c


v0.0.2 - 06 Apr 2014
--------------------

 * Initial version:

   - protobuf2json: define public API & simple messages support
   - json2protobuf: define public API & simple messages support
   - build: adapt libuv test runner
   - build: use autoconf
   - build: implement AX_SANITIZE macro
   - build: jansson_compat.h for old jansson
