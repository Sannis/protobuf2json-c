/*
 * Copyright (c) 2014-2016 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include "task.h"
#include "test.pb-c.h"
#include "protobuf2json.h"

#include "failed-alloc-helper.h"

TEST_IMPL(protobuf2json_string__required_field) {
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
    "  \"id\": 42,\n"
    "  \"phone\": []\n"
    "}"
  );

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__optional_field) {
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
    "  \"email\": \"john@doe.name\",\n"
    "  \"phone\": []\n"
    "}"
  );

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__default_values) {
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
    "  \"bytes_optional_default\": \"ZGVmYXVsdCB2YWx1ZSAz\",\n"
    "  \"enum_optional_default\": \"FIZZBUZZ\"\n"
    "}"
  );

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__oneof) {
  int result;

  Foo__Something something = FOO__SOMETHING__INIT;

  char *json_string;

  // FOO__SOMETHING__SOMETHING_ONEOF_STRING but not set
  something.something_case = FOO__SOMETHING__SOMETHING_ONEOF_STRING;
  something.oneof_string = NULL;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{}"
  );

  free(json_string);

  // FOO__SOMETHING__SOMETHING__NOT_SET
  something.something_case = FOO__SOMETHING__SOMETHING__NOT_SET;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{}"
  );

  free(json_string);

  // FOO__SOMETHING__SOMETHING_ONEOF_STRING
  something.something_case = FOO__SOMETHING__SOMETHING_ONEOF_STRING;
  something.oneof_string = "string";
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"oneof_string\": \"string\"\n"
    "}"
  );

  free(json_string);

  // FOO__SOMETHING__SOMETHING_ONEOF_BYTES
  something.oneof_bytes.len = 5;
  something.oneof_bytes.data = (uint8_t*)"bytes";
  something.something_case = FOO__SOMETHING__SOMETHING_ONEOF_BYTES;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  ASSERT_STRCMP(
    json_string,
    "{\n"
    "  \"oneof_bytes\": \"Ynl0ZXM=\"\n"
    "}"
  );

  free(json_string);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_in_nested_message) {
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

  free(person.phone);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_cannot_create_json_object) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string;
  failed_alloc_json_set_by_count(1);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY);

  const char *expected_error_string = \
    "Cannot allocate JSON structure using json_object()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_cannot_create_json_value) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string;
  failed_alloc_json_set_by_count(2);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY);

  const char *expected_error_string = \
    "Cannot allocate JSON structure in protobuf2json_process_field()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_in_json_object_set_new_1) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  char *json_string;
  failed_alloc_json_set_by_count(4);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_JANSSON_INTERNAL);

  const char *expected_error_string = \
    "Error in json_object_set_new()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_in_json_object_set_new_2) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  char *json_string;
  failed_alloc_json_set_by_count(9);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_JANSSON_INTERNAL);

  const char *expected_error_string = \
    "Error in json_object_set_new()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_cannot_create_json_array) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  person.n_phone = 1;

  char *json_string;
  failed_alloc_json_set_by_count(10);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_ALLOCATE_MEMORY);

  const char *expected_error_string = \
    "Cannot allocate JSON structure using json_array()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_in_json_object_set_new_3) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  person.email = "john@doe.name";

  Foo__Person__PhoneNumber person_phonenumber1 = FOO__PERSON__PHONE_NUMBER__INIT;

  person_phonenumber1.number = "+123456789";

  person.n_phone = 1;
  person.phone = calloc(person.n_phone, sizeof(Foo__Person__PhoneNumber*));
  ASSERT(person.phone);

  person.phone[0] = (Foo__Person__PhoneNumber*)&person_phonenumber1;

  char *json_string;
  failed_alloc_json_set_by_count(20);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_JANSSON_INTERNAL);

  const char *expected_error_string = \
    "Error in json_object_set_new()"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  free(person.phone);

  RETURN_OK();
}

TEST_IMPL(protobuf2json_string__error_cannot_dump_string) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  char *json_string;
  failed_alloc_json_set_by_size(16 /* STRBUFFER_MIN_SIZE */);
  result = protobuf2json_string(&person.base, 0, &json_string, error_string, sizeof(error_string));
  failed_alloc_json_unset();
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
