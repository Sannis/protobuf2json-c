/*
 * Copyright (c) 2014 Oleg Efimov <efimovov@gmail.com>
 *
 * protobuf2json-c is free software; you can redistribute it
 * and/or modify it under the terms of the MIT license.
 * See LICENSE for details.
 */

#include <libgen.h>
#include <math.h>

#include "task.h"
#include "person.pb-c.h"
#include "test.pb-c.h"
#include "protobuf2json.h"

extern char executable_path[MAXPATHLEN];

TEST_IMPL(json2protobuf_file__person__success) {
  int result;
  char error_string[256] = {0};

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/good.json", file_path);
  ASSERT(result > 0);

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_file((char *)file_name, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_ZERO(result);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
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

TEST_IMPL(json2protobuf_file__person__error_cannot_parse_bad_message) {
  int result;
  char error_string[256] = {0};

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/bad_message.json", file_path);
  ASSERT(result > 0);

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_file((char *)file_name, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_FIELD);

  const char *expected_error_string = \
    "Unknown field 'unknown_field' for message 'Foo.Person'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_file__person__error_cannot_parse_bad_json) {
  int result;
  char error_string[256] = {0};

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/bad_json.json", file_path);
  ASSERT(result > 0);

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_file((char *)file_name, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE);

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

TEST_IMPL(json2protobuf_file__person__error_cannot_parse_unexistent_file) {
  int result;
  char error_string[256] = {0};

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/unexistent.json", file_path);
  ASSERT(result > 0);

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_file((char *)file_name, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_FILE);

  const char *expected_error_string_beginning = \
    "JSON parsing error at line -1 column -1 (position 0): "
    "unable to open"
  ;

  ASSERT(strstr(error_string, expected_error_string_beginning) == error_string);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_cannot_parse_wrong_string) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = "...";

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, TEST_JSON_FLAGS, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

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

TEST_IMPL(json2protobuf_string__person__error_duplicate_field) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"name\": \"Jack Impostor\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, JSON_REJECT_DUPLICATES, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

  const char *expected_error_string = \
    "JSON parsing error at line 4 column 8 (position 44): "
    "duplicate object key near '\"name\"'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"email\": \"john@doe.name\",\n"
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
  ASSERT_ZERO(result);

  Foo__Person *person = (Foo__Person *)protobuf_message;

  ASSERT(person->id == 42);
  ASSERT(person->name);
  ASSERT_STRCMP(person->name, "John Doe");
  ASSERT(person->email);
  ASSERT_STRCMP(person->email, "john@doe.name");

  ASSERT(person->n_phone == 3);

  ASSERT(person->phone[1]->number);
  ASSERT_STRCMP(person->phone[1]->number, "+987654321");
  ASSERT(person->phone[1]->type == FOO__PERSON__PHONE_TYPE__MOBILE);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"email\": \"john@doe.name\",\n"
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
  ASSERT_ZERO(result);

  Foo__Bar *bar = (Foo__Bar *)protobuf_message;
  ASSERT_STRCMP(bar->string_required, "required");

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"string_required\": \"required\",\n"
    "  \"string_required_default\": \"default value 1\",\n"
    "  \"string_optional_default\": \"default value 2\",\n"
    "  \"enum_optional_default\": \"FIZZBUZZ\"\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__repeated_values__values) {
  int result;

  const char *initial_json_string = \
    "{\n"

    "  \"value_int32\": [2147483647, 0],\n"
    "  \"value_sint32\": [-2147483648, 0],\n"
    "  \"value_sfixed32\": [-2147483648, 0],\n"

    "  \"value_uint32\": [4294967295, 0],\n"\
    "  \"value_fixed32\": [4294967295, 0],\n"

    "  \"value_int64\": [9223372036854775807, 0],\n"
    "  \"value_sint64\": [-9223372036854775808, 0],\n"
    "  \"value_sfixed64\": [-9223372036854775808, 0],\n"

    /* JSON does not support max(unsigned long long) */
    "  \"value_uint64\": [9223372036854775807, 0],\n"
    "  \"value_fixed64\": [9223372036854775807, 0],\n"

    "  \"value_float\": [0.33000001311302185, 0],\n"
    "  \"value_double\": [0.0077705550333011103, 0],\n"

    "  \"value_bool\": [true, false, false]\n"

    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__RepeatedValues *repeated_values = (Foo__RepeatedValues *)protobuf_message;

  ASSERT(repeated_values->n_value_int32 == 2);
  ASSERT(repeated_values->value_int32[0] == 2147483647);
  ASSERT(repeated_values->value_int32[1] == 0);

  ASSERT(repeated_values->n_value_sint32 == 2);
  ASSERT(repeated_values->value_sint32[0] == -2147483647 - 1);
  ASSERT(repeated_values->value_sint32[1] == 0);

  ASSERT(repeated_values->n_value_sfixed32 == 2);
  ASSERT(repeated_values->value_sfixed32[0] == -2147483647 - 1);
  ASSERT(repeated_values->value_sfixed32[1] == 0);

  ASSERT(repeated_values->n_value_uint32 == 2);
  ASSERT(repeated_values->value_uint32[0] == 4294967295);
  ASSERT(repeated_values->value_uint32[1] == 0);

  ASSERT(repeated_values->n_value_fixed32 == 2);
  ASSERT(repeated_values->value_fixed32[0] == 4294967295);
  ASSERT(repeated_values->value_fixed32[1] == 0);

  ASSERT(repeated_values->n_value_int64 == 2);
  ASSERT(repeated_values->value_int64[0] == 9223372036854775807);
  ASSERT(repeated_values->value_int64[1] == 0);

  ASSERT(repeated_values->n_value_sint64 == 2);
  ASSERT(repeated_values->value_sint64[0] == -9223372036854775807 - 1);
  ASSERT(repeated_values->value_sint64[1] == 0);

  ASSERT(repeated_values->n_value_sfixed64 == 2);
  ASSERT(repeated_values->value_sfixed64[0] == -9223372036854775807 - 1);
  ASSERT(repeated_values->value_sfixed64[1] == 0);

  /* JSON does not support max(unsigned long long) */
  ASSERT(repeated_values->n_value_uint64 == 2);
  ASSERT(repeated_values->value_uint64[0] == 9223372036854775807);
  ASSERT(repeated_values->value_uint64[1] == 0);

  ASSERT(repeated_values->n_value_fixed64 == 2);
  ASSERT(repeated_values->value_fixed64[0] == 9223372036854775807);
  ASSERT(repeated_values->value_fixed64[1] == 0);

  ASSERT(repeated_values->n_value_float == 2);
  ASSERT(fabs(repeated_values->value_float[0] - 0.33000001311302185) < 1e-10);
  ASSERT(fabs(repeated_values->value_float[1]) < 1e-10);

  ASSERT(repeated_values->n_value_double == 2);
  ASSERT(fabs(repeated_values->value_double[0] - 0.0077705550333011103) < 1e-10);
  ASSERT(fabs(repeated_values->value_double[1]) < 1e-10);

  ASSERT(repeated_values->n_value_bool == 3);
  ASSERT(repeated_values->value_bool[0]);
  ASSERT(!repeated_values->value_bool[1]);
  ASSERT(!repeated_values->value_bool[2]);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"

    "  \"value_int32\": [\n"
    "    2147483647,\n"
    "    0\n"
    "  ],\n"
    "  \"value_sint32\": [\n"
    "    -2147483648,\n"
    "    0\n"
    "  ],\n"
    "  \"value_sfixed32\": [\n"
    "    -2147483648,\n"
    "    0\n"
    "  ],\n"

    "  \"value_uint32\": [\n"
    "    4294967295,\n"
    "    0\n"
    "  ],\n"\
    "  \"value_fixed32\": [\n"
    "    4294967295,\n"
    "    0\n"
    "  ],\n"

    "  \"value_int64\": [\n"
    "    9223372036854775807,\n"
    "    0\n"
    "  ],\n"
    "  \"value_sint64\": [\n"
    "    -9223372036854775808,\n"
    "    0\n"
    "  ],\n"
    "  \"value_sfixed64\": [\n"
    "    -9223372036854775808,\n"
    "    0\n"
    "  ],\n"

    /* JSON does not support max(unsigned long long) */
    "  \"value_uint64\": [\n"
    "    9223372036854775807,\n"
    "    0\n"
    "  ],\n"
    "  \"value_fixed64\": [\n"
    "    9223372036854775807,\n"
    "    0\n"
    "  ],\n"

    "  \"value_float\": [\n"
    "    0.33000001311302185,\n"
    "    0.0\n" /* Note: 0 -> 0.0 */
    "  ],\n"
    "  \"value_double\": [\n"
    "    0.0077705550333011103,\n"
    "    0.0\n" /* Note: 0 -> 0.0 */
    "  ],\n"

    "  \"value_bool\": [\n"
    "    true,\n"
    "    false,\n"
    "    false\n"
    "  ]\n"

    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__boolean_values__values) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"value_true\": true,\n"
    "  \"value_false\": false\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__boolean_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__BooleanValues *boolean_values = (Foo__BooleanValues *)protobuf_message;

  ASSERT(boolean_values->value_true);
  ASSERT(!boolean_values->value_false);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"value_true\": true,\n"
    "  \"value_false\": false\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__string_values__values) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"optional_string\": \"qwerty \\u0000 12345\",\n" /* Note: \0-byte terminated string */
    "  \"repeated_string\": [\n"
    "    \"qwerty \\u0000\",\n"                          /* Note: \0-byte terminated string */
    "    \"\\u0000 12345\"\n"                            /* Note: \0-byte terminated string */
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__string_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__StringValues *string_values = (Foo__StringValues *)protobuf_message;

  ASSERT(string_values->optional_string);
  ASSERT(strlen(string_values->optional_string) == 7);
  ASSERT_STRNCMP((const char *)string_values->optional_string, "qwerty ", strlen(string_values->optional_string));

  ASSERT(string_values->n_repeated_string == 2);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"optional_string\": \"qwerty \",\n" /* Note: \0-byte terminated string */
    "  \"repeated_string\": [\n"
    "    \"qwerty \",\n"                    /* Note: \0-byte terminated string */
    "    \"\"\n"                            /* Note: \0-byte terminated string */
    "  ]\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__bytes_values__values) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"optional_bytes\": \"qwerty \\u0000 12345\",\n"
    "  \"repeated_bytes\": [\n"
    "    \"qwerty \\u0000\",\n"
    "    \"\\u0000 12345\"\n"
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__bytes_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__BytesValues *bytes_values = (Foo__BytesValues *)protobuf_message;

  ASSERT(bytes_values->has_optional_bytes);
  ASSERT(bytes_values->optional_bytes.len == 14);
  ASSERT_STRNCMP((const char *)bytes_values->optional_bytes.data, "qwerty \0 12345", bytes_values->optional_bytes.len);

  ASSERT(bytes_values->n_repeated_bytes == 2);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"optional_bytes\": \"qwerty \\u0000 12345\",\n"
    "  \"repeated_bytes\": [\n"
    "    \"qwerty \\u0000\",\n"
    "    \"\\u0000 12345\"\n"
    "  ]\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_is_not_object) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = "[]";

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_OBJECT);

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
    "  \"unknown_field\": \"unknown_field_value\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_FIELD);

  const char *expected_error_string = \
    "Unknown field 'unknown_field' for message 'Foo.Person'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_unknown_enum_value) {
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

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_UNKNOWN_ENUM_VALUE);

  const char *expected_error_string = \
    "Unknown value 'UNKNOWN' for enum 'Foo.Person.PhoneType'"
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
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": {}\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_ARRAY);

  const char *expected_error_string = \
    "JSON is not an array required for repeatable GPB field"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__person__error_required_is_missing) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_REQUIRED_IS_MISSING);

  const char *expected_error_string = \
    "Required field 'id' is missing in message 'Foo.Person'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__bar__error_string_is_not_string_required_for_string) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"string_required\": 42\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_STRING);

  const char *expected_error_string = \
    "JSON value is not a string required for GPB string"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__bar__error_is_not_string_required_for_enum) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"string_required\": \"required\",\n"
    "  \"enum_optional\": 42"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__bar__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_STRING);

  const char *expected_error_string = \
    "JSON value is not a string required for GPB enum"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

