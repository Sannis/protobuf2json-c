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

#include <libgen.h> /* dirname */

char executable_path[MAXPATHLEN] = {0};

void oneof(void) {
  int result;

  Foo__Something something = FOO__SOMETHING__INIT;

  char *json_string;

  // FOO__SOMETHING__SOMETHING__NOT_SET
  something.something_case = FOO__SOMETHING__SOMETHING__NOT_SET;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);

  printf("Debug: %s\n", json_string);
  free(json_string);

  // FOO__SOMETHING__SOMETHING_ONEOF_STRING
  something.oneof_string = "string";
  something.something_case = FOO__SOMETHING__SOMETHING_ONEOF_STRING;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);

  printf("Debug: %s\n", json_string);
  free(json_string);

  // FOO__SOMETHING__SOMETHING_ONEOF_BYTES
  something.oneof_bytes.len = 5;
  something.oneof_bytes.data = (uint8_t*)"bytes";
  something.something_case = FOO__SOMETHING__SOMETHING_ONEOF_BYTES;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);

  printf("Debug: %s\n", json_string);
  free(json_string);
}

void person__debug(void) {
  int result;

  const char *initial_json_string = \
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
    "      \"number\": \"+555555555\"\n"
    "    }\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__Person *person = (Foo__Person *)protobuf_message;

  //asm volatile ("int $3");

  printf("Debug: person->id: %d\n", person->id);
  printf("Debug: person->name: %s\n", person->name);
  printf("Debug: person->n_phone: %zu\n", person->n_phone);
  printf("Debug: person->phone[0]->number: %s\n", person->phone[0]->number);
  printf("Debug: person->phone[0]->type: %d\n", person->phone[0]->type);
  printf("Debug: person->phone[1]->number: %s\n", person->phone[1]->number);
  printf("Debug: person->phone[1]->type: %d\n", person->phone[1]->type);
  printf("Debug: person->phone[2]->number: %s\n", person->phone[2]->number);
  printf("Debug: person->phone[2]->type: %d\n", person->phone[2]->type);

  ASSERT(person->id == 42);
  ASSERT_STRCMP(person->name, "John Doe");

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
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
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  printf("Debug: %s OK\n", __FUNCTION__);
}

void person__error_unknown_enum_value(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [\n"
    "    {\n"
    "      \"number\": \"+123456789\",\n"
    "      \"type\": \"UNKNOWN\"\n"
    "    }\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE);

  const char *expected_error_string = \
    "Unknown value 'UNKNOWN' for enum 'Foo.Person.PhoneType'"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  printf("Debug: %s OK\n", __FUNCTION__);
}

void person__bad_json_string(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = "...";

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

  const char *expected_error_string = \
    "JSON parsing error at line 1 column 1 (position 1): "
    "'[' or '{' expected near '.'"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  printf("Debug: %s OK\n", __FUNCTION__);
}

void person__error_is_not_array(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"phone\": {}\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_ARRAY);

  const char *expected_error_string = \
    "JSON is not an array required for repeatable GPB field"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  printf("Debug: %s OK\n", __FUNCTION__);
}

void person__error_unknown_field(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"unknown_field\": \"unknown value\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_FIELD);

  const char *expected_error_string = \
    "Unknown field 'unknown_field' for message 'Foo.Person'"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  printf("Debug: %s OK\n", __FUNCTION__);
}

void person__error_unknown_field_nested(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [\n"
    "    {\n"
    "      \"unknown_field\": \"unknown value\"\n"
    "    }\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_FIELD);

  const char *expected_error_string = \
    "Unknown field 'unknown_field' for message 'Foo.Person.PhoneNumber'"
  ;

  printf("Error: %s\n", error_string);

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  printf("Debug: %s OK\n", __FUNCTION__);
}

void repeated_values__error_is_not_string_required_for_string(void) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_string\": [\"42\", 42]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  //asm volatile ("int $3");

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_STRING);

  const char *expected_error_string = \
    "JSON value is not a string required for GPB string"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  printf("Debug: %s OK\n", __FUNCTION__);
}

void read_file_success(void) {
  int result;

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/good.json", file_path);
  ASSERT(result > 0);

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_file((char *)file_name, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
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

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  printf("Debug: %s OK\n", __FUNCTION__);
}

void bytes_encode(void) {
  int result;

  Foo__Something something = FOO__SOMETHING__INIT;

  char *json_string;

  // FOO__SOMETHING__SOMETHING_ONEOF_BYTES
  something.oneof_bytes.len = 1;
  something.oneof_bytes.data = (uint8_t*)"\0";
  something.something_case = FOO__SOMETHING__SOMETHING_ONEOF_BYTES;
  result = protobuf2json_string(&something.base, TEST_JSON_FLAGS, &json_string, NULL, 0);

  printf("Debug: %s\n", json_string);
  free(json_string);
}

int main(int argc, char **argv) {
  strncpy(executable_path, argv[0], sizeof(executable_path) - 1);

  oneof();

  person__debug();

  person__error_unknown_enum_value();

  person__bad_json_string();

  person__error_is_not_array();

  person__error_unknown_field();

  person__error_unknown_field_nested();

  repeated_values__error_is_not_string_required_for_string();

  read_file_success();

  bytes_encode();

  return 0;
}
