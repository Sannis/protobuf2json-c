/*
 * Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef PROTOBUF2JSON_H
#define PROTOBUF2JSON_H 1

#include <google/protobuf-c/protobuf-c.h>
#include <jansson.h>

/* Common errors */
#define PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY -001
#define PROTOBUF2JSON_ERR_UNSUPPORTED_FIELD_TYPE -002
#define PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE     -003

/* protobuf2json_string */
#define PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING     -101
/* protobuf2json_file */
#define PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE       -102
/* protobuf2json */
#define PROTOBUF2JSON_ERR_JANSSON_INTERNAL       -201

/*json2protobuf_string*/
#define PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING    -301
/* json2protobuf_file */
#define PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE      -302
/* json2protobuf */
#define PROTOBUF2JSON_ERR_UNKNOWN_FIELD          -401
#define PROTOBUF2JSON_ERR_IS_NOT_OBJECT          -402
#define PROTOBUF2JSON_ERR_IS_NOT_ARRAY           -403
#define PROTOBUF2JSON_ERR_IS_NOT_INTEGER         -404
#define PROTOBUF2JSON_ERR_IS_NOT_INTEGER_OR_REAL -405
#define PROTOBUF2JSON_ERR_IS_NOT_BOOLEAN         -406
#define PROTOBUF2JSON_ERR_IS_NOT_STRING          -407
#define PROTOBUF2JSON_ERR_REQUIRED_IS_MISSING    -408
/*#define PROTOBUF2JSON_ERR_DUPLICATE_FIELD      -???*/

#ifdef __cplusplus
extern "C" {
#endif

/* === Protobuf -> JSON === */

int protobuf2json_object(
  ProtobufCMessage *protobuf_message,
  json_t **json_object,
  char *error_string,
  size_t error_size
);

int protobuf2json_string(
  ProtobufCMessage *protobuf_message,
  size_t json_flags,
  char **json_string,
  char *error_string,
  size_t error_size
);

int protobuf2json_file(
  ProtobufCMessage *protobuf_message,
  size_t json_flags,
  char *json_file,
  char *fopen_mode,
  char *error_string,
  size_t error_size
);

/* === JSON -> Protobuf === */

int json2protobuf_object(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
);

int json2protobuf_string(
  char *json_string,
  size_t json_flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
);

int json2protobuf_file(
  char *json_file,
  size_t json_flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
);

/* === END === */

#ifdef __cplusplus
}
#endif

#endif /* PROTOBUF2JSON_H */