#define TEST_IMPL_IS_NOT_INTEGER(type) \
  TEST_IMPL(json2protobuf_string__repeated_values__error_is_not_integer_required_for_ ## type) { \
    int result;               \
    char error_string[256] = {0};                \
                     \
    const char *initial_json_string =            \
      "{\n"                   \
      "  \"value_" #type "\": [\"string\"]\n"             \
      "}"            \
    ;                \
                     \
    ProtobufCMessage *protobuf_message = NULL;            \
                     \
    result = json2protobuf_string(               \
      (char *)initial_json_string,               \
      0,             \
      &foo__repeated_values__descriptor,          \
      &protobuf_message,      \
      error_string,           \
      sizeof(error_string)    \
    );               \
                     \
    ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER);                 \
    const char *expected_error_string = "JSON value is not an integer required for GPB " #type;          \
    ASSERT_STRCMP(error_string, expected_error_string);                      \
    protobuf_c_message_free_unpacked(protobuf_message, NULL);                \
    RETURN_OK();              \
  }

TEST_IMPL_IS_NOT_INTEGER(int32)
TEST_IMPL_IS_NOT_INTEGER(sint32)
TEST_IMPL_IS_NOT_INTEGER(sfixed32)
TEST_IMPL_IS_NOT_INTEGER(uint32)
TEST_IMPL_IS_NOT_INTEGER(fixed32)
TEST_IMPL_IS_NOT_INTEGER(int64)
TEST_IMPL_IS_NOT_INTEGER(sint64)
TEST_IMPL_IS_NOT_INTEGER(sfixed64)
TEST_IMPL_IS_NOT_INTEGER(uint64)
TEST_IMPL_IS_NOT_INTEGER(fixed64)

TEST_IMPL(json2protobuf_string__repeated_values__error_is_not_real_number_required_for_float) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_float\": [\"string\"]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER_OR_REAL);

  const char *expected_error_string = \
    "JSON value is not a integer/real required for GPB float"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__repeated_values__error_is_not_real_number_required_for_double) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_double\": [\"string\"]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER_OR_REAL);

  const char *expected_error_string = \
    "JSON value is not a integer/real required for GPB double"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__boolean_values__error_is_not_boolean_required_for_bool) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_true\": \"string\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__boolean_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_BOOLEAN);

  const char *expected_error_string = \
    "JSON value is not a boolean required for GPB bool"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);

  RETURN_OK();
}
