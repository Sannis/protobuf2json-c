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

#include <math.h>
#include <inttypes.h>

TEST_IMPL(json2protobuf_string__error_cannot_parse_wrong_string) {
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

TEST_IMPL(json2protobuf_string__error_duplicate_field) {
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

TEST_IMPL(json2protobuf_string__error_is_not_array) {
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_unknown_field) {
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_unknown_enum_value) {
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

TEST_IMPL(json2protobuf_string__error_required_is_missing) {
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_is_not_object_required_for_message) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"name\": \"John Doe\",\n"
    "  \"id\": 42,\n"
    "  \"phone\": [[]]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__person__descriptor, &protobuf_message, error_string, sizeof(error_string));
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

#define TEST_IMPL_IS_NOT_INTEGER_OR_STRING(type)                                                 \
  TEST_IMPL(json2protobuf_string__error_is_not_integer_or_string_required_for_ ## type) {        \
    int result;                                                                                  \
    char error_string[256] = {0};                                                                \
                                                                                                 \
    const char *initial_json_string =                                                            \
      "{\n"                                                                                      \
      "  \"value_" #type "\": [true]\n"                                                          \
      "}"                                                                                        \
    ;                                                                                            \
                                                                                                 \
    ProtobufCMessage *protobuf_message = NULL;                                                   \
                                                                                                 \
    result = json2protobuf_string(                                                               \
      (char *)initial_json_string,                                                               \
      0,                                                                                         \
      &foo__repeated_values__descriptor,                                                         \
      &protobuf_message,                                                                         \
      error_string,                                                                              \
      sizeof(error_string)                                                                       \
    );                                                                                           \
                                                                                                 \
    ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER);                                     \
                                                                                                 \
    const char *expected_error_string =                                                          \
      "JSON value is not an integer or string required for GPB " #type " value_" #type;          \
    ASSERT_STRCMP(error_string, expected_error_string);                                          \
                                                                                                 \
    RETURN_OK();                                                                                 \
  }

TEST_IMPL_IS_NOT_INTEGER_OR_STRING(int32)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(sint32)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(sfixed32)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(uint32)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(fixed32)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(int64)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(sint64)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(sfixed64)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(uint64)
TEST_IMPL_IS_NOT_INTEGER_OR_STRING(fixed64)

#define TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(type)                                                \
  TEST_IMPL(json2protobuf_string__error_is_not_convertible_string_for_ ## type) {                \
    int result;                                                                                  \
    char error_string[256] = {0};                                                                \
                                                                                                 \
    const char *initial_json_string =                                                            \
      "{\n"                                                                                      \
      "  \"value_" #type "\": [\"string\"]\n"                                                    \
      "}"                                                                                        \
    ;                                                                                            \
                                                                                                 \
    ProtobufCMessage *protobuf_message = NULL;                                                   \
                                                                                                 \
    result = json2protobuf_string(                                                               \
      (char *)initial_json_string,                                                               \
      0,                                                                                         \
      &foo__repeated_values__descriptor,                                                         \
      &protobuf_message,                                                                         \
      error_string,                                                                              \
      sizeof(error_string)                                                                       \
    );                                                                                           \
                                                                                                 \
    ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER);                                     \
                                                                                                 \
    const char *expected_error_string =                                                          \
      "JSON string not successfully converted to value for GPB " #type " value_" #type;          \
    ASSERT_STRCMP(error_string, expected_error_string);                                          \
                                                                                                 \
    RETURN_OK();                                                                                 \
  }

TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(int32)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(sint32)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(sfixed32)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(uint32)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(fixed32)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(int64)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(sint64)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(sfixed64)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(uint64)
TEST_IMPL_IS_NOT_CONVERTIBLE_STRING(fixed64)

TEST_IMPL(json2protobuf_string__string_to_int_convert_success) {
  int result;
  char initial_json_string[1024];

  sprintf (initial_json_string,
    "{\n"

    "  \"value_int32\": [\"%" PRId32 "\", \"0\", \"%" PRId32 "\"],\n"
    "  \"value_sint32\": [\"%" PRId32 "\", \"0\", \"%" PRId32 "\"],\n"
    "  \"value_sfixed32\": [\"%" PRId32 "\", \"0\", \"%" PRId32 "\"],\n"

    "  \"value_uint32\": [\"0\", \"%" PRIu32 "\"],\n"
    "  \"value_fixed32\": [\"0\", \"%" PRIu32 "\"],\n"

    "  \"value_int64\": [\"%" PRId64 "\", \"0\", \"%" PRId64 "\"],\n"
    "  \"value_sint64\": [\"%" PRId64 "\", \"0\", \"%" PRId64 "\"],\n"
    "  \"value_sfixed64\": [\"%" PRId64 "\", \"0\", \"%" PRId64 "\"],\n"

    "  \"value_uint64\": [\"0\", \"%" PRIu64 "\"],\n"
    "  \"value_fixed64\": [\"0\", \"%" PRIu64 "\"]\n"
    "}",
    INT32_MIN, INT32_MAX, INT32_MIN, INT32_MAX, INT32_MIN, INT32_MAX,
    UINT32_MAX, UINT32_MAX,
    INT64_MIN, INT64_MAX, INT64_MIN, INT64_MAX, INT64_MIN, INT64_MAX,
    UINT64_MAX, UINT64_MAX
  );

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__RepeatedValues *repeated_values = (Foo__RepeatedValues *)protobuf_message;

  ASSERT(repeated_values->n_value_int32 == 3);
  ASSERT(repeated_values->value_int32[0] == INT32_MIN);
  ASSERT(repeated_values->value_int32[1] == 0);
  ASSERT(repeated_values->value_int32[2] == INT32_MAX);

  ASSERT(repeated_values->n_value_sint32 == 3);
  ASSERT(repeated_values->value_sint32[0] == INT32_MIN);
  ASSERT(repeated_values->value_sint32[1] == 0);
  ASSERT(repeated_values->value_sint32[2] == INT32_MAX);

  ASSERT(repeated_values->n_value_sfixed32 == 3);
  ASSERT(repeated_values->value_sfixed32[0] == INT32_MIN);
  ASSERT(repeated_values->value_sfixed32[1] == 0);
  ASSERT(repeated_values->value_sfixed32[2] == INT32_MAX);

  ASSERT(repeated_values->n_value_uint32 == 2);
  ASSERT(repeated_values->value_uint32[0] == 0);
  ASSERT(repeated_values->value_uint32[1] == UINT32_MAX);

  ASSERT(repeated_values->n_value_fixed32 == 2);
  ASSERT(repeated_values->value_fixed32[0] == 0);
  ASSERT(repeated_values->value_fixed32[1] == UINT32_MAX);

  ASSERT(repeated_values->n_value_int64 == 3);
  ASSERT(repeated_values->value_int64[0] == INT64_MIN);
  ASSERT(repeated_values->value_int64[1] == 0);
  ASSERT(repeated_values->value_int64[2] == INT64_MAX);

  ASSERT(repeated_values->n_value_sint64 == 3);
  ASSERT(repeated_values->value_sint64[0] == INT64_MIN);
  ASSERT(repeated_values->value_sint64[1] == 0);
  ASSERT(repeated_values->value_sint64[2] == INT64_MAX);

  ASSERT(repeated_values->n_value_sfixed64 == 3);
  ASSERT(repeated_values->value_sfixed64[0] == INT64_MIN);
  ASSERT(repeated_values->value_sfixed64[1] == 0);
  ASSERT(repeated_values->value_sfixed64[2] == INT64_MAX);

  ASSERT(repeated_values->n_value_uint64 == 2);
  ASSERT(repeated_values->value_uint64[0] == 0);
  ASSERT(repeated_values->value_uint64[1] == UINT64_MAX);

  ASSERT(repeated_values->n_value_fixed64 == 2);
  ASSERT(repeated_values->value_fixed64[0] == 0);
  ASSERT(repeated_values->value_fixed64[1] == UINT64_MAX);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"value_int32\": [\n"
    "    -2147483648,\n"
    "    0,\n"
    "    2147483647\n"
    "  ],\n"
    "  \"value_sint32\": [\n"
    "    -2147483648,\n"
    "    0,\n"
    "    2147483647\n"
    "  ],\n"
    "  \"value_sfixed32\": [\n"
    "    -2147483648,\n"
    "    0,\n"
    "    2147483647\n"
    "  ],\n"
    "  \"value_uint32\": [\n"
    "    0,\n"
    "    4294967295\n"
    "  ],\n"
    "  \"value_fixed32\": [\n"
    "    0,\n"
    "    4294967295\n"
    "  ],\n"
    "  \"value_int64\": [\n"
    "    \"-9223372036854775808\",\n"
    "    \"0\",\n"
    "    \"9223372036854775807\"\n"
    "  ],\n"
    "  \"value_sint64\": [\n"
    "    \"-9223372036854775808\",\n"
    "    \"0\",\n"
    "    \"9223372036854775807\"\n"
    "  ],\n"
    "  \"value_sfixed64\": [\n"
    "    \"-9223372036854775808\",\n"
    "    \"0\",\n"
    "    \"9223372036854775807\"\n"
    "  ],\n"
    "  \"value_uint64\": [\n"
    "    \"0\",\n"
    "    \"18446744073709551615\"\n"
    "  ],\n"
    "  \"value_fixed64\": [\n"
    "    \"0\",\n"
    "    \"18446744073709551615\"\n"
    "  ],\n"
    "  \"value_float\": [],\n"
    "  \"value_double\": [],\n"
    "  \"value_bool\": [],\n"
    "  \"value_enum\": [],\n"
    "  \"value_string\": [],\n"
    "  \"value_bytes\": [],\n"
    "  \"value_message\": []\n"
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

/* Values bigger that +/- int64 will result in a jansson parsing failure rather than a range error */
#define TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(type,limit,value)                       \
  TEST_IMPL(json2protobuf_string__error_out_of_range_number_for_ ## type ## _ ## limit) {        \
    int result;                                                                                  \
    char error_string[256] = {0};                                                                \
    char initial_json_string[256];                                                               \
                                                                                                 \
    sprintf (initial_json_string,                                                                \
      "{\n"                                                                                      \
      "  \"value_" #type "\": [%lld]\n"                                                          \
      "}", value                                                                                 \
    );                                                                                           \
                                                                                                 \
    ProtobufCMessage *protobuf_message = NULL;                                                   \
                                                                                                 \
    result = json2protobuf_string(                                                               \
      (char *)initial_json_string,                                                               \
      0,                                                                                         \
      &foo__repeated_values__descriptor,                                                         \
      &protobuf_message,                                                                         \
      error_string,                                                                              \
      sizeof(error_string)                                                                       \
    );                                                                                           \
                                                                                                 \
    ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER);                                     \
                                                                                                 \
    const char *expected_error_string =                                                          \
      "JSON value is not within range for GPB " #type " value_" #type                            \
    ;                                                                                            \
    ASSERT_STRCMP(error_string, expected_error_string);                                          \
                                                                                                 \
    RETURN_OK();                                                                                 \
  }

TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(int32,min,-2147483649LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(int32,max,2147483648LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(sint32,min,-2147483649LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(sint32,max,2147483648LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(sfixed32,min,-2147483649LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(sfixed32,max,2147483648LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(uint32,min,-1LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(uint32,max,4294967296LL)
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_NUMBER(uint64,min,-1LL)


/* Values bigger than +/- int64 will result in a conversion failure rather than a range error */
#define TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(type,limit,value)                       \
  TEST_IMPL(json2protobuf_string__error_out_of_range_string_for_ ## type ## _ ## limit) {        \
    int result;                                                                                  \
    char error_string[256] = {0};                                                                \
                                                                                                 \
    const char *initial_json_string =                                                            \
      "{\n"                                                                                      \
      "  \"value_" #type "\": [\"" value "\"]\n"                                                 \
      "}"                                                                                        \
    ;                                                                                            \
                                                                                                 \
    ProtobufCMessage *protobuf_message = NULL;                                                   \
                                                                                                 \
    result = json2protobuf_string(                                                               \
      (char *)initial_json_string,                                                               \
      0,                                                                                         \
      &foo__repeated_values__descriptor,                                                         \
      &protobuf_message,                                                                         \
      error_string,                                                                              \
      sizeof(error_string)                                                                       \
    );                                                                                           \
                                                                                                 \
    ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER);                                     \
                                                                                                 \
    const char *expected_error_string =                                                          \
      "JSON value is not within range for GPB " #type " value_" #type                            \
    ;                                                                                            \
    ASSERT_STRCMP(error_string, expected_error_string);                                          \
                                                                                                 \
    RETURN_OK();                                                                                 \
  }

TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(int32,min,"-2147483649")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(int32,max,"2147483648")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(sint32,min,"-2147483649")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(sint32,max,"2147483648")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(sfixed32,min,"-2147483649")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(sfixed32,max,"2147483648")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(uint32,min,"-1")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(uint32,max,"4294967296")
TEST_IMPL_OUT_OF_RANGE_CHECK_WITHIN_INT64_STRING(uint64,min,"-1")

#define TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(type,limit,value)                                 \
  TEST_IMPL(json2protobuf_string__error_is_too_big_to_convert_ ## type ## _ ## limit) {          \
    int result;                                                                                  \
    char error_string[256] = {0};                                                                \
                                                                                                 \
    const char *initial_json_string =                                                            \
      "{\n"                                                                                      \
      "  \"value_" #type "\": [\"" value "\"]\n"                                                 \
      "}"                                                                                        \
    ;                                                                                            \
                                                                                                 \
    ProtobufCMessage *protobuf_message = NULL;                                                   \
                                                                                                 \
    result = json2protobuf_string(                                                               \
      (char *)initial_json_string,                                                               \
      0,                                                                                         \
      &foo__repeated_values__descriptor,                                                         \
      &protobuf_message,                                                                         \
      error_string,                                                                              \
      sizeof(error_string)                                                                       \
    );                                                                                           \
                                                                                                 \
    ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_INTEGER);                                     \
                                                                                                 \
    const char *expected_error_string =                                                          \
      "JSON string not successfully converted to value for GPB " #type " value_" #type           \
    ;                                                                                            \
    ASSERT_STRCMP(error_string, expected_error_string);                                          \
                                                                                                 \
    RETURN_OK();                                                                                 \
  }

TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(int64,min,"-9223372036854775809")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(int64,max,"9223372036854775809")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(sint64,min,"-9223372036854775809")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(sint64,max,"9223372036854775809")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(sfixed64,min,"-9223372036854775809")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(sfixed64,max,"9223372036854775809")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(uint64,max,"18446744073709551616")
TEST_IMPL_IS_TOO_BIG_TO_CONVERT_STRING(fixed64,max,"18446744073709551616")

/* Confirm that number values larger than INT64_MAX generate a parsing failure */
TEST_IMPL(json2protobuf_string__error_is_too_big_to_convert_positive) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_uint64\": [9223372036854775808]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

  const char *expected_error_string =
    "JSON parsing error at line 2 column 38 (position 40): too big integer near '9223372036854775808'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

/* Confirm that number values below -INT64_MAX generate a parsing failure */
TEST_IMPL(json2protobuf_string__error_is_too_big_to_convert_negative) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_int64\": [-9223372036854775809]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_PARSE_STRING);

  const char *expected_error_string =
    "JSON parsing error at line 2 column 38 (position 40): too big negative integer near '-9223372036854775809'"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_is_not_real_number_required_for_float) {
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_is_not_real_number_required_for_double) {
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_is_not_boolean_required_for_bool) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_bool\": [\"string\"]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_BOOLEAN);

  const char *expected_error_string = \
    "JSON value is not a boolean required for GPB bool"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_is_not_string_required_for_enum) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_enum\": [42]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
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

TEST_IMPL(json2protobuf_string__error_is_not_string_required_for_string) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_string\": [42]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_STRING);

  const char *expected_error_string = \
    "JSON value is not a string required for GPB string"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(json2protobuf_string__error_is_not_string_required_for_bytes) {
  int result;
  char error_string[256] = {0};

  const char *initial_json_string = \
    "{\n"
    "  \"value_bytes\": [42]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message = NULL;

  result = json2protobuf_string((char *)initial_json_string, 0, &foo__repeated_values__descriptor, &protobuf_message, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_IS_NOT_STRING);

  const char *expected_error_string = \
    "JSON value is not a string required for GPB bytes"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}
