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

TEST_IMPL(protobuf2json) {
  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  printf("1\n");

  char *json_string = protobuf2json_string(&person.base, 0);
  ASSERT(json_string);

  printf("2\n");

  printf("\nJSON: %s\n\n", json_string);

  printf("3\n");

  RETURN_OK();
}
