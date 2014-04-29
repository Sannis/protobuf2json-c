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
  result = protobuf2json_string(&bar.base, JSON_INDENT(2) | JSON_PRESERVE_ORDER, &json_string, NULL, 0);
  ASSERT(result == 0);
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

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__numeric_types__values) {
  int result;

  Foo__NumericTypes numeric_types = FOO__NUMERIC_TYPES__INIT;

  numeric_types.has_value_int32 = 1;
  numeric_types.value_int32 = 2147483647;
  numeric_types.has_value_sint32 = 1;
  numeric_types.value_sint32 = -2147483647 - 1;
  numeric_types.has_value_sfixed32 = 1;
  numeric_types.value_sfixed32 = -2147483647 - 1;
  numeric_types.has_value_uint32 = 1;
  numeric_types.value_uint32 = 4294967295;
  numeric_types.has_value_fixed32 = 1;
  numeric_types.value_fixed32 = 4294967295;

  numeric_types.has_value_int64 = 1;
  numeric_types.value_int64 = 9223372036854775807;
  numeric_types.has_value_sint64 = 1;
  numeric_types.value_sint64 = -9223372036854775807 - 1;
  numeric_types.has_value_sfixed64 = 1;
  numeric_types.value_sfixed64 = -9223372036854775807 - 1;
  // JSON does not support max(unsigned long long)
  numeric_types.has_value_uint64 = 1;
  numeric_types.value_uint64 = 9223372036854775807;
  numeric_types.has_value_fixed64 = 1;
  numeric_types.value_fixed64 = 9223372036854775807;

  numeric_types.has_value_float = 1;
  numeric_types.value_float = 0.33;
  numeric_types.has_value_double = 1;
  numeric_types.value_double = 0.00777055503330111;

  char *json_string;
  result = protobuf2json_string(&numeric_types.base, JSON_INDENT(2) | JSON_PRESERVE_ORDER, &json_string, NULL, 0);
  ASSERT(result == 0);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"value_int32\": 2147483647,\n"
    "  \"value_sint32\": -2147483648,\n"
    "  \"value_sfixed32\": -2147483648,\n"
    "  \"value_uint32\": 4294967295,\n"\
    "  \"value_fixed32\": 4294967295,\n"
    "  \"value_int64\": 9223372036854775807,\n"
    "  \"value_sint64\": -9223372036854775808,\n"
    "  \"value_sfixed64\": -9223372036854775808,\n"
    "  \"value_uint64\": 9223372036854775807,\n"
    "  \"value_fixed64\": 9223372036854775807,\n"
    "  \"value_float\": 0.33000001311302185,\n"
    "  \"value_double\": 0.0077705550333011103\n"
    "}"
  );

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
  result = protobuf2json_string(&person.base, JSON_INDENT(2), &json_string, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE);

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
