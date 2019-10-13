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

TEST_IMPL(reversible__messages) {
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

TEST_IMPL(reversible__default_values) {
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
    "  \"bytes_optional_default\": \"ZGVmYXVsdCB2YWx1ZSAz\",\n"
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

TEST_IMPL(reversible__numbers) {
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

    /* 64 bit values are always written as strings. */
    "  \"value_int64\": [\n"
    "    \"9223372036854775807\",\n"
    "    \"0\"\n"
    "  ],\n"
    "  \"value_sint64\": [\n"
    "    \"-9223372036854775808\",\n"
    "    \"0\"\n"
    "  ],\n"
    "  \"value_sfixed64\": [\n"
    "    \"-9223372036854775808\",\n"
    "    \"0\"\n"
    "  ],\n"

    /* JSON does not support max(unsigned long long) */
    "  \"value_uint64\": [\n"
    "    \"9223372036854775807\",\n"
    "    \"0\"\n"
    "  ],\n"
    "  \"value_fixed64\": [\n"
    "    \"9223372036854775807\",\n"
    "    \"0\"\n"
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

TEST_IMPL(reversible__strings) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"value_string\": [\n"
    "    \"qwerty \\u0000 12345\",\n"                    /* Note: \0-byte terminated string */
    "    \"qwerty \\u0000\",\n"                          /* Note: \0-byte terminated string */
    "    \"\\u0000 12345\"\n"                            /* Note: \0-byte terminated string */
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__repeated_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__RepeatedValues *repeated_values = (Foo__RepeatedValues *)protobuf_message;

  ASSERT(repeated_values->n_value_string == 3);
  ASSERT(strlen(repeated_values->value_string[0]) == 7);
  ASSERT_STRNCMP((const char *)repeated_values->value_string[0], "qwerty ", strlen(repeated_values->value_string[0]));

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"value_string\": [\n"
    "    \"qwerty \",\n"                    /* Note: \0-byte terminated string */
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

TEST_IMPL(reversible__bytes) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"value_bytes\": [\n"
    "    \"AA==\",\n"                   /* "\0" */
    "    \"cXdlcnR5IAAgMTIzNA==\",\n"   /* "qwerty \0 12345" */
    "    \"cXdlcnR5IA==\",\n"           /* "qwerty \0" */
    "    \"ACAxMjM0NQ==\"\n"            /* "\0 12345" */
    "  ]\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__repeated_values__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__RepeatedValues *repeated_values = (Foo__RepeatedValues *)protobuf_message;

  ASSERT(repeated_values->n_value_bytes == 4);
  /* 0 */
  ASSERT(repeated_values->value_bytes[0].len == 1);
  ASSERT_STRNCMP((const char *)repeated_values->value_bytes[0].data, "\0", repeated_values->value_bytes[0].len);
  /* 1 */
  ASSERT(repeated_values->value_bytes[1].len == 13);
  ASSERT_STRNCMP((const char *)repeated_values->value_bytes[1].data, "qwerty \0 12345", repeated_values->value_bytes[1].len);
  /* 0 */
  ASSERT(repeated_values->value_bytes[2].len == 7);
  ASSERT_STRNCMP((const char *)repeated_values->value_bytes[2].data, "qwerty \0", repeated_values->value_bytes[2].len);
  /* 0 */
  ASSERT(repeated_values->value_bytes[3].len == 7);
  ASSERT_STRNCMP((const char *)repeated_values->value_bytes[3].data, "\0 12345", repeated_values->value_bytes[3].len);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"value_bytes\": [\n"
    "    \"AA==\",\n"                   /* "\0" */
    "    \"cXdlcnR5IAAgMTIzNA==\",\n"   /* "qwerty \0 12345" */
    "    \"cXdlcnR5IA==\",\n"           /* "qwerty \0" */
    "    \"ACAxMjM0NQ==\"\n"            /* "\0 12345" */
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

TEST_IMPL(reversible__oneof_none) {
  int result;

  const char *initial_json_string = \
    "{}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__something__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__Something *something = (Foo__Something *)protobuf_message;

  ASSERT_EQUALS(something->something_case, FOO__SOMETHING__SOMETHING__NOT_SET);
  ASSERT(something->oneof_string == NULL);

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = initial_json_string;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(reversible__oneof_one) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"oneof_string\": \"hello\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__something__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__Something *something = (Foo__Something *)protobuf_message;

  ASSERT_EQUALS(something->something_case, FOO__SOMETHING__SOMETHING_ONEOF_STRING);
  ASSERT_STRCMP(something->oneof_string, "hello");

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = initial_json_string;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(reversible__oneof_other) {
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"oneof_bytes\": \"d29ybGQ=\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__something__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__Something *something = (Foo__Something *)protobuf_message;

  ASSERT_EQUALS(something->something_case, FOO__SOMETHING__SOMETHING_ONEOF_BYTES);
  ASSERT_EQUALS((int)something->oneof_bytes.len, 5);
  ASSERT_STRCMP((const char*)something->oneof_bytes.data, "world");

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = initial_json_string;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
}

TEST_IMPL(reversible__oneof_both_first) {
#if JANSSON_VERSION_HEX < 0x020800
  RETURN_SKIP("behavior for multiple oneof values can only be guaranteed for Jansson >= 2.8");
#else
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"oneof_string\": \"hello\",\n"
    "  \"oneof_bytes\": \"d29ybGQ=\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__something__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__Something *something = (Foo__Something *)protobuf_message;

  // last input wins, irrespective of order in proto definition
  ASSERT_EQUALS(something->something_case, FOO__SOMETHING__SOMETHING_ONEOF_BYTES);
  ASSERT_EQUALS((int)something->oneof_bytes.len, 5);
  ASSERT_STRCMP((const char*)something->oneof_bytes.data, "world");

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"oneof_bytes\": \"d29ybGQ=\"\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
#endif
}

TEST_IMPL(reversible__oneof_both_second) {
#if JANSSON_VERSION_HEX < 0x020800
  RETURN_SKIP("behavior for multiple oneof values can only be guaranteed for Jansson >= 2.8");
#else
  int result;

  const char *initial_json_string = \
    "{\n"
    "  \"oneof_bytes\": \"d29ybGQ=\",\n"
    "  \"oneof_string\": \"hello\"\n"
    "}"
  ;

  ProtobufCMessage *protobuf_message;

  result = json2protobuf_string((char *)initial_json_string, JSON_ALLOW_NUL, &foo__something__descriptor, &protobuf_message, NULL, 0);
  ASSERT_ZERO(result);

  Foo__Something *something = (Foo__Something *)protobuf_message;

  // last input wins, irrespective of order in proto definition
  ASSERT_EQUALS(something->something_case, FOO__SOMETHING__SOMETHING_ONEOF_STRING);
  ASSERT_STRCMP(something->oneof_string, "hello");

  char *json_string;
  result = protobuf2json_string(protobuf_message, TEST_JSON_FLAGS, &json_string, NULL, 0);
  ASSERT_ZERO(result);
  ASSERT(json_string);

  const char *expected_json_string = \
    "{\n"
    "  \"oneof_string\": \"hello\"\n"
    "}"
  ;

  ASSERT_STRCMP(
    json_string,
    expected_json_string
  );

  protobuf_c_message_free_unpacked(protobuf_message, NULL);
  free(json_string);

  RETURN_OK();
#endif
}
