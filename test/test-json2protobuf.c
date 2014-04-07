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

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == 0);

  Foo__Person *person = (Foo__Person *)protobuf_message;
  ASSERT(person->id == 42);
  ASSERT(person->name);
  ASSERT_STRCMP(person->name, "John Doe");
  ASSERT(!person->email);

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__optional) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"email\": \"john@doe.name\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == 0);

  Foo__Person *person = (Foo__Person *)protobuf_message;
  ASSERT(person->id == 42);
  ASSERT(person->name);
  ASSERT_STRCMP(person->name, "John Doe");
  ASSERT(person->email);
  ASSERT_STRCMP(person->email, "john@doe.name");

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"email\": \"john@doe.name\"\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__repeated_message) {
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

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == 0);

  Foo__Person *person = (Foo__Person *)protobuf_message;
  ASSERT(person->id == 42);
  ASSERT(person->name);
  ASSERT_STRCMP(person->name, "John Doe");

  ASSERT(person->n_phone == 3);

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__unknown_field) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"unknown\": 42\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == PROTOBUF2JSON_ERR_UNKNOWN_FIELD);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__bar__default_values) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"string_required\": \"required\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, NULL, 0);
  ASSERT(result == 0);

  Foo__Bar *bar = (Foo__Bar *)protobuf_message;
  ASSERT_STRCMP(bar->string_required, "required");

  char *json_string;
  result = protobuf2json_string(protobuf_message, JSON_INDENT(2), &json_string);
  ASSERT(result == 0);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"string_required\": \"required\",\n"
    "  \"string_required_default\": \"default value 1\",\n"
    "  \"enum_optional_default\": \"FIZZBUZZ\",\n"
    "  \"string_optional_default\": \"default value 2\"\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_cannot_parse_string) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = "...";

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

  const char *expected_error_string = \
    "JSON parsing error at line 1 column 1 (position 1): "
    "'[' or '{' expected near '.'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_is_not_object) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = "[]";

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_IS_NOT_OBJECT);

  const char *expected_error_string = \
    "JSON is not an object required for GPB message"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_unknown_field) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"unknown_filed\": \"unknown_filed_value\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_UNKNOWN_FIELD);

  const char *expected_error_string = \
    "Unknown field 'unknown_filed' for message 'Foo.Bar'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_is_not_array) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"phone\": {}\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT(result == PROTOBUF2JSON_ERR_IS_NOT_ARRAY);

  const char *expected_error_string = \
    "JSON is not an array required for repeatable GPB field"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}
