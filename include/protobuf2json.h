/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#ifndef PROTOBUF2JSON_H
#define PROTOBUF2JSON_H 1

#include <google/protobuf-c/protobuf-c.h>
#include <jansson.h>

#define PROTOBUF2JSON_ERR_UNSUPPORTED_FIELD_TYPE -001
#define PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY -002
#define PROTOBUF2JSON_ERR_JANSSON_INTERNAL       -003

#define PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING     -101
#define PROTOBUF2JSON_ERR_CANNOT_PROCESS_MESSAGE -102

#define PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE      -301
#define PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING    -302

#define PROTOBUF2JSON_ERR_IS_NOT_OBJECT          -401
#define PROTOBUF2JSON_ERR_IS_NOT_ARRAY           -402
#define PROTOBUF2JSON_ERR_IS_NOT_STRING          -403
#define PROTOBUF2JSON_ERR_IS_NOT_INTEGER         -404
#define PROTOBUF2JSON_ERR_UNKNOWN_FIELD          -405
#define PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE     -406

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
  size_t flags,
  char **json_string,
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
  size_t flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message,
  char *error_string,
  size_t error_size
);

int json2protobuf_file(
  char *json_file,
  size_t flags,
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
