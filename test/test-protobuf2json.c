/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include <libgen.h>
#include <stdio.h>
#include <unistd.h>

#include "task.h"
#include "person.pb-c.h"
#include "test.pb-c.h"
#include "protobuf2json.h"

extern char executable_path[MAXPATHLEN];

TEST_IMPL(protobuf2json_file__person__success) {
  int result;

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};
  long json_string_length = 0;

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/existent.json", file_path);
  ASSERT(result > 0);

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  result = protobuf2json_file(&person.base, TEST_JSON_FLAGS, file_name, "w", NULL, 0);
  ASSERT_ZERO(result);

  FILE *fd = fopen(file_name, "r");
  ASSERT(fd);

  result = fseek(fd, 0, SEEK_END);
  ASSERT_ZERO(result);
  json_string_length = ftell(fd);
  ASSERT(json_string_length > 0);
  rewind(fd);

  char *json_string = (char *)calloc(sizeof(char), json_string_length + 1);
  ASSERT(json_string);

  result = fread(json_string, 1, json_string_length, fd);
  ASSERT(result == json_string_length);
  json_string[json_string_length] = '\0';

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42\n"
    "}"
  );

  free(json_string);
  fclose(fd);

  result = unlink(file_name);
  ASSERT_ZERO(result);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__person__required) {
  int result;

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string;
  result = protobuf2json_string(&person.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42\n"
    "}"
  );

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__person__optional) {
  int result;
  
  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  char *json_string;
  result = protobuf2json_string(&person.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"email\": \"john@doe.name\"\n"
    "}"
  );

  free(json_string);

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
  result = protobuf2json_string(&person.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
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

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__bar__default_values) {
  int result;

  Foo__Bar bar = FOO__BAR__INIT;

  bar.string_required = "required";

  char *json_string;
  result = protobuf2json_string(&bar.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"string_required\": \"required\",\n"
    "  \"string_required_default\": \"default value 1\",\n"
    "  \"string_optional_default\": \"default value 2\",\n"
    "  \"enum_optional_default\": \"FIZZBUZZ\"\n"
    "}"
  );

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__person__error_in_nested_message) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  Foo__Person__PhoneNumber person_phonenumber1 = FOO__PERSON__PHONE_NUMBER__INIT;

  person_phonenumber1.number = "+123456789";
  person_phonenumber1.has_type = 1;
  person_phonenumber1.type = 777; // Unknown enum value

  person.n_phone = 1;
  person.phone = calloc(person.n_phone, sizeof(Foo__Person__PhoneNumber*));
  ASSERT(person.phone);

  person.phone[0] = (Foo__Person__PhoneNumber*)&person_phonenumber1;

  char *json_string;
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE);

  const char *expected_error_string = \
    "Unknown value 777 for enum 'Foo.Person.PhoneType'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
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
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  json_set_alloc_funcs(malloc, free);
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_DUMP_STRING);

  const char *expected_error_string = \
    "Cannot dump JSON object to string using json_dumps()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}
