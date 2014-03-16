/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"
#include "person.pb-c.h"
#include "protobuf2json.h"

TEST_IMPL(protobuf2json_string__required) {
  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string = protobuf2json_string(&person.base, 0);
  ASSERT(json_string);

  ASSERT_STRCMP(json_string, "{\"name\": \"John Doe\", \"id\": 42}");

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__optional) {
  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  char *json_string = protobuf2json_string(&person.base, 0);
  ASSERT(json_string);

  ASSERT_STRCMP(json_string, "{\"name\": \"John Doe\", \"id\": 42, \"email\": \"john@doe.name\"}");

  RETURN_OK();
}
