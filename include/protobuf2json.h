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

#define PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING     -101
#define PROTOBUF2JSON_ERR_CANNOT_PROCESS_MESSAGE -102

#define PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE      -201
#define PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING    -202
#define PROTOBUF2JSON_ERR_IS_NOT_OBJECT          -203

#ifndef json_boolean
#define json_boolean(val)      ((val) ? json_true() : json_false())
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* === Protobuf -> JSON === */

int protobuf2json_object(ProtobufCMessage *protobuf_message, json_t **json_object);

int protobuf2json_string(ProtobufCMessage *protobuf_message, size_t flags, char **json_string);

/* === JSON -> Protobuf === */

int json2protobuf_object(
  json_t *json_object,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
);

int json2protobuf_string(
  char *json_string,
  size_t flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
  /*, json_error_t *json_error*/
);

int json2protobuf_file(
  char *json_file,
  size_t flags,
  const ProtobufCMessageDescriptor *protobuf_message_descriptor,
  ProtobufCMessage **protobuf_message
  /*, json_error_t *json_error*/
);

/* === END === */

#ifdef __cplusplus
}
#endif

#endif
