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

#ifndef json_boolean
#define json_boolean(val)      ((val) ? json_true() : json_false())
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* === Protobuf -> JSON === */

int protobuf2json_object(ProtobufCMessage *protobuf_message, json_t **json);

char* protobuf2json_string(ProtobufCMessage *protobuf_message, size_t flags);

/* === JSON -> Protobuf === */

int json2protobuf(json_t *json, ProtobufCMessage *protobuf_message);

/* === END === */

#ifdef __cplusplus
}
#endif

#endif
