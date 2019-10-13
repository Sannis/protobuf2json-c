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
#include <unistd.h>
#include <errno.h>

extern char executable_path[MAXPATHLEN];

TEST_IMPL(protobuf2json_file__success) {
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
    "  \"id\": 42,\n"
    "  \"phone\": []\n"
    "}"
  );

  free(json_string);
  fclose(fd);

  result = unlink(file_name);
  ASSERT_ZERO(result);

  RETURN_OK();
}

void* failed_malloc(size_t size) {
  return NULL;
}

TEST_IMPL(protobuf2json_file__error_alloc) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  json_set_alloc_funcs(failed_malloc, free);
  result = protobuf2json_file(&person.base, 0, "not null", "r", error_string, sizeof(error_string));
  json_set_alloc_funcs(malloc, free);
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

TEST_IMPL(protobuf2json_file__error_cannot_open_null_file) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  result = protobuf2json_file(&person.base, 0, NULL, "r", error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE);

  const char *expected_error_string = \
    "Cannot open NULL to dump JSON"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_file__error_cannot_open_null_fopen_mode) {
  int result;
  char error_string[256] = {0};

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  result = protobuf2json_file(&person.base, 0, "not null", NULL, error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE);

  const char *expected_error_string = \
    "Cannot open file with NULL fopen(3) mode to dump JSON"
  ;

  ASSERT_STRCMP(
    error_string,
    expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_file__error_cannot_open_unexistent_file) {
  int result;
  char error_string[256] = {0};

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};
  long json_string_length = 0;

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/unexistent.json", file_path);
  ASSERT(result > 0);

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  result = protobuf2json_file(&person.base, 0, file_name, "r", error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE);

  char expected_error_string[MAXPATHLEN + 100];
  sprintf(expected_error_string, "Cannot open file '%s' with mode 'r' to dump JSON, errno=%d", file_name, ENOENT);

  ASSERT_STRCMP(
    error_string,
    (const char *)expected_error_string
  );

  RETURN_OK();
}

TEST_IMPL(protobuf2json_file__error_cannot_dump_file) {
  int result;
  char error_string[256] = {0};

  char file_path[MAXPATHLEN] = {0};
  char file_name[MAXPATHLEN] = {0};
  long json_string_length = 0;

  result = realpath(dirname(executable_path), file_path) ? 1 : 0;
  ASSERT(result > 0);

  result = snprintf(file_name, sizeof(file_name) - 1, "%s/fixtures/good.json", file_path);
  ASSERT(result > 0);

  Foo__Person person = FOO__PERSON__INIT;

  person.name = "John Doe";
  person.id = 42;

  result = protobuf2json_file(&person.base, 0, file_name, "r", error_string, sizeof(error_string));
  ASSERT_EQUALS(result, PROTOBUF2JSON_ERR_CANNOT_DUMP_FILE);

  char expected_error_string[MAXPATHLEN + 100];
  sprintf(expected_error_string, "Cannot write JSON to file '%s' with mode 'r', errno=%d", file_name, EBADF);

  ASSERT_STRCMP(
    error_string,
    (const char *)expected_error_string
  );

  RETURN_OK();
}
