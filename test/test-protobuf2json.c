/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"
#include "person.pb-c.h"
#include "test.pb-c.h"
#include "protobuf2json.h"

TEST_IMPL(protobuf2json_string__person__required) {
  int result;

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string;
  result = protobuf2json_string(&person.base, JSON_INDENT(2), &json_string, NULL, 0);
  ASSERT(result == 0);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42\n"
    "}"
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__person__optional) {
  int result;
  
  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  char *json_string;
  result = protobuf2json_string(&person.base, JSON_INDENT(2), &json_string, NULL, 0);
  ASSERT(result == 0);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"email\": \"john@doe.name\"\n"
    "}"
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__person__repeated_message) {
  int result;

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

  Foo__Person__PhoneNumber person_phonenumber3 = FOO__PERSON__PHONE_NUMBER__INIT;

  person_phonenumber3.number = "+555555555";

  person.n_phone = 3;
  person.phone = calloc(person.n_phone, sizeof(Foo__Person__PhoneNumber*));
  ASSERT(person.phone);

  person.phone[0] = (Foo__Person__PhoneNumber*)&person_phonenumber1;
  person.phone[1] = (Foo__Person__PhoneNumber*)&person_phonenumber2;
  person.phone[2] = (Foo__Person__PhoneNumber*)&person_phonenumber3;

  char *json_string;
  result = protobuf2json_string(&person.base, JSON_INDENT(2), &json_string, NULL, 0);
  ASSERT(result == 0);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [\n"
    "    {\n"
    "      \"number\": \"+123456789\",\n"
    "      \"type\": \"WORK\"\n"
    "    },\n"
    "    {\n"
    "      \"number\": \"+987654321\",\n"
    "      \"type\": \"MOBILE\"\n"
    "    },\n"
    "    {\n"
    "      \"number\": \"+555555555\",\n"
    "      \"type\": \"HOME\"\n"
    "    }\n"
    "  ]\n"
    "}"
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__bar__default_values) {
  int result;

  Foo__Bar bar = FOO__BAR__INIT;

  bar.string_required = "required";

  char *json_string;
  result = protobuf2json_string(&bar.base, JSON_INDENT(2), &json_string, NULL, 0);
  ASSERT(result == 0);
  ASSERT(json_string);

  /* Notice strange fields order here */
  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"string_required\": \"required\",\n"
    "  \"string_required_default\": \"default value 1\",\n"
    "  \"enum_optional_default\": \"FIZZBUZZ\",\n"
    "  \"string_optional_default\": \"default value 2\"\n"
    "}"
  );

  RETURN_OK();
}

#define STRBUFFER_MIN_SIZE 16
void* failed_malloc(size_t size) {
  return (size == STRBUFFER_MIN_SIZE) ? NULL : malloc(size);
}

TEST_IMPL(protobuf2json_string__person__error_cannot_dump_string) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string;
  json_set_alloc_funcs(failed_malloc, free);
  result = protobuf2json_string(&person.base, JSON_INDENT(2), &json_string, error_string, sizeof(error_string));
  json_set_alloc_funcs(malloc, free);
  ASSERT(result == PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING);

  const char *expected_error_string = \
    "Cannot dump JSON object to string using json_dumps()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}
