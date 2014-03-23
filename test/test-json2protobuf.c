/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"
#include "person.pb-c.h"
#include "bar.pb-c.h"
#include "protobuf2json.h"

TEST_IMPL(json2protobuf_string__person__required) {
  int result;

  const char *expected_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)expected_json_string, 0, &foo__person__descriptor, &protobuf_message);
  ASSERT(result == 0);

  Foo__Person *person = (Foo__Person *)protobuf_message;
  ASSERT(person->id == 42);
  ASSERT_STRCMP(person->name, "John Doe");

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  RETURN_OK();
}
