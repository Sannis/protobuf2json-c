protobuf2json-c releases
========================

master - UNRELEASED
-------------------

  Nothing

v0.4.0 - 28 Nov 2016
--------------------

 * New features

   - protobuf2json: bytes fields now encoded with base64

v0.3.1 - 07 Feb 2016
--------------------

 * Fixes:

   - protobuf2json.h installation


v0.3.0 - 07 Feb 2016
--------------------

 * New features

   - protobuf2json: support oneof

 * Fixes:

   - protobuf2json: few memory leaks fixed

 * Other

   - build: add Travis OS X tests


v0.2.1 - 05 Jan 2015
--------------------

 * New features

   - protobuf2json: allow integers in JSON for float/double protobuf fields

 * Other

   - build: implement valgrind check
   - test: add tests for protobuf2json_file function fails
   - other: fix warnings that can be muted with -Wpointer-sign
   - test: fix coverage info generation on OS X
   - test: better coverage for repeated values support


v0.2.0 - 14 Dec 2014
--------------------

 * Incompatible changes

   - build: remove jansson_compat.h, require latest jansson version (2.7)

 * New features:

   - protobuf2json: implement protobuf2json_file() function
   - protobuf2json: implement bytes type support
   - protobuf2json: implement presented required fields check

 * Fixes:

   - protobuf2json: fix null pointer dereference reported by cppcheck
   - protobuf2json: fix memory leaks reported by valgrind
   - test: cover more types mismatch error messages, correct some error codes

 * Other

   - docs: API documentation
   - build: specify -version-info for libtool
   - build: run cppcheck in builds
   - test: get rid of protobuf_c_default_allocator usage
   - build: remove allow_failures from Travis CI config
   - build: add coveralls support


v0.1.1 - 04 May 2014
--------------------

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
