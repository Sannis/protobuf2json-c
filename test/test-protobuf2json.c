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

TEST_IMPL(protobuf2json_string__repeated_message) {
  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  Foo__Person__PhoneNumber person_phonenumber1 = FOO__PERSON__PHONE_NUMBER__INIT;

  person_phonenumber1.number = "+123456789";
  person_phonenumber1.has_type = 1;
  person_phonenumber1.type = FOO__PERSON__PHONE_TYPE__WORK;

  Foo__Person__PhoneNumber person_phonenumber2 = FOO__PERSON__PHONE_NUMBER__INIT;

  person_phonenumber2.number = "+987654321";
  person_phonenumber2.has_type = 1;
  person_phonenumber2.type = FOO__PERSON__PHONE_TYPE__MOBILE;

  person.n_phone = 2;
  person.phone = calloc(person.n_phone, sizeof(Foo__Person__PhoneNumber*));
  ASSERT(person.phone);

  person.phone[0] = (Foo__Person__PhoneNumber*)&person_phonenumber1;
  person.phone[1] = (Foo__Person__PhoneNumber*)&person_phonenumber2;

  char *json_string = protobuf2json_string(&person.base, 0);
  ASSERT(json_string);

  ASSERT_STRCMP(json_string, "{\"name\": \"John Doe\", \"id\": 42, \"phone\": [{\"number\": \"+123456789\", \"type\": \"WORK\"}, {\"number\": \"+987654321\", \"type\": \"MOBILE\"}]}");

  RETURN_OK();
}
