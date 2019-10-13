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

#include <libgen.h>

extern char executable_path[MAXPATHLEN];

TEST_IMPL(json2protobuf_file__success) {
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

  RETURN_OK();
}

TEST_IMPL(json2protobuf_file__error_cannot_parse_bad_message) {
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

TEST_IMPL(json2protobuf_file__error_cannot_parse_bad_json) {
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

TEST_IMPL(json2protobuf_file__error_cannot_parse_unexistent_file) {
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
